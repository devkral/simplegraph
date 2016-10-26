
#include "testplugin.h"
#include <iostream>

void testprovider::enter(const std::vector<sgraph::sgstreamspec*> &in, const std::vector<std::string> &out)
{
	if (in.size()!=0 || out.size()!=1)
		throw(sgraph::sgraphStreamException("invalid amount of in- or outstreams"));
	for (std::string elem : out)
	{
		std::cout << "Provider inits:" << elem << std::endl;
		this->getManager()->updateStreamspec(elem, new teststreamspec());
	}
	std::cout << "Name: " << this->getName() << std::endl;
}

void testprovider::run(const sgraph::sginstreams in)
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
	{
		std::cerr << "transformer " << in.size() << " " << out.size() << std::endl;
		throw(sgraph::sgraphStreamException("invalid amount of in- or outstreams"));
	}
	std::cout << "transformer inits:" << out[0] << std::endl;
	this->getManager()->updateStreamspec(out[0], new teststreamspec());

	
	if (out.size()==2)
	{
		this->getManager()->updateStreamspec(out[1], new sgraph::spec_log());
		std::cout << "debug inits:" << out[1] << std::endl;
		
	}
	std::cout << "Name: " << this->getName() << std::endl;
}

void testtransformer::run(const sgraph::sginstreams in)
{
	teststream* temp = (teststream*)in[0][0].get();
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
		throw(sgraph::sgraphStreamException("invalid amount of in- or outstreams"));
	
	std::cout << "Name: " << this->getName() << std::endl;
}

void testconsumer::run(const sgraph::sginstreams in)
{
	teststream* temp = 0;
	for (uint32_t count_sample=0; count_sample<in[0].size(); count_sample++)
	{
		temp = (teststream*)in[0][count_sample].get();
		if (temp!=0)
			std::cout << "sample: " << count_sample << ": " << temp->testout << std::endl;
		else
			std::cout << "sample: " << count_sample << ": " << "NULL" << std::endl;
	}
}
void testconsumer::leave()
{

}



sgraph::sgactor *create_pluginactor(const std::map<std::string,std::vector<std::string>> args)
{
	std::string type = sgraph::default_value_map(args, "type", "")[0];
	double freq = stod(sgraph::default_value_map(args, "freq", "1")[0]);
	int64_t blocking = stoi(sgraph::default_value_map(args, "blocking", "-1")[0]);
	int32_t parallelize = stoi(sgraph::default_value_map(args, "parallelize", "1")[0]);
	uint32_t samples = stoi(sgraph::default_value_map(args, "samples", "1")[0]);
	
	if (type=="provider")
	{
		return static_cast<sgraph::sgactor*>(new testprovider(freq, blocking, parallelize, samples));
	}else if (type=="transformer")
	{
		return static_cast<sgraph::sgactor*>(new testtransformer(freq, blocking, parallelize, samples));
	}else if (type=="consumer")
	{
		return static_cast<sgraph::sgactor*>(new testconsumer(freq, blocking, parallelize, samples));
	}
	else return 0;
}

