
cmake_minimum_required (VERSION 3.1)

include_directories(
	../../
)

add_library(testplugin SHARED testplugin.cpp)
target_link_libraries (testplugin sgbase)

add_executable(testmain testmain.cpp)
target_link_libraries (testmain testplugin sgbase)
