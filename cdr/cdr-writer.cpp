#include "cdr-writer.hpp"
#include "logger.hpp"
#include <chrono>
#include <iomanip>

CdrWriter::CdrWriter(const std::string &cdr_log_file)
    : cdr_stream(cdr_log_file) {
  if (!cdr_stream.is_open()) {
    Logger::critical("Failed to open CDR log file {}", cdr_log_file);
    throw std::runtime_error("Failed to open CDR log file: " + cdr_log_file);
  }
}

CdrWriter::~CdrWriter() {
  if (cdr_stream.is_open()) {
    cdr_stream.close();
  }
}

void CdrWriter::write(const std::string &imsi, const std::string &action) {
  if (cdr_stream.is_open()) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    cdr_stream << std::put_time(std::localtime(&now_c), "%F %T")
               << ". IMSI: " << imsi << " - " << action << std::endl;
  } else {
    Logger::critical("CDR stream is not opened");
    throw std::runtime_error("CDR stream is not opened");
  }
}
