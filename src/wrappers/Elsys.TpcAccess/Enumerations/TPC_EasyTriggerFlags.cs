using System;

namespace Elsys.TpcAccess.Enumerations {
    /// Trigger flags for the \ref TPC_SetTrigger function, flags can be combined with the or-operator
    [Flags()]
    public enum TPC_EasyTriggerFlags {
        /// Disables Input Multiplier and all AND trigger assignments
        tpc_etrgNone = 0x00,
        /// Adds Input to AND Group 1
        tpc_etrgANDGroup1 = 0x01,
        /// Adds Input to AND Group 2
        tpc_etrgANDGroup2 = 0x02,
        /// Adds Input to AND Group 3
        tpc_etrgANDGroup3 = 0x04,
        /// Adds Input to AND Group 4
        tpc_etrgANDGroup4 = 0x08,
        /// Input multiplier on
        tpc_etrgInputMultiplier = 0x10,
        /// Show also multiplied signal (use in conjuction with tpc_etrgInputMultiplier)
        tpc_etrgShowMultipliedSignal = 0x20,
        /// Set the master disable trigger flag
        tpc_etrgSetMasterDisable = 0x40,
        /// Only set trigger and comparator modes. Level, Hysteresis and Time Parameters are not changed.
        tpc_etrgIgnoreLevelAndTime = 0x80,
    }
}
