

#include <iostream>

#include "basespecs.h"


namespace sgraph{



debugactor::debugactor(uint8_t loglevel) : sgactor(0,-1) {
	this->loglevel=loglevel;
}
void debugactor::enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out)
{
	if (out.size()>0)
	{
		throw(sgraphException("outstreams are not allowed"));
	}
	/**for (sgstreamspec *elem: in)
	{
		if (elem==0)
		{
			throw(sgraphStreamException("stream"));
		}
	}*/ // getStreams should protect already
}
void debugactor::run(const std::vector<std::shared_ptr<sgstream>> in)
{
	unsigned int count=0;
	for (std::shared_ptr<sgstream> elem: in)
	{
		stream_log* logob=static_cast<stream_log*>(elem.get());
		if (logob->loglevel==0){
			std::cout << "Message: Stream " << count << ": " << logob->text << std::endl;
		}
		else if (logob->loglevel<=this->loglevel){
			std::cerr << "Error: Stream " << count << ": " << logob->text << std::endl;
		}
		count++;
	}
}


}
