using System.Runtime.InteropServices;

namespace Elsys.TpcAccess.Structs {

    [StructLayout(LayoutKind.Sequential)]
    public struct TPC_TempStatus {
        public int CPUTemp;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
        public int[] BoardTemp;
        public int Res1Temp;
        public int Res2Temp;
    }

}
