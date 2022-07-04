#include "02-monitor.h"
#include "02-semaphore.h"

class MonitorDinnerClass {
  std::vector<bool> need_signals;
  std::vector<Semaphore> signals;
  std::vector<bool> is_occupied;

  void test(int position) {
    int left_fork_id = position;
    int right_fork_id = (position + 1) % signals.size();

    if (need_signals[position] && is_occupied[left_fork_id] == false && is_occupied[right_fork_id] == false) {
      is_occupied[left_fork_id] = true;
      is_occupied[right_fork_id] = true;
      need_signals[position] = false;
      signals[position].Up();
    }
  }

 public:
  MonitorDinnerClass(int n) : need_signals(n, false), signals(n), is_occupied(n, false) {}

  void take_forks(int position) {
    need_signals[position] = true;
    test(position);
  }

  void put_forks(int position) {
    int left_position = (position - 1 + signals.size()) % signals.size();
    int right_position = (position + 1) % signals.size();

    int left_fork_id = position;
    int right_fork_id = (position + 1) % signals.size();
    is_occupied[left_fork_id] = false;
    is_occupied[right_fork_id] = false;

    test(left_position);
    test(right_position);
  }

  class Philosopher {
    friend class MonitorDinnerClass;
    Semaphore *s = nullptr;
    int position = 0;
    std::shared_ptr<std::thread> brain;
    bool is_alive = true;
   public:
    Philosopher() {}
    ~Philosopher() {
      StreamWithGlobalLock() << "position: " << position << ", ~Philosopher" << std::endl;
      is_alive = false;
      if (brain) { brain->join(); }
    }

    void eat() {
      StreamWithGlobalLock() << "position: " << position << ", eating" << std::endl;;
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    void think() {
      StreamWithGlobalLock() << "position: " << position << ", thinking" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    };

    void sit_down(Monitor<MonitorDinnerClass> *dinner, int position) {
      dinner->Execute(&MonitorDinnerClass::sit_down, this, position);
      StreamWithGlobalLock() << "position: " << position << ", sit down" << std::endl;
      brain.reset(new std::thread([this, dinner](){
        while (is_alive) {
          think();
          dinner->Execute(&MonitorDinnerClass::take_forks,this->position);
          s->Down();
          eat();
          dinner->Execute(&MonitorDinnerClass::put_forks, this->position);
        }
        StreamWithGlobalLock() << "position: " << this->position << ", leaves" << std::endl;
      }));
    }
  };

  void sit_down(Philosopher *p, int position) {
    p->position = position;
    p->s = &signals[position];
  }
};

using MonitorDinner = Monitor<MonitorDinnerClass>;
