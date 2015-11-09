
#include "test.h"
#include <iostream>

void testactor::enter(const std::vector<sgraph::sgstreamspec*> &in, const std::vector<std::string> &out)
{
	if (in.size()!=0 || out.size()!=1)
		throw(std::exception());
	for (std::string elem : out)
	{
		this->getManager()->updateStream(elem, new teststreamspec());
		std::cout << "mm name:" << elem << std::endl;
	}
}

testactor::testactor() : sgactor()
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


int main()
{
	
	sgraph::sgmanager man;
	
	std::vector<std::string> ac1streamsin;
	std::vector<std::string> ac1streamsout;
	ac1streamsout.push_back("stream1");
	std::vector<std::string> ac2streamsin;
	std::vector<std::string> ac2streamsout;
	ac2streamsin.push_back("stream1");
	ac2streamsout.push_back("stream2");
	man.addActor("actor1",static_cast<sgraph::sgactor*>(new testactor), ac1streamsin, ac1streamsout);
	man.addActor("actor2",static_cast<sgraph::sgactor*>(new testactor2), ac2streamsin, ac2streamsout);
	
	std::vector<std::string> actorsretrieve;
	actorsretrieve.push_back("actor1");
	actorsretrieve.push_back("actor2");
	for (sgraph::sgactor* actor : man.getActors(actorsretrieve))
	{
		std::cout << "Mega:" << actor->getName() << std::endl;
	
	}
	
	
	
	
}
