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
  $Id: Device.cpp 37 2024-01-29 15:58:48Z roman $
  This class holds all information and state for one device.
--------------------------------------------------------------------------------*/
#include "System.h"
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>        // std::abs
using namespace std;


#include "Device.h"


//-------------------------------------------------------------------------------


class GetAllAttributesEnumerator : public Attributes::AttributeEnumerator
{
public:
  GetAllAttributesEnumerator(TPC_AttributeEnumeratorCallback callback, void* userData)
  : m_callback(callback), m_userData(userData)
  {}

  virtual void SetTotal(int count) {};
  virtual bool Callback(int index, int boardAddress, int inputNumber, const string& name, const string& value)
  {
    return m_callback(m_userData, boardAddress, inputNumber, name.c_str(), value.c_str());
  }
private:
  TPC_AttributeEnumeratorCallback m_callback;
  void* m_userData;
};


enum TPC_OperationModesOld
{
  tpc_opModeBasicModeMask = 0x0F,
  tpc_opModeEventRecorder = 3,
  tpc_opModeDual = 0x040,
  tpc_opModeMultiChannel = 0x080,
};


//--- Status observation thread -------------------------------------------------


class StatusObserverThread : public WaitableThread
{
public:
  StatusObserverThread(Device* device, int recvTimeOut, int sendTimeOut);
  
  virtual void run();

  void PrepareToQuit(SoapDevice* soap) 
  { 
    m_shouldTerminate = true; 
    if (m_inWaitForStatusChange) {
      //###pg: no kill            soap->LeaveWaitStatus();
    }
    //cout << "Calling KillSocket..." << endl;
    //###pg: no kill        m_soap->KillSocket();
    //cout << "Called KillSocket" << endl;
  }

  void SetQuitFlag() 
  { 
    m_shouldTerminate = true; 
  }

private:
  Device* m_device; 
  SoapDevice* m_soap;
  volatile bool m_shouldTerminate;
  ns__Status m_status;
  bool m_inWaitForStatusChange;

  TPC_ErrorCode XErr(TPC_ErrorCode err);
  TPC_ErrorCode ReloadParams();
  TPC_ErrorCode ReloadPreviousParams();
  void ResetStatusNewMeasurement();
};



StatusObserverThread::StatusObserverThread(Device* device,int recvTimeOut, int sendTimeOut)
: WaitableThread()
{
  m_device = device;
  m_soap = new SoapDevice(device->Url(),recvTimeOut,sendTimeOut);
  m_shouldTerminate = false;
  m_inWaitForStatusChange = false;
}


TPC_ErrorCode StatusObserverThread::XErr(TPC_ErrorCode err)
{
  if (err == tpc_errTransmissionError) return tpc_errNoConnection;
  
  if (err == tpc_errCalibrating
      || err == tpc_errServerClientVersionConflict
      || err == tpc_errDeviceInitializationFailure
      || err == tpc_errHardwareFailure
      || err == tpc_errInternalError) 
  return err;

  if (err != tpc_noError) return tpc_errInternalError;
  
  return tpc_noError;
}


TPC_ErrorCode StatusObserverThread::ReloadParams()
{	
  TPC_ErrorCode err;
  // get Status Muxtex. Dies Function must be done in one step!
  cMutex* pstatusMutex;
  pstatusMutex = m_device->getSyncObject();
  pstatusMutex->lock();

  // Fetch parameters 
  ParameterSet params;
  err = m_soap->GetAllCurrentParameters(params);
  if (m_shouldTerminate) return err;

  err = XErr(err);
  if (err != tpc_noError) {
    m_device->SetDeviceStatusError(err);
    return err;
  }

  // Fetch attributes
  Attributes attr;
  err = m_soap->GetAllCurrentAttributes(attr);
  if (m_shouldTerminate) return err;

  err = XErr(err);
  if (err != tpc_noError) {
    m_device->SetDeviceStatusError(err);
    return err;
  }

  // Fetch associations
  AssociationRowList assoc;
  err = m_soap->GetCurrentAssociations(assoc);
  if (m_shouldTerminate) return err;

  err = XErr(err);
  if (err != tpc_noError) {
    m_device->SetDeviceStatusError(err);
    return err;
  }

  // Success, store them
  m_device->SetParamAttr(params, attr, assoc);
  pstatusMutex->unlock();
  return tpc_noError;
}


void StatusObserverThread::ResetStatusNewMeasurement()
{
  m_status.status.startInProgress = true;
  for (int b=0; b<tpc_maxBoards; b++) {
    m_status.status.boards[b].recordingState = tpc_recStarting;  
    m_status.status.boards[b].triggerState = tpc_recDisarmed;  
    m_status.status.boards[b].blockCounter = 0;  
    m_status.status.boards[b].dataCounter = 0;  
    m_status.status.boards[b].dataLostCounter = 0;  
    m_status.status.boards[b].blockLostCounter = 0;  
  }
}


TPC_ErrorCode StatusObserverThread::ReloadPreviousParams()
{	
  TPC_ErrorCode err;

  if (m_status.status.measurementNumber == 0) return tpc_errNoData;

  // Reload previous parameters
  ParameterSet params;
  err = m_soap->GetAllPreviousParameters(m_status.status.measurementNumber, params);
  if (m_shouldTerminate) return err;

  if (err == tpc_errNewMeasurement) {
    // Special treatment. 
    ResetStatusNewMeasurement();
    return err;
  }
  else if (err == tpc_errNoData) {
    return err;
  }
  else {
    err = XErr(err);
    if (err != tpc_noError) {
      m_device->SetDeviceStatusError(err);
      return err;
    }
  }


  // Reload previous attributes
  Attributes attr;
  err = m_soap->GetAllPreviousAttributes(m_status.status.measurementNumber, attr);
  if (m_shouldTerminate) return err;

  if (err == tpc_errNewMeasurement) {
    // Special treatment. 
    ResetStatusNewMeasurement();
    return err;
  }
  else if (err == tpc_errNoData) {
    return err;
  }
  else {
    err = XErr(err);
    if (err != tpc_noError) {
      m_device->SetDeviceStatusError(err);
      return err;
    }
  }


  // Reload y-metadata
  YMetaDataList metadata;
  err = m_soap->GetAllYTMetadata(m_status.status.measurementNumber, metadata);
  if (m_shouldTerminate) return err;

  if (err == tpc_errNewMeasurement) {
    // Special treatment. 
    ResetStatusNewMeasurement();
    return err;
  }
  else if (err == tpc_errNoData) {
    return err;
  }
  else {
    err = XErr(err);
    if (err != tpc_noError) {
      m_device->SetDeviceStatusError(err);
      return err;
    }
  }


  // Reload previous associations
  AssociationRowList assoc;
  err = m_soap->GetAllPreviousAssociations(m_status.status.measurementNumber, assoc);
  if (m_shouldTerminate) return err;

  if (err == tpc_errNewMeasurement) {
    // Special treatment. 
    ResetStatusNewMeasurement();
    return err;
  }
  else if (err == tpc_errNoData) {
    return err;
  }
  else {
    err = XErr(err);
    if (err != tpc_noError) {
      m_device->SetDeviceStatusError(err);
      return err;
    }
  }


  m_device->SetPreviousParamAttr(params, attr, metadata, assoc);
  return tpc_noError;
}


