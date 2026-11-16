cmake_minimum_required(VERSION 3.10)
project(Core_unit)

find_package(GTest REQUIRED)
enable_testing()

include_directories(./)

set(TESTS
        parser/ParserUtils.cpp
        main.cpp
        args/ArgsTest.cpp
        file/FilePathTest.cpp
        CryptTest.cpp
        buffer/StringWriterTest.cpp
        serialization/SerializeUtilsTest.cpp
        undo/UndoTest.cpp
        memory/FastMem.cpp
        memory/MemoryTest.cpp
        input/GamepadRegistryTest.cpp)

add_executable(Core_unit ${TESTS})

if (APPLE AND NOT WEB_ASM)
    find_package(glfw3 REQUIRED)
    set(APPLE_GAMEPAD_SOURCES
            input/AppleGamepadTest.mm
            ../Platform/Desktop/input/apple/AppleGamepad.mm
            ../Platform/Desktop/input/apple/AppleGamepads.mm)
    set_source_files_properties(${APPLE_GAMEPAD_SOURCES} PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
    target_sources(Core_unit PRIVATE ${APPLE_GAMEPAD_SOURCES})
    target_link_libraries(Core_unit glfw ${APPKIT_FRAMEWORK} ${GAMECONTROLLER_FRAMEWORK} ${IOKIT_FRAMEWORK})
endif ()

target_link_libraries(Core_unit
        Core
        nsLib
        GTest::gtest
        GTest::gtest_main
)

add_test(NAME Core_unit COMMAND Core_unit)
