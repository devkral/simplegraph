
#ifndef FFMPEGPLUGINBASE_H
#define FFMPEGPLUGINBASE_H


extern "C"
{
//#include <libavutil/opt.h>
//#include <libavutil/avutil.h>

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

class stream_ffmpeg_packet : public sgstream{
public:
	AVPacket packet;

	stream_ffmpeg_packet(AVPacket *packet);
	~stream_ffmpeg_packet();

};

class spec_ffmpeg_packet : public sgstreamspec{
public:
	AVCodecContext *audiocontext=0, *videocontext=0;
	AVCodec *audiocodec=0, *videocodec=0;
	bool encode=false;
	spec_ffmpeg_packet(bool encode, AVCodecContext *videocontext, AVCodecContext *audiocontext);
	~spec_ffmpeg_packet();
};

class ffmpegi_stream : public sgstream{
public:
	ffmpegi_stream();

};



/*class ffmpegi_stream : public sgstream{
public:
	ffmpegi_stream();

};*/


class spec_ffmpeg_image : public spec_image{
public:
	spec_ffmpeg_image(AVCodecContext *cod_context, uint8_t channels);

};

}

#endif
