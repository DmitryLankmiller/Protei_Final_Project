#include "blacklist-manager.hpp"
#include "cdr-writer.hpp"
#include "config.hpp"
#include "http-server.hpp"
#include "logger.hpp"
#include "message-handler.hpp"
#include "session-manager.hpp"
#include "udp-server.hpp"

#include <atomic>
#include <csignal>
#include <thread>

std::atomic<bool> *global_stop_requested = nullptr;

void signal_handler(int signal) {
  if (signal == SIGINT) {
    Logger::warn("Received SIGINT (Ctrl+C), stopping...");
    if (global_stop_requested) {
      *global_stop_requested = true;
    }
  }
}

int main() {
  std::atomic<bool> stop_requested(false);
  std::signal(SIGINT, signal_handler);
  global_stop_requested = &stop_requested;

  const Config config("server-config.json");

  const auto upd_ip = config.get_property<std::string>("udp_ip", "0.0.0.0");
  const auto udp_port = config.get_property<int>("udp_port", 9000);
  const auto session_timeout_s =
      config.get_property<int>("session_timeout_sec", 30);
  const auto cdr_log_file =
      config.get_property<std::string>("cdr_file", "cdr.log");
  const auto http_port = config.get_property<int>("http_port", 8080);
  const auto graceful_shutdown_rate =
      config.get_property<int>("graceful_shutdown_rate", 5);
  const auto log_file = config.get_property<std::string>("log_file", "pwg.log");
  const auto log_level = config.get_property<std::string>("log_level", "INFO");
  const auto blacklist_vector = config.get_property<std::vector<std::string>>(
      "blacklist", std::vector<std::string>{});
  const std::unordered_set<std::string> blacklist(blacklist_vector.begin(),
                                                  blacklist_vector.end());

  Logger::init(log_file, log_level);

  Logger::info("======= Read config file =======");
  Logger::info("UDP address:     {}:{}", upd_ip, udp_port);
  Logger::info("Session timeout: {} seconds", session_timeout_s);
  Logger::info("CDR logfile:     {}", cdr_log_file);
  Logger::info("HTTP port:       {}", http_port);
  Logger::info("Log file:        {}", log_file);
  Logger::info("Log level:       {}", log_level);
  Logger::info("Blacklist size:  {}", blacklist.size());
  Logger::info("===============================");

  Logger::info("Start inizialization");

  auto cdr_writer = std::make_shared<CdrWriter>("cdr.log");
  auto session_manager =
      std::make_shared<SessionManager>(cdr_writer, session_timeout_s);
  auto blacklist_manager = std::make_shared<BlackListManager>(blacklist);

  auto handler = std::make_shared<MessageHandler>(
      session_manager, blacklist_manager, cdr_writer);

  UdpServer udp_server(upd_ip, udp_port, handler);
  HttpServer http_server(session_manager, http_port, stop_requested);

  Logger::info("Inizialization done");

  Logger::info("Start UDP server");
  std::thread udp_thread([&]() {
    udp_server.run();
    Logger::debug("UDP thread done");
  });
  Logger::info("Start HTTP server");
  std::thread http_thread([&]() {
    http_server.run();
    Logger::debug("HTTP thread done");
  });
  Logger::info("Start cleaner-thread");
  std::thread cleaner_thread([&]() {
    while (!stop_requested) {
      std::this_thread::sleep_for(std::chrono::seconds(5));
      if (stop_requested)
        break;
      Logger::info("Cleaning expired sessions");
      session_manager->remove_expired_sessions();
    }
    Logger::debug("Cleaner thread done");
  });

  while (!stop_requested) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  Logger::info("Stop UDP server");
  udp_server.stop();
  Logger::info("Start graceful shutdown");
  session_manager->graceful_shutdown(graceful_shutdown_rate);
  Logger::info("Graceful shutdown done");
  Logger::info("Stop HTTP server");
  http_server.stop();

  udp_thread.join();
  http_thread.join();
  cleaner_thread.join();
}