#include "config.hpp"
#include "logger.hpp"
#include "udp-client.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: pgw_client <IMSI>\n";
    return 1;
  }

  std::string imsi = argv[1];

  Config config("client-config.json");

  const auto server_ip =
      config.get_property<std::string>("server_ip", "127.0.0.1");
  const auto server_port = config.get_property<int>("server_port", 9000);
  const auto log_file =
      config.get_property<std::string>("log_file", "client.log");
  const auto log_level = config.get_property<std::string>("log_level", "info");

  Logger::init(log_file, log_level);
  Logger::info("Client started with IMSI: {}", imsi);

  UdpClient client(server_ip, server_port);
  client.run(imsi);

  return 0;
}
