"""match-template cli"""

load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_proto//proto:defs.bzl", "proto_library")

cc_binary(
    name = "ma_cli",
    srcs = ["ma_cli.cc"],
    deps = [
        ":matching_agent_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings",
        "@opencv",
    ],
)

cc_proto_library(
    name = "matching_agent_cc_proto",
    deps = [":matching_agent_proto"],
)

proto_library(
    name = "matching_agent_proto",
    srcs = ["matching_agent.proto"],
)
