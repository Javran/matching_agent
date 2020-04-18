"""match-template cli"""

load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@rules_proto//proto:defs.bzl", "proto_library")

cc_binary(
    name = "mt_main",
    srcs = ["mt_main.cc"],
    deps = [
        ":matching_result_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings",
        "@opencv",
    ],
)

cc_proto_library(
    name = "matching_result_cc_proto",
    deps = [":matching_result_proto"],
)

proto_library(
    name = "matching_result_proto",
    srcs = ["matching_result.proto"],
)
