
#ifndef FFMPEGVIDEOWRITE_H
#define FFMPEGVIDEOWRITE_H

extern "C"
{
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}


#include "base.h"
#include "basespecs.h"
#include "ffmpegvideosource.h"



namespace sgraph{

class ffmpegvideowrite : public sgactor{
protected:
	std::string filename;
	std::string outformat;
	AVInputFormat *input_device_format=0;
	AVCodec *codec=0;
	AVFormatContext *form_context=0;
	AVCodecContext *cod_context=0;
	AVPacket packet;
	AVFrame *frame=0;
	int got_frame;
public:
	ffmpegvideowrite(double freq, int64_t blocking, std::string format, std::string out="a.out");
	~ffmpegvideowrite();
	void enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out);
	void run(const std::vector<std::shared_ptr<sgstream>> in);
	void leave();


};

}
#endif


