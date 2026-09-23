using System;

namespace Elsys.TpcAccess.Enumerations {
    /// <summary>The InvertMask for channels</summary>
    [Flags]
    public enum InvertMask {
        /// <summary>Inverts the analog data.</summary>
        Analog = 0x01,

        /// <summary>Inverts marker 1.</summary>
        Marker1 = 0x02,

        /// <summary>Inverts marker 2.</summary>
        Marker2 = 0x04
    }
}
