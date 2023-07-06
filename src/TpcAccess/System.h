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
 * (C) Copyright 2005 - 2023 Elsys AG. All rights reserved.
*/
//---------------------------------------------------------------------------
/*--------------------------------------------------------------------------------
  $Id: System.h 28 2017-06-06 13:31:56Z roman $
  This class holds all information and state for the system.
--------------------------------------------------------------------------------*/
#ifndef System_h
#define System_h System_h
//---------------------------------------------------------------------------------
#ifdef WIN32
#include <WinSock2.h>
#endif

#include "TpcAccess.h"
#include "TpcAccessElsys.h"

#include <vector>
using namespace std;

#include "Device.h"
#include "sysThreading.h"

//---------------------------------------------------------------------------------


class System
{
public: // Device list
	void ClearDeviceList();
	int AddDevice(const char* url,int recvTimeOut,int sendTimeOut);
	void RemoveDevice(const char* url);
	int NumDevices();
	TPC_ErrorCode GetDeviceUrl(int index, string& url);
	TPC_ErrorCode GetConnectionState();

public: // Device info and settings
	TPC_ErrorCode GetDeviceInfo(int deviceIx, TPC_DeviceInfo* deviceInfo, int structSize);
	TPC_ErrorCode SetDeviceSettings(int deviceIx, TPC_DeviceInfo deviceInfo);
	TPC_ErrorCode GetBoardInfo(int deviceIx, int boardAddress, TPC_BoardInfo* boardInfo, int structSize);
	TPC_ErrorCode GetInputInfo(int deviceIx, int boardAddress, int inputNumber, TPC_InputInfo* inputInfo, int structSize);
	TPC_ErrorCode GetFreeDiskSpace(int deviceIx, uint64_t* freeDiskSpace, uint64_t* DiskSize);
public: // Synchronisation
  static bool DeviceComparator(Device *d1, Device *d2);
  void GetSortedDevicesList(vector<Device *> &devices);
  TPC_ErrorCode AcquireSystemLock();
  TPC_ErrorCode ReleaseSystemLock();

public: // Configuration reset
	TPC_ErrorCode ResetConfiguration();

public: // Cluster configuration 
	TPC_ErrorCode GetClusterNumbers(int deviceIx, int* clusterNumbers);
	TPC_ErrorCode SetClusterNumbers(int deviceIx, int* clusterNumbers);
//	TPC_ErrorCode GetClusterInfo(int deviceIx, int boardAddress,	TPC_ClusterInfo* clusterInfo, int structSize);
	TPC_ErrorCode CanBeClustered(int deviceIx1, int boardAddress1, int deviceIx2, int boardAddress2);

public: // Parameters and Attributes
  static bool IsTimebaseParameter(TPC_Parameter parameter);
  static bool IsBoardParameter(TPC_Parameter parameter);
  void GetTimebaseParameterList(vector<TPC_Parameter> &parameters);
  TPC_ErrorCode GetAllClusterNumbers(vector<int> &clusterNumbers);
  TPC_ErrorCode ClusterNumberToBoardAddress(int clusterNumber, int &boardAddress);
  TPC_ErrorCode SetTimebaseParameter(int clusterNumber, TPC_Parameter parameter, double value);
  TPC_ErrorCode GetTimebaseParameter(int clusterNumber, TPC_Parameter parameter, double *value);
  TPC_ErrorCode GetTimebaseParameterChecked(int clusterNumber, TPC_Parameter parameter, double *value);
  template <class T> 
  TPC_ErrorCode IterateThroughAll(int clusterNumber, T &Func);
  TPC_ErrorCode HasCluster(bool *clustered);
  TPC_ErrorCode RestrictParameterValue(int clusterNumber, TPC_Parameter parameter, double *value);
  TPC_ErrorCode SynchronizeTimebaseParameters();
  TPC_ErrorCode CheckTimebaseParameters();

  TPC_ErrorCode SetParameter(int deviceIx, int boardAddress, int inputNumber, TPC_Parameter parameter, double value);
	TPC_ErrorCode GetParameter(int deviceIx, int boardAddress, int inputNumber, TPC_Parameter parameter, double* value);
	TPC_ErrorCode SetAttribute(int deviceIx, int boardAddress, int inputNumber, const char* key, const char* value);
	TPC_ErrorCode GetAttribute(int deviceIx, int boardAddress, int inputNumber, const char* key, string& value);
	TPC_ErrorCode GetAllAttributes(int deviceIx, TPC_AttributeEnumeratorCallback callback, void* userData);
	TPC_ErrorCode GetAssociatedChannels(int deviceIx, int boardAddress, int inputNumber, 
															TPC_AssociatedChannel* list, int* count);
	TPC_ErrorCode SetAssociatedChannels(int deviceIx, int boardAddress, int inputNumber, 
															TPC_AssociatedChannel* list, int count);
	TPC_ErrorCode SetTrigger(int deviceIx, int boardAddress, int inputNumber, TPC_EasyTriggerMode mode, TPC_EasyTriggerComperatorMode comp, 
										TPC_EasyTriggerFlags flags, double level, double hysteresis, int time, int time2);
	TPC_ErrorCode GetTrigger(int deviceIx, int boardAddress, int inputNumber, TPC_EasyTriggerMode *mode, TPC_EasyTriggerComperatorMode *comp, 
										TPC_EasyTriggerFlags *flags, double *level, double *hysteresis, int *time, int *time2);

