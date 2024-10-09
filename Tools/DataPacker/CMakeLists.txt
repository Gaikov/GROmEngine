cmake_minimum_required(VERSION 3.13)
project(DataPacker)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES
        Args.cpp
        DataPacker.cpp
        StdAfx.cpp
        UnpackStrategy.cpp
        PackStrategy.cpp
        PackStrategy.h
)

include_directories(
        ../../..
        ../..
)

add_executable(DataPacker ${SOURCES})

target_link_libraries(DataPacker nsLib Core)
