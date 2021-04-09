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
  $Id: SoapDevice.cpp 28 2017-06-06 13:31:56Z roman $
  Thin layer over the gSOAP connection to the remote device.
--------------------------------------------------------------------------------*/

#include "SoapDevice.h"

#include "intTypes.h"

#include "DataReceiver.h"

#include <string.h>

//-------------------------------------------------------------------------------

ns__Data::ns__Data()
{
	__ptr = NULL;
	__size = 0;
	id = NULL;
	type = NULL;
	options = NULL;
}


//-------------------------------------------------------------------------------


void Assign(TPC_DateTime& d, const ns__DateTime& s)
{
	d.year = s.year;
	d.month = s.month;
	d.day = s.day;
	d.hour = s.hour;
	d.minute = s.minute;
	d.second = s.second;
	d.milliSecond = s.milliSecond;
}


void Assign(ns__DateTime& d, const TPC_DateTime& s)
{
	d.year = s.year;
	d.month = s.month;
	d.day = s.day;
	d.hour = s.hour;
	d.minute = s.minute;
	d.second = s.second;
	d.milliSecond = s.milliSecond;
}


void Assign(TPC_DeviceInfo& d, const ns__DeviceInfo& s)
{
	memset(&d, 0x00, sizeof(TPC_DeviceInfo));
	d.deviceID = s.deviceID;
	d.serverSoftwareVersion = s.serverSoftwareVersion;
	d.simulatedHardware = s.simulatedHardware;
	int n = sizeof(d.deviceName)-1;
	strncpy(d.deviceName, s.deviceName, n);
	d.deviceName[n] = '\0';
	d.SyncLinkDetected = 0;

	if(d.serverSoftwareVersion > 10101){
		// alte Serverversionen liefern nicht alle Parameter
		

		d.AutoStartAutoSequence = s.AutoStartAutosequence;
		d.AutoStartMeasurement  = s.AutoStartMeasurement;
		
		n = sizeof(d.DataFileName) - 1;
		strncpy(d.DataFileName, s.DataFileName, n);
		d.DataFileName[n] = '\0';
		n = sizeof(d.deviceDescription) - 1;
		strncpy(d.deviceDescription, s.DeviceDescription, n);
		d.deviceDescription[n] = '\0';
		d.HdFlushInterval = s.HdFlushInterval;
		d.NumberOfBackupFiles = s.NumberOfBackupFiles;
		d.ServerPort1 = s.ServerPort;
		d.ServerPort2 = s.ServerPortTwoinOne;
		d.TargetPort  = s.TargetPort;
		d.TwoinOneEnabled = s.TwoInOne;
		n = sizeof(d.TwoInOnepassword) -1;
		strncpy(d.TwoInOnepassword, s.TwoInOnepassword, n);
		d.TwoInOnepassword[n] = '\0';
		d.UseLocalTime = s.UseLocalTime;
		d.WriteTroughCache = s.WriteTroughCache;
		d.ModelType = s.ModelType;
		d.SyncLinkDetected = s.SyncLinkDetected;
	}
	return;
	
}


void Assign(TPC_BoardInfo& d, const ns__BoardInfo& s)
{
	d.boardClass = s.boardClass;
	d.hardwareVersion = s.hardwareVersion;
	d.serialNr = s.serialNr;
	d.driverVersion = s.driverVersion;
	d.firmwareVersion = s.firmwareVersion;
	d.numberOfInputs = s.numberOfInputs;
	d.maxMemory = s.maxMemory;
	d.maxSpeed = s.maxSpeed;
	d.operationModeOptions = s.operationModeOptions;
	d.averageOptions = s.averageOptions;
	d.triggerOptions = s.triggerOptions;
	Assign(d.lastFactoryCalibration, s.lastFactoryCalibration);
	Assign(d.lastUserCalibration, s.lastUserCalibration);
}


void Assign(TPC_InputInfo& d, const ns__InputInfo& s)
{
	d.inputClass			= s.inputClass;
	d.hardwareVersion		= s.hardwareVersion;
	d.maxAdcSpeed			= s.maxAdcSpeed;
	d.adcResolution			= s.adcResolution;
	d.offsetOptions			= s.offsetOptions;
	d.filterOptions			= s.filterOptions;
	d.diffOptions			= s.diffOptions;
	d.inputCouplingOptions	= s.inputCouplingOptions;
	d.maxMarkerMask			= s.maxMarkerMask;
	memset(d.inputRanges, 0x00, sizeof(d.inputRanges));
	memcpy(d.inputRanges, s.inputRanges, sizeof(d.inputRanges));
}


void Assign(TPC_BoardStatus& d, const ns__BoardStatus& s)
{
	d.recordingState = (TPC_RecordingState)s.recordingState;
	d.triggerState = (TPC_TriggerState)s.triggerState;
	d.blockCounter = s.blockCounter;
	d.dataCounter = s.dataCounter;
	d.dataLostCounter = s.dataLostCounter;
	d.blockLostCounter = s.blockLostCounter;
}


