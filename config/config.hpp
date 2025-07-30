#pragma once
#include <nlohmann/json.hpp>
#include <string>

class Config {
public:
  Config(const std::string &filename);

  template <typename T>
  T get_property(const std::string &propery_name,
                   const T &default_value) const {
    if (config.contains(propery_name)) {
      return config[propery_name].get<T>();
    }

    return default_value;
  }

private:
  nlohmann::json config;
};
