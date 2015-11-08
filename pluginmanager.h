
#ifndef SGPLUGINMANAGER_H
#define SGPLUGINMANAGER_H

#include "base.h"

#include <iostream>

#if __unix__
#include <dlfcn.h>
#elif __win32__|__win64__
#include <windows.h>
#endif

namespace sgraph{

sgactor *loadLibrary(const std::string path, std::vector<std::string> args) //, const std::string name)
{
	void *soplugin=0;
	typedef sgactor* (*init_sgnewactor_type) (std::vector<std::string> );
	typedef void (*fptr)();
	void *sgnewactor;
#if __unix__
	soplugin = dlopen(path.c_str(), RTLD_LAZY);
#elif _WIN32|_WIN64
	soplugin = LoadLibrary(path.c_str());
#endif

	if (soplugin == 0)
	{
		std::cerr << "Error: \"" << path << "\" cannot be loaded" << std::endl;
		return 0;
	}

#if __unix__
	sgnewactor = dlsym(soplugin,"create_pluginactor");
#elif _WIN32|_WIN64
	sgnewactor = GetProcAddress(soplugin,"create_pluginactor");
#endif
//reinterpret_cast<init_sgnewactor_type> (

#if __unix__
	dlclose(soplugin);
#elif _WIN32|_WIN64
	FreeLibrary(soplugin);
#endif

	if (sgnewactor == 0)
	{
		std::cerr << "Error: \"" << path << "\" has not attribute: create_pluginactor " << std::endl;
		return 0;
	}
	init_sgnewactor_type sgnewactor2 = reinterpret_cast<init_sgnewactor_type>(reinterpret_cast<uint64_t>(sgnewactor));
	return (sgnewactor2)(args);
}


}

#endif

