#include "config.hpp"
#include <fstream>
#include <iostream>

Config::Config(const std::string &filename) {
  std::ifstream config_file(filename);
  if (!config_file.is_open()) {
    throw std::runtime_error("Failed to open config file");
  }

  try {
    config_file >> config;
  } catch (const nlohmann::json::parse_error &e) {
    std::string error_msg = "Error during JSON parsing: ";
    error_msg += e.what();
    throw std::runtime_error(error_msg);
  }
}
