using System;

namespace Elsys.TpcAccess.Enumerations {
    /// <summary>Differential input module options.</summary>
    [Flags()]
    public enum DifferentialInputOptions {
        None = 0,
        /// <summary>Differential input module type 1 is installed.</summary>
        DiffModule1 = 0x01
    }
}
