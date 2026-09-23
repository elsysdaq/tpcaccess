using System;

namespace Elsys.TpcAccess.Enumerations {
    /// <summary>ADC Input Overload State</summary>
    [Flags]
    public enum ADCOverloadState {
        /// <summary>Input signal is inside the input range.</summary>
        None = 0x00,

        /// <summary>Input signal is above the upper input limit.</summary>
        Positive = 0x01,

        /// <summary>Input signal is below the lower input limit.</summary>
        Negative = 0x02,
    }
}
