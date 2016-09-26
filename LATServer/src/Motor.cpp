#include "Motor.h"
#include <string>
#include "DeviceException.h"
#include <sstream>
#include <cmath>
#include <unistd.h>
#include <cstdlib>


//Various serial commands used by stepper motor controller
#define MOTORREFERENCE "p4"
#define MOTORRELATIVE 	"p1"
#define MOTORABSOLUTE 	"p2"
#define MOTORSTEPS 		"s"
#define MOTORLEFT		"d0"
#define MOTORRIGHT 		"d1"
#define MOTORSTART 		"A"
#define MOTORSTOP 		"S"
#define MOTORPOS 		"C"
#define MOTORRESET 		"D"
#define MOTORZEROED		":is_referenced"
#define MOTORSTATUS 	"$"
#define MOTORIOBITS		"ZY"



//Bits that indicate a certain status from MOTORSTATUS
#define READYBIT 		0
#define ZEROPOSBIT 		1
#define ERRORPOSBIT		2


//Software step limit of stepper motor
#define STEPLIMLOW 		0
#define STEPLIMHIGH 	7500

//Max number of times to check if home limit switch
//has been reached when initializing motor
#define MAXRESETCHECK 	30

//Max number of times to check if home limit switch
//has been released when initializing motor. (Checks if switch is stuck)
#define SWITCHEXITMAX 	100

//Input bit of the end switch
#define ENDBIT 			6

//Input bit of the front switch
#define STARTBIT 		5

using namespace lbtlat;

//Constructor with ip of host and port and wait time for connecting socket
Motor::Motor(std::string host, unsigned int port,unsigned int uwait) :
		CommControl(host, port, uwait)
{
}

//Virtual destructor
Motor::~Motor()
{

}


//Move motor to desire step position (pos)
void Motor::AbsMove(int pos){
	if (!IsReferenced())
		ThrowMotorException("Motor is not initialized. Please reference the motor.");
	if (PosError())
		ThrowMotorException("Position error. Please reference the motor.");
	if (!IsReady())
		ThrowMotorException("Motor is not ready. Try again.");

	//check if desired move exceeds defined position limits
 	if (pos < STEPLIMLOW)
		ThrowBoundsException("Desired motor position is less than ", STEPLIMLOW);
	if (pos > STEPLIMHIGH)
		ThrowBoundsException("Desired motor position is greater than ", STEPLIMHIGH);


	//tell motor to go into absolute mode and set desired position
	std::ostringstream ss;
    ss << MOTORSTEPS << pos;
    Comm(MOTORABSOLUTE);
    Comm(ss.str());

    //start motor
    Comm(MOTORSTART);
/*
    while (!IsReady()) {
    	sleep(1);
    	if (PosError())
    		ThrowBoundsException("Limit switch reached. Last known position: ", GetPos());
    }

*/

}

//Move the motor a relative number of steps (dist) from current position
void Motor::RelMove(int dist) {

	if (!IsReferenced())
		ThrowMotorException("Motor is not initialized. Please reference the motor.");
	if (PosError())
		ThrowMotorException("Position error. Please reference the motor.");
	if (!IsReady())
		ThrowMotorException("Motor is not ready.");

	//check if desired move exceeds defined position limits
	int currentPos = GetPos();
	int desiredPos = currentPos + dist;
	if ((desiredPos) > STEPLIMHIGH)
		ThrowBoundsException("Desired motor position is greater than ", STEPLIMHIGH);
	if ((desiredPos) < STEPLIMLOW)
		ThrowBoundsException("Desired motor position is less than ", STEPLIMLOW);

	std::ostringstream ss;
	Comm(MOTORRELATIVE);
    if (dist < 0) {
        Comm(MOTORLEFT);
        dist *= -1;
    } else
        Comm(MOTORRIGHT);
    ss << MOTORSTEPS << dist;
    Comm(ss.str());

    //start motor
    Comm(MOTORSTART);
/*
    while (!IsReady()) {
    	sleep(1);
    	if (PosError())
    		ThrowBoundsException("Limit switch reached. Last known position: ", GetPos());
    }
    */
}

//Stop the motor
void Motor::Stop() {
    Comm(MOTORSTOP);
}

//Initialize the motor to position 0
void Motor::Reset() {

	//Reset any outstanding position errors if position error is read
	if (PosError())
		Comm(MOTORRESET);

	//Set motor to reference mode and in direction of home switch
	Comm(MOTORREFERENCE);
    Comm(MOTORLEFT);

    //Start motor
    Comm(MOTORSTART);

    /*
    int checkNumber = 0;
    while (!IsReady()) {
    	sleep(1);
    	checkNumber++;
	std::cout << checkNumber << std::endl;
    	if (checkNumber > MAXRESETCHECK)
    		ThrowMotorException("Max time exceeded for reset. Please STOP motor");
    }
    if (!IsReferenced())
    	ThrowMotorException("Reference run failed. Try again.");
*/
}

//Return current step position of Motor
int Motor::GetPos() {
    std::string value = Comm(MOTORPOS);
    int pos = std::atoi(value.substr(2).c_str());
    return pos;
}

//General DeviceException thrown for general motor errors
void Motor::ThrowMotorException(std::string what) {
    std::ostringstream oss;
    oss << "Motor Error: " << what;
    std::string msg = oss.str();
    throw DeviceException(msg);
}

//General DeviceException with a given integer to say which bound was broken
void Motor::ThrowBoundsException(std::string what, int bound) {
    std::ostringstream oss;
    oss << "Bounds Error: " << what << bound;
    std::string msg = oss.str();
    throw DeviceException(msg);
}

//Return true if home switch is currently triggered
bool Motor::StartSwitch() {
    int switchStat =  std::atoi(Comm(MOTORIOBITS).substr(3).c_str());
    bool startSwitch = switchStat & 1 << STARTBIT;
    return startSwitch;
}

//Return true if end switch is currently triggered
bool Motor::EndSwitch() {
    int switchStat =  std::atoi(Comm(MOTORIOBITS).substr(3).c_str());
    bool endSwitch = switchStat & 1 << ENDBIT;
    return endSwitch;
}

bool Motor::PosError() {
    int motorStat =  std::atoi(Comm(MOTORSTATUS).substr(4).c_str());
    bool posError = motorStat & 1 << ERRORPOSBIT;
    return posError;
}

bool Motor::IsReady() {
    int motorStat =  std::atoi(Comm(MOTORSTATUS).substr(4).c_str());
    bool posError = motorStat & 1 << READYBIT;
    return posError;
}

bool Motor::IsReferenced() {
    int refStat =  std::atoi(Comm(MOTORZEROED).substr(16).c_str());
    return refStat;


}
