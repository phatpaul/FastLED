
# Copyright 2026 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.22.0)

cmake_path(APPEND conf_file_path ${CMAKE_CURRENT_LIST_DIR} ${board} prj.conf)
set(CONF_FILE ${conf_file_path} ${CONF_FILE})
include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

project(lpc845breakout_hello_world LANGUAGES C CXX ASM PROJECT_BOARD_PORT_PATH ${board_root}/${board}/demo_apps/hello_world)

include(${SdkRootDirPath}/CMakeLists.txt)


include(${board}/board_files.cmake)

# Point at the FastLED checkout by its REAL path rather than a local `FastLED`
# symlink. The symlink (-> /home/paul/repos/FastLED/src) made the compiler bake
# the symlink path into DWARF while VS Code canonicalized it to the real path,
# so editor breakpoints and GDB's symbols disagreed and breakpoints mis-bound.
# Using the real path directly keeps both sides on one path.
set(FASTLED_ROOT /home/paul/repos/FastLED/src)

# FastLED is NOT header-only: its implementation lives in the unity-build
# translation units under ${FASTLED_ROOT}/fl/build/*.cpp (this is what upstream
# "fbuild" compiles). A clockless NEOPIXEL blinky only needs the core + STL +
# system + platform units; the rest (audio/fx/gfx/codec/net/...) are pure flash
# bloat on this 64KB part and drag in unrelated -Werror nits. -ffunction-sections
# + -Wl,--gc-sections strips anything unused from these units.
# mcux_add_source() always prepends a base dir to each item (BASE_PATH, or
# CMAKE_CURRENT_LIST_DIR by default), so it can't take absolute paths -- they get
# concatenated onto the project dir. List these relative to FASTLED_ROOT and pass
# it as BASE_PATH below.
set(FASTLED_SOURCES
    fl/build/src.cpp          # FastLED core: CFastLED, CLEDController, CRGB, lib8tion
    fl/build/fl.stl+.cpp      # fl::memcpy, fl::micros (chrono), bit_cast support
    fl/build/fl.system+.cpp   # fl::delayMicroseconds and friends
    fl/build/platforms+.cpp   # LPC845 platform: SysTick time, IO, clockless driver
)

# FastLED's clockless driver force-compiles showLedData() at O2 via
# `#pragma GCC optimize("O2")` (FL_BEGIN_OPTIMIZE_FOR_EXACT_TIMING), even when the
# rest of the project is built at -O0. That makes the function unsteppable in GDB:
# breakpoints inside the bit-bang loop land on unrelated lines and always_inline
# helpers have no addressable line entries. Setting FL_TIMING_OPT_LEVEL=0 turns
# that pragma into O0 so the driver matches the -O0 debug build and is debuggable.
# Debug-only: O0 wrecks the WS2812 signal timing, so release builds keep the O2 default.
# mcux_add_macro(CX FL_TIMING_OPT_LEVEL=0 TARGETS debug)

# add -DFASTLED_M0_RAMFUNC
mcux_add_macro(CX FASTLED_M0_RAMFUNC)


mcux_add_source(
    SOURCES blinky-ws2811.cpp
)
# FastLED sources live outside the project dir; BASE_PATH makes mcux_add_source
# resolve them against the real FastLED checkout (and bake that real path into DWARF).
mcux_add_source(
    BASE_PATH ${FASTLED_ROOT}
    SOURCES ${FASTLED_SOURCES}
)
mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)
mcux_add_config_mex_path( PATH ${board} )
mcux_add_include(INCLUDES .)
# Like mcux_add_source, mcux_add_include prepends a base dir to each item, so an
# absolute INCLUDES path gets concatenated onto the project dir. Use BASE_PATH +
# "." so it resolves to the real FastLED checkout (where FastLED.h lives).
mcux_add_include(BASE_PATH ${FASTLED_ROOT} INCLUDES .)
