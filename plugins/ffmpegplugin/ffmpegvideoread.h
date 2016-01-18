
#ifndef FFMPEGVIDEOREAD_H
#define FFMPEGVIDEOREAD_H

#include <cstring>
#include "ffmpegpluginbase.h"




namespace sgraph{

class ffmpegvideoread : public sgactor{
protected:
	std::string sourcepath,sourceprovider;
	//AVInputFormat *input_device_format=0;
	ffmpeg::AVCodec *codec=0;
	ffmpeg::AVFormatContext *form_context=0;
	ffmpeg::AVCodecContext *cod_context=0;
	ffmpeg::AVPacket packet;
	ffmpeg::AVFrame *origframe=0, *convertframe=0;
	int got_frame;
	int video_stream_index=-1;
public:
	ffmpegvideoread(double freq, int64_t blocking, std::string sourcepath, std::string sourceprovider="");
	~ffmpegvideoread();
	void enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out);
	void run(const std::vector<std::shared_ptr<sgstream>> );
	void leave();


};

}
#endif


