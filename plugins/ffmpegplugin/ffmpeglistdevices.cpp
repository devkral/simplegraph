
#include "ffmpegpluginbase.h"
#include <iostream>

using namespace sgraph;

void list_devices()
{
	ffmpeg::avdevice_register_all ();

	ffmpeg::AVInputFormat *input_device_format=NULL;
	ffmpeg::AVOutputFormat *output_device_format=NULL;
	input_device_format = ffmpeg::av_input_video_device_next(input_device_format);
	while (input_device_format!=NULL)
	{
		std::cout << "input video: " << input_device_format->name << std::endl;
		input_device_format = ffmpeg::av_input_video_device_next(input_device_format);
	}
	
	output_device_format = ffmpeg::av_output_video_device_next(output_device_format);
	while (output_device_format!=NULL)
	{
		std::cout << "output video: " << output_device_format->name << std::endl;
		output_device_format = ffmpeg::av_output_video_device_next(output_device_format);
	}
	
	input_device_format = ffmpeg::av_input_audio_device_next(input_device_format);
	while (input_device_format!=NULL)
	{
		std::cout << "input audio: " << input_device_format->name << std::endl;
		input_device_format = ffmpeg::av_input_audio_device_next(input_device_format);
	}
	
	output_device_format = ffmpeg::av_output_audio_device_next(output_device_format);
	while (output_device_format!=NULL)
	{
		std::cout << "output audio: " << output_device_format->name << std::endl;
		output_device_format = ffmpeg::av_output_audio_device_next(output_device_format);
	}

}


int main(int argc, char *argv[])
{
	list_devices();
	return 0;
}

