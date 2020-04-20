#ifndef MATCHING_AGENT_MA_SERVICE_H_
#define MATCHING_AGENT_MA_SERVICE_H_

#include "absl/strings/string_view.h"
#include "boost/asio/ip/tcp.hpp"
#include "load_patterns.h"

namespace matching_agent {

using boost::asio::ip::tcp;

class MatchingAgentService {
 public:
  MatchingAgentService(int port, absl::string_view path_base);
  MatchingAgentService(const MatchingAgentService&) = delete;
  MatchingAgentService& operator=(const MatchingAgentService &) = delete;

  void Run();

 private:
  int port_;
  const PatternMap pattern_map_;
  void Session(tcp::socket sock);
};

}  // namespace matching_agent

#endif  // MATCHING_AGENT_MA_SERVICE_H_
