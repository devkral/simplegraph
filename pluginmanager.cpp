
#include "pluginmanager.h"

#include <fstream>

namespace sgraph{

pluginmanager::pluginmanager()
{
	this->manager = new sgmanager;

}
pluginmanager::~pluginmanager()
{
	delete this->manager;
	this->manager = 0;
}

void pluginmanager::parsefile(std::string filepath)
{
	std::ifstream stream = std::ifstream(filepath);
	std::string line;
	int8_t foundline=0;
	std::string name, path;
	std::vector<std::string> args, instreams, outstreams;
	std::tuple<std::string,size_t> tempret;
	while (stream.eof()==false)
	{
		std::getline(stream, line);
		if (foundline==0 && line.find("module:")!=std::string::npos)
		{
			tempret = string_split_single(line,line.find("module:")+7);
			name = std::get<0>(tempret);
			foundline=1;
		} else if(foundline==1 && line.find("module:")!=std::string::npos)
		{
			this->addPlugin(name, path, args, instreams, outstreams);
			tempret = string_split_single(line,line.find("module:")+7);
			name = std::get<0>(tempret);
			path="";
			args.clear();
			instreams.clear();
			outstreams.clear();
			foundline=0;
		}else if(foundline==1 && line.find("path=")!=std::string::npos)
		{
			tempret = string_split_single(line,line.find("path=")+5);
			path = std::get<0>(tempret);
			if (path=="")
			{
				path=name+_filesysdelimiter+name;
			}
		}else if(foundline==1 && line.find("args=")!=std::string::npos)
		{
			args = string_split_multiple(line, line.find("args=")+5);
		}else if(foundline==1 && line.find("instreams=")!=std::string::npos)
		{
			instreams = string_split_multiple(line, line.find("instreams=")+10);
		}else if(foundline==1 && line.find("outstreams=")!=std::string::npos)
		{
			outstreams = string_split_multiple(line, line.find("outstreams=")+11);
		}
		
		
	}
	stream.close();
	// add last module, which isn't terminated by module:
	if (foundline==1)
	{
		this->addPlugin(name, path, args, instreams, outstreams);
	}

}
bool pluginmanager::addPlugin(const std::string name, const std::string path, const std::vector<std::string> args, const std::vector<std::string> instreams, const std::vector<std::string> outstreams)
{
	sgactor *actor = loadActor(path, args);
	if (actor == 0)
	{
	
		return false;
	}
	this->manager->addActor(name, actor, instreams, outstreams);
	return true;
}
}

int main(int argc, char *argv[])
{
	sgraph::pluginmanager pluman;
	for (uint64_t count=0; count<argc; count++)
	{
		pluman.parsefile(argv[count]);
	}
	getchar();
}
