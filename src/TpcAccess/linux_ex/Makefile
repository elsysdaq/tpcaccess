
CC       = gcc
CXX      = g++
CFLAGS   = -pipe -O3 -Wall -D_LINUX 
CXXFLAGS = -pipe -O3 -Wall -D_LINUX 
INCPATH  = -I. -I../../tpcserver/tpcxcommon/Include -I ../../tpcserver/tpcxcommon/SysDep
LINK     = g++
LFLAGS   =  
LIBS     = -lpthread  -lxerces-c -ltpcaccess

OBJECTS = simpleTest.o \

TARGET   = SimpleTest

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

all: Makefile $(TARGET)

$(TARGET):  $(UICDECLS) $(OBJECTS) $(OBJMOC)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)




clean:
	rm -f $(OBJECTS)
	rm -f *~ core *.core
