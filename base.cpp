
#include "base.h"

namespace sgraph{

void sgmanager::pause()
{
	this->pause_threads_lock.lock();
	this->pause_threads = true;
	this->pause_threads_lock.unlock();
	//for (std::tuple<>)
}
void sgmanager::start()
{
	this->pause_threads_lock.lock();
	this->pause_threads = false;
	this->pause_threads_lock.unlock();
	this->_pause_threads_finished.notify_all();
	//for (std::tuple<>)
}

void sgmanager::stop()
{
	this->pause_threads_lock.lock();
	this->pause_threads = false;
	this->active=false;
	this->pause_threads_lock.unlock();
	this->_pause_threads_finished.notify_all();
	
	for (auto it: this->actordict)
	{
		it.second->stop();
	}
}

bool sgmanager::interrupt_thread(sgactor *actor)
{
	std::unique_lock<std::mutex> lck (this->pause_threads_lock);
	
	if (this->pause_threads)
	{
		this->_pause_threads_finished.wait(lck);
	}
	return this->active;
}

void sgmanager::updateStreamspec(const std::string &name, sgstreamspec* obj)
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
	this->protaccess.lock();
	/**if (interests>0)
	{
		std::unique_lock<std::recursive_mutex> lck(this->protaccess, std::adopt_lock);
		this->reading_finished.wait(lck);
	}*/
	
	if (this->is_stopping)
	{
		return;
	}
	this->stream = std::shared_ptr<sgstream> (streamob);
	
	updating_finished.notify_all();
	this->protaccess.unlock();
}
void sgstreamspec::stop()
{
	this->is_stopping=true;
	this->protaccess.unlock();
}

std::shared_ptr<sgstream> sgstreamspec::getStream(int64_t blockingtime)
{
	std::shared_ptr<sgstream> ret;
	
	this->protaccess.lock();
	//std::unique_lock<std::recursive_mutex> lck(this->protaccess, std::adopt_lock);
	if (blockingtime==-1)
	{
		this->updating_finished.wait(this->protaccess);
	}else if (blockingtime>0)
	{
		if (this->updating_finished.wait_for(this->protaccess, std::chrono::nanoseconds(blockingtime)) == std::cv_status::timeout)
		{
			return std::shared_ptr<sgstream> (0);
		}
	}
	if (this->is_stopping)
	{
		return std::shared_ptr<sgstream> (0);
	}
	ret = this->stream;
	
	this->reading_finished.notify_one();
	
	this->protaccess.unlock();
	return ret;
}



sgactor::sgactor(double freq, int64_t blockingtime)
{
	this->blockingtime = blockingtime;
	this->time_lock.lock();
	this->time_sleep = std::chrono::nanoseconds((int64_t)(1000000000.0L/freq));
	this->time_previous = std::chrono::steady_clock::now();
}

void sgactor::stop()
{
	this->active=false;
	this->time_lock.unlock();
	this->leave();

	if (this->intern_thread)
	{
		this->intern_thread->join();
		delete this->intern_thread;
	}
}

std::shared_ptr<sgstream> getStreamhelper(sgstreamspec *t, int64_t blockingtime)
{
	return t->getStream(blockingtime);

}


std::vector<std::shared_ptr<sgstream>> sgactor::getStreams()
{
	
	this->_tretgetStreams.clear();
	this->_thandlesgetStreams.clear();
	//std::vector<auto> handles;
	
	for (sgstreamspec* elem: this->streamsin)
	{
		if (elem==0)
		{
			throw(UninitializedStreamException());
		}
		_thandlesgetStreams.push_back(std::async(std::launch::async, getStreamhelper, elem, this->blockingtime));
	}
	
	for (std::future<std::shared_ptr<sgstream>> &elem: _thandlesgetStreams)
	{
		elem.wait();
		_tretgetStreams.push_back(elem.get());
	}
	return _tretgetStreams;
}


void sgactor::step(){
	this->active=this->manager->interrupt_thread(this);
	if (!this->active)
	{
		return;
	}
	auto tstart =  std::chrono::steady_clock::now();
	auto tosleep = this->time_sleep-(tstart-this->time_previous);
	if (this->time_lock.try_lock_for(std::chrono::duration_cast<std::chrono::nanoseconds> (tosleep)))
	{
		return;
	}

	this->run(this->getStreams());

	this->time_previous = std::chrono::steady_clock::now();
	//this->transform(sgactor, std::forward(sgactor));
};

void sgactor::init(const std::string &name, sgmanager *manager, const std::vector<std::string> &streamnamesin, const std::vector<std::string> &streamnamesout)
{
	this->name = name;
	this->manager = manager;
	this->owned_instreams.insert(streamnamesin.begin(),streamnamesin.end());
	this->owned_outstreams.insert(streamnamesout.begin(),streamnamesout.end());
	this->streamsin = manager->getStreamspecs(streamnamesin);
	this->_tretgetStreams = std::vector<std::shared_ptr<sgstream>> (this->streamsin.size());
	this->_thandlesgetStreams = std::vector<std::future<std::shared_ptr<sgstream>>> (this->streamsin.size());
	this->enter(this->streamsin, streamnamesout);
	this->streamsout = manager->getStreamspecs(streamnamesout);
}




}
