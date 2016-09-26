#include "IOControl.h"
#include <cstdlib>
#include <sstream>

//Wait time for shutter to open and close in milliseconds
#define SHUTTEROPENMS 2600
#define SHUTTERCLOSEMS 7300

//Commands to get and set the 32-bit bitmask that defines the status of the inputs
// and outputs of the stepper motor controller
#define IOGET "ZY"
#define IOSET "Y"

//The bit locations for the camera power and shutter valve power
#define CAMBIT 17
#define SHUTTERBIT 16

using namespace lbtlat;

//Constructor with ip of host and port and wait time for connecting socket
IOControl::IOControl(std::string host, unsigned port,unsigned uwait) :
		CommControl(host, port, uwait)
{

}

//Virtual destructor
IOControl::~IOControl()
{

}

//Set the shutter bit of the motor controller to 1.
void IOControl::shutterOpen() {
    std::ostringstream ss;
    std::string statusStr= Comm(IOGET);
    unsigned statusInt = std::atoi(statusStr.substr(3).c_str());
    unsigned response = statusInt | 1 << SHUTTERBIT;
    ss << IOSET << response;
    Comm(ss.str());

    //Delay end of function until shutter is open
    usleep(SHUTTEROPENMS * 1000);
}

//Set the shutter bit of the motor controller to 0
void IOControl::shutterClose() {
    std::ostringstream ss;
    std::string statusStr= Comm(IOGET);
    unsigned statusInt = std::atoi(statusStr.substr(3).c_str());
    unsigned response = statusInt & ~(1 << SHUTTERBIT);
    ss << IOSET << response;
    Comm(ss.str());

    //Delay end of function until shutter is closed
    usleep(SHUTTERCLOSEMS * 1000);
}

//Return the status of the shutter bit
bool IOControl::shutterStatus() {
    std::string statusStr= Comm(IOGET);
    unsigned statusInt = std::atoi(statusStr.substr(3).c_str());
    return statusInt & 1 << SHUTTERBIT;
}

//Set the camera bit of the motor controller to 1
void IOControl::camPowerOn() {
    std::ostringstream ss;
    std::string statusStr= Comm(IOGET);
    unsigned statusInt = std::atoi(statusStr.substr(3).c_str());
    unsigned response = statusInt | 1 << CAMBIT;
    ss << IOSET << response;
    Comm(ss.str());
}

//Set the camera bit of the motor controller to 0
void IOControl::camPowerOff() {
    std::ostringstream ss;
    std::string statusStr= Comm(IOGET);
    unsigned statusInt = std::atoi(statusStr.substr(3).c_str());
    unsigned response = statusInt & ~(1 << CAMBIT);
    ss << IOSET << response;
    Comm(ss.str());
}

//Return the status of the camera bit
bool IOControl::camPowerStatus() {
    std::string statusStr= Comm(IOGET);
    unsigned statusInt = std::atoi(statusStr.substr(3).c_str());
    return statusInt & 1 << CAMBIT;
}
