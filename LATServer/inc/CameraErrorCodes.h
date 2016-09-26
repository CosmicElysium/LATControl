/*
 * $Id: CameraErrorCodes.h 7776 2016-04-12 15:44:33Z borelli $
 */
#ifndef __CAMERA_ERROR_CODES_H_
#define __CAMERA_ERROR_CODES_H_

#include<iostream>
#include <map>

namespace lbtlat {

typedef std::map<int ,std::string> MapList;

class CameraErrorCodes
{
public:

    CameraErrorCodes();
    virtual ~CameraErrorCodes();

    std::string getCodeDescription(const int code);

protected:

    MapList list_;
    void createCodeList();
};

}

#endif
