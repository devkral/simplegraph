
#include "testplugin.h"
#include <iostream>

void testprovider::enter(const std::vector<sgraph::sgstreamspec*> &in, const std::vector<std::string> &out)
{
	if (in.size()!=0 || out.size()!=1)
		throw(sgraph::MissingStreamException("?"));
	for (std::string elem : out)
	{
		std::cout << "Provider inits:" << elem << std::endl;
		this->getManager()->updateStreamspec(elem, new teststreamspec());
	}
	std::cout << "Name: " << this->getName() << std::endl;
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
	std::cout << "transformer consumes:" << *this->getInstreams().begin() << std::endl;
	if (in.size()!=1 || out.size()!=1)
		throw(sgraph::MissingStreamException("?"));
	for (std::string elem : out)
	{	
		std::cout << "transformer inits:" << elem << std::endl;
		this->getManager()->updateStreamspec(elem, new teststreamspec());
	}
	std::cout << "Name: " << this->getName() << std::endl;
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
	
	std::cout << "consumer consumes:" << *this->getInstreams().begin() << std::endl;
	if (in.size()!=1 || out.size()!=0)
		throw(sgraph::MissingStreamException("?"));
		
	std::cout << "Name: " << this->getName() << std::endl;
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

