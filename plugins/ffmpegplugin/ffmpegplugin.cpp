
#include "ffmpegplugin.h"

#include <iostream>
#include <string>


sgraph::sgactor *create_pluginactor(const double freq, const int64_t blocking, const std::vector<std::string> args)
{
	if (args.size()==0)
	{
		std::cerr << "Too less arguments" << std::endl;
		return 0;
	}
	if (args[0]=="videosource")
	{
		return static_cast<sgraph::sgactor*>(new sgraph::ffmpegvideosource(freq, blocking, args[1], args[2]));
	}
}

