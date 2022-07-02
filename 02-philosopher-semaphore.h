#include "02-semaphore.h"
#include "base.h"

#include <thread>
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <sstream>

class SemaphoreDinner {
  enum Status {
    THINKING = 0,
    HUNGRY,
    EATING,
  };

  bool isBusinessHours = true;

  BinarySemaphore mutex; // 互斥，保护所有哲学家的 Philosopher::status

  class Philosopher;
  std::vector<std::shared_ptr<Philosopher>> philosophers;

  /*
   *  std::thread brain，用于模拟哲学家活动的线程
   *  Semaphore block，保护用于阻塞哲学家线程
   *  status status，哲学家的状态
   */
  class Philosopher {
    int position;
    SemaphoreDinner *dinner = nullptr;

    std::thread brain;
    Semaphore block;
    Status status = THINKING;

    void think() {
      StreamWithGlobalLock() << "position: " << position << ", thinking" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    };

    void eat() {
      StreamWithGlobalLock() << "position: " << position << ", eating" << std::endl;;
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    void take_fork() {
      dinner->mutex.Down();
      status = HUNGRY;
      test(position);
      dinner->mutex.Up();

      this->block.Down();
    }

    void put_fork() {
      dinner->mutex.Down();
      status = THINKING;
      int l = (position - 1 + dinner->philosophers.size()) % dinner->philosophers.size();
      int r = (position + 1) % dinner->philosophers.size();
      test(l);
      test(r);
      dinner->mutex.Up();
    }

    void test(int p) {
      int l = (p - 1 + dinner->philosophers.size()) % dinner->philosophers.size();
      int r = (p + 1) % dinner->philosophers.size();
      if (dinner->philosophers[p]->status == HUNGRY) {
        if (dinner->philosophers[l]->status != EATING && dinner->philosophers[r]->status != EATING) {
          dinner->philosophers[p]->status = EATING;
          dinner->philosophers[p]->block.Up();
        }
      }
    }

   public:
    Philosopher(int p, SemaphoreDinner *d) : position(p), dinner(d), brain([this](){
      while (dinner->isBusinessHours) {
        this->think();
        this->take_fork();
        this->eat();
        this->put_fork();
      }
      StreamWithGlobalLock() << "position: " << position << ", leaves" << std::endl;
    }) {}
    ~Philosopher() {
      brain.join();
    }
  };
 public:
  SemaphoreDinner(int n = 10) {
    for (int i = 0; i < n; i++) {
      philosophers.emplace_back(new Philosopher(i, this));
    }
  }
  ~SemaphoreDinner() {
    isBusinessHours = false;
  }
};
