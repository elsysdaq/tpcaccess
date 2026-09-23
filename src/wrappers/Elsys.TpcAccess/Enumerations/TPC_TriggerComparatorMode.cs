namespace Elsys.TpcAccess.Enumerations {

    public enum TPC_TriggerComparatorMode {
        /// Trigger disabled
		tpc_trgCompOff = 0,

        /// Triggers on positive slope
        tpc_trgCompPositiveSlope = 1,

        /// Triggers on negative slope
        tpc_trgCompNegativeSlope = 2,

        /// Triggers on positive and negative slope
        tpc_trgCompBothSlopes = 3,

        /// Triggers when the signal goes outside a window
        tpc_trgCompWindowOut = 4,

        /// Triggers when the signal goes inside a window
        tpc_trgCompWindowIn = 5,

        /// Triggers when state is outside a window or enables other triggers when is used in a AND trigger group, can only used for trigger comparator 1
        tpc_trgCompStateWindowOut = 6,

        /// Triggers when state is inside a window or enables other triggers when is used in a AND trigger group, can only used for trigger comparator 1
        tpc_trgCompStateWindowIn = 7,
    }

}
