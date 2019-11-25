//
// Created by Alexander Bychuk on 20.10.2019.
//

#ifndef RUNNABLE_CHAIN_RUNNABLECHAIN_H
#define RUNNABLE_CHAIN_RUNNABLECHAIN_H

#include <mutex>
#include <queue>

#include "ThreadPool.h"

class RunnableLink : public Runnable {
  ThreadPool &_tp;
  std::mutex &_queueLock;
  std::queue<std::unique_ptr<RunnableLink>> &_link;
  std::atomic_size_t &_inAction;
  std::unique_ptr<Runnable> _task;

 public:
  RunnableLink(ThreadPool &tp,
               std::mutex &queue_lock,
               std::queue<std::unique_ptr<RunnableLink>> &link,
               std::atomic_size_t &inAction,
               std::unique_ptr<Runnable> task);
  ~RunnableLink() override;
  void run() override;
};

class RunnableChain {
  ThreadPool &_tp;
  std::mutex _queueLock;
  std::queue<std::unique_ptr<RunnableLink>> _link;
  std::atomic_size_t _inAction{0};

 public:
  explicit RunnableChain(ThreadPool &tp);
  void addLink(std::unique_ptr<Runnable> task);
  template <typename Task, typename ...Args>
  void addLink(Args &&...args) {
    auto task = std::make_unique<Task>(std::forward<Args>(args)...);
    addLink(std::move(task));
  }
};

#endif  // RUNNABLE_CHAIN_RUNNABLECHAIN_H
