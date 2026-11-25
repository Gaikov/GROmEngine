cmake_minimum_required(VERSION 3.10)
project(Platform)

if (WEB_ASM)
    set(env-sources
            env/web/WebEnv.cpp
            env/web/WebGamepads.cpp
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL Windows)
    find_package(glfw3 REQUIRED)
    set(env-sources
            env/desktop/DesktopEnv.cpp
            env/desktop/DesktopEnvWin.cpp
            input/GlfwGamepads.cpp
    )
else ()
    find_package(glfw3 REQUIRED)
    set(env-sources
            env/desktop/DesktopEnv.cpp
    )
    if (APPLE)
        list(APPEND env-sources
                env/desktop/DesktopEnvApple.mm
                env/desktop/DesktopFrameApple.mm
                input/apple/AppleGamepad.mm
                input/apple/AppleGamepads.mm
        )
        set_source_files_properties(input/apple/AppleGamepad.mm input/apple/AppleGamepads.mm
                PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
    else ()
        list(APPEND env-sources
                env/desktop/DesktopEnvLinux.cpp
                input/GlfwGamepads.cpp
        )
    endif ()
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
        RasterCommon
)

if (WEB_ASM)
    set(WEB_INTERNAL_STORAGE_JS
            "${CMAKE_CURRENT_SOURCE_DIR}/env/web/WebInternalStorage.js"
    )

    target_link_options(Desktop INTERFACE
            -lidbfs.js
            "SHELL:--pre-js ${WEB_INTERNAL_STORAGE_JS}"
    )

    set_property(TARGET Desktop APPEND PROPERTY
            INTERFACE_LINK_DEPENDS "${WEB_INTERNAL_STORAGE_JS}"
    )
endif ()

if (APPLE AND NOT WEB_ASM)
    find_library(APPKIT_FRAMEWORK AppKit)
    find_library(GAMECONTROLLER_FRAMEWORK GameController)
    find_library(IOKIT_FRAMEWORK IOKit)
    target_link_libraries(Desktop ${APPKIT_FRAMEWORK} ${GAMECONTROLLER_FRAMEWORK} ${IOKIT_FRAMEWORK})
endif ()
