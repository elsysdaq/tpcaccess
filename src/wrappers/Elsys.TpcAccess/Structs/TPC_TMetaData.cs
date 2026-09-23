using System;
using System.Runtime.InteropServices;

namespace Elsys.TpcAccess.Structs {
    [StructLayout(LayoutKind.Sequential)]
    public struct TPC_TMetaData {
        [MarshalAs(UnmanagedType.I1)]
        public bool growing;

        public int timeBaseSource;
        [MarshalAs(UnmanagedType.R8)]
        public double sampleRate;
        public int externalClockDivisor;
        public TPC_DateTime startTime;
        public UInt64 triggerTime;
        public UInt64 triggerSample;
        public UInt64 blockLength;
        public UInt64 stopTriggerSample;

        [MarshalAs(UnmanagedType.I1)]
        public bool singleChannel;

        public int boardAddress;
        public int inputNumber;
    }
}
