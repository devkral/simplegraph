
#include "pluginmanager.h"



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

}
bool pluginmanager::addPlugin(const std::string name, const std::string path, const std::vector<std::string> args)
{
	sgactor *actor = loadActor(path, args);
	if (actor == 0)
	{
	
		return false;
	}
	this->manager->addActor(name, )
	return true;
}


int main(int argc, char *argv[])
{
	for (uint64_t count=0; count<argc; count++)

}
