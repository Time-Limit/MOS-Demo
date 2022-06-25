#include "02-semaphore.h"
#include "02.problem.36.h"

#include "googletest/googletest/include/gtest/gtest.h"

#include <chrono>

TEST(BinarySemaphore, OnlyOneThreadEntersCriticalSection) {
  BinarySemaphore bs;

  for (int i = 0; i < 64; i++) {
    std::vector<std::thread::id> vec;

    std::thread t1([&bs, &vec]() {
      bs.Down();
      for (int i = 0; i < 100; i++) {
        vec.push_back(std::this_thread::get_id());
      }
      bs.Up();
    });

    std::thread t2([&bs, &vec](){
      bs.Down();
      for (int i = 0; i < 100; i++) {
        vec.push_back(std::this_thread::get_id());
      }
      bs.Up();
    });

    t1.join();
    t2.join();

    ASSERT_TRUE(vec.size() == 200);
    for (int i = 0; i < 100; i++) {
      ASSERT_TRUE(vec[i] == vec[0]);
      ASSERT_TRUE(vec[i+100] == vec[100]);
    }
  }
}

TEST(Semaphore, Up) {
  std::vector<std::thread> threads; 

  Semaphore s;

  for (int i = 0; i < 256; i++) {
    threads.emplace_back([&s](){
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      for (int i = 0; i < 1000; i++) {
        s.Up();
      }
    });
  }

  for (auto &t : threads) {
    t.join();
  }

  ASSERT_TRUE(s.Value() == 1000*256);
}

TEST(Semaphore, UpAndDown) {
  std::vector<std::thread> threads; 

  Semaphore s;

  for (int i = 0; i < 256; i++) {
    threads.emplace_back([&s, i](){
      s.Down();
    });
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  ASSERT_TRUE(s.Value() == -256);

  for (int i = 0; i < 256; i++) {
    threads.emplace_back([&s](){ s.Up(); });
  }

  for (auto &t : threads) {
    t.join();
  }

  ASSERT_TRUE(s.Value() == 0);
}

TEST(Restaurant, Work) {
  Restaurant restaurant;
  std::thread send([&restaurant](){
    for (int i = 0; i < 100; i++) {
      restaurant.PlaceAnOrder(std::to_string(i));
    }
  });
  std::thread receive([&restaurant](){
    for (int i = 0; i < 100; i++) {
      Bag bag;
      restaurant.TakeMeal(std::to_string(i), &bag);
      ASSERT_TRUE(bag.customer == std::to_string(i));
      //std::cout << "customer: " << bag.customer
      //  << ", order_taker: " << bag.order_taker
      //  << ", cooker: " << bag.cooker
      //  << ", packaging_specialist: " << bag.packaging_specialist
      //  << ", cashier: " << bag.cashier << std::endl;
    }
  });

  send.join();
  receive.join();
}
