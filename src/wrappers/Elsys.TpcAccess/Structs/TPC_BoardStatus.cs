using Elsys.TpcAccess.Enumerations;
using System;
using System.Runtime.InteropServices;

namespace Elsys.TpcAccess.Structs {

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 8)]
    public struct TPC_BoardStatus {
        [MarshalAs(UnmanagedType.U4)]
        public RecordingState recordingState;
        [MarshalAs(UnmanagedType.U4)]
        public TriggerState triggerState;
        public int blockCounter;
        public Int64 dataCounter;
        public int dataLostCounter;
        public int blockLostCounter;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = TPC_Values.MaxInputs)]
        public TPC_InputStatus[] inputs;
    }

}
