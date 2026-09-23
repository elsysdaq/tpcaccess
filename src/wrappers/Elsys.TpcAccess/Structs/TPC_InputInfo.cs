using Elsys.TpcAccess.Enumerations;
using System.Runtime.InteropServices;

namespace Elsys.TpcAccess.Structs {
    [StructLayout(LayoutKind.Sequential)]
    public struct TPC_InputInfo {

        public int inputClass;
        public int hardwareVersion;
        public int maxAdcSpeed;
        public int adcResolution;

        [MarshalAs(UnmanagedType.I4)]
        public OffsetOptions offsetOptions;

        [MarshalAs(UnmanagedType.I4)]
        public FilterOptions filterOptions;

        [MarshalAs(UnmanagedType.I4)]
        public DifferentialInputOptions diffOptions;

        [MarshalAs(UnmanagedType.I4)]
        public InputCouplingOptions inputCouplingOptions;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 11)]
        public double[] inputRanges;

        public int maxMarkerMask;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 12)]
        public int[] chargeInputRanges;
    }
}
