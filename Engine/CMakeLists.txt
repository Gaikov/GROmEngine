cmake_minimum_required(VERSION 3.10)
project(Engine)

file(GLOB_RECURSE sources "*.cpp")

include_directories(./)

add_library(Engine STATIC ${sources})
target_include_directories(Engine PUBLIC ../Platform)