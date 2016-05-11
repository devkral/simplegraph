
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



ffmpegread::ffmpegread(double freq, int64_t blocking, std::string sourcepath, std::string sourceprovider, std::string type): sgactor(freq, blocking)
{
	this->sourcepath=sourcepath;
	this->sourceprovider=sourceprovider;
	this->type = type;
}

void ffmpegread::enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out)
{
	int errorno;
	av_register_all();
	//needed!
	avcodec_register_all();
	//needed!
	avdevice_register_all();

	if (in.size()!=0 || (out.size()!=1 && out.size()!=2))
		throw(sgraph::sgraphStreamException("invalid amount of in- or outstreams"));

	if (this->sourceprovider!="")
		errorno =  avformat_open_input (&this->form_context, this->sourcepath.c_str(), av_find_input_format(this->sourceprovider.c_str()), NULL);
	else
		errorno =  avformat_open_input (&this->form_context, this->sourcepath.c_str(), NULL, NULL);
	if (errorno<0)
	{
		throw(sgraphException("Error: opening file failed"));
	}
	av_dump_format(this->form_context, 0, this->sourcepath.c_str(), 0);

	AVCodecContext *audiocontext=0, *videocontext=0;
	if (out.size()==2 || this->type=="video" || this->type=="mixed")
	{
		this->video_stream_index = av_find_best_stream(this->form_context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
		if(this->video_stream_index >= 0)
		{
			videocontext = this->form_context->streams[this->video_stream_index]->codec;
		}
	}

	if (out.size()==2 || this->type=="audio" || this->type=="mixed")
	{
		this->audio_stream_index = av_find_best_stream(this->form_context, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
		if(this->audio_stream_index >= 0)
		{
			audiocontext = this->form_context->streams[this->audio_stream_index]->codec;
		}
	}
	if (out.size()==1)
		this->getManager()->updateStreamspec(out[0], new spec_ffmpeg_packet(false, videocontext, audiocontext));
	else
	{
		this->getManager()->updateStreamspec(out[0], new spec_ffmpeg_packet(false, videocontext, 0));
		this->getManager()->updateStreamspec(out[0], new spec_ffmpeg_packet(false, 0, audiocontext));
	}
}
void ffmpegread::run(const std::vector<std::shared_ptr<sgstream>> )
{
	av_init_packet(&this->packet);
	this->gotvideo=false;
	this->gotaudio=false;
	while((this->video_stream_index>=0 && this->gotvideo==false) && (this->audio_stream_index>=0 && this->gotaudio==false))
	{
		if (av_read_frame(this->form_context, &this->packet)<0 || this->packet.stream_index<0)
		{
			av_packet_unref(&this->packet);
			return;
		}
		if (this->packet.stream_index==this->video_stream_index)
		{
			this->streamsout[0]->updateStream(new stream_ffmpeg_packet(&this->packet));
			this->gotvideo=true;
		}
		if (this->packet.stream_index==this->audio_stream_index)
		{
			if (this->streamsout.size()==2)
				this->streamsout[1]->updateStream(new stream_ffmpeg_packet(&this->packet));
			else
				this->streamsout[0]->updateStream(new stream_ffmpeg_packet(&this->packet));
			this->gotaudio=true;
		}
	}
	av_packet_unref(&this->packet);
}
void ffmpegread::leave()
{

}

ffmpegread::~ffmpegread()
{
	if (this->form_context)
	{
		avformat_close_input (&this->form_context);
	}
}
}
