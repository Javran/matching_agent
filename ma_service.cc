#include "ma_service.h"

#include "absl/strings/string_view.h"
#include "load_patterns.h"

namespace matching_agent {

MatchingAgentService::MatchingAgentService(int port, absl::string_view path_base)
  : port_(port),
    pattern_map_(LoadPatterns(path_base)) {}

}  // namespace matching_agent
