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
 * (C) Copyright 2005 - 2024 Elsys AG. All rights reserved.
*/
//---------------------------------------------------------------------------
// $Id: TpcAccess.cpp 37 2024-01-29 15:58:48Z roman $

#define BUILDING_TPC_ACCESS 
#include "System.h"
#include "TpcAccess.h"
#include "TpcAccessElsys.h"
#include "TpcAccessA.h"
#include "version.h"

#include "sysThreading.h"

#include "soapH.h" 

#include <string>
using namespace std;

#include "Device.h"

#include "SystemList.h"
#ifndef NOSETTINGSFILE
#include "TpcSettings.h"
#endif
//---------------------------------------------------------------------------------

/// Copy string to char buffer
static TPC_ErrorCode CopyString(const string& s, char* buffer, int maxLen)
{
	TPC_ErrorCode retVal = tpc_noError;

	// Determine number of characters to copy.
	int n = s.size();
	if (n > maxLen-1) {
		n = maxLen-1;
		retVal = tpc_errBufferTooShort;
	}

	// Copy the string
	const char* q = s.c_str();
	char* p = buffer;
	for (int i=0; i<n; i++) *p++ = *q++;
	*p = '\0';

	return retVal;
}

/// Copy struct with size check for preventing buffer overflow 
static void CopyStruct(void* dest, int destSize, const void* source, int sourceSize)
{
	int m = sourceSize;
	if (m > destSize) m = destSize;
	memcpy(dest, source, m);

	int d = destSize - m;
	if (d > 0) {
		memset(((char*)dest) + m, 0, d);
	}
}

/// Simplified Interface for Python 
TPC_EXP TPC_ErrorCode  TPC_CC TPC_GetInputRange(int deviceIx, int boardAddress, int inputNumber, int RangeIdx, double* value) {
	if (RangeIdx >= tpc_maxInputRanges)
		return tpc_errInvalidIndex;

	TPC_InputInfo inputInfo;
	System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
	if (s == NULL) return tpc_errInvalidIndex;

	deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	s->GetInputInfo(deviceIx, boardAddress, inputNumber, &inputInfo, sizeof(TPC_InputInfo));

	*value =  inputInfo.inputRanges[RangeIdx];
	return tpc_noError;
}

/// Simplified Interface for Python 
TPC_EXP TPC_ErrorCode  TPC_CC TPC_GetChargeInputRange(int deviceIx, int boardAddress, int inputNumber, int RangeIdx, double* value) {
	if (RangeIdx >= tpc_maxChargeInputRanges)
		return tpc_errInvalidIndex;

	TPC_InputInfo inputInfo;
	System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
	if (s == NULL) return tpc_errInvalidIndex;
	deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	s->GetInputInfo(deviceIx, boardAddress, inputNumber, &inputInfo, sizeof(TPC_InputInfo));

	*value = inputInfo.chargeInputRanges[RangeIdx];
	return tpc_noError;
}

/// Direct Board Status Access for Python
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetBoardStatus(int deviceIx, int boardAddress, TPC_BoardStatus* status, int structSize)
{
	System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
	if (s == NULL) return tpc_errInvalidDeviceIx;
	deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	
	TPC_DeviceStatus devStatus; 

	s->GetDeviceStatus(deviceIx, &devStatus, sizeof(TPC_DeviceStatus));

	if (boardAddress > tpc_maxBoards) return tpc_errInvalidBoardAddress;
	
	CopyStruct(status, structSize, &devStatus.boards[boardAddress], sizeof(TPC_BoardStatus));

	return tpc_noError;

}

/// Direct Input Status Access for Python
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetInputStatus(int deviceIx, int boardAddress, int inputNumber, TPC_InputStatus* status, int structSize)
{
	System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
	if (s == NULL) return tpc_errInvalidDeviceIx;
	deviceIx = deviceIx % SYSTEM_MULTIPLIER;

	TPC_DeviceStatus devStatus;
	s->GetDeviceStatus(deviceIx, &devStatus, sizeof(TPC_DeviceStatus));

	if (boardAddress > tpc_maxBoards) return tpc_errInvalidBoardAddress;

	if (inputNumber > tpc_maxInputs) return tpc_errInvalidInputNumber;
	
	CopyStruct(status, structSize, &devStatus.boards[boardAddress].inputs[inputNumber], sizeof(TPC_InputStatus));
	return tpc_noError;

}

