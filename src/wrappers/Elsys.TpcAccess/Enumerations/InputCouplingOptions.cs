using System;

namespace Elsys.TpcAccess.Enumerations {
    /// <summary>Input coupling options.</summary>
    [Flags()]
    public enum InputCouplingOptions {
        None = 0,

        /// <summary>ICP (Integrated Current Power, 4 mA 
        /// constant current power supply for piezo sensors).</summary>
        IcpSource = 0x01,

        /// <summary>50 Ohm input coupling.</summary>
        Imp50Ohm = 0x02,

        /// <summary>
        /// Programmable ICP/IEPE current source which ranges from 4-50 mA
        /// </summary>
        IcpProg = 0x04,

        /// <summary>
        /// Charge module option (pC)
        /// </summary>
        Charge = 0x08,

        /// <summary>
        /// SGA Module option
        /// </summary>
        Sga = 0x10,
    }
}
