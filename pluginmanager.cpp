
#include "pluginmanager.h"

#include <fstream>

namespace sgraph{

pluginmanager::pluginmanager()
{
	this->manager = new sgmanager;

}
pluginmanager::~pluginmanager()
{
	//this->manager->deleteActors();
	delete this->manager;
	this->manager = 0;
	std::cout << "simplegraph cleaned up" << std::endl;
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
	bool foundline=false;
	double freq=1.0;
	int64_t blocking=0;
	size_t limitpos;
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
		limitpos = find_comment(line);
		if (foundline==false && line.find("module:")<limitpos)
		{
			tempret = string_split_single(line,line.find("module:")+7,limitpos);
			name = std::get<0>(tempret);
			foundline=true;
		} else if(foundline==true && line.find("module:")<limitpos)
		{
			if (path!="")
			{
				//path = (std::string)"plugins"+_filesysdelimiter+name+_filesysdelimiter+"lib"+name+_libraryending;
			
				this->addPlugin(path, path,freq,blocking, args, instreams, outstreams);
			}
			tempret = string_split_single(line,line.find("module:")+7,limitpos);
			name = std::get<0>(tempret);
			path="";
			freq=0;
			blocking=-1;
			args.clear();
			instreams.clear();
			outstreams.clear();
			foundline=true;
		}else if(foundline==true && line.find("blocking=")<limitpos)
		{
			tempret = string_split_single(line,line.find("blocking=")+9,limitpos);
			std::cout << "\"" << std::get<0>(tempret)  << "\"" << std::endl;
			blocking = std::stol(std::get<0>(tempret));
		}else if(foundline==true && line.find("frequency=")<limitpos)
		{
			tempret = string_split_single(line,line.find("frequency=")+10,limitpos);
			freq = std::stod(std::get<0>(tempret));
		}else if(foundline==true && line.find("path=")<limitpos)
		{
			tempret = string_split_single(line,line.find("path=")+5,limitpos);
			path = std::get<0>(tempret);
		}else if(foundline==true && line.find("args=")<limitpos)
		{
			args = string_split_multiple(line, line.find("args=")+5,limitpos);
		}else if(foundline==true && line.find("instreams=")<limitpos)
		{
			instreams = string_split_multiple(line, line.find("instreams=")+10,limitpos);
		}else if(foundline==true && line.find("outstreams=")<limitpos)
		{
			outstreams = string_split_multiple(line, line.find("outstreams=")+11,limitpos);
		}
		
		
	}
	stream.close();
	// add last module, which isn't terminated by module:
	if (foundline==true)
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
		catch(std::exception &e)
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
