
#include "base.h"

#include <iostream>
namespace sgraph{
const std::vector<std::string> default_value_map(const std::map<std::string,std::vector<std::string>> &ma, const std::string key, const std::vector<std::string> _default)
{
	try{
		return ma.at(key);
	}catch (const std::out_of_range& oor) {
		return _default;
	}
};


const std::vector<std::string> default_value_map(const std::map<std::string,std::vector<std::string>> &ma, const std::string key, const std::string _default)
{
	return default_value_map(ma, key, std::vector<std::string>({_default}));
}


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
			throw(sgraphException("Stream: \""+elem+"\" is missing"));
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
			throw(sgraphException("Actor: \""+elem+"\" is missing"));
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
			throw(sgraphException("Inputstream: \""+elem+"\" is uninitialized"));
		}
	}
	
	for (std::string elem: streamnamesout)
	{
		if (this->streamdict.count(elem)==1)
		{
			throw(sgraphException("Outputstream: \""+elem+"\" already exists"));
		}
		this->streamdict[elem] = std::shared_ptr<sgstreamspec>(0); //must be initialised in enter
	}
	
	if (this->actordict.count(name)==1)
	{

		throw(sgraphException("Actor: \""+name+"\" exists already"));
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
			if (it.second!=0)
			{
				it.second->stop();
			}
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



sgactor::sgactor(const double freq, const int64_t blockingtime, const int32_t parallelize)
{
	this->blockingtime = blockingtime;
	this->parallelize = parallelize;
	this->time_lock.lock();
	if (freq>0)
	{
		this->time_sleep = std::chrono::nanoseconds((int64_t)(1000000000.0L/freq));
	}else if (freq==0) // special case to ensure real zero
	{
		this->time_sleep = std::chrono::nanoseconds(0);
	}else
	{
		this->time_sleep = std::chrono::nanoseconds((int64_t)(-1.0L*freq));
	}
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
	this->pause_cond.notify_all(); // multiple threads possible
}
void sgactor::stop()
{
	if(this->stop_lock.try_lock()==false)
		return;
	this->active=false;
	this->time_lock.unlock();
	this->pause_cond.notify_all();
	if (this->intern_threads.size()>0)
	{
		this->intern_threads.back().join();
		this->intern_threads.pop_back();
	}
	this->leave();
	for (sgstreamspec* elem: this->streamsout)
	{
		if (elem!=0)
		{
			elem->stop();
		}
		else
		{
			std::cerr << "\"" << this->getName() << "\" has an unintialized stream" << std::endl;
		}
	}
}


std::shared_ptr<sgstream> getStreamhelper(sgstreamspec *t, int64_t blockingtime)
{
	return t->getStream(blockingtime);
}

std::vector<std::shared_ptr<sgstream>> sgactor::getStreams(bool do_block)
{
	// shortcut when no input stream is available
	if (this->streamsin.size()>0)
	{
		this->_tretgetStreams.clear();
		this->_thandlesgetStreams.clear();
		for (sgstreamspec* elem: this->streamsin)
		{
			if (elem==0)
			{
				throw(sgraphStreamException("stream in: \""+this->getName()+"\" is uninitialized"));
				// stops when using deleted stream
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
	}
	return _tretgetStreams;
}

void sgactor::start_new_thread()
{
	this->threads+=1;
	this->intern_threads.push_back(std::thread(sgraph::sgactor::thread_wrapper, this, this->threads-1));
}

void sgactor::init_threads()
{
	this->threads=0;
	this->start_new_thread();
	int32_t parallelize_temp=this->parallelize;
	if (parallelize_temp<0)
	{
		parallelize_temp = -parallelize_temp;
	}
	for (int32_t count=1;count<parallelize_temp; count++)
	{
		this->start_new_thread();
	}
}

// time_previous will be changed
void sgactor::step(sgactor_time_point &time_previous, uint32_t threadid){
	//this->active=this->manager->interrupt_thread(this);
	int32_t threads_temp;
	this->pause_lock.lock();
	if (this->is_pausing)
	{
		this->pause_cond.wait(this->pause_lock);
		time_previous=std::chrono::steady_clock::now()+this->time_sleep*threadid;
	}
	threads_temp = this->threads;
	this->pause_lock.unlock();
	if (!this->active)
	{
		return;
	}
	auto tstart =  std::chrono::steady_clock::now();
	auto tosleep = this->time_sleep*threads_temp-(tstart-time_previous);
	if (tosleep>std::chrono::nanoseconds(0))
	{
		if (this->time_lock.try_lock_for(std::chrono::duration_cast<std::chrono::nanoseconds> (tosleep)))
		{
			return;
		}
	}
	else if (this->parallelize <= 0 && this->time_sleep > std::chrono::nanoseconds(0))
	{
		this->pause_lock.lock();
		// calculate, check that integer does not overflow
		if (this->threads*-2>-2147483648 && (this->parallelize == 0 || this->parallelize>-this->threads*2))
		{
			this->start_new_thread();
		}
		this->pause_lock.unlock();
	}
	try{
		this->run(this->getStreams());
	}catch(StopStreamspec &e)
	{
		this->stop();
		return;
	}catch(sgraphStreamException &e)
	{
		//std::cerr << "Stream not initialized, stop actor:" << std::endl;
		std::cerr << " " << e.what() << std::endl;
		this->stop();
		return;
	}
	
	time_previous=std::chrono::steady_clock::now();
	//this->transform(sgactor, std::forward(sgactor));
}

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
	this->init_threads();
	this->streamsout = manager->getStreamspecs(streamnamesout);
}

}
