/*
 * $Id: CameraErrorCodes.cpp 7777 2016-04-13 09:06:01Z borelli $
 */

#include <iostream>
#include "CameraErrorCodes.h"
#include <PvApi.h>

using namespace lbtlat;

CameraErrorCodes::CameraErrorCodes()
{
    this->createCodeList();
}

CameraErrorCodes::~CameraErrorCodes()
{

}

void CameraErrorCodes::createCodeList()
{
    list_[0]= "No error";
    list_[1]= "Unexpected camera fault";
    list_[2]= "Unexpected fault in PvApi or driver";
    list_[3]= "Camera handle is invalid";
    list_[4]= "Bad parameter to API call";
    list_[5]= "Sequence of API calls is incorrect";
    list_[6]= "Camera or attribute not found";
    list_[7]= "Camera cannot be opened in the specified mode";
    list_[8]= "Camera was unplugged. LAT service must be restarted";
    list_[9]= "Setup is invalid (an attribute is invalid)";
    list_[10]= "System/network resources or memory not available";
    list_[11]= "1394 bandwidth not available";
    list_[12]= "Too many frames on queue";
    list_[13]= "Frame buffer is too small";
    list_[14]= "Frame cancelled by user";
    list_[15]= "The data for the frame was lost";
    list_[16]= "Some data in the frame is missing";
    list_[17]= "Timeout during wait";
    list_[18]= "Attribute value is out of the expected range";
    list_[19]= "Attribute is not this type (wrong access function)";
    list_[20]= "Attribute write forbidden at this time";
    list_[21]= "Attribute is not available at this time";
    list_[22]= "A firewall is blocking the traffic (Windows only)";
}

std::string CameraErrorCodes::getCodeDescription(const int code)
{
    MapList::iterator it;
    std::string msg;

    it= list_.find(code);

    if (it == list_.end())
    {
        msg= "Not listed error code";
    }
    else
    {
        msg= it->second;
    }
        return msg;
}
