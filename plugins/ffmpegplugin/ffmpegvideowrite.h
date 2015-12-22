
#ifndef FFMPEGVIDEOWRITE_H
#define FFMPEGVIDEOWRITE_H
#include "ffmpegpluginbase.h"



namespace sgraph{

class ffmpegvideowrite : public sgactor{
protected:
	std::string outsink;
	std::string outformat;
	ffmpeg::AVInputFormat *input_device_format=0;
	ffmpeg::AVCodec *codec=0;
	ffmpeg::AVFormatContext *form_context=0;
	ffmpeg::AVCodecContext *cod_context=0;
	ffmpeg::AVPacket packet;
	ffmpeg::AVFrame *frame=0;
	int got_frame;
public:
	ffmpegvideowrite(double freq, int64_t blocking, std::string outsink, std::string outformat);
	~ffmpegvideowrite();
	void enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out);
	void run(const std::vector<std::shared_ptr<sgstream>> in);
	void leave();


};

}
#endif


