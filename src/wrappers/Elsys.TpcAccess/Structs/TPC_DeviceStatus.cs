using Elsys.TpcAccess.Enumerations;
using System.Runtime.InteropServices;

namespace Elsys.TpcAccess.Structs {

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 8)]
    public struct TPC_DeviceStatus {
        [MarshalAs(UnmanagedType.U4)]
        public ErrorCode deviceError;
        public int measurementNumber;
        public TPC_DateTime measurementStartTime;
        [MarshalAs(UnmanagedType.U1)]
        public bool startInProgress;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = TPC_Values.MaxBoards)]
        public TPC_BoardStatus[] boards;
        public int settingsChangesCounter;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = TPC_Values.MaxBoards)]
        public int[] clusterNrs;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = TPC_Values.MaxBoards)]
        public int[] operationModes;
        [MarshalAs(UnmanagedType.U4)]
        public AutoseqStatus autosequenceState;
        [MarshalAs(UnmanagedType.U4)]
        public ExternStartState extStartState;
        public TPC_GPSStatus gpsStatus;
        public TPC_TempStatus tempStatus;
    }

    public class TPC_Values {
        /// <summary> Maximum number of boards in a device. </summary>
        public const int MaxBoards = 16;

        /// <summary> Maximum number of inputs on a board. </summary>
        public const int MaxInputs = 8;
    }

}
