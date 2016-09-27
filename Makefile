#OS architecture. x86 or x64
ARCH = x64

#g++ compiler version. 4.1 to 4.8
#COMPILER = 4.8
#COMPILER = 5.2
COMPILER = 4.4

#LINUX or WINDOWS
OS = LINUX



OBJSHARE = lat.o
OBJCLI = Client.o FitsTool.o menu.o
OBJSERV = Server.o Camera.o CommControl.o IOControl.o LatI.o Motor.o Sockets.o CameraErrorCodes.o

DEP = lat.h
DEPCLI = FitsTool.h menu.h lat.h
DEPSERV = Camera.h CommControl.h IOControl.h LatI.h Motor.h Sockets.h CameraErrorCodes.h

CXXFLAGS = -Wall -fexceptions -g -Ishare
CXXFLAGSCLI = -ILATClient/inc -Icfits
CXXFLAGSSERV = -ILATServer/inc -D_$(OS) -D_$(ARCH)

LDLIBS = -lIce -lIceUtil -lpthread -lrt
LDLIBSCLI =  -lcfitsio
LDLIBSSERV = -lPvAPI

LDFLAGS =
LDFLAGSCLI =
LDFLAGSSERV = -LLATServer/lib/library/$(ARCH)/$(COMPILER)

SRCDIR = share/src
SRCDIRCLI = LATClient/src
SRCDIRSERV = LATServer/src

ODIR = share/obj
ODIRCLI = LATClient/obj
ODIRSERV = LATServer/obj

DIRSHARE = share
DIRCLI = LATClient
DIRSERV = LATServer

VPATH = share:LATServer/obj:LATServer/src:LATServer/inc:LATClient/obj:LATClient/src:LATClient/inc:share

all: LATServer LATClient

LATServer : $(OBJSERV) $(OBJSHARE)
	$(CXX) -o LATServer/bin/LATServer LATServer/obj/Server.o LATServer/obj/Sockets.o LATServer/obj/Motor.o LATServer/obj/LatI.o LATServer/obj/IOControl.o LATServer/obj/CameraErrorCodes.o LATServer/obj/CommControl.o LATServer/obj/Camera.o share/obj/lat.o $(LDFLAGSSERV) $(LDFLAGS) $(LDLIBSSERV) $(LDLIBS)

LATClient : $(OBJCLI) $(OBJSHARE)
	$(CXX) -o LATClient/bin/LATClient share/obj/lat.o LATClient/obj/Client.o LATClient/obj/FitsTool.o LATClient/obj/menu.o $(LDFLAGSCLI) $(LDFLAGS) $(LDLIBSCLI) $(LDLIBS)

Server.o : Server.cpp LatI.h lat.h
	$(CXX) $(CXXFLAGS) $(CXXFLAGSSERV) -c LATServer/src/Server.cpp -o LATServer/obj/Server.o

CameraErrorCodes.o: CameraErrorCodes.cpp CameraErrorCodes.h
	$(CXX) $(CXXFLAGS) $(CXXFLAGSSERV) -c LATServer/src/CameraErrorCodes.cpp -o LATServer/obj/CameraErrorCodes.o

Camera.o : LATServer/src/Camera.cpp Camera.h DeviceException.h CameraErrorCodes.h
	$(CXX) $(CXXFLAGS) $(CXXFLAGSSERV) -c LATServer/src/Camera.cpp -o LATServer/obj/Camera.o

CommControl.o : CommControl.cpp CommControl.h DeviceException.h Sockets.h
	$(CXX) $(CXXFLAGS) $(CXXFLAGSSERV) -c LATServer/src/CommControl.cpp -o LATServer/obj/CommControl.o
	
IOControl.o : IOControl.cpp IOControl.h CommControl.h
	$(CXX) $(CXXFLAGS) $(CXXFLAGSSERV) -c LATServer/src/IOControl.cpp -o LATServer/obj/IOControl.o

LatI.o : LatI.cpp LatI.h DeviceException.h lat.h Motor.h IOControl.h CommControl.h
	$(CXX) $(CXXFLAGS) $(CXXFLAGSSERV) -c LATServer/src/LatI.cpp -o LATServer/obj/LatI.o

Motor.o : Motor.cpp Motor.h CommControl.h
	$(CXX) $(CXXFLAGS) $(CXXFLAGSSERV) -c LATServer/src/Motor.cpp -o LATServer/obj/Motor.o

Sockets.o : Sockets.cpp Sockets.h
	$(CXX) $(CXXFLAGS) $(CXXFLAGSSERV) -c LATServer/src/Sockets.cpp -o LATServer/obj/Sockets.o

menu.o : menu.cpp menu.h
	$(CXX) $(CXXFLAGS) $(CXXFLAGSCLI) -c LATClient/src/menu.cpp -o LATClient/obj/menu.o

FitsTool.o : FitsTool.cpp FitsTool.h
	$(CXX) $(CXXFLAGS) $(CXXFLAGSCLI) -c LATClient/src/FitsTool.cpp -o LATClient/obj/FitsTool.o

Client.o : Client.cpp lat.h FitsTool.h menu.h
	$(CXX) $(CXXFLAGS) $(CXXFLAGSCLI) -c LATClient/src/Client.cpp -o LATClient/obj/Client.o
	
lat.o : lat.cpp lat.h
	$(CXX) $(CXXFLAGS) -Ishare -c share/lat.cpp -o share/obj/lat.o

lat.cpp lat.h : lat.ice
	slice2cpp share/lat.ice --output-dir share

.PHONY: clean

clean :
	-rm LATClient/bin/LATClient LATServer/bin/LATServer share/lat.h share/lat.cpp share/obj/lat.o LATClient/obj/Client.o LATClient/obj/FitsTool.o LATClient/obj/menu.o LATServer/obj/Server.o LATServer/obj/Camera.o LATServer/obj/CommControl.o LATServer/obj/IOControl.o LATServer/obj/LatI.o LATServer/obj/Motor.o LATServer/obj/Sockets.o LATServer/obj/CameraErrorCodes.o


