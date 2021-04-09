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
 * (C) Copyright 2005 - 2021 Elsys AG. All rights reserved.
*/

//---------------------------------------------------------------------------
/*--------------------------------------------------------------------------------
  $Id: System.cpp 29 2019-09-26 10:22:37Z roman $
  This class holds all information and state for the system.
--------------------------------------------------------------------------------*/
#include "System.h"
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;


#include "sysThreading.h"

#include <time.h>

//-------------------------------------------------------------------------------

System* System::s_singleton = NULL;


System* System::TheSystem()
{
	if (s_singleton == NULL) {
		s_singleton = new System();
	}
	return s_singleton;
}



void System::PrepareToQuitProgram()
{
	if (s_singleton != NULL) {
		delete s_singleton;
		s_singleton = NULL;
	}
}



System::System()
{
	m_callback = NULL;
	m_callbackUserData = NULL;
}



System::~System()
{
	ClearDeviceList();
}


//-------------------------------------------------------------------------------


int __stricmp(const char *s1, const char *s2)
{
  char c1, c2;
  do 
  {
    c1 = (char)toupper(*s1);
    c2 = (char)toupper(*s2);
    s1++;
    s2++;
  } while (c1 && c1 == c2);

  return (int) (c1 - c2);
}



bool System::IsUrlEqual(const char* url1, const char* url2)
{
	int x = __stricmp(url1, url2);
	return (x == 0);
}



Device* System::FindDevice(int ix)
{
    if (ix < 0) return NULL;
    if (ix >= (int)m_devices.size()) return NULL;
	return m_devices[ix];
}



Device* System::FindDevice(const char* url)
{
	vector<Device*>::iterator i;
	for (i = m_devices.begin(); i != m_devices.end(); ++i)
	{
		Device* d = *i;
		string u = d->Url();
		if (IsUrlEqual(url, u.c_str())) return d;
	}
	return NULL;
}


//--- Device list ---------------------------------------------------------------


void System::ClearDeviceList()
{
	// Do this in three steps because it may take some time for each device to 
	// close the connections.
	vector<Device*>::iterator i;

	// Step 1: Initiate disconnection
	for (i = m_devices.begin(); i != m_devices.end(); ++i) {
		(*i)->InitiateDisconnection();
	}

	// Step 2: Wait until disconnected. This step may take most time because
	// of the waiting for the status thread to terminate.
	for (i = m_devices.begin(); i != m_devices.end(); ++i) {
		(*i)->FinishDisconnection();
	}

	// Step 3: Dispose the device objects
	while (!m_devices.empty()) {
		Device* d = m_devices.back();
		m_devices.pop_back();
		delete d;
	}
}


int System::AddDevice(const char* url,int recvTimeOut,int sendTimeOut)
{
	// Check if the device is already in the list.
	Device* d = FindDevice(url);
	
	// If not, add it.
	if (d == NULL) {
		// Create the device object and add it to the list.
		int ix = m_devices.size();
		d = new Device(url, ix, this,recvTimeOut,sendTimeOut);
		m_devices.push_back(d);
	}
	
	return d->Index();
}

void System::RemoveDevice(const char* url)
{
	// Check if the device is already in the list.
	Device* d = FindDevice(url);

	if (d != NULL) {
		d->InitiateDisconnection();
		d->FinishDisconnection();
		m_devices.erase(std::remove(m_devices.begin(), m_devices.end(), d), m_devices.end());
		delete d;
	}

	// Reset design indexes.
	int index = 0;
	vector<Device*>::iterator i;
	for (i = m_devices.begin(); i != m_devices.end(); ++i)
	{
		Device* d = *i;
		d->SetIndex(index);
		index++;
	}
}

int System::NumDevices()
{
	return m_devices.size();
}



TPC_ErrorCode System::GetDeviceUrl(int index, string& url)
{
	if (index < 0 || index >= (int)m_devices.size()) return tpc_errInvalidIndex;

	Device* d = m_devices[index];
	url = d->Url();
	return tpc_noError;
}


TPC_ErrorCode System::GetConnectionState()
{
	TPC_ErrorCode err = tpc_noError;
	for (vector<Device*>::iterator i = m_devices.begin(); i != m_devices.end(); ++i) {
		TPC_DeviceStatus status;
		TPC_ErrorCode e = (*i)->GetDeviceStatus(&status, sizeof(status));
		if (err == 0) err = e;
		if (status.deviceError == tpc_errNoConnection) return tpc_errNoConnection;
		if (status.deviceError != 0) err = status.deviceError;
	}
	return err;
}


//--- Device info ---------------------------------------------------------------

TPC_ErrorCode System::GetDeviceInfo(int deviceIx, TPC_DeviceInfo* deviceInfo, int structSize)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetDeviceInfo(deviceInfo, structSize);
}

TPC_ErrorCode  System::SetDeviceSettings(int deviceIx, TPC_DeviceInfo deviceInfo){
	
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->SetDeviceSettings(deviceInfo);

}
TPC_ErrorCode System::GetBoardInfo(int deviceIx, int boardAddress, TPC_BoardInfo* boardInfo, int structSize)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetBoardInfo(boardAddress, boardInfo, structSize);
}



TPC_ErrorCode System::GetInputInfo(int deviceIx, int boardAddress, int inputNumber, TPC_InputInfo* inputInfo, int structSize)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetInputInfo(boardAddress, inputNumber, inputInfo, structSize);
}

TPC_ErrorCode System::GetFreeDiskSpace(int deviceIx, uint64_t* freeDiskSpace, uint64_t* DiskSize){
	
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetFreeDiskSpace(freeDiskSpace, DiskSize);
	
}

