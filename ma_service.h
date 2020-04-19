#ifndef MATCHING_AGENT_MA_SERVICE_H_
#define MATCHING_AGENT_MA_SERVICE_H_

#include "absl/strings/string_view.h"
#include "load_patterns.h"

namespace matching_agent {

class MatchingAgentService {
 public:
  MatchingAgentService(int port, absl::string_view path_base);
  MatchingAgentService(const MatchingAgentService&) = delete;
  MatchingAgentService& operator=(const MatchingAgentService &) = delete;

 private:
  int port_;
  const PatternMap pattern_map_;
};

}  // namespace matching_agent

#endif  // MATCHING_AGENT_MA_SERVICE_H_
