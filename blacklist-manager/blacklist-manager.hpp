#pragma once
#include <string>
#include <unordered_set>

class BlackListManager {
public:
  BlackListManager(std::unordered_set<std::string> imsi_set);
  bool is_blocked(const std::string &imsi) const;

private:
  std::unordered_set<std::string> blacklist;
};
