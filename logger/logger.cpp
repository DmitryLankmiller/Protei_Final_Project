#include "logger.hpp"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Logger::logger = nullptr;

void Logger::init(const std::string &log_file, const std::string &level,
                  const bool enable_console) {
  if (logger)
    return;

  std::vector<spdlog::sink_ptr> sinks;

  if (enable_console) {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    sinks.push_back(console_sink);
  }

  if (!log_file.empty()) {
    auto file_sink =
        std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file, true);
    sinks.push_back(file_sink);
  }

  logger =
      std::make_shared<spdlog::logger>("pgw_logger", begin(sinks), end(sinks));
  spdlog::register_logger(logger);

  logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");

  if (level == "debug")
    logger->set_level(spdlog::level::debug);
  else if (level == "info")
    logger->set_level(spdlog::level::info);
  else if (level == "warn")
    logger->set_level(spdlog::level::warn);
  else if (level == "error")
    logger->set_level(spdlog::level::err);
  else if (level == "critical")
    logger->set_level(spdlog::level::critical);
  else
    logger->set_level(spdlog::level::info);
}

void Logger::debug(const std::string &msg) { logger->debug(msg); }
void Logger::info(const std::string &msg) { logger->info(msg); }
void Logger::warn(const std::string &msg) { logger->warn(msg); }
void Logger::error(const std::string &msg) { logger->error(msg); }
void Logger::critical(const std::string &msg) { logger->critical(msg); }
