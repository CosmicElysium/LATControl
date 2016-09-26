#include "LatI.h"
#include <string>
#include <iostream>
#include "DeviceException.h"

#include "Motor.h"
#include "IOControl.h"


#define USHORT uint16_t
#define SHORT int16_t

#define FAILED false
#define SUCCESS true

#define TARGET "no target"

using namespace std;
using namespace lbtlat;

LatI::LatI(Ice::CommunicatorPtr comm)
{
    // Put the communicator and the properties in
    // member variables for further use
    comm_= comm;
    props_= comm->getProperties();

    // Create a logger object. Any data sent to info
    // will show up in the system syslog in var/log/messages
    Ice::Print info(comm->getLogger());

    // Create a starting message
    string msg = "Starting LBT Lat service... Done [Ctrl-c to exit]";

    //Send message to system syslog
    info << msg;
    info.flush();

    controlHost_= props_->getPropertyWithDefault("Control.Host","192.168.26.126"); //can't read for some reason!!!
    controlPort_  = props_->getPropertyAsIntWithDefault("Control.Port",4001);
    controlUwait_  = props_->getPropertyAsIntWithDefault("Control.Uwait",30);
    camIP_ = props_->getPropertyWithDefault("Camera.Host","192.168.26.238");
}

LatI::~LatI()
{

}

void LatI::moveFocusRel(int steps, const Ice::Current&)
{
    try
    {
        Motor focuser(controlHost_,controlPort_,controlUwait_);
        focuser.RelMove(steps);
        logNotice("Focus moved");
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
}

void LatI::moveFocusAbs(int positionInSteps, const Ice::Current&)
{
    IceUtil::RecMutex::Lock lock(_mutex);
    try
    {
        Motor focuser(controlHost_,controlPort_,controlUwait_);
        focuser.AbsMove(positionInSteps);
        logNotice("Focus moved");
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
}

void LatI::stopFocus(const Ice::Current&)
{
    IceUtil::RecMutex::Lock lock(_mutex);
    try
    {
        Motor focuser(controlHost_,controlPort_,controlUwait_);
        focuser.Stop();
        logNotice("Focus stopped");
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
}

void LatI::focusHome(const Ice::Current&)
{
    IceUtil::RecMutex::Lock lock(_mutex);
    try
    {
        logNotice("Resetting focus...");
        Motor focuser(controlHost_,controlPort_,controlUwait_);
        focuser.Reset();
        logNotice("Focus was reset.");
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
}

int LatI::getFocusPositionInSteps(const Ice::Current&)
{
    int pos = 99999999; //ridiculously high position to test if it's a real position
    try
    {
        Motor focuser(controlHost_,controlPort_,controlUwait_);
        pos = focuser.GetPos();
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
    return pos;
}

void LatI::turnCameraOn(const Ice::Current&)
{
    IceUtil::RecMutex::Lock lock(_mutex);
    try
    {
        logNotice("Turning camera on");
        IOControl camPower(controlHost_,controlPort_,controlUwait_);
        camPower.camPowerOn();
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
}

void LatI::turnCameraOff(const Ice::Current&)
{
    IceUtil::RecMutex::Lock lock(_mutex);
    try
    {
        logNotice("Turning camera off");
        IOControl camPower(controlHost_,controlPort_,controlUwait_);
        camPower.camPowerOff();
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
}


bool LatI::isCameraOn(const Ice::Current&)
{
    bool status;
    try
    {
        IOControl camPower(controlHost_,controlPort_,controlUwait_);
        status = camPower.camPowerStatus();
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
    return status;
}

void LatI::initCamera(const Ice::Current&)
{
    logNotice("Init camera");

    IceUtil::RecMutex::Lock lock(_mutex);
    camera_.init();
    camera_.cameraSetup(inet_addr(camIP_.c_str()));
}

Frame LatI::getImage(const Ice::Current&)
{
    logNotice("Getting image");
    Frame frame;
    try
    {
        // Make a call thread safe
        IceUtil::RecMutex::Lock lock(_mutex);
        {
            camera_.cameraStart();
            Image image = camera_.getImage();
            camera_.cameraStop();

            frame.height = image.height;
            frame.width = image.width;
            int frameSize = frame.height * frame.width;
            for (int i = 0; i < frameSize; i++)
                frame.ImageVector.push_back((USHORT)image.dataVector[i]);
            frame.expTimeInMicroseconds = image.expTime;
            frame.binningXFactor = image.binX;
            frame.binningYFactor = image.binY;

            Motor focuser(controlHost_,controlPort_,controlUwait_);
            frame.focPosInSteps = focuser.GetPos();
            frame.target = TARGET;
        }
    }
    catch (const DeviceException& dE)
    {
        camera_.cameraStop();
        throwLatException(dE.what());
    }
    return frame;
}

int LatI::getHeight(const Ice::Current&)
{
    int value;
    try
    {
        value= camera_.getHeight();
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
    return value;
}


int LatI::getWidth(const Ice::Current&)
{
    int value;
    try
    {
        value = camera_.getWidth();
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
    return value;
}

int LatI::getExpTimeInMicroseconds(const Ice::Current&)
{
    int value;
    try
    {
        value = camera_.getExp();
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
    return value;
}

int LatI::getBinningXFactor(const Ice::Current&)
{
    int value;
    try
    {
        value = camera_.getBinX();
    } catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
    return value;
}

int LatI::getBinningYFactor(const Ice::Current&)
{
    int value;
    try
    {
        value = camera_.getBinY();
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
    return value;
}

void LatI::setExpTime(int expTimeInMicroseconds, const Ice::Current&)
{
    IceUtil::RecMutex::Lock lock(_mutex);
    try
    {
        logNotice("Setting exposure time");
        camera_.setExp(expTimeInMicroseconds);
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
}

void LatI::setBinningX(int factor, const Ice::Current&)
{
    IceUtil::RecMutex::Lock lock(_mutex);
    try
    {
        logNotice("Setting binning X");
        camera_.setBinX(factor);
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
}

void LatI::setBinningY(int factor, const Ice::Current&)
{
    IceUtil::RecMutex::Lock lock(_mutex);
    try
    {
        logNotice("Setting binning Y");
        camera_.setBinY(factor);
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
}

void LatI::openDoor(const Ice::Current&)
{
    try
    {
        logNotice("Opening shutter...");
        IOControl shutter(controlHost_,controlPort_,controlUwait_);
        shutter.shutterOpen();
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
    logNotice("Shutter open");
}

void LatI::closeDoor(const Ice::Current&)
{
    try
    {
        logNotice("Closing shutter...");
        IOControl shutter(controlHost_,controlPort_,controlUwait_);
        shutter.shutterClose();
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
    logNotice("Shutter closed");
}

bool LatI::isDoorOpen(const Ice::Current&)
{
    bool status;
    try
    {
        IOControl shutter(controlHost_,controlPort_,controlUwait_);
        status = shutter.shutterStatus();
    }
    catch (const DeviceException& dE)
    {
        throwLatException(dE.what());
    }
    return status;
}

//Private function to encapsulate the following steps in one function
void LatI::throwLatException(std::string what)
{
    logError(what);
    LatException ex;
    ex.what = what;
    throw ex;
}

void LatI::logNotice(std::string msg)
{
    Ice::Print info(comm_->getLogger());
    info << "[NOTICE]:" << msg;
    info.flush();
}

void LatI::logError(std::string msg)
{
    Ice::Error error(comm_->getLogger());
    error << "[ERROR]: " << msg;
    error.flush();
}
