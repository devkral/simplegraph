
	
#ifndef SGBASESPECS_H
#define SGBASESPECS_H


#include "base.h"

//#include <cstdlib>

namespace sgraph{

class stream_data : public sgstream{
public:
	uint8_t *ptr;
	size_t size;
	stream_data(uint8_t *ptr, size_t size) : sgstream()
	{
		this->ptr=ptr;
		this->size=size;
	}
	~stream_data()
	{
		free(this->ptr);
	}

};
class spec_image : public sgstreamspec
{
public:
	spec_image() : sgstreamspec()
	{
		this->capabilities.insert("spec_image");
	}
	uint64_t width,height;
	uint8_t channels;
};


class stream_pos : public sgstream{
public:
	int64_t x, y;
	stream_pos(int64_t x, int64_t y) : sgstream()
	{
		this->x=x;
		this->y=y;
	}
};

class spec_pos : public sgstreamspec
{
public:
	spec_pos() : sgstreamspec()
	{
		this->capabilities.insert("spec_pos");
	}

};


class stream_text :  public sgstream{
public:
	std::string text;
	stream_text(const std::string &text) : sgstream()
	{
		this->text=text;
	}
};

class spec_text : public sgstreamspec{
public:
	spec_text() : sgstreamspec(){
		this->capabilities.insert("spec_text");
	};

};

class stream_log : public stream_text{
public:
	int8_t loglevel;
	stream_log(const std::string &text, int8_t loglevel) : stream_text(text)
	{
		this->text=text;
		this->loglevel=loglevel;
	}
};

class spec_log : public spec_text{
public:
	spec_log() : spec_text(){
		this->capabilities.insert("spec_log");
	}

};

class debugactor : public sgactor{
private:
	uint8_t loglevel;
public:
	
	debugactor(uint8_t loglevel);
	void enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out);
	void run(const sginstreams in);
	void leave(){}
};

}
#endif
