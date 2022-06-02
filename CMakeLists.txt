cmake_minimum_required(VERSION 3.23.0-rc3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf")

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

add_subdirectory(bsp/bsp_interface)
add_subdirectory(sensors/sensor_interface)
add_subdirectory(sensors/max30100)

add_executable(Smartwatch_app main.cpp)
target_link_libraries(Smartwatch_app bsp max30100)