void StatusObserverThread::run()
{
  const int requiredVersion = 1;

  int lastSettingsCounter = 0;
  int lastMeasurementNumber = 0;
  bool connected = false;
  bool settingsChanged = false;
  int lastBlkCounter[tpc_maxBoards];

  while (!m_shouldTerminate) {
    //=== Disconnected state ===
    while (!m_shouldTerminate && !connected) {
      //--- Try to connect and retrieve the interface version.
      int version;
      TPC_ErrorCode err = m_soap->GetInterfaceVersion(version);
      if (m_shouldTerminate) break;

      err = XErr(err);
      if (err != tpc_noError) {
        m_device->SetDeviceStatusError(err);
      }
      else {

        if (version < requiredVersion) 
        m_device->SetDeviceStatusError(tpc_errServerClientVersionConflict);
        else {

          //--- Try to connect and retrieve device status.

          err = m_soap->GetStatus(m_status);
          if (m_shouldTerminate) break;
          
          err = XErr(err);
          if (err != tpc_noError) {
            m_device->SetDeviceStatusError(err);
          }
          else {
            if (m_status.status.deviceError != 0) {
              m_device->SetDeviceStatusError((TPC_ErrorCode)m_status.status.deviceError);
            }
            else {
              //--- Status retrieval successful. Try loading hardware info, parameters and attribtues.

              err = m_soap->GetHardwareInfo(m_device->m_deviceInfo);
              if (m_shouldTerminate) break;
              
              err = XErr(err);
              if (err != tpc_noError) {
                m_device->SetDeviceStatusError(err);
              }
              else {
                // Fetch current attr + parameters
                err = ReloadParams();
                if (m_shouldTerminate) break;
                if (err == tpc_noError) {
                  // Try to fetch previous attr + parameters
                  err = ReloadPreviousParams();
                  if (m_shouldTerminate) break;
                  
                  if (err == tpc_noError) {
                    lastMeasurementNumber = m_status.status.measurementNumber;
                    connected = true;
                    break;
                  }
                  else if (err == tpc_errNewMeasurement || err == tpc_errNoData) {
                    lastMeasurementNumber = m_status.status.measurementNumber-1;
                    connected = true;
                    break;
                  }

                  // We got all we need for initialization
                  m_device->SetStatus(m_status);
                  lastSettingsCounter = m_status.status.settingsChangesCounter;

                  if (connected) {
                    m_device->CallStatusCallback();
                  }
                }
                for (int i=0; i<tpc_maxBoards; i++) 
                lastBlkCounter[i] = m_status.status.boards[i].blockCounter;
              }
            }
          }
        }
      }
      
      if (!connected) {
        // Retry after a while
        for (int i=0; i<30; i++) {
          NonWaitableThread::sleep(100);
          if (m_shouldTerminate) break;
        }
      }
    }

    //=== Connected state ===
    while (!m_shouldTerminate && connected) {
      m_inWaitForStatusChange = true;
      if (m_shouldTerminate) break;
      TPC_ErrorCode err = m_soap->WaitForStatusChange(m_status);
      m_inWaitForStatusChange = false;
      if (m_shouldTerminate) break;
      
      err = XErr(err);
      if (err != tpc_noError) {
        m_device->SetDeviceStatusError(err);
        connected = false;
        break;
      }
      else {
        if (m_status.status.deviceError != 0) {
          m_device->SetDeviceStatusError((TPC_ErrorCode)m_status.status.deviceError);
          connected = false;
          break;
        }
        else {

          //--- Status retrieval successful.
          // Check if we need to reload the parameters and attributes.
          if (lastSettingsCounter != m_status.status.settingsChangesCounter) {
            
            // Reload parameters and attributes
            err = ReloadParams();
            if (m_shouldTerminate) break;
            if (err != tpc_noError) {
              connected = false;
              break;
            }
            
            lastSettingsCounter = m_status.status.settingsChangesCounter;
            settingsChanged = true;
          }

          // Check if scope mode needs an update
          bool reload = false;
          if (settingsChanged)
          {
            for (int i=0; i<tpc_maxBoards; i++) {
              if (m_status.status.boards[i].blockCounter != lastBlkCounter[i]) {
                // Check if this board is in scope mode
                double d;
                if (m_device->m_previousParameters.GetParameter(i, 0, tpc_parOperationMode, &d)) {
                  int om = (int)d;
                  if ((om & tpc_opModeBasicModeMask) == tpc_omScope) {
                    settingsChanged = false;
                    reload = true;
                    break;
                  }
                }
              }
            }
          }

          // Check the measurement number
          if (m_status.status.measurementNumber != lastMeasurementNumber || reload) {
            err = ReloadPreviousParams();
            if (m_shouldTerminate) break;

            if (err == tpc_noError) 
            lastMeasurementNumber = m_status.status.measurementNumber;
            else if (err != tpc_errNewMeasurement && err != tpc_errNoData) {
              connected = false;
            }

            for (int i=0; i<tpc_maxBoards; i++) lastBlkCounter[i] = m_status.status.boards[i].blockCounter;
          }
          
          if (connected) {
            m_device->SetStatus(m_status);
            if (!m_shouldTerminate) m_device->CallStatusCallback();
          }
        }
      }
    }
    if (!m_shouldTerminate) { m_device->CallStatusCallback(); } //###pg: no kill
  }
  delete m_soap;
}



//-------------------------------------------------------------------------------



Device::Device(const char* url, int index, System* system,int recvTimeOut,int sendTimeOut)
: m_statusMutex(true)
{
  // Initialize variables
  m_url = url;
  m_index = index;
  m_system = system;
  m_soap = new SoapDevice(url,recvTimeOut,sendTimeOut);

  m_deviceStatusError = tpc_errNoConnection;

  m_deferredSets = 0;

  m_settingsChangesCounter = 0;
  m_settingsChanged = false;

  // Start observation thread.
   m_statusThread = new StatusObserverThread(this,recvTimeOut,sendTimeOut);
  m_statusThread->start();
}


Device::~Device()
{
  delete m_soap;
  m_statusThread->SetQuitFlag();
  // The m_statusThread object will not be freed.
}


//--- Device info ---------------------------------------------------------------


void CopyStruct(void* dest, int destSize, const void* source, int sourceSize)
{
  int m = sourceSize;
  if (m > destSize) m = destSize;
  memcpy(dest, source, m);

  int d = destSize-m;
  if (d > 0) {
    memset(((char*)dest)+m, 0, d);
  }
}


TPC_ErrorCode Device::GetDeviceInfo(TPC_DeviceInfo* deviceInfo, int structSize)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  TPC_DeviceInfo devInfo = m_deviceInfo.GetDeviceInfo();
  CopyStruct(deviceInfo, structSize, &devInfo, sizeof(devInfo));

  return tpc_noError;
}

TPC_ErrorCode Device::SetDeviceSettings(TPC_DeviceInfo deviceInfo){
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;
  if (m_deviceInfo.GetDeviceInfo().serverSoftwareVersion > 10101){
	  return m_soap->SetHardwareSettings(deviceInfo);
  }
  return tpc_errNotImplemented;
}

TPC_ErrorCode Device::GetBoardInfo(int boardAddress, TPC_BoardInfo* boardInfo, int structSize)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckBoard(boardAddress);
  if (err != 0) return err;

  TPC_BoardInfo brdInfo = m_deviceInfo.GetBoardInfo(boardAddress);
  CopyStruct(boardInfo, structSize, &brdInfo, sizeof(brdInfo));

  return err;
}



TPC_ErrorCode Device::GetInputInfo(int boardAddress, int inputNumber, TPC_InputInfo* inputInfo, int structSize)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckInput(boardAddress, inputNumber);
  if (err != 0) return err;

  TPC_InputInfo inpInfo = m_deviceInfo.GetInputInfo(boardAddress, inputNumber);
  CopyStruct(inputInfo, structSize, &inpInfo, sizeof(inpInfo));

  return err;
}

TPC_ErrorCode Device::GetFreeDiskSpace(uint64_t* freeDiskSpace, uint64_t* DiskSize){
	TPC_ErrorCode err = CheckStatus();
	if (err != 0) return err;
	if (m_deviceInfo.GetDeviceInfo().serverSoftwareVersion > 10101){
		return m_soap->GetFreeDiskSpace(freeDiskSpace, DiskSize);
	}
	*freeDiskSpace = 0;
	return tpc_errNotImplemented;
}

//--- Synchronisation ----------------------------------------------------------

TPC_ErrorCode Device::AcquireDeviceLock()
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  return m_soap->AcquireDeviceLock();
}


TPC_ErrorCode Device::ReleaseDeviceLock()
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  return m_soap->ReleaseDeviceLock();
}


//--- Configuration reset -------------------------------------------------------

TPC_ErrorCode Device::ResetConfiguration()
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = m_soap->ResetConfiguration();
  return err;
}


//--- Cluster configuration -----------------------------------------------------


TPC_ErrorCode Device::GetLastClusterNumbers(int clusterNumbers[tpc_maxBoards])
{
  m_statusMutex.lock();
  memcpy(clusterNumbers, m_currentStatus.clusters.clusterNumbers, sizeof(int) * tpc_maxBoards);
  m_statusMutex.unlock();
  return tpc_noError;
}


TPC_ErrorCode Device::GetClusterNumbers(int* clusterNumbers)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  return GetLastClusterNumbers(clusterNumbers);

  // Ist zu langsam, da zu oft aufgerufen.
  // err = m_soap->GetClusterConfiguration(clusterNumbers);
  // return err;
}



TPC_ErrorCode Device::SetClusterNumbers(int* clusterNumbers)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = m_soap->SetClusterConfiguration(clusterNumbers);
  if (err != 0) return err;

  // TODO: TpcAccess benötigt m_currentStatus.clusters.clusterNumbers für die 
  // Umsetzung der Boardadressen. Wenn die ClusterNummern nicht zurückgelesen 
  // würden, könnte es passieren, dass TpcAccess die Boardadressen falsch 
  // umsetzt (passiert z.B beim laden vom tps File). Das Problem ist aber, 
  // dass genau zu diesem Zeitpunkt auch der Thread dabei sein könnte die 
  // clusterNumbers zurückzulesen, welche aber veraltet sind. Dieses Problem 
  // tritt nicht nur bei den clusterNumbers zu sondern auch bei den Parametern 
  // und Attributen was aber nicht so tragisch ist.
  // Also warten wir bis der Thread sicher fertig ist. Besser währe es wenn 
  // wir mit dem Thread synchronisieren könnten.
  WaitableThread::sleep(200);

  return tpc_noError;
}



