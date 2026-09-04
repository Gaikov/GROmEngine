cmake_minimum_required(VERSION 3.10)
project(Share)

set(sources
        ShareBackend.h
        ShareService.cpp
        ShareService.h
)

if (CMAKE_SYSTEM_NAME STREQUAL Android)
    list(APPEND sources backends/AndroidShareBackend.cpp)
elseif (WEB_ASM)
    list(APPEND sources backends/WebShareBackend.cpp)
elseif (APPLE)
    list(APPEND sources backends/AppleShareBackend.mm)
elseif (WIN32)
    list(APPEND sources backends/WindowsShareBackend.cpp)
else ()
    list(APPEND sources backends/DesktopClipboardShareBackend.cpp)
endif ()

add_library(Share STATIC ${sources})
target_include_directories(Share PUBLIC ..)
target_link_libraries(Share PUBLIC nsLib)

if (CMAKE_SYSTEM_NAME STREQUAL Android)
    target_link_libraries(Share PRIVATE AndroidPlatform)
elseif (NOT WEB_ASM)
    find_package(glfw3 REQUIRED)
    target_link_libraries(Share PRIVATE glfw)
endif ()

if (APPLE)
    find_library(APPKIT_FRAMEWORK AppKit)
    target_link_libraries(Share PRIVATE ${APPKIT_FRAMEWORK})
elseif (WIN32 AND MSVC)
    target_link_libraries(Share PRIVATE windowsapp)
endif ()
