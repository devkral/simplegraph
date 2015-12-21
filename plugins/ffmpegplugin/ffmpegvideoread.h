
#ifndef FFMPEGVIDEOREAD_H
#define FFMPEGVIDEOREAD_H

#include "ffmpegpluginbase.h"




namespace sgraph{

class ffmpegvideoread : public sgactor{
protected:
	std::string filename;
	//AVInputFormat *input_device_format=0;
	AVCodec *codec=0;
	AVFormatContext *form_context=0;
	AVCodecContext *cod_context=0;
	AVPacket packet;
	AVFrame *frame=0;
	int got_frame;
	int video_stream_index=-1;
public:
	ffmpegvideoread(double freq, int64_t blocking, std::string filename);
	~ffmpegvideoread();
	void enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out);
	void run(const std::vector<std::shared_ptr<sgstream>> in);
	void leave();


};

}
#endif


