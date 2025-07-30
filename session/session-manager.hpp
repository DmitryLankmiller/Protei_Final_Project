#pragma once

#include "icdr-writer.hpp"
#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#define SESSION_CREATED 0
#define SESSION_EXTENDED 1

struct Session {
  std::string imsi;
  std::chrono::steady_clock::time_point created_at;
};

class SessionManager {
public:
  SessionManager(std::shared_ptr<ICdrWriter> cdr_writer, int session_timeout_s);

  /**
   * Создаёт новую сессию или продлевает существующую
   *
   * @param imsi IMSI пользователя, для которого создаём сессию
   * @return Возвращает SESSION_CREATED, если сессия создана; возвращает
   * SESSION_EXTENDED, если сессия продлена;
   */
  int create_session(const std::string &imsi);

  bool has_session(const std::string &imsi);

  void remove_expired_sessions();

  void graceful_shutdown(int rate_per_s);

private:
  std::unordered_map<std::string, Session> sessions;
  std::mutex mutex;
  std::chrono::seconds session_timeout;
  std::shared_ptr<ICdrWriter> cdr_writer;

  void remove_session(const std::string &imsi);
  std::unordered_map<std::string, Session>::iterator
  remove_session(std::unordered_map<std::string, Session>::iterator &);
};
