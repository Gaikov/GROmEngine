cmake_minimum_required(VERSION 3.10)
project(Platform)

set(CMAKE_CXX_STANDARD 20)
set(sources
        AndroidAssetReader.cpp
        AndroidPlatform.cpp
        AndroidLogPolicy.cpp
        JavaObject.cpp
        AndroidKeyboard.cpp
        GLNativeContext.cpp
)

include_directories(.)

add_library(AndroidPlatform
        STATIC
        ${sources}
)

find_package(games-frame-pacing REQUIRED CONFIG)
target_link_libraries(AndroidPlatform PUBLIC games-frame-pacing::swappy_static)
