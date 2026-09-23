using System;

namespace Elsys.TpcAccess.Enumerations {
    /// <summary>Operaion mode options.</summary>
    [Flags()]
    public enum OperationModeOptions {
        /// <summary>Event recorder extensions (Retrigger, Stop trigger).</summary>
        EcrExtensions = 0x01
    }
}
