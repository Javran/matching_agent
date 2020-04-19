#include "load_patterns.h"

#include <string>
#include <iostream>
#include <filesystem>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/match.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "opencv2/imgcodecs.hpp"

namespace matching_agent {

PatternMap LoadPatterns(absl::string_view base_path) {
  PatternMap pm;
  for (const auto &item: std::filesystem::directory_iterator(base_path)) {
    auto &path = item.path();
    if (item.is_regular_file() &&
        absl::EqualsIgnoreCase(std::string(path.extension()), ".png")) {
      std::string basename = path.stem();
      std::vector<absl::string_view> parts =
        absl::StrSplit(basename, absl::MaxSplits('_', 1));
      if (parts.size() == 0) continue;
      pm[parts[0]].emplace_back(cv::imread(path));
    }
  }
  return pm;
}

}  // namespace matching_agent
