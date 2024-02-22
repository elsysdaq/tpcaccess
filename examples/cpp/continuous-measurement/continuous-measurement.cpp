// -----------------------------------------------------------------------------------------------------------------
/*
* TpcAccess Example Program for Continuous Measurement
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
* Copyright 2021 Elsys AG
* Author: Roman Bertschi
* Contact: info@elsys.ch
* Licence: MIT License
* See Licence file for more information about the used licence. 
*
*/ 
// -----------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <algorithm>        // used for min/max function  
#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "TpcAccess.h"

using namespace std;

// declare some global variables
TPC_StatusCallbackFunc myCallbackFunctionPointer;       // Call back function pointer

volatile int m_iMeasurementNumber;                      // measurement number for tracking measurement start
 
int64_t m_datacounter;

// Function declaration
void __stdcall StatusCallback(void* userData, int deviceIx, struct TPC_DeviceStatus* status);


int main()
{
    int deviceIx;
    TPC_ErrorCode err; 

    // Connecting to one device:
    TPC_BeginSystemDefinition();
    deviceIx = TPC_AddDevice("127.0.0.1:10010");        // Replace the IP address with your device address
    err = TPC_EndSystemDefinition(1000);                   // 1s timeout till connection failed
    if (err != tpc_noError) {
        cout << "TpcAccess Connection Error!" << endl;
        return 1;
    }
     
    int brd = 0;    // board and group parameters must be send always to board 0, input 0
    int inp = 0;

    // Set default parameters
    TPC_ResetConfiguration();
    
    // Set recording mode to continuous
    TPC_SetParameter(deviceIx, brd, inp, tpc_parOperationMode, tpc_opModeContinuous);

    // Set Sampling frequency to 1MHz
    TPC_SetParameter(deviceIx, brd, inp, tpc_parSamplingFrequency, 1000000);

    m_datacounter = 0;  // Initialize the data counter to 0

    // install a callback function for getting the device status as fast as possible
    myCallbackFunctionPointer = (TPC_StatusCallbackFunc)StatusCallback;
    TPC_SetStatusCallback(*myCallbackFunctionPointer, NULL);


    // get current measurement number (needet for detecting the start of measurement )
    TPC_DeviceStatus status;
    TPC_GetDeviceStatus(deviceIx, &status, sizeof(TPC_DeviceStatus));
    
    int currentMeasurementNr    = status.measurementNumber;
    m_iMeasurementNumber        = status.measurementNumber;
    
    // send start recording command (asynchron) 
    TPC_ExecuteSystemCommand(tpc_cmdStart);
    
    // wait till recording has started = new measurment number is set in the callback function
    while (currentMeasurementNr == m_iMeasurementNumber) {
        // ToDO programm abord condition if start measurement failed
    };
    cout << "Measurement has started" << endl;

    int64_t const   MaxRecordingLenght = 5 * 1024 * 1024;      // define recording length by the number of samples
    int const       ReadChunkSize = 32 * 1024;                  // read chunk size
    int32_t*        buffer;                                     // data buffer

    // init buffer to chunk size
    buffer = new int32_t[ReadChunkSize];

    int64_t     ReadDataCounter = 0;    // actual read data counter
    
    // frite data to a binary file, one file per channel
    ofstream datafile_ch0, datafile_ch1;
    datafile_ch0.open("data_ch0.bin", ios::out | ios::binary);
    datafile_ch1.open("data_ch1.bin", ios::out | ios::binary);

    while (ReadDataCounter < MaxRecordingLenght) {
        int64_t tempCounter = m_datacounter;
        if (tempCounter > MaxRecordingLenght){
            TPC_ExecuteSystemCommand(tpc_cmdStop);
            cout << "Measurement has stopped" << endl;
       }
        int count = tempCounter - ReadDataCounter;
      
            while (count > 0) {
                int c = min(count, ReadChunkSize);
                cout << "Read Data: " << c << " \t from " << ReadDataCounter << endl;
                
                // read channel 0
                TPC_GetRawData(deviceIx, brd, 0, 0, m_iMeasurementNumber, ReadDataCounter, c, buffer);
                datafile_ch0.write((char*)buffer, c*sizeof(int32_t));

                // read channel 1
                TPC_GetRawData(deviceIx, brd, 0, 1, m_iMeasurementNumber, ReadDataCounter, c, buffer);
                datafile_ch1.write((char*)buffer, c*sizeof(int32_t));

                // update read counters
                ReadDataCounter += c;
                count -= c;
            }
    };

    // start recording
   
    cout << ReadDataCounter << " samples stored to the file" << endl;

    datafile_ch0.close();
    datafile_ch1.close();
    delete[] buffer;
}

// *****************************************************************************************************
// Status Callback Function
// *****************************************************************************************************
void __stdcall StatusCallback(void* userData, int deviceIx, struct TPC_DeviceStatus* status) {

    m_iMeasurementNumber = status->measurementNumber;
   
    switch (status->boards[0].recordingState) {
        case tpc_recAborted:
        case tpc_recStopped:
            // Recording is stopped or abborded (stop was send)
            break;
        case tpc_recStarting:
            break;
        case tpc_recRecording:
            m_datacounter = status->boards[0].dataCounter;  // update current data counter for continuous measurements
            break;
        case tpc_recStartError:
            break;
    }
}