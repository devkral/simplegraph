

#include "base.h"

#ifndef TEST1_H
#define TEST1_H



class teststreamspec : public sgraph::sgstreamspec{
public:
	int lol=3;

};
class testactor : public sgraph::sgactor{
public:
	testactor(const double freq, const int64_t blocking);
	~testactor();
	void enter(const std::vector<sgraph::sgstreamspec*> &in, const std::vector<std::string> &out);
	void run(const std::vector<std::shared_ptr<sgraph::sgstream>> in);
	void leave();

};
class testactor2 : public sgraph::sgactor{
public:
	testactor2();
	~testactor2();
	void enter(const std::vector<sgraph::sgstreamspec*> &in,const std::vector<std::string> &out);
	void run(const std::vector<std::shared_ptr<sgraph::sgstream>> in);
	void leave();

};



extern "C" sgraph::sgactor* create_pluginactor(const double freq, const int64_t blocking, const std::vector<std::string> args);


#endif
