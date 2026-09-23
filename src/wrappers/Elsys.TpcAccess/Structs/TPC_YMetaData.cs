using System;
using System.Runtime.InteropServices;

namespace Elsys.TpcAccess.Structs {
    /// <summary>y-Axis meta data.</summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct TPC_YMetaData {
        [MarshalAs(UnmanagedType.I1)]
        public bool inputActive;
        public Int32 analogMask;
        public Int32 markerMask;
        public int numberOfMarkerBits;
        public int resolutionInBits;
        public int bytesPerSample;
        public double binToVoltFactor;
        public double binToVoltConstant;
        public double binToPhysicalFactor;
        public double binToPhysicalConstant;
        public double voltToPhysicalFactor;
        public double voltToPhysicalConstant;
    }
}
