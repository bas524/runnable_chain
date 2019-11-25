//
// Created by Alexander Bychuk on 20.10.2019.
//

#include "RunnableChain.h"
RunnableLink::RunnableLink(ThreadPool& tp,
                           std::mutex& queue_lock,
                           std::queue<std::unique_ptr<RunnableLink>>& link,
                           std::atomic_size_t& inAction,
                           std::unique_ptr<Runnable> task)
    : _tp(tp), _queueLock(queue_lock), _link(link), _inAction(inAction), _task(std::move(task)) {
  ++_inAction;
}
void RunnableLink::run() {
  if (_task != nullptr) {
    _task->run();
  }
}
RunnableLink::~RunnableLink() {
  try {
    _task.reset();
    std::unique_ptr<Runnable> nextLink;
    {
      std::lock_guard<std::mutex> lock(_queueLock);
      if (!_link.empty()) {
        std::unique_ptr<RunnableLink> nl = std::move(_link.front());
        nextLink.reset(dynamic_cast<Runnable*>(nl.release()));
        _link.pop();
      }
    }
    if (nextLink != nullptr) {
      _tp.addTask(std::move(nextLink));
    }
    --_inAction;
  } catch (...) {
  }
}
RunnableChain::RunnableChain(ThreadPool& tp) : _tp(tp) {}
void RunnableChain::addLink(std::unique_ptr<Runnable> task) {
  auto newLink = std::make_unique<RunnableLink>(_tp, _queueLock, _link, _inAction, std::move(task));

  std::lock_guard<std::mutex> lock(_queueLock);
  if (_link.empty() && _inAction == 1) {
    _tp.addTask(std::unique_ptr<Runnable>(newLink.release()));
  } else {
    _link.push(std::move(newLink));
  }
}
