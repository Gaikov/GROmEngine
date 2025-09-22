cmake_minimum_required(VERSION 3.10)
project(GROmEngine)

macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach ()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

set(CMAKE_CXX_STANDARD 20)
message(OSName: ${CMAKE_SYSTEM_NAME})

if (CMAKE_SYSTEM_NAME STREQUAL Android)
    add_compile_definitions(MOBILE)
endif ()

if (NOT CMAKE_SYSTEM_NAME STREQUAL Android)
    set(PLATFORM Desktop)
    add_subdirectory(Platform/Desktop)
else ()
    set(PLATFORM AndroidPlatform)
    add_subdirectory(Platform/AndroidPlatform)
endif ()

set(SOUND_LIB SoundOAL)

set(ENGINE_LIBS
        ${PLATFORM}
        Engine
        RasterGL
        ImageDecoder
        ${SOUND_LIB}
        SoundBase
        Core
        nsLib
)

set(GROM_ENGINE_LIBS ${ENGINE_LIBS} PARENT_SCOPE)

add_subdirectory(ThirdParty/OggVorbis)

include_directories(
        ./
        Platform
        ThirdParty/OggVorbis/include
)

if ((NOT WEB_ASM) AND (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux"))
    include_directories(
        ThirdParty/openal-soft/include
    )
    add_subdirectory(ThirdParty/openal-soft)
endif()

add_subdirectory(nsLib)
add_subdirectory(Core)
add_subdirectory(ImageDecoder)
add_subdirectory(SoundBase)
add_subdirectory(${SOUND_LIB})
add_subdirectory(RasterGLES3)
add_subdirectory(Engine)
if (NOT NO_NETWORKING)
    add_subdirectory(Networking)
endif()

if (NOT ANDROID AND NOT WEB_ASM) #TODO: disable tests for non Desktop
    add_subdirectory(nsLib_unit)
    add_dependencies(nsLib_unit nsLib)
    add_subdirectory(Core_unit)
    add_dependencies(Core_unit Core)
    add_subdirectory(Tools)
    add_subdirectory(TestsApp)

    add_dependencies(functional_tests Engine)
endif()

add_dependencies(Core nsLib)
add_dependencies(SoundBase Core)
add_dependencies(${SOUND_LIB} SoundBase)
add_dependencies(RasterGL Core)
add_dependencies(ImageDecoder Core)
add_dependencies(Engine Core ${SOUND_LIB} RasterGL ${PLATFORM})

