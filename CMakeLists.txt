#######################################
# SVoIP project
# Authors:
# 	Felix Kletti, Thomas Picariello
#######################################
cmake_minimum_required(VERSION 2.8.8)

set(PROJECT "SVoIP")
project(${PROJECT})

set(CMAKE_BUILD_TYPE Release)

add_subdirectory(libs)
add_subdirectory(src)
