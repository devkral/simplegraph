



#ifndef TESTPLUGIN1_H
#define TESTPLUGIN1_H

#include "base.h"
#include "basespecs.h"


#include <iostream>


class teststream : public sgraph::sgstream{
public:
	teststream(uint64_t in){testout=in;}
	uint64_t testout=0;

};

class teststreamspec : public sgraph::sgstreamspec{
public:
	int testvariable=0;

};
class testprovider : public sgraph::sgactor{
public:
	testprovider(const double freq, const int64_t blocking, const int32_t parallelize, const uint32_t samples): sgactor(freq, blocking, parallelize, 1) {}
	//~testprovider();
	void enter(const std::vector<sgraph::sgstreamspec*> &in, const std::vector<std::string> &out);
	void run(const sgraph::sginstreams in);
	void leave();

};

class testtransformer : public sgraph::sgactor{
public:
	testtransformer(const double freq, const int64_t blocking, const int32_t parallelize, const uint32_t samples): sgactor(freq, blocking, parallelize, samples) {}
	//~testtransformer();
	void enter(const std::vector<sgraph::sgstreamspec*> &in, const std::vector<std::string> &out);
	void run(const sgraph::sginstreams in);
	void leave();

};


class testconsumer : public sgraph::sgactor{
public:
	testconsumer(const double freq, const int64_t blocking, const int32_t parallelize, const uint32_t samples): sgactor(freq, blocking, parallelize, samples) {}
	//~testconsumer();
	void enter(const std::vector<sgraph::sgstreamspec*> &in,const std::vector<std::string> &out);
	void run(const sgraph::sginstreams in);
	void leave();

};

extern "C" sgraph::sgactor* create_pluginactor(const std::map<std::string,std::vector<std::string>> args);


#endif
