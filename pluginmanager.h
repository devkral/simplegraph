
#ifndef SGPLUGINMANAGER_H
#define SGPLUGINMANAGER_H

#include "base.h"

#include <iostream>
#include <string>
#include <tuple>

#if _WIN32|_WIN64
#include <windows.h>
#else
//#elif __unix__
#include <dlfcn.h>
#endif




namespace sgraph{

#if _WIN32|_WIN64
const char _filesysdelimiter = '\\';
#else
//#elif __unix__
const char _filesysdelimiter = '/';
#endif


const size_t find_comment(const std::string &inp, size_t startpos=0, size_t limitpos=std::string::npos)
{
	bool isopen=false;
	size_t pos=startpos, poscomment;
	poscomment=inp.find_first_of('#', pos);
	pos=inp.find_first_of('"', pos);
	while (pos<limitpos)
	{
		if (!isopen)
		{
			if (poscomment<pos)
				return poscomment;
			poscomment=inp.find_first_of('#', pos);
		}
		isopen = (false==isopen);

		pos=inp.find_first_of('"', pos+1);
	}
	return limitpos;
}


const std::tuple<std::string, size_t> string_split_single(const std::string &inp, size_t startpos=0, size_t limitpos=std::string::npos)
{
	size_t first=startpos,last=0;
	first=inp.find_first_of('"', first);
	if (first>=limitpos)
	{
		return std::tuple<std::string, size_t>("", std::string::npos);
	}
	
	if (inp.at(first-1)=='\\')
	{
		std::cerr << "Error: floating \\\" : \"" << inp << "\"" << std::endl;
		return std::tuple<std::string, size_t>("", std::string::npos);
	}
	//first += 1; // set to first character
	last=inp.find_first_of('"', first+1);
	while (last<limitpos && inp.at(last-1)=='\\')
	{
		last=inp.find_first_of('"', last+1);
	}
	if (last>=limitpos)
	{
		std::cerr << "Error: \" not closed: \"" << inp << "\"" << std::endl;
		return std::tuple<std::string, size_t>("", std::string::npos);
	}
	else if (last==first+1)
	{
		return std::tuple<std::string, size_t>("", last+1);
	}
	return std::tuple<std::string, size_t>(inp.substr(first+1, (last-1)-first), last+1);
}

const std::vector<std::string> string_split_multiple(const std::string &inp, size_t startpos=0, size_t limitpos=std::string::npos)
{
	std::vector<std::string> ret;
	size_t pos=startpos;
	std::tuple<std::string, size_t> temp = string_split_single(inp, pos, limitpos);
	while (std::get<1>(temp)<limitpos)
	{
		pos = std::get<1>(temp);
		ret.push_back(std::get<0>(temp));
		temp = string_split_single(inp, pos, limitpos);
	}
	return ret;
}





sgactor *loadActor(const std::string &path, const double &freq, const int64_t &blocking, const std::vector<std::string> &args)
{
	void *soplugin=0;
	typedef sgactor* (*init_sgnewactor_type) (const double, const int64_t, const std::vector<std::string> );
	void *sgnewactor=0;
	sgactor *ret=0;
#if _WIN32|_WIN64
	soplugin = LoadLibrary(path.c_str());
#else
//#elif __unix__
	soplugin = dlopen(path.c_str(), RTLD_LAZY|RTLD_NODELETE);
	dlerror(); // clears existing error messages
#endif
	
	if (soplugin == 0)
	{
		std::cerr << "Error: \"" << path << "\" cannot be loaded" << std::endl;
		return 0;
	}

#if _WIN32|_WIN64
	sgnewactor = GetProcAddress(soplugin,"create_pluginactor");
#else
//#elif __unix__
	sgnewactor = dlsym(soplugin, "create_pluginactor");
	//std::cerr << dlerror() << std::endl;
#endif
//reinterpret_cast<init_sgnewactor_type> (

	
	if (sgnewactor == 0)
	{
		std::cerr << "Error: \"" << path << "\" could not load: create_pluginactor" << std::endl;
		ret = 0;
	}
	else
	{
		init_sgnewactor_type sgnewactor2 = reinterpret_cast<init_sgnewactor_type>(reinterpret_cast<uint64_t>(sgnewactor));
		ret = (sgnewactor2)(freq, blocking, args);
	}
#if _WIN32|_WIN64
	FreeLibrary(soplugin);
#else
//#elif __unix__
	dlclose(soplugin);
#endif
	return ret;
}

class pluginmanager{
private:
	sgmanager *manager=0;

public:
	~pluginmanager();
	pluginmanager();
	void parsefile(const std::string &filepath);
	bool addPlugin(const std::string &name, const std::string &path, const double &freq, const int64_t &blocking, const std::vector<std::string> &args, const std::vector<std::string> &instreams, const std::vector<std::string> &outstreams);



};




}

#endif

