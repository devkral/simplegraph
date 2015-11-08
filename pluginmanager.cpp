
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
	int16_t foundline=0;
	std::string name, path;
	std::vector<std::string> args, instreams, outstreams;
	std::string tempargs;
	int64_t found=0,pos=0;
	while (stream.eof()==false)
	{
		std::getline(stream, line);
		if (foundline==0 && line.find("name:")!=std::string::npos)
		{
			name = line.substr(line.find("name:")+5);
			foundline=1;
		} else if(foundline==1 && line.find("name:")!=std::string::npos)
		{
			this->addPlugin(name, path, args, instreams, outstreams);
			name=line.substr(line.find("name:")+5);
			path="";
			args.clear();
			instreams.clear();
			outstreams.clear();
			foundline=0;
		}else if(foundline==1 && line.find("args=")!=std::string::npos)
		{
			tempargs=line.substr(line.find("args=")+5);
			pos=0;
			while (tempargs.find(",",pos)!=std::string::npos)
			{
				found=tempargs.find(",",pos);
				if (tempargs.substr(std::max(found-1,0l),1)=="\\")
				{
					pos=tempargs.find(",",pos)+1;
				}else
				{
					args.push_back(tempargs.substr(0,found-1));
					tempargs=tempargs.substr(0,found+1);
					pos=0;
				}
			}
		}else if(foundline==1 && line.find("instreams=")!=std::string::npos)
		{
			tempargs=line.substr(line.find("instreams=")+10);
			pos=0;
			while (tempargs.find(",",pos)!=std::string::npos)
			{
				found=tempargs.find(",",pos);
				if (tempargs.substr(std::max(found-1,0l),1)== "\\")
				{
					pos=tempargs.find(",",pos)+1;
				}else
				{
					instreams.push_back(tempargs.substr(0,found-1));
					tempargs=tempargs.substr(0,found+1);
					pos=0;
				}
			}
		}else if(foundline==1 && line.find("outstreams=")!=std::string::npos)
		{
			tempargs=line.substr(line.find("outstreams=")+11);
			pos=0;
			while (tempargs.find(",",pos)!=std::string::npos)
			{
				found=tempargs.find(",",pos);
				if (tempargs.substr(std::max(found-1,0l),1)=="\\")
				{
					pos=tempargs.find(",",pos)+1;
				}else
				{
					outstreams.push_back(tempargs.substr(0,found-1));
					tempargs=tempargs.substr(0,found+1);
					pos=0;
				}
			}
		}else if(foundline==1 && line.find("path=")!=std::string::npos)
		{
			path= line.substr(line.find("path=")+5);
		}
		
		
	}
	stream.close();

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
