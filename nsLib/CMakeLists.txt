cmake_minimum_required(VERSION 3.10)
project(nsLib)

file(GLOB_RECURSE sources "*.cpp" "*.c" "*.h")

include_directories(
        ./
)

add_library(nsLib STATIC ${sources})