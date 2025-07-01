cmake_minimum_required(VERSION 3.10)
project(Platform)

if (WEB_ASM)
    set(env-sources
            env/web/WebEnv.cpp
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL WIN32)
    find_package(glfw3 REQUIRED)
    set(env-sources
            env/desktop/DesktopEnv.cpp
            env/desktop/DesktopEnvWin.cpp
    )
else ()
    find_package(glfw3 REQUIRED)
    set(env-sources
            env/desktop/DesktopEnv.cpp
            env/desktop/DesktopEnvLinux.cpp
    )
endif ()

set(sources
        DesktopKeyNames.cpp
        DesktopPlatform.cpp
        DesktopMain.cpp
        env/Env.cpp
        env/Env.h
        DesktopCommon.h
)

include_directories(.)

add_library(Desktop
        STATIC
        ${sources}
        ${env-sources}
)

target_link_libraries(Desktop
        glfw
)