TPC_ErrorCode Device::BoardAddressToGroupNumber(int boardAddress, int &groupNumber)
{
  groupNumber = -1;
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckBoard(boardAddress);
  if (err != 0) return err;

  m_statusMutex.lock();
  groupNumber = m_currentStatus.clusters.clusterNumbers[boardAddress];
  m_statusMutex.unlock();

  if (groupNumber == -1)
    return tpc_errInvalidBoardAddress;
  return tpc_noError;
}


TPC_ErrorCode Device::GroupNumberToBoardAddress(int groupNumber, int &boardAddress)
{
  boardAddress = -1;
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  m_statusMutex.lock();
  ns__ClusterConfiguration clusters = m_currentStatus.clusters;
  m_statusMutex.unlock();

  for (int index = 0; index < tpc_maxBoards; index++)
  {
    if (clusters.clusterNumbers[index] == groupNumber)
    {
      boardAddress = index;
      return tpc_noError;
    }
  }
  return tpc_errInvalidBoardAddress;
}


TPC_ErrorCode Device::GetAllClusterNumbers(vector<int> &clusterNumbers)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  m_statusMutex.lock();
  ns__ClusterConfiguration clusters = m_currentStatus.clusters;
  m_statusMutex.unlock();

  for (int index = 0; index < tpc_maxBoards; index++)
  {
    int number = clusters.clusterNumbers[index];
    if (number != -1)
    {
      bool contains = find(clusterNumbers.begin(), clusterNumbers.end(), number) != clusterNumbers.end();
      if (!contains)
        clusterNumbers.push_back(number);
    }
  }
  return tpc_noError;
}


TPC_ErrorCode Device::GetBoardRestrictions(int boardAddress, BoardRestrictions* br)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckBoard(boardAddress);
  if (err != 0) return err;

  *br = m_deviceInfo.GetBoardRestrictions(boardAddress);

  return err;
}


//--- Parameters and Attributes -------------------------------------------------


TPC_ErrorCode Device::SetParameter(int boardAddress, int inputNumber, TPC_Parameter parameter, double value)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckInput(boardAddress, inputNumber);
  if (err != 0) return err;

  if (m_deferredSets) {
    m_parameterModifications.Add(boardAddress, inputNumber, parameter, value);
    return tpc_noError;
  }
  else {
    // Send to device
    double roundedValue;
	int SetParamError;
    err = m_soap->SetOneParameter(boardAddress, inputNumber, parameter, value, &roundedValue, &SetParamError);
	
	if(err == 0){
		err = (TPC_ErrorCode)SetParamError;
	}
    // Update local copy
    if (err == 0) {
      double previousValue;
      m_statusMutex.lock();

      m_currentParameters.GetParameter(boardAddress, inputNumber, parameter, &previousValue);
      bool chgd = (previousValue != roundedValue);
      if (chgd) {
        m_currentParameters.SetParameter(boardAddress, inputNumber, parameter, roundedValue);

        if (parameter == tpc_parBlkBlockLength || parameter == tpc_parScopeBlockLength) {
            m_currentParameters.SetParameter(boardAddress, inputNumber, tpc_parBlkBlockLength, roundedValue);
            m_currentParameters.SetParameter(boardAddress, inputNumber, tpc_parScopeBlockLength, roundedValue);
        }
        if (parameter == tpc_parBlkTriggerDelay || parameter == tpc_parScopeTriggerDelay) {
            m_currentParameters.SetParameter(boardAddress, inputNumber, tpc_parBlkTriggerDelay, roundedValue);
            m_currentParameters.SetParameter(boardAddress, inputNumber, tpc_parScopeTriggerDelay, roundedValue);
        }
      }
      m_statusMutex.unlock();
      if (chgd) {
        SettingsChanged();
      }
    }
  }
  return err;
}


TPC_ErrorCode Device::SetTimebaseParameterToGroup(int groupNumber, TPC_Parameter parameter, double value)
{
  int boardAddress;
  TPC_ErrorCode err = GroupNumberToBoardAddress(groupNumber, boardAddress);
  if (err != 0) return err;

  return SetParameter(boardAddress, 0, parameter, value);
}


TPC_ErrorCode Device::GetParameter(int boardAddress, int inputNumber, TPC_Parameter parameter, double* value)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckInput(boardAddress, inputNumber);
  if (err != 0) return err;

  // Get from local copy
  m_statusMutex.lock();
  bool ok = m_currentParameters.GetParameter(boardAddress, inputNumber, parameter, value);
  m_statusMutex.unlock();
  if (!ok)
  return tpc_errInvalidParameter;

  return tpc_noError;
}


TPC_ErrorCode Device::GetTimebaseParameterFromGroup(int groupNumber, TPC_Parameter parameter, double* value)
{
  int boardAddress;
  TPC_ErrorCode err = GroupNumberToBoardAddress(groupNumber, boardAddress);
  if (err != 0) return err;

  return GetParameter(boardAddress, 0, parameter, value);
}


TPC_ErrorCode Device::SetAttribute(int boardAddress, int inputNumber, const char* key, const char* value)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  if (m_deferredSets) {
    m_attributeModifications.SetAttributeE(boardAddress, inputNumber, key, value);
    return tpc_noError;
  }
  else {
    // Send to device
    Attributes a;
    a.SetAttributeE(boardAddress, inputNumber, key, value);
    err = m_soap->SetAttributes(a);

    // Update local copy
    if (err == 0) {
      m_statusMutex.lock();
      string previousValue = m_currentAttributes.GetAttribute(boardAddress, inputNumber, key);
      bool chgd = (previousValue != value);
      if (chgd) {
        m_currentAttributes.SetAttribute(boardAddress, inputNumber, key, value);
      }
      m_statusMutex.unlock();
      if (chgd) {
        SettingsChanged();
      }
    }
  }
  return err;
}



TPC_ErrorCode Device::GetAttribute(int boardAddress, int inputNumber, const char* key, string& value)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  // Get from local copy
  m_statusMutex.lock();
  value = m_currentAttributes.GetAttribute(boardAddress, inputNumber, key);
  m_statusMutex.unlock();
  return tpc_noError;
}


TPC_ErrorCode Device::GetAllAttributes(TPC_AttributeEnumeratorCallback callback, void* userData)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  // Get from local copy
  GetAllAttributesEnumerator e(callback, userData);
  m_statusMutex.lock();
  m_currentAttributes.EnumerateAttributes(&e);
  m_statusMutex.unlock();
  return tpc_noError;
}


TPC_ErrorCode Device::GetAssociatedChannels(int boardAddress, int inputNumber, 
TPC_AssociatedChannel* list, int* count)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckInput(boardAddress, inputNumber);
  if (err != 0) return err;

  // Get from local copy
  m_statusMutex.lock();
  err = m_currentAssociations.GetAssociatedChannels(boardAddress, inputNumber, list, count);
  m_statusMutex.unlock();
  return err;
}


TPC_ErrorCode Device::GetMetaDataAssociatedChannels(int boardAddress, int inputNumber, 
int measurementNumber, TPC_AssociatedChannel* list, int* count)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckMeasurementNumber(measurementNumber);
  if (err != 0) return err;

  err = CheckInput(boardAddress, inputNumber);
  if (err != 0) return err;

  // Get from local copy
  m_statusMutex.lock();
  err = m_previousAssociations.GetAssociatedChannels(boardAddress, inputNumber, list, count);
  m_statusMutex.unlock();
  return err;
}


TPC_ErrorCode Device::SetAssociatedChannels(int boardAddress, int inputNumber, 
TPC_AssociatedChannel* list, int count)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckInput(boardAddress, inputNumber);
  if (err != 0) return err;

  m_associationModifications.SetAssociatedChannels(boardAddress, inputNumber, list, count);

  if (!m_deferredSets) {
    // Send to device
    TPC_ErrorCode erra = m_soap->SetAssociations(m_associationModifications);
    if (err == 0) err = erra;

    // Update local copy
    FetchCurrentAssociations(m_soap);

    // Clear modification list.
    m_associationModifications.Clear();
  }
  return err;
}


