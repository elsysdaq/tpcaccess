using System;

namespace Elsys.TpcAccess.Enumerations {
    /// <summary>Trigger extension options.</summary>
    [Flags()]
    public enum TriggerExtensionOptions {
        /// <summary>Slew rate trigger.</summary>
        SlewRate = 0x01,

        ///// <summary>Reference band trigger.</summary>
        //ReferenceBand = 0x02,

        /// <summary>Power trigger.</summary>
        Power = 0x04
    }
}