void Assign(TPC_DeviceStatus& d, const ns__DeviceStatus& s)
{
	d.deviceError = (TPC_ErrorCode)s.deviceError;
	d.settingsChangesCounter = s.settingsChangesCounter;
	for (int i=0; i<tpc_maxBoards; i++) Assign(d.boards[i], s.boards[i]);
}

/*
void Assign(ns__Status& d, const Device::Status& s)
{
	d.eventCounter = s.eventCounter;
	Assign(d.status, s.status);
	for (int i=0; i<tpc_maxBoards; i++) d.clusters.clusterNumbers[i] = s.clusterNumbers[i];
}
*/


//-------------------------------------------------------------------------------


static void* dime_write_open(struct soap* soap, const char* id, const char* type, const char* options)
{
	// we can return NULL without setting soap->error if we don't want to use the streaming callback for this DIME attachment
	DataReceiver* r = (DataReceiver*)soap->user;
	return (void*)r;
}



static void dime_write_close(struct soap* soap, void* handle)
{ 
}



static int dime_write(struct soap* soap, void* handle, const char* buf, size_t len)
{
	DataReceiver* r = (DataReceiver*)handle;
	r->PushBytes((uint8_t*)buf, len);
	return SOAP_OK;
}


//-------------------------------------------------------------------------------


SoapDevice::SoapDevice(const string& url, int recvTimeOut, int sendTimeOut)
{
	m_url = url;

	// Initialize the soap context for the main connection
	soap_init1(&m_soap, SOAP_IO_KEEPALIVE);
	m_soap.recv_timeout = recvTimeOut; // 
	m_soap.send_timeout = sendTimeOut; // 
    m_soap.error = 0x0;
	// set DIME callbacks
	m_soap.fdimewriteopen = dime_write_open;
	m_soap.fdimewriteclose = dime_write_close;
	m_soap.fdimewrite = dime_write;
}



SoapDevice::~SoapDevice()
{
	soap_destroy(&m_soap); // delete deserialized class instances (for C++ only)
	soap_end(&m_soap); // remove deserialized data and clean up
	soap_done(&m_soap); // detach the gSOAP environment
}


int SoapDevice::KillSocket()
{
    int retVal = 0;
    // by RBE --
	/*
	if (m_soap.fdsocket > 0) {
        int err = m_soap.fclosesocket(&m_soap, m_soap.fdsocket);
        if (err != 0) retVal = err;
//cout << m_soap.fdsocket << ", " << err << endl;
    }
	*/
    if (m_soap.socket > 0) {
        int err = m_soap.fclosesocket(&m_soap, m_soap.socket);
        if (err != 0) retVal = err;
//cout << m_soap.socket << ", " << err << endl;
    }
    return retVal;
}


//-------------------------------------------------------------------------------


TPC_ErrorCode SoapDevice::GetInterfaceVersion(int& version)
{
	int err = soap_call_ns__GetInterfaceVersion(&m_soap, m_url.c_str(), NULL, version);
	// After disconnecting and reconnecting, the SOAP End of File error may appear. One retry.
	if (err == -1)
		err = soap_call_ns__GetInterfaceVersion(&m_soap, m_url.c_str(), NULL, version);

	return Epilog();
}



TPC_ErrorCode SoapDevice::ResetConfiguration()
{
	ns__DummyResponse Response;
	int err = soap_call_ns__ResetConfiguration(&m_soap, m_url.c_str(), NULL, &Response);
	// After disconnecting and reconnecting, the SOAP End of File error may appear. One retry.
	if (err == -1)
		err = soap_call_ns__ResetConfiguration(&m_soap, m_url.c_str(), NULL, &Response);

	return Epilog();
}

//--- Synchronisation -----------------------------------------------------------


TPC_ErrorCode SoapDevice::AcquireDeviceLock()
{
  ns__DummyResponse Response;
  int err = soap_call_ns__AcquireDeviceLock(&m_soap, m_url.c_str(), NULL, &Response);
  // After disconnecting and reconnecting, the SOAP End of File error may appear. One retry.
	if (err == -1)
		err = soap_call_ns__AcquireDeviceLock(&m_soap, m_url.c_str(), NULL, &Response);

	return Epilog();
}


TPC_ErrorCode SoapDevice::ReleaseDeviceLock()
{
  ns__DummyResponse Response;
  int err = soap_call_ns__ReleaseDeviceLock(&m_soap, m_url.c_str(), NULL, &Response);
  // After disconnecting and reconnecting, the SOAP End of File error may appear. One retry.
	if (err == -1)
		err = soap_call_ns__ReleaseDeviceLock(&m_soap, m_url.c_str(), NULL, &Response);

	return Epilog();
}


//--- Cluster configuration -----------------------------------------------------

TPC_ErrorCode SoapDevice::GetClusterConfiguration(int* clusters)
{
	ns__ClusterConfiguration clusterNumbers;

	int err = soap_call_ns__GetClusterConfiguration(&m_soap, m_url.c_str(), NULL, clusterNumbers);
	// After disconnecting and reconnecting, the SOAP End of File error may appear. One retry.
	if (err == -1)
		err = soap_call_ns__GetClusterConfiguration(&m_soap, m_url.c_str(), NULL, clusterNumbers);

	if (err == 0) {
		for (int i=0; i<tpc_maxBoards; i++) 
			clusters[i] = clusterNumbers.clusterNumbers[i];
	}
	else {
		for (int i=0; i<tpc_maxBoards; i++) 
			clusters[i] = -1; 
	}

	return Epilog();
}


