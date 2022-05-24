cmake_minimum_required(VERSION 3.23.0-rc3)

add_subdirectory(bsp/bsp_interface)

if(BUILD_TARGET STREQUAL arm)

    message(STATUS "Building for target: arm")

    # include toolchain before CMake chooses it itself...
    include(bsp/nucleo-l476rg/toolchain.cmake)
    # ...what happens after specifying languages
    project(Smartwatch CXX C ASM)
    add_subdirectory(bsp/nucleo-l476rg)
    set(CMAKE_EXECUTABLE_SUFFIX .elf)

elseif(BUILD_TARGET STREQUAL x86)

    message(STATUS "Building for target: x86")

else()

    message(FATAL_ERROR "No target specified")

endif()

add_executable(Smartwatch_app main.cpp)
target_link_libraries(Smartwatch_app bsp)
