cmake_minimum_required(VERSION 3.10)
project(Core_unit)

find_package(GTest REQUIRED)
enable_testing()

include_directories(./)

set(TESTS
        parser/ParserUtils.cpp
        main.cpp
        args/ArgsTest.cpp
        CryptTest.cpp
        undo/UndoTest.cpp
        memory/FastMem.cpp)

add_executable(Core_unit ${TESTS})

target_link_libraries(Core_unit
        Core
        nsLib
        gtest
        gtest_main
)