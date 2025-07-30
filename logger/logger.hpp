#pragma once
#include <memory>
#include <spdlog/spdlog.h>
#include <string>

class Logger {
public:
  static void init(const std::string &log_file, const std::string &level,
                   const bool enable_console = true);

  static void debug(const std::string &msg);
  static void info(const std::string &msg);
  static void warn(const std::string &msg);
  static void critical(const std::string &msg);
  static void error(const std::string &msg);

  template <typename... Args>
  static void debug(const fmt::format_string<Args...> fmt, Args &&...args) {
    logger->debug(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void info(const fmt::format_string<Args...> fmt, Args &&...args) {
    logger->info(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void warn(const fmt::format_string<Args...> fmt, Args &&...args) {
    logger->warn(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void error(const fmt::format_string<Args...> fmt, Args &&...args) {
    logger->error(fmt, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void critical(const fmt::format_string<Args...> fmt, Args &&...args) {
    logger->critical(fmt, std::forward<Args>(args)...);
  }

private:
  static std::shared_ptr<spdlog::logger> logger;
};
