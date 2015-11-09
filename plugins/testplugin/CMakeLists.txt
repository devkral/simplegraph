
cmake_minimum_required (VERSION 3.0)

include_directories(
	../../
)

add_executable(testplugin testplugin.cpp)
target_link_libraries (testplugin sgbase)
