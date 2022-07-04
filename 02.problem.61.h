#pragma once

#include <vector>
#include <thread>

class StrictAlternation {
  std::vector<std::thread> threads;
  uint32_t turn = 0;

 public:
  StrictAlternation(uint32_t thread_number, int repeated_time, std::function<void(uint32_t id)> work) {
    for (int i = 0; i < thread_number; i++) {
      threads.emplace_back([thread_number, repeated_time, work, this, id = i] () {
        for (int j = 0; j < repeated_time; j++) {
          while (turn != id) {}
          work(id);
          turn = (turn+1)%thread_number;
        }
      });
    }
  }

  ~StrictAlternation() {
    for (auto &thread : threads) {
      thread.join();
    }
  }
};
