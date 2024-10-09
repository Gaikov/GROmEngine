cmake_minimum_required(VERSION 3.10)
project(GROmEngine)

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
add_subdirectory(ThirdParty/OggVorbis)

include_directories(
        ./
        Platform
        ThirdParty/OggVorbis/include
        ThirdParty/openal-soft/include
)

add_subdirectory(nsLib)
add_subdirectory(Core)
add_subdirectory(ImageDecoder)
add_subdirectory(SoundBase)
add_subdirectory(ThirdParty/openal-soft)
add_subdirectory(${SOUND_LIB})
add_subdirectory(RasterGL)
add_subdirectory(Engine)

if (NOT ANDROID)
    add_subdirectory(nsLib_unit)
    add_dependencies(nsLib_unit nsLib)
    add_subdirectory(Core_unit)
    add_dependencies(Core_unit Core)
endif()

add_dependencies(Core nsLib)
add_dependencies(SoundBase Core)
add_dependencies(${SOUND_LIB} SoundBase)
add_dependencies(RasterGL Core)
add_dependencies(ImageDecoder Core)
add_dependencies(Engine Core ${SOUND_LIB} RasterGL ${PLATFORM})
