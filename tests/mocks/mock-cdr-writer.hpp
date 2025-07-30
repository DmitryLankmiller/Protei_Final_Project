#pragma once

#include "icdr-writer.hpp"
#include <gmock/gmock.h>

class MockCdrWriter : public ICdrWriter {
public:
  MOCK_METHOD(void, write, (const std::string &imsi, const std::string &action),
              (override));
};
