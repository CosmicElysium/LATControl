#include "CommControl.h"

#include "DeviceException.h"
#include <string>

//Prefix and suffix for serial commands to stepper motor controller
#define COMMPREFIX "#1"
#define COMMSUFFIX "\r"

#define CONNECTED true
#define DISCONNECTED false


using namespace lbtlat;

CommControl::CommControl(std::string host, unsigned int port,unsigned int uwait) : status_(DISCONNECTED)
{
    if ( ! sock_.create() )
        throwCommException("CommControl::CommControl-> Could not create the client socket.");

    if ( ! sock_.connect ( host, port ) )
        throwCommException( "CommControl::CommControl-> Could not connect to port." );

    status_ = CONNECTED;

}

CommControl::~CommControl()
{
    if (status_ == CONNECTED) {
        sock_.close();
        status_ = DISCONNECTED;
    }
}


std::string CommControl::Comm(const std::string &msg) {

    std::string s;
    std::string command = COMMPREFIX + msg + COMMSUFFIX;
//#1$ page 30 in manual
    if ( ! sock_.send (command) )
      throwCommException( "CommControl::Comm -> Could not write [" + command + "] to the socket." );
    usleep(uwait_);
    if ( ! sock_.recv ( s ) )
      throwCommException( "CommControl::Comm -> Could not read from socket." );
    if (s.find('?') != std::string::npos)
        throwCommException("CommControl::Comm -> [" + s + "] invalid command for motor.");

    return s;
}

void CommControl::throwCommException(std::string what) {
    std::string msg = "Comm Error: " + what;
    throw DeviceException(msg);
}