	TPC_ErrorCode ResetSerTrg(int deviceIx, int boardAddress);
	TPC_ErrorCode GetSerTrgProtocol(int deviceIx, int boardAddress, TPC_SerialTriggerProtocol *protocol);
	TPC_ErrorCode SetI2CTrigger(int deviceIx, int boardAddress, TPC_I2CTriggerSettings *settings);
	TPC_ErrorCode GetI2CTrigger(int deviceIx, int boardAddress, TPC_I2CTriggerSettings *settings);
	TPC_ErrorCode SetCANTrigger(int deviceIx, int boardAddress, TPC_CANTriggerSettings *settings);
	TPC_ErrorCode GetCANTrigger(int deviceIx, int boardAddress, TPC_CANTriggerSettings *settings);
	
	TPC_ErrorCode BeginSet();
	TPC_ErrorCode EndSet();
	TPC_ErrorCode CancelSet();

public: // Commands 
	TPC_ErrorCode ExecuteSystemCommand(TPC_SystemCommand command);
	TPC_ErrorCode MakeMeasurement(int timeout, int *measurementNumber);
	TPC_ErrorCode StartCalibration(int deviceIx, TPC_CalibrationType command);

public: // Device status 
	TPC_ErrorCode GetDeviceStatus(int deviceIx, TPC_DeviceStatus* status, int structSize);
	TPC_ErrorCode SetStatusCallback(TPC_StatusCallbackFunc callback, void* userData);

public: // Data readout 
	TPC_ErrorCode GetYMetaData(int deviceIx, int boardAddress, int inputNumber, 
					 int measurementNumber,
					 TPC_YMetaData* metaData, int structSize);
	TPC_ErrorCode GetMetaDataParameter(int deviceIx, int boardAddress, int inputNumber, 
								int measurementNumber, TPC_Parameter parameter, double* value);
	TPC_ErrorCode GetMetaDataAttribute(int deviceIx, int boardAddress, int inputNumber, 
							 int measurementNumber, const char* key, string& value);
	TPC_ErrorCode GetAllMetaDataAttributes(int deviceIx, int measurementNumber,
							 TPC_AttributeEnumeratorCallback callback, void* userData);
	TPC_ErrorCode GetMetaDataAssociatedChannels(int deviceIx, int boardAddress, int inputNumber, 
									  int measurementNumber, TPC_AssociatedChannel* list, int* count);

	TPC_ErrorCode GetTMetaData(int deviceIx, int boardAddress, int blockNumber, 
					 int measurementNumber,
					 TPC_TMetaData* metaData, int structSize);
    TPC_ErrorCode GetTMetaData(int deviceIx, int boardAddress, int blockNumberFrom, 
					 int blockNumberTo, int measurementNumber,
					 TPC_TMetaData* metaData, int structSize);

	TPC_ErrorCode GetData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
				int measurementNumber, uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, double* data, Device::ReadoutType type);
	TPC_ErrorCode GetRawData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
				int measurementNumber, uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, int32_t* data, Device::ReadoutType type);

	TPC_ErrorCode DeferredGetData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, double* data, TPC_ErrorCode* error, Device::ReadoutType type);
	TPC_ErrorCode DeferredGetRawData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, int32_t* data, TPC_ErrorCode* error, Device::ReadoutType type);

	TPC_ErrorCode ProcessDeferredDataRequests(int measurementNumber);
	TPC_ErrorCode CancelDeferredDataRequests();

public:
  TPC_ErrorCode SendServerCustomCommand(int deviceIx, char *command, char *result, int length);

public: // Elsys functions
	TPC_ErrorCode WriteDev(int deviceIx, int boardAddress, int type, unsigned count, void* data, unsigned aux1, unsigned aux2);
	TPC_ErrorCode ReadDev(int deviceIx, int boardAddress, int type, unsigned count, void* data, unsigned aux1, unsigned aux2);
  TPC_ErrorCode ReadWriteTwi(int deviceIx, int boardAddress, int amplifier, int count, char *data);

public:
	// Currently there is only one system.
	static System* TheSystem();
	static void PrepareToQuitProgram();

public:
	// Auto Sequence Functions
	TPC_ErrorCode LoadAutosequence(	int deviceIx, char * cData, int iLength);
	TPC_ErrorCode GetAutoSequence(	int deviceIx, char * cData, int iLength);
	TPC_ErrorCode StartAutoSequence( int deviceIx);
	TPC_ErrorCode StopAutoSequence(	 int deviceIx);

	TPC_ErrorCode GetGPSLogList( int deviceIx, int from, int to, char *log);
private:
	System();
	~System();
	static System* s_singleton;

private:
	vector<Device*> m_devices;

	TPC_StatusCallbackFunc m_callback;
	void* m_callbackUserData;
	cMutex m_callbackMutex;

	bool IsUrlEqual(const char* url1, const char* url2);
	Device* FindDevice(int ix);
	Device* FindDevice(const char* url);

private:
	friend class Device;
	friend class SystemList;
	void ExecuteCallback(int deviceIx, TPC_DeviceStatus* status);
};


//---------------------------------------------------------------------------------
#endif // System_h

