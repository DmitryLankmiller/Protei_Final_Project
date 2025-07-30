#pragma once

#include "message-handler.hpp"
#include <stdint.h>

class UdpServer {
public:
  UdpServer(const std::string &ip, uint16_t port,
            std::shared_ptr<MessageHandler> handler);
  ~UdpServer();

  void run();
  void stop();

private:
  const int EPOLL_SIZE = 10000;
  int server_fd = -1;
  int epoll_fd = -1;
  bool running = false;
  std::shared_ptr<MessageHandler> handler;

  void setup_socket(const std::string &ip, uint16_t port);
  void setup_epoll();
};