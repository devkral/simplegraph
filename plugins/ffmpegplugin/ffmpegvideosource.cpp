
#include "ffmpegvideosource.h"
#include <thread>

namespace sgraph{

//stream_ffmpegi::stream_ffmpegi(uint8_t * data[4], enum AVPixelFormat pix_fmt) : sgstream()
//{
	/**av_image_fill_pointers 	(data,pix_fmt,height,
		this->ptr,
		const int  	linesizes[4] 
	) 	*/

//}



spec_ffmpegi::spec_ffmpegi(AVCodecContext *cod_context, uint8_t channels) : spec_image()
{
	this->capabilities.insert("spec_ffmpegi");
	this->width = cod_context->width;
	this->height = cod_context->height;
	this->channels=channels;
	//RGBA
	//uint64_t width, uint64_t height, uint32_t channels
}


ffmpegvideosource::ffmpegvideosource(double freq, int64_t blocking, std::string format, std::string device): sgactor(freq, blocking)
{
	this->devicename=device;
}

void ffmpegvideosource::enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out)
{
	if (in.size()!=0 || out.size()!=1)
		throw(sgraph::MissingStreamException("?"));
	
	this->input_device_format=0;
	av_input_video_device_next(this->input_device_format);
	while (this->input_device_format!=0)
	{
		if( this->devicename!="" || this->devicename.c_str()!=this->input_device_format->name)
			break;
		av_input_video_device_next(this->input_device_format);
	}
	if (this->input_device_format==0)
		return;
	
	av_init_packet(&this->packet);
	this->frame = av_frame_alloc();
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

	this->input_device_format->read_packet(this->form_context, &this->packet);
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
void ffmpegvideosource::leave()
{
	
	
}

ffmpegvideosource::~ffmpegvideosource()
{
	av_free_packet(&this->packet);
	av_frame_free(&this->frame);
	if (this->input_device_format)
		av_freep (this->input_device_format);
	if (this->codec)
		av_freep (this->codec);
	if (this->cod_context)
		av_freep (this->cod_context);
	if (this->form_context)
		av_freep (this->form_context);
}


}
