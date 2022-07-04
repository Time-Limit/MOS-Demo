#pragma once

#include "02-semaphore.h"

template<typename Class>
class Monitor {
  // 过程，变量以及数据结构封装在 Class 中。
  // Class object 是私有的，外部不可能访问到，只能通过 Execute 调用其成员函数。
  Class object;
  BinarySemaphore mutex;
 public:
  template<typename... Types>
  Monitor(Types&&... values) : object(std::forward<Types>(values)...) {}

  // 任一时刻，最多只有一个线程在执行成员函数
  template<typename F, typename... Args>
  decltype(auto) Execute(F f, Args&&... args) {
    struct Keeper {
      BinarySemaphore &bs_;
      Keeper(BinarySemaphore &bs) : bs_(bs) { bs.Down(); }
      ~Keeper() { bs_.Up(); }
    } keeper(mutex);
    return (object.*f)(std::forward<Args>(args)...);
  }
};
