cc_library(
    name = "DataNormalization",
    srcs = ["DataNormalization.cpp"],
    hdrs = ["DataNormalization.h"],
    visibility = ["//visibility:public"],
    copts = ["-std=c++20"],
    deps = [
        "@simdjson//:simdjson",
        "//Types:ExchangeBookTypes",
        "//Types:SharedTypes",
        "//Types:TradeTypes",
    ],
)

cc_library(
    name = "FeedProcessing",
    srcs = ["FeedProcessing.cpp"],
    hdrs = ["FeedProcessing.h"],
    visibility = ["//visibility:public"],
    copts = ["-std=c++20"],
    deps = [
        ":DataNormalization",
        "//Types:ExchangeBookTypes",
        "//Types:SharedTypes",
        "//Types:TradeTypes",
        "//IPCConnection:IPCReceiver",
        "//IPCConnection:IPCSender",
        "//Orderbook:Orderbook",
    ],
)

cc_test(
    name = "DataNormalizationTest",
    srcs = ["DataNormalizationTest.cpp"],
    deps = [
        ":DataNormalization",
        "@googletest//:gtest_main",
    ],
    copts = ["-std=c++20"],
)

cc_test(
    name = "FeedProcessingTest",
    srcs = ["FeedProcessingTest.cpp"],
    deps = [
        ":FeedProcessing",
        "//IPCConnection:IPCSender",
        "//IPCConnection:IPCReceiver", 
        "//Orderbook:Orderbook",
        "@googletest//:gtest_main",
    ],
    copts = ["-std=c++20"],
)
