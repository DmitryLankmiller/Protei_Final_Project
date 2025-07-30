#include "udp-client.hpp"
#include "logger.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

UdpClient::UdpClient(const std::string &ip, int port)
    : server_ip(ip), server_port(port) {}

std::vector<uint8_t> UdpClient::encode_imsi_bcd(const std::string &imsi) {
  std::vector<uint8_t> bcd;
  for (size_t i = 0; i < imsi.size(); i += 2) {
    uint8_t digit1 = imsi[i] - '0';
    uint8_t digit2 = (i + 1 < imsi.size()) ? (imsi[i + 1] - '0') : 0xF;
    bcd.push_back((digit2 << 4) | digit1);
  }
  return bcd;
}

void UdpClient::run(const std::string &imsi) {
  int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (client_fd < 0) {
    Logger::critical("Failed to create socket");
    throw std::runtime_error("Failed to create socket");
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);
  inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);

  std::vector<uint8_t> bcd_imsi = encode_imsi_bcd(imsi);

  ssize_t sent =
      sendto(client_fd, bcd_imsi.data(), bcd_imsi.size(), 0,
             reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr));

  if (sent < 0) {
    Logger::error("Failed to send UDP packet");
    close(client_fd);
    return;
  }

  Logger::info("Sent IMSI to {}:{} ({} bytes)", server_ip, server_port, sent);

  char buffer[1024];
  sockaddr_in from{};
  socklen_t from_len = sizeof(from);

  ssize_t received = recvfrom(client_fd, buffer, sizeof(buffer) - 1, 0,
                              reinterpret_cast<sockaddr *>(&from), &from_len);
  if (received < 0) {
    Logger::error("Failed to receive response");
    close(client_fd);
    return;
  }

  buffer[received] = '\0';
  std::string response(buffer);
  Logger::info("Received response: {}", response);
  std::cout << "Response from server: " << response << std::endl;

  close(client_fd);
}
