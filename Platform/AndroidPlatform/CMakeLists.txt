cmake_minimum_required(VERSION 3.10)
project(Platform)

set(CMAKE_CXX_STANDARD 17)
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