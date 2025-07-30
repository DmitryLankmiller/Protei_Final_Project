#pragma once

#include "base-fixture.hpp"
#include "blacklist-manager.hpp"
#include <gtest/gtest.h>

class BlackListManagerTest : public BaseTestFixture {
protected:
  void SetUp() override { BaseTestFixture::SetUp(); }
};
