namespace Elsys.TpcAccess.Enumerations {
    /// <summary>
    /// Trigger comparator modes.
    /// To be used with <see cref="TriggerSettings.ComparatorMode"/>.
    /// </summary>
    public enum ComparatorMode {
        /// <summary>Trigger disabled.</summary>
        Off = 0,

        /// <summary>Triggers on positive slope.</summary>
        PositiveSlope = 1,

        /// <summary>Triggers on negative slope.</summary>
        NegativeSlope = 2,

        /// <summary>Triggers on positive and negative slope.</summary>
        BothSlopes = 3,

        /// <summary>Triggers when the signal goes outside a window.</summary>
        WindowOut = 4,

        /// <summary>Triggers when the signal goes inside a window.</summary>
        WindowIn = 5,

        /// <summary>Enables multiple triggers (AND-Link) when the signal is OUTSIDE of two specified levels.</summary>
        StateWindowOut = 6,

        /// <summary>Enables multiple triggers (AND-Link) when the signal is INSIDE of two specified levels.</summary>
        StateWindowIn = 7,
    }
}
