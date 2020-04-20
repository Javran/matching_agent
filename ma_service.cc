#include "ma_service.h"

#include <array>
#include <memory>
#include <thread>
#include <vector>

#include "absl/strings/string_view.h"
#include "boost/asio.hpp"
#include "boost/interprocess/streams/bufferstream.hpp"
#include "load_patterns.h"
#include "matching_agent.pb.h"

namespace matching_agent {

MatchingAgentService::MatchingAgentService(int port, absl::string_view path_base)
  : port_(port),
    pattern_map_(LoadPatterns(path_base)) {}

void MatchingAgentService::Run() {
  boost::asio::io_context ioc;
  tcp::endpoint endpoint(tcp::v4(), port_);
  tcp::acceptor acceptor(ioc, endpoint);
  acceptor.set_option(tcp::acceptor::reuse_address(true));
  for (;;) {
    std::thread(&MatchingAgentService::Session, this, acceptor.accept()).detach();
  }
}

void MatchingAgentService::Session(tcp::socket sock) {
  uint32_t size;
  {
    std::array<uint8_t,4> raw_size;
    boost::asio::read(sock, boost::asio::buffer(raw_size), boost::asio::transfer_at_least(4));
    // assumed local representation is little-endian.
    size = raw_size[0];
    size |= uint32_t(raw_size[1]) << 8;
    size |= uint32_t(raw_size[2]) << 16;
    size |= uint32_t(raw_size[3]) << 24;
  }
  std::vector<char> buff(size);
  boost::asio::read(sock, boost::asio::buffer(buff), boost::asio::transfer_at_least(size));
  boost::interprocess::bufferstream input(buff.data(), buff.size());
  FindTagRequest req;
  req.ParseFromIstream(&input);
  sock.close();
}

}  // namespace matching_agent
