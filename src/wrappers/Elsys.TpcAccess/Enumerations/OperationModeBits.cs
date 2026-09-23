using System;

namespace Elsys.TpcAccess.Enumerations {
    /// <summary>
    /// Operation modes as used by the native TpcAccess API.
    /// </summary>
    /// <remarks>
    /// The operation mode consists of a basic mode and options.
    /// One of the basic modes must be selected. Options are optional
    /// available in the given basic mode and can be or-ed with the basic mode
    /// to build the final operation mode value.
    /// <code>
    ///     Possible combinations:
    ///
    ///     Basic mode           Dual    Multi channel
    ///     ------------------------------------------
    ///     Scope                  -            -
    ///     Block                  -            -
    ///     Continuous             -            -
    ///     EventRecorder          X            X
    ///
    ///     - : Option not available
    ///     X : Option available
    ///     1/0 : Option used / not used
    /// </code>
    /// </remarks>
    [Flags()]
    internal enum OperationModeBits {
        //-- Basic modes

        /// <summary>Bit mask for the basic mode.</summary>
        BasicModeMask = 0x0F,

        /// <summary>Scope basic mode.</summary>
        Scope = 0,

        /// <summary>Block basic mode.</summary>
        Block = 1,

        /// <summary>Continuous basic mode.</summary>
        Continuous = 2,

        /// <summary>Event recorder basic mode.</summary>
        EventRecorder = 3,

        /// <summary>Block basic mode where Parameter.BlkNumberOfBlocks has no effect (taken as 1).</summary>
        SingleBlock = 4,


        //-- Additional options for some of the basic modes.

        /// <summary>Dual mode option.</summary>
        Dual = 0x040,

        /// <summary>Multi-channel option.</summary>
        MultiChannel = 0x080,
    }
}