#pragma warning( disable: 4701 )
TPC_ErrorCode SoapDevice::SetClusterConfiguration(int* clusters)
{
	ns__ClusterConfiguration clusterNumbers;
	for (int i=0; i<tpc_maxBoards; i++) 
		clusterNumbers.clusterNumbers[i] = clusters[i];
	ns__DummyResponse Response;
	int err = soap_call_ns__SetClusterConfiguration(&m_soap, m_url.c_str(), NULL, clusterNumbers, &Response);
	// After disconnecting and reconnecting, the SOAP End of File error may appear. One retry.
	if (err == -1)
		err = soap_call_ns__SetClusterConfiguration(&m_soap, m_url.c_str(), NULL, clusterNumbers, &Response);

	return Epilog();
#pragma warning(default:4701)
}


//--- Hardware info -------------------------------------------------------------

TPC_ErrorCode SoapDevice::GetHardwareInfo(DeviceInfo& deviceInfo)
{
	ns__HardwareInfo h;
    uint64_t macAddress = 0;
	int err = soap_call_ns__GetHardwareInfo(&m_soap, m_url.c_str(), NULL, h);
	// After disconnecting and reconnecting, the SOAP End of File error may appear. One retry.
	if (err == -1)
		err = soap_call_ns__GetHardwareInfo(&m_soap, m_url.c_str(), NULL, h);
    if (err == 0) {
		if (h.device.serverSoftwareVersion > 10101){
			char myIp[32];
			strncpy(myIp,m_url.c_str(),sizeof(myIp)-1);
			myIp[31] = NULL;
			
			int err2 = soap_call_ns__GetMACAddress(&m_soap, m_url.c_str(), NULL, myIp,macAddress);
			// After disconnecting and reconnecting, the SOAP End of File error may appear. One retry.
			if (err2 == -1)
				err2 = soap_call_ns__GetMACAddress(&m_soap, m_url.c_str(), NULL, myIp,macAddress);
			if (err2 == -1)
				macAddress = 0;
		}
		deviceInfo.Clear();
		
		// Loop through the boards. At the same time build the 'installed boards' bitmask.
		unsigned int boardMask = 0;
		for (int i = 0; i<h.boards.__size; i++) {
			TPC_BoardInfo bi;
			Assign(bi, h.boards.__ptr[i].info);
			int boardAddr = h.boards.__ptr[i].boardAddress;
			deviceInfo.SetBoardInfo(boardAddr, bi);
			if (bi.numberOfInputs > 0) boardMask |= (1<<boardAddr);

			BoardRestrictions br;
			br.maxMemory = bi.maxMemory;
			br.maxSpeed = bi.maxSpeed;
			br.maxAdcSpeed = h.boards.__ptr[i].info.maxAdcSpeed; 
			br.boardClock = h.boards.__ptr[i].info.boardClock; 
			br.version = bi.hardwareVersion;
			deviceInfo.SetBoardRestrictions(h.boards.__ptr[i].boardAddress, br);
		}
		
		// Store the device info
		TPC_DeviceInfo dev;
		Assign(dev, h.device);
		dev.installedBoards = boardMask;
		dev.deviceMACAddress = macAddress;
		deviceInfo.SetDeviceInfo(dev);
		
		// Loop through the inputs
		for (int i = 0; i<h.inputs.__size; i++) {
			TPC_InputInfo ii;
			Assign(ii, h.inputs.__ptr[i].info);
			deviceInfo.SetInputInfo(h.inputs.__ptr[i].boardAddress, h.inputs.__ptr[i].inputNumber, ii);
		}
	}

	return Epilog();
}

TPC_ErrorCode SoapDevice::SetHardwareSettings(TPC_DeviceInfo deviceInfo){
	
	ns__DeviceInfo settings;
	
	settings.AutoStartAutosequence	= deviceInfo.AutoStartAutoSequence;
	settings.AutoStartMeasurement	= deviceInfo.AutoStartMeasurement;
	settings.DataFileName			= deviceInfo.DataFileName;
	settings.DeviceDescription		= deviceInfo.deviceDescription;
	settings.deviceName				= deviceInfo.deviceName;
	settings.HdFlushInterval		= deviceInfo.HdFlushInterval;
	settings.NumberOfBackupFiles	= deviceInfo.NumberOfBackupFiles;
	settings.ServerPort				= deviceInfo.ServerPort1;
	settings.ServerPortTwoinOne		= deviceInfo.ServerPort2;
	settings.simulatedHardware		= deviceInfo.simulatedHardware;
	settings.TargetPort				= deviceInfo.TargetPort;
	settings.TwoInOne				= deviceInfo.TwoinOneEnabled;
	settings.UseLocalTime			= deviceInfo.UseLocalTime;
	settings.WriteTroughCache		= deviceInfo.WriteTroughCache;
	settings.TwoInOnepassword		= deviceInfo.TwoInOnepassword;

    ns__DummyResponse Response;
	int err = soap_call_ns__SetDeviceSettings(&m_soap, m_url.c_str(), NULL, settings,&Response);
	// After disconnecting and reconnecting, the SOAP End of File error may appear. One retry.
	if (err == -1)
		err = soap_call_ns__SetDeviceSettings(&m_soap, m_url.c_str(), NULL, settings,&Response);
    
	return Epilog();
}

