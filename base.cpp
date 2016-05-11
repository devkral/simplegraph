
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

const std::vector<sgraph::sgstreamspec*> sgmanager::getStreamspecs(const std::vector<std::string> &streamnames)
{
	std::vector<sgstreamspec*> ret(streamnames.size());
	for (size_t count=0; count<streamnames.size(); count++)
	{
		try
		{
			ret[count]=this->streamdict.at(streamnames[count]).get();
		}catch(std::out_of_range)
		{
			throw(sgraphException("Stream: \""+streamnames[count]+"\" is missing"));
		}
	}
	return ret;
}

std::vector<sgactor*> sgmanager::getActors(const std::vector<std::string> &actornames)
{
	std::vector<sgactor*> ret(actornames.size());
	for (size_t count=0; count<actornames.size(); count++)
	{
		try
		{
			ret[count]=this->actordict.at(actornames[count]).get();
		}catch(std::out_of_range)
		{
			throw(sgraphException("Actor: \""+actornames[count]+"\" is missing"));
		}
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
		if (this->updating_finished.wait_for(this->protaccess, sgtimeunit(blockingtime)) == std::cv_status::timeout)
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
	this->global_time_previous=std::chrono::steady_clock::now()-this->time_sleep*this->threads;
	this->time_lock.lock();
	if (freq>0)
	{
		this->time_sleep = sgtimeunit((int64_t)((double)sgtimeunit_second/freq));
	}else if (freq==0) // special case to ensure real zero
	{
		this->time_sleep = sgtimeunit(0);
	}else
	{
		this->time_sleep = sgtimeunit((int64_t)(-1.0L*freq));
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
	this->global_time_previous=std::chrono::steady_clock::now()-this->time_sleep*this->threads;
	this->adaptcount=0;
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
	while (this->intern_threads.size()>0)
	{
		try
		{
			this->intern_threads.back().join();
		} catch (...)
		{
			//do nothing
		}

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

const std::vector<std::shared_ptr<sgstream>> sgactor::getStreams(bool do_block)
{
	std::vector<std::shared_ptr<sgstream>> _tretgetStreams(this->streamsin.size());
	std::vector<std::future<std::shared_ptr<sgstream>>> _thandlesgetStreams(this->streamsin.size());
	// shortcut when no input stream is available
	if (this->streamsin.size()>0)
	{
		for (size_t count=0; count<this->streamsin.size(); count++)
		{
			if (this->streamsin[count]==0)
			{
				throw(sgraphStreamException("stream in: \""+this->getName()+"\" is uninitialized"));
				// stops when using deleted stream
				//throw StopStreamspec(); // stops when using deleted stream
			}
			if (do_block==true)
			{
				_thandlesgetStreams[count]=std::async(std::launch::async, getStreamhelper, this->streamsin[count], -1);
			}else
			{
				_thandlesgetStreams[count]=std::async(std::launch::async, getStreamhelper, this->streamsin[count], this->blockingtime);
			}
		}

		for (size_t count=0; count<_thandlesgetStreams.size(); count++)
		{
			_thandlesgetStreams[count].wait();
			_tretgetStreams[count]=_thandlesgetStreams[count].get();
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
void sgactor::step(uint32_t threadid){
	//this->active=this->manager->interrupt_thread(this);
	uint32_t threads_temp;
	this->pause_lock.lock();
	if (this->is_pausing)
	{
		this->pause_cond.wait(this->pause_lock);
	}
	threads_temp=this->threads;
	this->pause_lock.unlock();
	if (!this->active)
	{
		return;
	}
	auto tstart =  std::chrono::steady_clock::now();
	// subtract difference from slot from sleep time
	// if slot is missed, parallelize if not fixed
	// (sleep time)-(past time since last time_previous)
	//auto tosleep = this->time_sleep*threads_temp-(tstart-time_previous-threadid*this->time_sleep);
	auto tosleep = this->time_sleep*(threads_temp+threadid)-(tstart-this->global_time_previous);
	if (tosleep > sgtimeunit(0))
	{
		if (this->time_lock.try_lock_for(std::chrono::duration_cast<sgtimeunit> (tosleep)))
		{
			return;
		}
	}
	else if (this->parallelize <= 0 && this->time_sleep > sgtimeunit(0))
	{
		this->pause_lock.lock();
		// calculate, check that integer does not overflow
		if (this->threads*-2>-2147483648 && (this->parallelize == 0 || this->parallelize>-this->threads*2))
		{
			this->start_new_thread();
			threads_temp++;
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
		std::cerr <<  e.what() << std::endl;
		this->stop();
		return;
	}
	if (threadid==0)
	{
		auto tend =  std::chrono::steady_clock::now();
		this->pause_lock.lock();
		if (this->adaptcount<1 && tend-tstart>this->time_sleep)
		{
			this->global_time_previous = tend-this->time_sleep/2;
			this->adaptcount++;

		}else
		{
			this->global_time_previous = tend;
		}
		this->pause_lock.unlock();
	}
	//this->transform(sgactor, std::forward(sgactor));
}

void sgactor::init(const std::string &name, sgmanager *manager, const std::vector<std::string> &streamnamesin, const std::vector<std::string> &streamnamesout)
{
	this->name = name;
	this->manager = manager;
	this->owned_instreams.insert(streamnamesin.begin(), streamnamesin.end());
	this->owned_outstreams.insert(streamnamesout.begin(), streamnamesout.end());
	this->streamsin = manager->getStreamspecs(streamnamesin);
	this->enter(this->streamsin, streamnamesout);
	this->init_threads();
	this->streamsout = manager->getStreamspecs(streamnamesout);
}

}
