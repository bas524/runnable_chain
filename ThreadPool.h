//
// Created by Alexander Bychuk on 20.10.2019.
//

#ifndef RUNNABLE_CHAIN_THREADPOOL_H
#define RUNNABLE_CHAIN_THREADPOOL_H

#include <deque>
#include <thread>
#include <vector>

#include "Runnable.h"

class ThreadPool {
  size_t _count{1};
  std::vector<std::thread> _thread;
  std::vector<std::mutex> _eventMutex;
  std::vector<std::condition_variable> _event;
  mutable std::mutex _queueLock;
  std::deque<std::unique_ptr<Runnable>> _runnable;
  std::atomic_size_t _runnedNum{0};
  std::atomic_bool _isRunning{true};
  std::unique_ptr<Runnable> getTask();
  bool isEmpty() const;

public:
  explicit ThreadPool(size_t count);
  virtual ~ThreadPool();
  void addTask(std::unique_ptr<Runnable> task);
};

#endif // RUNNABLE_CHAIN_THREADPOOL_H
