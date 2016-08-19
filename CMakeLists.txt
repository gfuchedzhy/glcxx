cmake_minimum_required(VERSION 3.3)
project(glcxx CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)

# search for glm
if(NOT GLM_ROOT_DIR AND DEFINED ENV{GLM_ROOT_DIR})
  set(GLM_ROOT_DIR $ENV{GLM_ROOT_DIR})
endif()
find_path(GLM_INCLUDE_DIR "glm/glm.hpp" PATHS ${GLM_ROOT_DIR} NO_DEFAULT_PATH)
find_path(GLM_INCLUDE_DIR "glm/glm.hpp")
include(FindPackageHandleStandardArgs)
set(GLM_FIND_REQUIRED on)
find_package_handle_standard_args(GLM DEFAULT_MSG GLM_INCLUDE_DIR)

# add opengl include definition, client should provide own definition depending
# on platform gl loader, if not this fallback will work on linux
if(NOT GLCXX_GL_INLCUDE_DEF)
  set(GLCXX_GL_INLCUDE_DEF "#define GL_GLEXT_PROTOTYPES\n#include <GL/gl.h>")
endif()
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/glcxx/gl.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/include/glcxx/gl.hpp)

set(GLCXX_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(${GLCXX_INCLUDE_DIRS})
file(GLOB SRC src/*.cpp)
add_library(glcxx ${SRC})
install(TARGETS glcxx DESTINATION lib)
install(DIRECTORY ${GLCXX_INCLUDE_DIRS} DESTINATION . FILES_MATCHING PATTERN "*.hpp")

# export variables and functions
set(GLCXX_INCLUDE_DIRS ${GLCXX_INCLUDE_DIRS} PARENT_SCOPE)
set(GLCXX_LIBRARIES glcxx PARENT_SCOPE)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/glcxx/cmake PARENT_SCOPE)
