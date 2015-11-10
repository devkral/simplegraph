
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

void pluginmanager::parsefile(const std::string &filepath)
{
	if (filepath=="")
	{
		return;
	}
	std::ifstream stream = std::ifstream();
	stream.open(filepath);
	std::string line;
	int8_t foundline=0;
	double freq=1;
	int64_t blocking=-1;
	std::string name, path;
	std::vector<std::string> args, instreams, outstreams;
	std::tuple<std::string,size_t> tempret;
	if (stream.is_open())
	{
		std::cout << "\"" << filepath << "\" loaded" << std::endl;
	}
	
	while (stream.is_open() && stream.eof()==false)
	{
		std::getline(stream, line);
		if (foundline==0 && line.find("module:")!=std::string::npos)
		{
			tempret = string_split_single(line,line.find("module:")+7);
			name = std::get<0>(tempret);
			foundline=1;
		} else if(foundline==1 && line.find("module:")!=std::string::npos)
		{
			this->addPlugin(name, path,freq,blocking, args, instreams, outstreams);
			tempret = string_split_single(line,line.find("module:")+7);
			name = std::get<0>(tempret);
			path="";
			freq=1;
			blocking=-1;
			args.clear();
			instreams.clear();
			outstreams.clear();
			foundline=0;
		}else if(foundline==1 && line.find("blocking=")!=std::string::npos)
		{
			tempret = string_split_single(line,line.find("blocking=")+9);
			blocking = std::stol(std::get<0>(tempret));
		}else if(foundline==1 && line.find("frequency=")!=std::string::npos)
		{
			tempret = string_split_single(line,line.find("frequency=")+10);
			freq = std::stod(std::get<0>(tempret));
		}else if(foundline==1 && line.find("path=")!=std::string::npos)
		{
			tempret = string_split_single(line,line.find("path=")+5);
			path = std::get<0>(tempret);
			if (path=="")
			{
				path=(std::string)"plugins"+_filesysdelimiter+name+_filesysdelimiter+name;
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
		this->addPlugin(name, path,freq,blocking, args, instreams, outstreams);
	}

}
bool pluginmanager::addPlugin(const std::string &name, const std::string &path,  const double &freq, const int64_t &blocking, const std::vector<std::string> &args, const std::vector<std::string> &instreams, const std::vector<std::string> &outstreams)
{
	sgactor *actor = loadActor(path,freq,blocking, args);
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
	for (uint64_t count=1; count<argc; count++)
	{
		try{
			pluman.parsefile(argv[count]);
		}
		catch(std::exception e)
		{
			std::cerr << "Caught exception:" << std::endl;
			std::cerr << e.what() << std::endl;
			return 1;
		}
	}
	std::cout << "simplegraph started, press any key to exit" << std::endl;
	getchar();
	return 0;
}
