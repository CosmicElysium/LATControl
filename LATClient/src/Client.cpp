#include <iostream>
#include <Ice/Ice.h>
#include <lat.h>
#include <FitsTool.h>
#include <menu.h>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <stdint.h>

#define USHORT uint16_t
#define SHORT int16_t
#define UNSET 9999999

#define FILEPATHDEFAULT "/tmp"
#define FILEPREFIXDEFAULT "NoName"

#define VMAJOR 1
#define VMINOR 1

using namespace std;
using namespace lbtlat;


std::string getDateTime() {
        time_t t = time(0);   // get time now
        tm * now = localtime( & t );
        ostringstream ss;
        ss << (now->tm_year + 1900)
         << (now->tm_mon + 1)
         <<  now->tm_mday
         << setw(2) << setfill('0')
         << now->tm_hour
         << setw(2) << setfill('0')
         << now->tm_min
         << setw(2) << setfill('0')
         << now->tm_sec;
         return ss.str();
}



int main(int argc, char *argv[])
{

    int status = EXIT_SUCCESS;


	/* Read the configuration file. Look in the local directory first,
	 * and if it is not found, check the environment variable LAT_CLIENT_CONFIG.
	 This file MUST exist in order to find the server*/
	std::string config_file = "../etc/client.config";
	if(access(config_file.c_str(),R_OK) == -1)
	{
		config_file = getenv("LAT_CLIENT_CONFIG");
		if(access(config_file.c_str(),R_OK) == -1)
		{
			cout << "No configuration file found" << endl;
			return EXIT_FAILURE;
		}
	}

	// Define a communicator to talk with the LAT server
	Ice::CommunicatorPtr lat_comm;



    Frame frame;
    menu m;
    //    int* img = new int[width * height];
    //USHORT* imgbuffer;

    //if run with no parameters, show the help/usage menu
    if (argc == 1)
	{
		m.printMain(VMAJOR,VMINOR);
		return EXIT_SUCCESS;
	}

    //Check if the command exists. If not, print an error message.
	int cmd = m.CheckCmd(argv[1]);
	if(cmd == -1)
	{
		cerr << "  " << argv[1] << ": Invalid command" << endl;
		return EXIT_FAILURE;
	}
	if(cmd == -2)
	{
		cerr << "  " << argv[1] << ": Ambiguous command" << endl;
		return EXIT_FAILURE;
	}

	if(cmd == HELP) {
		if(argc==3)
			cout << m.HelpCmd(m.CheckCmd(argv[2])) << endl;
		else
			cout << m.HelpCmd(cmd) << endl;
	    	return EXIT_SUCCESS;
	}

	if(cmd == LIST) {
		m.list();
		return EXIT_SUCCESS;
	}

    try
	{
	// Open the client configuration file, get the properties,
	// and associate them to the communicator.
        Ice::PropertiesPtr props = Ice::createProperties();
        props->load(config_file);
        Ice::InitializationData id;
        id.properties = props;

        //Initialize the object communicators
        lat_comm = Ice::initialize(argc, argv, id);

        //Create the proxy that talks to the LAT service
        LatPrx latUser;
        latUser = LatPrx::uncheckedCast(lat_comm->propertyToProxy("Lat.Proxy"));


    		//Get the repository location(by default use /newdata)
		std::string fits_path = props->getPropertyWithDefault("Lat.Path",FILEPATHDEFAULT);
		std::string fits_prefix = props->getPropertyWithDefault("Lat.Prefix",FILEPREFIXDEFAULT);

		//bool fits_count = props->getPropertyAsInt("Lat.FileCount");
		bool fits_date = props->getPropertyAsInt("Lat.FileDate");

        int pos = UNSET;
        int param;
        int value;

        switch (cmd)
		{

        case CAMPOWERON:

            if (argc!=2) throw (m.HelpCmd(cmd));

            if (!latUser) throw "LATServer is not running.";

            if (latUser->isCameraOn())
                throw "Camera is already ON";
            else {
                cout << "Turning the camera ON.." << endl;
                latUser->turnCameraOn();
                cout << "Camera is ON" << endl;
            }
            return EXIT_SUCCESS;
         //-------------------------------------------------------------------------------------
        case CAMPOWEROFF:
            if (argc!=2) throw (m.HelpCmd(cmd));

            if (!latUser) throw "LATServer is not running.";
            if (!latUser->isCameraOn())
                throw "Camera is already OFF";
            else {
                cout << "Turning the camera OFF..." << endl;
                latUser->turnCameraOff();
                cout << "Camera is OFF" << endl;
            }
            return EXIT_SUCCESS;
        //-------------------------------------------------------------------------------------
        case CAMPOWERSTAT:
            if (argc!=2) throw m.HelpCmd(cmd);
            if (!latUser) throw "LATServer is not running.";

            if (latUser->isCameraOn())
                cout << "Camera is On" << endl;
            else
                cout << "Camera is Off" << endl;
            return EXIT_SUCCESS;
         //-------------------------------------------------------------------------------------
        case GETIMAGE:
            {

            if (argc!=2) throw m.HelpCmd(cmd);

            if (!latUser) throw "LATServer is not running.";

            if (!latUser->isCameraOn()) throw "Camera is not on!";

            //while (!latUser->rollEmpty()) {
            //    latUser->getImage();
           // }

            //int nimages = (argc == 2 ? 1 : atoi(argv[2]));

            //img = (USHORT &)latUser->getImage(1000);
            //latUser->takeImage(nimages);
            std::string dateTime = getDateTime();

            //for (int i=1; i<=nimages; i++) {

                frame = latUser->getImage();

                //SHORT *imgptr = &frame.ImageVector[0];
                //USHORT *convimg;
                //convimg = (USHORT*) imgptr;

                                //		imgbuffer = (USHORT &) img;
                USHORT* tmpBuffer = reinterpret_cast<USHORT*>(&frame.ImageVector[0]);

                //= new USHORT[2* frame.width * frame.height];

                //std::memcpy(tmpBuffer, convimg, 2*frame.height*frame.width);
                //* pScanLine = reinterpret_cast<USHORT*>(tmpBuffer);
                FitsTool fits;

                fits.setDirectory(fits_path);
                fits.setPrefix(fits_prefix);

                ostringstream ssExpTime;
                ostringstream ssfocPos;
                ostringstream ssBinX;
                ostringstream ssBinY;
                //ostringstream ssFRate;

                ssExpTime << frame.expTimeInMicroseconds;
                ssfocPos << frame.focPosInSteps;
                ssBinX << frame.binningXFactor;
                ssBinY << frame.binningYFactor;
                //ssFRate << frame.fRate;

                //Fits header!!
                fitskey expTimeDat = {"TLONG","EXPTIME",ssExpTime.str(),"Exposure Time in microseconds"};
                fitskey focPosDat = {"TLONG","FOCPOS",ssfocPos.str(),"Distance from camera to telescope in steps"};
                fitskey binningXDat = {"TINT","BINX",ssBinX.str(),"Horizontal Binning"};
                fitskey binningYDat = {"TINT","BINY",ssBinY.str(),"Vertical Binning"};
                fitskey targetDat = {"TSTRING","TARGET",frame.target,"Optional celestial target"};
                //fitskey fRateDat = {"TFLOAT","FRATE",ssFRate.str(),"Pseudo-framerate of images from sleep function"};


                fitskey keysArray[] = {expTimeDat,focPosDat,binningXDat,binningYDat,targetDat};//,fRateDat};
                std::vector<fitskey> keys (keysArray, keysArray + sizeof(keysArray) / sizeof(keysArray[0]));

                fits.setKeys(keys);

                std::string fits_name;
                /*
                if (fits_count) {
                    ostringstream ssNImages;
                    ssNImages << i;
                    fits_name += "_";
                    fits_name += ssNImages.str();
                }
                */
                if (fits_date) {
                    fits_name += "_";
                    fits_name += dateTime;
                }


                if(fits.write16(fits_name, tmpBuffer , frame.width, frame.height)) //Change to fits...
                    printf("Failed to save the grabbed frame!\n");
                else
                    std::cout << "Frame saved to " << std::endl <<fits_path << "/" << fits_prefix << fits_name <<".fits"<< std::endl;

            return EXIT_SUCCESS;
            }
         //-------------------------------------------------------------------------------------
        case FOCHOME:
            if (argc!=2) throw (m.HelpCmd(cmd));

            if (!latUser) throw "LATServer is not running.";
            cout << "Moving focus..."  << endl;
            latUser->focusHome();
            cout << "Focus is reset!" << endl;
            return EXIT_SUCCESS;
         //-------------------------------------------------------------------------------------
        case FOCMOVEREL:
			if (argc!=3) throw m.HelpCmd(cmd);



            if (!latUser) throw "LATServer is not running.";

			latUser->moveFocusRel(atoi(argv[2]));
			cout << "Focus moved " << argv[2] << " steps!" << endl;
			return EXIT_SUCCESS;

         //-------------------------------------------------------------------------------------
        case FOCMOVEABS:
			if (argc!=3) throw m.HelpCmd(cmd);

            if (!latUser) throw "LATServer is not running.";
            cout << "Moving focus..."  << endl;
			latUser->moveFocusAbs(atoi(argv[2]));
			cout << "Focus moved to step #" << argv[2] << endl;

			return EXIT_SUCCESS;
         //-------------------------------------------------------------------------------------
        case FOCPOS:
            if (argc!=2) throw m.HelpCmd(cmd);

            if (!latUser) throw "LATServer is not running.";

            pos = latUser->getFocusPositionInSteps();
            cout << "Current focuser position: " << endl;
			cout << pos << endl;

            return EXIT_SUCCESS;
         //-------------------------------------------------------------------------------------
        case FOCSTOP:
            if (argc!=2) throw m.HelpCmd(cmd);

            if (!latUser) throw "LATServer is not running.";

            latUser->stopFocus();
            cout << "Focus stopped!" << endl;

            return EXIT_SUCCESS;
        //-------------------------------------------------------------------------------------
        case GETCAMPARAM:
            if (argc!=3) throw m.HelpCmd(cmd);

            if (!latUser) throw "LATServer is not running.";

            if (!latUser->isCameraOn()) throw "Camera is not on!";

            param = m.CheckParam(argv[2]);
            if(param == -1)
            {
                cerr << "  " << argv[2] << ": Invalid parameter" << endl;
                return EXIT_FAILURE;
            }
            if(param == -2)
            {
                cerr << "  " << argv[1] << ": Ambiguous parameter" << endl;
                return EXIT_FAILURE;
            }
            cout << "Camera " << argv[2] <<" is" << endl;
            switch(param) {
                    case HEIGHT:
                            cout << latUser->getHeight() << endl;
                            break;
                    case WIDTH:
                            cout << latUser->getWidth() << endl;
                            break;
                    case EXPTIME:
                            cout << latUser->getExpTimeInMicroseconds() << endl;
                            break;
                    case BINNINGX:
                            cout << latUser->getBinningXFactor() << endl;
                            break;
                    case BINNINGY:
                            cout << latUser->getBinningYFactor() << endl;
                            break;
            }


            return EXIT_SUCCESS;
        //-------------------------------------------------------------------------------------
        case SETCAMPARAM:
            if (argc!=4) throw m.HelpCmd(cmd);

            if (!latUser) throw "LATServer is not running.";

            if (!latUser->isCameraOn()) throw "Camera is not on!";

            param = m.CheckParam(argv[2]);
            if(param == -1)
            {
                cerr << "  " << argv[2] << ": Invalid parameter" << endl;
                return EXIT_FAILURE;
            }
            if(param == -2)
            {
                cerr << "  " << argv[1] << ": Ambiguous parameter" << endl;
                return EXIT_FAILURE;
            }
            value = std::atof(argv[3]);
            switch(param) {
                    case HEIGHT:
                    	throw "Cannot change camera height!";
                            break;
                    case WIDTH:
                    	throw "Cannot change camera width!";
                            break;
                    case EXPTIME:
                    	latUser->setExpTime(value);;
                            break;
                    case BINNINGX:
                    	latUser->setBinningX(value);
                            break;
                    case BINNINGY:
                    	latUser->setBinningY(value);
                            break;
            }
            cout << argv[2] << " successfully set to" << endl;
            cout << argv[3] << endl;


            return EXIT_SUCCESS;


        //-------------------------------------------------------------------------------------
        case SHUTTEROPEN:
            if (argc!=2) throw m.HelpCmd(cmd);

            if (!latUser) throw "LATServer is not running.";

            if (latUser->isDoorOpen())
                throw "Shutter is already OPEN";
            else {
                cout << "Opening the shutter..." << endl;
                latUser->openDoor();
                cout << "Shutter is OPEN" << endl;
            }

            return EXIT_SUCCESS;
        //-------------------------------------------------------------------------------------
        case SHUTTERCLOSE:
            if (argc!=2) throw m.HelpCmd(cmd);

            if (!latUser) throw "LATServer is not running.";
            if (!latUser->isDoorOpen())
                throw "Shutter is already CLOSED";
            else {
                cout << "Closing the shutter..." << endl;
                latUser->closeDoor();
                cout << "Shutter is CLOSED" << endl;
            }
            return EXIT_SUCCESS;
        //-------------------------------------------------------------------------------------
        case SHUTTERSTAT:
            if (argc!=2) throw m.HelpCmd(cmd);

            if (!latUser) throw "LATServer is not running.";

            if (latUser->isDoorOpen())
                cout << "Shutter is open" << endl;
            else
                cout << "Shutter is closed" << endl;

            return EXIT_SUCCESS;


		}

	}
	catch (const LatException& e)
	{
		cerr << "Error: " << e.what << endl;
		status = EXIT_FAILURE;
	}

	catch (const Ice::ConnectionRefusedException&)
	{
		cerr << "Error: Connection refused by LAT service. ";
		cerr << "The LAT service must be running to use the Client." << endl;
		status = EXIT_FAILURE;
	}

	catch (const Ice::Exception& e)
	{
		cerr << "LAT Service Error: " << e << endl;
		status = EXIT_FAILURE;
	}

	catch (const char* msg)
	{
		cerr << msg << endl;
		status = EXIT_FAILURE;
	}

	catch (string msg)
	{
		cerr << msg << endl;
		status = EXIT_FAILURE;
	}

	//Destroy the communicators
	if(lat_comm) lat_comm->destroy();


	return status;
}
