using Elsys.TpcAccess.Enumerations;
using System.Runtime.InteropServices;

namespace Elsys.TpcAccess.Structs {
    [StructLayout(LayoutKind.Sequential)]
    public struct TPC_BoardInfo {
        public int boardClass;
        public int hardwareVersion;
        public int serialNr;
        public int driverVersion;
        public int firmwareVersion;
        public int numberOfInputs;
        public int maxMemory;
        public int maxSpeed;

        [MarshalAs(UnmanagedType.I4)]
        public OperationModeOptions operationModeOptions;

        [MarshalAs(UnmanagedType.I4)]
        public AveragingOptions averageOptions;

        [MarshalAs(UnmanagedType.I4)]
        public TriggerExtensionOptions triggerOptions;

        public TPC_DateTime lastFactoryCalibration;
        public TPC_DateTime lastUserCalibration;

        [MarshalAs(UnmanagedType.I4)]
        public FirmwareCapabilities firmwareCapabilities;
    }
}
