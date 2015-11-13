
#include "pluginmanager.h"

#include <fstream>
#include "basespecs.h"

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
	uint8_t foundline=0;
	uint8_t loglevel=0;
	double freq=1.0;
	int64_t blocking=0;
	bool debug_initialized=false;
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
		if (foundline==0 && line.find("module:")<limitpos)
		{
			tempret = string_split_single(line,line.find("module:")+7,limitpos);
			name = std::get<0>(tempret);
			foundline=1;
		}else if (foundline==0 && line.find("debug:")<limitpos && debug_initialized==false)
		{
			//name = "debug";
			foundline=2;
		}else if(foundline==1 && line.find("debug:")<limitpos)
		{
			if (path!="")
			{
				//path = (std::string)"plugins"+_filesysdelimiter+name+_filesysdelimiter+"lib"+name+_libraryending;
			
				this->addPlugin(name, path,freq,blocking, args, instreams, outstreams);
			}
			path="";
			freq=1.0;
			blocking=-1;
			args.clear();
			instreams.clear();
			outstreams.clear();
			foundline=2;
		
		}
		else if(foundline>=1 && line.find("module:")<limitpos)
		{
			if (path!="")
			{
				//path = (std::string)"plugins"+_filesysdelimiter+name+_filesysdelimiter+"lib"+name+_libraryending;
			
				this->addPlugin(name, path,freq,blocking, args, instreams, outstreams);
			}else if(foundline==2)
			{
				sgactor *dgactor=new debugactor(loglevel);
				this->manager->addActor("debug", dgactor,instreams, outstreams);
			}
			tempret = string_split_single(line,line.find("module:")+7,limitpos);
			name = std::get<0>(tempret);
			path="";
			freq=1.0;
			blocking=-1;
			args.clear();
			instreams.clear();
			outstreams.clear();
			foundline=1;
		}else if(foundline==1 && line.find("blocking=")<limitpos)
		{
			tempret = string_split_single(line,line.find("blocking=")+9,limitpos);
			std::cout << "\"" << std::get<0>(tempret)  << "\"" << std::endl;
			blocking = std::stol(std::get<0>(tempret));
		}else if(foundline==1 && line.find("frequency=")<limitpos)
		{
			tempret = string_split_single(line,line.find("frequency=")+10,limitpos);
			freq = std::stod(std::get<0>(tempret));
		}else if(foundline==1 && line.find("path=")<limitpos)
		{
			tempret = string_split_single(line,line.find("path=")+5,limitpos);
			path = std::get<0>(tempret);
		}else if(foundline==1 && line.find("args=")<limitpos)
		{
			args = string_split_multiple(line, line.find("args=")+5,limitpos);
		}else if(foundline>=1 && line.find("instreams=")<limitpos)
		{
			instreams = string_split_multiple(line, line.find("instreams=")+10,limitpos);
		}else if(foundline==1 && line.find("outstreams=")<limitpos)
		{
			outstreams = string_split_multiple(line, line.find("outstreams=")+11,limitpos);
		}else if(foundline==2 && line.find("loglevel=")<limitpos)
		{
			tempret = string_split_single(line,line.find("loglevel=")+9,limitpos);
			loglevel = std::stoi(std::get<0>(tempret));
		}

		
		
	}
	stream.close();
	// add last module, which isn't terminated by module:
	if (foundline==1)
	{
		this->addPlugin(name, path,freq,blocking, args, instreams, outstreams);
	}
	if (foundline==2 && debug_initialized==false)
	{
		sgactor *dgactor=new debugactor(loglevel);
		this->manager->addActor("debug", dgactor,instreams, outstreams);
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