//--- Synchronisation -------------------------------------------------------


bool System::DeviceComparator(Device *d1, Device *d2)
{
  return d1->GetDeviceID() < d2->GetDeviceID();
}


void System::GetSortedDevicesList(vector<Device *> &devices)
{
  devices = m_devices;
  sort(devices.begin(), devices.end(), DeviceComparator);
}


TPC_ErrorCode System::AcquireSystemLock()
{
  vector<Device *> devices;
  GetSortedDevicesList(devices);
  for (int index = 0; index < (int)devices.size(); index++)
  {
    TPC_ErrorCode err = devices[index]->AcquireDeviceLock();
    if (err != 0)
    {
      ReleaseSystemLock();
      return err;
    }
  }
  return tpc_noError;
}


TPC_ErrorCode System::ReleaseSystemLock()
{
  vector<Device *> devices;
  GetSortedDevicesList(devices);
  for (int index = 0; index < (int)devices.size(); index++)
    devices[index]->ReleaseDeviceLock();
  return tpc_noError;
}


//--- Configuration reset -------------------------------------------------------

TPC_ErrorCode System::ResetConfiguration()
{
	TPC_ErrorCode err = tpc_noError;
	for (vector<Device*>::iterator i = m_devices.begin(); i != m_devices.end(); ++i) {
		TPC_ErrorCode e = (*i)->ResetConfiguration();
		if (err == 0) err = e;
	}
	return err;
}



//--- Cluster configuration -----------------------------------------------------

TPC_ErrorCode System::GetClusterNumbers(int deviceIx, int* clusterNumbers)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetClusterNumbers(clusterNumbers);
}



TPC_ErrorCode System::SetClusterNumbers(int deviceIx, int* clusterNumbers)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->SetClusterNumbers(clusterNumbers);
}



//	int GetClusterInfo(int deviceIx, int boardAddress,	TPC_ClusterInfo* clusterInfo, int structSize)
TPC_ErrorCode System::CanBeClustered(int deviceIx1, int boardAddress1, int deviceIx2, int boardAddress2)
{
	Device* d1 = FindDevice(deviceIx1);
	if (d1 == NULL) return tpc_errInvalidDeviceIx;
	Device* d2 = FindDevice(deviceIx2);
	if (d2 == NULL) return tpc_errInvalidDeviceIx;

//########
	return tpc_errNotImplemented;
}



//--- Parameters and Attributes -------------------------------------------------
bool System::IsBoardParameter(TPC_Parameter parameter){
	
	if(IsTimebaseParameter(parameter)) return true;

	switch (parameter){
		case tpc_parANDTriggerMask:
		case tpc_parAveragingMode:
			return true;
		default:
			return false;
	}
}

bool System::IsTimebaseParameter(TPC_Parameter parameter)
{
  switch (parameter)
  {
    case tpc_parOperationMode:
    case tpc_parMultiplexerMode:
    case tpc_parTimebaseSource:
    case tpc_parSamplingFrequency:
    case tpc_parExternalClockDivisor:
    case tpc_parExtClockSamplingFrequency:
    case tpc_parExternalTriggerMode:
    case tpc_parScopeBlockLength:
    case tpc_parScopeTriggerDelay:
    case tpc_parScopeAutoTrigger:
    case tpc_parScopeSingleShot:
    case tpc_parBlkNumberOfBlocks:
    case tpc_parBlkBlockLength:
    case tpc_parBlkTriggerDelay:
    case tpc_parContMaximumDataLength:
    case tpc_parContStopTrailer:
    case tpc_parContEnableStopTrigger:
    case tpc_parContEnableRecLengthLimit:
    case tpc_parEcrDualMode:							//MU
    case tpc_parEcrMinimumBlockLength:
    case tpc_parEcrTriggerDelay:
    case tpc_parEcrPreTrigger:
    case tpc_parEcrPostTrigger:
    case tpc_parEcrMaximumNumberOfBlocks:
    case tpc_parEcrHoldOffTime:
    case tpc_parEcrMaximumBlockLength:
    case tpc_parEcrRetriggerTime:
    case tpc_parEcrTrailer:
    case tpc_parEcrClockDivisor:
    case tpc_parEcrEnableHoldOff:
    case tpc_parEcrDualEnableTrailer:
    case tpc_parEcrDualTrailer:
    case tpc_parEcrDualEnableRecLengthLimit:
    case tpc_parEcrDualMaxRecLength:
    case tpc_parEcrEnableStopTrigger:
    case tpc_parEcrEnableRetrigger:
	//case tpc_parLXIMsgConfig:
	case tpc_parGPSSync:
	case tpc_parSyncPingMode:
	case tpc_parSyncDelay:
      return true;
    default:
      return false;
  }
}


