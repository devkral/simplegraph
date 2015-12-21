
#ifndef FFMPEGPLUGINBASE_H
#define FFMPEGPLUGINBASE_H

#include "base.h"
#include "basespecs.h"

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

namespace sgraph{
class ffmpegi_stream : public sgstream{
public:
	ffmpegi_stream();

};

class spec_ffmpegi : public spec_image{
public:
	spec_ffmpegi(AVCodecContext *cod_context, uint8_t channels);

};
}

#endif