TPC_ErrorCode Device::SetTrigger(int boardAddress, int inputNumber, TPC_EasyTriggerMode mode, TPC_EasyTriggerComperatorMode comp, TPC_EasyTriggerFlags flags, double level, double hysteresis, int time, int time2)
{
	TPC_ErrorCode err = CheckStatus();
	if (err != 0) return err;

	err = CheckInput(boardAddress, inputNumber);
	if (err != 0) return err;

	// Trigger mode is not set directly, because InputMulitplier flag has to be set together with mode
	int trgMode;	

	// Check Comperator Mode parameter and convert to int
	int compMode;
	if (comp == tpc_etrgCompNegative || comp == tpc_etrgCompPositive || comp == tpc_etrgCompBoth) {
		if (comp == tpc_etrgCompBoth) {
			switch(mode) {
				case tpc_etrgWindow:
				case tpc_etrgEqualPeriod:
				case tpc_etrgEqualPulse:
				case tpc_etrgNotEqualPeriod:
				case tpc_etrgNotEqualPulse:
				case tpc_etrgLongPeriod:
				case tpc_etrgLongPulse:
				case tpc_etrgShortPeriod:
				case tpc_etrgShortPulse:
					return tpc_errInvalidParameterValue;
					break;
				default:
					// ok
					break;
			}
		}
		compMode = comp;
	} else {
		return tpc_errInvalidParameterValue;
	}

	// Recalculate Hysteresis and Trigger
	bool update = true;
	if ((flags & tpc_etrgIgnoreLevelAndTime)!=0x00) {
		update = false;
		if (!m_triggerSettings.GetParameter(boardAddress, inputNumber, 0, &level)) {
			err = GetParameter(boardAddress, inputNumber, tpc_parTrgLevelA, &level);
			if (err != 0) return err;	
		}
		if (!m_triggerSettings.GetParameter(boardAddress, inputNumber, (mode == tpc_etrgWindow)?2:1, &hysteresis)) {
			hysteresis = 0.1;
		}
	} else {
		double inputRange;
		err = GetParameter(boardAddress, inputNumber, tpc_parRange, &inputRange);
		if (err != 0) return err;
		level /= inputRange * 0.01;
		hysteresis = hysteresis / (inputRange * 0.01);
	}

	if (mode != tpc_etrgSlewRate) {
		m_triggerSettings.SetParameter(boardAddress, inputNumber, 0, level);
		m_triggerSettings.SetParameter(boardAddress, inputNumber, (mode == tpc_etrgWindow)?2:1, hysteresis);
	}

	if (mode != tpc_etrgSlewRate && mode != tpc_etrgState && mode != tpc_etrgWindow) {
		hysteresis = abs(hysteresis);
		if (comp == tpc_etrgCompBoth) {
			level -= 0.5 * hysteresis;
		} else if (comp == tpc_etrgCompPositive) {
			level -= hysteresis;
		}
		hysteresis = level + hysteresis;
	}

	// Set all Parameters together at the end
	err = TPC_BeginSet();
	if (err != 0) return err;

	// Update AND-Mask
	double andMaskConv;
	int andMask;
	err = GetParameter(boardAddress, inputNumber, tpc_parANDTriggerMask, &andMaskConv);
	if (err != 0) {
		TPC_CancelSet();
		return err;
	}
	andMask = (int)andMaskConv;

	int tempMask = 0x01000000 << inputNumber;
	for(int flagMask = 0x08; flagMask != 0x00; flagMask >>= 1) {
		if ((flags & flagMask) != 0x00) {
			andMask |= tempMask;
		} else {
			andMask &= ~tempMask;
		}
		tempMask >>= 8;
	}
	andMaskConv = andMask;
	SetParameter(boardAddress, inputNumber, tpc_parANDTriggerMask, andMaskConv);

	// Update Mode
	switch(mode) {
		case tpc_etrgOff:
			double trgModeConv;
			err = GetParameter(boardAddress, inputNumber, tpc_parTrgMode, &trgModeConv);
			if (err != 0) {
				TPC_CancelSet();
				return err;
			}
			trgMode = (int)trgModeConv | tpc_trgMasterDisable;
			break;

		case tpc_etrgWindow:
			compMode = (compMode == tpc_etrgCompPositive)?tpc_trgCompWindowIn:tpc_trgCompWindowOut;
		case tpc_etrgSlope:
			trgMode = 0x00;
			SetParameter(boardAddress, inputNumber, tpc_parTrgComparatorMode, compMode);
			SetParameter(boardAddress, inputNumber, tpc_parTrgLevelA, level);
			SetParameter(boardAddress, inputNumber, tpc_parTrgLevelB, hysteresis);
			break;

		case tpc_etrgState:
			trgMode = 0x00;
			compMode = (compMode == tpc_etrgCompPositive)?tpc_trgCompStateWindowIn:tpc_trgCompStateWindowOut;
			SetParameter(boardAddress, inputNumber, tpc_parTrgComparatorMode, compMode);
			SetParameter(boardAddress, inputNumber, tpc_parTrgLevelA, level);
			SetParameter(boardAddress, inputNumber, tpc_parTrgLevelB, 100.0);
			break;

		case tpc_etrgSlewRate:
			trgMode = tpc_trgSlewRate;
			SetParameter(boardAddress, inputNumber, tpc_parTrgSlewRateComparatorMode, compMode);
			if (update) {
				SetParameter(boardAddress, inputNumber, tpc_parTrgSlewRateDTSamples, time);
				SetParameter(boardAddress, inputNumber, tpc_parTrgSlewRateHysteresis, hysteresis);
			}
			break;

		case tpc_etrgEqualDelay:
		case tpc_etrgEqualPeriod:
		case tpc_etrgEqualPulse:
		case tpc_etrgNotEqualDelay:
		case tpc_etrgNotEqualPeriod:
		case tpc_etrgNotEqualPulse:
			if (update) SetParameter(boardAddress, inputNumber, tpc_parTrgPreTimerTSamples, time2);
		case tpc_etrgLongDelay:
		case tpc_etrgLongPeriod:
		case tpc_etrgLongPulse:
		case tpc_etrgShortDelay:
		case tpc_etrgShortPeriod:
		case tpc_etrgShortPulse:
			trgMode = mode;
			SetParameter(boardAddress, inputNumber, tpc_parTrgComparatorMode, compMode);
			SetParameter(boardAddress, inputNumber, tpc_parTrgLevelA, level);
			SetParameter(boardAddress, inputNumber, tpc_parTrgLevelB, hysteresis);
			if (update)	SetParameter(boardAddress, inputNumber, tpc_parTrgTimerTSamples, time);
			break;

		default:
			return tpc_errInvalidParameterValue;
			break;
	}

	// Check InputMultiplier und MasterDisable flag, and then write TriggerMode
	if ((flags & tpc_etrgInputMultiplier) != 0x00) {
		trgMode |= tpc_trgInputMultiplier;
	} else {
		trgMode &= ~tpc_trgInputMultiplier;
	}
	if ((flags & tpc_etrgSetMasterDisable)!=0x00) trgMode |= tpc_trgMasterDisable;
	SetParameter(boardAddress, inputNumber, tpc_parTrgMode, trgMode);

	// Update show mulitplied signal
	SetParameter(boardAddress, inputNumber, tpc_parShowTrgProduct, ((flags & tpc_etrgShowMultipliedSignal)!=0x00)?1.0:0.0);

	// Set all parameters
	err = TPC_EndSet();
	if (err != 0) return err;

	// All's well...
	return tpc_noError;
}


