cmake_minimum_required(VERSION 3.10)
project(RasterGL)

message(STATUS "GLES 3.0:${CMAKE_SYSTEM_NAME}")

if (WEB_ASM)
    message(STATUS "No gl linking for webasm")
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(OPENGL_LIBRARIES GL GLU)
elseif(CMAKE_SYSTEM_NAME STREQUAL Android)
    set(OPENGL_LIBRARIES GLESv1_CM)
else()
    #find_library(GLESv2_LIB GLESv2)
    #set(OPENGL_LIBRARIES GLESv2)
    #message(STATUS "gles windows ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY}")
    find_package(OpenGL)
endif()

file(GLOB_RECURSE sources "*.cpp" "*.c" "*.h")

include_directories(
        ./
)

add_library(RasterGL STATIC ${sources})
target_link_libraries(RasterGL ${OPENGL_LIBRARIES})