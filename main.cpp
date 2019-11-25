#include <iostream>
#include <fstream>
#include "RunnableChain.h"

class TestRunnable : public Runnable {
  size_t _count;
  std::string _chain;
  std::ofstream _ofs;
 public:
  explicit TestRunnable(size_t count, std::string chain) : _count(count), _chain(std::move(chain)), _ofs(_chain, std::ios::app) {}
  void run() override {
    _ofs << _count << " started [" << std::this_thread::get_id() << "]" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(_count));
  }
  ~TestRunnable() override { _ofs << _count << " finished" << std::endl; }
};

int main() {
  ThreadPool tp(4);

  RunnableChain rch1(tp);
  RunnableChain rch2(tp);
  RunnableChain rch3(tp);
  RunnableChain rch4(tp);

  for (size_t i = 0; i < 100; ++i) {
    rch1.addLink<TestRunnable>(1 + i, "rch1");
    rch2.addLink<TestRunnable>(1 + i, "rch2");
    rch3.addLink<TestRunnable>(1 + i, "rch3");
    rch4.addLink<TestRunnable>(1 + i, "rch4");
  }

  std::this_thread::sleep_for(std::chrono::seconds(30));

  return 0;
}
