//
// Created by Alexander Bychuk on 20.10.2019.
//

#include <iostream>
#include "ThreadPool.h"
ThreadPool::ThreadPool(size_t count) : _count(count), _thread(_count), _eventMutex(_count), _event(_count) {
  for (size_t i = 0; i < _count; ++i) {
    _thread[i] = std::thread([this]() {
      size_t num = _runnedNum++;
      while (_isRunning) {
        std::unique_ptr<Runnable> task = getTask();
        if (task != nullptr) {
          try {
            task->run();
          } catch (std::exception &ex) {
            std::cerr << "invalid runnable : " << ex.what() << std::endl;
          }
        }
        if (!isEmpty()) {
          continue;
        }
        {
          std::unique_lock<std::mutex> lock(_eventMutex[num]);
          _event[num].wait_for(lock, std::chrono::milliseconds(100));
        }
      }
    });
  }
}
ThreadPool::~ThreadPool() {
  _isRunning = false;
  try {
    std::for_each(_thread.begin(), _thread.end(), [](std::thread &thr) { thr.join(); });
  } catch (...) {
  }
}
std::unique_ptr<Runnable> ThreadPool::getTask() {
  std::lock_guard<std::mutex> lock(_queueLock);
  std::unique_ptr<Runnable> task;
  if (!_runnable.empty()) {
    task = std::move(_runnable.front());
    _runnable.pop_front();
  }
  return task;
}
void ThreadPool::addTask(std::unique_ptr<Runnable> task) {
  {
    std::lock_guard<std::mutex> lock(_queueLock);
    _runnable.push_back(std::move(task));
  }
  std::for_each(_event.rbegin(), _event.rend(), [](std::condition_variable &cv) { cv.notify_one(); });
}
bool ThreadPool::isEmpty() const {
  std::lock_guard<std::mutex> lock(_queueLock);
  return _runnable.empty();
}
