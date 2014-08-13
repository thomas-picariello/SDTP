#######################################
# SVoIP project
# Authors:
# 	Felix Kletti, Thomas Picariello
#######################################
cmake_minimum_required(VERSION 2.8.9)

set(PROJECT "SVoIP")
project(${PROJECT})

include(${PROJECT_SOURCE_DIR}/cmake/macros.cmake)

#add cmake macros
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_BUILD_TYPE Release)

add_definitions(-DQT_NO_DEBUG_OUTPUT)


add_subdirectory(src)
