
#ifndef FFMPEGVIDEOWRITE_H
#define FFMPEGVIDEOWRITE_H


#include "ffmpegpluginbase.h"



namespace sgraph{

class ffmpegwrite : public sgactor{
protected:
	std::string outsink;
	std::string outformat;
	AVPacket packet;
	AVFormatContext *form_context=0;
	int got_frame=0;
	int video_stream_index=-1;
	int audio_stream_index=-1;
public:
	ffmpegwrite(double freq, int64_t blocking, int32_t parallelize, std::string outsink, std::string outformat="");
	~ffmpegwrite();
	void enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out);
	void run(const std::vector<std::shared_ptr<sgstream>> in);
	void leave();


};

}
#endif


