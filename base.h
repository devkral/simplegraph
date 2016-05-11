

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
#include <iostream>

namespace sgraph{

extern const std::vector<std::string> default_value_map(const std::map<std::string, std::vector<std::string>> &ma, const std::string key, const std::vector<std::string> _default=std::vector<std::string>());

extern const std::vector<std::string> default_value_map(const std::map<std::string, std::vector<std::string>> &ma, const std::string key, const std::string _default);

typedef std::chrono::steady_clock::time_point sgactor_time_point;
typedef std::chrono::nanoseconds sgtimeunit;
const int64_t sgtimeunit_second=1000000000L;

class sgstream;
class sgstreamspec;
class sgactor;
class sgmanager;




class StopStreamspec : public std::exception{};



class sgraphException : public std::exception{
private:
	std::string reasonstr;

public:
	sgraphException(std::string reason) : std::exception()
	{
		this->reasonstr=""+reason;
	}
	virtual ~sgraphException(){}

	const char* what()
	{
		return reasonstr.c_str();
	}
};

class sgraphStreamException : public sgraphException
{
public:
	sgraphStreamException(std::string reason) : sgraphException(reason){}

};

class sgmanager{
private:
	std::map<std::string, std::shared_ptr<sgactor>> actordict;
	std::map<std::string, std::shared_ptr<sgstreamspec>> streamdict;

protected:
	void deleteStreamspecs(const std::set<std::string> specnames);
public:
	virtual ~sgmanager(){this->deleteActors();}
	const std::vector<sgstreamspec*> getStreamspecs(const std::vector<std::string> &streamnames);
	std::vector<sgactor*> getActors(const std::vector<std::string> &actornames);
	void updateStreamspec(const std::string &name, sgstreamspec* obj);
	void addActor(const std::string &name, sgactor *actor, const std::vector<std::string> &streamnamesin, const std::vector<std::string> &streamnamesout);
	void deleteActors(const std::set<std::string> actornames=std::set<std::string>());
	void cleanupActors();
	void pause(const std::set<std::string> actornames=std::set<std::string>());
	void start(const std::set<std::string> actornames=std::set<std::string>());
};

class sgstream{
public:
	virtual ~sgstream(){}

};

class sgstreamspec{
private:
	std::recursive_mutex protaccess;
	bool is_updating=false, is_stopping=false;
	std::condition_variable_any updating_finished;
	std::shared_ptr<sgstream> stream;
	//sgactor *owner;
public:
	std::set<std::string> capabilities;
	virtual ~sgstreamspec(){}
	std::shared_ptr<sgstream> getStream(int64_t blockingtime);
	void updateStream(sgstream* streamob);
	void stop();
	bool stopping(){return this->is_stopping;}
};


class sgactor{
private:
	sgmanager* manager; // don't delete
	std::timed_mutex time_lock;
	sgactor_time_point global_time_previous;
	bool is_pausing=true; // start paused
	int32_t parallelize=1; // 0 adapt, >0 fix amount, <0 set start amount and limit to the double of start amount
	uint32_t threads=0; // thread count
	int32_t adaptcount=0;
	std::mutex pause_lock, stop_lock;
	std::condition_variable_any pause_cond;
	std::vector<sgstreamspec*> streamsin;
protected:
	std::vector<std::thread> intern_threads;
	std::vector<sgstreamspec*> streamsout;

	static void thread_wrapper(sgactor *t, uint32_t threadid){
		while (t->active == true)
		{
			t->step(threadid);
		}
	}
	virtual void init_threads();
	virtual void start_new_thread();
	int64_t blockingtime;
	sgtimeunit time_sleep;
	const std::vector<std::shared_ptr<sgstream>> getStreams(bool do_block=false);
	std::string name;
	
	std::set<std::string> owned_instreams;
	std::set<std::string> owned_outstreams;

public:
	virtual ~sgactor(){}
	// blocking time: -1 wait infinitely for an update, 0 (default) take current element, >0 wait <sgtimeunit> for update, return elsewise NULL
	// parallelize
	sgactor(const double freq, const int64_t blockingtime, const int32_t parallelize);
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
	virtual void leave(){}
	void step(uint32_t threadid=0);
};

}
#endif
