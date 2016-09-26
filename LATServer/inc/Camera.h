/*
 * Camera.h
 *
 *  Created on: Jun 9, 2015
 *      Author: sivi
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <iostream>
#include <PvApi.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <stdint.h>

#define USHORT uint16_t
#define SHORT int16_t
#define ULONG uint32_t


namespace lbtlat
{

struct Image
{
    std::vector<USHORT> dataVector;
    ULONG height;
    ULONG width;
    ULONG expTime;
    ULONG binX;
    ULONG binY;
};

class Camera
{
public:
    Camera();
    virtual ~Camera();
    void cameraSetup(ULONG);
    void init();
    void cameraStart();
    Image getImage();
    void cameraStop();
    void cameraSnap(unsigned);
    void cameraUnsetup();
    void setExp(unsigned);
    void setBinX(unsigned);
    void setBinY(unsigned);
    void setFRate(float);
    unsigned getExp();
    unsigned getBinX();
    unsigned getBinY();
    float getFRate();
    unsigned getHeight();
    unsigned getWidth();

private:
    tPvErr errCode_;
    tPvHandle handle_;
    bool isApiInitialized_;
    bool isCameraOpen_;
    bool isCameraStarted_;

    //utilities
    void sleep(unsigned);
    void throwCamException(std::string,int);
};

}


#endif /* CAMERA_H_ */
