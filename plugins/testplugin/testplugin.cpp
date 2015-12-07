
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
	this->intern_thread=new std::thread(sgraph::sgactor::thread_wrapper, this);
	
}

void testprovider::run(std::vector<std::shared_ptr<sgraph::sgstream>> in)
{
	streamsout[0]->updateStream(new teststream(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count()));

}
void testprovider::leave()
{

}


void testtransformer::enter(const std::vector<sgraph::sgstreamspec*> &in, const std::vector<std::string> &out)
{
	std::cout << "transformer consumes:" << *this->getInstreams().begin() << std::endl;
	if (in.size()!=1 || (out.size()!=2 && out.size()!=1 ))
		throw(sgraph::MissingStreamException("?"));
	std::cout << "transformer inits:" << out[0] << std::endl;
	this->getManager()->updateStreamspec(out[0], new teststreamspec());

	
	if (out.size()==2)
	{
		this->getManager()->updateStreamspec(out[1], new sgraph::spec_log());
		std::cout << "debug inits:" << out[1] << std::endl;
		
	}
	std::cout << "Name: " << this->getName() << std::endl;
	this->intern_thread=new std::thread(sgraph::sgactor::thread_wrapper, this);
}

void testtransformer::run(std::vector<std::shared_ptr<sgraph::sgstream>> in)
{
	teststream* temp = (teststream*)in[0].get();
	if (temp!=0)
	{
		streamsout[0]->updateStream(new teststream(temp->testout));
		if (streamsout.size()==2)
			streamsout[1]->updateStream(new sgraph::stream_log("test",1));
	}

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
	this->intern_thread=new std::thread(sgraph::sgactor::thread_wrapper, this);
}

void testconsumer::run(std::vector<std::shared_ptr<sgraph::sgstream>> in)
{
	if (in[0]!=0)
		std::cout << ((teststream*)in[0].get())->testout << std::endl;
	else
		std::cout << "NULL" << std::endl;
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

