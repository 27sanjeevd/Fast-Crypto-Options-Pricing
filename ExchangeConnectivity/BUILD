cc_library(
    name = "ExchangeConnectivity",
    srcs = ["ExchangeConnectivity.cpp"],
    hdrs = ["ExchangeConnectivity.h"],
    deps = [
        "//Types:ExchangeBookTypes",
        "//Types:TradeTypes",
        "//Types:SharedTypes",
        "@boost//:asio",
        "@boost//:asio_ssl",
        "@boost//:beast",
        "@boost//:system",
        "@boringssl//:ssl",
        "@boringssl//:crypto",
    ],
    copts = [
        "-std=c++20",
    ],
    linkopts = [],
    visibility = ["//visibility:public"]
)
