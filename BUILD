"""match-template cli"""

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
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

cc_library(
    name = "load_patterns",
    srcs = ["load_patterns.cc"],
    hdrs = ["load_patterns.h"],
    deps = [
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@opencv",
    ],
)

cc_library(
    name = "ma_service",
    srcs = ["ma_service.cc"],
    hdrs = ["ma_service.h"],
    deps = [
        ":load_patterns",
        ":matching_agent_cc_proto",
        "@boost//:asio",
        "@boost//:interprocess",
        "@com_google_absl//absl/strings",
    ],
)

cc_binary(
    name = "ma_server",
    srcs = [
        "ma_server.cc",
    ],
    deps = [
        ":ma_service",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
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
