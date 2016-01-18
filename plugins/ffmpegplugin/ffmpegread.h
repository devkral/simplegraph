
#ifndef FFMPEGVIDEOREAD_H
#define FFMPEGVIDEOREAD_H

#include <cstring>
#include "ffmpegpluginbase.h"




namespace sgraph{

class ffmpegread : public sgactor{
protected:
	std::string sourcepath,sourceprovider;
	//AVInputFormat *input_device_format=0;
	ffmpeg::AVPacket packet;
	ffmpeg::AVFormatContext *form_context=0;
	int got_frame=0;
	int video_stream_index=-1;
	int audio_stream_index=-1;
public:
	ffmpegread(double freq, int64_t blocking, std::string sourcepath, std::string sourceprovider="");
	~ffmpegread();
	void enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out);
	void run(const std::vector<std::shared_ptr<sgstream>> );
	void leave();


};

}
#endif


