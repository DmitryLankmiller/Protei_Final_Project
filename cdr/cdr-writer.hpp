#pragma once
#include "icdr-writer.hpp"
#include <fstream>
#include <iostream>
#include <string>

class CdrWriter : public ICdrWriter {
public:
  CdrWriter(const std::string &cdr_log_file);
  ~CdrWriter();

  void write(const std::string &imsi, const std::string &action);

private:
  std::ofstream cdr_stream;
};
