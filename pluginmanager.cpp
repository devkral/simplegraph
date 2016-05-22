
#include "pluginmanager.h"

#include <fstream>
#include "basespecs.h"

namespace sgraph{

pluginmanager::pluginmanager(const std::string &callpath)
{
	this->manager = new sgmanager;
	this->callpath = callpath;
}
pluginmanager::~pluginmanager()
{
	//this->manager->deleteActors();
	delete this->manager;
	this->manager = 0;
	std::cout << "simplegraph cleaned up" << std::endl;
}

const std::vector<std::string> pluginmanager::parse_path(const std::vector<std::string> &inp, const std::string &configpath)
{
	std::vector<std::string> out;
	for (std::string elem: inp)
	{
		std::string newelem;
		if (elem.compare(0, 7, "$config")==0)
		{
			newelem=configpath.substr(0, configpath.find_last_of("/\\"))+elem.substr(7);
		}
		else if (elem.compare(0, 5, "$call")==0)
		{
			newelem=this->callpath.substr(0, this->callpath.find_last_of("/\\"))+elem.substr(5);
		}
		else
			newelem=elem;
#if _WIN32|_WIN64
		size_t pos = newelem.find("/", 0);
		while (pos!=std::string::npos)
		{
			newelem = newelem.substr(0, pos)+"\\"+newelem.substr(pos+1, std::string::npos);
			pos = newelem.find("/", pos);
		}
#endif
		std::ifstream stream = std::ifstream();
		stream.open(newelem);
		if (stream.is_open())
		{
			stream.close();
			out.push_back(newelem);
		}
	}
	if (inp.size()==0)
	{
		std::cerr << "Error: no path specified in config" << std::endl;
	}else if (out.size()==0)
	{
		std::cerr << "Error: no valid path. Defined pathes:" << std::endl;
		for (std::string elem: inp)
		{
			std::cerr << "    " << elem << std::endl;
		}
	}
	return out;
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
		}else if (std::get<0>(parsed_line) == "path")
		{
			tmargs["path"] = parse_path(std::get<1>(parsed_line), filepath);
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
	if (tmargs.at("path").size()==0)
	{
		return false;
	}
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
	sgraph::pluginmanager pluman(argv[0]);
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