/// Generate Error Message as string from error code
TPC_EXP TPC_ErrorCode TPC_CC TPC_ErrorToString(int errorCode, char* errorString, int maxLen)
{
	string s;
	switch (errorCode) {
	case tpc_noError:
		s = "No Error.";
		break;
	case tpc_errCalibrating:
		s = "Device is calibrating.";
		break;
	case tpc_errNoConnection:
		s = "Network connection error.";
		break;
	case tpc_errServerClientVersionConflict:
		s = "Server/client software version conflict.";
		break;
	case tpc_errDeviceInitializationFailure:
		s = "Device initialization failure.";
		break;
	case tpc_errHardwareFailure:
		s = "Hardware failure.";
		break;
	case tpc_errTransmissionError:
		s = "Network transmission error.";
		break;
	case tpc_errStartError:
		s = "Measurement start error.";
		break;
	case tpc_errIncompatibleBoards:
		s = "Incompatible boards.";
		break;
	case tpc_errInvalidDeviceIx:
		s = "Invalid device index.";
		break;
	case tpc_errInvalidBoardAddress:
		s = "Invalid board address.";
		break;
	case tpc_errInvalidInputNumber:
		s = "Invalid input number.";
		break;
	case tpc_errInvalidIndex:
		s = "Invalid index.";
		break;
	case tpc_errBufferTooShort:
		s = "Buffer too short.";
		break;
	case tpc_errInvalidParameter:
		s = "Invalid parameter.";
		break;
	case tpc_errInvalidParameterValue:
		s = "Invalid parameter value.";
		break;
	case tpc_errInvalidCommand:
		s = "Invalid command.";
		break;
	case tpc_errWrongSystemState:
		s = "Wrong system state.";
		break;
	case tpc_errInvalidBlockNumber:
		s = "Invalid block number.";
		break;
	case tpc_errNoData:
		s = "No data available.";
		break;
	case tpc_errNewMeasurement:
		s = "Old data unavailable due to new measurement.";
		break;
	case tpc_errNotImplemented:
		s = "Feature not implemented.";
		break;
	case tpc_errLoadSettingFile:
		s = "Error reading configuration file.";
		break;
	case tpc_errWriteSettingFile:
		s = "Error writing configuration file.";
		break;
	case tpc_errInternalError:
		s = "Internal error.";
		break;
	case tpc_errTimeout:
		s = "Timeout.";
		break;
	case tpc_errMeasurementAborted:
		s = "Measurement aborted.";
		break;

   default:
        s = "Unknown error code";
	}

	TPC_ErrorCode err = CopyString(s, errorString, maxLen);
	return err;
}

/// get API version (obsolete)
TPC_EXP int TPC_CC TPC_GetApiVersion()
{
	// The current version is 1.4. (obsolet)

	return 104;
}

// Get API Version info struct
TPC_EXP TPC_Version TPC_CC TPC_GetVersion(){
	
	TPC_Version version;
	version.major = 1;
	version.minor = 4;
	version.build = 0;
	version.revision = REV_VERSION;
	return version;
}

/// Terminate the TpcAccess connection
TPC_EXP void TPC_CC TPC_TerminateTpcAccess()
{
	System::PrepareToQuitProgram();
}

/// Begin system definition
TPC_EXP TPC_ErrorCode TPC_CC TPC_BeginSystemDefinition()
{
	System::TheSystem()->ClearDeviceList();
	return tpc_noError;
}

/// Add a device defined by a IP address to the system
TPC_EXP int TPC_CC TPC_AddDevice(const char* url)
{
	return System::TheSystem()->AddDevice(url,10,60);
}

/// Add a device defined by a IP address to the system, define separate receive and send timeout 
TPC_EXP int TPC_CC TPC_AddDeviceEx(const char* url, int recvTimeOut, int sendTimeout)
{
	return System::TheSystem()->AddDevice(url,recvTimeOut,sendTimeout);
}

