



#ifndef TESTPLUGIN1_H
#define TESTPLUGIN1_H

#include "base.h"

#include <iostream>


class teststream : public sgraph::sgstream{
public:
	teststream(int in){testout=in;}
	int testout=0;

};

class teststreamspec : public sgraph::sgstreamspec{
public:
	int testvariable=0;

};
class testprovider : public sgraph::sgactor{
public:
	testprovider(const double freq, const int64_t blocking): sgactor(freq, blocking) {}
	//~testprovider();
	void enter(const std::vector<sgraph::sgstreamspec*> &in, const std::vector<std::string> &out);
	void run(const std::vector<std::shared_ptr<sgraph::sgstream>> in);
	void leave();

};

class testtransformer : public sgraph::sgactor{
public:
	testtransformer(const double freq, const int64_t blocking): sgactor(freq, blocking) {}
	//~testtransformer();
	void enter(const std::vector<sgraph::sgstreamspec*> &in, const std::vector<std::string> &out);
	void run(const std::vector<std::shared_ptr<sgraph::sgstream>> in);
	void leave();

};


class testconsumer : public sgraph::sgactor{
public:
	testconsumer(const double freq, const int64_t blocking): sgactor(freq, blocking) {}
	//~testconsumer();
	void enter(const std::vector<sgraph::sgstreamspec*> &in,const std::vector<std::string> &out);
	void run(const std::vector<std::shared_ptr<sgraph::sgstream>> in);
	void leave();

};

extern "C" sgraph::sgactor* create_pluginactor(const double freq, const int64_t blocking, const std::vector<std::string> args);


#endif
