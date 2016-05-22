
cmake_minimum_required (VERSION 3.1)

include(FindPkgConfig)


pkg_check_modules (FFMPEG REQUIRED libavcodec libavformat libavutil libavdevice libswresample)

include_directories(
../../
) 

add_library(ffmpegplugin SHARED ffmpegplugin.cpp ffmpegread.cpp ffmpegwrite.cpp ffmpegpluginbase.cpp)
target_link_libraries (ffmpegplugin sgbase ${FFMPEG_LIBRARIES})

add_executable(ffmpegmain ffmpegmain.cpp)
target_link_libraries (ffmpegmain ffmpegplugin sgbase)


add_executable(ffmpeglistdevices ffmpeglistdevices.cpp)
target_link_libraries (ffmpeglistdevices ffmpegplugin sgbase)


install(TARGETS ffmpegplugin ffmpegmain ffmpeglistdevices DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins/ffmpegplugin)
install(FILES ffmpeg.config DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins/ffmpegplugin)