/// Remove a device from the system list
TPC_EXP void TPC_CC TPC_RemoveDevice(const char* url)
{
	return System::TheSystem()->RemoveDevice(url);
}

/// end system generation and conenct to the defined devices
TPC_EXP TPC_ErrorCode TPC_CC TPC_EndSystemDefinition(int connectionTimeoutMs)
{
	int t = 0;
	for(;;) {
		// Check the connection to the devices.
		TPC_ErrorCode err = System::TheSystem()->GetConnectionState();
		if (err != tpc_errNoConnection) return err;

		// Wait a little, then check again until timeout.
		const int deltaT = 50;
		NonWaitableThread::sleep(deltaT);
		t += deltaT;
		if (t > connectionTimeoutMs) return tpc_errNoConnection;
	}
}

/// get the number of devices per system
TPC_EXP int TPC_CC TPC_NumDevices()
{
	return System::TheSystem()->NumDevices();
}

/// get the device url from a defined system
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetDeviceUrl(int index, char* buffer, int maxLen)
{
	string url;
	TPC_ErrorCode err = System::TheSystem()->GetDeviceUrl(index, url);
	if (err != 0) return err;

	err = CopyString(url, buffer, maxLen);
	return err;
}

/// get the device Info structure
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetDeviceInfo(int deviceIx, TPC_DeviceInfo* deviceInfo, int structSize)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetDeviceInfo(deviceIx, deviceInfo, structSize);
}

/// get free disk space on the target device
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetFreeDiskSpace(int deviceIx, uint64_t* freeDiskSpace,
													uint64_t* DiskSize){
	
	System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetFreeDiskSpace(deviceIx,freeDiskSpace,DiskSize);	

}

/// write device settings to the tpcxsrv.ini and tpcxhw.ini files
TPC_EXP TPC_ErrorCode TPC_CC TPC_SetDeviceSettings(int deviceIx, TPC_DeviceInfo deviceSettings){
	System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->SetDeviceSettings(deviceIx, deviceSettings);
}

/// get Board info structure
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetBoardInfo(int deviceIx, int boardAddress, 
					 TPC_BoardInfo* boardInfo, int structSize)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetBoardInfo(deviceIx, boardAddress, boardInfo, structSize);
}

/// get the input info structure
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetInputInfo(int deviceIx, int boardAddress, int inputNumber, 
					 TPC_InputInfo* inputInfo, int structSize)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetInputInfo(deviceIx, boardAddress, inputNumber, inputInfo, structSize);
}

/// set all parameters to default state
TPC_EXP TPC_ErrorCode TPC_CC TPC_ResetConfiguration()
{
	return System::TheSystem()->ResetConfiguration();
}

/// get the used cluster numbers from a device
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetClusterNumbers(int deviceIx, int* clusterNumbers)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetClusterNumbers(deviceIx, clusterNumbers);
}

/// set the cluster configuration
TPC_EXP TPC_ErrorCode TPC_CC TPC_SetClusterNumbers(int deviceIx, int* clusterNumbers)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->SetClusterNumbers(deviceIx, clusterNumbers);
}

/// Returns the first board number of the device which is in the given cluster
TPC_EXP TPC_ErrorCode TPC_CC TPC_ClusterNumberToBoardAddress(int deviceIx, int clusterNumber, int *boardAddress)
{
  System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
  if (s == NULL) return tpc_errInvalidDeviceIx;
  return s->ClusterNumberToBoardAddress(clusterNumber, *boardAddress);
}

/// check wheter the system can be clusteres
TPC_EXP TPC_ErrorCode TPC_CC CanBeClustered(int deviceIx1, int boardAddress1, int deviceIx2, int boardAddress2)
{
    // First check if the two deviceIx are from the same system.
    System* s1 = SystemList::TheSystemList()->FindSystem(deviceIx1);
    System* s2 = SystemList::TheSystemList()->FindSystem(deviceIx2);
    if (s1 != s2) return tpc_errInvalidDeviceIx;

    // Then let this system do the check.
	return s1->CanBeClustered(deviceIx1, boardAddress1, deviceIx2, boardAddress2);
}