TPC_ErrorCode Device::GetTrigger(int boardAddress, int inputNumber, TPC_EasyTriggerMode *mode, TPC_EasyTriggerComperatorMode *comp, TPC_EasyTriggerFlags *flags, double *level, double *hysteresis, int *time, int *time2)
{
	TPC_ErrorCode err = CheckStatus();
	if (err != 0) return err;
	double tempPar;
	int tempMode;
	int tempFlags;
	int tempComp;

	err = GetParameter(boardAddress, inputNumber, tpc_parTrgMode, &tempPar);
	if (err != 0) return err;
	tempMode = (int)tempPar;

	err = GetParameter(boardAddress, inputNumber, tpc_parTrgComparatorMode, &tempPar);
	if (err != 0) return err;
	tempComp = (int)tempPar;

	// Get Input Multiplier Flags
	tempFlags = (tempMode & tpc_trgInputMultiplier) * tpc_etrgInputMultiplier;
	tempMode &= ~tpc_trgInputMultiplier;

	err = GetParameter(boardAddress, inputNumber, tpc_parShowTrgProduct, &tempPar);
	if (err != 0) return err;
	if (tempPar != 0) tempFlags |= tpc_etrgShowMultipliedSignal;

	// Do the main task;
	if (((tempMode & tpc_trgMasterDisable) != 0x00) || (tempComp == 0x00)) {	
		if ((*flags & tpc_etrgSetMasterDisable) != 0x00) {
			tempFlags |= tpc_etrgSetMasterDisable;
			tempMode &= ~tpc_trgMasterDisable;
		} else {
			tempMode = tpc_etrgOff;	// == tpc_trgMasterDisable
		}
	}

	if ((tempMode & tpc_trgMasterDisable) == 0x00) {
		err = GetParameter(boardAddress, inputNumber, tpc_parTrgLevelA, level);
		if (err != 0) return err;
		err = GetParameter(boardAddress, inputNumber, tpc_parTrgLevelB, hysteresis);
		if (err != 0) return err;

		switch(tempMode) {
			case tpc_trgBasic:
				switch(tempComp) {
					case tpc_trgCompPositiveSlope:
					case tpc_trgCompNegativeSlope:
					case tpc_trgCompBothSlopes:
						tempMode = tpc_etrgSlope;
						break;

					case tpc_trgCompWindowIn:
						tempMode = tpc_etrgWindow;
						tempComp = tpc_etrgCompPositive;
						break;
					case tpc_trgCompWindowOut:
						tempMode = tpc_etrgWindow;
						tempComp = tpc_etrgCompNegative;
						break;

					case tpc_trgCompStateWindowIn:
						tempMode = tpc_etrgState;
						tempComp = tpc_etrgCompPositive;
						*hysteresis = 0;
						break;
					case tpc_trgCompStateWindowOut:
						tempMode = tpc_etrgState;
						tempComp = tpc_etrgCompNegative;
						*hysteresis = 0;
						break;
				}
				break;

			case tpc_trgSlewRate:
				tempMode = tpc_etrgSlewRate;
				err = GetParameter(boardAddress, inputNumber, tpc_parTrgSlewRateComparatorMode, &tempPar);
				if (err != 0) return err;
				tempComp = (int)tempPar;
				err = GetParameter(boardAddress, inputNumber, tpc_parTrgSlewRateHysteresis, hysteresis);
				if (err != 0) return err;
				err = GetParameter(boardAddress, inputNumber, tpc_parTrgSlewRateDTSamples, &tempPar);
				if (err != 0) return err;
				*time = (int)tempPar;
				*level = 0;
				break;

			case tpc_trgEqualDelay:
			case tpc_trgEqualPeriod:
			case tpc_trgEqualPulse:
			case tpc_trgNotEqualDelay:
			case tpc_trgNotEqualPeriod:
			case tpc_trgNotEqualPulse:
				err = GetParameter(boardAddress, inputNumber, tpc_parTrgPreTimerTSamples, &tempPar);
				if (err != 0) return err;
				*time2 = (int)tempPar;
			case tpc_trgLongDelay:
			case tpc_trgLongPeriod:
			case tpc_trgLongPulse:
			case tpc_trgShortDelay:
			case tpc_trgShortPeriod:
			case tpc_trgShortPulse:
				err = GetParameter(boardAddress, inputNumber, tpc_parTrgTimerTSamples, &tempPar);
				if (err != 0) return err;
				*time = (int)tempPar;
				break;

			default:
				return tpc_errNotImplemented;
				break;
		}

		// Calculate correct level and hysteresis
		if (tempMode != tpc_etrgSlewRate && tempMode != tpc_etrgState && tempMode != tpc_etrgWindow) {
			double temp = (tempComp == tpc_etrgCompNegative)?min(*level, *hysteresis):max(*level, *hysteresis);
			*hysteresis = abs(*level - *hysteresis);
			*level = temp;
			if (tempComp == tpc_etrgCompBoth) {
				*level -= 0.5 * *hysteresis;
			}
		}

		// Convert level and hysteresis into volt
		err = GetParameter(boardAddress, inputNumber, tpc_parRange, &tempPar);
		if (err != 0) return err;
		*level *= tempPar * 0.01;
		*hysteresis *= tempPar * 0.01;
	}

	// Parse AND-Mask
	err = GetParameter(boardAddress, inputNumber, tpc_parANDTriggerMask, &tempPar);
	if (err != 0) return err;
	if (tempPar != 0) {
		int andMask = ((int)tempPar) & (0x01010101 << inputNumber);
		int tempMask = 0xff000000;
		for(int flagMask = 0x08; flagMask != 0x00; flagMask >>= 1) {
			if ((andMask & tempMask) != 0x00) {
				tempFlags |= flagMask;
			}
			tempMask >>= 8;
		}
	}

	// Write ints into enums
	*mode = (TPC_EasyTriggerMode)tempMode;
	*flags = (TPC_EasyTriggerFlags)tempFlags;
	*comp = (TPC_EasyTriggerComperatorMode)tempComp;

	// All's well
	return tpc_noError;
}

TPC_ErrorCode Device::ResetSerTrg(int boardAddress) {
	if (this->CheckBoard(boardAddress) != tpc_noError) return tpc_errInvalidBoardAddress;
	return m_soap->ResetSerTrg(boardAddress);
}

TPC_ErrorCode Device::GetSerTrgProtocol(int boardAddress, TPC_SerialTriggerProtocol *protocol) {
	if (this->CheckBoard(boardAddress) != tpc_noError) return tpc_errInvalidBoardAddress;
	if (protocol == NULL) return tpc_errInvalidParameterValue;
	return m_soap->GetSerTrgProtocol(boardAddress, protocol);
}

TPC_ErrorCode Device::SetI2CTrigger(int boardAddress, TPC_I2CTriggerSettings *settings) {
	if (this->CheckBoard(boardAddress) != tpc_noError) return tpc_errInvalidBoardAddress;
	if (settings == NULL) return tpc_errInvalidParameterValue;
	if (this->CheckInput(boardAddress, settings->chDataInputNumber) != tpc_noError) return tpc_errInvalidInputNumber;
	if (this->CheckInput(boardAddress, settings->chClockInputNumber) != tpc_noError) return tpc_errInvalidInputNumber;
	if ((settings->mode == tpc_i2cAddressAndData) && (settings->dataLength < 1 || settings->dataLength > 16)) return tpc_errInvalidParameterValue;
	if ((settings->mode == tpc_i2cAddressAndData) && (settings->dataStartByte < 0 || settings->dataStartByte > 253)) return tpc_errInvalidParameterValue;
	return m_soap->SetI2CTrigger(boardAddress, settings);
}

TPC_ErrorCode Device::GetI2CTrigger(int boardAddress, TPC_I2CTriggerSettings *settings) {
	if (this->CheckBoard(boardAddress) != tpc_noError) return tpc_errInvalidBoardAddress;
	if (settings == NULL) return tpc_errInvalidParameterValue;
	return m_soap->GetI2CTrigger(boardAddress, settings);
}

TPC_ErrorCode Device::SetCANTrigger(int boardAddress, TPC_CANTriggerSettings *settings) {
	if (this->CheckBoard(boardAddress) != tpc_noError) return tpc_errInvalidBoardAddress;
	if (settings == NULL) return tpc_errInvalidParameterValue;
	if (this->CheckInput(boardAddress, settings->chDataInputNumber) != tpc_noError) return tpc_errInvalidInputNumber;
	if (settings->bitRate < 2000 || settings->bitRate > 1000000) return tpc_errInvalidParameterValue;
	if (settings->bitSamplePoint < 0 || settings->bitSamplePoint > 1.0) return tpc_errInvalidParameterValue;
	if ((settings->mode == tpc_canIdAndData) && (settings->dataLength < 0 || settings->dataLength > 8)) return tpc_errInvalidParameterValue;
	return m_soap->SetCANTrigger(boardAddress, settings);
}

TPC_ErrorCode Device::GetCANTrigger(int boardAddress, TPC_CANTriggerSettings *settings) {
	if (this->CheckBoard(boardAddress) != tpc_noError) return tpc_errInvalidBoardAddress;
	if (settings == NULL) return tpc_errInvalidParameterValue;
	return m_soap->GetCANTrigger(boardAddress, settings);
}


TPC_ErrorCode Device::BeginSet()
{
  m_deferredSets++;
  return tpc_noError;
}



TPC_ErrorCode Device::EndSet()
{
  TPC_ErrorCode err = tpc_noError;

  m_deferredSets--;
  if (m_deferredSets <= 0) {
    m_deferredSets = 0;

    if (m_parameterModifications.Count() > 0) {
      // Send off parameter changes
      err = m_soap->SetMultipleParameters(m_parameterModifications);

      // Update local copy
      FetchCurrentParameters(m_soap);

      // Clear modification list.
      m_parameterModifications.Clear();
    }

    if (m_attributeModifications.Count() > 0) {
      // Send off attribute changes
      TPC_ErrorCode erra = m_soap->SetAttributes(m_attributeModifications);
      if (err == 0) err = erra;

      // Update local copy
      FetchCurrentAttributes(m_soap);

      // Clear modification list.
      m_attributeModifications.Clear();
    }

    if (m_associationModifications.Count() > 0) {
      // Send off new association rows
      TPC_ErrorCode erra = m_soap->SetAssociations(m_associationModifications);
      if (err == 0) err = erra;

      // Update local copy
      FetchCurrentAssociations(m_soap);

      // Clear modification list.
      m_associationModifications.Clear();
    }
  }
  return err;
}



TPC_ErrorCode Device::CancelSet()
{
  m_parameterModifications.Clear();
  m_attributeModifications.Clear();
  m_deferredSets = 0;

  return tpc_noError;
}


//--- Commands ------------------------------------------------------------------


TPC_ErrorCode Device::ExecuteSystemCommand(TPC_SystemCommand command)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = m_soap->ExecuteCommand(command);

  if (command == tpc_cmdStart) {
    m_statusMutex.lock();
    m_currentStatus.status.startInProgress = true;
    for (int b=0; b<tpc_maxBoards; b++) {
      m_currentStatus.status.boards[b].recordingState = tpc_recStarting;  
      m_currentStatus.status.boards[b].triggerState = tpc_recDisarmed;  
      m_currentStatus.status.boards[b].blockCounter = 0;  
      m_currentStatus.status.boards[b].dataCounter = 0;  
      m_currentStatus.status.boards[b].dataLostCounter = 0;  
      m_currentStatus.status.boards[b].blockLostCounter = 0;  
    }
    m_statusMutex.unlock();
  }		

  return err;
}



TPC_ErrorCode Device::StartCalibration(TPC_CalibrationType command)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = m_soap->StartCalibration(command);
  
  if (err == 0) 
  SetStatus();
  return err;
}


