#pragma once

#include <list>
#include <memory>
#include <vector>

struct CommandHolder {
  std::string _res_command;
  std::string _time_start;
};

class IObserver {
 public:
  virtual void UpdateEnd(const CommandHolder& comand_holder) = 0;
  virtual void UpdateStart(){};
  virtual ~IObserver() = default;
};

class IObservered {
 public:
  virtual IObservered& AddObserver(std::shared_ptr<IObserver> observer) = 0;
  virtual ~IObservered() = default;
};

class ObserveredCmd : public IObservered {
 public:
  IObservered& AddObserver(std::shared_ptr<IObserver> observer) override {
    _observers.push_back(std::move(observer));
    return *this;
  };

  void NotifyBlockComplited(const CommandHolder& comand_holder) {
    for (auto it = _observers.begin(); it != _observers.end(); ++it) {
      (*it)->UpdateEnd(comand_holder);
    }
  }

 private:
  std::vector<std::shared_ptr<IObserver>> _observers;
};
