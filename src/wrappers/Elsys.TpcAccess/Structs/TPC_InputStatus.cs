using Elsys.TpcAccess.Enumerations;
using System.Runtime.InteropServices;

namespace Elsys.TpcAccess.Structs {

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 4)]
    public struct TPC_InputStatus {
        /// Actual (live) overload status
        [MarshalAs(UnmanagedType.U4)]
        public ADCOverloadState overloadLive;
        /// Overload status during this measurement.
        [MarshalAs(UnmanagedType.U4)]
        public ADCOverloadState overloadMeas;
        /// IEPE Connection Status (TraNET FE 408 only)
        [MarshalAs(UnmanagedType.U4)]
        public TPC_IEPEStatus IEPEStatus;
    }

}
