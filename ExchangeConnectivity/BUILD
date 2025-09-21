cc_library(
    name = "ExchangeConnectivity",
    srcs = ["ExchangeConnectivity.cpp"],
    hdrs = ["ExchangeConnectivity.h"],
    visibility = ["//visibility:public"],
    copts = [
        "-O2",
        "-std=c++20",
        "-pthread",
    ],
    deps = [
        "@boost.system//:boost.system",
        "@boost.thread//:boost.thread", 
        "@boost.asio//:boost.asio",
        "@boost.beast//:boost.beast",
        "@boost.json//:boost.json",
        "@openssl//:openssl",
    ],
)
