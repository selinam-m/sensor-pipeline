cmake_minimum_required(VERSION 3.16)
project(sensor_pipeline_host CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)
add_executable(host src/host.cpp src/SerialReader.cpp src/FrameParser.cpp)