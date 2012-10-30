project(libmxp)

cmake_minimum_required(VERSION 2.3)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} )

SET(CMAKE_CXX_FLAGS "-Wall -O2")

add_subdirectory( src )

