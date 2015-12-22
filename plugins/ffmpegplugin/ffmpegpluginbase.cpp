
#include "ffmpegpluginbase.h"

namespace sgraph{
spec_ffmpegi::spec_ffmpegi(ffmpeg::AVCodecContext *cod_context, uint8_t channels) : spec_image()
{
	this->capabilities.insert("spec_ffmpegi");
	this->width = cod_context->width;
	this->height = cod_context->height;
	this->channels=channels;
	//RGBA
	//uint64_t width, uint64_t height, uint32_t channels
}

}

