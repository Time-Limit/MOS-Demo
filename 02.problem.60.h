#pragma once

#include "02-semaphore.h"

class Bathroom {
  // 互斥量，保护数据
  std::mutex mutex;

  // 条件变量，用于阻塞线程
  std::condition_variable cv;

  // 0 表示无人；大于 0 表示有女生，小于 0 表示有男生
  int64_t count = 0;

 public:
  // 如果进入不成功，则阻塞
  void WomanWantsToEnter() {
    std::unique_lock<std::mutex> lock(mutex);
    if (count < 0) {
      cv.wait(lock, [this](){ return this->count >= 0; });
    }
    count++;
  }

  // 如果进入不成功，则阻塞
  void ManWantsToEnter() {
    std::unique_lock<std::mutex> lock(mutex);
    if (count > 0) {
      cv.wait(lock, [this](){ return this->count <= 0; });
    }
    count--;
  }
  
  void WomanLeaves() {
    bool need_notify = false;
    {
      std::unique_lock<std::mutex> lock(mutex);
      need_notify = !(--count);
    }
    if (need_notify) {
      cv.notify_all();
    }
  }

  void ManLeaves() {
    bool need_notify = false;
    {
      std::unique_lock<std::mutex> lock(mutex);
      need_notify = !(++count);
    }
    if (need_notify) {
      cv.notify_all();
    }
  }
};
