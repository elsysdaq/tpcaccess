// ********************************************************************************/
/* TpcAccess API Example
/*
/* This short example shows how to use the TpcAccess.dll 
/* It shows how to build up a TPC_System, how to set recording parameter,
/* how to redout the device status and how to redout the measurement data. 
/*
/* Copyright: 2022 ELSYS AG
/* Programmer: Roman Bertschi
/*
/* Contact: email: info@elsys.ch
/*
// ********************************************************************************/
// -----------------------------------------------------------------------------------------------------------------
/*
* TpcAccess Example Program for ECR Measurement
*
* This short example shows how to use the TpcAccess.dll 
* It shows how to build up a TPC_System, how to set recording parameter,
* how to redout the device status and how to redout the measurement data. 
*
* ELSYS EXPRESSLY DISCLAIMS ALL WARRANITIES OR REPRESENTATIONS WITH RESPECT TO
* THIS SOFTWARE AND DOCUMENTATION AND ANY SUPPORT OR MAINTENANCE SERVICES THAT
* ELSYS MAY CHOOSE TO PROVIDE IN RELATION THERETO(INCLUDING, WITHOUT
* LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE OR THAT THE SOFTWARE : WILL BE
* ERROR - FREE, WILL OPERATE WITHOUT INTERUPTION, WILL NOT INFRINGE THE RIGHTS OF A
* THIRD PARTY, OR WILL PRODUCE RESULTS IN CONNECTION WITH ITS USE).FURTHER,
* ELSYS EXPRESSLY DISCLAIMS ALL EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
* WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
*
* Copyright 2022 Elsys AG
* Author: Roman Bertschi
* Contact: info@elsys.ch
* Licence: MIT License
* See Licence file for more information about the used licence.
*
*/
// -----------------------------------------------------------------------------------------------------------------

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <iostream>
#include "TpcAccess.h"
#include "ecr-multi-channel.h"

using namespace std;

