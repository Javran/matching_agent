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

namespace {

using boost::asio::ip::tcp;

template <class Message>
absl::optional<Message> RecvProto(tcp::socket *sock) {
  if (sock == nullptr) return absl::nullopt;
  uint32_t size;
  {
    std::array<uint8_t,4> raw_size;
    boost::asio::read(*sock,
                      boost::asio::buffer(raw_size),
                      boost::asio::transfer_at_least(4));
    // size is transferred using little endian.
    size = raw_size[0];
    size |= uint32_t(raw_size[1]) << 8;
    size |= uint32_t(raw_size[2]) << 16;
    size |= uint32_t(raw_size[3]) << 24;
    size = boost::endian::little_to_native(size);
  }
  Message req;
  std::vector<char> buff(size);
  boost::asio::read(*sock,
                    boost::asio::buffer(buff),
                    boost::asio::transfer_at_least(size));
  boost::interprocess::bufferstream input(buff.data(), buff.size());
  req.ParseFromIstream(&input);
  return req;
}

template <class Message>
void SendProto(tcp::socket *sock, const Message &msg) {
  if (sock == nullptr) return;

  std::string raw_msg;
  msg.SerializeToString(&raw_msg);
  uint32_t size = raw_msg.size();
  size = boost::endian::native_to_little(size);
  std::array<uint8_t,4> raw_size =
    { uint8_t(size & 0xff),
      uint8_t((size >> 8) & 0xff),
      uint8_t((size >> 16) & 0xff),
      uint8_t((size >> 24) & 0xff),
    };
  sock->send(boost::asio::buffer(raw_size, 4));
  sock->send(boost::asio::buffer(raw_msg.c_str(),
                                 raw_msg.size()));
}

}  // namespace

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
  absl::optional<FindTagRequest> request = RecvProto<FindTagRequest>(&sock);
  if (!request.has_value()) return;
  FindTagRequest &req = request.value();

  // TODO: convert to a Mat by first holding the data in a vector
  // and then use imdecode on it.
  // there might be better way to do this, for now I'm just making sure this is correct.
  std::vector<char> tmp(req.payload().begin(), req.payload().end());
  FindTagResponse response;
  cv::Mat mat = cv::imdecode(std::move(tmp), cv::IMREAD_COLOR);
  if (auto result = FindTag(mat);
      result.has_value()) {
    response.set_tag(std::string(result.value().first));
    response.set_result(result.value().second);
  }
  SendProto(&sock, response);
  sock.close();
}

absl::optional<MatchingAgentService::FindTagResult>
  MatchingAgentService::FindTag(const cv::Mat image) {
  // TODO: generalize method.
  int method = cv::TM_CCORR_NORMED;
  absl::optional<FindTagResult> current_best;
  for (auto const &tag_pair : pattern_map_) {
    // std::cout << "On tag: " << tag_pair.first << ", ";
    for (auto const &templ : tag_pair.second) {
      if (image.cols >= templ.cols && image.rows >= templ.rows) {
        cv::Mat result;
        cv::matchTemplate(image, templ, result, method);
        double maxVal;
        cv::minMaxLoc(result, nullptr, &maxVal);
        // std::cout << maxVal << ' ';
        if (!current_best.has_value() ||
            current_best.value().second < maxVal) {
          current_best = {tag_pair.first, maxVal};
        }
      }
    }
    // std::cout << '\n';
  }
  return current_best;
}

}  // namespace matching_agent
