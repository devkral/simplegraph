
#include "base.h"

//#include <iostream>
namespace sgraph{


void sgmanager::start(const std::set<std::string> actornames)
{
	for (auto it: this->actordict)
	{
		if (actornames.size()==0 || actornames.count(it.first)!=0)
		{
			it.second->start();
		}
	}
}

void sgmanager::pause(const std::set<std::string> actornames)
{
	for (auto it: this->actordict)
	{
		if (actornames.size()==0 || actornames.count(it.first)!=0)
		{
			it.second->pause();
		}
	}
}



void sgmanager::updateStreamspec(const std::string &name, sgstreamspec* obj)
{
	this->streamdict.at(name).reset(obj);
}

std::vector<sgraph::sgstreamspec*> sgmanager::getStreamspecs(const std::vector<std::string> &streamnames)
{
	std::vector<sgstreamspec*> ret;
	ret.reserve(streamnames.size());
	for (std::string elem: streamnames)
	{
		if (this->streamdict.count(elem)==0)
		{
			throw(MissingStreamException(elem));
		}
		ret.emplace_back(this->streamdict.at(elem).get());
	}
	return ret;
}

std::vector<sgactor*> sgmanager::getActors(const std::vector<std::string> &actornames)
{
	std::vector<sgactor*> ret;
	ret.reserve(actornames.size());
	for (std::string elem: actornames)
	{
		if (this->actordict.count(elem)==0)
		{
			throw(MissingActorException(elem));
		}
		ret.emplace_back(this->actordict.at(elem).get());
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

void sgmanager::deleteStreamspecs(const std::set<std::string> specnames)
{
	for (auto it: this->streamdict)
	{
		if (specnames.size()==0 || specnames.count(it.first)!=0)
		{
			it.second->stop();
			this->streamdict.erase(it.first);
		}
	}
}

void sgmanager::deleteActors(const std::set<std::string> actornames)
{
	for (auto it: this->actordict)
	{
		if (actornames.size()==0 || actornames.count(it.first)!=0)
		{
			it.second->stop();
		}
	}
	this->cleanupActors();
}

void sgmanager::cleanupActors()
{
	for (auto it: this->actordict)
	{
		if (!it.second->active)
		{
			this->deleteStreamspecs(it.second->getOutstreams());
			this->actordict.erase(it.first);
		}
	}

}


/////////////////////////////////////////////////////////////

void sgstreamspec::updateStream(sgstream* streamob)
{
	
	this->protaccess.lock();
	if (this->is_stopping)
	{
		this->protaccess.unlock();
		updating_finished.notify_all();
		return;
	}
	this->stream = std::shared_ptr<sgstream> (streamob);
	this->protaccess.unlock();
	updating_finished.notify_all();
	
}
void sgstreamspec::stop()
{
	this->is_stopping=true;
	this->protaccess.unlock();
	updating_finished.notify_all();
}

std::shared_ptr<sgstream> sgstreamspec::getStream(int64_t blockingtime)
{
	std::shared_ptr<sgstream> ret;
	if (this->is_stopping)
	{
		throw StopStreamspec();
		//return std::shared_ptr<sgstream> (0);
	}
	this->protaccess.lock();
	//std::unique_lock<std::recursive_mutex> lck(this->protaccess, std::adopt_lock);
	if (blockingtime==-1)
	{
		this->updating_finished.wait(this->protaccess);
	}else if (blockingtime>0)
	{
		if (this->updating_finished.wait_for(this->protaccess, std::chrono::nanoseconds(blockingtime)) == std::cv_status::timeout)
		{
			this->protaccess.unlock();
			return std::shared_ptr<sgstream> (0);
		}
	}
	if (this->is_stopping)
	{
		//this->protaccess.unlock();
		throw StopStreamspec();
		//ret = std::shared_ptr<sgstream> (0);
	}
	else
	{
		ret = this->stream;
	}
	
	this->protaccess.unlock();
	return ret;
}



sgactor::sgactor(double freq, int64_t blockingtime)
{
	this->blockingtime = blockingtime;
	this->time_lock.lock();
	if (freq>0)
	{
		this->time_sleep = std::chrono::nanoseconds((int64_t)(1000000000.0L/freq));
	}
	else
	{
		this->time_sleep = std::chrono::nanoseconds((int64_t)(-1.0L*freq));
	}
	this->time_previous = std::chrono::steady_clock::now();
}

void sgactor::pause()
{
	this->pause_lock.lock();
	this->is_pausing = true;
	this->pause_lock.unlock();
	//for (std::tuple<>)
}
void sgactor::start()
{
	this->pause_lock.lock();
	this->is_pausing = false;
	this->pause_lock.unlock();
	this->pause_cond.notify_one(); // should be only one
}
void sgactor::stop()
{
	if(this->stop_lock.try_lock()==false)
		return;
	this->active=false;
	this->time_lock.unlock();
	this->pause_cond.notify_all();
	if (this->intern_thread)
	{
		this->intern_thread->join();
		delete this->intern_thread;
	}
	this->leave();
	for (sgstreamspec* elem: this->streamsout)
	{
		elem->stop();
	}
}


std::shared_ptr<sgstream> getStreamhelper(sgstreamspec *t, int64_t blockingtime)
{
	return t->getStream(blockingtime);
}

std::vector<std::shared_ptr<sgstream>> sgactor::getStreams(bool do_block)
{
	this->_tretgetStreams.clear();
	this->_thandlesgetStreams.clear();
	for (sgstreamspec* elem: this->streamsin)
	{
		if (elem==0)
		{
			throw UninitializedStreamException(); // stops when using deleted stream
			//throw StopStreamspec(); // stops when using deleted stream
		}
		if (do_block==true)
		{
			_thandlesgetStreams.emplace_back(std::async(std::launch::async, getStreamhelper, elem, -1));
		}else
		{
			_thandlesgetStreams.emplace_back(std::async(std::launch::async, getStreamhelper, elem, this->blockingtime));
		}
	}
	
	for (std::future<std::shared_ptr<sgstream>> &elem: _thandlesgetStreams)
	{
		elem.wait();
		_tretgetStreams.emplace_back(elem.get());
	}
	return _tretgetStreams;
}


void sgactor::step(){
	//this->active=this->manager->interrupt_thread(this);
	
	this->pause_lock.lock();
	if (this->is_pausing)
	{
		this->pause_cond.wait(this->pause_lock);
	}
	this->pause_lock.unlock();
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
	try{
		this->run(this->getStreams());
	}catch(StopStreamspec &e)
	{
		this->stop();
		return;
	}catch(UninitializedStreamException &e)
	{
		//std::cerr << "Stream not initialized, stop actor:" << std::endl;
		//std::cerr << " " << e.what() << std::endl;
		this->stop();
		return;
	}
	
	this->time_previous = std::chrono::steady_clock::now();
	//this->transform(sgactor, std::forward(sgactor));
};

void sgactor::init(const std::string &name, sgmanager *manager, const std::vector<std::string> &streamnamesin, const std::vector<std::string> &streamnamesout)
{
	this->name = name;
	this->manager = manager;
	this->owned_instreams.insert(streamnamesin.begin(), streamnamesin.end());
	this->owned_outstreams.insert(streamnamesout.begin(), streamnamesout.end());
	this->streamsin = manager->getStreamspecs(streamnamesin);
	this->_tretgetStreams = std::vector<std::shared_ptr<sgstream>> (this->streamsin.size());
	this->_thandlesgetStreams = std::vector<std::future<std::shared_ptr<sgstream>>> (this->streamsin.size());
	this->enter(this->streamsin, streamnamesout);
	this->streamsout = manager->getStreamspecs(streamnamesout);
}




}
