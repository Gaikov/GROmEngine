cmake_minimum_required(VERSION 3.10)
project(Analytics_unit)

find_package(GTest REQUIRED)
enable_testing()

add_executable(Analytics_unit AnalyticsServiceTest.cpp)
target_link_libraries(Analytics_unit
        Analytics
        Http
        Core
        nsLib
        GTest::gtest
        GTest::gtest_main
)

add_test(NAME Analytics_unit COMMAND Analytics_unit)
