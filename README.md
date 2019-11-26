# runnable_chain

This code is a POC.
It contains an example of orderd task execution in independed chains.

## Example

```cpp
int main() {
  ThreadPool tp(4); // simple tread pool with 4 threads

  RunnableChain rch1(tp); // chain 1
  RunnableChain rch2(tp); // chain 2
  RunnableChain rch3(tp); // chain 3
  RunnableChain rch4(tp); // chain 4

  for (size_t i = 0; i < 100; ++i) { // add  100 tasks to every chain
    rch1.addLink<TestRunnable>(1 + i, "rch1");
    rch2.addLink<TestRunnable>(1 + i, "rch2");
    rch3.addLink<TestRunnable>(1 + i, "rch3");
    rch4.addLink<TestRunnable>(1 + i, "rch4");
  }

  std::this_thread::sleep_for(std::chrono::seconds(30));

  return 0;
}
```


## Runnable

It is a simple object with virtual method run

```cpp
class Runnable {
 public:
  virtual void run() = 0;
  virtual ~Runnable() = default;
};
```

## ThreadPool

This code contains simple ThreadPool with basic functionality.
```cpp
//...
void addTask(std::unique_ptr<Runnable> task);
//...
```

It can run Runnable object.

## Runnable Link

It's a link of chain. Runnable Link looks like Runnable object. 
And it has a link to the queue of Runnable Links from parent RunnableChain.
This object execute one task, and in destructor it pushs next Runnable Link to the ThreadPool queue.

```cpp
class RunnableLink : public Runnable {
  //...
 public:
  RunnableLink(ThreadPool &tp,
               std::mutex &queue_lock,
               std::queue<std::unique_ptr<RunnableLink>> &link,
               std::atomic_size_t &inAction,
               std::unique_ptr<Runnable> task);
  ~RunnableLink() override;
  void run() override;
};
```

## RunnableChain

It's a queue of Runnable objects. And it contains a link to the ThreadPool.
Method ```addLink``` add Runnable object (RunnableLink) to the ThreadPool if this chain is not in execution now, 
or add Runnable object to the queue.

```cpp
class RunnableChain {
  ThreadPool &_tp;
  std::mutex _queueLock;
  std::queue<std::unique_ptr<RunnableLink>> _link;

 public:
  explicit RunnableChain(ThreadPool &tp);
  void addLink(std::unique_ptr<Runnable> task);
  template <typename Task, typename ...Args>
  void addLink(Args &&...args);
};
```
