cmake_minimum_required(VERSION 3.15)
project(President_Detection)

set(CMAKE_CXX_STANDARD 17)
#find_library(OpenCV)
find_package(OpenCV)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(President_Detection main.cpp People.cpp People.h)
target_link_libraries(President_Detection ${OpenCV_LIBS})