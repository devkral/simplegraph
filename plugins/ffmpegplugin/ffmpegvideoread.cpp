
#include "ffmpegvideoread.h"
#include <thread>
#include <iostream>
#include <exception>

namespace sgraph{

//stream_ffmpegi::stream_ffmpegi(uint8_t * data[4], enum AVPixelFormat pix_fmt) : sgstream()
//{
	/**av_image_fill_pointers 	(data,pix_fmt,height,
		this->ptr,
		const int  	linesizes[4] 
	) 	*/

//}



ffmpegvideoread::ffmpegvideoread(double freq, int64_t blocking, std::string sourcepath, std::string sourceprovider): sgactor(freq, blocking)
{
	this->sourcepath=sourcepath;
	this->sourceprovider=sourceprovider;
}

void ffmpegvideoread::enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out)
{
	int errorno;
	ffmpeg::av_register_all();
	//needed?
	ffmpeg::avcodec_register_all();

	if (in.size()!=0 || out.size()!=1)
		throw(sgraph::sgraphStreamException("invalid amount of in- or outstreams"));

	ffmpeg::AVInputFormat *input_device_format=NULL;
	if (this->sourceprovider!="")
	{
		//needed?
		ffmpeg::avdevice_register_all();
		input_device_format = ffmpeg::av_input_video_device_next(input_device_format);
		while (input_device_format!=NULL)
		{
			if(strcmp(this->sourceprovider.c_str(), input_device_format->name)==0)
				break;
			input_device_format = ffmpeg::av_input_video_device_next(input_device_format);
		}

		if (input_device_format==NULL)
		{
			throw(sgraphException("Error: finding video input format failed"));
		}
	}

	errorno =  ffmpeg::avformat_open_input (&this->form_context, this->sourcepath.c_str(), NULL, NULL);
	if (errorno<0)
	{
		throw(sgraphException("Error: opening video file failed"));
	}
	ffmpeg::av_dump_format(this->form_context, 0, this->sourcepath.c_str(), 0);

	this->video_stream_index = ffmpeg::av_find_best_stream(this->form_context, ffmpeg::AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if(this->video_stream_index < 0)
	{
		throw(sgraphException("Error: no videostream found"));
	}
	this->cod_context = this->form_context->streams[this->video_stream_index]->codec;
	//av_opt_set_int(this->codec, "refcounted_frames", 1, 0);
	this->codec = ffmpeg::avcodec_find_decoder(this->cod_context->codec_id);
	if (this->codec == NULL) {
		throw(sgraphException("Error: unsupported codec"));
	}

	// init the video decoder
	errorno = ffmpeg::avcodec_open2(this->cod_context, this->codec, NULL);
	if (errorno < 0) {
		throw(sgraphException("Error: codec could not be opened"));
	}

	this->getManager()->updateStreamspec(out[0], new spec_ffmpegi(this->cod_context, 4));

	ffmpeg::av_init_packet(&this->packet);
	this->origframe = ffmpeg::av_frame_alloc();
	this->convertframe = ffmpeg::av_frame_alloc();
	this->intern_thread=new std::thread(sgactor::thread_wrapper, this);
}
void ffmpegvideoread::run(const std::vector<std::shared_ptr<sgstream>> )
{
	//this->packet.data=0;
	//this->packet.size=0;
	if (ffmpeg::av_read_frame(this->form_context, &this->packet)<0)
		return;
	// dismiss wrong packets
	//std::cout << "read1\n";
	while(packet.stream_index!=this->video_stream_index)
	{
		if (ffmpeg::av_read_frame(this->form_context, &this->packet)<0)
			return;
	}
	//std::cout << "read2\n";


	//this->input_device_format->read_packet(this->form_context, this->packet);
	//av_read_frame(&this->contex, &packet)
	ffmpeg::avcodec_decode_video2(this->cod_context, this->origframe, &this->got_frame, &this->packet);
	if (this->got_frame==0)
		return;
	int size = ffmpeg::av_image_get_buffer_size(ffmpeg::AV_PIX_FMT_RGBA64, this->origframe->width, this->origframe->height, 0);
	//std::cout << size << std::endl;
	//uint8_t *buffer=(uint8_t*)calloc(sizeof(uint8_t),size);
	//ffmpeg::sws_scale(sws_ctx, (uint8_t const * const *)this->origframe->data,
	//		 this->convertframe->linesize, 0, this->cod_context->height,
	//		 pFrameRGB->data, pFrameRGB->linesize)
	//ffmpeg::av_image_copy_to_buffer(buffer,size, this->convertframe->data, this->convertframe->linesize, ffmpeg::AV_PIX_FMT_RGBA64, this->convertframe->width, this->convertframe->height, 0);
	//this->streamsout[0]->updateStream(new stream_data(buffer, size));
	//this->packet.data=0;
	//this->packet.size=0;
	
}
void ffmpegvideoread::leave()
{

	//if (this->input_device_format)
	//	av_freep (this->input_device_format);
	// Close the codec
	// Close the video file
	ffmpeg::av_frame_free(&this->origframe);
	ffmpeg::av_frame_free(&this->convertframe);
	ffmpeg::av_free_packet(&this->packet);

	ffmpeg::avcodec_close (this->cod_context);
	this->cod_context = 0;

	ffmpeg::avformat_close_input (&this->form_context);
	this->form_context = 0;

}

ffmpegvideoread::~ffmpegvideoread()
{


	//if (this->input_device_format)
	//	av_freep (this->input_device_format);
	// Close the codec
	if (this->cod_context)
		ffmpeg::avcodec_close (this->cod_context);
	// Close the video file
	if (this->form_context)
		ffmpeg::avformat_close_input (&this->form_context);

}


}
