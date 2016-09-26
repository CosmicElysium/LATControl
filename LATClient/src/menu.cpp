#include "menu.h"
#include <stdio.h>

using namespace std;

namespace lbtlat
{

menu::menu()
{
//CAMON, CAMOFF, CAMSTAT, GETPARAM, SETPARAM, GETIMG, OPEN, CLOSE, DOORSTAT, FOCHOME, FOCMOVEREL, FOCMOVEABS, GETFOC, FOCSTOP, HELP, LIST

    commands.insert(pair<string,int>("CamPowerOn",CAMPOWERON));
    commands.insert(pair<string,int>("CamPowerOff",CAMPOWEROFF));
    commands.insert(pair<string,int>("CamPowerStat",CAMPOWERSTAT));
    commands.insert(pair<string,int>("GetCamParam",GETCAMPARAM));
    commands.insert(pair<string,int>("SetCamParam",SETCAMPARAM));
    commands.insert(pair<string,int>("GetImage",GETIMAGE));
    commands.insert(pair<string,int>("ShutterOpen",SHUTTEROPEN));
    commands.insert(pair<string,int>("ShutterClose",SHUTTERCLOSE));
    commands.insert(pair<string,int>("ShutterStat",SHUTTERSTAT));
    commands.insert(pair<string,int>("FocHome",FOCHOME));
    commands.insert(pair<string,int>("FocMoveRel",FOCMOVEREL));
    commands.insert(pair<string,int>("FocMoveAbs",FOCMOVEABS));
    commands.insert(pair<string,int>("FocPos",FOCPOS));
    commands.insert(pair<string,int>("FocStop",FOCSTOP));

	commands.insert(pair<string,int>("-list",LIST));
	commands.insert(pair<string,int>("-help",HELP));

	params.insert(pair<string,int>("Height",HEIGHT));
	params.insert(pair<string,int>("Width",WIDTH));
	params.insert(pair<string,int>("ExpTime",EXPTIME));
	//params.insert(pair<string,int>("FrameRate",FRAMERATE));
	params.insert(pair<string,int>("BinningX",BINNINGX));
	params.insert(pair<string,int>("BinningY",BINNINGY));

}

menu::~menu()
{
}

//! Prints out the main LAT menu
void menu::printMain(int IRVmajor, int IRVminor)
{
	cout << "\nLaser Alignment Telescope (LAT) Client " << IRVmajor << "." << IRVminor << "\n\n";
	cout << "SYNOPSIS\n\tLATClient [Command] [Parameter 1] ... [Parameter N]\n\n";
	cout << "DESCRIPTION\n\tThe daemon for the LAT is a client-server ";
	cout << "application which uses ICE as its communication platform.\n\n";
	cout << "COMMANDS\n\t-list\n\t\tshows the list of commands available in the interface.\n\n";
	cout << "\t-help Command\n\t\tshows the command format, parameters, usage and examples.\n\n";
};

//! Prints out the command list, in alphabetical order.
void menu::list()
{
//CAMON, CAMOFF, CAMSTAT, GETPARAM, SETPARAM, GETIMG, OPEN, CLOSE, DOORSTAT, FOCHOME, FOCMOVEREL, FOCMOVEABS, GETFOC, FOCSTOP, HELP, LIST
	printf("%-25s","CamPowerOn");
	printf("%-25s","CamPowerOff");
	printf("%-25s","CamPowerStat");
	printf("%-25s","GetCamParam");
	printf("\n");
	printf("%-25s","SetCamParam");
	printf("%-25s","GetImage");
	printf("%-25s","ShutterOpen");
	printf("%-25s","ShutterClose");
	printf("\n");
	printf("%-25s","FocPos");
	printf("%-25s","FocHome");
	printf("%-25s","FocMoveRel");
	printf("%-25s","FocMoveAbs");
	printf("\n");
	printf("%-25s","FocStop");
	printf("%-25s","-help");
	printf("%-25s","-list");
	printf("\n");
};


//! Check if the given command exists. If so, return the command number.
//!  Returns -1 if command not found, -2 if command is ambiguous.
int menu::CheckCmd(string cmd)
{
	int ret=-1,n;

	/* check for exact match */
	map<string,int>::iterator entry = commands.find(cmd);
	if(entry != commands.end()) return entry->second;

	/* check for abbreviations */
	n = 0;
	for(map<string,int>::iterator i=commands.begin(); i!=commands.end(); i++) {
		if(strncmp(i->first.c_str(),cmd.c_str(),strlen(cmd.c_str())) == 0) {
			ret = i->second;
			n++;
		}
	}
	if(n == 0) return -1;  // not found
	if(n > 1) return -2;   // ambiguous
	return ret;            // found
}


//! Check if the given param exists. If so, return the param number.
//!  Returns -1 if param not found, -2 if param is ambiguous.
int menu::CheckParam(string param)
{
	int ret=-1,n;

	/* check for exact match */
	map<string,int>::iterator entry = params.find(param);
	if(entry != params.end()) return entry->second;

	/* check for abbreviations */
	n = 0;
	for(map<string,int>::iterator i=params.begin(); i!=params.end(); i++) {
		if(strncmp(i->first.c_str(),param.c_str(),strlen(param.c_str())) == 0) {
			ret = i->second;
			n++;
		}
	}
	if(n == 0) return -1;  // not found
	if(n > 1) return -2;   // ambiguous
	return ret;            // found
}



//! Get the help message associated with the specified command number.
string menu::HelpCmd(int cmd)
{
	switch (cmd)
	{
		case CAMPOWERON:
			return  (string)"Usage:\n"+
				(string)"\tCamPowerOn\n\n";
				(string)"Turn on the LAT Camera\n\n";

		case CAMPOWEROFF:
			return  (string)"Usage:\n"+
				(string)"\tCamPowerOff\n\n";
				(string)"Shutdown the LAT Camera\n";

		case CAMPOWERSTAT:
			return  (string)"Usage:\n"+
				(string)"\tCamPowerStat\n\n";
				(string)"Check if Camera is on\n";

		case SHUTTEROPEN:
			return  (string)"Usage:\n"+
				(string)"\tShutterOpen\n\n";
				(string)"Open the LAT shutter\n";

		case SHUTTERCLOSE:
			return  (string)"Usage:\n"+
				(string)"\tShutterClose\n\n";
				(string)"Close the LAT shutter\n";

		case SHUTTERSTAT:
			return  (string)"Usage:\n"+
				(string)"\tShutterStat\n\n";
				(string)"Check if LAT shutter is open or closed\n";

		case GETIMAGE:
			return  (string)"Usage:\n"+
				(string)"\tGetImage NUMBER-IMAGES\n\n";
				(string)"Take the specified number of images. Default is 1 image\n\n";
				(string)"\ti.e GetImage 5\n";

		case FOCPOS:
			return  (string)"Usage:\n"+
				(string)"\tFocPos\n\n";
				(string)"Check current position of the focus (in steps).\n";
                (string)"Note that 1 step ~ 9.03 micrometers\n";

		case FOCMOVEABS:
			return  (string)"Usage:\n"+
				(string)"\tFocMoveAbs ABSOLUTE-POSITION[step]\n\n"+
				(string)"FocMoveAbs moves the focus to the given step position.\n"+
				(string)"Focus can only occupy steps 0 to 10000\n\n"+
				(string)"\ti.e FocMoveAbs 1000\n";

        case FOCHOME:
			return  (string)"Usage:\n"+
				(string)"\tFocHome\n\n"+
				(string)"FocHome initializes the focus to home (zero) position.\n\n"+
				(string)"\ti.e FocHome\n";


		case FOCMOVEREL:
			return  (string)"Usage:\n"+
				(string)"\tFocMoveRel RELATIVE-POSITION[steps]\n\n"+
				(string)"FocMoveRel moves the focus the given amount of steps.\n"+
				(string)"Positive steps moves the focus away from the telescope \n"+
				(string)"and negative moves the focus towards the telescope.\n"+
				(string)"Focus can only occupy steps 0 to 10000\n\n"+
				(string)"\ti.e FocMoveRel -500\n";

		case HELP:
			return  (string)"Usage:\n"+
				(string)"\t-help COMMAND\n\n"+
				(string)"Shows the command usage.\n\n"+
				(string)"\ti.e. -help FocHome\n";

		case GETCAMPARAM:
			return  (string)"Usage:\n"+
				(string)"\tGetCamParam CAMERA-PARAMETER\n\n"+
				(string)"GetCamParam returns the current value of the specified parameter.\n\n"+
				(string)"\ti.e.GetCamParam ExpTime\n";

		case SETCAMPARAM:
			return  (string)"Usage:\n"+
				(string)"\tSetCamParam CAMERA-PARAMETER VALUE\n\n"+
				(string)"GetCamParam sets the value of the specified parameter to the specified value.\n\n"+
				(string)"\ti.e.GetCamParam ExpTime 10000\n";


		case -1:
			return 	(string)"Command not found.";

		case -2:
			return  (string)"Ambiguous command.";
	}
	return 	"Command not found.";
}

/*
//! Get the help message associated with the specified command number.
string menu::HelpParam(int param)
{
	switch (param)
	{
	    case HEIGHT:
			return  (string)"Usage:\n"+
				(string)"\t-start\n\n";
				(string)"Start the IRS server\n";

        case WIDTH:
            return (string)"Width in pixels of
*/

}
