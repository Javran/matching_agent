#include "ma_service.h"

#include <array>
#include <memory>
#include <thread>
#include <vector>
#include <iostream>

#include "absl/strings/string_view.h"
#include "absl/types/optional.h"
#include "boost/asio.hpp"
#include "boost/endian/conversion.hpp"
#include "boost/interprocess/streams/bufferstream.hpp"
#include "load_patterns.h"
#include "matching_agent.pb.h"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

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

// TODO: bullet-proof against exceptions.
void MatchingAgentService::Session(tcp::socket sock) {
  uint32_t size;
  {
    std::array<uint8_t,4> raw_size;
    boost::asio::read(sock,
                      boost::asio::buffer(raw_size),
                      boost::asio::transfer_at_least(4));
    // size is transferred using little endian.
    size = raw_size[0];
    size |= uint32_t(raw_size[1]) << 8;
    size |= uint32_t(raw_size[2]) << 16;
    size |= uint32_t(raw_size[3]) << 24;
    size = boost::endian::little_to_native(size);
  }
  FindTagRequest req;
  {
    std::vector<char> buff(size);
    boost::asio::read(sock,
                      boost::asio::buffer(buff),
                      boost::asio::transfer_at_least(size));
    boost::interprocess::bufferstream input(buff.data(), buff.size());
    req.ParseFromIstream(&input);
  }

  // TODO: stupid, but works.
  std::vector<char> tmp(req.payload().begin(), req.payload().end());
  FindTagResponse response;
  cv::Mat mat = cv::imdecode(std::move(tmp), cv::IMREAD_COLOR);
  if (auto result = FindTag(mat);
      result.has_value()) {
    response.set_tag(std::string(result.value().first));
    response.set_result(result.value().second);
  }
  // TODO: send encoded message.
  sock.close();
}

absl::optional<MatchingAgentService::FindTagResult>
  MatchingAgentService::FindTag(const cv::Mat image) {
  // TODO: generalize method.
  int method = cv::TM_CCOEFF_NORMED;
  absl::optional<FindTagResult> current_best;
  for (auto const &tag_pair : pattern_map_) {
    for (auto const &templ : tag_pair.second) {
      if (image.cols >= templ.cols && image.rows >= templ.rows) {
        cv::Mat result;
        cv::matchTemplate(image, templ, result, method);
        double maxVal;
        cv::minMaxLoc(result, nullptr, &maxVal);
        if (!current_best.has_value() ||
            current_best.value().second < maxVal) {
          current_best = {tag_pair.first, maxVal};
        }
      }
    }
  }
  return current_best;
}

}  // namespace matching_agent
