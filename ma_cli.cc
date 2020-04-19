#include <string>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "matching_agent.pb.h"

namespace ma_cli {
enum class MatchMode {
  kSQDIFF = cv::TM_SQDIFF,
  kSQDIFF_NORMED = cv::TM_SQDIFF_NORMED,
  kCCORR = cv::TM_CCORR,
  kCCORR_NORMED = cv::TM_CCORR_NORMED,
  kCCOEFF = cv::TM_CCOEFF,
  kCCOEFF_NORMED = cv::TM_CCOEFF_NORMED,
};

bool AbslParseFlag(absl::string_view text,
                   MatchMode *mode,
                   std::string *error) {
  #define DECL(NAME) \
    if (text == #NAME) { \
      *mode = MatchMode::k##NAME; \
      return true; \
    }
  #define DECL2(NAME) \
    DECL(NAME) \
    DECL(NAME##_NORMED)
  DECL2(SQDIFF)
  DECL2(CCORR)
  DECL2(CCOEFF)
  #undef DECL2
  #undef DECL
  *error = "unknown value for MatchMode";
  return false;
}

std::string AbslUnparseFlag(MatchMode mode) {
  #define CASE(NAME) \
    case MatchMode::k##NAME: return #NAME;
  #define CASE2(NAME) \
    CASE(NAME) \
    CASE(NAME##_NORMED)

  switch (mode) {
    CASE2(SQDIFF)
    CASE2(CCORR)
    CASE2(CCOEFF)
    default: return absl::StrCat(mode);
  }
  #undef CASE2
  #undef CASE
}

}

ABSL_FLAG(std::string, image_path, "", "Image path.");
ABSL_FLAG(std::string, template_path, "", "Template path.");
ABSL_FLAG(std::string, output_path, "", "Output path.");
ABSL_FLAG(ma_cli::MatchMode, match_mode, ma_cli::MatchMode::kSQDIFF, "Match mode.");

int main(int argc, char* argv[]) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  absl::ParseCommandLine(argc, argv);
  cv::Mat image = cv::imread(absl::GetFlag(FLAGS_image_path), 1);
  cv::Mat templ = cv::imread(absl::GetFlag(FLAGS_template_path), 1);
  int method = static_cast<int>(absl::GetFlag(FLAGS_match_mode));

  cv::Mat result;
  int result_cols = image.cols - templ.cols + 1;
  int result_rows = image.rows - templ.rows + 1;
  if (result_cols <= 0 && result_rows <= 0) {
    return EXIT_FAILURE;
  }

  result.create(result_rows,result_cols, CV_32FC1);
  cv::matchTemplate(image, templ, result, method);
  matching_agent::MatchingResult result_pb;
  result_pb.set_rows(result_rows);
  result_pb.set_cols(result_cols);
  // TODO: let's do the stupid but correct way first...
  for (int r = 0; r < result_rows; ++r) {
    for (int c = 0; c < result_cols; ++c) {
      result_pb.add_payload(result.at<float>(r,c));
    }
  }

  std::fstream fout(absl::GetFlag(FLAGS_output_path),
                    std::ios::out | std::ios::trunc | std::ios::binary);
  if (!result_pb.SerializeToOstream(&fout)) {
    std::cerr << "Failed to write result." << std::endl;
    return EXIT_FAILURE;
  }
  return 0;
}
