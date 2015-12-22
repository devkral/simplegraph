
#include "ffmpegplugin.h"

#include <iostream>
#include <string>

sgraph::sgactor *create_pluginactor(const double freq, const int64_t blocking, const std::vector<std::string> args)
{
	if (args.size()==0)
	{
		std::cerr << "Too few arguments" << std::endl;
		return 0;
	}
	if (args[0]=="videoread")
	{
		if (args.size()==2)
		{
			return static_cast<sgraph::sgactor*>(new sgraph::ffmpegvideoread(freq, blocking, args[1]));
		}
		else
		{
			return static_cast<sgraph::sgactor*>(new sgraph::ffmpegvideoread(freq, blocking, args[1], args[2]));
		}
	} else if (args[0]=="videowrite")
	{
		return static_cast<sgraph::sgactor*>(new sgraph::ffmpegvideowrite(freq, blocking, args[1], args[2]));
	}
	return 0;
}

