#include "blacklist-manager.hpp"

BlackListManager::BlackListManager(std::unordered_set<std::string> imsi_set)
    : blacklist(std::move(imsi_set)) {}

bool BlackListManager::is_blocked(const std::string &imsi) const {
  return blacklist.find(imsi) != blacklist.end();
}