/// set a parameter
TPC_EXP TPC_ErrorCode TPC_CC TPC_SetParameter(int deviceIx, int boardAddress, int inputNumber, TPC_Parameter parameter, double value)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->SetParameter(deviceIx, boardAddress, inputNumber, parameter, value);
}

/// get a parameter
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetParameter(int deviceIx, int boardAddress, int inputNumber, TPC_Parameter parameter, double* value)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetParameter(deviceIx, boardAddress, inputNumber, parameter, value);
}

/// set an attribute key-value pair
TPC_EXP TPC_ErrorCode TPC_CC TPC_SetAttribute(int deviceIx, int boardAddress, int inputNumber, const char* key, const char* value)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->SetAttribute(deviceIx, boardAddress, inputNumber, key, value);
}

/// get an attribute value from a key
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetAttribute(int deviceIx, int boardAddress, int inputNumber, const char* key, char* buffer, int maxLen)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;

    string attr;
	TPC_ErrorCode err = s->GetAttribute(deviceIx, boardAddress, inputNumber, key, attr);
	if (err != 0) return err;
	err = CopyString(attr, buffer, maxLen);
	return err;
}

/// get a list of all set attributes
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetAllAttributes(int deviceIx, TPC_AttributeEnumeratorCallback callback, void* userData)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetAllAttributes(deviceIx, callback, userData);
}

/// start a system build configuration
TPC_EXP TPC_ErrorCode TPC_CC TPC_BeginSet()
{
	return System::TheSystem()->BeginSet();
}

/// end a system build configuration
TPC_EXP TPC_ErrorCode TPC_CC TPC_EndSet()
{
	return System::TheSystem()->EndSet();
}

/// delete the defined set
TPC_EXP TPC_ErrorCode TPC_CC TPC_CancelSet()
{
	return System::TheSystem()->CancelSet();
}

/// ECR single channel, get associated channels
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetAssociatedChannels(int deviceIx, int boardAddress, int inputNumber, 
													   TPC_AssociatedChannel* list, int* count)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetAssociatedChannels(deviceIx, boardAddress, inputNumber, list, count);
}

/// ECR single channel, set the channel association list
TPC_EXP TPC_ErrorCode TPC_CC TPC_SetAssociatedChannels(int deviceIx, int boardAddress, int inputNumber, 
													   TPC_AssociatedChannel* list, int count)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->SetAssociatedChannels(deviceIx, boardAddress, inputNumber, list, count);
}

/// Execute a system command
TPC_EXP TPC_ErrorCode TPC_CC TPC_ExecuteSystemCommand(TPC_SystemCommand command)
{
	return System::TheSystem()->ExecuteSystemCommand(command);
}

/// start a measurement and wait till done or timeout
TPC_EXP TPC_ErrorCode TPC_CC TPC_MakeMeasurement(int timeout, int *measurementNumber)
{
	return System::TheSystem()->MakeMeasurement(timeout, measurementNumber);
}

/// start the auto calibration process
TPC_EXP TPC_ErrorCode TPC_CC TPC_StartCalibration(int deviceIx)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->StartCalibration(deviceIx, tpc_calAuto);
}

/// get the actual device status
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetDeviceStatus(int deviceIx, TPC_DeviceStatus* status, int structSize)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetDeviceStatus(deviceIx, status, structSize);
}


/// register a status callback function 
TPC_EXP TPC_ErrorCode TPC_CC TPC_SetStatusCallback(TPC_StatusCallbackFunc callback, void* userData)
{
	return System::TheSystem()->SetStatusCallback(callback, userData);
}

/// get Y-Axis Meta data
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetYMetaData(int deviceIx, int boardAddress, int inputNumber, 
					 int measurementNumber,
					 TPC_YMetaData* metaData, int structSize)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetYMetaData(deviceIx, boardAddress, inputNumber, 
					measurementNumber, metaData, structSize);
}

