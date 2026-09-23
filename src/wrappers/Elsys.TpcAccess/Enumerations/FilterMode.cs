namespace Elsys.TpcAccess.Enumerations {
    /// <summary>
    /// Filter modes. 
    /// To be used with <see cref="AmplifierSettings.FilterMode"/>.
    /// </summary>
    public enum FilterMode {
        /// <summary>No filter is used.</summary>
        Off = 0,

        /// <summary>The first RC filter is used.</summary>
        RC1 = 1,

        /// <summary>The second RC filter is used.</summary>
        RC2 = 2,

        /// <summary>The filter module is used. </summary>
        Module = 3
    }
}
