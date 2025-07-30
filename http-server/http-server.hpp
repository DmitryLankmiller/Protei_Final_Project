#pragma once
#include "httplib.h"
#include "session-manager.hpp"
#include <cstdint>
#include <memory>

class HttpServer {
public:
  HttpServer(std::shared_ptr<SessionManager> session_manager, uint16_t port,
             std::atomic<bool> &stop_requested);

  void run();
  void stop();

private:
  httplib::Server server;
  uint16_t port;
  std::shared_ptr<SessionManager> session_manager;
  std::atomic<bool> &stop_requested;

  void init();
};
