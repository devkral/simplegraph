
#include <thread>
#include <iostream>
#include <exception>
#include "ffmpegread.h"

namespace sgraph{

//stream_ffmpegi::stream_ffmpegi(uint8_t * data[4], enum AVPixelFormat pix_fmt) : sgstream()
//{
	/**av_image_fill_pointers 	(data,pix_fmt,height,
		this->ptr,
		const int  	linesizes[4] 
	) 	*/

//}



ffmpegread::ffmpegread(double freq, int64_t blocking, std::string sourcepath, std::string sourceprovider): sgactor(freq, blocking)
{
	this->sourcepath=sourcepath;
	this->sourceprovider=sourceprovider;
}

void ffmpegread::enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out)
{
	int errorno;
	ffmpeg::av_register_all();
	//needed!
	ffmpeg::avcodec_register_all();
	//needed!
	ffmpeg::avdevice_register_all();

	if (in.size()!=0 || out.size()!=1)
		throw(sgraph::sgraphStreamException("invalid amount of in- or outstreams"));

	if (this->sourceprovider!="")
		errorno =  ffmpeg::avformat_open_input (&this->form_context, this->sourcepath.c_str(), ffmpeg::av_find_input_format(this->sourceprovider.c_str()), NULL);
	else
		errorno =  ffmpeg::avformat_open_input (&this->form_context, this->sourcepath.c_str(), NULL, NULL);
	if (errorno<0)
	{
		throw(sgraphException("Error: opening file failed"));
	}
	ffmpeg::av_dump_format(this->form_context, 0, this->sourcepath.c_str(), 0);

	ffmpeg::AVCodecContext *audiocontext=0, *videocontext=0;
	this->video_stream_index = ffmpeg::av_find_best_stream(this->form_context, ffmpeg::AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if(this->video_stream_index >= 0)
	{
		videocontext = this->form_context->streams[this->video_stream_index]->codec;
	}

	this->audio_stream_index = ffmpeg::av_find_best_stream(this->form_context, ffmpeg::AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	if(this->audio_stream_index >= 0)
	{
		audiocontext = this->form_context->streams[this->audio_stream_index]->codec;
	}

	this->getManager()->updateStreamspec(out[0], new spec_ffmpeg_packet(false, videocontext, audiocontext));
	this->intern_thread=new std::thread(sgactor::thread_wrapper, this);
}
void ffmpegread::run(const std::vector<std::shared_ptr<sgstream>> )
{
	ffmpeg::av_init_packet(&this->packet);
	if (ffmpeg::av_read_frame(this->form_context, &this->packet)<0)
		return;
	while(this->packet.stream_index!=this->video_stream_index && this->packet.stream_index!=this->audio_stream_index)
	{
		if (ffmpeg::av_read_frame(this->form_context, &this->packet)<0)
		{
			return;
		}
	}
	this->streamsout[0]->updateStream(new stream_ffmpeg_packet(&this->packet));
	ffmpeg::av_packet_unref(&this->packet);
}
void ffmpegread::leave()
{

}

ffmpegread::~ffmpegread()
{
	if (this->form_context)
	{
		ffmpeg::avformat_close_input (&this->form_context);
	}
}
}
