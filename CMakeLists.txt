cmake_minimum_required(VERSION 3.15)
project(LCR2)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)
add_executable(LCR2
        src/graph.h
        src/graph.cpp
        src/main.cpp
)