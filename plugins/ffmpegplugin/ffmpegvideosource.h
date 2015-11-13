
#ifndef FFMPEGVIDEOSOURCE_H
#define FFMPEGVIDEOSOURCE_H


//#include <libavutil/opt.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
 
//#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
//#include <libavutil/samplefmt.h>

#include "base.h"
#include "basespecs.h"




namespace sgraph{
class ffmpegi_stream : public sgstream{
public:
	ffmpegi_stream();

};

class spec_ffmpegi : public spec_image{
public:
	spec_ffmpegi(AVInputFormat *input_device_format);

};

class ffmpegvideosource : public sgactor{
protected:
	std::string devicename;
	AVInputFormat *input_device_format=0;
	AVFormatContext context;
	AVPacket packet;
	int got_frame;
public:
	ffmpegvideosource(double freq, int64_t blocking, std::string device="");
	void enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out);
	void run(const std::vector<std::shared_ptr<sgstream>> in);
	void leave();


};

}
#endif


