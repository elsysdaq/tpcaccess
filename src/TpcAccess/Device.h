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
  $Id: Device.h 28 2017-06-06 13:31:56Z roman $
  This class holds all information and state for one device.
--------------------------------------------------------------------------------*/
#ifndef Device_h
#define Device_h Device_h
//---------------------------------------------------------------------------------

#include <string>
#include <vector>
using namespace std;

#include "TpcAccess.h"
#include "TpcAccessElsys.h"

#include "sysThreading.h"
#include "SoapDevice.h"
#include "DeviceInfo.h"
#include "ParameterSet.h"
#include "Attributes.h"
#include "YMetaData.h"
#include "AssocRow.h"

//---------------------------------------------------------------------------------


class System;
class StatusObserverThread;

class Device
{
public:
	Device(const char* url, int index, System* system, int recvTimeOut,int sendTimeOut);
	virtual ~Device();

	int Index() { return m_index; }
	void SetIndex(int index) { m_index = index; }
	string Url() { return m_url; }

public: // Device info
	TPC_ErrorCode GetDeviceInfo(TPC_DeviceInfo* deviceInfo, int structSize);
	TPC_ErrorCode SetDeviceSettings(TPC_DeviceInfo deviceInfo);
	TPC_ErrorCode GetBoardInfo(int boardAddress, TPC_BoardInfo* boardInfo, int structSize);
	TPC_ErrorCode GetInputInfo(int boardAddress, int inputNumber, TPC_InputInfo* inputInfo, int structSize);
	TPC_ErrorCode GetFreeDiskSpace( uint64_t* freeDiskSpace, uint64_t* DiskSize);
public: // Synchronisation
	TPC_ErrorCode AcquireDeviceLock();
	TPC_ErrorCode ReleaseDeviceLock();

public: // Configuration reset
	TPC_ErrorCode ResetConfiguration();

public: // Cluster configuration 
	TPC_ErrorCode GetLastClusterNumbers(int clusterNumbers[tpc_maxBoards]);
	TPC_ErrorCode GetClusterNumbers(int* clusterNumbers);
	TPC_ErrorCode SetClusterNumbers(int* clusterNumbers);
	TPC_ErrorCode BoardAddressToGroupNumber(int boardAddress, int &groupNumber);
	TPC_ErrorCode GroupNumberToBoardAddress(int groupNumber, int &boardAddress);
	TPC_ErrorCode GetAllClusterNumbers(vector<int> &clusterNumbers);

	TPC_ErrorCode GetBoardRestrictions(int boardAddress, BoardRestrictions* br);

public: // Parameters and Attributes 
	TPC_ErrorCode SetParameter(int boardAddress, int inputNumber, TPC_Parameter parameter, double value);
	TPC_ErrorCode SetTimebaseParameterToGroup(int groupNumber, TPC_Parameter parameter, double value);
	TPC_ErrorCode GetParameter(int boardAddress, int inputNumber, TPC_Parameter parameter, double* value);
	TPC_ErrorCode GetTimebaseParameterFromGroup(int groupNumber, TPC_Parameter parameter, double* value);
	TPC_ErrorCode SetAttribute(int boardAddress, int inputNumber, const char* key, const char* value);
	TPC_ErrorCode GetAttribute(int boardAddress, int inputNumber, const char* key, string& value);
	TPC_ErrorCode GetAllAttributes(TPC_AttributeEnumeratorCallback callback, void* userData);
	TPC_ErrorCode GetAssociatedChannels(int boardAddress, int inputNumber, TPC_AssociatedChannel* list, int* count);
	TPC_ErrorCode SetAssociatedChannels(int boardAddress, int inputNumber, TPC_AssociatedChannel* list, int count);
	TPC_ErrorCode SetTrigger(int boardAddress, int inputNumber, TPC_EasyTriggerMode mode, TPC_EasyTriggerComperatorMode comp,
				TPC_EasyTriggerFlags flags, double level, double hysteresis, int time, int time2);
	TPC_ErrorCode GetTrigger(int boardAddress, int inputNumber, TPC_EasyTriggerMode *mode, TPC_EasyTriggerComperatorMode *comp, 
				TPC_EasyTriggerFlags *flags, double *level, double *hysteresis, int *time, int *time2);

	TPC_ErrorCode ResetSerTrg(int boardAddress);
	TPC_ErrorCode GetSerTrgProtocol(int boardAddress, TPC_SerialTriggerProtocol *protocol);
	TPC_ErrorCode SetI2CTrigger(int boardAddress, TPC_I2CTriggerSettings *settings);
	TPC_ErrorCode GetI2CTrigger(int boardAddress, TPC_I2CTriggerSettings *settings);
	TPC_ErrorCode SetCANTrigger(int boardAddress, TPC_CANTriggerSettings *settings);
	TPC_ErrorCode GetCANTrigger(int boardAddress, TPC_CANTriggerSettings *settings);