void System::GetTimebaseParameterList(vector<TPC_Parameter> &parameters)
{
  parameters.push_back(tpc_parOperationMode);
  parameters.push_back(tpc_parMultiplexerMode);
  parameters.push_back(tpc_parTimebaseSource);
  parameters.push_back(tpc_parSamplingFrequency);
  parameters.push_back(tpc_parExternalClockDivisor);
  parameters.push_back(tpc_parExtClockSamplingFrequency);
  parameters.push_back(tpc_parExternalTriggerMode);
  parameters.push_back(tpc_parScopeBlockLength);
  parameters.push_back(tpc_parScopeTriggerDelay);
  parameters.push_back(tpc_parScopeAutoTrigger);
  parameters.push_back(tpc_parScopeSingleShot);
  parameters.push_back(tpc_parBlkNumberOfBlocks);
  parameters.push_back(tpc_parBlkBlockLength);
  parameters.push_back(tpc_parBlkTriggerDelay);
  parameters.push_back(tpc_parContMaximumDataLength);
  parameters.push_back(tpc_parContStopTrailer);
  parameters.push_back(tpc_parContEnableStopTrigger);
  parameters.push_back(tpc_parContEnableRecLengthLimit);
  parameters.push_back(tpc_parEcrDualMode);						//MU
  parameters.push_back(tpc_parEcrPreTrigger);
  parameters.push_back(tpc_parEcrPostTrigger);
  parameters.push_back(tpc_parEcrMaximumNumberOfBlocks);
  parameters.push_back(tpc_parEcrHoldOffTime);
  parameters.push_back(tpc_parEcrMaximumBlockLength);
  parameters.push_back(tpc_parEcrRetriggerTime);
  parameters.push_back(tpc_parEcrTrailer);
  parameters.push_back(tpc_parEcrClockDivisor);
  parameters.push_back(tpc_parEcrEnableHoldOff);
  parameters.push_back(tpc_parEcrDualEnableTrailer);
  parameters.push_back(tpc_parEcrDualTrailer);
  parameters.push_back(tpc_parEcrDualEnableRecLengthLimit);
  parameters.push_back(tpc_parEcrDualMaxRecLength);
  parameters.push_back(tpc_parEcrEnableStopTrigger);
  parameters.push_back(tpc_parEcrEnableRetrigger);
 // parameters.push_back(tpc_parLXIMsgConfig);
  parameters.push_back(tpc_parGPSSync);
  parameters.push_back(tpc_parSyncPingMode);
  parameters.push_back(tpc_parSyncDelay);
}


TPC_ErrorCode System::GetAllClusterNumbers(vector<int> &clusterNumbers)
{
  for (int deviceIdx = 0; deviceIdx < (int)m_devices.size(); deviceIdx++)
  {
    vector<int> numbers;
    m_devices[deviceIdx]->GetAllClusterNumbers(numbers);
    for (int index = 0; index < (int)numbers.size(); index++)
    {
      int number = numbers[index];
      bool contains = find(clusterNumbers.begin(), clusterNumbers.end(), number) != clusterNumbers.end();
      if (!contains)
        clusterNumbers.push_back(number);
    }
  }
  return tpc_noError;
}


TPC_ErrorCode System::ClusterNumberToBoardAddress(int clusterNumber, int &boardAddress)
{
  boardAddress = -1;
  vector<Device *> devices;
  GetSortedDevicesList(devices);
  for (int index = 0; index < (int)devices.size(); index++)
  {
    TPC_ErrorCode err = devices[index]->GroupNumberToBoardAddress(clusterNumber, boardAddress);
    if (err != tpc_errInvalidBoardAddress)
        return err;
  }
  return tpc_errInvalidBoardAddress;
}


TPC_ErrorCode System::SetTimebaseParameter(int clusterNumber, TPC_Parameter parameter, double value)
{
  for (int index = 0; index < (int)m_devices.size(); index++)
  {
    TPC_ErrorCode err = m_devices[index]->SetTimebaseParameterToGroup(clusterNumber, parameter, value);
    if (err != tpc_noError && err != tpc_errInvalidBoardAddress) 
      return err;
  }
  return tpc_noError;
}


TPC_ErrorCode System::GetTimebaseParameter(int clusterNumber, TPC_Parameter parameter, double *value)
{
  vector<Device *> devices;
  GetSortedDevicesList(devices);
  for (int index = 0; index < (int)devices.size(); index++)
  {
    TPC_ErrorCode err = devices[index]->GetTimebaseParameterFromGroup(clusterNumber, parameter, value);
    if (err != tpc_errInvalidBoardAddress)
        return err;
  }
  return tpc_errInvalidBoardAddress;
}


TPC_ErrorCode System::GetTimebaseParameterChecked(int clusterNumber, TPC_Parameter parameter, double *value)
{
  bool valueValid = false;
  for (int index = 0; index < (int)m_devices.size(); index++)
  {
    double groupValue;
    TPC_ErrorCode err = m_devices[index]->GetTimebaseParameterFromGroup(clusterNumber, parameter, &groupValue);
    if (err != tpc_errInvalidBoardAddress)
    {
      if (err != tpc_noError)
        return err;

      if (!valueValid)
      {
        *value = groupValue;
        valueValid = true;
      }
      else if (groupValue != *value)
        return tpc_errInvalidParameterValue;
    }
  }
  return tpc_noError;
}


class GroupParamterRestriction
{
public:
  GroupParamterRestriction()
  {
    value = INT_MAX;
    errorCode = tpc_errInvalidBoardAddress;
  }

  TPC_ErrorCode operator()(Device *device, int boardAddress)
  {
    if (errorCode != tpc_noError && errorCode != tpc_errInvalidBoardAddress)  
      return errorCode;
    TPC_BoardInfo info;
    errorCode = device->GetBoardInfo(boardAddress, &info, sizeof(TPC_BoardInfo));
    if (errorCode != tpc_noError) 
      return errorCode;
    CheckRestriction(info);
    return tpc_noError;
  }

  virtual void CheckRestriction(TPC_BoardInfo &info) = 0;

  int GetValue() {return value;}
  TPC_ErrorCode GetErrorCode() {return errorCode;}

protected:
  int value;
  TPC_ErrorCode errorCode;
};


class OperationModeOptions : public GroupParamterRestriction
{
  void CheckRestriction(TPC_BoardInfo &info) {value &= info.operationModeOptions;}
};


