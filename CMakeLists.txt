cmake_minimum_required(VERSION 3.23.0-rc3)

# for some reason CMake chooses the wrong tools if toolchain is included later
include(bsp/nucleo-l476rg/toolchain.cmake)

project(Smartwatch CXX C ASM)

add_subdirectory(bsp/bsp_interface)
add_subdirectory(bsp/nucleo-l476rg)

set(CMAKE_EXECUTABLE_SUFFIX .elf)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Target is 64 bits")
endif()

if(CMAKE_SYSTEM_PROCESSOR EQUAL arm)
    message(STATUS "Target is arm")
endif()

add_executable(Smartwatch_app main.cpp)

target_link_libraries(Smartwatch_app bsp)
