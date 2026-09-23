using Elsys.TpcAccess.Enumerations;
using Elsys.TpcAccess.Structs;
using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Elsys.TpcAccess {

    public class NativeMethods {
        private const string dllName = "TpcAccess.dll";

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetFreeDiskSpace(int deviceIx, out UInt64 freeDiskSpace, out UInt64 DiskSize);

        /*[DllImport(dllName)]
        public static extern ErrorCode TPC_SetDeviceSettings(int deviceIx, TPC_DeviceInfo deviceSettings);*/
        [DllImport(dllName)]
        public static extern ErrorCode TPC_SetDeviceSettings(int deviceIx, IntPtr deviceSettings);


        [DllImport(dllName)]
        public static extern ErrorCode TPC_ErrorToString(ErrorCode errorCode, StringBuilder errorString, int maxLen);

        [DllImport(dllName)]
        public static extern int TPC_GetApiVersion();

        [DllImport(dllName)]
        public static extern void TPC_TerminateTpcAccess();

        [DllImport(dllName)]
        public static extern void TPC_GetDeviceUrl(int index, StringBuilder buffer, int maxLen);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetDeviceUrlSystem(int id, int index, StringBuilder buffer, int maxLen);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetDeviceInfo(int deviceIx, IntPtr status, int structSize);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetBoardInfo(int deviceIx, int boardAddress,
                                                        out TPC_BoardInfo boardInfo, int structSize);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetInputInfo(int deviceIx, int boardAddress, int inputNumber,
                                                        out TPC_InputInfo boardInfo, int structSize);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetDeviceStatus(int deviceIx, IntPtr status, int structSize);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetClusterNumbers(int deviceIx, IntPtr clusterNumbers);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_SetClusterNumbers(int deviceIx, IntPtr clusterNumbers);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_SetParameter(int deviceIx, int boardAddress, int inputNumber, Parameter parameter, double value);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetParameter(int deviceIx, int boardAddress, int inputNumber, Parameter parameter, out double value);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetParameterAvailableValues(int deviceIx, int boardAddress, int inputNumber, Parameter parameter, out ParameterAvailableValuesType type, out IntPtr buffer, out int length);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_FreeParameterAvailableValues(IntPtr buffer);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_SetAttribute(int deviceIx, int boardAddress, int inputNumber, string key, string value);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetAttribute(int deviceIx, int boardAddress, int inputNumber, string key, StringBuilder buffer, int maxLen);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetAssociatedChannels(int deviceIx, int boardAddress, int inputNumber, IntPtr dataPtr, ref int count);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_SetAssociatedChannels(int deviceIx, int boardAddress, int inputNumber, IntPtr dataPtr, int count);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetAllAttributes(int deviceIx, TPC_AttributeEnumeratorCallback callback, IntPtr userData);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_StartCalibration(int deviceIx);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetYMetaData(int deviceIx, int boardAddress, int inputNumber,
                                                        int measurementNumber,
                                                        out TPC_YMetaData metaData, int structSize);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetMetaDataParameter(int deviceIx, int boardAddress, int inputNumber,
                                                                int measurementNumber,
                                                                Parameter parameter, out double value);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetMetaDataAttribute(int deviceIx, int boardAddress, int inputNumber,
                                                                int measurementNumber,
                                                                string key, StringBuilder buffer, int maxLen);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetAllMetaDataAttributes(int deviceIx, int measurementNumber,
                                                                    TPC_AttributeEnumeratorCallback callback,
                                                                    IntPtr userData);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetMetaDataAssociatedChannels(int deviceIx, int boardAddress,
                                                                         int inputNumber,
                                                                         int measurementNumber,
                                                                         IntPtr dataPtr, ref int count);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetTMetaData(int deviceIx, int boardAddress, int blockNumber,
                                                        int measurementNumber,
                                                        out TPC_TMetaData metaData, int structSize);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetAllTMetaData(int deviceIx, int boardAddress, int blockNumberFrom, int blockNumberTo,
                                                           int measurementNumber, IntPtr pTMetaDataArray, int structSize);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetData(int deviceIx, int boardAddress, int inputNumber, int blockNumber,
                                                   int measurementNumber, UInt64 dataStart, int dataLength,
                                                   IntPtr data);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetRawData(int deviceIx, int boardAddress, int inputNumber,
                                                      int blockNumber,
                                                      int measurementNumber, UInt64 dataStart, int dataLength,
                                                      IntPtr data);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetMinMaxData(int deviceIx, int boardAddress, int inputNumber,
                                                         int blockNumber,
                                                         int measurementNumber, UInt64 dataStart, UInt64 dataLength,
                                                         int resultLength, IntPtr data);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetRawMinMaxData(int deviceIx, int boardAddress, int inputNumber,
                                                            int blockNumber,
                                                            int measurementNumber, UInt64 dataStart,
                                                            UInt64 dataLength,
                                                            int resultLength, IntPtr data);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_DeferredGetData(int deviceIx, int boardAddress, int inputNumber,
                                                           int blockNumber,
                                                           UInt64 dataStart, int dataLength,
                                                           IntPtr data,
                                                           IntPtr error);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_DeferredGetRawData(int deviceIx, int boardAddress, int inputNumber,
                                                              int blockNumber,
                                                              UInt64 dataStart, int dataLength,
                                                              IntPtr data, IntPtr error);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_DeferredGetMinMaxData(int deviceIx, int boardAddress, int inputNumber,
                                                                 int blockNumber,
                                                                 UInt64 dataStart, UInt64 dataLength,
                                                                 int resultLength,
                                                                 IntPtr data, IntPtr error);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_DeferredGetRawMinMaxData(int deviceIx, int boardAddress, int inputNumber,
                                                                    int blockNumber,
                                                                    UInt64 dataStart, UInt64 dataLength,
                                                                    int resultLength, IntPtr data,
                                                                    IntPtr error);


        [DllImport(dllName)]
        public static extern ErrorCode TPC_NewSystem(out int id);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_DeleteSystem(int id);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_BeginSystemDefinition();

        [DllImport(dllName)]
        public static extern ErrorCode TPC_BeginSystemDefinitionSystem(int id);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_AddDevice(string url);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_AddDeviceSystem(int id, string url, out int deviceIx);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_AddDeviceEx(string url, int recvTimeOut, int sendTimeOut);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_AddDeviceSystemEx(int id, string url, int recvTimeOut, int sendTimeOut, out int deviceIx);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_RemoveDeviceSystem(int id, string url);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_EndSystemDefinition(int connectionTimeoutMilliseconds);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_EndSystemDefinitionSystem(int id, int connectionTimeoutMilliseconds);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_NumDevices();

        [DllImport(dllName)]
        public static extern ErrorCode TPC_NumDevicesSystem(int id, out int n);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_ResetConfiguration();

        [DllImport(dllName)]
        public static extern ErrorCode TPC_ResetConfigurationSystem(int id);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_BeginSet();

        [DllImport(dllName)]
        public static extern ErrorCode TPC_BeginSetSystem(int id);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_EndSet();

        [DllImport(dllName)]
        public static extern ErrorCode TPC_EndSetSystem(int id);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_CancelSet();

        [DllImport(dllName)]
        public static extern ErrorCode TPC_CancelSetSystem(int id);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_ExecuteSystemCommand(TPC_SystemCommand command);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_ExecuteSystemCommandSystem(int id, TPC_SystemCommand command);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_MakeMeasurement(int timeout, out int measurementNumber);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_SetStatusCallbackSystem(int id, TPC_StatusCallbackFunc callback,
                                                                   IntPtr userData);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_ProcessDeferredDataRequestsSystem(int id, int measurementNumber);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_CancelDeferredDataRequestsSystem(int id);


        [DllImport(dllName)]
        public static extern ErrorCode TPC_Elsys_StartCalibration(int deviceIx, int command);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_WriteDev(int deviceIx, int boardAddress, int type, uint count, IntPtr data, uint aux1, uint aux2);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_ReadDev(int deviceIx, int boardAddress, int type, uint count, IntPtr data, uint aux1, uint aux2);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_ReadWriteTwi(int deviceIx, int boardAddress, int amplifier, int count, IntPtr data);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_LoadAutosequence(int deviceIx, StringBuilder buffer, int maxLen);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetAutoSequence(int deviceIx, StringBuilder buffer, int maxLen);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_StartAutoSequence(int deviceIx);

        [DllImport(dllName)]
        public static extern ErrorCode TPC_StopAutoSequence(int deviceIx);

        // Easy trigger & recording functions
        [DllImport(dllName)]
        public static extern ErrorCode TPC_SetTrigger(int deviceIx, int boardAddress, int inputNumber, TPC_EasyTriggerMode mode, TPC_EasyTriggerComparatorMode comp, TPC_EasyTriggerFlags flags, double level, double hysteresis, int time, int time2);

        // Easy trigger & recording functions
        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetTrigger(int deviceIx, int boardAddress, int inputNumber, out TPC_EasyTriggerMode mode, out TPC_EasyTriggerComparatorMode compMode, out TPC_EasyTriggerFlags flags, out double level, out double hysteresis, out int time, out int time2);


        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetInputRanges(int deviceIx, int boardAddress, int inputNumber, InputCoupling inputType, IntPtr inputRanges, int nrOfRanges);


        [DllImport(dllName)]
        public static extern ErrorCode TPC_GetBitReadoutErrors(int deviceIx, int boardAddress, out TPC_BitReadoutTestResult result);


        [DllImport(dllName)]
        public static extern ErrorCode TPC_WritePhaseParameters(int deviceIx, int boardAddress);
    }

    public delegate bool TPC_AttributeEnumeratorCallback(IntPtr userData, int boardAddress, int inputNumber, string key, string value);
    public delegate void TPC_StatusCallbackFunc(IntPtr userData, int deviceIx, IntPtr status);

}
