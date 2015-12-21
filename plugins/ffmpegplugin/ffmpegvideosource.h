
#ifndef FFMPEGVIDEOSOURCE_H
#define FFMPEGVIDEOSOURCE_H
#include "ffmpegpluginbase.h"

namespace sgraph{

class ffmpegvideosource : public sgactor{
protected:
	std::string devicename;
	std::string formatname;
	//AVInputFormat *input_device_format=0;
	AVDeviceInfoList *devices=0;
	AVCodec *codec=0;
	AVInputFormat *input_device_format=0;
	AVFormatContext *input_context=0;
	AVCodecContext *cod_context=0;
	AVPacket *packet=0;
	AVFrame *frame=0;
	int got_frame;
public:
	ffmpegvideosource(double freq, int64_t blocking, std::string format, std::string device="");
	~ffmpegvideosource();
	void enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out);
	void run(const std::vector<std::shared_ptr<sgstream>> in);
	void leave();


};

}
#endif


