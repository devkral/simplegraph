
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
	if (args[0]=="read")
	{
		if (args.size()==1)
		{
			return static_cast<sgraph::sgactor*>(new sgraph::ffmpegread(freq, blocking, args[1]));
		}
		else if (args.size()==2)
		{
			return static_cast<sgraph::sgactor*>(new sgraph::ffmpegread(freq, blocking, args[1], args[2]));
		}
		else if (args.size() >= 3)
		{
			return static_cast<sgraph::sgactor*>(new sgraph::ffmpegread(freq, blocking, args[1], args[2], args[3]));
		}
	} else if (args[0]=="write")
	{
		if (args.size()==1)
		{
			return static_cast<sgraph::sgactor*>(new sgraph::ffmpegwrite(freq, blocking, args[1]));
		}
		else
		{
			return static_cast<sgraph::sgactor*>(new sgraph::ffmpegwrite(freq, blocking, args[1], args[2]));
		}
	}
	return 0;
}