/// get the paramater which was set at the last measurement
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetMetaDataParameter(int deviceIx, int boardAddress, int inputNumber, 
								int measurementNumber, TPC_Parameter parameter, double* value)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetMetaDataParameter(deviceIx, boardAddress, inputNumber, measurementNumber, parameter, value);
}

/// get the attributes which was set at the last measurement
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetMetaDataAttribute(int deviceIx, int boardAddress, int inputNumber, 
							 int measurementNumber,
							 const char* key, char* buffer, int maxLen)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	string attr;
	TPC_ErrorCode err = s->GetMetaDataAttribute(deviceIx, boardAddress, inputNumber, measurementNumber, key, attr);
	if (err != 0) return err;
	err = CopyString(attr, buffer, maxLen);
	return err;
}

/// get all attributes which were set at the last measurement
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetAllMetaDataAttributes(int deviceIx, 
				  							    int measurementNumber,
												TPC_AttributeEnumeratorCallback callback, void* userData)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetAllMetaDataAttributes(deviceIx, measurementNumber, callback, userData);
}

/// get the associated channel list valid during the last measurement
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetMetaDataAssociatedChannels(int deviceIx, int boardAddress, int inputNumber, 
													  int measurementNumber, TPC_AssociatedChannel* list, int* count)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetMetaDataAssociatedChannels(deviceIx, boardAddress, inputNumber, measurementNumber, list, count);
}


/// Get Time-Meta data 
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetTMetaData(int deviceIx, int boardAddress, int blockNumber, 
					 int measurementNumber,
					 TPC_TMetaData* metaData, int structSize)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	TPC_ErrorCode err;
	err = s->GetTMetaData(deviceIx, boardAddress, blockNumber, 
					measurementNumber, metaData, structSize);
	return err;
}

/// get all time-meta data as a list
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetAllTMetaData(int deviceIx, int boardAddress, int blockNumberFrom, 
					 int blockNumberTo, int measurementNumber,
					 TPC_TMetaData* metaData, int structSize)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	TPC_ErrorCode err;
	err = s->GetTMetaData(deviceIx, boardAddress, blockNumberFrom, 
					blockNumberTo, measurementNumber, metaData, structSize);
	return err;
}

/// get measurement data scaled to the volt or charge
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
				int measurementNumber, uint64_t dataStart, int dataLength, double* data)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetData(deviceIx, boardAddress, inputNumber, blockNumber, 
				measurementNumber, dataStart, dataLength, 
				dataLength, data, Device::readoutRange);
}


/// get raw integer date 
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetRawData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
				int measurementNumber, uint64_t dataStart, int dataLength, int32_t* data)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetRawData(deviceIx, boardAddress, inputNumber, blockNumber, 
				measurementNumber, dataStart, dataLength, 
			    dataLength, data, Device::readoutRange);
}

/// register a data request, scaled to volt or charge
TPC_EXP TPC_ErrorCode TPC_CC TPC_DeferredGetData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, int dataLength, double* data, TPC_ErrorCode* error)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->DeferredGetData(deviceIx, boardAddress, inputNumber, blockNumber, 
				dataStart, dataLength, 
			    dataLength, data, error, Device::readoutRange);
}


/// register a data request in  raw format
TPC_EXP TPC_ErrorCode TPC_CC TPC_DeferredGetRawData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
								uint64_t dataStart, int dataLength, int32_t* data, TPC_ErrorCode* error)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->DeferredGetRawData(deviceIx, boardAddress, inputNumber, blockNumber, 
				dataStart, dataLength, 
			    dataLength, data, error, Device::readoutRange);
}


/// get min-max envelope data, scalled to volt or charge
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetMinMaxData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
				int measurementNumber, uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, double* data)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;

    if ((resultLength & 1) != 0 || resultLength/2 > dataLength) return tpc_errInvalidParameter;

	return s->GetData(deviceIx, boardAddress, inputNumber, blockNumber, 
				measurementNumber, dataStart, dataLength, 
			    resultLength, data, Device::readoutMinMax);
}

