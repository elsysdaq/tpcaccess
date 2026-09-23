namespace Elsys.TpcAccess.Enumerations {

    /// <summary>
    /// Input couplings. 
    /// To be used with <see cref="AmplifierSettings.InputCoupling"/>.
    /// </summary>
    public enum InputCoupling {
        /// <summary>DC coupling.</summary>
        DC = 0,

        /// <summary>AC coupling.</summary>
        AC = 1,

        /// <summary>
        /// ICP coupling (Integrated Current Power, 4 mA constant current power supply for piezo sensors).
        /// </summary>
        Icp = 3,

        /// <summary>DC coupling with 50 Ohm Impedance.</summary>
        DC50 = 4,

        /// <summary>AC coupling with 50 Ohm Impedance.</summary>
        AC50 = 5,

        /// <summary>
        /// Charge input (optional)
        /// </summary>
        Charge = 6,

        /// <summary>
        /// Strain Gauge Input (optional)
        /// </summary>
        Strain = 7,
    }

}