TPC_ErrorCode Device::PrepareStart(double* delayTime)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = m_soap->PrepareStart(delayTime);
  return err;
}


TPC_ErrorCode Device::SetStartInfo(const TPC_DateTime& startTime, int measurementNr)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = m_soap->SetStartInfo(startTime, measurementNr);
  return err;
}

//--- Device status -------------------------------------------------------------


void Device::CallStatusCallback()
{
  TPC_DeviceStatus status;
  FillDeviceStatus(&status);
  m_system->ExecuteCallback(m_index, &status);
}

/// Copy/Fill the device status from the current internal device status.
void Device::FillDeviceStatus(TPC_DeviceStatus* status)
{
  status->deviceError = CheckStatus();
  m_statusMutex.lock();
  status->measurementNumber = m_currentStatus.status.measurementNumber;
  Assign(status->measurementStartTime, m_currentStatus.status.startTime);
  status->startInProgress = m_currentStatus.status.startInProgress; 
  for (int b=0; b<tpc_maxBoards; b++) {
    status->boards[b].recordingState = (TPC_RecordingState)m_currentStatus.status.boards[b].recordingState; 
    status->boards[b].triggerState = (TPC_TriggerState)m_currentStatus.status.boards[b].triggerState; 
    status->boards[b].blockCounter = m_currentStatus.status.boards[b].blockCounter; 
    status->boards[b].dataCounter = m_currentStatus.status.boards[b].dataCounter; 
    status->boards[b].dataLostCounter = m_currentStatus.status.boards[b].dataLostCounter; 
    status->boards[b].blockLostCounter = m_currentStatus.status.boards[b].blockLostCounter; 

	for (int i = 0; i < tpc_maxInputs; i++){
		status->boards[b].inputs[i].OverloadLive = m_currentStatus.status.boards[b].inputs[i].OverloadLive;
		status->boards[b].inputs[i].OverloadMeas = m_currentStatus.status.boards[b].inputs[i].OverloadMeas;
        status->boards[b].inputs[i].IEPEStatus = m_currentStatus.status.boards[b].inputs[i].IEPEStatus;
	}
  }
  status->settingsChangesCounter = m_settingsChangesCounter;

  memcpy(status->clusterNrs, m_currentStatus.clusters.clusterNumbers, sizeof(int) * tpc_maxBoards);

  for (int b=0; b<tpc_maxBoards; b++) {
    status->operationModes[b] = 0; 
    double value;
    bool ok = m_previousParameters.GetParameter(b, 0, tpc_parOperationMode, &value);
    if (ok) status->operationModes[b] = (int)value; 
    else status->operationModes[b] = 99;
  }
  status->autosequenceState = (TPC_AUTOSEQ_STATUS)m_currentStatus.status.autosequenceState;
  status->extStartState		= (TPC_ExtStartState)m_currentStatus.status.extStartState;

  CopyStruct(&status->gpsStatus,sizeof(TPC_GPSStatus),&m_currentStatus.status.gpsStatus, sizeof(TPC_GPSStatus));
  
  CopyStruct(&status->tempStatus, sizeof(TPC_TempStatus), &m_currentStatus.status.tempStatus, sizeof(TPC_TempStatus));

  m_statusMutex.unlock();
}



TPC_ErrorCode Device::GetDeviceStatus(TPC_DeviceStatus* status, int structSize)
{
  TPC_DeviceStatus st;
  FillDeviceStatus(&st);

  CopyStruct(status, structSize, &st, sizeof(st));

  return tpc_noError;
}


//--- Data readout --------------------------------------------------------------


TPC_ErrorCode Device::GetYMetaData(int boardAddress, int inputNumber, 
int measurementNumber,
TPC_YMetaData* metaData, int structSize)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckMeasurementNumber(measurementNumber);
  if (err != 0) return err;

  err = CheckInput(boardAddress, inputNumber);
  if (err != 0) return err;

  RawYMetaData r;
  m_statusMutex.lock();
  bool ok = m_yMetadata.GetMetaData(boardAddress, inputNumber, &r);
  m_statusMutex.unlock();
  if (!ok) return tpc_errInternalError;

  TPC_YMetaData m;
  m.inputActive = r.isActive;
  m.analogMask = r.analogMask;
  m.markerMask = r.markerMask;
  m.numberOfMarkerBits = r.numberOfMarkerBits;
  m.resolutionInBits = r.resolutionInBits;
  m.bytesPerSample = r.bytesPerSample;
  m.binToVoltFactor = r.binToVoltFactor;
  m.binToVoltConstant = r.binToVoltConst;
  m.voltToPhysicalFactor = r.voltToPhysicalFactor;		
  m.voltToPhysicalConstant = r.voltToPhysicalConstant;

  m.binToPhysicalFactor = m.voltToPhysicalFactor * m.binToVoltFactor;		
  m.binToPhysicalConstant = m.voltToPhysicalFactor * m.binToVoltConstant + m.voltToPhysicalConstant;	

  m_statusMutex.lock();
  CopyStruct(metaData, structSize, &m, sizeof(m));
  m_statusMutex.unlock();
  return err;
}


TPC_ErrorCode Device::GetMetaDataParameter(int boardAddress, int inputNumber, int measurementNumber, 
TPC_Parameter parameter, double* value)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckInput(boardAddress, inputNumber);
  if (err != 0) return err;

  err = CheckMeasurementNumber(measurementNumber);
  if (err != 0) return err;

  m_statusMutex.lock();
  bool ok = m_previousParameters.GetParameter(boardAddress, inputNumber, parameter, value);
  m_statusMutex.unlock();
  if (!ok)
  return tpc_errInvalidParameter;

  return tpc_noError;
}


TPC_ErrorCode Device::GetMetaDataAttribute(int boardAddress, int inputNumber, 
int measurementNumber, const char* key, string& value)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckMeasurementNumber(measurementNumber);
  if (err != 0) return err;

  m_statusMutex.lock();
  value = m_previousAttributes.GetAttribute(boardAddress, inputNumber, key);
  m_statusMutex.unlock();
  return tpc_noError;
}


TPC_ErrorCode Device::GetAllMetaDataAttributes(int measurementNumber,
TPC_AttributeEnumeratorCallback callback, void* userData)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckMeasurementNumber(measurementNumber);
  if (err != 0) return err;

  // Get from local copy
  GetAllAttributesEnumerator e(callback, userData);
  m_statusMutex.lock();
  m_previousAttributes.EnumerateAttributes(&e);
  m_statusMutex.unlock();
  return tpc_noError;
}



TPC_ErrorCode Device::GetTMetaData(int boardAddress, int blockNumber, 
int measurementNumber,
TPC_TMetaData* metaData, int structSize)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckMeasurementNumber(measurementNumber);
  if (err != 0) return err;

  ns__Metadata NewMetaData;
  err = m_soap->GetMetadata(measurementNumber, boardAddress, blockNumber, blockNumber, &NewMetaData);
  if (err != 0) return err;

  TPC_TMetaData m;

  double x = 0;
  m_statusMutex.lock();
  bool found = m_previousParameters.GetParameter(boardAddress, 0, tpc_parOperationMode, &x);
  m_statusMutex.unlock();
  if (!found) err = tpc_errInternalError;
  int om = (int)x;
  bool dualMode = ((om & tpc_opModeBasicModeMask) == tpc_opModeEventRecorder && (om & tpc_opModeDual) != 0);

  //--- Growing state for block 0
  m.growing = false;
  if (blockNumber == 0) {
    // Growing block 0 is possible in Continuous mode and Dual mode
    if (((om & tpc_opModeBasicModeMask) == tpc_opModeContinuous) || dualMode) {
      // Check if measurement is still in progress
      m_statusMutex.lock();
      if (m_currentStatus.status.boards[boardAddress].recordingState == tpc_recRecording) {
        m.growing = true;
      }
      m_statusMutex.unlock();
    }
  }

  //--- Time base parameters
  x = tpc_tbSrcInternal;
  m_statusMutex.lock();
  found = m_previousParameters.GetParameter(boardAddress, 0, tpc_parTimebaseSource, &x);
  m_statusMutex.unlock();
  if (!found) err = tpc_errInternalError;
  m.timeBaseSource = (int)x;

  m.sampleRate = 0;
  m_statusMutex.lock();
  if(m.timeBaseSource == tpc_tbSrcExternal){
	  found = m_previousParameters.GetParameter(boardAddress, 0, tpc_parExtClockSamplingFrequency, &m.sampleRate);
  } else{
	  found = m_previousParameters.GetParameter(boardAddress, 0, tpc_parSamplingFrequency, &m.sampleRate);
  }
  m_statusMutex.unlock();
  if (!found) err = tpc_errInternalError;
  if (dualMode && blockNumber == 0) {
    double d;
    m_statusMutex.lock();
    found = m_previousParameters.GetParameter(boardAddress, 0, tpc_parEcrClockDivisor, &d);
    m_statusMutex.unlock();
    if (!found) err = tpc_errInternalError;
    m.sampleRate /= d;
  }

  x = 1;
  m_statusMutex.lock();
  found = m_previousParameters.GetParameter(boardAddress, 0, tpc_parExternalClockDivisor, &x);
  m_statusMutex.unlock();
  if (!found) err = tpc_errInternalError;
  m.externalClockDivisor = (int)x;

  //--- Time information
  m_statusMutex.lock();
  Assign(m.startTime, m_currentStatus.status.startTime); 
  m_statusMutex.unlock();

  m.triggerTime = NewMetaData.triggerTime; 
  m.triggerSample = NewMetaData.triggerSample; 
  m.blockLength = NewMetaData.blockLength;
  m.stopTriggerSample = NewMetaData.stopTriggerSample; 
  
  //--- Channel info
  m.singleChannel = (NewMetaData.boardAddress >= 0 && NewMetaData.inputNumber >= 0);
  m.boardAddress = NewMetaData.boardAddress; 
  m.inputNumber = NewMetaData.inputNumber;  

  CopyStruct(metaData, structSize, &m, sizeof(m));
  return err;
}