class MaximumSampleRate : public GroupParamterRestriction
{
  void CheckRestriction(TPC_BoardInfo &info) {value = min(value, info.maxSpeed);}
};


class MaximumBlockLength : public GroupParamterRestriction
{
  void CheckRestriction(TPC_BoardInfo &info) {value = min(value, info.maxMemory);}
};


class MaximumNumberOfInputs : public GroupParamterRestriction
{
public:
  MaximumNumberOfInputs() {value = 0;}
private:
  void CheckRestriction(TPC_BoardInfo &info) {value = max(value, info.numberOfInputs);}
};



template <class T> 
TPC_ErrorCode System::IterateThroughAll(int clusterNumber, T &Func)
{
  for (int deviceIndex = 0; deviceIndex < (int)m_devices.size(); deviceIndex++)
  {
    int clusterNumbers[tpc_maxBoards];
    TPC_ErrorCode err = m_devices[deviceIndex]->GetLastClusterNumbers(clusterNumbers);
    if (err != 0) return err;
    for (int index = 0; index < tpc_maxBoards; index++)
    {
      if (clusterNumbers[index] == clusterNumber)
      {
        err = Func(m_devices[deviceIndex], index);
        if (err != 0) return err;
      }
    }
  }
  return tpc_noError;
}


TPC_ErrorCode System::HasCluster(bool *clustered)
{
  *clustered = false;
  vector<int> clusterNumbers;
  TPC_ErrorCode err = GetAllClusterNumbers(clusterNumbers);
  if (err != tpc_noError) return err;
  if (m_devices.size() > 1 || clusterNumbers.size() > 1)
    *clustered = true;
  return tpc_noError; 
}


template <class T>
static T round(double Val)
{
  return static_cast<T>(std::floor(Val + ((Val >= 0) ? 0.5 : -0.5)));
}


TPC_ErrorCode System::RestrictParameterValue(int clusterNumber, TPC_Parameter parameter, double *value)
{
  TPC_ErrorCode err = tpc_noError;

  if (parameter == tpc_parOperationMode)
  {
    if (*value == tpc_opModeEventRecorderSingle || *value == tpc_opModeEventRecorderSingleDual ||
        *value == tpc_opModeEventRecorderMulti || *value == tpc_opModeEventRecorderMultiDual)
    {
      OperationModeOptions operationModeOptions;
      err = IterateThroughAll(clusterNumber, operationModeOptions);
      if (err != 0) return err;
      err = operationModeOptions.GetErrorCode();
      if (err != 0) return err;
      if ((operationModeOptions.GetValue() & tpc_optEcrExtensions) == 0)
        *value = tpc_opModeScope;
    }
  }
  else if (parameter == tpc_parMultiplexerMode)
  {
    MaximumNumberOfInputs numberOfInputs;
    err = IterateThroughAll(clusterNumber, numberOfInputs);
    if (err != 0) return err;
    err = numberOfInputs.GetErrorCode();
    if (err != 0) return err;
    *value = numberOfInputs.GetValue();
  }
  else if (parameter == tpc_parSamplingFrequency)
  { 
    MaximumSampleRate sampleRate;
    err = IterateThroughAll(clusterNumber, sampleRate);
    if (err != 0) return err;
    err = sampleRate.GetErrorCode();
    if (err != 0) return err;

    *value = max(1.0, *value);
    int divisor = round<int>(sampleRate.GetValue() / *value);
    divisor = max(1, divisor);
    *value = (double)sampleRate.GetValue() / divisor;
  }
  else if (parameter == tpc_parScopeBlockLength || parameter == tpc_parBlkBlockLength || 
           parameter == tpc_parContStopTrailer || parameter == tpc_parEcrPreTrigger ||
           parameter == tpc_parBlkNumberOfBlocks)
  {
    MaximumBlockLength blockLength;
    err = IterateThroughAll(clusterNumber, blockLength);
    if (err != 0) return err;
    err = blockLength.GetErrorCode();
    if (err != 0) return err;
    MaximumNumberOfInputs numberOfInputs;
    err = IterateThroughAll(clusterNumber, numberOfInputs);
    if (err != 0) return err;
    err = numberOfInputs.GetErrorCode();
    if (err != 0) return err;

    int channelMemory = blockLength.GetValue() / numberOfInputs.GetValue();
    if (parameter != tpc_parBlkNumberOfBlocks)
    {
      *value = min((double)channelMemory, *value);
    }
    else
    {
      double length;
      err = GetTimebaseParameter(clusterNumber, tpc_parBlkBlockLength, &length);
      if (err != 0) return err;
      if (channelMemory <= length)
        *value = 1;
      else
        *value = min(*value, channelMemory / length);
    }
  }
  else if (parameter == tpc_parScopeSingleShot && *value == 0)
  {
    bool clustered;
    err = HasCluster(&clustered);
    if (err != 0) return err;
    if (clustered)
      *value = 1;
  }
  return err;
}


class DeferredParamterSet
{
public:
  DeferredParamterSet(System *system)
  {
    this->system = system;
    system->BeginSet();
    cancel = true;
  }

  ~DeferredParamterSet()
  {
    if (cancel)
      system->CancelSet();
  }

  void EndSet()
  {
    system->EndSet();
    cancel = false;
  }

private:
  System *system;
  bool cancel;
};


