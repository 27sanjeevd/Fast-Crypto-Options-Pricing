cc_library(
    name = "IPCReceiver",
    srcs = ["IPCReceiver.cpp"],
    hdrs = ["IPCReceiver.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "IPCSender",
    srcs = ["IPCSender.cpp"],
    hdrs = ["IPCSender.h"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "IPCConnectionTest",
    srcs = ["IPCConnectionTest.cpp"],
    deps = [
        ":IPCReceiver",
        ":IPCSender",
        "@googletest//:gtest_main",
    ],
)
