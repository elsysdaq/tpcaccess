//---------------------------------------------------------------------------
/*
 *
 * ELSYS EXPRESSLY DISCLAIMS ALL WARRANITIES OR REPRESENTATIONS WITH RESPECT TO
 * THIS SOFTWARE AND DOCUMENTATION AND ANY SUPPORT OR MAINTENANCE SERVICES THAT
 * ELSYS MAY CHOOSE TO PROVIDE IN RELATION THERETO (INCLUDING, WITHOUT
 * LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE OR THAT THE SOFTWARE: WILL BE
 * ERROR-FREE, WILL OPERATE WITHOUT INTERUPTION, WILL NOT INFRINGE THE RIGHTS OF A
 * THIRD PARTY, OR WILL PRODUCE RESULTS IN CONNECTION WITH ITS USE). FURTHER,
 * ELSYS EXPRESSLY DISCLAIMS ALL EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * (C) Copyright 2005 - 2019 Elsys AG. All rights reserved.
*/

//---------------------------------------------------------------------------
/*--------------------------------------------------------------------------------
  $Id: SoapDevice.h 28 2017-06-06 13:31:56Z roman $
  Thin layer over the gSOAP connection to the remote device.
--------------------------------------------------------------------------------*/
#ifndef SoapDevice_h
#define SoapDevice_h SoapDevice_h
//---------------------------------------------------------------------------------

#include <string>
using namespace std;

#include "soapH.h" 

#include "TpcAccess.h"
#include "TpcAccessElsys.h"

#include "DeviceInfo.h"
#include "ParameterSet.h"
#include "Attributes.h"
#include "DataReceiver.h"
#include "YMetaData.h"
#include "AssocRow.h"

//---------------------------------------------------------------------------------

class SoapDevice
{
public:
	SoapDevice(const string& url, int recvTimeOut, int sendTimeOut);
	~SoapDevice();
    int KillSocket();

public: // Initialization 
	TPC_ErrorCode GetInterfaceVersion(int& version);
	TPC_ErrorCode ResetConfiguration();

public:
  TPC_ErrorCode AcquireDeviceLock();
  TPC_ErrorCode ReleaseDeviceLock();

public: // Cluster configuration 
	TPC_ErrorCode GetClusterConfiguration(int* clusters);
	TPC_ErrorCode SetClusterConfiguration(int* clusters);

public: // Hardware info 
	TPC_ErrorCode GetHardwareInfo(DeviceInfo& deviceInfo);
	TPC_ErrorCode SetHardwareSettings(TPC_DeviceInfo deviceInfo);
	TPC_ErrorCode GetFreeDiskSpace(uint64_t* freeDiskSpace, uint64_t* DiskSize);
public: // Recording parameters 
	TPC_ErrorCode SetOneParameter(int boardAddress, int inputNumber, int parameterIndex, 
						double value, double* roundedValue, int* error);

	TPC_ErrorCode SetMultipleParameters(ParameterModificationList& parameters);

	TPC_ErrorCode GetAllCurrentParameters(ParameterSet& parameters);
	TPC_ErrorCode GetAllPreviousParameters(int currentMeasurementNumber, ParameterSet& parameters);

public: // Serial protocol trigger
	TPC_ErrorCode ResetSerTrg(int boardAddress);
	TPC_ErrorCode GetSerTrgProtocol(int boardAddress, TPC_SerialTriggerProtocol *protocol);
	TPC_ErrorCode SetI2CTrigger(int boardAddress, TPC_I2CTriggerSettings *settings);
	TPC_ErrorCode GetI2CTrigger(int boardAddress, TPC_I2CTriggerSettings *settings);
	TPC_ErrorCode SetCANTrigger(int boardAddress, TPC_CANTriggerSettings *settings);
	TPC_ErrorCode GetCANTrigger(int boardAddress, TPC_CANTriggerSettings *settings);

public: // Attributes 
	TPC_ErrorCode SetAttributes(const Attributes& attributes);
	TPC_ErrorCode GetAllCurrentAttributes(Attributes& result);
	TPC_ErrorCode GetAllPreviousAttributes(int currentMeasurementNumber, Attributes& result);

public: // Associations
	TPC_ErrorCode GetCurrentAssociations(AssociationRowList& associations);
	TPC_ErrorCode GetAllPreviousAssociations(int measurementNumber, AssociationRowList& associations);
	TPC_ErrorCode SetAssociations(const AssociationRowList& associations);

public: // Read/Write Device 
	TPC_ErrorCode WriteDev(int boardAddress, int type, int count, void* data, unsigned aux1, unsigned aux2);
	TPC_ErrorCode ReadDev(int boardAddress, int type, int count, void* data,	unsigned aux1, unsigned aux2);
  TPC_ErrorCode ReadWriteTwi(int boardAddress, int amplifier, int count, char *data);

public: // Commands 
	TPC_ErrorCode PrepareStart(double* delayTime);
	TPC_ErrorCode SetStartInfo(const TPC_DateTime& startTime, int measurementNr);
	TPC_ErrorCode ExecuteCommand(TPC_SystemCommand command);
	TPC_ErrorCode StartCalibration(TPC_CalibrationType type);

public: // Device state 
	TPC_ErrorCode GetStatus(ns__Status& status);
	TPC_ErrorCode WaitForStatusChange(ns__Status& status);
	TPC_ErrorCode LeaveWaitStatus();

public: // Data readout 
	TPC_ErrorCode GetAllYTMetadata(int currentMeasurementNumber, YMetaDataList& metadata);

	TPC_ErrorCode GetMetadata(int currentMeasurementNumber, int boardAddress, int blockFrom, int blockTo, ns__Metadata metadata[]);
	TPC_ErrorCode GetData(int currentMeasurementNumber, DataReceiver* receiver);

public:
  TPC_ErrorCode SendServerCustomCommand(char *command, char *result, int length);

public: // Auto Sequences
	TPC_ErrorCode LoadAutosequence( char * cData, int iLength);
	TPC_ErrorCode GetAutoSequence(	char * cData, int iLength);
	TPC_ErrorCode StartAutoSequence();
	TPC_ErrorCode StopAutoSequence(	);

	TPC_ErrorCode GetGPSLogList(int from, int to, char *log);
private:
	struct soap m_soap;  // main gSOAP runtime environment
	string m_url;

	TPC_ErrorCode CheckSoapError();
	TPC_ErrorCode Epilog();
	TPC_ErrorCode Epilog(TPC_ErrorCode err);
};


void Assign(TPC_DateTime& d, const ns__DateTime& s);


//---------------------------------------------------------------------------------
#endif // SoapDevice_h

