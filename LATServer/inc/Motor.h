#ifndef MOTOR_H
#define MOTOR_H

#include "CommControl.h"


namespace lbtlat
{

class Motor : public CommControl
{
    public:
        Motor(std::string,unsigned,unsigned);
        virtual ~Motor();

        void AbsMove(int);
        void RelMove(int);
        void Reset();
        void Stop();
        int GetPos();

    protected:
        bool StartSwitch();
        bool EndSwitch();
        bool PosError();
        bool IsReady();
        bool IsReferenced();

    private:

        void ThrowMotorException(std::string);
        void ThrowBoundsException(std::string,int);

};
};

#endif // MOTOR_H
