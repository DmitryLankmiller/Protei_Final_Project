#pragma once

#include <string>
#include <vector>

class UdpClient {
public:
  UdpClient(const std::string &ip, int port);
  void run(const std::string &imsi);

private:
  std::string server_ip;
  int server_port;

  std::vector<uint8_t> encode_imsi_bcd(const std::string &imsi);
};
