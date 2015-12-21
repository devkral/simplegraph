
#include "ffmpegvideosource.h"
#include <thread>
#include <iostream>
#include <cstring>
#include <exception>

namespace sgraph{


// TODO: bring this code to run; ffmpeg magic does return NOTHING

ffmpegvideosource::ffmpegvideosource(double freq, int64_t blocking, std::string format, std::string device): sgactor(freq, blocking)
{
	this->formatname=format;
	this->devicename=device;
}

void ffmpegvideosource::enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out)
{
	av_register_all();
	avdevice_register_all();
	avcodec_register_all();
	av_init_packet(this->packet);
	this->frame = av_frame_alloc();
	if (in.size()!=0 || out.size()!=1)
		throw(sgraph::sgraphStreamException("invalid amount of in- or outstreams"));


	AVInputFormat *input_device_format=NULL;
	input_device_format = av_input_video_device_next(input_device_format);
	while (input_device_format!=NULL)
	{
		if( this->formatname!="" || strcmp(this->formatname.c_str(), input_device_format->name)==0)
			break;
		input_device_format = av_input_video_device_next(input_device_format);
	}
	if (input_device_format==NULL)
	{
		throw(sgraphException("Error: finding video input format failed"));
	}
	AVFormatContext *formcont=NULL;
	AVDeviceInfoList *devices=NULL;





	input_device_format;
	this->input_device_format->read_header(this->form_context);
	int video_stream_index = av_find_best_stream(this->form_context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	this->cod_context = this->form_context->streams[video_stream_index]->codec;
	//av_opt_set_int(this->codec, "refcounted_frames", 1, 0);
	/* init the video decoder */
	if (avcodec_open2(this->cod_context, NULL, NULL) < 0) {
		return;
	}
	
	this->getManager()->updateStreamspec(out[0], new spec_ffmpegi(this->cod_context, 4));
	
		
		
	this->intern_thread=new std::thread(sgactor::thread_wrapper, this);
}
void ffmpegvideosource::run(const std::vector<std::shared_ptr<sgstream>> in)
{
	//this->packet.data=0;
	//this->packet.size=0;

	this->input_device_format->read_packet(this->form_context, this->packet);
	//av_read_frame(&this->contex, &packet)
	avcodec_decode_video2(this->cod_context, this->frame, &this->got_frame, this->packet);
	if (this->got_frame==0)
		return;
	int size = av_image_get_buffer_size(AV_PIX_FMT_RGBA64, this->frame->width, this->frame->height, 4);
	uint8_t *buffer=(uint8_t*)calloc(sizeof(uint8_t),size);
	 av_image_copy_to_buffer(buffer,size, this->frame->data, this->frame->linesize, AV_PIX_FMT_RGBA64, this->frame->width, this->frame->height, 4);
	this->streamsout[0]->updateStream(new stream_data(buffer, size));
	//this->packet.data=0;
	//this->packet.size=0;
	
}
void ffmpegvideosource::leave()
{
	
	
}

ffmpegvideosource::~ffmpegvideosource()
{
	av_free_packet(this->packet);
	av_frame_free(&this->frame);
	if (this->input_device_format)
		av_freep (this->input_device_format);
	if (this->codec)
		av_freep (this->codec);
	if (this->cod_context)
		avcodec_close (this->cod_context);
	if (this->form_context)
		avformat_close_input (&this->form_context);
}


}
