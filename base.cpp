
#include "base.h"

namespace sgraph{

void sgmanager::pause()
{
	this->pause_threads_lock.lock();
	this->pause_threads = true;
	this->pause_threads_lock.unlock();
	//for (std::tuple<>)
	
}

void sgmanager::interrupt_thread()
{
	std::unique_lock<std::mutex> lck (this->pause_threads_lock);
	
	if (this->pause_threads)
	{
		this->_pause_threads_finished.wait(lck);
	}
}

void sgmanager::updateStream(const std::string &name, sgstreamspec* obj)
{
	this->streamdict.at(name).reset(obj);
}

std::vector<sgraph::sgstreamspec*> sgmanager::getStreamspecs(const std::vector<std::string> &streamnames)
{
	std::vector<sgstreamspec*> ret;
	for (std::string elem: streamnames)
	{
		if (this->streamdict.count(elem)==0)
		{
			throw(MissingStreamException(elem));
		}
		ret.push_back(this->streamdict.at(elem).get());
	}
	return ret;
}

std::vector<sgactor*> sgmanager::getActors(const std::vector<std::string> &actornames)
{
	std::vector<sgactor*> ret;
	for (std::string elem: actornames)
	{
		if (this->actordict.count(elem)==0)
		{
			throw(MissingActorException(elem));
		}
		ret.push_back(this->actordict.at(elem).get());
	}
	return ret;

}

void sgmanager::addActor(const std::string &name, sgactor *actor, const std::vector<std::string> &streamnamesin, const std::vector<std::string> &streamnamesout)
{
	for (std::string elem: streamnamesin)
	{
		if (this->streamdict.count(elem)==0)
		{
			throw(MissingStreamException(elem));
		}
	}
	
	for (std::string elem: streamnamesout)
	{
		if (this->streamdict.count(elem)==1)
		{
			throw(NameCollisionException("Outputstream: \""+elem+"\" already exists"));
		}
		this->streamdict[elem] = std::shared_ptr<sgstreamspec>(0); //must be initialised in enter
	}
	
	if (this->actordict.count(name)==1)
	{
		throw(NameCollisionException("Actor: \""+name+"\" already exists"));
	}
	actor->init(name, this, streamnamesin, streamnamesout);
	this->actordict[name] = std::shared_ptr<sgactor>(actor);
}




void sgstreamspec::updateStream(sgstream* streamob)
{
	
	std::unique_lock<std::mutex> lck (this->finish_lock);
	this->updating_val = true;
	if (this->reading_val)
	{
		this->reading_finished.wait(lck);
	}
	this->stream = std::shared_ptr<sgstream> (streamob);
	this->protinterests.lock();
	this->updating_val = false;
	if (this->interests>0)
	{
		this->reading_val=true;
	}
	this->protinterests.unlock();
	updating_finished.notify_all();
}

std::shared_ptr<sgstream> sgstreamspec::getStream(int64_t blockingtime)
{
	std::shared_ptr<sgstream> ret;
	this->protinterests.lock();
	this->interests += 1;
	this->protinterests.unlock();
	
	std::unique_lock<std::mutex> lck (this->finish_lock);
	bool success = this->updating_val==false;
	
	if (!success)
	{
		if (blockingtime==-1)
		{
			this->updating_finished.wait(lck);
			success = true;
		} else if (blockingtime==0)
		{
			lck.unlock();
		}
		else
		{
			success = (this->updating_finished.wait_for(lck, std::chrono::nanoseconds(blockingtime))==std::cv_status::no_timeout);
		}
	}
	
	
	if (!success)
	{
		ret = std::shared_ptr<sgstream>(0);
	}
	else
	{
		ret = this->stream;
	}
	
	this->protinterests.lock();
	this->interests -= 1;
	if (this->interests==0)
	{
		this->reading_val=false;
		this->reading_finished.notify_all();
	}
	this->protinterests.unlock();
	return ret;
}



sgactor::sgactor(double freq, int64_t blockingtime)
{
	this->blockingtime = blockingtime;
	this->time_sleep = std::chrono::nanoseconds((uint64_t)(1000000/freq));
	this->time_previous = std::chrono::steady_clock::now();
}



std::shared_ptr<sgstream> getStreamhelper(sgstreamspec *t, int64_t blockingtime)
{
	return t->getStream(blockingtime);

}


std::vector<std::shared_ptr<sgstream>> sgactor::getStreams()
{
	std::vector<std::shared_ptr<sgstream>> ret;
	std::vector<std::future<std::shared_ptr<sgstream>>> handles;
	//std::vector<auto> handles;
	for (sgstreamspec* elem: this->streamsin)
	{
		if (elem==0)
		{
			throw(UninitializedStreamException());
		}
		handles.push_back(std::async(std::launch::async, getStreamhelper, elem, this->blockingtime));
	}
	
	for (std::future<std::shared_ptr<sgstream>> &elem: handles)
	{
		elem.wait();
		ret.push_back(elem.get());
	}
	return ret;
}


void sgactor::step(){
	this->manager->interrupt_thread();
	auto tstart =  std::chrono::steady_clock::now();
	auto tosleep = this->time_sleep-(tstart-this->time_previous);
	std::this_thread::sleep_for(std::chrono::duration_cast<std::chrono::nanoseconds> (tosleep));

	this->run(this->getStreams());

	this->time_previous = std::chrono::steady_clock::now();
	//this->transform(sgactor, std::forward(sgactor));
};

void sgactor::init(const std::string &name, sgmanager *manager, const std::vector<std::string> &streamnamesin, const std::vector<std::string> &streamnamesout)
{
	this->name = name;
	this->manager = manager;
	this->owned_instreams=streamnamesin;
	this->owned_outstreams=streamnamesout;
	this->streamsin = manager->getStreamspecs(streamnamesin);
	this->enter(this->streamsin, streamnamesout);
	this->streamsout = manager->getStreamspecs(streamnamesout);
}




}