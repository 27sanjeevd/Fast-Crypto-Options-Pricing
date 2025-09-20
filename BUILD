cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        "//Orderbook:Orderbook",
        "//DataProcessing:DataProcessing",
        "//ExchangeConnectivity:ExchangeConnectivity",
        "//Pricing:Pricing",
        "@boost//:system",
        "@boost//:thread",
        "@boost//:asio",
        "@boost//:json",
        "@openssl//:openssl",
        "@curl//:curl",
    ],
    copts = [
        "-O2",
        "-std=c++20",
        "-pthread",
    ],
)
