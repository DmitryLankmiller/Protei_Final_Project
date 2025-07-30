#pragma once
#include <string>

class ICdrWriter {
public:
  virtual ~ICdrWriter() = default;
  virtual void write(const std::string &imsi, const std::string &action) = 0;
};
