#ifndef DEVICEEXCEPTION_H
#define DEVICEEXCEPTION_H

#include <exception>

namespace lbtlat {


class DeviceException: public std::exception
{
    public:
        DeviceException() throw () {};
        DeviceException ( const std::string & msg ) throw () : m_What ( msg ) {};
        ~DeviceException() throw () {};
        virtual const char *what () const throw ()
        {
            return m_What.c_str ();
        };

    protected:
        std::string m_What;
};


}

#endif // DEVICEEXCEPTION_H



