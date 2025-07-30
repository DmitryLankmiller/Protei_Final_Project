#include "session-manager.hpp"
#include "logger.hpp"
#include <thread>

std::chrono::steady_clock::time_point now() {
  return std::chrono::steady_clock::now();
}

SessionManager::SessionManager(std::shared_ptr<ICdrWriter> cdr_writer,
                               int session_timeout_s)
    : cdr_writer(cdr_writer), session_timeout(session_timeout_s) {}

int SessionManager::create_session(const std::string &imsi) {
  std::lock_guard<std::mutex> lock(mutex);

  auto it = sessions.find(imsi);

  if (it != sessions.end()) {
    it->second.created_at = now();
    Logger::info("Session extended for IMSI {}", imsi);
    return SESSION_EXTENDED;
  }

  sessions.emplace(imsi, Session{.imsi = imsi, .created_at = now()});
  Logger::info("Session created for IMSI {}", imsi);
  return SESSION_CREATED;
}

bool SessionManager::has_session(const std::string &imsi) {
  std::lock_guard<std::mutex> lock(mutex);

  Logger::debug("Check if session exists for IMSI {}", imsi);

  auto it = sessions.find(imsi);
  if (it != sessions.end()) {
    const auto current_time = now();

    if (current_time - it->second.created_at > session_timeout) {
      remove_session(it);
      Logger::debug("Session is not exist for IMSI {}", imsi);
      return false;
    }

    Logger::debug("Session exists for IMSI {}", imsi);
    return true;
  }

  Logger::debug("Session is not exist for IMSI {}", imsi);
  return false;
}

void SessionManager::remove_session(const std::string &imsi) {
  cdr_writer->write(imsi, "session removed");
  Logger::info("Session removed for IMSI {}", imsi);
  sessions.erase(imsi);
}

std::unordered_map<std::string, Session>::iterator
SessionManager::remove_session(
    std::unordered_map<std::string, Session>::iterator &it) {
  cdr_writer->write(it->second.imsi, "session removed");
  Logger::info("Session removed for IMSI {}", it->second.imsi);
  return sessions.erase(it);
}

void SessionManager::remove_expired_sessions() {
  Logger::debug("SessionManager::remove_expired_sessions start");
  std::lock_guard<std::mutex> lock(mutex);

  const auto current_time = now();

  for (auto it = sessions.begin(); it != sessions.end();) {
    if (current_time - it->second.created_at > session_timeout) {
      Logger::info("Session expired for IMSI {}", it->second.imsi);
      it = remove_session(it);
    } else {
      ++it;
    }
  }
  Logger::debug("SessionManager::remove_expired_sessions end");
}

void SessionManager::graceful_shutdown(int rate_per_s) {
  std::lock_guard<std::mutex> lock(mutex);
  while (!sessions.empty()) {
    auto it = sessions.begin();
    for (int i = 0; i < rate_per_s && it != sessions.end(); ++i) {
      it = remove_session(it);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}