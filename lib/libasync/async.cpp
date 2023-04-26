#include "async.h"

#include <stdio.h>

#include <cstring>
#include <memory>
#include <unordered_map>

#include "command_handler.h"
#include "log_users.h"

namespace async {

std::unordered_map<void*, std::unique_ptr<CommandHandler>> _command_handler;

handle_t connect(std::size_t bulk) {
  std::unique_ptr<IObserver> ostream_logger = std::make_unique<OstreamLogger>();
  std::shared_ptr<IObserver> file_logger = FileLogger::GetInst();
  std::unique_ptr ch = std::make_unique<CommandHandler>(bulk);
  ch->AddObserver(file_logger).AddObserver(std::move(ostream_logger));
  CommandHandler* ptr = ch.get();
  _command_handler.emplace(ptr, std::move(ch));

  return ptr;
}

void receive(handle_t handle, const char* data, std::size_t size) {
  auto it = _command_handler.find(handle);
  if (it == _command_handler.end()) {
    return;
  }
  auto& [_, c_h] = *it;
  // что бы разбить строку на отдельные команды по \n
  std::stringstream ss(std::string(data, size));
  std::string command;
  while (std::getline(ss, command)) {
    c_h->add(std::move(command));
  }
}

void disconnect(handle_t handle) {
  _command_handler.erase(handle);
}

}  // namespace async