	TPC_ErrorCode BeginSet();
	TPC_ErrorCode EndSet();
	TPC_ErrorCode CancelSet();

public: // Commands 
	TPC_ErrorCode ExecuteSystemCommand(TPC_SystemCommand command);
	TPC_ErrorCode StartCalibration(TPC_CalibrationType command);
	TPC_ErrorCode PrepareStart(double* delayTime);
	TPC_ErrorCode SetStartInfo(const TPC_DateTime& startTime, int measurementNr);

public: // Device status 
	TPC_ErrorCode GetDeviceStatus(TPC_DeviceStatus* status, int structSize);

public: // Data readout 
	TPC_ErrorCode GetYMetaData(int boardAddress, int inputNumber, 
					 int measurementNumber,
					 TPC_YMetaData* metaData, int structSize);
	TPC_ErrorCode GetMetaDataParameter(int boardAddress, int inputNumber, int measurementNumber, 
								TPC_Parameter parameter, double* value);
	TPC_ErrorCode GetMetaDataAttribute(int boardAddress, int inputNumber, int measurementNumber, 
								const char* key, string& value);
	TPC_ErrorCode GetAllMetaDataAttributes(int measurementNumber,
							 TPC_AttributeEnumeratorCallback callback, void* userData);
	TPC_ErrorCode GetMetaDataAssociatedChannels(int boardAddress, int inputNumber, 
									  int measurementNumber, TPC_AssociatedChannel* list, int* count);
	TPC_ErrorCode GetTMetaData(int boardAddress, int blockNumber, 
					 int measurementNumber,
					 TPC_TMetaData* metaData, int structSize);
	TPC_ErrorCode GetTMetaData(int boardAddress, int blockNumberFrom, int blockNumberTo,
					int measurementNumber,TPC_TMetaData * metaData, int structSize);

	enum ReadoutType { readoutRange, readoutMinMax };

	TPC_ErrorCode GetData(int boardAddress, int inputNumber, int blockNumber, 
				int measurementNumber, uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, double* data, ReadoutType type);
	TPC_ErrorCode GetRawData(int boardAddress, int inputNumber, int blockNumber, 
				int measurementNumber, uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, int32_t* data, ReadoutType type);

	TPC_ErrorCode DeferredGetData(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, double* data, TPC_ErrorCode* error, ReadoutType type);
	TPC_ErrorCode DeferredGetRawData(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, int32_t* data, TPC_ErrorCode* error, ReadoutType type);

	TPC_ErrorCode ProcessDeferredDataRequests(int measurementNumber);
	TPC_ErrorCode CancelDeferredDataRequests();

    TPC_ErrorCode SendServerCustomCommand(char *command, char *result, int length);

public: // Elsys functions
	TPC_ErrorCode WriteDev(int boardAddress, int type, unsigned count, void* data, unsigned aux1, unsigned aux2);
	TPC_ErrorCode ReadDev(int boardAddress, int type, unsigned count, void* data, unsigned aux1, unsigned aux2);
    TPC_ErrorCode ReadWriteTwi(int boardAddress, int amplifier, int count, char *data);

public: // Auto Sequence functions
	TPC_ErrorCode LoadAutosequence( char * cData, int iLength);
	TPC_ErrorCode GetAutoSequence(	char * cData, int iLength);
	TPC_ErrorCode StartAutoSequence();
	TPC_ErrorCode StopAutoSequence(	);

	TPC_ErrorCode GetGPSLogList(int from, int to, char *log);

private: // Initialization
	string m_url;
	int m_index;
	SoapDevice* m_soap;
	System* m_system;

	friend class System;
	void InitiateDisconnection();
	void FinishDisconnection();

private: // Device hardware info
	DeviceInfo m_deviceInfo;

private: // Device status
	friend class StatusObserverThread;

	void SetDeviceStatusError(TPC_ErrorCode err);
	void SetParamAttr(const ParameterSet& params, const Attributes& attr, const AssociationRowList& assoc);
	void SetPreviousParamAttr(const ParameterSet& params, const Attributes& attr, 
							const YMetaDataList& metadata, const AssociationRowList& assoc);
	void SetStatus(const ns__Status& status);
	bool m_settingsChanged;

	TPC_ErrorCode m_deviceStatusError;
	ns__Status m_currentStatus;

	cMutex m_statusMutex; // Mutex for exclusive access btw. main and status thread
	StatusObserverThread* m_statusThread;
	cMutex* getSyncObject(){ return &m_statusMutex;};

	void FillDeviceStatus(TPC_DeviceStatus* status);
	void CallStatusCallback();

  uint64_t GetDeviceID() {return m_deviceInfo.GetDeviceInfo().deviceID;}

private: // Parameters and attributes
	ParameterSet				m_currentParameters;
	ParameterModificationList	m_parameterModifications;
	Attributes					m_currentAttributes;
	Attributes					m_attributeModifications;
	int							m_deferredSets;
	int							m_settingsChangesCounter;
	AssociationRowList			m_associationModifications;
	AssociationRowList			m_currentAssociations;
	AssociationRowList			m_previousAssociations;
	ParameterSet				m_previousParameters;
	Attributes					m_previousAttributes;
	YMetaDataList				m_yMetadata;

	ParameterSet				m_triggerSettings;

    TPC_ErrorCode FetchCurrentParameters(SoapDevice* soap);
	TPC_ErrorCode FetchCurrentParameters();
	TPC_ErrorCode FetchCurrentAttributes(SoapDevice* soap);
	TPC_ErrorCode FetchCurrentAssociations(SoapDevice* soap);
	
	void SettingsChanged();

private: // Data readout
	DataReceiver m_dataReceiver; // Stores deferred read requests

private: // Auxiliary functions
	void SetStatus();
	
	TPC_ErrorCode CheckStatus();
	TPC_ErrorCode CheckBoard(int boardAddress);
	TPC_ErrorCode CheckInput(int boardAddress, int inputNumber);
	TPC_ErrorCode CheckMeasurementNumber(int measurementNumber);
};



//---------------------------------------------------------------------------------
#endif // Device_h

