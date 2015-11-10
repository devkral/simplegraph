
#include "testplugin.h"
#include <iostream>

void testprovider::enter(const std::vector<sgraph::sgstreamspec*> &in, const std::vector<std::string> &out)
{
	if (in.size()!=0 || out.size()!=1)
		throw(sgraph::MissingStreamException("?"));
	for (std::string elem : out)
	{
		this->getManager()->updateStream(elem, new teststreamspec());
	}
}

void testprovider::run(std::vector<std::shared_ptr<sgraph::sgstream>> in)
{
	streamsout[0]->updateStream(new teststream(3));

}
void testprovider::leave()
{

}


void testtransformer::enter(const std::vector<sgraph::sgstreamspec*> &in, const std::vector<std::string> &out)
{
	if (in.size()!=1 || out.size()!=1)
		throw(sgraph::MissingStreamException("?"));
	for (std::string elem : out)
	{
		this->getManager()->updateStream(elem, new teststreamspec());
	}
}

void testtransformer::run(std::vector<std::shared_ptr<sgraph::sgstream>> in)
{
	streamsout[0]->updateStream(new teststream(((teststream*)in[0].get())->testout));

}
void testtransformer::leave()
{

}


void testconsumer::enter(const std::vector<sgraph::sgstreamspec*> &in, const std::vector<std::string> &out)
{
	if (in.size()!=1 || out.size()!=0)
		throw(sgraph::MissingStreamException("?"));
}

void testconsumer::run(std::vector<std::shared_ptr<sgraph::sgstream>> in)
{
	std::cout << ((teststream*)in[0].get())->testout << std::endl;

}
void testconsumer::leave()
{

}



sgraph::sgactor *create_pluginactor(const double freq, const int64_t blocking, const std::vector<std::string> args)
{

	if (args[0]=="provider")
	{
		return static_cast<sgraph::sgactor*>(new testprovider(freq, blocking));
	}else if (args[0]=="transformer")
	{
		return static_cast<sgraph::sgactor*>(new testtransformer(freq, blocking));
	}else if (args[0]=="consumer")
	{
		return static_cast<sgraph::sgactor*>(new testconsumer(freq, blocking));
	}
	else return 0;
}

