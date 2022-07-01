#pragma once

#include <mutex>
#include <iostream>

class StreamWithGlobalLock {
  std::mutex *global_lock = nullptr;
 public:
  StreamWithGlobalLock() {
    static std::mutex GLOBAL_LOCK;
    global_lock = &GLOBAL_LOCK;
    global_lock->lock();
  }

  template <typename T>
  std::ostream& operator<<(const T& rhs) {
    return (std::cout << rhs);
  }

  ~StreamWithGlobalLock() {
    global_lock->unlock();
  }
};