TPC_ErrorCode SoapDevice::GetFreeDiskSpace(uint64_t* freeDiskSpace, uint64_t* DiskSize){
	
	ns__DiskSpace ds;
	int err = soap_call_ns__GetFreeDiskSpace(&m_soap, m_url.c_str(), NULL, ds);
	// After disconnecting and reconnecting, the SOAP End of File error may appear. One retry.
	if (err == -1)
		err = soap_call_ns__GetFreeDiskSpace(&m_soap, m_url.c_str(), NULL, ds);
	
	*DiskSize = ds.DiskSize;
	*freeDiskSpace = ds.freeDiskSpace;
	return Epilog();

}

//--- Recording parameters ------------------------------------------------------

TPC_ErrorCode SoapDevice::SetOneParameter(int boardAddress, int inputNumber, int parameterIndex,
					double value, double* roundedValue, int* error)
{
	ns__SetOneParameterResult r;
	int err = soap_call_ns__SetOneParameter(&m_soap, m_url.c_str(), NULL, 
				boardAddress, inputNumber, parameterIndex, value, r);
	// After disconnecting and reconnecting, the SOAP End of File error may appear. One retry.
	if (err == -1)
		err = soap_call_ns__SetOneParameter(&m_soap, m_url.c_str(), NULL, 
				boardAddress, inputNumber, parameterIndex, value, r);

	if (err == 0) {
		if (roundedValue != NULL) *roundedValue = r.roundedValue;
		if (error != NULL) *error = r.error;
	}
	else {
		if (error != NULL) *error = err;
	}

	return Epilog();
}



TPC_ErrorCode SoapDevice::SetMultipleParameters(ParameterModificationList& parameters)
{
	ns__ParameterList list;

	int count = parameters.Count();
	list.__size = count;
	list.__ptr = (ns__Parameter*)soap_malloc(&m_soap, sizeof(ns__Parameter)*count);
	for (int j=0; j<count; j++) {
		list.__ptr[j].boardAddress = parameters[j].boardAddress;
		list.__ptr[j].inputNumber = parameters[j].inputNumber;
		list.__ptr[j].parameterIndex = parameters[j].parameterIndex;
		list.__ptr[j].value = parameters[j].value;
	}
	ns__SetParameterResultList result;
    
	int err = soap_call_ns__SetMultipleParameters(&m_soap, m_url.c_str(), NULL, list, result);

	if (result.__size != count) return Epilog(tpc_errInternalError);
	
	if (err == 0) {
		for (int i=0; i<count; i++) {
			parameters.SetResult(i, result.__ptr[i].roundedValue, result.__ptr[i].error);
		}
	}
	else {
		for (int i=0; i<count; i++) {
			parameters.SetResult(i, 0, err);
		}
	}

	return Epilog();
}



TPC_ErrorCode SoapDevice::GetAllCurrentParameters(ParameterSet& parameters)
{
	ns__ParameterList list;

	int err = soap_call_ns__GetAllCurrentParameters(&m_soap, m_url.c_str(), NULL, list);
	// After disconnecting and reconnecting, the SOAP End of File error may appear. One retry.
	if (err == -1)
		err = soap_call_ns__GetAllCurrentParameters(&m_soap, m_url.c_str(), NULL, list);

	parameters.Clear();
	if (err == 0) {
		for (int i=0; i<list.__size; i++) {
			parameters.SetParameter(list.__ptr[i].boardAddress, list.__ptr[i].inputNumber,
							list.__ptr[i].parameterIndex, list.__ptr[i].value);
		}
	}
	return Epilog();
}



TPC_ErrorCode SoapDevice::GetAllPreviousParameters(int currentMeasurementNumber, ParameterSet& parameters)
{
	ns__ParameterList list;

	int err = soap_call_ns__GetAllPreviousParameters(&m_soap, m_url.c_str(), NULL, currentMeasurementNumber, list);
	// After disconnecting and reconnecting, the SOAP End of File error may appear. One retry.
	if (err == -1)
		err = soap_call_ns__GetAllPreviousParameters(&m_soap, m_url.c_str(), NULL, currentMeasurementNumber, list);

	parameters.Clear();
	if (err == 0) {
		for (int i=0; i<list.__size; i++) {
			parameters.SetParameter(list.__ptr[i].boardAddress, list.__ptr[i].inputNumber,
							list.__ptr[i].parameterIndex, list.__ptr[i].value);
		}
	}
	return Epilog();
}

//--- Serial protocol trigger ---------------------------------------------------

TPC_ErrorCode SoapDevice::ResetSerTrg(int boardAddress) {
	ns__DummyResponse Response;
	soap_call_ns__ResetSerTrg(&m_soap, m_url.c_str(), NULL, boardAddress, &Response);
	return Epilog();
}

