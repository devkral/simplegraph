
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

sgmanager::~sgmanager()
{
	this->deleteActors();
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
		if (!it.second->active())
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
	// check if is stopping after lock acquired
	if (!this->active())
	{
		this->protaccess.unlock();
		//this->updating_finished.notify_all();
		return;
	}
	this->stream = std::shared_ptr<sgstream> (streamob);
	this->last_time = std::chrono::steady_clock::now();
	this->is_uninitialized = false;
	this->protaccess.unlock();
	this->updating_finished.notify_all();
}
void sgstreamspec::stop()
{
	this->is_active = false;
	this->is_uninitialized = true;
	this->updating_finished.notify_all();
}

std::shared_ptr<sgstream> sgstreamspec::getStream(const int64_t &blockingtime, const sgtimeunit &mintimediff)
{
	std::shared_ptr<sgstream> ret=std::shared_ptr<sgstream>(0);
	this->protaccess.lock();
	// check if is stopping after lock acquired
	if (!this->active())
	{
		this->protaccess.unlock();
		throw StopStreamspec();
	}
	// if below mintime
	auto temp = std::chrono::steady_clock::now();
	if (this->is_uninitialized || this->last_time+mintimediff < temp)
	{
		if (blockingtime == -1)
		{
			this->updating_finished.wait(this->protaccess);
		}else if (blockingtime > 0)
		{
			if (this->updating_finished.wait_for(this->protaccess, sgtimeunit(blockingtime)) == std::cv_status::timeout)
			{
				this->protaccess.unlock();
				return ret;
			}
		}
	}
	if (!this->active())
	{
		this->protaccess.unlock();
		throw StopStreamspec();
	}
	else
	{
		ret = this->stream;
	}
	
	this->protaccess.unlock();
	return ret;
}
/////////////////////////////////////////////////////////////////////////////////

sgactor::sgactor(const double &freq, const int64_t &blockingtime, const int32_t &parallelize, const uint32_t &samples)
{
	this->blockingtime = blockingtime;
	this->parallelize = parallelize;
	// minimum is 1 sample
	this->samples = samples==0 ? 1 : samples;
	this->global_time_previous=std::chrono::steady_clock::now()-this->time_sleep*this->threads;
	
	if (freq > 0)
	{
		this->time_sleep = sgtimeunit((int64_t)((double)sgtimeunit_second/freq));
	}else if (freq == 0) // special case to ensure real zero
	{
		this->time_sleep = sgtimeunit(0);
	}else
	{
		this->time_sleep = sgtimeunit((int64_t)(-1.0L*freq));
	}
}

void sgactor::pause()
{
	this->sync_lock.lock();
	this->is_pausing = true;
	this->sync_lock.unlock();
}
void sgactor::start()
{
	this->global_time_previous=std::chrono::steady_clock::now()-this->time_sleep*this->threads;
	this->sync_lock.lock();
	this->is_pausing = false;
	this->sync_lock.unlock();
	this->pause_cond.notify_all(); // multiple threads possible
}
void sgactor::stop()
{
	// call stop only once
	if(!this->stop_lock.try_lock())
		return;
	this->is_active = false;
	this->is_pausing = false;
	// sync
	this->sync_lock.lock();
	this->sync_lock.unlock();
	// unlock
	this->pause_cond.notify_all();
	for (size_t count=0; count < this->intern_threads_locks.size(); count++)
	{
		this->intern_threads_locks[count]->unlock();
	}
	
	// stop threads
	while (this->intern_threads.size() > 0)
	{
		try
		{
			this->intern_threads.back().join();

		} catch (...)
		{
			//do nothing
		}
		this->intern_threads.pop_back();
		//delete this->intern_threads_locks.back();
		this->intern_threads_locks.pop_back();
	}
	// then execute a last action
	this->leave();
	// now cleanup streamsout
	for (sgstreamspec* elem: this->streamsout)
	{
		if (elem!=0)
		{
			elem->stop();
		}
		else
		{
			std::cerr << "\"" << this->getName() << "\" has an uninitialized stream" << std::endl;
		}
	}
}


std::shared_ptr<sgstream> getStreamhelper(sgstreamspec *t, const int64_t &blockingtime, const sgtimeunit &mintimediff)
{
	return t->getStream(blockingtime, mintimediff);
}

