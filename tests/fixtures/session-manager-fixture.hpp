#pragma once

#include "base-fixture.hpp"
#include "mock-cdr-writer.hpp"
#include "session-manager.hpp"
#include <gtest/gtest.h>
#include <memory>

class SessionManagerTest : public BaseTestFixture {
protected:
  std::shared_ptr<MockCdrWriter> mock_cdr_writer;
  std::shared_ptr<SessionManager> session_manager;

  void SetUp() override {
    BaseTestFixture::SetUp();
    mock_cdr_writer = std::make_shared<MockCdrWriter>();
    session_manager = std::make_shared<SessionManager>(
        std::static_pointer_cast<ICdrWriter>(mock_cdr_writer), 30);
  }
};
