cmake_minimum_required(VERSION 3.22)
project(mm)

set(CMAKE_CXX_STANDARD 17)

add_executable(mm testing.cpp src/mm.hpp src/allocator_interface.h src/allocator_stdlib.h)
