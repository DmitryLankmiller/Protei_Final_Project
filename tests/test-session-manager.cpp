#include "session-manager-fixture.hpp"

TEST_F(SessionManagerTest, CreatesSessionSuccess) {
  std::string imsi = "001010123456789";

  session_manager->create_session(imsi);
  EXPECT_TRUE(session_manager->has_session(imsi));
}

TEST_F(SessionManagerTest, NoSessionSuccess) {
  std::string imsi = "001010123456789";

  EXPECT_FALSE(session_manager->has_session(imsi));
}

TEST_F(SessionManagerTest, ExtendSession) {
  std::string imsi = "001010123456789";

  session_manager->create_session(imsi);
  session_manager->create_session(imsi);
  EXPECT_TRUE(session_manager->has_session(imsi));
}
