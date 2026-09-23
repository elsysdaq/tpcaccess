using System;

namespace Elsys.TpcAccess.Enumerations {
    /// <summary>Trigger mode used by the native TpcAccess API.</summary>
    /// <remarks>
    /// The trigger mode is composed of the options or modes of
    /// the components of the trigger system. The basic mode
    /// is 0, which is a simple comparator trigger.
    /// The modes for each of the trigger system components
    /// can be or-ed together to build the final trigger mode value.
    /// Note : Not all components can be combined!
    /// <code>
    ///		Possible combinations:
    /// 
    ///		Multiplier   Slew Rate     Timer       2nd Comparator
    ///		---------------------------------------------------------
    ///	        X            1           X                -
    ///	        X            X           1                -
    ///	        X            X           -                1
    ///		
    ///		- : Option not available
    ///		X : Option available
    ///		1/0 : Option used / not used
    /// </code>
    /// </remarks>
    [Flags()]
    public enum TriggerModeBits {
        /// <summary>If this bit is 1, the trigger is off regardless of the other bits. </summary>
        MasterDisable = 0x8000,

        /// <summary>0 is the basic mode (simple comparator trigger).</summary>
        Basic = 0,

        //-- Input multiplier
        /// <summary>Input multiplier on.</summary>
        InputMultiplier = 0x01,

        //-- Slew rate
        /// <summary>Slew rate (feeds d/dt into the comparator).</summary>
        SlewRate = 0x02,

        //-- Timer
        /// <summary> Timer off.</summary>
        TimerOff = 0x00,
        ///<summary> Detects long pulses, Pulse &gt; T.</summary>
        LongPulse = 0x04,
        ///<summary> Detects short pulses, Pulse &lt; T.</summary>
        ShortPulse = 0x08,
        ///<summary> Detects long periods, Period &gt; T.</summary>
        LongPeriod = 0x0C,
        ///<summary> Detects short periods, Period &lt; T.</summary>
        ShortPeriod = 0x10,

        /// Detect pulse between &lt; T &gt;
        NotEqualPulse = 0x24,
        /// Detect pulse between &gt; T &lt;
        EqualPulse = 0x20,

        /// Detect delays between 2 channels &gt; T
        LongDelay = 0x28,
        /// Detect delays between 2 channels &lt; T
        ShortDelay = 0x2C,
        /// Detect delays between 2 channels inside T1 &lt; Delay &lt; T
        EqualDelay = 0x30,
        /// Detect delays between 2 channels outside T1 &gt; Delay &gt; T
        NotEqualDelay = 0x34,
        /// Detect period between 2 channels inside T1 &gt; Period &gt; T
        EqualPeriod = 0x38,
        /// Detect period between 2 channels outside T1 &gt; Period &gt; T
        NotEqualPeriod = 0x3C,

        /// <summary>Mask to filter out timer mode bits</summary>
        TimerModeMask = 0x3C,

        //-------
        /// <summary> Special pulse width timer modes with two independent comparators 
        /// for the rising and falling edge. Detects long pulses, Pulse &gt; T.</summary>
        LongPulseTwoComparators = 0x14,
        /// <summary> Special pulse width timer modes with two independent comparators 
        /// for the rising and falling edge. Detects short pulses, Pulse &lt; T.</summary>
        ShortPulseTwoComparators = 0x18,

        /// <summary>Special mode using two comparators. Either comparator can trigger.</summary>
        TwoComparatorsOr = 0x80,

        /// <summary>Special mode using two comparators. 
        /// To detect a trigger, comparator one must first detect a trigger condition 
        /// followed by comparator two.</summary>
        TwoComparatorsAnd = 0x100,

        /// <summary>Mask to filter out twoComparator mode bits</summary>
        TwoComparatorsMask = 0x180,
    }
}