TPC_ErrorCode Device::GetTMetaData(int boardAddress, int blockNumberFrom, int blockNumberTo,
						   int measurementNumber,TPC_TMetaData * metaData, int structSize){

  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckMeasurementNumber(measurementNumber);
  if (err != 0) return err;
  
  int iNumberOfBlocks = blockNumberTo - blockNumberFrom+1;
  if (iNumberOfBlocks < 1) return err;

  ns__Metadata *NewMetaData = new ns__Metadata[iNumberOfBlocks];
  err = m_soap->GetMetadata(measurementNumber, boardAddress, blockNumberFrom, blockNumberTo, NewMetaData);
  if (err != 0) {
	  delete NewMetaData;
	  return err;
  }
  
  TPC_TMetaData TempMetaData;
  double x = 0;
  
  // Get Operational Mode
  m_statusMutex.lock();
  bool found = m_previousParameters.GetParameter(boardAddress, 0, tpc_parOperationMode, &x);
  m_statusMutex.unlock();
  if (!found) err = tpc_errInternalError;
  
  int om = (int)x;	// om = Operational Mode
  bool dualMode = ((om & tpc_opModeBasicModeMask) == tpc_opModeEventRecorder && (om & tpc_opModeDual) != 0);
  
  // Growing state for block 0
  TempMetaData.growing = false;
  if (blockNumberFrom == 0) {
    // Growing block 0 is possible in Continuous mode and Dual mode
    if (((om & tpc_opModeBasicModeMask) == tpc_opModeContinuous) || dualMode) {
      // Check if measurement is still in progress
      m_statusMutex.lock();
      if (m_currentStatus.status.boards[boardAddress].recordingState == tpc_recRecording) {
        TempMetaData.growing = true;
      }
      m_statusMutex.unlock();
    }
  }
  
  //--- Time base parameters
  x = tpc_tbSrcInternal;
  m_statusMutex.lock();
  found = m_previousParameters.GetParameter(boardAddress, 0, tpc_parTimebaseSource, &x);
  m_statusMutex.unlock();
  if (!found) err = tpc_errInternalError;
  TempMetaData.timeBaseSource = (int)x;

  TempMetaData.sampleRate = 0;
  double SampleRateDivisor = 1;
  m_statusMutex.lock();
  if(TempMetaData.timeBaseSource == tpc_tbSrcExternal){
	  found = m_previousParameters.GetParameter(boardAddress, 0, tpc_parExtClockSamplingFrequency, &TempMetaData.sampleRate);
  } else{
	  found = m_previousParameters.GetParameter(boardAddress, 0, tpc_parSamplingFrequency, &TempMetaData.sampleRate);
  }
  m_statusMutex.unlock();
  if (!found) err = tpc_errInternalError;
  if (dualMode && blockNumberFrom == 0) {
    
    m_statusMutex.lock();
    found = m_previousParameters.GetParameter(boardAddress, 0, tpc_parEcrClockDivisor, &SampleRateDivisor);
    m_statusMutex.unlock();
    if (!found) err = tpc_errInternalError;
    
  }

  x = 1;
  m_statusMutex.lock();
  found = m_previousParameters.GetParameter(boardAddress, 0, tpc_parExternalClockDivisor, &x);
  m_statusMutex.unlock();
  if (!found) err = tpc_errInternalError;
  TempMetaData.externalClockDivisor = (int)x;

  //--- Time information
  m_statusMutex.lock();
  Assign(TempMetaData.startTime, m_currentStatus.status.startTime); 
  m_statusMutex.unlock();
  
  for(int i = 0; i < iNumberOfBlocks; i++){
	  
	  metaData[i].blockLength				= NewMetaData[i].blockLength;
	  metaData[i].boardAddress				= NewMetaData[i].boardAddress;
	  metaData[i].externalClockDivisor		= TempMetaData.externalClockDivisor;
	  metaData[i].growing					= false; 
	  metaData[i].inputNumber				= NewMetaData[i].inputNumber;
	  metaData[i].sampleRate				= TempMetaData.sampleRate;
	  metaData[i].singleChannel				= (NewMetaData[i].boardAddress >= 0 && 
												NewMetaData[i].inputNumber >= 0);
	  metaData[i].startTime					= TempMetaData.startTime;
	  metaData[i].stopTriggerSample			= NewMetaData[i].stopTriggerSample;
	  metaData[i].timeBaseSource			= TempMetaData.timeBaseSource;
	  metaData[i].triggerSample				= NewMetaData[i].triggerSample;
	  metaData[i].triggerTime				= NewMetaData[i].triggerTime;
  }
  if (blockNumberFrom == 0){
	  metaData[0].growing					= TempMetaData.growing;
	  metaData[0].sampleRate				/= SampleRateDivisor;
  }
  delete NewMetaData;
  return err;

}

TPC_ErrorCode Device::GetData(int boardAddress, int inputNumber, int blockNumber, 
int measurementNumber, uint64_t dataStart, uint64_t dataLength, 
int resultLength, double* data, ReadoutType type)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckMeasurementNumber(measurementNumber);
  if (err != 0) return err;

  TPC_YMetaData metaData;
  err = GetYMetaData(boardAddress, inputNumber, measurementNumber, &metaData, sizeof(metaData));
  if (err != 0) return err;

  DataReceiver dataReceiver;
  TPC_ErrorCode error;
  if (type == readoutRange) 
  dataReceiver.AddDataRangeRequest(boardAddress, inputNumber, blockNumber,  
  dataStart, dataLength, resultLength, data, &error, 
  metaData.analogMask, metaData.binToVoltFactor, metaData.binToVoltConstant);
  else
  dataReceiver.AddDataMinMaxRequest(boardAddress, inputNumber, blockNumber,  
  dataStart, dataLength, resultLength, data, &error, 
  metaData.analogMask, metaData.binToVoltFactor, metaData.binToVoltConstant);

  err = m_soap->GetData(measurementNumber, &dataReceiver);
  if (err == 0) err = error;
  return err;
}



TPC_ErrorCode Device::GetRawData(int boardAddress, int inputNumber, int blockNumber, 
int measurementNumber, uint64_t dataStart, uint64_t dataLength, 
int resultLength, int32_t* data, ReadoutType type)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckMeasurementNumber(measurementNumber);
  if (err != 0) return err;

  DataReceiver dataReceiver;
  TPC_ErrorCode error;
  if (type == readoutRange) 
  dataReceiver.AddRawDataRangeRequest(boardAddress, inputNumber, blockNumber,
  dataStart, dataLength, resultLength, data, &error);
  else
  dataReceiver.AddRawDataMinMaxRequest(boardAddress, inputNumber, blockNumber,
  dataStart, dataLength, resultLength, data, &error);

  err = m_soap->GetData(measurementNumber, &dataReceiver);
  if (err == 0) err = error;
  return err;
}



TPC_ErrorCode Device::DeferredGetData(int boardAddress, int inputNumber, int blockNumber, 
uint64_t dataStart, uint64_t dataLength, 
int resultLength, double* data, TPC_ErrorCode* error, ReadoutType type)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  TPC_YMetaData m;
  m_statusMutex.lock();
  int mn = m_currentStatus.status.measurementNumber;
  m_statusMutex.unlock();
  err = GetYMetaData(boardAddress, inputNumber, mn, &m, sizeof(m));
  if (err != 0) return err;

  if (type == readoutRange) 
  m_dataReceiver.AddDataRangeRequest(boardAddress, inputNumber, blockNumber, 
  dataStart, dataLength, resultLength, data, error, 
  m.analogMask, m.binToVoltFactor, m.binToVoltConstant);
  else
  m_dataReceiver.AddDataMinMaxRequest(boardAddress, inputNumber, blockNumber, 
  dataStart, dataLength, resultLength, data, error, 
  m.analogMask, m.binToVoltFactor, m.binToVoltConstant);

  return tpc_noError;
}