TPC_ErrorCode System::SynchronizeTimebaseParameters()
{
  DeferredParamterSet parameterSet(this);

  vector<int> clusterNumbers;
  TPC_ErrorCode err = GetAllClusterNumbers(clusterNumbers);
  if (err != 0) return err;
  vector<TPC_Parameter> parameters;
  GetTimebaseParameterList(parameters);
  for (int clusterIndex = 0; clusterIndex < (int)clusterNumbers.size(); clusterIndex++)
  {
    int clusterNumber = clusterNumbers[clusterIndex];
    for (int parameterIndex = 0; parameterIndex < (int)parameters.size(); parameterIndex++)
    {
      TPC_Parameter parameter = parameters[parameterIndex];
      double value;
      err = GetTimebaseParameter(clusterNumber, parameter, &value);
      //if (err != 0) return err;
      err = RestrictParameterValue(clusterNumber, parameter, &value);
      //if (err != 0) return err;
      err = SetTimebaseParameter(clusterNumber, parameter, value);
      //if (err != 0) return err;
    }
  }

  parameterSet.EndSet();
  return tpc_noError;
}


TPC_ErrorCode System::CheckTimebaseParameters()
{
  vector<int> clusterNumbers;
  TPC_ErrorCode err = GetAllClusterNumbers(clusterNumbers);
  if (err != 0) return err;
  vector<TPC_Parameter> parameters;
  GetTimebaseParameterList(parameters);
  for (int clusterIndex = 0; clusterIndex < (int)clusterNumbers.size(); clusterIndex++)
  {
    int clusterNumber = clusterNumbers[clusterIndex];
    for (int parameterIndex = 0; parameterIndex < (int)parameters.size(); parameterIndex++)
    {
      TPC_Parameter parameter = parameters[parameterIndex];
      double value;
      err = GetTimebaseParameterChecked(clusterNumber, parameter, &value);
      if (err != 0) return err;
    }
  }
  return tpc_noError;
}



TPC_ErrorCode System::SetParameter(int deviceIx, int boardAddress, int inputNumber, TPC_Parameter parameter, double value)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;

  bool clustered = false;
  if (IsTimebaseParameter(parameter))
  {
    TPC_ErrorCode err = HasCluster(&clustered);
    if (err != 0) return err;
  }

  if (clustered)
  {
    int clusterNumber;
    TPC_ErrorCode err = d->BoardAddressToGroupNumber(boardAddress, clusterNumber);
    if (err != 0) return err;
    err = RestrictParameterValue(clusterNumber, parameter, &value);
    if (err != 0) return err;
    return SetTimebaseParameter(clusterNumber, parameter, value);
  }
  else{
	  if(IsBoardParameter(parameter)) return d->SetParameter(boardAddress, 0, parameter, value);
	  return d->SetParameter(boardAddress, inputNumber, parameter, value);
  }
}


TPC_ErrorCode System::GetParameter(int deviceIx, int boardAddress, int inputNumber, TPC_Parameter parameter, double* value)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;

  bool clustered = false;
  if (IsTimebaseParameter(parameter))
  {
    TPC_ErrorCode err = HasCluster(&clustered);
    if (err != 0) return err;
  }

  if (clustered)
  {
    int clusterNumber;
    TPC_ErrorCode err = d->BoardAddressToGroupNumber(boardAddress, clusterNumber);
    if (err != 0) return err;
    err = GetTimebaseParameter(clusterNumber, parameter, value);
    if (err != 0) return err;
    return RestrictParameterValue(clusterNumber, parameter, value);
  }
  else{
	if(IsBoardParameter(parameter)) return d->GetParameter(boardAddress, 0, parameter, value);
    return d->GetParameter(boardAddress, inputNumber, parameter, value);
  }
}



TPC_ErrorCode System::SetAttribute(int deviceIx, int boardAddress, int inputNumber, const char* key, const char* value)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->SetAttribute(boardAddress, inputNumber, key, value);
}



TPC_ErrorCode System::GetAttribute(int deviceIx, int boardAddress, int inputNumber, const char* key, string& value)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetAttribute(boardAddress, inputNumber, key, value);
}


TPC_ErrorCode System::GetAllAttributes(int deviceIx, TPC_AttributeEnumeratorCallback callback, void* userData)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetAllAttributes(callback, userData);
}


TPC_ErrorCode System::GetAssociatedChannels(int deviceIx, int boardAddress, int inputNumber, 
													   TPC_AssociatedChannel* list, int* count)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetAssociatedChannels(boardAddress, inputNumber, list, count);
}


TPC_ErrorCode System::SetAssociatedChannels(int deviceIx, int boardAddress, int inputNumber, 
													   TPC_AssociatedChannel* list, int count)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->SetAssociatedChannels(boardAddress, inputNumber, list, count);
}


TPC_ErrorCode System::SetTrigger(int deviceIx, int boardAddress, int inputNumber, TPC_EasyTriggerMode mode, TPC_EasyTriggerComperatorMode comp, TPC_EasyTriggerFlags flags, double level, double hysteresis, int time, int time2)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;

	return d->SetTrigger(boardAddress, inputNumber, mode, comp, flags, level, hysteresis, time, time2);
}

TPC_ErrorCode System::GetTrigger(int deviceIx, int boardAddress, int inputNumber, TPC_EasyTriggerMode *mode, TPC_EasyTriggerComperatorMode *comp, TPC_EasyTriggerFlags *flags, double *level, double *hysteresis, int *time, int *time2)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;

	return d->GetTrigger(boardAddress, inputNumber, mode, comp, flags, level, hysteresis, time, time2);
}

TPC_ErrorCode System::ResetSerTrg(int deviceIx, int boardAddress) {
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;

	return d->ResetSerTrg(boardAddress);
}

TPC_ErrorCode System::GetSerTrgProtocol(int deviceIx, int boardAddress, TPC_SerialTriggerProtocol *protocol) {
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;

	return d->GetSerTrgProtocol(boardAddress, protocol);
}

