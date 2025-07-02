cmake_minimum_required(VERSION 3.10)
project(SoundOAL)

if (WEB_ASM)
    set(OAL_LIB "")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(OpenAL REQUIRED)

    if (OpenAL_FOUND)
        message(STATUS "OpenAL found: ${OPENAL_LIBRARY}")
        set(OAL_LIB ${OPENAL_LIBRARY})
    endif ()
else ()
    set(OAL_LIB OpenAL)
endif ()

file(GLOB sources
        "*.cpp"
)

add_library(SoundOAL STATIC ${sources})
target_link_libraries(SoundOAL ${OAL_LIB} SoundBase)

#print_all_variables()