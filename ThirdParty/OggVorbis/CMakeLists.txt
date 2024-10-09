cmake_minimum_required(VERSION 3.10)
project(OggVorbis)

include(CheckIncludeFiles)

check_include_files(inttypes.h INCLUDE_INTTYPES_H)
check_include_files(stdint.h INCLUDE_STDINT_H)
check_include_files(sys/types.h INCLUDE_SYS_TYPES_H)

#[[
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(SIZE16 int16_t)
set(USIZE16 uint16_t)
set(SIZE32 int32_t)
set(USIZE32 uint32_t)
set(SIZE64 int64_t)
set(USIZE64 uint64_t)
]]

#include(CheckSizes)

#configure_file(include/ogg/config_types.h.in include/ogg/config_types.h @ONLY)

include_directories(
        include
        lib
        #${CMAKE_CURRENT_BINARY_DIR}/include
)

file(GLOB OGG_SOURCES
        "src/*.c"
        )

set(VORBIS_SOURCES
        lib/mdct.c
        lib/smallft.c
        lib/block.c
        lib/envelope.c
        lib/window.c
        lib/lsp.c
        lib/lpc.c
        lib/analysis.c
        lib/synthesis.c
        lib/psy.c
        lib/info.c
        lib/floor1.c
        lib/floor0.c
        lib/res0.c
        lib/mapping0.c
        lib/registry.c
        lib/codebook.c
        lib/sharedbook.c
        lib/lookup.c
        lib/bitrate.c
        lib/vorbisfile.c
        lib/vorbisenc.c
)


add_library(OggVorbis STATIC ${OGG_SOURCES} ${VORBIS_SOURCES})

