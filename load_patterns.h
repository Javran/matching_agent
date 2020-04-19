#ifndef MATCHING_AGENT_LOAD_PATTERNS_H_
#define MATCHING_AGENT_LOAD_PATTERNS_H_

#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/string_view.h"
#include "opencv2/core/mat.hpp"

namespace matching_agent {

using PatternMap = absl::flat_hash_map<std::string, std::vector<cv::Mat>>;

PatternMap LoadPatterns(absl::string_view base_path);

}  // namespace matching_agent


#endif  // MATCHING_AGENT_LOAD_PATTERNS_H_
