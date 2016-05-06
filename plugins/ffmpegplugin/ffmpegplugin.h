
#ifndef FFMPEGPLUGIN1_H
#define FFMPEGPLUGIN1_H

#include "base.h"

#include "ffmpegread.h"
#include "ffmpegwrite.h"

extern "C" sgraph::sgactor* create_pluginactor(const std::map<std::string,std::vector<std::string>> args);



#endif