/// get min-max envelope raw data
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetRawMinMaxData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
				int measurementNumber, uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, int32_t* data)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;

    if ((resultLength & 1) != 0 || resultLength/2 > dataLength) return tpc_errInvalidParameter;

	return s->GetRawData(deviceIx, boardAddress, inputNumber, blockNumber, 
				measurementNumber, dataStart, dataLength, 
			    resultLength, data, Device::readoutMinMax);
}

/// register a deferred min-max data request
TPC_EXP TPC_ErrorCode TPC_CC TPC_DeferredGetMinMaxData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, double* data, TPC_ErrorCode* error)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;

    if ((resultLength & 1) != 0 || resultLength/2 > dataLength) return tpc_errInvalidParameter;

	return s->DeferredGetData(deviceIx, boardAddress, inputNumber, blockNumber, 
				dataStart, dataLength, 
			    resultLength, data, error, Device::readoutMinMax);
}


/// register a deferred raw min-max data request
TPC_EXP TPC_ErrorCode TPC_CC TPC_DeferredGetRawMinMaxData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
								uint64_t dataStart, uint64_t dataLength, 
								int resultLength, int32_t* data, TPC_ErrorCode* error)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
        
    if ((resultLength & 1) != 0 || resultLength/2 > dataLength) return tpc_errInvalidParameter;

	return s->DeferredGetRawData(deviceIx, boardAddress, inputNumber, blockNumber, 
				dataStart, dataLength, 
			    resultLength, data, error, Device::readoutMinMax);
}

/// process all registered data requests
TPC_EXP TPC_ErrorCode TPC_CC TPC_ProcessDeferredDataRequests(int measurementNumber)
{
	return System::TheSystem()->ProcessDeferredDataRequests(measurementNumber);
}

/// cancel all registered data requestes
TPC_EXP TPC_ErrorCode TPC_CC TPC_CancelDeferredDataRequests()
{
	return System::TheSystem()->CancelDeferredDataRequests();
}

/// Write hardware settings to xml a settings file (not implemented on all repos)
TPC_EXP TPC_ErrorCode TPC_CC TPC_WriteSettingFile(const char* filename)
{
#ifndef NOSETTINGSFILE
	std::string errormsg;
    TPC_ErrorCode status = WriteSettingFile(filename, errormsg);
    if (status == tpc_errWriteSettingFile) {
        // NYI: store errormsg for later access.
    }
    return status;
#else
	return tpc_errNotImplemented;
#endif
}

/// Load hardware settings for a xml settings file (not implemented on all repos)
TPC_EXP TPC_ErrorCode TPC_CC TPC_LoadSettingFile(const char* filename)
{
#ifndef NOSETTINGSFILE
	std::string errormsg;
    TPC_ErrorCode status = LoadSettingFile(filename, errormsg);
    if (status == tpc_errLoadSettingFile) {
        // NYI: store errormsg for later access.
    }
    return status;
#else
	return tpc_errNotImplemented;
#endif
}

/// send server custom command (no function impemented)
TPC_EXP TPC_ErrorCode TPC_CC TPC_SendServerCustomCommand(int deviceIx, char *command, char *result, int length)
{
  System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
  if (s == NULL) return tpc_errInvalidDeviceIx;
  deviceIx = deviceIx % SYSTEM_MULTIPLIER;
  return s->SendServerCustomCommand(deviceIx, command, result, length);
}

/// set new EasyTrigger parameter 
TPC_EXP TPC_ErrorCode TPC_CC TPC_SetTrigger(int deviceIx, int boardAddress, int inputNumber, TPC_EasyTriggerMode mode, TPC_EasyTriggerComperatorMode comp, TPC_EasyTriggerFlags flags, double level, double hysteresis, int time, int time2)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;

	return s->SetTrigger(deviceIx, boardAddress, inputNumber, mode, comp, flags, level, hysteresis, time, time2);
}

///get new EasyTrigger parameters
TPC_EXP TPC_ErrorCode TPC_CC TPC_GetTrigger(int deviceIx, int boardAddress, int inputNumber, TPC_EasyTriggerMode *mode, TPC_EasyTriggerComperatorMode *compMode, TPC_EasyTriggerFlags *flags, double *level, double *hysteresis, int *time, int *time2)
{
	System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;

	return s->GetTrigger(deviceIx, boardAddress, inputNumber, mode, compMode, flags, level, hysteresis, time, time2);
}