TPC_ErrorCode System::SetI2CTrigger(int deviceIx, int boardAddress, TPC_I2CTriggerSettings *settings) {
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;

	return d->SetI2CTrigger(boardAddress, settings);
}

TPC_ErrorCode System::GetI2CTrigger(int deviceIx, int boardAddress, TPC_I2CTriggerSettings *settings) {
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;

	return d->GetI2CTrigger(boardAddress, settings);
}

TPC_ErrorCode System::SetCANTrigger(int deviceIx, int boardAddress, TPC_CANTriggerSettings *settings) {
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;

	return d->SetCANTrigger(boardAddress, settings);
}

TPC_ErrorCode System::GetCANTrigger(int deviceIx, int boardAddress, TPC_CANTriggerSettings *settings) {
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;

	return d->GetCANTrigger(boardAddress, settings);
}

TPC_ErrorCode System::BeginSet()
{
	for (vector<Device*>::iterator i = m_devices.begin(); i != m_devices.end(); ++i) {
		(*i)->BeginSet();
	}
	return tpc_noError;
}



TPC_ErrorCode System::EndSet()
{
	TPC_ErrorCode err = tpc_noError;
	for (vector<Device*>::iterator i = m_devices.begin(); i != m_devices.end(); ++i) {
		TPC_ErrorCode e = (*i)->EndSet();
		if (err == 0) err = e;
	}
	return err;
}



TPC_ErrorCode System::CancelSet()
{
	for (vector<Device*>::iterator i = m_devices.begin(); i != m_devices.end(); ++i) {
		(*i)->CancelSet();
	}
	return tpc_noError;
}



//--- Commands ------------------------------------------------------------------

static TPC_DateTime GetCurrentSystemTime()
{
	
	time_t ts = time(NULL);

	tm t = *localtime(&ts); 

	TPC_DateTime tt;
	tt.year = t.tm_year + 1900;
	tt.month = t.tm_mon + 1;
	tt.day = t.tm_mday;
	tt.hour = t.tm_hour;
	tt.minute = t.tm_min;
	tt.second = t.tm_sec;
	tt.milliSecond = 0;

	return tt;
}


class SystemLock
{
public:
  SystemLock(System *system)
  {
    this->system = system;
    release = true;
    system->AcquireSystemLock();
  }

  ~SystemLock()
  {
    if (release)
      system->ReleaseSystemLock();
  }

  void Release()
  {
    release = false;
    system->ReleaseSystemLock();
  }

private:
  System *system;
  bool release;
};



TPC_ErrorCode System::ExecuteSystemCommand(TPC_SystemCommand command)
{
	// Check if all devices are OK and determine the primary device.
	int measurementNr = 0;
	Device* primary = NULL;
	uint64_t minId = LLONG_MAX;
	bool running = false;
	bool SyncLinkDetectedonAll = true;
	vector<Device*>::iterator i;
	for (i = m_devices.begin(); i != m_devices.end(); ++i) {
		Device* d = *i;

		// Check status
		TPC_DeviceStatus status;
		TPC_ErrorCode err = d->GetDeviceStatus(&status, sizeof(status));
		if (err != 0) return err;
		if (status.deviceError != 0) return status.deviceError;

		for (int b=0; b<tpc_maxBoards; b++) {
			if (status.boards[b].recordingState == tpc_recStarting
				|| status.boards[b].recordingState == tpc_recRecording) {
				running = true;
			}
		}

		// Compare device id
		TPC_DeviceInfo deviceInfo;
		err = d->GetDeviceInfo(&deviceInfo, sizeof(deviceInfo));
		if (err != 0) return err;
		uint64_t id = deviceInfo.deviceID;
		if (primary == NULL || id < minId) {
			minId = id;
			primary = d;
		}
		if (deviceInfo.SyncLinkDetected != tpc_sync_detected) SyncLinkDetectedonAll = false;
		measurementNr = max(measurementNr, status.measurementNumber + 1);
	}

	if (primary == NULL) {
		return tpc_errInvalidDeviceIx;
	}

	if (command == tpc_cmdStart) {
		if (running) return tpc_errWrongSystemState;

		SystemLock systemLock(this);

		TPC_ErrorCode err = SynchronizeTimebaseParameters();
		if (err != tpc_noError) return err;

		#ifdef _DEBUG
			err = CheckTimebaseParameters();
			if (err != tpc_noError) return err;
		#endif

		// Lock the mutex on the primary device.
		//err = primary->ExecuteSystemCommand((TPC_SystemCommand)tpc_cmdStartLock);
		//if (err != tpc_noError) return err;

		// Let every device prepare for the start
		double waitTime = 0;
		for (i = m_devices.begin(); i != m_devices.end(); ++i) {
			Device* d = *i;

			double t;
			err = d->PrepareStart(&t);
			if (err != tpc_noError) return err;
			if (t > waitTime) waitTime = t;
		}
		if (waitTime > 0)
			NonWaitableThread::sleep((int)waitTime*1000);

		// Determine the start time and measurement number.
		TPC_DateTime startTime = GetCurrentSystemTime();

		if(SyncLinkDetectedonAll == true || m_devices.size() == 1){
			// Send start command to the primary device.
			err = primary->ExecuteSystemCommand(tpc_cmdStart);
			if (err != 0) return err;
		} else{
			// Send start command to all devices
			for (i = m_devices.begin(); i != m_devices.end(); ++i){
				Device* d = *i;
				TPC_ErrorCode e;
				e = d->ExecuteSystemCommand(tpc_cmdStart);
				if (err == 0) err = e;
			}
		}

		// Distribute the start time and measurement number to the other devices.
		for (i = m_devices.begin(); i != m_devices.end(); ++i){
			Device* d = *i;
			TPC_ErrorCode e = d->SetStartInfo(startTime, measurementNr);
			if (err == 0) err = e;
		}

		systemLock.Release();
		return err;
	} else if (command == tpc_cmdStartGPSSynced){
		if (running) return tpc_errWrongSystemState;

		SystemLock systemLock(this);

		TPC_ErrorCode err = SynchronizeTimebaseParameters();
		if (err != tpc_noError) return err;

		#ifdef _DEBUG
			err = CheckTimebaseParameters();
			if (err != tpc_noError) return err;
		#endif

		// Lock the mutex on the primary device.
		//err = primary->ExecuteSystemCommand((TPC_SystemCommand)tpc_cmdStartLock);
		//if (err != tpc_noError) return err;

		
		double waitTime = 0;
		for (i = m_devices.begin(); i != m_devices.end(); ++i) {
			Device* d = *i;

			double t;
			err = d->PrepareStart(&t);
			if (err != tpc_noError) return err;
			if (t > waitTime) waitTime = t;
		}
		if (waitTime > 0)
			NonWaitableThread::sleep((int)waitTime*1000);

		// Determine the start time and measurement number.
		TPC_DateTime startTime = GetCurrentSystemTime();
		startTime.second++; // The measurement is started at the next second pulse. 
		// Distribute the start time and measurement number to the other devices.
		for (i = m_devices.begin(); i != m_devices.end(); ++i){
			Device* d = *i;
			TPC_ErrorCode e;
			e = d->ExecuteSystemCommand(tpc_cmdStartGPSSynced);
			if (err == 0) err = e;
		}
		for (i = m_devices.begin(); i != m_devices.end(); ++i){
			Device* d = *i;
			TPC_ErrorCode e;
			e = d->SetStartInfo(startTime, measurementNr);
			if (err == 0) err = e;
		}

		// Unlock the mutex on the primary device.
		//primary->ExecuteSystemCommand((TPC_SystemCommand)tpc_cmdStartUnlock);

    systemLock.Release();
		return err;
	} else {
		if (command == tpc_cmdStop){
			 // Der Stopp wird an alle Devices geschickt, um eventuelle Fehler zur�ckzusetzten.
			vector<Device*>::iterator i;
			for (i = m_devices.begin(); i != m_devices.end(); ++i) {
				Device* d = *i;
				d->ExecuteSystemCommand(tpc_cmdStop);
			}
			return tpc_noError;
		} else if(SyncLinkDetectedonAll == true || m_devices.size() == 1) {
		  // Send the command to the primary device.
		  TPC_ErrorCode err = primary->ExecuteSystemCommand(command);
		  return err;
		} else{
			vector<Device*>::iterator i;
			for (i = m_devices.begin(); i != m_devices.end(); ++i) {
				Device* d = *i;
				d->ExecuteSystemCommand(command);
			}
			return tpc_noError;
		}
	}
}


