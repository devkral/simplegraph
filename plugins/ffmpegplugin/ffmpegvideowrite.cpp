
#include <thread>
#include <iostream>
#include <cstring>
#include <exception>
#include "ffmpegvideowrite.h"

namespace sgraph{

//stream_ffmpegi::stream_ffmpegi(uint8_t * data[4], enum AVPixelFormat pix_fmt) : sgstream()
//{
	/**av_image_fill_pointers 	(data,pix_fmt,height,
		this->ptr,
		const int  	linesizes[4] 
	) 	*/

//}




ffmpegvideowrite::ffmpegvideowrite(double freq, int64_t blocking, std::string outformat, std::string outfile): sgactor(freq, blocking)
{
	this->outformat=outformat;
	this->filename=outfile;
}

void ffmpegvideowrite::enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out)
{
	avdevice_register_all();
	av_register_all();
	if (in.size()!=1 || out.size()!=0)
		throw(sgraph::sgraphStreamException("outstreams specified"));
	

	av_init_packet(&this->packet);
	this->frame = av_frame_alloc();
	//this->cod_context = this->form_context->streams[video_stream_index]->codec;
	//av_opt_set_int(this->codec, "refcounted_frames", 1, 0);
	/* init the video decoder */

	this->intern_thread=new std::thread(sgactor::thread_wrapper, this);
}
void ffmpegvideowrite::run(const std::vector<std::shared_ptr<sgstream>> in)
{
	
}
void ffmpegvideowrite::leave()
{
	
	
}

ffmpegvideowrite::~ffmpegvideowrite()
{
	av_free_packet(&this->packet);
}


}
