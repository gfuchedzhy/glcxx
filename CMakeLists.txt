cmake_minimum_required (VERSION 3.3.0)

# search for glm
if(NOT GLM_ROOT_DIR AND DEFINED ENV{GLM_ROOT_DIR})
  set(GLM_ROOT_DIR $ENV{GLM_ROOT_DIR})
endif()
find_path(GLM_INCLUDE_DIR "glm/glm.hpp" PATHS ${GLM_ROOT_DIR} NO_DEFAULT_PATH)
find_path(GLM_INCLUDE_DIR "glm/glm.hpp")
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLM DEFAULT_MSG GLM_INCLUDE_DIR)

# build library
project(glcxx CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
add_definitions(-DGL_GLEXT_PROTOTYPES)
include_directories(include)
file(GLOB SRC src/*.cpp)
add_library(glcxx ${SRC})
install(TARGETS glcxx DESTINATION lib)
install(DIRECTORY include/glcxx DESTINATION include)
