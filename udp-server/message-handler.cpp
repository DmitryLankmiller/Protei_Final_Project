#include "message-handler.hpp"
#include "logger.hpp"

MessageHandler::MessageHandler(
    std::shared_ptr<SessionManager> session_manager,
    std::shared_ptr<BlackListManager> blacklist_manager,
    std::shared_ptr<ICdrWriter> cdr_writer)
    : session_manager(session_manager), blacklist_manager(blacklist_manager),
      cdr_writer(cdr_writer) {}

std::string MessageHandler::decode_imsi_bcd(const uint8_t *data, size_t len) {
  std::string imsi;
  for (size_t i = 0; i < len; ++i) {
    uint8_t byte = data[i];
    uint8_t digit1 = byte & 0x0F;
    uint8_t digit2 = (byte >> 4) & 0x0F;

    imsi += ('0' + digit1);
    if (digit2 != 0xF)
      imsi += ('0' + digit2);
  }
  return imsi;
}

ResponseMessage MessageHandler::handle_message(const uint8_t *data,
                                               size_t len) {
  const std::string imsi = decode_imsi_bcd(data, len);

  ResponseMessage response;

  if (blacklist_manager->is_blocked(imsi)) {
    response = ResponseMessage::Rejected;
  } else {
    auto r = session_manager->create_session(imsi);

    switch (r) {
    case SESSION_CREATED:
      response = ResponseMessage::Created;
      break;
    case SESSION_EXTENDED:
      response = ResponseMessage::Extended;
      break;
    }
  }

  cdr_writer->write(imsi, to_string(response));
  Logger::info("Handle message for IMSI {}: {}", imsi, to_string(response));
  return response;
}
