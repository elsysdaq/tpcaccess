namespace Elsys.TpcAccess.Enumerations {
    /// <summary>
    /// Time base source.
    /// To be used with <see cref="TimeBaseSettings.TimeBaseSource"/>.
    /// </summary>
    public enum TimeBaseSource {
        /// <summary>Time base is generated from the crystal oscillator.</summary>
        Internal = 0,

        /// <summary>Time base is taken from external input.</summary>
        External = 1,

        /// <summary>Time base will be feeded to armed out output. (Example: Is used for external high speed camera as a trigger)</summary>
        ExternalTimebaseOutFreq = 66,
    }
}
