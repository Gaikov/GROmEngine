cmake_minimum_required(VERSION 2.4.4...3.15.0)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

project(zlib C)

set(VERSION "1.3.1")

file(GLOB sources
        "*.c"
)

include_directories(
        ./
)

add_library(zlib STATIC ${sources})