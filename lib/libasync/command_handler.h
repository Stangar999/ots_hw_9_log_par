#pragma once

#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>

#include "observer.h"

using namespace std::literals;

class CommandHandler : public ObserveredCmd {
  const int _block_size;
  int _current_n;
  CommandHolder _command_holder;
  std::vector<std::string> _commands;
  const std::string start_din;
  const std::string end_din;
  bool _din_blok_is_active;
  size_t _cout_inline_command;

  void EndDinBlock() {
    _din_blok_is_active = false;
    print_block();
  }

 public:
  CommandHandler(int block_size)
      : _block_size(block_size),
        _current_n(block_size),
        _din_blok_is_active(false),
        start_din("{"),
        end_din("}"),
        _cout_inline_command(0) {
  }

  ~CommandHandler() {
    if (!_din_blok_is_active) {
      print_block();
    }
  }

  bool add(std::string cur_command) {
    if (cur_command.empty()) {
      return false;
    }

    if (cur_command == start_din) {
      _din_blok_is_active == true ? void() : print_block();
      _din_blok_is_active = true;
      ++_cout_inline_command;
    } else if (cur_command == end_din) {
      if (_cout_inline_command != 0) {
        --_cout_inline_command;
        _cout_inline_command == 0 ? EndDinBlock() : void();
      }
    } else {
      _din_blok_is_active == true ? 0 : --_current_n;
      // запоминаем время первой команды
      if (_commands.empty()) {
        std::time_t t = std::time(nullptr);
        _command_holder._time_start = std::to_string(t);
      }
      _commands.push_back(cur_command);
    }

    bool end = _din_blok_is_active || _current_n > 0 ? false : true;

    if (end) {
      print_block();
    }
    return false;
  }

  void print_block() {
    _current_n = _block_size;
    if (_commands.empty()) {
      return;
    }
    _command_holder._res_command = "bulk: "s;
    bool start = true;
    for (const auto& var : _commands) {
      if (!start) {
        _command_holder._res_command += ", ";
      }
      _command_holder._res_command += var;
      start = false;
    }
    _commands.clear();
    NotifyBlockComplited(_command_holder);
  }
};