// main entry point
int main(){
	
	int deviceIx;
	TPC_ErrorCode err;


	// Connecting to one device:
	TPC_BeginSystemDefinition();
	deviceIx = TPC_AddDevice("localhost:10010");		 // Replace the IP address with your device address
	err = TPC_EndSystemDefinition(1000); // 1s timeout till connection failed
	if(!CheckErrorCode(err))
		return 1;


	// Set default parameters
	CheckErrorCode(TPC_ResetConfiguration());

	// Set recording mode to ECR Multi-Channel
	CheckErrorCode(TPC_SetParameter(deviceIx, 0, 0, tpc_parOperationMode, tpc_opModeEventRecorderMulti));
	
	TPC_DeviceInfo devInfo;

	TPC_GetDeviceInfo(deviceIx, &devInfo, sizeof(TPC_DeviceInfo));
	
	int boardMask = 0x1;
	int boardIndex = 0;
	for (int b = 0; b < 16; b++) {
		if ((devInfo.installedBoards & boardMask) != 0) {
			cout << "Board " << boardIndex << " installed" << endl;
			
			TPC_BoardInfo boardInfo;
			CheckErrorCode(TPC_GetBoardInfo(deviceIx, boardIndex, &boardInfo, sizeof(TPC_BoardInfo)));

			// Turn off all Channels
			for (int input = 0; input < boardInfo.numberOfInputs; input++) {
				CheckErrorCode(TPC_SetParameter(deviceIx, b, input, tpc_parInputMode, tpc_inpOff));
			}
		}
		boardMask = boardMask << 1;
		boardIndex++;
	}

	// Set Sampling frequency to 10MHz
	CheckErrorCode(TPC_SetParameter(deviceIx, 0, 0, tpc_parSamplingFrequency,10000000));

	// Set trigger pretrigger to 1kS and Post Trigger 10kS
	CheckErrorCode(TPC_SetParameter(deviceIx, 0, 0, tpc_parEcrPreTrigger, 1024));
	CheckErrorCode(TPC_SetParameter(deviceIx, 0, 0, tpc_parEcrPostTrigger, 1024*10));

	// Turn on HoldOff Trigger Mode, HoldOff Time = 10ks
	CheckErrorCode(TPC_SetParameter(deviceIx, 0, 0, tpc_parEcrEnableHoldOff, 1));
	CheckErrorCode(TPC_SetParameter(deviceIx, 0, 0, tpc_parEcrHoldOffTime, 1024*10));

	// Set Block Size is equal Pre and Post Trigger Time
	m_iBlockSize = 1024 + 10 * 1024;

	// Limit NUmber of Blocks to 10
	unsigned int maxNumberOfBlocks = 10;
	CheckErrorCode(TPC_SetParameter(deviceIx, 0, 0, tpc_parEcrMaximumNumberOfBlocks, maxNumberOfBlocks));
	
	// Turn on Channel A1
	CheckErrorCode(TPC_SetParameter(deviceIx, 0, 0, tpc_parInputMode, tpc_inpSingleEnded));

	// Set Board 0, Channel 0, Input Range to 5V, 0% Offset
	CheckErrorCode(TPC_SetParameter(deviceIx, 0, 0, tpc_parRange, 5));
	CheckErrorCode(TPC_SetParameter(deviceIx, 0, 0, tpc_parOffset, 50));

	TPC_SetTrigger(deviceIx,0,0,tpc_etrgSlope,tpc_etrgCompPositive,tpc_etrgNone, 1,0.1);

	
	// install a callback function for getting the device status as fast as possible
	myCallbackFunctionPointer = (TPC_StatusCallbackFunc)StatusCallback;
	TPC_SetStatusCallback(*myCallbackFunctionPointer, NULL);
	
	int measurementNumber = 0;

	// get current measurement number (needet for end of recording detection)
	TPC_DeviceStatus status;
	CheckErrorCode(TPC_GetDeviceStatus(deviceIx,&status, sizeof(TPC_DeviceStatus)));
	int currentMeasurementNr = status.measurementNumber;
	m_iMeasurementNumber = status.measurementNumber;
	// start recording
	CheckErrorCode(TPC_ExecuteSystemCommand(tpc_cmdStart));

	// wait till recording has started = new measurment number is set in the callback function
	while(currentMeasurementNr == m_iMeasurementNumber){};
	// wait till recording has finished
	while(m_recordingStopped == false){};
	
	// read out y meta data
	TPC_YMetaData ymetaData;
	CheckErrorCode(TPC_GetYMetaData(deviceIx, 0, 0, m_iMeasurementNumber, &ymetaData, sizeof(TPC_YMetaData)));
	uint32_t analogMask = ymetaData.analogMask;
	uint32_t markerMask = ymetaData.markerMask;
	double bintoVoltFactor = ymetaData.binToVoltFactor;
	double bintoVoltConstant = ymetaData.binToVoltConstant;

	int32_t* rawData, * analogData, * markerData;
	rawData = new int32_t[m_iBlockSize];
	analogData = new int32_t[m_iBlockSize];
	markerData = new int32_t[m_iBlockSize];

	double* voltageData;
	voltageData = new double[m_iBlockSize];

	for (int block = 0; block < maxNumberOfBlocks; block++) {
		// readout time meta data
		TPC_TMetaData tmetaData;
		CheckErrorCode(TPC_GetTMetaData(deviceIx, 0, block, m_iMeasurementNumber, &tmetaData, sizeof(TPC_TMetaData)));
		cout << endl << "Meas. Start Time: " << tmetaData.startTime.hour << ":"
			<< tmetaData.startTime.minute << ":"
			<< tmetaData.startTime.second << endl;
		cout << "Trigger Sample: " << tmetaData.triggerSample << endl;
		cout << "Trigger Time:   " << tmetaData.triggerTime << endl;

		// get raw data
		CheckErrorCode(TPC_GetRawData(deviceIx, 0, 0, block, m_iMeasurementNumber, 0, m_iBlockSize, rawData));

		// get analog an marker data
		for (int i = 0; i < m_iBlockSize; i++) {
			analogData[i] = rawData[i] & analogMask;
			markerData[i] = rawData[i] & markerMask;

			// scale to voltage (this gives the same data as the TPC_GetData function)
			voltageData[i] = (analogData[i] * bintoVoltFactor) + bintoVoltConstant;
			//cout << voltageData[i] << endl;
		}

		
		
	}
	
	int input;
	cin >> input;

	delete[] rawData, analogData, markerData, voltageData;
	
	return 0;
}

bool CheckErrorCode(TPC_ErrorCode err){
    
	char cError[32];
	if (err != tpc_noError){
		TPC_ErrorToString(err,cError,sizeof(cError));
		cout << cError;
		return false;
	}
	return true;

}

 // Status Callback Function
void __stdcall StatusCallback(void* userData, int deviceIx, struct TPC_DeviceStatus* status){
	
	//cout << "Enter Status Callback Function m= " << status->measurementNumber << endl;
	m_iMeasurementNumber = status->measurementNumber;
	m_recordingStopped = false;
	switch (status->boards[0].recordingState){
		case tpc_recAborted:
			m_recordingStopped = true;
			//cout << "Board 0 Recording state: aborded." << endl;
			break;
		case tpc_recStarting:
			//cout << "Board 0 Recording state: started." << endl;
			break;
		case tpc_recRecording:
			//cout << "Board 0 Recording state: recording." << endl;
			if(status->boards[0].blockCounter > 0)
				cout << "Block Nr " << status->boards[0].blockCounter << " triggered" << endl;
			break;
		case tpc_recStopped:
			m_recordingStopped = true;
			//cout << "Board 0 Recording state: stopped." << endl;
			break;
		case tpc_recStartError:
			m_recordingStopped = true;
			//cout << "Board 0 Recording state: Start Error." << endl;
			break;
	}	
	
	
}

