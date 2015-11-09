
#ifndef TEST1_H
#define TEST1_H

#include "../../base.h"

class teststreamspec : public sgraph::sgstreamspec{
public:
	int lol=3;

};
class testactor : public sgraph::sgactor{
public:
	testactor();
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



#endif
