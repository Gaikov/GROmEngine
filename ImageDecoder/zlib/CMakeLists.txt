cmake_minimum_required(VERSION 2.4.4...3.15.0)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

project(zlib C)

set(VERSION "1.3.1")

file(GLOB sources
        adler32.c
        compress.c
        crc32.c
        deflate.c
        inflate.c
        infback.c
        inftrees.c
        inffast.c
        trees.c
        uncompr.c
        zutil.c
)

include_directories(
        ./
)

add_library(zlib STATIC ${sources})