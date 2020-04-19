#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "load_patterns.h"
#include "opencv2/core/mat.hpp"

/*
  TODO
  Server design:

  - grpc is basically PITA to setup so we have to improvise something
    simple, for now the plan for communication is to encode proto size
    as word32 in little-endian, send this number over, and then the proto.

  - pattern_base is a directory of png files, which are expected to be
    named in a specific way: <tag>_<anything>.png, in which:

    + <tag> can contain anything but underscore.
    + <anything> can be, well, anything, the implementation won't care
    + it however must end with ".png", case insensitive.

  - when an image payload is sent over, it's matched against
    all those patterns, and the server returns one tag with
    highest confidence.

  - pattern images are loaded when the server starts, and will stay
    unchanged throughout.

 */

ABSL_FLAG(int, port, 17151, "Port to listen to.");
ABSL_FLAG(std::string, pattern_base, "", "Base directory to pattern images.");

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  auto pm = matching_agent::LoadPatterns(absl::GetFlag(FLAGS_pattern_base));
  for (const auto & pair : pm) {
    std::cout << pair.first << ": "
              << pair.second.size() << " items\n"
              << "  ";

    std::vector<std::string> pretty;
    std::transform(pair.second.begin(),
                   pair.second.end(),
                   std::back_inserter(pretty),
                   [](const cv::Mat &mat) {
                     return absl::StrCat(mat.rows, "x", mat.cols);
                   });
    std::cout << absl::StrJoin(pretty, ", ") << '\n';
  }
  return 0;
}