TPC_ErrorCode SoapDevice::GetSerTrgProtocol(int boardAddress, TPC_SerialTriggerProtocol *settings) {
	int settingsCopy;
	soap_call_ns__GetSerTrgProtocol(&m_soap, m_url.c_str(), NULL, boardAddress, settingsCopy);
	*settings = (TPC_SerialTriggerProtocol)settingsCopy;
	return Epilog();
}

TPC_ErrorCode SoapDevice::SetI2CTrigger(int boardAddress, TPC_I2CTriggerSettings *settings) {
	ns__DummyResponse Response;
	ns__I2CTriggerSettings settingsCopy;
	memcpy(&settingsCopy, settings, sizeof(TPC_I2CTriggerSettings));
	soap_call_ns__SetI2CTrigger(&m_soap, m_url.c_str(), NULL, boardAddress, settingsCopy ,&Response);
	return Epilog();
}

TPC_ErrorCode SoapDevice::GetI2CTrigger(int boardAddress, TPC_I2CTriggerSettings *settings) {
	ns__I2CTriggerSettings settingsCopy;
	soap_call_ns__GetI2CTrigger(&m_soap, m_url.c_str(), NULL, boardAddress, settingsCopy);
	memcpy(settings, &settingsCopy, sizeof(TPC_I2CTriggerSettings));
	return Epilog();
}

TPC_ErrorCode SoapDevice::SetCANTrigger(int boardAddress, TPC_CANTriggerSettings *settings) {
	ns__DummyResponse Response;
	ns__CANTriggerSettings settingsCopy;
	memcpy(&settingsCopy, settings, sizeof(TPC_CANTriggerSettings));
	soap_call_ns__SetCANTrigger(&m_soap, m_url.c_str(), NULL, boardAddress, settingsCopy ,&Response);
	return Epilog();
}

TPC_ErrorCode SoapDevice::GetCANTrigger(int boardAddress, TPC_CANTriggerSettings *settings) {
	ns__CANTriggerSettings settingsCopy;
	soap_call_ns__GetCANTrigger(&m_soap, m_url.c_str(), NULL, boardAddress, settingsCopy);
	memcpy(settings, &settingsCopy, sizeof(TPC_CANTriggerSettings));
	return Epilog();
}

//--- Attributes ----------------------------------------------------------------


TPC_ErrorCode SoapDevice::SetAttributes(const Attributes& attributes)
{
	ns__AttributeList list;
	int count = attributes.Count();

	list.__size = count;
	list.__ptr = (ns__Attribute*)soap_malloc(&m_soap, sizeof(ns__Attribute)*count);
	ns__Attribute* p = list.__ptr;
	for (Attributes::const_iterator i = attributes.begin(); i != attributes.end(); ++i) {
		p->boardAddress = (*i).first.boardAddress();
		p->inputNumber = (*i).first.inputNumber();
		string key = (*i).first.name();
		p->name = (char*)soap_malloc(&m_soap, key.size()+1);
		strcpy(p->name, key.c_str());
		string val = (*i).second;
		p->value = (char*)soap_malloc(&m_soap, val.size()+1);
		strcpy(p->value, val.c_str());
		p++;
	}
    ns__DummyResponse Response;
	soap_call_ns__SetAttributes(&m_soap, m_url.c_str(), NULL, list, &Response);
	return Epilog();
}


TPC_ErrorCode SoapDevice::GetAllCurrentAttributes(Attributes& result)
{
	ns__AttributeList list;

	int err = soap_call_ns__GetAllCurrentAttributes(&m_soap, m_url.c_str(), NULL, list);
	result.Clear();
	if (err == 0) {
		for (int i=0; i<list.__size; i++) {
			result.SetAttribute(list.__ptr[i].boardAddress, list.__ptr[i].inputNumber, 
				list.__ptr[i].name, list.__ptr[i].value);
		}
	}
	return Epilog();
}


TPC_ErrorCode SoapDevice::GetAllPreviousAttributes(int currentMeasurementNumber, Attributes& result)
{
	ns__AttributeList list;

	int err = soap_call_ns__GetAllPreviousAttributes(&m_soap, m_url.c_str(), NULL, currentMeasurementNumber, list);
	result.Clear();
	if (err == 0) {
		for (int i=0; i<list.__size; i++) {
			result.SetAttribute(list.__ptr[i].boardAddress, list.__ptr[i].inputNumber, 
				list.__ptr[i].name, list.__ptr[i].value);
		}
	}
	return Epilog();
}


//--- Associations ----------------------------------------------------------------


