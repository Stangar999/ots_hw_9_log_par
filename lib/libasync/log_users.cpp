#include "log_users.h"

#include <functional>

std::shared_ptr<FileLogger> FileLogger::_file_logger = nullptr;
std::mutex FileLogger::_mx;

std::shared_ptr<FileLogger> FileLogger::GetFileLogger() {
  {
    // TODO пришлось испотльзовать блокировку,
    // static переменную не получается потокобезопасно заного инициализировать
    // после вызова DeInit,
    // а у call_once флаг сбрасывать нельзя
    // и я не придумал как без блокировки,
    std::scoped_lock l(_mx);
    // что бы после вызова DeInit можно было работать дальше по 1 предложению
    // "переходя в состояние, эквивалентное состоянию до первого вызова
    // connect."
    if (!_file_logger) {
      _file_logger.reset(new FileLogger);
    }
  }
  return _file_logger;
}

void FileLogger::Deinit() {
  std::scoped_lock l(_mx);
  if (_file_logger && _file_logger.unique()) {
    _file_logger.reset();
  }
}

FileLogger::~FileLogger() {
  std::cout << "~FileLogger_start" << std::endl;
  _is_end = true;
  _cv.notify_all();
  for (auto &thread : _pool_threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  std::cout << "~FileLogger_end" << std::endl;
}

void FileLogger::UpdateEnd(const CommandHolder &comand_holder) {
  {
    std::scoped_lock l(_m_queue);
    _comand_queue.push(comand_holder);
  }
  _cv.notify_one();
}

void FileLogger::WriteAndWaite() {
  while (!_is_end || !_comand_queue.empty()) {
    CommandHolder command;
    std::filesystem::path cur_path;
    {
      std::unique_lock ul(_m_unique_path);
      _cv.wait(ul, [this] { return !_comand_queue.empty() || _is_end; });

      //  потокобезопасно меняем _comand_queue
      //  command заранее сохраняем исходя из того что операция быстрая
      //  а cur_file << command << std::endl; долгая
      {
        std::scoped_lock l(_m_queue);
        if (!_comand_queue.empty()) {
          command = std::move(_comand_queue.front());
          _comand_queue.pop();
        }
      }
      if (command._res_command.empty()) {
        _cv.notify_all();
        continue;
      }

      // под этим мьютексом формируем уникальное имя файла
      std::ostringstream ss;
      ss << std::this_thread::get_id();

      cur_path = command._time_start + "_thread_" + ss.str() + ".log";
      if (std::filesystem::exists(cur_path)) {
        cur_path = command._time_start + "_" + std::to_string(_counter++) +
                   "_thread_" + ss.str() + ".log";
      } else {
        _counter = 1;
      }
    }

    // следующий поток может начинать
    _cv.notify_one();

    std::ofstream cur_file;
    cur_file.open(cur_path);
    cur_file << command._res_command << std::endl;
    std::cout << "Write in file " << command._res_command << std::endl;
    cur_file.close();
    // std::this_thread::sleep_for(std::chrono::duration<int, std::deci>(30));
  }
}
//------------------------------------------------------------------------------
std::queue<CommandHolder> OstreamLogger::_comand_queue;
std::condition_variable_any OstreamLogger::cv;
std::mutex OstreamLogger::m;
// тут поток создается не лениво, лениво потоки создаются в FileLogger
std::unique_ptr<std::jthread> OstreamLogger::th_log_cons(
    new std::jthread(WriteConsole));

void OstreamLogger::UpdateEnd(const CommandHolder &comand_holder) {
  {
    std::unique_lock l(m);
    _comand_queue.push(comand_holder);
  }
  cv.notify_one();
}

// по 2 предложению, вызов DeInit только из функции finalize()
// поэтому механизм поавторной инициализации не предусмотрен
void OstreamLogger::DeInit() {
  th_log_cons.reset(nullptr);
}

void OstreamLogger::WriteConsole(std::stop_token stoken) {
  while (!stoken.stop_requested() || !_comand_queue.empty()) {
    std::unique_lock l(m);
    cv.wait(l, [&stoken]() {
      return !_comand_queue.empty() || stoken.stop_requested();
    });
    if (!_comand_queue.empty()) {
      std::cout << _comand_queue.front()._res_command << std::endl;
      _comand_queue.pop();
    }
  }
}
