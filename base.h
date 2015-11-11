

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


class StopStreamspec : public std::exception{};

class NameCollisionException: public std::exception{
	std::string reasonstr;
public:
	NameCollisionException(std::string reasonstr) : std::exception()
	{
		this->reasonstr=reasonstr;
	}
	const char* what()
	{
		return reasonstr.c_str();
	}

};

class UninitializedStreamException : public std::exception{
public:
	const char* what() 
	{
		return "Stream uninitialized";
	}
};



class MissingStreamException : public std::exception{
	std::string reasonstr;

public:
	MissingStreamException(std::string name) : std::exception()
	{
		this->reasonstr="Input Stream: \""+name+"\" does not exist";
	}
	
	const char* what()
	{
		return reasonstr.c_str();
	}
};

class MissingActorException : public std::exception{
	std::string reasonstr;

public:
	MissingActorException(std::string name) : std::exception()
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
	void deleteStreamspecs(const std::set<std::string> specnames);
public:
	virtual ~sgmanager(){this->deleteActors();}
	std::vector<sgstreamspec*> getStreamspecs(const std::vector<std::string> &streamnames);
	std::vector<sgactor*> getActors(const std::vector<std::string> &actornames);
	void updateStreamspec(const std::string &name, sgstreamspec* obj);
	void addActor(const std::string &name, sgactor *actor, const std::vector<std::string> &streamnamesin, const std::vector<std::string> &streamnamesout);
	void deleteActors(const std::set<std::string> actornames=std::set<std::string>());
	void pause(const std::set<std::string> actornames=std::set<std::string>());
	void start(const std::set<std::string> actornames=std::set<std::string>());
};

class sgstream{
public:
	virtual ~sgstream(){};

};

class sgstreamspec{
private:
	std::recursive_mutex protaccess;
	bool is_updating=false, is_stopping=false;
	std::condition_variable_any updating_finished, reading_finished;
	std::shared_ptr<sgstream> stream;
	//sgactor *owner;

public:
	virtual ~sgstreamspec(){};
	std::shared_ptr<sgstream> getStream(int64_t blockingtime);
	void updateStream(sgstream* streamob);
	void stop();
	bool stopping(){return this->is_stopping; }
};


class sgactor{
private:
	std::chrono::steady_clock::time_point time_previous;
	sgmanager* manager; // don't delete
	std::timed_mutex time_lock;
	bool is_pausing=false;
	std::mutex pause_lock, stop_lock;
	std::condition_variable_any pause_cond;
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
	// blocking time: -1 wait infinitely for an update, 0 (default) take current element, >0 wait <nanoseconds> for update, return elsewise NULL
	sgactor(double freq=1, int64_t blockingtime=0);
	void stop();
	void start();
	void pause();
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
