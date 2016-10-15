project(stdgl)

cmake_minimum_required(VERSION 2.8)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

set(stdgl_SRCS
  Types.hpp
  Box.cpp
  Box.hpp
  Texture2D.cpp
  Texture2D.hpp
  Texture3D.cpp
  Texture3D.hpp
  TextureBuffer.cpp
  TextureBuffer.hpp
  VoxelData.cpp
  VoxelData.hpp
  ShaderProgram.cpp
  ShaderProgram.hpp
  )

add_library(stdgl STATIC ${stdgl_SRCS})