TPC_ErrorCode SoapDevice::GetCurrentAssociations(AssociationRowList& associations)
{
	ns__AssociationRowList list;
	int err = soap_call_ns__GetAllCurrentAssociations(&m_soap, m_url.c_str(), NULL, list);
	associations.Clear();
	if (err == 0) {
		for (int i=0; i<list.__size; i++) {
			TPC_AssociatedChannel row[tpc_maxBoards*tpc_maxInputs];
			// Convert one row
			int boardAddress = list.__ptr[i].boardAddress;
			int inputNumber = list.__ptr[i].inputNumber;
			int rowLen = list.__ptr[i].associatedChannels.__size;
			for (int j=0; j<rowLen; j++) {
				row[j].boardAddress = list.__ptr[i].associatedChannels.__ptr[j].boardAddress;
				row[j].inputNumber = list.__ptr[i].associatedChannels.__ptr[j].inputNumber;
			}
			associations.SetAssociatedChannels(boardAddress, inputNumber, row, rowLen);
		}
	}
	return Epilog();
}


TPC_ErrorCode SoapDevice::GetAllPreviousAssociations(int measurementNumber, AssociationRowList& associations)
{
	ns__AssociationRowList list;
	int err = soap_call_ns__GetAllPreviousAssociations(&m_soap, m_url.c_str(), NULL, measurementNumber, list);
	associations.Clear();
	if (err == 0) {
		for (int i=0; i<list.__size; i++) {
			TPC_AssociatedChannel row[tpc_maxBoards*tpc_maxInputs];
			// Convert one row
			int boardAddress = list.__ptr[i].boardAddress;
			int inputNumber = list.__ptr[i].inputNumber;
			int rowLen = list.__ptr[i].associatedChannels.__size;
			for (int j=0; j<rowLen; j++) {
				row[j].boardAddress = list.__ptr[i].associatedChannels.__ptr[j].boardAddress;
				row[j].inputNumber = list.__ptr[i].associatedChannels.__ptr[j].inputNumber;
			}
			associations.SetAssociatedChannels(boardAddress, inputNumber, row, rowLen);
		}
	}
	return Epilog();
}


TPC_ErrorCode SoapDevice::SetAssociations(const AssociationRowList& associations)
{
	// Allocate space for the list of rows
	ns__AssociationRowList list;
	int nRows = associations.Count();
	list.__size = nRows;
	list.__ptr = (ns__AssociationRow*)soap_malloc(&m_soap, sizeof(ns__AssociationRow)*nRows); 

	// Loop over the entries and convert the rows
	for (int j=0; j<nRows; j++) {
		ns__AssociationRow* rowD = &list.__ptr[j];
		const AssociationRow* rowS = &associations[j];

		int rowLen = rowS->Count();
		rowD->boardAddress = rowS->GetBoardAddress();
		rowD->inputNumber = rowS->GetInputNumber();
		rowD->associatedChannels.__size = rowLen;
		int min = rowLen <= 0 ? 1 : rowLen;
		rowD->associatedChannels.__ptr = (ns__AssociationChannel*)soap_malloc(
										&m_soap, sizeof(ns__AssociationChannel)*min);
		for (int i=0; i<rowLen; i++) {
			rowD->associatedChannels.__ptr[i].boardAddress = (*rowS)[i].boardAddress;
			rowD->associatedChannels.__ptr[i].inputNumber = (*rowS)[i].inputNumber;
		}
	}

	// Call soap
	ns__DummyResponse Response;
	soap_call_ns__SetAssociations(&m_soap, m_url.c_str(), NULL, list, &Response);
	return Epilog();
}


//--- Read/Write Device ---------------------------------------------------------


TPC_ErrorCode SoapDevice::WriteDev(int boardAddress, int type, int count, void* data, unsigned aux1, unsigned aux2)
{
	ns__DeviceData d;
	d.__size = count;
	d.__ptr = (unsigned char*)soap_malloc(&m_soap, count);
	for (int i=0; i<count; i++) {
		d.__ptr[i] = ((char*)data)[i];
	}
	ns__DummyResponse Response;
	soap_call_ns__WriteDev(&m_soap, m_url.c_str(), NULL, boardAddress, type, d, aux1, aux2, &Response);
	return Epilog();
}



TPC_ErrorCode SoapDevice::ReadDev(int boardAddress, int type, int count, void* data,	unsigned aux1, unsigned aux2)
{
	ns__DeviceData result;
	int err = soap_call_ns__ReadDev(&m_soap, m_url.c_str(), NULL, boardAddress, type, count, aux1, aux2, result);
	if (err == 0) {
		if (count != result.__size) return Epilog(tpc_errInternalError);
		for (int i=0; i<count; i++) {
			((char*)data)[i] = result.__ptr[i];
		}
	}
	return Epilog();
}


TPC_ErrorCode SoapDevice::ReadWriteTwi(int boardAddress, int amplifier, int count, char *data)
{
	ns__DeviceData d;
	d.__size = count;
	d.__ptr = (unsigned char*)soap_malloc(&m_soap, count);
  memcpy(d.__ptr, data, count);
  ns__DeviceData result;
  int err = soap_call_ns__ReadWriteTwi(&m_soap, m_url.c_str(), NULL, boardAddress, amplifier, d, result);
  if (err == 0)
  {
    if (count != result.__size) 
      return Epilog(tpc_errInternalError);
    memcpy(data, result.__ptr, count);
  }
  return Epilog();
}


//--- Commands ------------------------------------------------------------------

