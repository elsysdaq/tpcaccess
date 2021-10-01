// -----------------------------------------------------------------------------------------------------------------
/*
* TpcAccess C# Example Program for Continuous Measurement
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
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using System.IO;

using Elsys.TpcAccess.Enumerations;
using Elsys.TpcAccess.Structs;

namespace Elsys.TpcAccess.ContMeasurementExample
{
    class Program
    {
        static UInt64 m_datacounter;
        static int m_iMeasurementNumber;

       
        static void Main(string[] args)
        {
            Console.WriteLine("Elsys TpcAccess Continuouse Mode Measurement Demo");

            string url = "192.168.0.60:10010";          // Replace the IP address with your device address
            int deviceIx;
            ErrorCode err;

            #region Initialize connection to device
            int systemId;
            NativeMethods.TPC_NewSystem(out systemId);

            NativeMethods.TPC_BeginSystemDefinitionSystem(systemId);
            err = NativeMethods.TPC_AddDeviceSystemEx(systemId, url, 20, 60, out deviceIx);

            if (CheckError(err)){
                return;
            }

            // Set new device Index
            deviceIx = systemId + deviceIx;

            err = NativeMethods.TPC_EndSystemDefinitionSystem(systemId,1000);                    // 1s timeout till connection failed

            if (CheckError(err))
            {
                return;
            }
            #endregion

            #region Configure the device

            // Set default parameters
            err = NativeMethods.TPC_ResetConfiguration();

            if (CheckError(err)){
                return;
            }

            int brd = 0;    // board and group parameters must be send always to board 0, input 0
            int inp = 0;

            // Set recording mode to continuous
            err = NativeMethods.TPC_SetParameter(deviceIx, brd, inp, Parameter.OperationMode, (double)TimeBaseMode.Continuous);

            if (CheckError(err))
            {
                return;
            }

            // Set Sampling frequency to 1MHz
            err = NativeMethods.TPC_SetParameter(deviceIx, brd, inp, Parameter.SamplingFrequency, 1000000);

            if (CheckError(err))
            {
                return;
            }

            #endregion

            #region InitalizeStatusUpdate and start measurement

            m_datacounter = 0;  // Initialize the data counter to 0

            // install a callback function for getting the device status as fast as possible
            NativeMethods.TPC_SetStatusCallbackSystem(systemId, StatusCall, IntPtr.Zero);

            // Initialize unmanged memory to hold the struct.
            IntPtr NativeStatus = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(TPC_DeviceStatus)));

            int s = Marshal.SizeOf(typeof(TPC_DeviceStatus));

            NativeMethods.TPC_GetDeviceStatus(deviceIx, NativeStatus, s);
            TPC_DeviceStatus deviceStatus = (TPC_DeviceStatus)Marshal.PtrToStructure(NativeStatus, typeof(TPC_DeviceStatus));

            // get current measurement number (needet for detecting the start of measurement )

            int currentMeasurementNr = deviceStatus.measurementNumber;
            m_iMeasurementNumber = deviceStatus.measurementNumber;

            Marshal.FreeHGlobal(NativeStatus);

            // send start recording command (asynchron) 
            NativeMethods.TPC_ExecuteSystemCommandSystem(systemId, TpcAccess.Enumerations.TPC_SystemCommand.Start);
           
            // wait till recording has started = new measurment number is set in the callback function
            while (currentMeasurementNr == m_iMeasurementNumber)
            {
                // ToDO programm abord condition if start measurement failed
                Thread.Sleep(200);
            };
            Console.WriteLine("Measurement has started");

            #endregion

            #region DataReadout

            UInt64 MaxRecordingLenght = 5 * 1024 * 1024;      // define recording length by the number of samples
            int ReadChunkSize = 32 * 1024;                   // read chunk size
            Int32[] rawData = new Int32[ReadChunkSize];      // data buffer

            GCHandle gcHandleData = GCHandle.Alloc(rawData, GCHandleType.Pinned);
            IntPtr dataPtr = gcHandleData.AddrOfPinnedObject();

            UInt64 ReadDataCounter = 0;    // actual read data counter

            BinaryWriter writer_CH0 = new BinaryWriter(File.Open("data_ch0.bin", FileMode.Create));
            BinaryWriter writer_CH1 = new BinaryWriter(File.Open("data_ch1.bin", FileMode.Create));
            

            while (ReadDataCounter < MaxRecordingLenght)
            {
                UInt64 tempCounter = m_datacounter;
                if (tempCounter > MaxRecordingLenght)
                {
                    NativeMethods.TPC_ExecuteSystemCommandSystem(systemId,TpcAccess.Enumerations.TPC_SystemCommand.Stop);
                    Console.WriteLine("Measurement has stopped");
                }
                int count = (int)(tempCounter - ReadDataCounter);

                while (count > 0)
                {
                    int c = Math.Min(count, ReadChunkSize);
                    Console.Write("Read Data: ");
                    Console.Write(c);
                    Console.Write("\t from ");
                    Console.WriteLine(ReadDataCounter);


                    // read channel 0
                    NativeMethods.TPC_GetRawData(deviceIx, brd, 0, 0, m_iMeasurementNumber, ReadDataCounter, c, dataPtr);
                    for(int i = 0; i < ReadChunkSize; i++)
                    {
                        writer_CH0.Write(rawData[i]);
                    }
                    
                    // read channel 1
                    NativeMethods.TPC_GetRawData(deviceIx, brd, 1, 0, m_iMeasurementNumber, ReadDataCounter, c, dataPtr);
                    for (int i = 0; i < ReadChunkSize; i++)
                    {
                        writer_CH1.Write(rawData[i]);
                    }

                    // update read counters
                    ReadDataCounter += (ulong)c;
                    count -= c;
                }
            };

            gcHandleData.Free();

            writer_CH0.Close();
            writer_CH1.Close();

           
            #endregion
        }

        private static bool CheckError(ErrorCode errCode)
        {
            if (errCode != ErrorCode.NoError)
            {
                Console.WriteLine("Error: {0}", errCode.ToString());
                Console.ReadLine();
                return true;
            }
            else
            {
                return false;
            }
        }

        private static void StatusCall(IntPtr userData, int deviceIndex, IntPtr nativeStatus)
        {
            TPC_DeviceStatus deviceStatus = (TPC_DeviceStatus)Marshal.PtrToStructure(nativeStatus, typeof(TPC_DeviceStatus));
           
            m_iMeasurementNumber = deviceStatus.measurementNumber;
            Console.WriteLine(m_iMeasurementNumber);
            // Check actual recording state of board 0
            switch (deviceStatus.boards[0].recordingState)
            {
                case TpcAccess.Enumerations.RecordingState.Aborted:
                case TpcAccess.Enumerations.RecordingState.Stopped:
                    // Recording is stopped or abborded (stop was send)
                    break;
                case TpcAccess.Enumerations.RecordingState.Starting:
                    break;
                case TpcAccess.Enumerations.RecordingState.Recording:
                    m_datacounter = (ulong)deviceStatus.boards[0].dataCounter;  // update current data counter for continuous measurements
                    break;
                case TpcAccess.Enumerations.RecordingState.StartError:
                    break;
            }

        }
    }
}
