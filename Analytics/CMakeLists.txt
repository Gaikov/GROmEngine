cmake_minimum_required(VERSION 3.10)
project(Analytics)

add_library(Analytics STATIC
        AnalyticsEvent.cpp
        AnalyticsEvent.h
        AnalyticsService.cpp
        AnalyticsService.h
)

target_include_directories(Analytics PUBLIC ..)
target_link_libraries(Analytics PUBLIC Core Http)
