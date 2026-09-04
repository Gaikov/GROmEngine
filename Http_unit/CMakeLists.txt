cmake_minimum_required(VERSION 3.10)
project(Http_unit)

find_package(GTest REQUIRED)
enable_testing()

add_executable(Http_unit HttpClientTest.cpp)
target_link_libraries(Http_unit Http GTest::gtest GTest::gtest_main)
add_test(NAME Http_unit COMMAND Http_unit)
