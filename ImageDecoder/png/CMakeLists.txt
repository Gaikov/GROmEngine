
# CMakeLists.txt - CMake lists for libpng
#
# Copyright (c) 2018-2024 Cosmin Truta.
# Copyright (c) 2007-2018 Glenn Randers-Pehrson.
# Originally written by Christian Ehrlicher, 2007.
#
# Use, modification and distribution are subject to
# the same licensing terms and conditions as libpng.
# Please see the copyright notice in png.h or visit
# http://libpng.org/pub/png/src/libpng-LICENSE.txt
#
# For copyright and licensing purposes, please see
# the accompanying file scripts/cmake/AUTHORS.md
#
# SPDX-License-Identifier: libpng-2.0

cmake_minimum_required(VERSION 3.6)

set(PNGLIB_MAJOR 1)
set(PNGLIB_MINOR 6)
set(PNGLIB_REVISION 43)
set(PNGLIB_SUBREVISION 0)
set(PNGLIB_VERSION ${PNGLIB_MAJOR}.${PNGLIB_MINOR}.${PNGLIB_REVISION})
set(PNGLIB_ABI_VERSION ${PNGLIB_MAJOR}${PNGLIB_MINOR})
set(PNGLIB_SHARED_VERSION ${PNGLIB_ABI_VERSION}.${PNGLIB_REVISION}.${PNGLIB_SUBREVISION})

project(libpng
        VERSION ${PNGLIB_VERSION}
        LANGUAGES C ASM)

file(GLOB sources
        "*.c"
        "arm/*.c"
        "*.S"
)

include_directories(
        ./
)

add_library(libpng STATIC ${sources})

