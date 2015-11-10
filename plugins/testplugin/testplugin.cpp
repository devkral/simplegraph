
#include "testplugin.h"
#include <iostream>

void testactor::enter(const std::vector<sgraph::sgstreamspec*> &in, const std::vector<std::string> &out)
{
	if (in.size()!=0 || out.size()!=1)
		throw(sgraph::MissingStreamException("?"));
	for (std::string elem : out)
	{
		this->getManager()->updateStream(elem, new teststreamspec());
		std::cout << "mm name:" << elem << std::endl;
	}
}
//testactor::testactor(){};
testactor::testactor(const double freq, const int64_t blocking) : sgactor(freq, blocking)
{

};
testactor::~testactor() // : sgraph::sgactor::~sgactor()
{
	
};

void testactor::run(std::vector<std::shared_ptr<sgraph::sgstream>> in)
{


}
void testactor::leave()
{


}


void testactor2::enter(const std::vector<sgraph::sgstreamspec*> &in, const std::vector<std::string> &out)
{
	if (in.size()!=1 || out.size()!=1)
		throw(std::exception());
	for (sgraph::sgstreamspec* elem : in)
	{
		//std::cout << "in name:" << elem->getName() << std::endl;
	}
	for (std::string elem : out)
	{
		this->getManager()->updateStream(elem, new teststreamspec());
		std::cout << "out name:" << elem << std::endl;
	}
}

testactor2::testactor2() : sgactor()
{

};
testactor2::~testactor2() // : sgraph::sgactor::~sgactor()
{
	
};

void testactor2::run(std::vector<std::shared_ptr<sgraph::sgstream>> in)
{


}
void testactor2::leave()
{


}


sgraph::sgactor *create_pluginactor(const double freq, const int64_t blocking, const std::vector<std::string> args)
{
	//testactor(freq, blocking);
	return static_cast<sgraph::sgactor*>(new testactor(freq, blocking));

}

