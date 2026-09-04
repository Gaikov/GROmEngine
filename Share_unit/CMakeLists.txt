cmake_minimum_required(VERSION 3.10)
project(Share_unit)

find_package(GTest REQUIRED)
enable_testing()

add_executable(Share_unit ShareServiceTest.cpp)
target_link_libraries(Share_unit Share GTest::gtest GTest::gtest_main)
add_test(NAME Share_unit COMMAND Share_unit)
