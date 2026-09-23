using System;

namespace Elsys.TpcAccess.Enumerations {

    /// <summary>Filter module options.</summary>
    [Flags()]
    public enum FilterOptions {
        None = 0,
        /// <summary>
        /// Filter Module type 1 (5 MHz installed)
        /// </summary>
        FilterModule1 = 0x01,

        /// <summary>
        /// Filter Module type 2 (200 kHz installed)
        /// </summary>
        FilterModule2 = 0x03,
    }

}
