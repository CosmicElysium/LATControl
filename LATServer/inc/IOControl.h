#ifndef IOCONTROL_H
#define IOCONTROL_H

#include "CommControl.h"


namespace lbtlat
{

class IOControl : public CommControl
{
    public:
        IOControl(std::string,unsigned,unsigned);
        virtual ~IOControl();
        void shutterOpen();
        void shutterClose();
        bool shutterStatus();
        void camPowerOn();
        void camPowerOff();
        bool camPowerStatus();
    protected:
    private:
};

}

#endif // IOCONTROL_H
