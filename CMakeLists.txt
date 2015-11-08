cmake_minimum_required (VERSION 3.0)

set (CMAKE_CXX_FLAGS "-std=c++11")
#IF(CMAKE_SYSTEM_NAME STREQUAL Linux OR CMAKE_SYSTEM_NAME STREQUAL Darwin))
IF(CMAKE_SYSTEM_NAME STREQUAL Windows)
ELSE()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -ldl")
ENDIF()

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
#include_directories("${PROJECT_BINARY_DIR}")

add_library(sgbase base.cpp)

# add the executable
add_executable(pluginmanager pluginmanager.cpp)
target_link_libraries (pluginmanager sgbase)
