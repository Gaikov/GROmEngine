cmake_minimum_required(VERSION 3.10)
project(ImageDecoder)

file(GLOB sources
        "*.cpp"
)

include_directories(
        ./
)

add_subdirectory(png)
add_subdirectory(zlib)
add_subdirectory(jpeg)

add_library(ImageDecoder STATIC ${sources})
target_link_libraries(ImageDecoder myJpeg libpng zlib)
