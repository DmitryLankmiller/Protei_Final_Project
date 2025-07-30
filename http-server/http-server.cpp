#include "http-server.hpp"
#include "logger.hpp"

HttpServer::HttpServer(std::shared_ptr<SessionManager> session_manager,
                       uint16_t port, std::atomic<bool> &stop_requested)
    : session_manager(session_manager), port(port),
      stop_requested(stop_requested) {}

void HttpServer::init() {
  server.Get("/check_subscriber", [this](const httplib::Request &request,
                                         httplib::Response &response) {
    Logger::info("Received /check_subscriber request");

    if (!request.has_param("imsi")) {
      Logger::warn("Missing 'imsi' parameter in request");
      response.status = 400;
      response.set_content("Missing imsi", "text/plain");
      return;
    }
    std::string imsi = request.get_param_value("imsi");

    std::string response_msg =
        session_manager->has_session(imsi) ? "active" : "not active";

    Logger::info("Subscriber with IMSI {} is {}", imsi, response_msg);
    response.set_content(response_msg, "text/plain");
  });

  server.Get("/stop", [this](const httplib::Request &request,
                             httplib::Response &response) {
    Logger::warn("Received /stop request — initiating shutdown");
    stop_requested = true;
    response.set_content("Stopping", "text/plain");
  });
}

void HttpServer::run() {
  init();
  server.listen("0.0.0.0", port);
}

void HttpServer::stop() { server.stop(); }