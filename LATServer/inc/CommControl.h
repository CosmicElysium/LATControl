#ifndef COMMCONTROL_H
#define COMMCONTROL_H

#include "Sockets.h"


namespace lbtlat {

class CommControl : private Sockets
{
    public:
        CommControl(std::string,unsigned,unsigned);
        virtual ~CommControl();
    protected:
        std::string Comm(const std::string&);
        bool status_;
    private:
        Sockets sock_;
        std::string serverIP_;
        unsigned int port_;
        unsigned int uwait_;

        void throwCommException(std::string);
};

}
#endif // COMMCONTROL_H
