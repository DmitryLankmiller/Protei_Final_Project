#include "blacklist-manager-fixture.hpp"

TEST_F(BlackListManagerTest, IsBlockedTrueForCurrentIMSI) {
  std::string imsi = "001010123456789";
  BlackListManager blacklist_manager(std::unordered_set<std::string>{imsi});

  EXPECT_TRUE(blacklist_manager.is_blocked(imsi));
}

TEST_F(BlackListManagerTest, IsBlockedFalseForNotIncludedIMSI) {
  std::string imsi = "001010123456789";
  BlackListManager blacklist_manager(std::unordered_set<std::string>{imsi});

  EXPECT_FALSE(blacklist_manager.is_blocked("000000000000000"));
}

TEST_F(BlackListManagerTest, IsBlockedFalseForAnyIMSIWithEmptyBlacklist) {
  BlackListManager blacklist_manager(std::unordered_set<std::string>{});

  EXPECT_FALSE(blacklist_manager.is_blocked("001010123456789"));
  EXPECT_FALSE(blacklist_manager.is_blocked("000000000000000"));
}
