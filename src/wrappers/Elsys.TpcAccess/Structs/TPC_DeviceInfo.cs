using System;
using System.Runtime.InteropServices;

namespace Elsys.TpcAccess.Structs {
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 4)]
    public struct TPC_DeviceInfo {
        public UInt64 deviceId;
        public int serverSoftwareVersion;
        [MarshalAs(UnmanagedType.U1)]
        public bool simulatedHardware;
        public int installedBoards;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 9)]
        public string deviceName;             // char[9]

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 65)]
        public string deviceDescription;      // char[65]
        public Int64 deviceMACAddress;
        [MarshalAs(UnmanagedType.U1)]
        public bool UseLocalTime;
        [MarshalAs(UnmanagedType.U1)]
        public bool AutoStartMeasurement;
        [MarshalAs(UnmanagedType.U1)]
        public bool AutoStartAutoSequence;
        public int ServerPort1;
        public int ServerPort2;
        public int TargetPort;
        [MarshalAs(UnmanagedType.U1)]
        public bool TwoinOneEnabled;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
        public string TwoInOnepassword;   // char[16]
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
        public string DataFileName;       // char[tpc_maxPathLength] tpc_maxPathLength=128
        public int NumberOfBackupFiles;
        public int HdFlushInterval;
        [MarshalAs(UnmanagedType.U1)]
        public bool WriteTroughCache;
        public int ModelType;
        public int SyncLinkDetected;
    }
}
