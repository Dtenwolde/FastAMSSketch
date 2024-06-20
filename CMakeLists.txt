cmake_minimum_required(VERSION 3.28)
project(FastAMSSketch)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)

add_executable(FastAMSSketch main.cpp FastAMS.cpp)
