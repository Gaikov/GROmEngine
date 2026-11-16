project(Input_unit)
find_package(GTest REQUIRED)
find_package(glfw3 REQUIRED)
add_executable(Input_unit
        InputTest.cpp
        HeadlessConsole.cpp
        ../Engine/Input.cpp
        ../Engine/console/TextBuffer.cpp
        ../Engine/console/StringBuffer.cpp)
target_link_libraries(Input_unit PRIVATE Core nsLib glfw GTest::gtest GTest::gtest_main)
add_test(NAME Input_unit COMMAND Input_unit)
