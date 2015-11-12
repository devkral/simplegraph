
	
#ifndef SGBASESPECS_H
#define SGBASESPECS_H
#include "base.h"

namespace sgraph{

class stream_image_raw : public sgstream{
public:
	uint8_t *ptr;
	stream_image_raw(uint8_t *ptr) : sgstream()
	{
		this->ptr=ptr;
	}
	~stream_image_raw()
	{
		free(ptr);
	}

};
class spec_image : public sgstreamspec
{
public:
	spec_image(uint64_t x, uint64_t y, uint64_t w, uint64_t h) : sgstreamspec()
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}
	uint64_t x,y,w,h;
	uint8_t hint=0;
};


class stream_text :  public sgstream{
public:
	std::string text;
	stream_text(const std::string &text) : sgstream()
	{
		this->text=text;
	}
};

class spec_text : public sgstreamspec
{
public:
	spec_text() : sgstreamspec(){};

};

}
#endif
