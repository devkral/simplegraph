

#ifndef SGBASE_H
#define SGBASE_H

#include <condition_variable>
#include <map>
#include <mutex>
#include <vector>
#include <memory>
//#include <thread>
#include <future>
//#include <initializer_list>
#include <cstdint>
#include <chrono>
#include <string>
#include <set>


namespace sgraph{


class sgstream;
class sgstreamspec;
class sgactor;
class sgmanager;

class NameCollisionException : std::exception{
	std::string reasonstr;
public:
	NameCollisionException(std::string reasonstr)
	{
		this->reasonstr=reasonstr;
	}
	const char* what()
	{
		return reasonstr.c_str();
	}

};

class UninitializedStreamException : std::exception{
public:
	const char* what()
	{
		return "Stream uninitialized";
	}
};



class MissingStreamException : std::exception{
	std::string reasonstr;

public:
	MissingStreamException(std::string name)
	{
		this->reasonstr="Input Stream: \""+name+"\" does not exist";
	}
	
	const char* what()
	{
		return reasonstr.c_str();
	}
};

class MissingActorException : std::exception{
	std::string reasonstr;

public:
	MissingActorException(std::string name)
	{
		this->reasonstr="Actor: \""+name+"\" does not exist";
	}
	
	const char* what()
	{
		return reasonstr.c_str();
	}
};



class sgmanager{
private:
	std::map<std::string, std::shared_ptr<sgactor>> actordict;
	std::map<std::string, std::shared_ptr<sgstreamspec>> streamdict;
	//mutex
	std::condition_variable _pause_threads_finished;

protected:
	bool active=true;
	bool pause_threads = false;
	std::mutex pause_threads_lock;
public:
	virtual ~sgmanager(){this->stop();}
	bool interrupt_thread(sgactor *actor);
	std::vector<sgstreamspec*> getStreamspecs(const std::vector<std::string> &streamnames);
	std::vector<sgactor*> getActors(const std::vector<std::string> &actornames);
	void updateStreamspec(const std::string &name, sgstreamspec* obj);
	void addActor(const std::string &name, sgactor *actor, const std::vector<std::string> &streamnamesin, const std::vector<std::string> &streamnamesout);
	//void removeActor(const std::string &name);
	void pause();
	void stop();
	void start();
};

class sgstream{
public:
	virtual ~sgstream(){};

};

class sgstreamspec{
private:
	std::mutex protaccess;
	bool is_updating=false;
	uint64_t interests=0;
	std::mutex protinterests;
	std::mutex finish_lock;
	std::condition_variable updating_finished, reading_finished;
	std::shared_ptr<sgstream> stream;
	//sgactor *owner;

public:
	virtual ~sgstreamspec(){};
	std::shared_ptr<sgstream> getStream(int64_t blockingtime);
	void updateStream(sgstream* streamob);
};


class sgactor{
private:
	std::chrono::steady_clock::time_point time_previous;
	sgmanager* manager; // don't delete
	std::vector<sgstreamspec*> streamsin;
	std::vector<std::shared_ptr<sgstream>> _tretgetStreams;
	std::vector<std::future<std::shared_ptr<sgstream>>> _thandlesgetStreams;
protected:
	std::thread *intern_thread=0;
	std::vector<sgstreamspec*> streamsout;

	static void thread_wrapper(sgactor *t){
		while (t->active == true)
		{
			t->step();
		}
	}
	int64_t blockingtime;
	std::chrono::nanoseconds time_sleep;
	std::vector<std::shared_ptr<sgstream>> getStreams();
	std::string name;
	
	std::set<std::string> owned_instreams;
	std::set<std::string> owned_outstreams;

public:
	virtual ~sgactor(){};
	sgactor(double freq=1, int64_t blockingtime=-1);
	void join();
	inline const std::string getName(){return this->name;}
	inline sgmanager* getManager(){return this->manager;}
	inline const std::set<std::string> getInstreams(){return this->owned_instreams;}
	inline const std::set<std::string> getOutstreams(){return this->owned_outstreams;}
	//sgactor(bool blocking=true){this->blocking};
	bool active=true;
	void init(const std::string &name, sgmanager *manager, const std::vector<std::string> &streamnamesin, const std::vector<std::string> &streamnamesout);
	virtual void enter(const std::vector<sgstreamspec*> &in,const std::vector<std::string> &out)=0;
	virtual void run(const std::vector<std::shared_ptr<sgstream>> in)=0;
	virtual void leave(){};
	void step();

};

}
#endif
