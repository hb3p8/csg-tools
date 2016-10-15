project(csgframework)

cmake_minimum_required(VERSION 2.8)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(csgframework_SRCS
  CsgTree.cpp
  CsgTree.hpp
  CsgLoader.cpp
  CsgLoader.hpp
  )

add_library(csgframework STATIC ${csgframework_SRCS})
target_link_libraries (csgframework csgparser stdgl)