#include "ma_service.h"

#include "absl/strings/string_view.h"
#include "boost/asio.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "load_patterns.h"

namespace matching_agent {

MatchingAgentService::MatchingAgentService(int port, absl::string_view path_base)
  : port_(port),
    pattern_map_(LoadPatterns(path_base)) {}

void MatchingAgentService::Run() {
  boost::asio::io_context ioc;
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port_);
  boost::asio::ip::tcp::acceptor acceptor(ioc, endpoint);
  acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
}

}  // namespace matching_agent
