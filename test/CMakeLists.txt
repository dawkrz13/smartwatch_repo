cmake_minimum_required(VERSION 3.23.0-rc3)

project(smartwatch_tests)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY  https://github.com/google/googletest.git
    GIT_TAG         release-1.11.0
)

FetchContent_Declare(
    max30100_test_lib
    GIT_REPOSITORY  https://github.com/dawkrz13/MAX30100_for_STM32_HAL.git
    GIT_TAG         origin/test
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest max30100_test_lib)

add_library(max30100_test
    ${max30100_test_lib_SOURCE_DIR}/pulse_detector.c
)
target_include_directories(max30100_test PUBLIC
    ${max30100_test_lib_SOURCE_DIR}
)

enable_testing()

add_executable(
  smartwatch_tests
  pulse_detector_test.cpp
)

target_link_libraries(
  smartwatch_tests
  gtest_main
  gmock_main
  max30100_test
)

include(GoogleTest)
gtest_discover_tests(smartwatch_tests)
