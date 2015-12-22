
cmake_minimum_required (VERSION 3.1)

include(FindPkgConfig)


pkg_check_modules (FFMPEG REQUIRED libavcodec libavformat libavutil libavdevice libswresample)

include_directories(
../../
) #${FFMPEG_INCLUDE_DIRS}

#link_libraries(
#	${FFMPEG_LIBRARY_DIRS}
#)

#avcodec avformat avutil avdevice 
# avcodec avformat avutil avdevice
#  why the hell is api so uncomprehendable?
add_library(ffmpegplugin SHARED ffmpegplugin.cpp ffmpegvideoread.cpp ffmpegvideowrite.cpp ffmpegpluginbase.cpp)
target_link_libraries (ffmpegplugin sgbase ${FFMPEG_LIBRARIES})

add_executable(ffmpegmain ffmpegmain.cpp) # ${FFMPEG_LDFLAGS} ${FFMPEG_CFLAGS}
target_link_libraries (ffmpegmain ffmpegplugin sgbase)
