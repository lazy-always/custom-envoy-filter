load(
    "@envoy//bazel:envoy_build_system.bzl",
    "envoy_cc_binary",
    "envoy_cc_library",
)
load("@envoy_api//bazel:api_build_system.bzl", "api_proto_package")

package(default_visibility = ["//visibility:public"])

envoy_cc_binary(
    name = "envoy",
    repository = "@envoy",
    deps = [
        ":awesome_filter_config",
        "@envoy//source/exe:envoy_main_entry_lib",
    ],
)

api_proto_package()

envoy_cc_library(
    name = "http_filter_lib",
    srcs = ["http_filter.cc"],
    hdrs = ["http_filter.h"],
    repository = "@envoy",
    deps = [
        ":pkg_cc_proto",
        "@envoy//source/exe:envoy_common_lib",
    ],
)

envoy_cc_library(
    name = "http_filter_config",
    srcs = ["http_filter_config.cc"],
    repository = "@envoy",
    deps = [
        ":http_filter_lib",
        "@envoy//include/envoy/server:filter_config_interface",
    ],
)

envoy_cc_library(
    name = "awesome_filter_config",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    repository = "@envoy",
    deps = [
        ":awesome_filter_lib",
        "@envoy//include/envoy/server:filter_config_interface",
    ],
)

envoy_cc_library(
    name = "awesome_filter_lib",
    srcs = [
        "awesome_filter.cc",
        "awesome_filter_impl.cc",
    ],
    hdrs = [
        "awesome.h",
        "awesome_filter.h",
        "awesome_filter_impl.h",
    ],
    repository = "@envoy",
    deps = [
        ":pkg_cc_proto",
        "@envoy//source/exe:envoy_common_lib",
    ],
)
