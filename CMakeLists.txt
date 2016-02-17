cmake_minimum_required (VERSION 3.1)

project(simplegraph)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

IF(UNIX)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
	set (pluginlinkerflags "-ldl")
	#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -ldl")
ENDIF(UNIX)

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
