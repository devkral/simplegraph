
cmake_minimum_required (VERSION 3.1)

include_directories(
	../../
)


add_library(testplugin SHARED testplugin.cpp)
target_link_libraries (testplugin sgbase)

add_executable(testmain testmain.cpp)
target_link_libraries (testmain testplugin sgbase)

install(TARGETS testplugin testmain DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins/testplugin)
install(FILES test.config test2.config DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins/testplugin)
