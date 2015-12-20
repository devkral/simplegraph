
#include "ffmpegplugin.h"
#include <iostream>


using namespace sgraph;

int main()
{
	
	sgraph::sgmanager man;
	
	std::vector<std::string> ac1streamsin;
	std::vector<std::string> ac1streamsout;
	ac1streamsout.push_back("stream1");
	try{
	man.addActor("video",new ffmpegvideosource(1,1,"",""), ac1streamsin, ac1streamsout);
	}
	catch(sgraphException &e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	std::vector<std::string> actorsretrieve;
	actorsretrieve.push_back("video");
	for (sgraph::sgactor* actor : man.getActors(actorsretrieve))
	{
		std::cout << "Actorname:" << actor->getName() << std::endl;
	
	}
	man.start();
	std::cout << "simplegraph test started, press any key to exit" << std::endl;
	getchar();
	return 0;
}

