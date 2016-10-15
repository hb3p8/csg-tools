find_path (GLFW_INCLUDE_DIRS 
  NAMES
    GLFW/glfw3.h
  PATHS
    "${GLFW_LOCATION}/include"
  DOC 
    "The directory where GLFW/glfw3.h resides"
)

if (CMAKE_SIZEOF_VOID_P EQUAL 8) # 64-bit target
  set (GLFW_LIB_LOCATION "${GLFW_LOCATION}/lib-vc2010-64")
else() # 32-bit target
  set (GLFW_LIB_LOCATION "${GLFW_LOCATION}/lib-vc2010-32")
endif()

find_library (GLFW_LIBRARIES
  NAMES 
    glfw3
  PATHS
    "${GLFW_LIB_LOCATION}"
  DOC 
    "The GLFW library"
)
