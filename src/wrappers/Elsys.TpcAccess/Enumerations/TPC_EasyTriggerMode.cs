using System;

namespace Elsys.TpcAccess.Enumerations {
    /// Trigger modes for the \ref TPC_SetTrigger function
    [Flags()]
    public enum TPC_EasyTriggerMode {
        /// No trigger will be detected on this channel
        tpc_etrgOff = 0x8000,

        /// Slope Trigger; Time parameters are ignored
        tpc_etrgSlope = 0,
        /// WindowTrigger; Parameter Level holds one level, Hysteresis holds the other level, time parameters are ignored
        tpc_etrgWindow = 0x0100,
        /// State Trigger; All parameters except level are ignored
        tpc_etrgState = 0x0101,
        /// Slew rate; All parameters except Hysteresis and Time are ignored
        tpc_etrgSlewRate = 0x02,

        /// Detects long pulses, Pulse>T; Time2 parameter is ignored
        tpc_etrgLongPulse = 0x04,
        /// Detects short pulses, Pulse<T; Time2 parameter is ignored
        tpc_etrgShortPulse = 0x08,
        /// Detects long periods, Period>T; Time2 parameter is ignored
        tpc_etrgLongPeriod = 0x0C,
        /// Detects short periods, Period<T; Time2 parameter is ignored
        tpc_etrgShortPeriod = 0x10,
        /// Detect delays between 2 channels > T; Time2 parameter is ignored
        tpc_etrgLongDelay = 0x28,
        /// Detect delays between 2 channels < T; Time2 parameter is ignored
        tpc_etrgShortDelay = 0x2C,

        /// Detects pulses Time2 < Pulse < Time
        tpc_etrgEqualPulse = 0x20,
        /// Detects pulses Time2 > Pulse || Pulse > Time
        tpc_etrgNotEqualPulse = 0x24,
        /// Detect periodes Time2 < Period < Time
        tpc_etrgEqualPeriod = 0x38,
        /// Detect periodes Time2 > Period || Period > Time
        tpc_etrgNotEqualPeriod = 0x3C,
        /// Detect delays between 2 channels Time2 < Delay < Time
        tpc_etrgEqualDelay = 0x30,
        /// Detect delays between 2 channels Time2 > Delay > Time
        tpc_etrgNotEqualDelay = 0x34,
    }
}
