
#include "ffmpegvideoread.h"
#include "ffmpegpluginbase.h"
#include <iostream>

using namespace sgraph;

int mmm()
{
	ffmpeg::av_register_all();
	ffmpeg::avdevice_register_all ();
	ffmpeg::avcodec_register_all();

	ffmpeg::AVInputFormat *input_device_format=NULL, *input_device_format2=NULL;
	input_device_format = ffmpeg::av_input_video_device_next(input_device_format);
	ffmpeg::AVInputFormat *lala=NULL;
	ffmpeg::AVFormatContext *lala2=0; //avformat_alloc_context();
	//ff_alloc_input_device_context(&lala2, lala, lala->name);
	ffmpeg::AVDeviceInfoList *devices=0;
	while (input_device_format!=NULL)
	{
		if (strcmp(input_device_format->name,"video4linux2,v4l2")==0)
		{
			lala = input_device_format;
		}
		std::cout << "deviceformat: " << input_device_format->name << std::endl;
		input_device_format = ffmpeg::av_input_video_device_next(input_device_format);
	}

	if (lala==0)
	{
		return 1;
	}
	std::cerr << "cur deviceformat: " << lala->name << std::endl;
	if (ffmpeg::avformat_open_input(&lala2, "/dev/video0", lala, NULL)!=0)
	{
		std::cerr << "init lala2 failed\n";
		return -1;
	}
	int errorno = ffmpeg::avdevice_list_devices(lala2,&devices);
	if (errorno<0)
	{
		std::cerr << lala2->iformat->get_device_list << "\n";
		char charbuf[1000];
		ffmpeg::av_strerror(errorno, charbuf, 1000);
		std::cerr << "list devices failed: " << errorno << " " << charbuf << std::endl;
		ffmpeg::avformat_free_context(lala2);
		return errorno;
	}
	for (int count=0;count<devices->nb_devices; count++)
	{
		std::cout << "device: " << devices->devices[count]->device_name << std::endl;
	}
	ffmpeg::avdevice_free_list_devices(&devices);
	ffmpeg::avformat_close_input (&lala2);
	return 0;
}


int main(int argc, char *argv[])
{
	if (argc<2)
	{
		std::cerr << "needs argument" << std::endl;
		return -1;
	}
	sgraph::sgmanager man;
	
	std::vector<std::string> ac1streamsin;
	std::vector<std::string> ac1streamsout;
	ac1streamsout.push_back("stream1");
	try{
		man.addActor("video",new ffmpegvideoread(1,1,argv[1]), ac1streamsin, ac1streamsout);

	}
	catch(sgraphException &e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	std::vector<std::string> actorsretrieve;
	actorsretrieve.push_back("video");
	for (sgraph::sgactor* actor : man.getActors(actorsretrieve))
	{
		std::cout << "Actorname:" << actor->getName() << std::endl;
	
	}
	man.start();
	man.cleanupActors();
	std::cout << "simplegraph test started, press any key to exit" << std::endl;
	getchar();
	return 0;
}

