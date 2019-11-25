//
// Created by Alexander Bychuk on 20.10.2019.
//

#ifndef RUNNABLE_CHAIN_RUNNABLE_H
#define RUNNABLE_CHAIN_RUNNABLE_H

class Runnable {
 public:
  virtual void run() = 0;
  virtual ~Runnable() = default;
};

#endif // RUNNABLE_CHAIN_RUNNABLE_H
