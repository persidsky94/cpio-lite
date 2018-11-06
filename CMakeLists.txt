cmake_minimum_required(VERSION 2.8)
project(cpio-lite)
add_library(cpio-lite SHARED src/cpio-lite.cpp src/descriptorwrapper.cpp src/util.cpp)
include_directories(include)
