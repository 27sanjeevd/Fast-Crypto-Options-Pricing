cc_library(
    name = "simdjson",
    srcs = ["simdjson_impl.cpp"],
    hdrs = ["singleheader/simdjson.h"],
    includes = ["singleheader"],
    copts = [
        "-std=c++17",
        "-O3",
        "-DSIMDJSON_DISABLE_DEPRECATED_API",
        "-DSIMDJSON_IMPLEMENTATION_HASWELL=1",
        "-DSIMDJSON_IMPLEMENTATION_WESTMERE=1", 
        "-DSIMDJSON_IMPLEMENTATION_FALLBACK=1",
    ],
    visibility = ["//visibility:public"],
)
