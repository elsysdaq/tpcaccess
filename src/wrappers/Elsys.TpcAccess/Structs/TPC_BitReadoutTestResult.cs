using System.Runtime.InteropServices;

namespace Elsys.TpcAccess.Structs {

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 8)]
    public struct TPC_BitReadoutTestResult {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = TPC_Values.MaxInputs)]
        public int[] channelBitErrors;
        public int size;
    }

}
