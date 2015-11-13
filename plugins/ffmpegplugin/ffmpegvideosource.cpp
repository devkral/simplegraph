
#include "ffmpegvideosource.h"
#include <thread>

namespace sgraph{

stream_ffmpegi::stream_ffmpegi(uint8_t * data[4], enum AVPixelFormat pix_fmt) : sgstream()
{
	/**av_image_fill_pointers 	(data,pix_fmt,height,
		this->ptr,
		const int  	linesizes[4] 
	) 	*/

}



spec_ffmpegi::spec_ffmpegi(AVInputFormat *input_format) : spec_image()
{
	this->capabilities.insert("ffmpeg");
}


ffmpegvideosource::ffmpegvideosource(double freq, int64_t blocking, std::string device): sgactor(freq, blocking)
{
	this->devicename=device;
}

void ffmpegvideosource::enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out)
{
	if (in.size()!=0 || out.size()!=1)
		throw(sgraph::MissingStreamException("?"));
	
	this->input_device_format=0;
	av_input_video_device_next(input_device_format);
	while (this->input_device_format!=0)
	{
		if( this->devicename!="" || this->devicename.c_str()!=this->input_device_format.name)
			break;
		av_input_video_device_next(this->input_device_format);
	}
	if (this->input_device_format==0)
		return;
	
	this->frame = *av_frame_alloc();
	av_init_packet(&this->packet);
	this->input_device_format->read_header(&this->context)
	this->getManager()->updateStreamspec(out[0], new spec_ffmpegi(this->input_device_format));
	
		
		
	this->intern_thread=new std::thread(target=sgactor::thread_wrapper, this);
}
void ffmpegvideosource::run(const std::vector<std::shared_ptr<sgstream>> in)
{
	//this->packet.data=0;
	//this->packet.size=0;
	AVFrame frame;
	this->input_device_format->read_packet(&this->context, &this->packet);
	//av_read_frame(&this->contex, &packet)
	avcodec_decode_video2(&this->context, this->frame, &this->got_frame, &this->packet);
	int size = av_image_get_buffer_size(this->frame->pix_fmt, this->frame->width, this->frame->height, this->frame->align);
	this->streamsout[0]->updateStream(new stream_data(this->frame.data, size));
	//this->packet.data=0;
	//this->packet.size=0;
	
}
void ffmpegvideosource::leave()
{
	
	av_free_packet(&this->packet);
	if (this->input_device)
	{
		delete this->input_device;
		this->input_device=0;
	}
}


}
