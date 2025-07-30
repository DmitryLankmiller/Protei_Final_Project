#pragma once

#include "logger.hpp"
#include <gtest/gtest.h>

class BaseTestFixture : public ::testing::Test {
protected:
  void SetUp() override { Logger::init("", "debug", false); }
};
