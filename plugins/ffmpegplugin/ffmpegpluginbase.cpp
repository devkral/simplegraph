
#include "ffmpegpluginbase.h"

namespace sgraph{



// copies packet
stream_ffmpeg_packet::stream_ffmpeg_packet(AVPacket *packet) : sgstream()
{
	av_copy_packet(&this->packet, packet);
}

stream_ffmpeg_packet::~stream_ffmpeg_packet()
{
	av_packet_unref(&this->packet);
}


spec_ffmpeg_packet::spec_ffmpeg_packet(bool encode, AVCodecContext *videocontext=0, AVCodecContext *audiocontext=0) : sgstreamspec()
{
	this->capabilities.insert("spec_ffmpeg");
	this->capabilities.insert("spec_packet");
	this->videocontext = videocontext;
	this->audiocontext = audiocontext;

	if (this->videocontext)
	{
		if (encode)
			this->videocodec = avcodec_find_encoder(this->videocontext->codec_id);
		else
			this->videocodec = avcodec_find_decoder(this->videocontext->codec_id);
		if (this->videocodec == 0) {
			throw(sgraphException("Error: unsupported video codec"));
		}
		if (avcodec_open2(this->videocontext, this->videocodec, NULL) < 0) {
			throw(sgraphException("Error: video codec could not be opened"));
		}
	}
	if (this->audiocontext)
	{

		if (encode)
			this->audiocodec = avcodec_find_encoder(this->audiocontext->codec_id);
		else
			this->audiocodec = avcodec_find_decoder(this->audiocontext->codec_id);
		if (this->audiocodec == 0) {
			throw(sgraphException("Error: unsupported audio codec"));
		}

		if (avcodec_open2(this->audiocontext, this->audiocodec, NULL) < 0) {
			throw(sgraphException("Error: audio codec could not be opened"));
		}
	}
}
spec_ffmpeg_packet::~spec_ffmpeg_packet()
{
	//cleared by close_input, but must be closed here
	if (this->videocontext)
		avcodec_close (this->videocontext);
	if (this->audiocontext)
		avcodec_close (this->audiocontext);
}

spec_ffmpeg_image::spec_ffmpeg_image(AVCodecContext *cod_context, uint8_t channels) : spec_image()
{
	this->capabilities.insert("spec_ffmpegi");
	this->width = cod_context->width;
	this->height = cod_context->height;
	this->channels=channels;
	//RGBA
	//uint64_t width, uint64_t height, uint32_t channels
}

}

