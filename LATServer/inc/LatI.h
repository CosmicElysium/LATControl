#ifndef LBT_LAT_I_H
#define LBT_LAT_I_H

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "Camera.h"
#include "lat.h"
#include <queue>

namespace lbtlat
{

enum camParams {
    HEIGHT, WIDTH, EXPTIME, BINNINGX, BINNINGY
};

class LatI : public virtual Lat
{


public:

    /**@brief constructor */
    LatI(Ice::CommunicatorPtr comm);
    virtual ~LatI();

    virtual void turnCameraOn(const Ice::Current&);
    virtual void turnCameraOff(const Ice::Current&);
    virtual bool isCameraOn(const Ice::Current&);
    virtual void initCamera(const Ice::Current&);
    virtual Frame getImage(const Ice::Current&);
    virtual int getHeight(const Ice::Current&);
    virtual int getWidth(const Ice::Current&);
    virtual int getExpTimeInMicroseconds(const Ice::Current&);
    virtual int getBinningXFactor(const Ice::Current&);
    virtual int getBinningYFactor(const Ice::Current&);
    virtual void setExpTime(int,const Ice::Current&);
    virtual void setBinningX(int,const Ice::Current&);
    virtual void setBinningY(int,const Ice::Current&);

    virtual void moveFocusAbs(int,const Ice::Current&);
    virtual void moveFocusRel(int, const Ice::Current&);
    virtual void stopFocus(const Ice::Current&);
    virtual void focusHome(const Ice::Current&);
    virtual int getFocusPositionInSteps(const Ice::Current&);

    virtual void openDoor(const Ice::Current&);
    virtual void closeDoor(const Ice::Current&);
    virtual bool isDoorOpen(const Ice::Current&);

protected:


    Ice::CommunicatorPtr comm_;
    Ice::PropertiesPtr props_;

    // Recursive mutex to protect the critical parts of the code.
    IceUtil::RecMutex _mutex;

    void throwLatException(std::string what);
    void logNotice(std::string);
    void logError(std::string);

private:
    std::string controlHost_;
    unsigned controlPort_;
    unsigned controlUwait_;
    std::string camIP_;
    Camera camera_;

    std::queue<Image> imageRoll_;

};

};

#endif
