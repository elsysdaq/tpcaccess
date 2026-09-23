using System.Runtime.InteropServices;

namespace Elsys.TpcAccess.Structs {

    [StructLayout(LayoutKind.Sequential)]
    public struct TPC_GPSStatus {
        [MarshalAs(UnmanagedType.U1)]
        public bool locked;
        public int NrOfSatVisible;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
        public string gpsTime; // char[128]
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
        public string lastFrame; // char[128]
        public int nrOfloggedPositions;
    }

}
