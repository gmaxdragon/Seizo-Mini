cmake_minimum_required(VERSION 3.13)
if(NOT PICO_SDK_PATH AND DEFINED ENV{PICO_SDK_PATH})
 set(PICO_SDK_PATH "$ENV{PICO_SDK_PATH}")
endif()
if(NOT PICO_SDK_PATH OR NOT EXISTS "${PICO_SDK_PATH}/external/pico_sdk_import.cmake")
 message(FATAL_ERROR "Set PICO_SDK_PATH to the installed official Pico SDK. No downloads occur automatically.")
endif()
set(PICO_BOARD waveshare_rp2040_zero CACHE STRING "Exact controller module")
include("${PICO_SDK_PATH}/external/pico_sdk_import.cmake")
project(seizo_r4_locked_bench C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
option(SEIZO_R4_COMMISSIONING_UNLOCKED "Enable explicitly armed, unloaded bench motion after commissioning" OFF)
add_executable(seizo_r4_locked_bench main.cpp)
target_compile_definitions(seizo_r4_locked_bench PRIVATE
 SEIZO_R4_COMMISSIONING_UNLOCKED=$<BOOL:${SEIZO_R4_COMMISSIONING_UNLOCKED}>
 PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=0
 PICO_STDIO_USB_STDOUT_TIMEOUT_US=1000
 PICO_STDIO_USB_ENABLE_RESET_VIA_BAUD_RATE=0
 PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE=0)
target_compile_options(seizo_r4_locked_bench PRIVATE -Wall -Wextra -Werror)
target_link_libraries(seizo_r4_locked_bench pico_stdlib pico_stdio_usb pico_rand hardware_uart hardware_watchdog hardware_sync)
pico_enable_stdio_usb(seizo_r4_locked_bench 1)
pico_enable_stdio_uart(seizo_r4_locked_bench 0)
pico_add_extra_outputs(seizo_r4_locked_bench)