const sginstreams sgactor::getStreams(const uint32_t &threadid, const sgtimeunit &mintimediff)
{
	sginstreams _tretgetStreams(this->streamsin.size(), sginstream(this->samples));
	std::vector<std::future<std::shared_ptr<sgstream>>> _thandlesgetStreams(this->streamsin.size());
	// shortcut when no input stream is available
	if (this->streamsin.size()>0)
	{
		for (size_t count_sample=0; count_sample<this->samples; count_sample++)
		{
			for (size_t count_stream=0; count_stream<this->streamsin.size(); count_stream++)
			{
				if (this->streamsin[count_stream]==0)
				{
					throw(sgraphStreamException("stream in: \""+this->getName()+"\" is uninitialized"));
					// stops when using deleted stream
					//throw StopStreamspec(); // stops when using deleted stream
				}
				_thandlesgetStreams[count_stream]=std::async(std::launch::async, getStreamhelper, this->streamsin[count_stream], this->blockingtime, mintimediff);
			}

			for (size_t count_stream=0; count_stream<_thandlesgetStreams.size(); count_stream++)
			{
				_thandlesgetStreams[count_stream].wait();
				_tretgetStreams[count_stream][count_sample]=_thandlesgetStreams[count_stream].get();
			}
			
			// if last don't block
			if (count_sample < this->samples-1)
			{
				// wait
				if (this->intern_threads_locks[threadid]->try_lock_for(std::chrono::duration_cast<sgtimeunit> (this->time_sleep)))
				{
					// stop run (only call of getStreams)
					throw(StopStreamspec());
				}
			}
		}
	}
	return _tretgetStreams;
}

void sgactor::start_new_thread()
{
	this->threads += 1;
	this->intern_threads_locks.push_back(std::make_shared<std::timed_mutex>());
	this->intern_threads_locks.back()->lock();
	this->intern_threads.push_back(std::thread(sgraph::sgactor::thread_wrapper, this, this->threads-1));
}

void sgactor::init_threads()
{
	this->sync_lock.lock();
	this->threads = 0;
	this->start_new_thread();
	int32_t parallelize_temp = this->parallelize;
	if (parallelize_temp < 0)
	{
		parallelize_temp = -parallelize_temp;
	}
	for (int32_t count=1;count<parallelize_temp; count++)
	{
		if (!this->active())
			break;
		this->start_new_thread();
	}
	this->sync_lock.unlock();
}

void sgactor::step(uint32_t threadid){
	uint32_t threads_temp;
	this->sync_lock.lock();
	if (this->is_pausing)
	{
		this->pause_cond.wait(this->sync_lock);
	}
	threads_temp = this->threads;
	this->sync_lock.unlock();
	// check if is active
	if (!this->active())
	{
		return;
	}
	auto tstart = std::chrono::steady_clock::now();
	// subtract difference from slot from sleep time
	// if slot is missed, parallelize if not fixed
	// (sleep time)-(past time since last time_previous)
	// auto tosleep = this->time_sleep*threads_temp-(tstart-time_previous-threadid*this->time_sleep);
	auto tempcalc = this->time_sleep*(threads_temp+threadid);
	auto tosleep = this->global_time_previous+tempcalc-tstart;
	if (tosleep > sgtimeunit(0))
	{
		if (this->intern_threads_locks[threadid]->try_lock_for(std::chrono::duration_cast<sgtimeunit> (tosleep)))
		{
			return;
		}
	}else if (this->time_sleep <= sgtimeunit(0))
	{
		std::this_thread::yield();
	}
	try{
		this->run(this->getStreams(threadid, this->time_sleep));
	}catch(StopStreamspec &e)
	{
		this->stop();
		return;
	}catch(sgraphStreamException &e)
	{
		std::cerr << e.what() << std::endl;
		this->stop();
		return;
	}
	auto tend = std::chrono::steady_clock::now();
	// should be positive
	auto overtimecalc = tend-(tstart+this->time_sleep*threads_temp*this->samples);
	if (overtimecalc>sgtimeunit(0))
	{
		//std::cerr << "over time: " << this->getName() << " : " << overtimecalc.count() << std::endl;
		if (this->parallelize <= 0)
		{
			this->sync_lock.lock();
			// check if is active
			if (!this->active())
			{
				this->sync_lock.unlock();
				return;
			}
			// calculate, failsafe
			// check that number of threads is less or equal to parallelize
			if (this->threads>sgactor_max_autothreads && (this->parallelize==0 || -this->parallelize<=this->threads))
			{
				this->start_new_thread();
			}
			this->sync_lock.unlock();
		}
	}
		
	if (threadid==0 && this->time_sleep>sgtimeunit(0))
	{
		this->sync_lock.lock();
		this->global_time_previous = tend;
		this->sync_lock.unlock();
	}
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