TPC_ErrorCode SoapDevice::PrepareStart(double* delayTime)
{
	int err = soap_call_ns__PrepareStart(&m_soap, m_url.c_str(), NULL, delayTime);
	if (err != 0) delayTime = 0;
	return Epilog();
}



TPC_ErrorCode SoapDevice::SetStartInfo(const TPC_DateTime& startTime, int measurementNr)
{
	ns__DateTime dt;
	ns__DummyResponse Response;
	Assign(dt, startTime);
	soap_call_ns__SetStartInfo(&m_soap, m_url.c_str(), NULL, dt, measurementNr, &Response);
	return Epilog();
}



TPC_ErrorCode SoapDevice::ExecuteCommand(TPC_SystemCommand command)
{
	ns__DummyResponse Response;
	soap_call_ns__ExecuteCommand(&m_soap, m_url.c_str(), NULL, command, &Response);
	return Epilog();
}



TPC_ErrorCode SoapDevice::StartCalibration(TPC_CalibrationType type)
{
	ns__DummyResponse Response;
	soap_call_ns__StartCalibration(&m_soap, m_url.c_str(), NULL, type, &Response);
	return Epilog();
}


//--- Device state --------------------------------------------------------------

TPC_ErrorCode SoapDevice::GetStatus(ns__Status& status)
{
	soap_call_ns__GetStatus(&m_soap, m_url.c_str(), NULL, status);
	return Epilog();
}



TPC_ErrorCode SoapDevice::WaitForStatusChange(ns__Status& status)
{
	soap_call_ns__WaitForStatusChange(&m_soap, m_url.c_str(), NULL, status.eventCounter, status);
	return Epilog();
}


TPC_ErrorCode SoapDevice::LeaveWaitStatus()
{
	ns__DummyResponse Response;
	soap_call_ns__LeaveWaitStatus(&m_soap, m_url.c_str(), NULL, &Response);
	return Epilog();
}



//--- Data readout --------------------------------------------------------------


void Assign(RawYMetaData& d, const ns__YMetadata& s)
{
	d.isActive = s.isActive;
	d.resolutionInBits = s.resolutionInBits;
	d.analogMask = s.analogMask;
	d.markerMask = s.markerMask;
	d.numberOfMarkerBits = s.numberOfMarkerBits;
	d.bytesPerSample = s.bytesPerSample;
	d.binToVoltFactor = s.binToVoltFactor;
	d.binToVoltConst = s.binToVoltConst;
	d.voltToPhysicalFactor = s.voltToPhysicalFactor;
	d.voltToPhysicalConstant = s.voltToPhysicalConstant;
}


TPC_ErrorCode SoapDevice::GetAllYTMetadata(int currentMeasurementNumber, YMetaDataList& metadata)
{
	metadata.Clear();

	ns__YMetadataList list;
	int err = soap_call_ns__GetAllYMetadata(&m_soap, m_url.c_str(), NULL, 
			currentMeasurementNumber, list);
	if (err == 0) {
		for (int i=0; i<list.__size; i++) {
			RawYMetaData m;
			Assign(m, list.__ptr[i]);
			metadata.SetMetaData(list.__ptr[i].boardAddress, list.__ptr[i].inputNumber, m);
		}
	}
	return Epilog();
}



TPC_ErrorCode SoapDevice::GetMetadata(int currentMeasurementNumber, int boardAddress, int blockFrom, int blockTo, ns__Metadata metadata[])
{
	ns__MetadataList list;
	int err = soap_call_ns__GetMetadata(&m_soap, m_url.c_str(), NULL, 
			currentMeasurementNumber, boardAddress, blockFrom, blockTo, list);
	
	if (err == 0) {
		int n = blockTo-blockFrom+1;
		if (n != list.__size) return Epilog(tpc_errInternalError);
		for (int i=0; i<n; i++) {
			metadata[i].triggerTime			= list.__ptr[i].triggerTime;
			metadata[i].triggerSample		= list.__ptr[i].triggerSample;
			metadata[i].blockLength			= list.__ptr[i].blockLength;
			metadata[i].stopTriggerSample	= list.__ptr[i].stopTriggerSample;
			
			metadata[i].boardAddress		= list.__ptr[i].boardAddress;
			metadata[i].inputNumber			= list.__ptr[i].inputNumber;
			
			
			
		}
	}
	return Epilog();
}



TPC_ErrorCode SoapDevice::GetData(int currentMeasurementNumber, DataReceiver* receiver)
{
	ns__DataSpecificationArray requests = receiver->PrepareForSoap(&m_soap);

	m_soap.user = (void*)receiver;

	soap_imode(&m_soap, SOAP_IO_KEEPALIVE|SOAP_IO_CHUNK);
	soap_omode(&m_soap, SOAP_IO_KEEPALIVE);

	ns__Data data;
	soap_call_ns__GetData(&m_soap, m_url.c_str(), NULL, currentMeasurementNumber, requests, data);

	soap_imode(&m_soap, SOAP_IO_KEEPALIVE);
	soap_omode(&m_soap, SOAP_IO_KEEPALIVE);
	return Epilog();
}



