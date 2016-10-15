cmake_minimum_required (VERSION 2.8.11)
project (csgtools)

set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include_directories ("${PROJECT_SOURCE_DIR}/src")
include_directories ("${PROJECT_SOURCE_DIR}/include")

add_definitions (-std=c++11)

add_subdirectory ("${PROJECT_SOURCE_DIR}/src/json11")

# csgparser lib
set (csgparser_SRCS src/csgparser.cpp src/csgparser.hpp)
add_library (csgparser STATIC ${csgparser_SRCS})
target_link_libraries (csgparser json11)

# csg2json exe
add_executable (csg2json src/csg2json.cpp)
target_link_libraries (csg2json csgparser)

# TODO: make graphic stuff optional
find_package (OpenGL)

if (WIN32)
  set (GLFW_LOCATION "${PROJECT_SOURCE_DIR}/libs/glfw")
  find_package (GLFW)
else (WIN32)
  find_package (PkgConfig REQUIRED)
  pkg_search_module (GLFW REQUIRED glfw3)
endif (WIN32)

include_directories (${OPENGL_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS})

add_subdirectory ("${PROJECT_SOURCE_DIR}/src/imgui")

include_directories ("${PROJECT_SOURCE_DIR}/src/stdgl")
include_directories ("${PROJECT_SOURCE_DIR}/src/csgframework")

add_subdirectory ("${PROJECT_SOURCE_DIR}/src/stdgl")
add_subdirectory ("${PROJECT_SOURCE_DIR}/src/csgframework")

# csgviewer exe
add_executable (csgviewer src/csgviewer.cpp)
target_link_libraries (csgviewer csgparser imgui stdgl csgframework ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})

