using System;

namespace Elsys.TpcAccess.Enumerations {
    /// <summary>Offset options.</summary>
    [Flags()]
    public enum OffsetOptions {
        None = 0,

        /// <summary>Full offset range (0..100%) instead of just 0% and 50%.</summary>
        OffsetFullRange = 0x01,
    }
}