/**********************************************************************************/
/* Auto sequences
/**********************************************************************************/
TPC_ErrorCode SoapDevice::LoadAutosequence( char * cData, int iLength){
	
	char *string;
	ns__AutoSequence Sequence;
	Sequence.__ptr	= cData;
	Sequence.__size	=  iLength;
	soap_call_ns__LoadAutosequence(&m_soap,m_url.c_str(),NULL,Sequence,&string);
	return Epilog();
}

/**********************************************************************************/
/* Get Auto Sequene witch is stored on the server
/**********************************************************************************/
TPC_ErrorCode SoapDevice::GetAutoSequence(	char * cData, int iLength){
	
	//char * pcBuffer = NULL;
	ns__AutoSequence Sequence;
	soap_call_ns__GetAutoSequence(&m_soap,m_url.c_str(),NULL,Sequence);
	if ( Sequence.__size != 0){
		if(Sequence.__size < iLength){
			memcpy(cData,Sequence.__ptr,Sequence.__size);
		}
		else{
			memcpy(cData,Sequence.__ptr,iLength);
		}
		//pcBuffer = new char[Sequence.__size+1]; 
		//memset(pcBuffer,0x00,Sequence.__size+1);
		//memcpy(pcBuffer,Sequence.__ptr,sizeof(char)*Sequence.__size);

		//iLength = Sequence.__size;
		//*cData = pcBuffer;
	}
	
	return Epilog();
}
/**********************************************************************************/
/* Start the Autosequence
/**********************************************************************************/
TPC_ErrorCode SoapDevice::StartAutoSequence(){
	
	char *string;
	soap_call_ns__StartAutoSequence(&m_soap,m_url.c_str(),NULL,&string);
	
	return Epilog();
}
/**********************************************************************************/
/* Stop the Autosequence
/**********************************************************************************/
TPC_ErrorCode SoapDevice::StopAutoSequence(	){

	char *string;
	soap_call_ns__StopAutoSequence(&m_soap,m_url.c_str(),NULL,&string);
	return Epilog();
}


TPC_ErrorCode SoapDevice::GetGPSLogList(int from, int to, char *log){

	ns__GPSLogList list;
	soap_call_ns__GetGPSLogList(&m_soap,m_url.c_str(),NULL,from,to,list);

	memcpy(log,list.__ptr, list.__size);

	return Epilog();
}

TPC_ErrorCode SoapDevice::SendServerCustomCommand(char *command, char *result, int length)
{
  bool bufferTooShort = false;
  char *string;
  soap_call_ns__SendServerCustomCommand(&m_soap, m_url.c_str(), NULL, command, &string);
  memset(result, '\0', length);
  if (string != NULL && length > 0)
  {
    strncpy(result, string, length - 1);
    bufferTooShort = length < static_cast<int>(strlen(string) + 1);
  }

  TPC_ErrorCode err = Epilog();
  if (err == tpc_noError && bufferTooShort)
    return tpc_errBufferTooShort;
  return err;
}


//-------------------------------------------------------------------------------


TPC_ErrorCode SoapDevice::CheckSoapError()
{
	string lastFaultCode = "";
	string lastFaultString = "";
	string lastFaultDetail = "";

	TPC_ErrorCode err = tpc_noError;

	if (m_soap.error) { 
		if (m_soap.error > 0) 
			err = tpc_errTransmissionError;
		else if (m_soap.error < 0) 
			err = tpc_errTransmissionError; // SOAP Eof Error #######

		if (!*soap_faultcode(&m_soap))
			soap_set_fault(&m_soap);

		const char **s = soap_faultcode(&m_soap);
		if (s && *s) 
			lastFaultCode = *s;

		s = soap_faultstring(&m_soap);
		if (s && *s) 
			lastFaultString = *s;

		s = soap_faultdetail(&m_soap);
		if (s && *s) 
			lastFaultDetail = *s;

		if (m_soap.error == 2 
			&& (lastFaultCode == "SOAP-ENV:Server") 
			&& strncmp(lastFaultString.c_str(), "TPCError:", 9) == 0) {
			// TarnsPC error: extract error number from m_lastFaultString. Format: "TPCError:11"
			int errNr = atoi(lastFaultString.c_str()+9);
			err = (TPC_ErrorCode)errNr;
			lastFaultCode = "TPC Error";
			lastFaultString = lastFaultDetail;
			lastFaultDetail = "";
		}

#ifdef _DEBUG
#ifdef WIN32
		string errStr = lastFaultCode+ ";" + lastFaultString + ";" + lastFaultDetail;
		OutputDebugStringA(errStr.c_str());
		OutputDebugStringA("\n");
#endif
#endif

	}

	return err;
}


TPC_ErrorCode SoapDevice::Epilog()
{
	TPC_ErrorCode err = CheckSoapError();
	soap_destroy(&m_soap); // delete deserialized class instances
	soap_end(&m_soap); // remove deserialized data and clean up
	return err;
}


TPC_ErrorCode SoapDevice::Epilog(TPC_ErrorCode err)
{
	soap_destroy(&m_soap); // delete deserialized class instances
	soap_end(&m_soap); // remove deserialized data and clean up
	return err;
}


//-------------------------------------------------------------------------------

