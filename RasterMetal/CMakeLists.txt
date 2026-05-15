cmake_minimum_required(VERSION 3.10)
project(RasterMetal)

message(STATUS "Metal renderer")

find_library(METAL_LIB Metal REQUIRED)
find_library(METALKIT_LIB MetalKit)
find_library(QUARTZCORE_LIB QuartzCore REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE sources "*.cpp" "*.mm" "*.h")

include_directories(
    ./
    /opt/homebrew/include
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc")

add_library(RasterMetal STATIC ${sources})
target_link_libraries(RasterMetal
    ${METAL_LIB}
    ${QUARTZCORE_LIB}
    ${METALKIT_LIB}
    glfw
    "-framework AppKit"
)
