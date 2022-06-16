cmake_minimum_required(VERSION 3.23.0-rc3)

#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_library(tests
#    pulse_detector_test.cpp
#)

add_executable(tests pulse_detector_test.cpp pulse_detector.c)
