cmake_minimum_required(VERSION 3.23.0-rc3)

include(bsp/nucleo-l476rg/toolchain.cmake)

project(Smartwatch CXX C ASM)

add_subdirectory(bsp/bsp_interface)
add_subdirectory(bsp/nucleo-l476rg)

set(CMAKE_EXECUTABLE_SUFFIX .elf)

#include(bsp/nucleo-l476rg/toolchain.cmake)

#set(CMAKE_SYSTEM_PROCESSOR arm)
#set(CPU_COMPILE_FLAGS "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections  -Wl,-T${CMAKE_CURRENT_LIST_DIR}/cubeMX/STM32L476RGTx_FLASH.ld -specs=nosys.specs")

set(tools "C:/gcc_arm_none_eabi_10_2020_q4_major")
set(CMAKE_C_COMPILER ${tools}/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER ${tools}/bin/arm-none-eabi-gcc)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Target is 64 bits")
endif()

if(CMAKE_SYSTEM_PROCESSOR EQUAL arm)
    message(STATUS "Target is arm")
endif()

add_executable(Smartwatch_app main.cpp)

#add_subdirectory(bsp/bsp_interface)
#add_subdirectory(bsp/nucleo-l476rg)
target_link_libraries(Smartwatch_app bsp)
