#include "LatI.h"
#include <Ice/Application.h>

using namespace std;
using namespace lbtlat;

//Server class.
class LatService : public Ice::Application
{
public:

    virtual int run(int, char*[]);
};

void printHelpAndExit(std::string programName)
{
	std::cout << "Usage: " << programName << " <config_file_name>" << std::endl;
	std::cout << "Example: " << std::endl;
	std::cout << "\t" << programName << " /home/argos/apps/etc/server.conf" << std::endl;
	exit(-1);
}

int main(int argc, char* argv[])
{
	//read the server configuration file.
    LatService app;
    char * config_file;
    if (argc < 2)
    	printHelpAndExit(argv[0]);
    config_file = argv[1];
    return app.main(argc, argv, config_file);
}

int LatService::run(int argc, char* argv[])
{
	//Create the service object adapter.
    Ice::ObjectAdapterPtr latAdapter = communicator()->createObjectAdapter("Lat");

    Ice::ObjectPtr object = new lbtlat::LatI(communicator());

    //Add the adapter to the communicator.
    latAdapter->add(object,
    		        communicator()->stringToIdentity("Lat"));

    //Activate the adapter.
    latAdapter->activate();

    //Wait for kill signal or ctrl-c
    communicator()->waitForShutdown();

    //Exit.
    return EXIT_SUCCESS;
}


