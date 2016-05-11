
#include "testplugin.h"


int main()
{
	
	sgraph::sgmanager man;
	
	std::vector<std::string> ac1streamsin;
	std::vector<std::string> ac1streamsout;
	ac1streamsout.push_back("stream1");
	std::vector<std::string> ac2streamsin;
	std::vector<std::string> ac2streamsout;
	ac2streamsin.push_back("stream1");
	ac2streamsout.push_back("stream2");
	std::vector<std::string> ac3streamsin;
	std::vector<std::string> ac3streamsout;
	ac3streamsin.push_back("stream2");
	
	man.addActor("provider",new testprovider(1,1,1), ac1streamsin, ac1streamsout);
	man.addActor("transformer",new testtransformer(1,-1,4), ac2streamsin, ac2streamsout);
	man.addActor("consumer",new testconsumer(1,1000000000,1), ac3streamsin, ac3streamsout);
	
	std::vector<std::string> actorsretrieve;
	actorsretrieve.push_back("provider");
	actorsretrieve.push_back("transformer");
	actorsretrieve.push_back("consumer");
	for (sgraph::sgactor* actor : man.getActors(actorsretrieve))
	{
		std::cout << "Actorname: " << actor->getName() << std::endl;
	
	}
	man.start();
	std::cout << "simplegraph test started, press any key to exit" << std::endl;
	getchar();
	return 0;
}

