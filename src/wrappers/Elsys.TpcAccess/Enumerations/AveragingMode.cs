namespace Elsys.TpcAccess.Enumerations {
    /// <summary>
    /// Averaging modes. 
    /// To be used with <see cref="AmplifierSettings.AveragingMode"/>.
    /// </summary>
    public enum AveragingMode {
        /// <summary>No averaging.</summary>
        Off = 0,

        /// <summary>Averaging with 14 bit result (and 2 marker bits) .</summary>
        Avg14Bit = 1,

        /// <summary>Averaging with 16 bit result (no marker bits) .</summary>
        Avg16Bit = 2,
    }
}
