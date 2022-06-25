#pragma once

#include <thread>
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>

#include <mutex>
#include <condition_variable>
#include "02-semaphore.h"

template<typename Message>
class MessagePassing {
  // mutex 用来保护 messages，任意时刻最多有一个线程修改 messages 的哈希结构。
  std::mutex mutex;

  // messages::first → destination
  // messages::second::first → 用于无消息时阻塞 Receive 线程
  // messages::second::second → 互斥，用于保护 messages::second::third
  // messages::second::third → 用于存储未被接收的消息
  std::unordered_map<std::string, std::tuple<Semaphore, BinarySemaphore, std::queue<Message>>> messages;

 public:
  // 将 msg 发送给 destination
  void Send(const std::string &destination, Message &&msg) {
    typename decltype(messages)::mapped_type *tuple = nullptr;
    {
      std::unique_lock<std::mutex> lock(mutex);
      tuple = &messages[destination];
    }
    std::get<1>(*tuple).Down();
    std::get<2>(*tuple).push(std::move(msg));
    std::get<1>(*tuple).Up();
    std::get<0>(*tuple).Up();
  }

  // 接收发送给 destination 的msg，如无则阻塞。
  void Receive(const std::string &destination, Message *msg) {
    typename decltype(messages)::mapped_type *tuple = nullptr;
    {
      std::unique_lock<std::mutex> lock(mutex);
      tuple = &messages[destination];
    }
    std::get<0>(*tuple).Down();
    std::get<1>(*tuple).Down();
    *msg = std::move(std::get<2>(*tuple).front());
    std::get<2>(*tuple).pop();
    std::get<1>(*tuple).Up();
  }
};
