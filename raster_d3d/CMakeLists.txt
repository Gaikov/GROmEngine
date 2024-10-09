cmake_minimum_required(VERSION 3.10)
project(raster_d3d)

file(GLOB sources
        "*.h"
        "*.cpp"
        )

include_directories(../sdk/dx8/include ../)
link_directories(../sdk/dx8/lib)

add_library(raster_d3d STATIC ${sources})
target_link_libraries(raster_d3d -ld3d8.libwe)