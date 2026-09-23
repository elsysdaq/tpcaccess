using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Elsys.TpcAccess.Enumerations;
using Elsys.TpcAccess.Structs;

namespace Elsys.TpcAccess.Example {

    class Program {

        static void Main(string[] args) {
            string url = "10.0.0.20:10010";
            int deviceIx;
            ErrorCode err;

            #region Define system and install status callback

            int systemId;
            NativeMethods.TPC_NewSystem(out systemId);
            err = NativeMethods.TPC_SetStatusCallbackSystem(systemId, StatusCallbackSystem, IntPtr.Zero);

            #endregion

            #region Initialize connection to device

            Console.WriteLine($"Initialize connection to device with Url '{url}'...");

            // Initialize the connection to the instrument
            NativeMethods.TPC_BeginSystemDefinitionSystem(systemId);
            err = NativeMethods.TPC_AddDeviceSystemEx(systemId, url, 20, 60, out deviceIx);

            if (CheckError(err)) {
                return;
            }

            // Set new device Index
            deviceIx = systemId + deviceIx;

            err = NativeMethods.TPC_EndSystemDefinitionSystem(systemId, 1000);                    // 1s timeout till connection failed

            if (CheckError(err)) {
                return;
            }

            #endregion

            #region Configure the device

            Console.WriteLine($"Configure the device...");

            // Set default parameters
            err = NativeMethods.TPC_ResetConfiguration();

            if (CheckError(err)) {
                return;
            }

            // Set recording mode to scope
            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.OperationMode, 0);

            if (CheckError(err)) {
                return;
            }

            // Set single shot and turn on auto trigger
            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.ScopeAutoTrigger, 1);