/// Reset serial protocol trigger 
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ResetSerTrg(int deviceIx, int boardAddress) {
	System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;

	return s->ResetSerTrg(deviceIx, boardAddress);
}

/// get serial protocol trigger parameters
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetSerTrgProtocol(int deviceIx, int boardAddress, TPC_SerialTriggerProtocol *protocol) {
	System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;

	return s->GetSerTrgProtocol(deviceIx, boardAddress, protocol);
}

///set I2C protocol trigger parameters
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_SetI2CTrigger(int deviceIx, int boardAddress, TPC_I2CTriggerSettings *settings) {
	System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;

	return s->SetI2CTrigger(deviceIx, boardAddress, settings);
}

/// get I2C protocol trigger parameters
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetI2CTrigger(int deviceIx, int boardAddress, TPC_I2CTriggerSettings *settings) {
	System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;

	return s->GetI2CTrigger(deviceIx, boardAddress, settings);
}

/// set CAN protocol trigger parameters
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_SetCANTrigger(int deviceIx, int boardAddress, TPC_CANTriggerSettings *settings) {
	System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;

	return s->SetCANTrigger(deviceIx, boardAddress, settings);
}

/// get CAN protocol trigger parameters
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetCANTrigger(int deviceIx, int boardAddress, TPC_CANTriggerSettings *settings) {
	System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;

	return s->GetCANTrigger(deviceIx, boardAddress, settings);
}

/// Start Full, Fast or auto calibration routing
TPC_EXP TPC_ErrorCode TPC_CC TPC_Elsys_StartCalibration(int deviceIx, TPC_CalibrationType command)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->StartCalibration(deviceIx, command);
}

/// Low Level Access: write to EEPROM or FPGA register directly
TPC_EXP TPC_ErrorCode TPC_CC TPC_WriteDev(int deviceIx, int boardAddress, int type, unsigned count, void* data, unsigned aux1, unsigned aux2)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->WriteDev(deviceIx, boardAddress, type, count, data, aux1, aux2);
}


/// low Level Access: read from EEPROM or FPGA register directly
TPC_EXP TPC_ErrorCode TPC_CC TPC_ReadDev(int deviceIx, int boardAddress, int type, unsigned count, void* data, unsigned aux1, unsigned aux2)
{
    System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
    if (s == NULL) return tpc_errInvalidDeviceIx;
    deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->ReadDev(deviceIx, boardAddress, type, count, data, aux1, aux2);
}

/// I2C Interface access
TPC_EXP TPC_ErrorCode TPC_CC TPC_ReadWriteTwi(int deviceIx, int boardAddress, int amplifier, int count, char *data)
{
  System* s = SystemList::TheSystemList()->FindSystem(deviceIx);
  if (s == NULL) return tpc_errInvalidDeviceIx;
  deviceIx = deviceIx % SYSTEM_MULTIPLIER;
  return s->ReadWriteTwi(deviceIx, boardAddress, amplifier, count, data);
}

/// create a new system definition
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_NewSystem(int* id)
{
    *id = SystemList::TheSystemList()->NewSystem();
    return tpc_noError;
}

/// delete a system definition
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_DeleteSystem(int id)
{
    return SystemList::TheSystemList()->DeleteSystem(id);
}

/// start a new system definition lsit
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_BeginSystemDefinitionSystem(int id)
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
	s->ClearDeviceList();
	return tpc_noError;
}

/// add a new device to a specific system
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_AddDeviceSystem(int id, const char* url, int* deviceIx)
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
   	*deviceIx = s->AddDevice(url,10,60);
    return tpc_noError;
}

/// add a new device to a specific system and define timeouts
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_AddDeviceSystemEx(int id, const char* url, int recvTimeout, int sendTimeOut,int* deviceIx )
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
	*deviceIx = s->AddDevice(url,recvTimeout,sendTimeOut);
    return tpc_noError;
}

