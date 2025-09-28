cc_library(
    name = "ExchangeBookTypes",
    hdrs = ["ExchangeBookTypes.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "TradeTypes",
    hdrs = ["TradeTypes.h"],
    visibility = ["//visibility:public"],
    deps = [":SharedTypes"],
)

cc_library(
    name = "SharedTypes",
    srcs = ["SharedTypes.cpp"],
    hdrs = ["SharedTypes.h"],
    visibility = ["//visibility:public"],
    deps = [":ExchangeBookTypes"],
)