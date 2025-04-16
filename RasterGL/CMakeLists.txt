cmake_minimum_required(VERSION 3.10)
project(RasterGL)

if (WEB_ASM)
    message(STATUS "No gl linking for webasm")
elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(OPENGL_LIBRARIES GL GLU)
elseif(NOT CMAKE_SYSTEM_NAME STREQUAL Android)
    find_package(OpenGL REQUIRED)
else()
    set(OPENGL_LIBRARIES GLESv1_CM)
endif()

set(sources
        GLDebug.cpp
        GLRenderDevice.cpp
        GLTexture.cpp
        GLTexturesCache.cpp
        GLUtils.cpp
        GLVertexBuffer.cpp
        shaders/GLShader.cpp
        shaders/GLShadersManager.cpp
        GLLightManager.cpp
        GLDisplayModes.cpp
        stencil/GLStencilState.cpp
        stencil/GLStencilState.h
        stencil/GLStencilManager.cpp
        stencil/GLStencilManager.h
)

include_directories(
        ./
)

add_library(RasterGL STATIC ${sources})
target_link_libraries(RasterGL ${OPENGL_LIBRARIES})