/// remove a device from a system
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_RemoveDeviceSystem(int id, const char* url)
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
   	s->RemoveDevice(url);
    return tpc_noError;
}

/// stop the system definition list creation process and connect
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_EndSystemDefinitionSystem(int id, int connectionTimeoutMilliseconds)
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
	int t = 0;
	for(;;) {
		// Check the connection to the devices.
		TPC_ErrorCode err = s->GetConnectionState();
		if (err != tpc_errNoConnection) return err;

		// Wait a little, then check again until timeout.
		const int deltaT = 50;
		NonWaitableThread::sleep(deltaT);
		t += deltaT;
		if (t > connectionTimeoutMilliseconds) return tpc_errNoConnection;
	}
}

/// get the number of device from a system
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_NumDevicesSystem(int id, int* n)
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
   	*n = s->NumDevices();
    return tpc_noError;
}

/// get a devcie URL from a system and device
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetDeviceUrlSystem(int id, int index, char* buffer, int maxLen)
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
	string url;
	TPC_ErrorCode err = s->GetDeviceUrl(index, url);
	if (err != 0) return err;

	err = CopyString(url, buffer, maxLen);
	return err;
}

/// reset a specific system
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ResetConfigurationSystem(int id)
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
	return s->ResetConfiguration();
}

/// begin system definition
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_BeginSetSystem(int id)
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
	return s->BeginSet();
}

/// end system definition
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_EndSetSystem(int id)
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
	return s->EndSet();
}

/// cancel a system list
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_CancelSetSystem(int id)
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
	return s->CancelSet();
}

/// execute a command on a specific system
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ExecuteSystemCommandSystem(int id, enum TPC_SystemCommand command)
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
	return s->ExecuteSystemCommand(command);
}

/// register the status callback functiuon for a specific system
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_SetStatusCallbackSystem(int id, TPC_StatusCallbackFunc callback, void* userData)
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
	return s->SetStatusCallback(callback, userData);
}

/// process all registers data requests for a specific system
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ProcessDeferredDataRequestsSystem(int id, int measurementNumber)
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
	return s->ProcessDeferredDataRequests(measurementNumber);
}

/// cancel all registers data request from a specific system
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_CancelDeferredDataRequestsSystem(int id)
{
    System* s = SystemList::TheSystemList()->FindSystem(id);
    if (s == NULL) return tpc_errInvalidDeviceIx;
	return s->CancelDeferredDataRequests();
}

/// Send and store an autosequence/measurement flow control file on the remote device
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_LoadAutosequence(	int deviceIx, char * cData, int iLength){
	System * s = SystemList::TheSystemList()->FindSystem(deviceIx);
	if (s == NULL) return tpc_errInvalidDeviceIx;
	deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->LoadAutosequence(deviceIx, cData, iLength);

}

/// Read the stored autosequence / Measurement flow control file
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetAutoSequence(	int deviceIx, char * cData, int iLength){
	System * s = SystemList::TheSystemList()->FindSystem(deviceIx);
	if (s == NULL) return tpc_errInvalidDeviceIx;
	deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetAutoSequence(deviceIx, cData, iLength);
}

/// Start Remote Auto Sequence/Measurement Flow Control
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_StartAutoSequence(int deviceIx){
	System * s = SystemList::TheSystemList()->FindSystem(deviceIx);
	if (s == NULL) return tpc_errInvalidDeviceIx;
	deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->StartAutoSequence(deviceIx);
}

/// Stop Remote Auto Sequence/Measurement Flow Control
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_StopAutoSequence(	int deviceIx){
	System * s = SystemList::TheSystemList()->FindSystem(deviceIx);
	if (s == NULL) return tpc_errInvalidDeviceIx;
	deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->StopAutoSequence(deviceIx);
}

/// Get the GPS log list as char array
TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetGPSLogList(int deviceIx, int from, int to, char *log){
	System * s = SystemList::TheSystemList()->FindSystem(deviceIx);
	if (s == NULL) return tpc_errInvalidDeviceIx;
	deviceIx = deviceIx % SYSTEM_MULTIPLIER;
	return s->GetGPSLogList(deviceIx, from,to,log);
}

