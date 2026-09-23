namespace Elsys.TpcAccess.Enumerations {
    /// <summary>
    /// External trigger input modes.
    /// To be used with <see cref="TimeBaseSettings.ExternalTriggerMode"/>.
    /// </summary>
    public enum ExternalTriggerMode {
        /// <summary>External trigger input does not trigger.</summary>
        Off = 0,

        /// <summary>Triggers on a positive slope.</summary>
        PositiveSlope = 1,

        /// <summary>Triggers on a negative slope.</summary>
        NegativeSlope = 2,
    }
}
