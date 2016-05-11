
#include <iostream>
#include <string>

#include "ffmpegplugin.h"



sgraph::sgactor *create_pluginactor(const std::map<std::string,std::vector<std::string>> args)
{
	std::string type = sgraph::default_value_map(args, "type", "")[0];
	double freq = stod(sgraph::default_value_map(args, "freq", "1")[0]);
	int64_t blocking = stoi(sgraph::default_value_map(args, "blocking", "-1")[0]);
	int32_t parallelize = stoi(sgraph::default_value_map(args, "parallelize", "1")[0]);
	if (type=="read")
	{
/**		if (args.size()==1)
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
		}*/
	} else if (type=="write")
	{
	/*
		if (args.size()==1)
		{
			return static_cast<sgraph::sgactor*>(new sgraph::ffmpegwrite(freq, blocking, args[1]));
		}
		else
		{
			return static_cast<sgraph::sgactor*>(new sgraph::ffmpegwrite(freq, blocking, args[1], args[2]));
		}*/
	}
	return 0;
}

