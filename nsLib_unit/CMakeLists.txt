cmake_minimum_required(VERSION 3.10)
project(nsLib_unit)

find_package(GTest REQUIRED)

enable_testing()
set(TESTS
        example/ExampleTest.cpp
        example/ExampleTest2.cpp
        tests/nsFilePathTest.cpp
        main.cpp
        tests/nsStringTest.cpp
        tests/nsLockTest.cpp
        tests/EventDispatcherTest.cpp
        tests/EventDispatcherTest.cpp
        math/RectTest.cpp
        structs/FastListTest.cpp
        math/Matrix2Text.cpp
)

include_directories(./)

add_executable(nsLib_unit ${TESTS})

target_link_libraries(nsLib_unit
        nsLib
        gtest
        gtest_main
)