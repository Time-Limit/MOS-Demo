#pragma once

#include "02-message-passing.h"

struct Bag {
  std::string customer;
  std::string order_taker;
  std::string cooker;
  std::string packaging_specialist;
  std::string cashier;
};

class Restaurant {
  std::vector<std::string> order_taker_list;
  std::vector<std::string> cooker_list;
  std::vector<std::string> packaging_specialist_list;
  std::vector<std::string> cashier_list;

  std::vector<std::thread> employees;

  MessagePassing<Bag> message_passing;

  bool is_closed = false;
  std::atomic<int64_t> processing;

 public:
  Restaurant()
    : order_taker_list{"o1"}
    , cooker_list{"co1", "co2", "co3"}
    , packaging_specialist_list{"p1"}
    , cashier_list{"c1"}
    , processing(0)
  {

#define SEND_TO_NEXT_CAREER(next_career) { \
  message_passing.Send(next_career##_list[i], std::move(bag)); \
  if (++i >= next_career##_list.size()) { i = 0; } \
}
#define SEND_TO_CUSTOMER() { \
  message_passing.Send(bag.customer, std::move(bag)); \
}
#define PROCESS(career, send_code) {\
  for (const auto &name : career##_list) { \
    employees.emplace_back([this, name](){ \
      int i = 0; \
      while (true) { \
        Bag bag; \
        message_passing.Receive(name, &bag); \
        if (bag.customer.empty()) { break; } \
        bag.career = name; \
        send_code; \
      } \
    }); \
  } \
} 

    PROCESS(order_taker, SEND_TO_NEXT_CAREER(cooker));
    PROCESS(cooker, SEND_TO_NEXT_CAREER(packaging_specialist));
    PROCESS(packaging_specialist, SEND_TO_NEXT_CAREER(cashier));
    PROCESS(cashier, SEND_TO_CUSTOMER());
#undef PROCESS
  }

  ~Restaurant() {
    std::cout << "closed" << std::endl;
    is_closed = true;
    while (processing != 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      std::cout << "processing: " << processing << std::endl;
    }
    for (auto list : {&order_taker_list, &cooker_list, &packaging_specialist_list, &cashier_list}) {
      for (const auto &name : *list) {
        message_passing.Send(name, {});
      }
    }
    for (auto &e : employees) {
      e.join();
    }
  }

  void PlaceAnOrder(const std::string &customer) {
    if (is_closed) {
      return;
    }
    processing += 1;
    message_passing.Send(order_taker_list[0], {customer});
  }

  void TakeMeal(const std::string &customer, Bag *bag) {
    message_passing.Receive(customer, bag);
    processing -= 1;
  }
};
