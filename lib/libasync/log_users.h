#pragma once

#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>

#include "observer.h"

class FileLogger : public IObserver {
 public:
  static std::shared_ptr<FileLogger> GetInst() {
    static std::shared_ptr<FileLogger> _inst(new FileLogger);
    return _inst;
  }

  ~FileLogger() {
    std::cout << "~FileLogger_start" << std::endl;
    _is_end = true;
    _cv.notify_one();
    for (auto& thread : _pool_threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
    std::cout << "~FileLogger_end" << std::endl;
  }

  void UpdateEnd(const CommandHolder& comand_holder) override {
    {
      std::scoped_lock l(_m_comands);
      _comand_queue.push(comand_holder);
    }
    //_cv.notify_one();
  }

 private:
  FileLogger() {
    _pool_threads.emplace_back(&FileLogger::WriteAndWaite, this);
    _pool_threads.emplace_back(&FileLogger::WriteAndWaite, this);
  }

  void WriteAndWaite() {
    while (!_is_end || !_comand_queue.empty()) {
      CommandHolder command;
      std::filesystem::path cur_path;
      {
        std::unique_lock ul(_m);
        //_cv.wait(ul, [this]{return /*!_comand_queue.empty() || */_is_end;});

        // TODO (для простоты показания что из деструктора не нотифаится)
        // хочу накопить команды в очереди,
        // а когда произойдет вызов деструктора
        // ~FileLogger() нотифайнуть и записать в файлы, но почему то при вызове
        // нотифая из деструктора поток не просыпается, и дальнейший джойн
        // приводит к блокировке если нотифаить из UpdateEnd то потоки
        // просыпаются нормально хотя и с задержкой, но в деструкторе все равно
        // нотифаить надо что б выставить флаг конца и выйти из цикла
        // не могу найти, понять почему так вроде запретов нет?
        while (!_is_end) {
          std::cout << "Stop_stand " << std::this_thread::get_id() << std::endl;
          _cv.wait(ul);
          std::cout << "Wake_up " << std::this_thread::get_id() << std::endl;
        }
        //  потокобезопасно меняем _comand_queue
        //  command заранее сохраняем исходя из того что операция быстрая
        //  а cur_file << command << std::endl; долгая
        {
          std::scoped_lock l(_m_comands);
          if (!_comand_queue.empty()) {
            command = std::move(_comand_queue.front());
            _comand_queue.pop();
          }
        }
        if (command._res_command.empty()) {
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

      // следующий поток может
      _cv.notify_one();

      std::ofstream cur_file;
      cur_file.open(cur_path);
      cur_file << command._res_command << std::endl;
      std::cout << "Write " << command._res_command << std::endl;
      cur_file.close();
      // std::this_thread::sleep_for(std::chrono::duration<int, std::deci>(30));
    }
  }

  std::queue<CommandHolder> _comand_queue;
  std::vector<std::thread> _pool_threads;
  std::mutex _m;
  std::mutex _m_comands;
  std::condition_variable _cv;
  std::atomic_bool _is_end = false;
  std::atomic_uint _counter = 1;
};

// так наверное не верно так как поток должен быть создан единожды как я понял
// из задания опять повторять синглтон как FileLogger не хотелось
class OstreamLogger : public IObserver {
 public:
  OstreamLogger()
      : OstreamLogger(std::cout) {
  }

  OstreamLogger(std::ostream& out)
      : _out(out) {
  }

  void UpdateEnd(const CommandHolder& comand_holder) override {
    auto Write = [](CommandHolder comand_holder, std::ostream& out) {
      out << comand_holder._res_command << std::endl;
    };
    std::thread log(Write, comand_holder, std::ref(_out));
    log.detach();
  }

 private:
  std::ostream& _out;
};

// TODO пробовал так: создать один статический поток и этим потоком выводить
// для всех экземпляров класса но не выходит
// если джоин писать в деструкторе то можно создавать только один экземпляр
// этого класса что повторяет синглтон выше если сделать статический jthread то
// обьект с очередью может уничтожиться раньше чем поток ее обработает а нотифай
// из деструктора тут работает так падает потому что  в деструкторе join а
// обьекта два создается при копировании
// class OstreamLogger : public IObserver {
// public:
//  OstreamLogger()
//  {
//    call_once(_flag_init, &OstreamLogger::vInitThread, this);
//  }

//  ~OstreamLogger() {
//    _end = true;
//    fl.test_and_set();
//    fl.notify_one();
//    _log.join();
//  }

//  void UpdateEnd(const CommandHolder& comand_holder) override {
//    {
//      std::scoped_lock l(_m_comands);
//      _comand_queue.push(comand_holder);
//    }
//    //fl.notify_one();
//  }

// private:
//  void vInitThread() {
//    _log = std::jthread(&OstreamLogger::Write, this, std::ref(std::cout));
//  }

//  void Write(std::ostream& out) {
//    CommandHolder command;
//    while(!_end || !_comand_queue.empty()) {
//      fl.wait(!_end /*&& !_comand_queue.empty()*/);
//      // потокобезопасно меняем _comand_queue
//      // command заранее сохраняем исходя из того что операцция быстрая
//      // а cur_file << command << std::endl; долгая
//      {
//        std::scoped_lock l(_m_comands);
//        if(!_comand_queue.empty()) {
//          command = std::move(_comand_queue.front());
//          _comand_queue.pop();
//        }
//      }
//      if(command._res_command.empty()) {
//        continue;
//      }
//      out << command._res_command << std::endl;
//    }
//  }

//  std::atomic_flag fl;
//  bool _end = false;
//  std::mutex _m_comands;
//  inline static std::jthread _log;
//  inline static std::once_flag _flag_init;
//  std::queue<CommandHolder> _comand_queue;
//};
