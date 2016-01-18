
#include <thread>
#include <iostream>
#include <cstring>
#include <exception>
#include "ffmpegwrite.h"

namespace sgraph{

//stream_ffmpegi::stream_ffmpegi(uint8_t * data[4], enum AVPixelFormat pix_fmt) : sgstream()
//{
	/**av_image_fill_pointers 	(data,pix_fmt,height,
		this->ptr,
		const int  	linesizes[4] 
	) 	*/

//}




ffmpegwrite::ffmpegwrite(double freq, int64_t blocking, std::string outsink, std::string outformat): sgactor(freq, blocking)
{
	this->outsink=outsink;
	this->outformat=outformat;
}

void ffmpegvideowrite::enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out)
{
	int error;
	ffmpeg::av_register_all();
	ffmpeg::avdevice_register_all();
	if (in.size()!=1 || out.size()!=0)
		throw(sgraph::sgraphStreamException("outstreams specified"));
	if (this->outformat=="")
		avformat_alloc_output_context2(&this->form_context, NULL, NULL, this->outsink.c_str());
	else
		avformat_alloc_output_context2(&this->form_context, NULL, this->outformat.c_str(), this->outsink.c_str());
	//this->cod_context = this->form_context->streams[video_stream_index]->codec;
	//av_opt_set_int(this->codec, "refcounted_frames", 1, 0);
	/* init the video decoder */



	error = avformat_write_header(this->form_context, NULL);
	if (error < 0) {
		throw(sgraph::sgraphStreamException("Error occurred when opening output file"));
	}

	this->intern_thread=new std::thread(sgactor::thread_wrapper, this);
}
void ffmpegwrite::run(const std::vector<std::shared_ptr<sgstream>> in)
{
	stream_ffmpeg_packet *temp = (stream_ffmpeg_packet*)in[0].get();
	ffmpeg::av_interleaved_write_frame(this->form_context, &temp->packet);

}
void ffmpegwrite::leave()
{

	ffmpeg::avformat_free_context(this->form_context);
	this->form_context=0;
}

ffmpegwrite::~ffmpegwrite()
{
	if (this->form_context)
		ffmpeg::avformat_free_context (this->form_context);
}


}
