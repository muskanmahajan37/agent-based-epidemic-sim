package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "zstdlib",
    srcs = glob([
        "common/*.c",
        "common/*.h",
        "compress/*.c",
        "compress/*.h",
        "decompress/*.c",
        "decompress/*.h",
    ]),
    hdrs = ["zstd.h"],
)