TPC_ErrorCode System::MakeMeasurement(int timeout, int *measurementNumber)
{
	Device* d = *m_devices.begin();
	TPC_DeviceStatus status;
	int time = 0;
	const int waittime = 50;

	// Get old measurement number
	TPC_ErrorCode err = d->GetDeviceStatus(&status, sizeof(status));
	if (err != 0) return err;
	int measurement_nr = status.measurementNumber;

	// Start measurement
	err = ExecuteSystemCommand(tpc_cmdStart);
	if (err != 0) return err;

	do {
		err = d->GetDeviceStatus(&status, sizeof(status));
		if (err != 0) return err;
		
		// Check for timeout
		NonWaitableThread::sleep(waittime);
		time += waittime;
		if (time > timeout) {
			ExecuteSystemCommand(tpc_cmdStop);
			return tpc_errTimeout;			
		}
	} while((measurement_nr == status.measurementNumber) || (status.boards[0].recordingState == tpc_recStarting) ||
					(status.boards[0].recordingState == tpc_recRecording));

	// Measurement no finished as expected?
	if (status.boards[0].recordingState == tpc_recAborted || status.boards[0].recordingState == tpc_recStartError)
			return tpc_errMeasurementAborted;

	// Update measurement number
	err = d->GetDeviceStatus(&status, sizeof(status));
	if (err != 0) return err;
	if (measurementNumber != NULL) *measurementNumber = status.measurementNumber; 

	return tpc_noError;
}


TPC_ErrorCode System::StartCalibration(int deviceIx, TPC_CalibrationType command)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	TPC_ErrorCode err = d->StartCalibration(command);
	return err;
}


//---  Device status ------------------------------------------------------------

TPC_ErrorCode System::GetDeviceStatus(int deviceIx, TPC_DeviceStatus* status, int structSize)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetDeviceStatus(status, structSize);
}


TPC_ErrorCode System::SetStatusCallback(TPC_StatusCallbackFunc callback, void* userData)
{
	m_callbackMutex.lock();
	m_callback = callback;
	m_callbackUserData = userData;
    m_callbackMutex.unlock();
	return tpc_noError;
}


void System::ExecuteCallback(int deviceIx, TPC_DeviceStatus* status)
{
	if (m_callback != NULL) {
		m_callbackMutex.lock();
		m_callback(m_callbackUserData, deviceIx, status);
		m_callbackMutex.unlock();
	}
}


//--- Data readout --------------------------------------------------------------

TPC_ErrorCode System::GetYMetaData(int deviceIx, int boardAddress, int inputNumber, 
					int measurementNumber,
					TPC_YMetaData* metaData, int structSize)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetYMetaData(boardAddress, inputNumber, measurementNumber, metaData, structSize);
}


