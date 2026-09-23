namespace Elsys.TpcAccess.Enumerations {
    /// <summary>
    /// Trigger mode.
    /// To be used with <see cref="TriggerSettings.TriggerMode"/>.
    /// </summary>
    public enum TriggerMode {
        /// <summary>Is input disabled.</summary>
        MasterDisable = TriggerModeBits.MasterDisable,

        /// <summary>Level trigger.</summary>
        Level = TriggerModeBits.Basic,

        /// <summary>Detects high slew rates.</summary>
        SlewRate = TriggerModeBits.SlewRate,

        ///<summary> Detects long pulses, Pulse &gt; T.</summary>
        LongPulse = TriggerModeBits.LongPulse,
        ///<summary> Detects short pulses, Pulse &lt; T.</summary>
        ShortPulse = TriggerModeBits.ShortPulse,
        ///<summary> Detects long periods, Period &gt; T.</summary>
        LongPeriod = TriggerModeBits.LongPeriod,
        ///<summary> Detects short periods, Period &lt; T.</summary>
        ShortPeriod = TriggerModeBits.ShortPeriod,

        ////-------
        ///// <summary> Special pulse width timer modes with two independent comparators 
        ///// for the rising and falling edge. Detects long pulses, Pulse &gt; T.</summary>
        //LongPulseTwoComparators = TriggerModeBits.LongPulseTwoComparators,
        ///// <summary> Special pulse width timer modes with two independent comparators 
        ///// for the rising and falling edge. Detects short pulses, Pulse &lt; T.</summary>
        //ShortPulseTwoComparators = TriggerModeBits.ShortPulseTwoComparators,

        ///// <summary>Special mode using two comparators. Either comparator can trigger.</summary>
        //TwoComparatorsOr = TriggerModeBits.TwoComparatorsOr,

        ///// <summary>Special mode using two comparators. 
        ///// To detect a trigger, comparator one must first detect a trigger condition 
        ///// followed by comparator two.</summary>
        //TwoComparatorsAnd = TriggerModeBits.TwoComparatorsAnd,

        /// <summary> Detect if the pulse is between t1 and t2 </summary>
        EqualPulse = TriggerModeBits.EqualPulse,
        /// <summary> Detect if the pulse is outside t1 and t2 </summary>
        NotEqualPulse = TriggerModeBits.NotEqualPulse,
        /// <summary> Detect if the signal from the second channel is shorter then t1  </summary>
        ShortDelay = TriggerModeBits.ShortDelay,
        /// <summary> Detect if the signal from the second channel is longer then t1  </summary>
        LongDelay = TriggerModeBits.LongDelay,
        /// <summary> Detect if the signal from the second channel is between  t1 and t2 </summary>
        EqualDelay = TriggerModeBits.EqualDelay,
        /// <summary> Detect if the signal from the second channel is outside  t2 and t2 </summary>
        NotEqualDelay = TriggerModeBits.NotEqualDelay,

        EqualPeriod = TriggerModeBits.EqualPeriod,
        NotEqualPeriod = TriggerModeBits.NotEqualPeriod,

        NotDefined = TriggerModeBits.Basic
    }
}
