namespace Elsys.TpcAccess.Enumerations {
    /// <summary>
    /// Input modes. 
    /// To be used with <see cref="AmplifierSettings.InputMode"/>.
    /// </summary>
    public enum InputMode {
        /// <summary>The input does not record data.</summary>
        Off = 0,

        /// <summary>Single ended input.</summary>
        SingleEnded = 1,

        /// <summary>Differential input. </summary>
        Differential = 2,
    }
}
