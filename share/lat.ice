//Laser Aligment Telescope Interface
//Draft version.

#ifndef LBT_LAT
#define LBT_LAT

module lbtlat
{

exception LatException
{
	int code;
	string what;
};


sequence<short> SeqShort;

struct Frame
{
    SeqShort ImageVector;
    int expTimeInMicroseconds;
    int width;
    int height;
    int binningXFactor;
    int binningYFactor;
    int focPosInSteps;
    string target;

};


interface Lat
{
    void turnCameraOn() throws LatException;
    void turnCameraOff() throws LatException;
    idempotent bool isCameraOn()throws LatException;
    
    void initCamera() throws LatException;
    Frame getImage() throws LatException;

	idempotent int getHeight() throws LatException;
	idempotent int getWidth() throws LatException;
	idempotent int getExpTimeInMicroseconds() throws LatException;
	idempotent int getBinningXFactor() throws LatException;
	idempotent int getBinningYFactor() throws LatException;
	void setExpTime(int expTimeInMicroseconds) throws LatException;
	void setBinningX(int factor) throws LatException;
	void setBinningY(int factor) throws LatException;

    void openDoor() throws LatException;
    void closeDoor() throws LatException;
    idempotent bool isDoorOpen() throws LatException;

    void moveFocusAbs(int positionInSteps) throws LatException;
    void moveFocusRel(int steps) throws LatException;
    void stopFocus() throws LatException;
    void focusHome() throws LatException;
    idempotent int getFocusPositionInSteps() throws LatException;


};
};
#endif

