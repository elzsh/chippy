cmake_minimum_required(VERSION 4.1)
project(chippy)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(chippy
        src/main.cpp
        src/chip8.cpp
)