            if (CheckError(err)) {
                return;
            }

            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.ScopeSingleShot, 1);

            if (CheckError(err)) {
                return;
            }

            // Set Sampling frequency to 10MHz
            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.SamplingFrequency, 10000000);

            if (CheckError(err)) {
                return;
            }

            // Set trigger delay to -50%
            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.ScopeTriggerDelay, -50);

            if (CheckError(err)) {
                return;
            }

            // Set Block Size to 16k Samples
            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.ScopeBlockLength, 4096);

            if (CheckError(err)) {
                return;
            }

            // Set Board 0, Channel 0, Input Range to 5V, 0% Offset
            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.Range, 5);

            if (CheckError(err)) {
                return;
            }

            err = NativeMethods.TPC_SetParameter(deviceIx, 0, 0, Parameter.Offset, 0);

            if (CheckError(err)) {
                return;
            }

            // Set Trigger Board 0, Channel 0, Positive Slope at 1V, 0.1V hysteris
            err = NativeMethods.TPC_SetTrigger(deviceIx, 0, 0, TPC_EasyTriggerMode.tpc_etrgSlope, TPC_EasyTriggerComparatorMode.tpc_etrgCompPositive, TPC_EasyTriggerFlags.tpc_etrgNone, 1, 0.1, 0, 0);

            #endregion

            #region Start measurement and readout scaling information

            Console.WriteLine($"Start measurement and readout scaling information...");

            int measurementNumber;
            MakeMeasurement(systemId, deviceIx, out measurementNumber);

            // readout time meta data
            TPC_TMetaData tMetaData;
            err = NativeMethods.TPC_GetTMetaData(deviceIx, 0, 0, measurementNumber, out tMetaData, Marshal.SizeOf(typeof(TPC_TMetaData)));

            if (CheckError(err)) {
                return;
            }

            DateTime startTime = ConvertDateTime(tMetaData.startTime);
            Console.WriteLine("Meas. Start Time: {0}", startTime.ToString());
            Console.WriteLine("Trigger Sample: {0}", tMetaData.triggerSample);

            // read out y meta data
            TPC_YMetaData yMetaData;
            err = NativeMethods.TPC_GetYMetaData(deviceIx, 0, 0, measurementNumber, out yMetaData, Marshal.SizeOf(typeof(TPC_YMetaData)));

            if (CheckError(err)) {
                return;
            }

            int analogMask = yMetaData.analogMask;
            int markerMask = yMetaData.markerMask;
            double bintoVoltFactor = yMetaData.binToVoltFactor;
            double bintoVoltConstant = yMetaData.binToVoltConstant;

            #endregion

            #region Get measurement data

            Console.WriteLine($"Get measurement data...");

            // get raw data
            int blockLength = 4096;
            Int32[] rawData = new Int32[blockLength];
            GCHandle gcHandleData = GCHandle.Alloc(rawData, GCHandleType.Pinned);

            try {
                IntPtr dataPtr = gcHandleData.AddrOfPinnedObject();
                err = NativeMethods.TPC_GetRawData(deviceIx, 0, 0, 0, measurementNumber, 0, blockLength, dataPtr);
            } finally {
                gcHandleData.Free();
            }

            if (CheckError(err)) {
                return;
            }

            double[] voltageData = new double[blockLength];
            int[] analogData = new int[blockLength];
            int[] markerData = new int[blockLength];

            // get analog an marker data
            for (int i = 0; i < blockLength; i++) {
                analogData[i] = rawData[i] & analogMask; // Mask digital marker data from the analog data
                markerData[i] = rawData[i] & markerMask; // Extract the digital marker signals

                // scale to voltage (this gives the same data as the TPC_GetData function)
                voltageData[i] = (analogData[i] * bintoVoltFactor) + bintoVoltConstant;
            }

            #endregion

            Console.WriteLine($"Press any key to continue...");
            Console.ReadLine();

            #region Dispose

            NativeMethods.TPC_SetStatusCallbackSystem(systemId, null, IntPtr.Zero);
            NativeMethods.TPC_DeleteSystem(systemId);

            #endregion

        }

        private static bool CheckError(ErrorCode errCode) {
            if (errCode != ErrorCode.NoError) {
                Console.WriteLine("Error: {0}", errCode.ToString());
                Console.ReadLine();
                return true;
            } else {
                return false;
            }
        }

        private static void StatusCallbackSystem(IntPtr userData, int deviceIndex, IntPtr nativeStatus) {
            TPC_DeviceStatus deviceStatus = (TPC_DeviceStatus)Marshal.PtrToStructure(nativeStatus, typeof(TPC_DeviceStatus));
        }

        private static void MakeMeasurement(int systemId, int deviceIndex, out int measurementNumber) {
            var status = GetDeviceStatus(deviceIndex);
            int measurementNumberBefore = status.measurementNumber;
            ErrorCode errorCode = NativeMethods.TPC_ExecuteSystemCommandSystem(systemId, TPC_SystemCommand.Start);
            measurementNumber = measurementNumberBefore;

            // Wait until measurement started
            while (measurementNumber == measurementNumberBefore) {
                status = GetDeviceStatus(deviceIndex);
                measurementNumber = status.measurementNumber;
                Thread.Sleep(10);
            }

            while (IsRecording(deviceIndex)) {
                Thread.Sleep(200);
            }
        }

        private static TPC_DeviceStatus GetDeviceStatus(int deviceIndex) {
            TPC_DeviceStatus deviceStatus = new TPC_DeviceStatus();
            int size = Marshal.SizeOf(typeof(TPC_DeviceStatus));
            IntPtr ptr = Marshal.AllocCoTaskMem(size);

            if (ptr == IntPtr.Zero) {
                throw new OutOfMemoryException();
            }

            try {
                NativeMethods.TPC_GetDeviceStatus(deviceIndex, ptr, size);
                deviceStatus = (TPC_DeviceStatus)Marshal.PtrToStructure(ptr, typeof(TPC_DeviceStatus));
            } catch(Exception) {
                ;
            }finally {
                Marshal.FreeCoTaskMem(ptr);
            }

            return deviceStatus;
        }

        private static bool IsRecording(int deviceIndex) {
            var status = GetDeviceStatus(deviceIndex);
            bool isRecording = false;

            foreach (var boardStatus in status.boards) {
                isRecording |= boardStatus.recordingState == RecordingState.Starting || boardStatus.recordingState == RecordingState.Recording;
            }

            return isRecording;
        }

        private static DateTime ConvertDateTime(TPC_DateTime dateTime) {
            try {
                if (dateTime.year <= 0
                    || dateTime.month <= 0 || dateTime.month > 12
                    || dateTime.day <= 0 || dateTime.day > 31
                    || dateTime.hour < 0 || dateTime.hour > 23
                    || dateTime.minute < 0 || dateTime.minute > 59
                    || dateTime.second < 0 || dateTime.second > 59) {
                    return DateTime.MinValue;
                }

                return new DateTime(dateTime.year, dateTime.month, dateTime.day,
                                    dateTime.hour, dateTime.minute, dateTime.second, dateTime.milliSecond, DateTimeKind.Local);
            } catch (ArgumentOutOfRangeException) {
                return DateTime.MinValue;
            }
        }

    }

}
