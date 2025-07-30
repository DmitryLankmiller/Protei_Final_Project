#pragma once
#include "blacklist-manager.hpp"
#include "icdr-writer.hpp"
#include "session-manager.hpp"
#include <memory>
#include <netinet/in.h>
#include <string>

enum class ResponseMessage { Created, Extended, Rejected };

inline std::string to_string(ResponseMessage r) {
  switch (r) {
  case ResponseMessage::Created:
    return "created";
  case ResponseMessage::Extended:
    return "extended";
  case ResponseMessage::Rejected:
    return "rejected";
  }
  throw std::logic_error("Unknown ResponseMessage value");
}

class MessageHandler {
public:
  MessageHandler(std::shared_ptr<SessionManager> session_manager,
                 std::shared_ptr<BlackListManager> blacklist_manager,
                 std::shared_ptr<ICdrWriter> cdr_writer);

  ResponseMessage handle_message(const uint8_t *data, size_t len);

private:
  std::shared_ptr<SessionManager> session_manager;
  std::shared_ptr<BlackListManager> blacklist_manager;
  std::shared_ptr<ICdrWriter> cdr_writer;

  std::string decode_imsi_bcd(const uint8_t *data, size_t len);
};