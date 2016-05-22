
#ifndef SGPLUGINMANAGER_H
#define SGPLUGINMANAGER_H

#include "base.h"

#include <iostream>
#include <string>
#include <tuple>
#include <map>

#if _WIN32|_WIN64
#include <windows.h>
#else
//#elif __unix__
#include <dlfcn.h>
#endif




namespace sgraph{

/**
#if _WIN32|_WIN64
const char _filesysdelimiter = '\\';
#else
//#elif __unix__
const char _filesysdelimiter = '/';
#endif
*/


const size_t find_token(const std::string &inp, char token)
{
	bool isopen=false;
	bool isignore=false;
	for (size_t count=0; count<inp.size(); count++)
	{
		if (isignore)
		{
			isignore=false;
			continue;
		}
		if (inp[count]=='\\')
		{
			isignore=true;
			continue;
		}
		if (inp[count]=='"')
		{
			if (isopen==false)
			{
				isopen=true;
				continue;
			}
			else
			{
				isopen=false;
				continue;
			}
		}
		// " handler takes care
		if (isopen==true)
			continue;
		if (inp[count]==token)
		{
			return count;
		}
	}
	return std::string::npos;
}


const size_t find_comment(const std::string &inp)
{
	return find_token(inp, '#');
}

const size_t find_split(const std::string &inp)
{
	return find_token(inp, ':');
}

const std::string strip(const std::string &inp)
{
	
	if (inp.size()==0)
	{
		return inp;
	}
	size_t first=0,last=0;
	for (first=0; first<inp.size(); first++)
	{
		if (isblank(inp[first]) == false)
			break;
	}
	
	// unsigned so move 0 to 1
	for (last=inp.size()-1; last+1>=1; last--)
	{
		if (isblank(inp[last]) == false)
			break;
	}
	// +1 to get last char
	return inp.substr(first,last-first+1);
}

const std::string strip_paragraphs(const std::string &inp)
{
	if (inp[0] == '"' && inp[inp.length()-1] == '"')
	{
		/**for (size_t count=1; count < inp.length()-2; count++)
		{
			//contains " because of multiple args, so break and abort
			if (inp[count] == '"')
			{
				return inp;
			}
		}*/
		return inp.substr(1, inp.length()-2);
	}
	return inp;
}

const std::vector<std::string> parse_args(const std::string &inp)
{
	std::vector<std::string> ret;
	bool isopen=false;
	bool istoken=false;
	bool isignore=false;
	size_t last=0;
	for (size_t count=0; count<inp.size();count++)
	{
		if (isignore)
		{
			isignore=false;
			continue;
		}
		if (inp[count]=='\\')
		{
			isignore=true;
			continue;
		}
		// use tokenhandler if istoken
		if (inp[count]=='"' && istoken==false)
		{
			if (isopen==false)
			{
				isopen=true;
				last=count+1;
				continue;
			}
			else
			{
				ret.push_back(inp.substr(last, count-last));
				isopen=false;
				continue;
			}
		}
		// " handler cares ignore other methods of token
		if (isopen==true)
			continue;
		
		if (isblank(inp[count])==false && istoken==false)
		{
			istoken=true;
			last=count;
		}else if(isblank(inp[count])==true && istoken==true)
		{
			ret.push_back(inp.substr(last, count-last));
			istoken=false;
		}
	}
	if (istoken==true)
	{
		ret.push_back(inp.substr(last));
	}
	if (isopen==true)
	{
		std::cerr << "Unterminated string in config detected: " << inp << std::endl;
	}
	return ret;
}

const std::tuple<std::string, std::vector<std::string>> parse_line(const std::string &line)
{
	std::string searchstring = line.substr(0, find_comment(line));
	size_t splitpos=find_split(searchstring);
	std::string name = strip_paragraphs(strip(searchstring.substr(0, splitpos)));
	if (splitpos==std::string::npos)
		return std::tuple<std::string, std::vector<std::string>>(name, std::vector<std::string>());
	std::vector<std::string> margs = parse_args(searchstring.substr(splitpos+1));
	return std::tuple<std::string, std::vector<std::string>>(name, margs);
}



sgactor *loadActor(const std::string &path, const std::map<std::string, std::vector<std::string>> &args)
{
	void *soplugin=0;
	typedef sgactor* (*init_sgnewactor_type) (const std::map<std::string, std::vector<std::string>>);
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
		ret = (sgnewactor2)(args);
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
	std::string callpath;
public:
	~pluginmanager();
	pluginmanager(const std::string &callpath="");
	void parsefile(const std::string &filepath);
	bool addPlugin(const std::map<std::string, std::vector<std::string>> args);
	void start();
	void pause();
	const std::vector<std::string> parse_path(const std::vector<std::string> &inp, const std::string &configpath);
};




}

#endif

