
#ifndef FFMPEGVIDEOSOURCE_H
#define FFMPEGVIDEOSOURCE_H

extern "C"
{
//#include <libavutil/opt.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>

//#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
//#include <libavutil/samplefmt.h>
}


#include "base.h"
#include "basespecs.h"




namespace sgraph{
class ffmpegi_stream : public sgstream{
public:
	ffmpegi_stream();

};

class spec_ffmpegi : public spec_image{
public:
	spec_ffmpegi(AVCodecContext *cod_context, uint8_t channels);

};

class ffmpegvideosource : public sgactor{
protected:
	std::string devicename;
	std::string formatname;
	AVInputFormat *input_device_format=0;
	AVCodec *codec=0;
	AVFormatContext *form_context=0;
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