TPC_ErrorCode Device::DeferredGetRawData(int boardAddress, int inputNumber, int blockNumber, 
uint64_t dataStart, uint64_t dataLength, 
int resultLength, int32_t* data, TPC_ErrorCode* error, ReadoutType type)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  if (type == readoutRange) 
  m_dataReceiver.AddRawDataRangeRequest(boardAddress, inputNumber, blockNumber,
  dataStart, dataLength, resultLength, data, error);
  else
  m_dataReceiver.AddRawDataMinMaxRequest(boardAddress, inputNumber, blockNumber,
  dataStart, dataLength, resultLength, data, error);

  return tpc_noError;
}



TPC_ErrorCode Device::ProcessDeferredDataRequests(int measurementNumber)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckMeasurementNumber(measurementNumber);
  if (err != 0) return err;

  err = m_soap->GetData(measurementNumber, &m_dataReceiver);
  m_dataReceiver.Clear();
  return err;
}



TPC_ErrorCode Device::CancelDeferredDataRequests()
{
  m_dataReceiver.Clear();
  return tpc_noError;
}



TPC_ErrorCode Device::SendServerCustomCommand(char *command, char *result, int length)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  return m_soap->SendServerCustomCommand(command, result, length);
}


//--- Elsys functions -----------------------------------------------------------

TPC_ErrorCode Device::WriteDev(int boardAddress, int type, unsigned count, void* data, unsigned aux1, unsigned aux2)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckBoard(boardAddress);
  if (err != 0) return err;

  err = m_soap->WriteDev(boardAddress, type, count, data, aux1, aux2);

  return err;
}


TPC_ErrorCode Device::ReadDev(int boardAddress, int type, unsigned count, void* data, unsigned aux1, unsigned aux2)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckBoard(boardAddress);
  if (err != 0) return err;

  err = m_soap->ReadDev(boardAddress, type, count, data, aux1, aux2);

  return err;
}


TPC_ErrorCode Device::ReadWriteTwi(int boardAddress, int amplifier, int count, char *data)
{
  TPC_ErrorCode err = CheckStatus();
  if (err != 0) return err;

  err = CheckBoard(boardAddress);
  if (err != 0) return err;

  return m_soap->ReadWriteTwi(boardAddress, amplifier, count, data);
}
/**********************************************************************************/
TPC_ErrorCode Device::LoadAutosequence( char * cData, int iLength){
	TPC_ErrorCode err = CheckStatus();
	if (err != 0) return err;

	return m_soap->LoadAutosequence( cData, iLength);
}

/**********************************************************************************/
TPC_ErrorCode Device::GetAutoSequence(	char * cData, int iLength){
	TPC_ErrorCode err = CheckStatus();
	if (err != 0) return err;

	return m_soap->GetAutoSequence(cData,iLength);
}

/**********************************************************************************/
TPC_ErrorCode Device::StartAutoSequence(){
	TPC_ErrorCode err = CheckStatus();
	if (err != 0) return err;
	
	return m_soap->StartAutoSequence();
}

/**********************************************************************************/
TPC_ErrorCode Device::StopAutoSequence(	){
	TPC_ErrorCode err = CheckStatus();
	if (err != 0) return err;

	return m_soap->StopAutoSequence();
}

//-------------------------------------------------------------------------------

void Device::InitiateDisconnection()
{
  // Tell the status observer thread to terminate.
  m_statusThread->PrepareToQuit(m_soap);
}



void Device::FinishDisconnection()
{
  // Wait until the status observer has terminated.
  //###pg: no kill    m_statusThread->wait();
}


//-------------------------------------------------------------------------------


void Device::SetStatus()
{
  cScopedLock lock(m_statusMutex);

  m_deviceStatusError = tpc_errCalibrating;
}



TPC_ErrorCode Device::CheckStatus()
{
  cScopedLock lock(m_statusMutex);

  return m_deviceStatusError;
}



TPC_ErrorCode Device::CheckBoard(int boardAddress)
{
  if (!m_deviceInfo.BoardExists(boardAddress)) return tpc_errInvalidBoardAddress;
  return tpc_noError;
}



TPC_ErrorCode Device::CheckInput(int boardAddress, int inputNumber)
{
  if (!m_deviceInfo.InputExists(boardAddress, inputNumber)) {
    if (!m_deviceInfo.BoardExists(boardAddress)) return tpc_errInvalidBoardAddress;
    return tpc_errInvalidInputNumber;
  }
  return tpc_noError;
}



TPC_ErrorCode Device::CheckMeasurementNumber(int measurementNumber)
{
  cScopedLock lock(m_statusMutex);

  if (m_currentStatus.status.startInProgress) return tpc_errNewMeasurement;
  if (m_currentStatus.status.measurementNumber == 0) return tpc_errNoData;
  if (measurementNumber != m_currentStatus.status.measurementNumber) return tpc_errNewMeasurement;
  return tpc_noError;
}



TPC_ErrorCode Device::FetchCurrentAttributes(SoapDevice* soap)
{
  Attributes a;
  bool changed = false;
  TPC_ErrorCode err = soap->GetAllCurrentAttributes(a);
  if (err == 0) {
    m_statusMutex.lock();
    if (m_currentAttributes != a) {
      m_currentAttributes = a;
      changed = true; 
    }
    m_statusMutex.unlock();
  }

  if (changed) SettingsChanged();
  return err;
}



TPC_ErrorCode Device::FetchCurrentParameters(SoapDevice* soap)
{
  ParameterSet p;
  bool changed = false;
  TPC_ErrorCode err = soap->GetAllCurrentParameters(p);
  if (err == 0) {
    m_statusMutex.lock();
    if (m_currentParameters != p) {
      m_currentParameters = p;
      changed = true; 
    }
    m_statusMutex.unlock();
  }

  if (changed) SettingsChanged();
  return err;
}


TPC_ErrorCode Device::FetchCurrentParameters()
{
  return FetchCurrentParameters(m_soap);
}


TPC_ErrorCode Device::FetchCurrentAssociations(SoapDevice* soap)
{
  AssociationRowList a;
  bool changed = false;
  TPC_ErrorCode err = soap->GetCurrentAssociations(a);
  if (err == 0) {
    m_statusMutex.lock();
    if (m_currentAssociations != a) {
      m_currentAssociations = a;
      changed = true; 
    }
    m_statusMutex.unlock();
  }

  if (changed) SettingsChanged();
  return err;
}



void Device::SettingsChanged()
{
  m_settingsChangesCounter++;
  //### raise event
}


void Device::SetDeviceStatusError(TPC_ErrorCode err)
{
  m_statusMutex.lock();

  bool changed = false;
  if (m_deviceStatusError != err) {
    m_deviceStatusError = err;
    changed = true;
  }

  if (m_settingsChanged) changed = true;
  m_settingsChanged = false;

  m_statusMutex.unlock();

  if (changed) SettingsChanged();
}


void Device::SetParamAttr(const ParameterSet& params, const Attributes& attr, const AssociationRowList& assoc)
{
  // Da diese Funktion nur von StatusObserverThread::ReloadParams() aufgerufen wird diese Funktion
  // auserhalb gelocked. Dies ist nötig da ansonsten Parameter Updates die durch einen Callback
  // ausgelesen werden inkonsisten sind.
  //m_statusMutex.lock();
	
  if (m_currentParameters != params) {
    m_currentParameters = params;
    m_settingsChanged = true;
  }

  if (m_currentAttributes != attr) {
    m_currentAttributes = attr;
    m_settingsChanged = true;
  }

  if (m_currentAssociations != assoc) {
    m_currentAssociations = assoc;
    m_settingsChanged = true;
  }

  //m_statusMutex.unlock();
}


void Device::SetPreviousParamAttr(const ParameterSet& params, const Attributes& attr, 
const YMetaDataList& metadata, const AssociationRowList& assoc)
{
  m_statusMutex.lock();

  m_previousParameters = params;
  m_previousAttributes = attr;
  m_yMetadata = metadata;
  m_previousAssociations = assoc;

  m_statusMutex.unlock();
}


void Device::SetStatus(const ns__Status& status)
{
  m_statusMutex.lock();

  bool settingsChgd = false;
  bool newMeas = false;

  if (m_deviceStatusError != 0) {
    m_deviceStatusError = tpc_noError;
    settingsChgd = true;
  }
  else {
    // Compare cluster setting
    for (int i=0; i<tpc_maxBoards; i++) {
      if (m_currentStatus.clusters.clusterNumbers[i] != status.clusters.clusterNumbers[i]) 
      settingsChgd = true;
    }

    // Compare measurement number
    if (m_currentStatus.status.measurementNumber != status.status.measurementNumber)
    newMeas = true;
  }

  m_currentStatus = status;

  if (m_settingsChanged) settingsChgd = true;
  m_settingsChanged = false;

  m_statusMutex.unlock();

  if (settingsChgd) SettingsChanged();
  if (newMeas) {
    //### raise event
  }
}

TPC_ErrorCode Device::GetGPSLogList(int from, int to, char *log){

	TPC_ErrorCode err = CheckStatus();
	if (err != 0) return err;

	return m_soap->GetGPSLogList(from,to, log);

}

