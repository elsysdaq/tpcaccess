
CC       = gcc
CXX      = g++
CFLAGS   = -fPIC -DPIC -pipe -O3 -Wall -D_LINUX -DXML_LIBRARY -fomit-frame-pointer
CXXFLAGS = -fPIC -DPIC -pipe -O3 -Wall -D_LINUX -DXML_LIBRARY -fomit-frame-pointer
INCPATH  = -I. -I../tpcserver/tpcxcommon/gSOAP -I../tpcserver/tpcxcommon/gSOAPgenerated -I../tpcserver/tpcxcommon/Include -I../tpcserver/tpcxcommon/SysDep
LINK     = g++
LFLAGS   = -shared -Wl,-soname,libtpcaccess.so.1
LIBS     = -lpthread -lxerces-c
LIBDIR	 = /usr/lib

OBJECTS = stdsoap2.o \
		soapC.o \
		soapClient.o \
		sysThreading.o \
		main.o \
		Attributes.o \
		DataReceiver.o \
		Device.o \
		DeviceInfo.o \
		ParameterSet.o \
		SoapDevice.o \
		System.o \
		SystemList.o \
		TpcAccess.o \
		YMetaData.o \
		AssocRow.o \
		StringConversions.o \
		TpcSettings.o \
		TpcSettingsFromXml.o \
		TpcSettingsToXml.o \
		xerces_link.o \
		XercesUtils.o

DESTDIR  = 
TARGET   = libtpcaccess.so



first: all
####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(UICDECLS) $(OBJECTS) $(OBJMOC)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

clean:
	rm -f $(OBJECTS)
	rm -f *~ core *.core

FORCE:

install: $(TARGET)
	install -D $(TARGET) $(LIBDIR)/$(TARGET)
	ldconfig

####### Compile

stdsoap2.o: ../tpcserver/tpcxcommon/gSOAP/stdsoap2.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o stdsoap2.o ../tpcserver/tpcxcommon/gSOAP/stdsoap2.cpp

soapC.o: ../tpcserver/tpcxcommon/gSOAPgenerated/soapC.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o soapC.o ../tpcserver/tpcxcommon/gSOAPgenerated/soapC.cpp

soapClient.o: ../tpcserver/tpcxcommon/gSOAPgenerated/soapClient.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o soapClient.o ../tpcserver/tpcxcommon/gSOAPgenerated/soapClient.cpp

sysThreading.o: ../tpcserver/tpcxcommon/SysDep/sysThreading.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o sysThreading.o ../tpcserver/tpcxcommon/SysDep/sysThreading.cpp

main.o: TpcAccess/main.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o TpcAccess/main.cpp

Attributes.o: TpcAccess/Attributes.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Attributes.o TpcAccess/Attributes.cpp

DataReceiver.o: TpcAccess/DataReceiver.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o DataReceiver.o TpcAccess/DataReceiver.cpp

Device.o: TpcAccess/Device.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o Device.o TpcAccess/Device.cpp

DeviceInfo.o: TpcAccess/DeviceInfo.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o DeviceInfo.o TpcAccess/DeviceInfo.cpp

ParameterSet.o: TpcAccess/ParameterSet.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ParameterSet.o TpcAccess/ParameterSet.cpp

SoapDevice.o: TpcAccess/SoapDevice.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o SoapDevice.o TpcAccess/SoapDevice.cpp

System.o: TpcAccess/System.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o System.o TpcAccess/System.cpp

SystemList.o: TpcAccess/SystemList.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o SystemList.o TpcAccess/SystemList.cpp

TpcAccess.o: TpcAccess/TpcAccess.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o TpcAccess.o TpcAccess/TpcAccess.cpp

YMetaData.o: TpcAccess/YMetaData.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o YMetaData.o TpcAccess/YMetaData.cpp

AssocRow.o: TpcAccess/AssocRow.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o AssocRow.o TpcAccess/AssocRow.cpp

StringConversions.o: TpcAccess/StringConversions.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o StringConversions.o TpcAccess/StringConversions.cpp

TpcSettings.o: TpcAccess/TpcSettings.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o TpcSettings.o TpcAccess/TpcSettings.cpp

TpcSettingsFromXml.o: TpcAccess/TpcSettingsFromXml.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o TpcSettingsFromXml.o TpcAccess/TpcSettingsFromXml.cpp

TpcSettingsToXml.o: TpcAccess/TpcSettingsToXml.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o TpcSettingsToXml.o TpcAccess/TpcSettingsToXml.cpp

xerces_link.o: TpcAccess/xerces_link.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o xerces_link.o TpcAccess/xerces_link.cpp

XercesUtils.o: TpcAccess/XercesUtils.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o XercesUtils.o TpcAccess/XercesUtils.cpp
