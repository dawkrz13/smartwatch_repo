# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_TOOLS_PATH}/tools/cmake/project.cmake)
include(./toolchain.cmake)
project(ble-hrm-idf C CXX ASM)
