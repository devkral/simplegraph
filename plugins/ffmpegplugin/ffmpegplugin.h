
#ifndef FFMPEGPLUGIN1_H
#define FFMPEGPLUGIN1_H

#include "base.h"

#include "ffmpegread.h"
#include "ffmpegwrite.h"

extern "C" sgraph::sgactor* create_pluginactor(const double freq, const int64_t blocking, const std::vector<std::string> args);



#endif
