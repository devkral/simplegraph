
cmake_minimum_required (VERSION 3.0)

include_directories(
	../../
)
add_library(ffmpegplugin SHARED ffmpegplugin.cpp ffmpegvideosource.cpp)
target_link_libraries (ffmpegplugin sgbase avcodec avformat avutil avdevice)
