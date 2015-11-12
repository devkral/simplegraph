cmake_minimum_required (VERSION 3.0)

#project(simplegraph-base)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
#set (distrolinkerflags "")
#IF(CMAKE_SYSTEM_NAME STREQUAL Windows) #Linux OR CMAKE_SYSTEM_NAME STREQUAL Darwin)
#ELSE()

IF(UNIX)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
	set (pluginlinkerflags "-ldl")
	#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -ldl")
ENDIF()


# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
#include_directories("${PROJECT_BINARY_DIR}")

add_library(sgbase SHARED base.cpp basespecs.cpp)

# compile pluginmanager
add_executable(pluginmanager pluginmanager.cpp)
target_link_libraries (pluginmanager sgbase ${pluginlinkerflags})

file (GLOB sublibs "libs/*")
foreach(thislib ${sublibs})
	add_subdirectory(${thislib})
endforeach()

file (GLOB subplugins "plugins/*")
foreach(thisplugin ${subplugins})
	add_subdirectory(${thisplugin})
endforeach()