TPC_ErrorCode System::GetMetaDataParameter(int deviceIx, int boardAddress, int inputNumber, 
								int measurementNumber, TPC_Parameter parameter, double* value)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetMetaDataParameter(boardAddress, inputNumber, measurementNumber, parameter, value);
}


TPC_ErrorCode System::GetMetaDataAttribute(int deviceIx, int boardAddress, int inputNumber, 
							int measurementNumber, const char* key, string& value)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetMetaDataAttribute(boardAddress, inputNumber, measurementNumber, key, value);
}


TPC_ErrorCode System::GetAllMetaDataAttributes(int deviceIx, int measurementNumber,
							 TPC_AttributeEnumeratorCallback callback, void* userData)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetAllMetaDataAttributes(measurementNumber, callback, userData);
}


TPC_ErrorCode System::GetMetaDataAssociatedChannels(int deviceIx, int boardAddress, int inputNumber, 
									  int measurementNumber, TPC_AssociatedChannel* list, int* count)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetMetaDataAssociatedChannels(boardAddress, inputNumber, measurementNumber, list, count);
}



TPC_ErrorCode System::GetTMetaData(int deviceIx, int boardAddress, int blockNumber, 
					int measurementNumber,
					TPC_TMetaData* metaData, int structSize)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetTMetaData(boardAddress, blockNumber, measurementNumber, metaData, structSize);
}

TPC_ErrorCode System::GetTMetaData(int deviceIx, int boardAddress, int blockNumberFrom, 
					int blockNumberTo,int measurementNumber,
					TPC_TMetaData * metaData, int structSize)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetTMetaData(boardAddress, blockNumberFrom, blockNumberTo, measurementNumber,
		metaData, structSize);
}




TPC_ErrorCode System::GetData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
				int measurementNumber, uint64_t dataStart, uint64_t dataLength, 
				int resultLength, double* data, Device::ReadoutType type)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetData(boardAddress, inputNumber, blockNumber, 
			measurementNumber, dataStart, dataLength, 
			resultLength, data, type);
}



TPC_ErrorCode System::GetRawData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
				int measurementNumber, uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, int32_t* data, Device::ReadoutType type)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetRawData(boardAddress, inputNumber, blockNumber, 
			measurementNumber, dataStart, dataLength, 
			resultLength, data, type);
}



TPC_ErrorCode System::DeferredGetData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, double* data, TPC_ErrorCode* error, Device::ReadoutType type)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->DeferredGetData(boardAddress, inputNumber, blockNumber, 
			dataStart, dataLength, resultLength, data, error, type);
}



TPC_ErrorCode System::DeferredGetRawData(int deviceIx, int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, int32_t* data, TPC_ErrorCode* error, Device::ReadoutType type)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->DeferredGetRawData(boardAddress, inputNumber, blockNumber, 
			dataStart, dataLength, resultLength, data, error, type);
}



TPC_ErrorCode System::ProcessDeferredDataRequests(int measurementNumber)
{
	TPC_ErrorCode err = tpc_noError;
	for (vector<Device*>::iterator i = m_devices.begin(); i != m_devices.end(); ++i) {
		TPC_ErrorCode e = (*i)->ProcessDeferredDataRequests(measurementNumber);
		if (err == 0) err = e;
	}
	return err;
}



TPC_ErrorCode System::CancelDeferredDataRequests()
{
	for (vector<Device*>::iterator i = m_devices.begin(); i != m_devices.end(); ++i) {
		(*i)->CancelDeferredDataRequests();
	}
	return tpc_noError;
}


//---  -----------------------------------------------------------

TPC_ErrorCode System::SendServerCustomCommand(int deviceIx, char *command, char *result, int length)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->SendServerCustomCommand(command, result, length);
}

/**********************************************************************************/
/* AUTO SEQUENCE FUNCTIONS
/**********************************************************************************/

/**********************************************************************************/
TPC_ErrorCode System::LoadAutosequence(int deviceIx, char * cData, int iLength){
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->LoadAutosequence(cData, iLength);
}
/**********************************************************************************/
TPC_ErrorCode System::GetAutoSequence(int deviceIx, char * cData, int iLength){
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetAutoSequence(cData, iLength);
}

/**********************************************************************************/
TPC_ErrorCode System::StartAutoSequence(int deviceIx){
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->StartAutoSequence();
}

/**********************************************************************************/
TPC_ErrorCode System::StopAutoSequence(int deviceIx){
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->StopAutoSequence();
}	

//--- Elsys functions -----------------------------------------------------------

TPC_ErrorCode System::WriteDev(int deviceIx, int boardAddress, int type, unsigned count, void* data, unsigned aux1, unsigned aux2)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->WriteDev(boardAddress, type, count, data, aux1, aux2);
}



TPC_ErrorCode System::ReadDev(int deviceIx, int boardAddress, int type, unsigned count, void* data, unsigned aux1, unsigned aux2)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->ReadDev(boardAddress, type, count, data, aux1, aux2);
}


TPC_ErrorCode System::ReadWriteTwi(int deviceIx, int boardAddress, int amplifier, int count, char *data)
{
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
  return d->ReadWriteTwi(boardAddress, amplifier, count, data);
}


TPC_ErrorCode System::GetGPSLogList( int deviceIx, int from, int to, char *log){
	Device* d = FindDevice(deviceIx);
	if (d == NULL) return tpc_errInvalidDeviceIx;
	return d->GetGPSLogList(from, to, log);
}
//-------------------------------------------------------------------------------

