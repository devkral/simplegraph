
#include "ffmpegvideoread.h"
#include <thread>
#include <iostream>
#include <cstring>
#include <exception>

namespace sgraph{

//stream_ffmpegi::stream_ffmpegi(uint8_t * data[4], enum AVPixelFormat pix_fmt) : sgstream()
//{
	/**av_image_fill_pointers 	(data,pix_fmt,height,
		this->ptr,
		const int  	linesizes[4] 
	) 	*/

//}



ffmpegvideoread::ffmpegvideoread(double freq, int64_t blocking, std::string filename): sgactor(freq, blocking)
{
	this->filename=filename;
}

void ffmpegvideoread::enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out)
{
	int errorno;
	av_register_all();
	av_init_packet(&this->packet);
	this->frame = av_frame_alloc();
	if (in.size()!=0 || out.size()!=1)
		throw(sgraph::sgraphStreamException("invalid amount of in- or outstreams"));

	errorno =  avformat_open_input (&this->form_context, this->filename.c_str(), NULL, NULL);
	if (errorno<0)
	{
		throw(sgraphException("Error: opening video file failed"));
	}
	av_dump_format(this->form_context, 0, this->filename.c_str(), 0);

	video_stream_index = av_find_best_stream(this->form_context, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if(video_stream_index < 0)
	{
		throw(sgraphException("Error: no videostream found"));
	}
	this->cod_context = this->form_context->streams[video_stream_index]->codec;
	//av_opt_set_int(this->codec, "refcounted_frames", 1, 0);
	this->codec = avcodec_find_decoder(this->cod_context->codec_id);
	if (this->codec == NULL) {
		throw(sgraphException("Error: unsupported codec"));
	}

	// init the video decoder
	errorno = avcodec_open2(this->cod_context, this->codec, NULL);
	if (errorno < 0) {
		throw(sgraphException("Error: codec could not be opened"));
	}



	this->getManager()->updateStreamspec(out[0], new spec_ffmpegi(this->cod_context, 4));
	this->intern_thread=new std::thread(sgactor::thread_wrapper, this);
}
void ffmpegvideoread::run(const std::vector<std::shared_ptr<sgstream>> in)
{
	//this->packet.data=0;
	//this->packet.size=0;
	if (av_read_frame(this->form_context, &this->packet)<0)
		return;
	// dismiss wrong packets
	while(packet.stream_index!=video_stream_index)
	{
		if (av_read_frame(this->form_context, &this->packet)<0)
			return;
	}


	//this->input_device_format->read_packet(this->form_context, this->packet);
	//av_read_frame(&this->contex, &packet)
	avcodec_decode_video2(this->cod_context, this->frame, &this->got_frame, &this->packet);
	if (this->got_frame==0)
		return;
	int size = av_image_get_buffer_size(AV_PIX_FMT_RGBA64, this->frame->width, this->frame->height, 4);
	uint8_t *buffer=(uint8_t*)calloc(sizeof(uint8_t),size);
	 av_image_copy_to_buffer(buffer,size, this->frame->data, this->frame->linesize, AV_PIX_FMT_RGBA64, this->frame->width, this->frame->height, 4);
	this->streamsout[0]->updateStream(new stream_data(buffer, size));
	//this->packet.data=0;
	//this->packet.size=0;
	
}
void ffmpegvideoread::leave()
{

	av_free_packet(&this->packet);
	if (this->frame)
	{
		av_frame_free(&this->frame);
		this->frame =0;
	}
	//if (this->input_device_format)
	//	av_freep (this->input_device_format);
	// Close the codec
	if (this->cod_context)
	{
		avcodec_close (this->cod_context);
		this->cod_context = 0;
	}
	// Close the video file
	if (this->form_context)
	{
		avformat_close_input (&this->form_context);
		this->form_context = 0;
	}
}

ffmpegvideoread::~ffmpegvideoread()
{

	if (this->frame)
		av_frame_free(&this->frame);
	//if (this->input_device_format)
	//	av_freep (this->input_device_format);
	// Close the codec
	if (this->cod_context)
		avcodec_close (this->cod_context);
	// Close the video file
	if (this->form_context)
		avformat_close_input (&this->form_context);

}


}
