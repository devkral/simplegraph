
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
	
	std::map<std::string, std::vector<std::string>> tmargs;
	
	std::tuple<std::string, std::vector<std::string>> parsed_line;
	if (stream.is_open())
	{
		std::cout << "\"" << filepath << "\" loaded" << std::endl;
	}
	
	while (stream.is_open() && stream.eof()==false)
	{
		std::getline(stream, line);
		parsed_line = parse_line(line);
		if (std::get<0>(parsed_line).length()==0 && std::get<1>(parsed_line).size()==0)
			continue;
		if(foundline>=1 && (std::get<0>(parsed_line) == "debug" || std::get<0>(parsed_line) == "module"))
		{
			//path = (std::string)"plugins"+_filesysdelimiter+name+_filesysdelimiter+"lib"+name+_libraryending;
			
			if (foundline==1)
			{
				this->addPlugin(tmargs);
			}
			else
			{
				sgactor *dgactor=new debugactor(atoi(default_value_map(tmargs, "loglevel", "0")[0].c_str()));
				this->manager->addActor(default_value_map(tmargs, "name", "debug").at(0),
					dgactor,
					default_value_map(tmargs, "instreams"),
					default_value_map(tmargs, "outstreams"));
			}
		
			tmargs.clear();
			foundline=0;
		}
		
		if (std::get<0>(parsed_line) == "module")
		{
			tmargs["name"] = std::get<1>(parsed_line);
			foundline=1;
		}else if (std::get<0>(parsed_line) == "debug")
		{
			tmargs["name"] = std::get<1>(parsed_line);
			foundline=2;
		}else
		{
			tmargs[std::get<0>(parsed_line)] = std::get<1>(parsed_line);
		}
	}
	stream.close();
	// add last module, which isn't terminated by module:
	if (foundline==1)
	{
		this->addPlugin(tmargs);
	}
	if (foundline==2)
	{
		sgactor *dgactor=new debugactor(atoi(default_value_map(tmargs, "loglevel", "0")[0].c_str()));
		this->manager->addActor(default_value_map(tmargs, "name", "debug").at(0),
			dgactor,
			default_value_map(tmargs, "instreams"),
			default_value_map(tmargs, "outstreams"));
	}
}
bool pluginmanager::addPlugin(const std::map<std::string, std::vector<std::string>> tmargs)
{
	sgactor *actor = loadActor(tmargs.at("path")[0], tmargs);
	if (actor == 0)
	{
		return false;
	}
	this->manager->addActor(tmargs.at("name")[0],
		actor,
		default_value_map(tmargs, "instreams"),
		default_value_map(tmargs, "outstreams"));
	return true;
}

void pluginmanager::start()
{
	this->manager->start();
}
void pluginmanager::pause()
{
	this->manager->pause();
}

} // end sgraph

int main(int argc, char *argv[])
{
	sgraph::pluginmanager pluman;
	for (uint64_t count=1; count<argc; count++)
	{
		try{
			pluman.parsefile(argv[count]);
		}
		catch(sgraph::sgraphException &e)
		{
			std::cerr << "Caught exception:" << std::endl;
			std::cerr << e.what() << std::endl;
			return 1;
		}
	}
	std::cout << "starting simplegraph" << std::endl;
	try{
		pluman.start();
	}
	catch(sgraph::sgraphException &e)
	{
		std::cerr << "Caught exception:" << std::endl;
		std::cerr << e.what() << std::endl;
		return 1;
	}
	std::cout << "simplegraph started, press any key to exit" << std::endl;
	getchar();
	return 0;
}
