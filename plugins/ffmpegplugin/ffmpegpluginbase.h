
#ifndef FFMPEGPLUGINBASE_H
#define FFMPEGPLUGINBASE_H

#include "base.h"
#include "basespecs.h"

namespace ffmpeg
{
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
}

namespace sgraph{

class stream_ffmpeg_packet : public sgstream{
public:
	ffmpeg::AVPacket packet;

	stream_ffmpeg_packet(ffmpeg::AVPacket *packet);
	~stream_ffmpeg_packet();

};

class spec_ffmpeg_packet : public sgstreamspec{
public:
	ffmpeg::AVCodecContext *audiocontext=0, *videocontext=0;
	ffmpeg::AVCodec *audiocodec=0, *videocodec=0;
	bool encode=false;
	spec_ffmpeg_packet(bool encode, ffmpeg::AVCodecContext *videocontext, ffmpeg::AVCodecContext *audiocontext);
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
	spec_ffmpeg_image(ffmpeg::AVCodecContext *cod_context, uint8_t channels);

};

}

#endif
