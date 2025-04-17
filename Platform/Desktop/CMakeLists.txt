cmake_minimum_required(VERSION 3.10)
project(Platform)

if (WEB_ASM)
    set(env-sources
            env/web/WebLog.cpp
            env/web/WebLog.h
            env/web/WebEnv.cpp
            env/web/WebEnv.h
    )
else ()
    find_package(GLFW3 REQUIRED)
    set(env-sources
            env/desktop/DesktopEnv.cpp
            env/desktop/DesktopEnv.h
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