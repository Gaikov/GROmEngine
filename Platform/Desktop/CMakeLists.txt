cmake_minimum_required(VERSION 3.10)
project(Platform)

if (NOT WEB_ASM)
find_package(GLFW3 REQUIRED)
endif()

set(sources
        DesktopKeyNames.cpp
        DesktopPlatform.cpp
        DesktopMain.cpp
)

include_directories(.)

add_library(Desktop
        STATIC
        ${sources}
)

target_link_libraries(Desktop
        glfw
)