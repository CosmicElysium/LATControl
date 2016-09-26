/*
 * Camera.cpp
 *
 *  Created on: Jun 9, 2015
 *      Author: sivi
 */

#include "Camera.h"
#include <sstream>
#include <assert.h>
#include <cstring>
#include "DeviceException.h"
#include "CameraErrorCodes.h"
#include <arpa/inet.h>


#include <unistd.h>
#include <time.h>

#define HEIGHTPIX 2048
#define WIDTHPIX 2048


#define CAMOPENTRIES 3

namespace lbtlat
{

Camera::Camera()
{
    isApiInitialized_= false;
    isCameraOpen_= false;
    isCameraStarted_= false;
}

void Camera::init()
{
    if (!isApiInitialized_)
    {
        if((errCode_ = PvInitialize()) != ePvErrSuccess)
        {
            isApiInitialized_= false;
            throwCamException("PvInitialize error", errCode_);
        }
        isApiInitialized_= true;
    }
}

Camera::~Camera()
{
    this->cameraStop();
    this->cameraUnsetup();

    if (isApiInitialized_)
        PvUnInitialize();
}


void Camera::sleep(unsigned int time)
{
    struct timespec t,r;

    t.tv_sec    = time / 1000;
    t.tv_nsec   = (time % 1000) * 1000000;

    while(nanosleep(&t,&r)==-1)
        t = r;
}

// open the camera
void Camera::cameraSetup(ULONG camIP)
{
    this->init();
    if (!isCameraOpen_)
    {
        unsigned tries = CAMOPENTRIES;
        do {
            errCode_ = PvCameraOpenByAddr(camIP,ePvAccessMaster,&(handle_));

        } while (errCode_!= ePvErrSuccess && tries-- != 0);

        if (errCode_ != ePvErrSuccess)
            throwCamException("Failed to open camera",errCode_);

        if ((errCode_ = PvAttrEnumSet(handle_,"PixelFormat", "Mono16")) != ePvErrSuccess)
        {
            PvCameraClose(handle_);
            isCameraOpen_= false;
            throwCamException("Failed to setup camera",errCode_);
        }
        isCameraOpen_= true;
    }
}

// setup and start streaming
void Camera::cameraStart()
{
    if (isApiInitialized_)
    {
        if(isCameraOpen_)
        {
            if (!isCameraStarted_)
            {
                if((errCode_ = PvCaptureStart(handle_)) != ePvErrSuccess)
                    throwCamException("Camera failed to start capturing",errCode_);

                if ((errCode_ = PvAttrEnumSet(handle_,"FrameStartTriggerMode","Freerun")) != ePvErrSuccess)
                    throwCamException("Camera failed to enter Freerun",errCode_);

                // and set the acquisition mode into continuous
                if((errCode_ = PvCommandRun(handle_,"AcquisitionStart")) != ePvErrSuccess)
                {
                    // if that fail, we reset the camera to non capture mode
                    PvCaptureEnd(handle_);
                    isCameraStarted_= false;
                    throwCamException("Failed to start frame acquisition", errCode_);
                }
                isCameraStarted_= true;
            }
        }
        else
            throwCamException("Camera is not open", 999);
    }
    else
        throwCamException("Camera not initialized", 999);
}

Image Camera::getImage()
{
    Image currentFrame;
    tPvFrame frame_;
    memset(&frame_, 0, sizeof(frame_));
    unsigned long FrameSize = 0;

    if (isApiInitialized_)
    {
        if (isCameraOpen_)
        {
            if (isCameraStarted_)
            {
                if(!PvAttrUint32Get(handle_,"TotalBytesPerFrame",&FrameSize))
                {
                    std::vector<USHORT> imageBufferVec (FrameSize/2,0);
                    frame_.ImageBuffer = &imageBufferVec[0];
                    if (frame_.ImageBuffer)
                        frame_.ImageBufferSize = FrameSize;
                    else
                        throwCamException("Failed to set frame Image Buffer",999);

                    if((errCode_ = PvCaptureQueueFrame(handle_,&(frame_),NULL)) != ePvErrSuccess)
                        throwCamException("failed to enqueue the frame",errCode_);
                    else
                    {
                        //waiting for the frame to be done ...
                        while(PvCaptureWaitForFrameDone(handle_,&(frame_),PVINFINITE) == ePvErrTimeout)
                        {
                            //still waiting...
                        }
                        if(frame_.Status != ePvErrSuccess)
                            throwCamException("the frame failed to be captured",frame_.Status);
                        else
                        {
                            currentFrame.dataVector = imageBufferVec;
                            currentFrame.height = getHeight();
                            currentFrame.width = getWidth();
                            currentFrame.expTime = getExp();
                            currentFrame.binX = getBinX();
                            currentFrame.binY = getBinY();
                        }
                    }
                }
            }
            else
                throwCamException("Camera is not running", 999);
        }
        else
            throwCamException("Camera is not open", 999);
    }
    else
        throwCamException("Camera not initialized", 999);

    return currentFrame;
}


// stop streaming
void Camera::cameraStop()
{
    if (isCameraStarted_)
    {
        if ((errCode_ = PvCommandRun(handle_,"AcquisitionStop")) != ePvErrSuccess)
            throwCamException("Failed to stop acquisition",errCode_);
        if ((errCode_ = PvCaptureEnd(handle_)) != ePvErrSuccess)
            throwCamException("Failed to end capture",errCode_);
        isCameraStarted_= false;
    }
}


// unsetup the camera
void Camera::cameraUnsetup()
{
    if (isCameraOpen_)
    {
        if ((errCode_ = PvCameraClose(handle_)) != ePvErrSuccess)
            throwCamException("Failed to close camera",errCode_);
        isCameraOpen_= false;
    }
}

void Camera::setExp(unsigned newExp)
{
    if (1/getFRate() < newExp/1000000.0)
        throwCamException("Inverse of frame rate cannot be less than exposure time!",999);
    if ((errCode_ = PvAttrUint32Set(handle_, "ExposureValue", newExp)) != ePvErrSuccess)
        throwCamException("Error setting exposure time",errCode_);
}

void Camera::setBinX(unsigned newBinX)
{
    if ((errCode_ = PvAttrUint32Set(handle_, "BinningX", 1)) != ePvErrSuccess)
        throwCamException("Error setting BinningX",errCode_);
    if ((errCode_ = PvAttrUint32Set(handle_, "Width", WIDTHPIX)) != ePvErrSuccess)
            throwCamException("Error setting BinningX",errCode_);
    if ((errCode_ = PvAttrUint32Set(handle_, "BinningX", newBinX)) != ePvErrSuccess)
        throwCamException("Error setting BinningX",errCode_);
}

void Camera::setBinY(unsigned newBinY)
{
    if ((errCode_ = PvAttrUint32Set(handle_, "BinningY", 1)) != ePvErrSuccess)
            throwCamException("Error setting BinningY",errCode_);
    if ((errCode_ = PvAttrUint32Set(handle_, "Height", HEIGHTPIX)) != ePvErrSuccess)
                throwCamException("Error setting BinningY",errCode_);
    if ((errCode_ = PvAttrUint32Set(handle_, "BinningY", newBinY)) != ePvErrSuccess)
        throwCamException("Error setting BinningY",errCode_);
}

void Camera::setFRate(float newFRate)
{
    if (1/newFRate < getExp()/1000000.0)
        throwCamException("Inverse of frame rate cannot be less than exposure time!",999);
    if ((errCode_ = PvAttrFloat32Set(handle_, "FrameRate", newFRate)) != ePvErrSuccess)
        throwCamException("Error setting framerate",errCode_);
}

unsigned Camera::getExp()
{
    long unsigned exp;
    errCode_ = PvAttrUint32Get(handle_,"ExposureValue",&exp);
    if(errCode_ != ePvErrSuccess)
        throwCamException("ERROR finding Exp!\n",errCode_);
    return exp;
}

unsigned Camera::getBinX() {
    long unsigned binX;
    errCode_ = PvAttrUint32Get(handle_,"BinningX",&binX);
    if(errCode_ != ePvErrSuccess)
        throwCamException("ERROR finding BinningX!\n",errCode_);
    return binX;
}

unsigned Camera::getBinY() {
    long unsigned binY;
    errCode_ = PvAttrUint32Get(handle_,"BinningY",&binY);
    if(errCode_ != ePvErrSuccess)
        throwCamException("ERROR finding BinningY!\n",errCode_);
    return binY;
}

float Camera::getFRate() {       //remember 1/exposure dealio!!
    float fRate;
    errCode_ = PvAttrFloat32Get(handle_,"FrameRate",&fRate);
    if(errCode_ != ePvErrSuccess)
        throwCamException("ERROR finding FrameRate!\n",errCode_);
    return fRate;
}

unsigned Camera::getWidth()
{
    long unsigned width;
    errCode_ = PvAttrUint32Get(handle_,"Height",&width);
    if(errCode_ != ePvErrSuccess)
        throwCamException("ERROR finding Width!\n",errCode_);
    return width;
}

unsigned Camera::getHeight()
{
    long unsigned height;
    errCode_ = PvAttrUint32Get(handle_,"Width",&height);
    if(errCode_ != ePvErrSuccess)
        throwCamException("ERROR finding Height!\n",errCode_);
    return height;
}

void Camera::throwCamException(std::string what, int code)
{
    CameraErrorCodes camError;

    std::ostringstream oss;
    oss << what << ": " << camError.getCodeDescription(code)
        << "(#" << code << ")";
    std::string msg = oss.str();
    throw DeviceException(msg);
}

}
