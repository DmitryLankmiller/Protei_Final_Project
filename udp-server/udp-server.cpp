#include "udp-server.hpp"
#include "logger.hpp"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>

void set_non_blocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    Logger::critical("Failed to set non-blocking mode for fd {}", fd);
    throw std::runtime_error("Failed to set non-blocking mode");
  }
}

UdpServer::UdpServer(const std::string &ip, uint16_t port,
                     std::shared_ptr<MessageHandler> handler)
    : handler(handler) {
  setup_socket(ip, port);
  setup_epoll();
}

UdpServer::~UdpServer() {
  stop();
  if (server_fd != -1) {
    close(server_fd);
  }
  if (epoll_fd != -1) {
    close(epoll_fd);
  }
}

void UdpServer::setup_socket(const std::string &ip, uint16_t port) {
  Logger::debug("Creating socket with ip {} and port {}", ip, port);
  server_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_fd == -1) {
    Logger::critical("Failed to create socket");
    throw std::runtime_error("Failed to create socket");
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) < 0) {
    Logger::critical("Failed to bind UDP socket on {}:{}", ip, port);
    throw std::runtime_error("Failed to bind socket");
  }

  set_non_blocking(server_fd);
  Logger::debug("Successfully created socket");
}

void UdpServer::setup_epoll() {
  Logger::debug("Creating epoll");
  epoll_fd = epoll_create1(0);
  epoll_event ev;
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = server_fd;

  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
    Logger::critical("Failed to add server socket to epoll");
    throw std::runtime_error("Failed to add server socket to epoll");
  }
  Logger::debug("Successfully created epoll");
}

void UdpServer::run() {
  Logger::debug("UdpServer::run start");
  running = true;
  epoll_event events[EPOLL_SIZE];

  while (running) {
    Logger::debug("Epoll wait...");
    int events_count = epoll_wait(epoll_fd, events, EPOLL_SIZE, 1000);

    if (events_count == -1) {
      if (errno == EINTR) {
        Logger::debug("Epoll got EINTR, continue...");
        continue;
      }
      Logger::critical("Epoll wait failed");
      throw std::runtime_error("epoll_wait failed");
    }

    Logger::debug("Epoll got {} events", events_count);

    for (int i = 0; i < events_count; i++) {
      if (events[i].data.fd == server_fd) {
        uint8_t buffer[1024];
        sockaddr_in client_addr{};
        socklen_t addr_len = sizeof(client_addr);

        Logger::debug("Recieve message from client");
        ssize_t n = recvfrom(server_fd, buffer, sizeof(buffer), 0,
                             (sockaddr *)&client_addr, &addr_len);
        if (n > 0) {
          auto response_msg = handler->handle_message(buffer, n);
          auto resp = to_string(response_msg);
          Logger::debug("Send message to client");
          n = sendto(server_fd, resp.c_str(), resp.size(), 0,
                     (sockaddr *)&client_addr, addr_len);

          if (n != static_cast<ssize_t>(resp.size())) {
            Logger::critical("Failed to send response to client");
            throw std::runtime_error("failed to send response to client");
          }
        }
      }
    }
  }
  Logger::debug("UdpServer::run end");
}

void UdpServer::stop() { running = false; }