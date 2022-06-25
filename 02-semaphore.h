#pragma once

#include <thread>
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <sstream>

#include <mutex>
#include <condition_variable>

// 二元信号量的定义(74页)：
// * 初值为 1。
// * 保证任意时刻最多有一个线程进入临界区. 但这一条需要调用线程配合，即进入前调用 Down，退出前调用 Up.
class BinarySemaphore {
  // 互斥量，保护数据
  std::mutex mutex;

  // 条件变量，用于阻塞线程
  std::condition_variable cv;

  int64_t value = 1;

 public:
  void Up() {
    {
      std::unique_lock<std::mutex> lock(mutex);
      ++value;
    }
    if (value > 0) {
      cv.notify_one();
    }
  }
  void Down() {
    std::unique_lock<std::mutex> lock(mutex);
    while (value == 0) {
      cv.wait(lock, [this]() -> bool { return value != 0; });
    }
    --value;
  }
};

class Semaphore {
  BinarySemaphore mutex;
  BinarySemaphore block;
  int64_t value = 0;

 public:
  Semaphore() {
    // 先 Down 一次，把初始值消耗掉
    block.Down();
  }

  int64_t Value() const {
    return value;
  }

  void Down() {
    mutex.Down();
    if (value > 0) {
      value--;
      mutex.Up();
    } else {
      value--;
      mutex.Up();
      block.Down();
    }
  }

  void Up() {
    mutex.Down();
    if (value < 0) {
      value++;
      block.Up();
      mutex.Up();
    } else {
      value++;
      mutex.Up();
    }
  }
};
