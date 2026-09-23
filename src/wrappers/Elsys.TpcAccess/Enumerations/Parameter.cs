using System;

namespace Elsys.TpcAccess.Enumerations {
    /// <summary>
    /// Recording parameter.
    /// </summary>
    /// <remarks>
    /// <para>
    /// 'Parameter' is just a collective name for settings in the form of double numbers that 
    /// are assigned with a single input (channel parameters, e.g. input voltage range), or 
    /// a cluster of the system (timebase parameters, e.g. sample rate).
    /// </para>
    /// <para>
    /// There are two kinds of parameters: Mode parameters and value parameters.
    /// Mode parameters set a certain mode of operation, e.g. the trigger mode or the
    /// filter mode. They use integer constants to specify the mode. Value parameters 
    /// set a numerical value like the sample rate or the input range. The main difference
    /// between mode and value parameters is in the error handling when trying to set
    /// invalid values for the parameter. Mode parameters return an error if a invalid 
    /// number is given. Value parameters just restrict the value to the closest possible 
    /// one and no error is returned. 
    /// </para>
    /// </remarks>
    public enum Parameter {
        //=== Timebase parameters, apply per cluster ===

        //--- Common parameters ---

        /// <summary>Block, Continuous, Event Recorder, etc.</summary>
        OperationMode = 1,

        /// <summary>Channel Multiplexer, 4 or 8.</summary>
        /// <remarks>
        /// Please note: In mux 8 mode the maximum sample rate, maximum block length and 
        /// maximum number of blocks are limited to half of the values in mux 4.
        /// If the sample frequency, block length or number of blocks was set to the
        /// maximum when switching from mux 4 to mux 8, these parameters will be changed.
        /// </remarks>
        MultiplexerMode = 2,


        /// <summary>Sample rate clock source: Internal or External. See <see cref="Devices.TimeBaseSource"/>.</summary>
        TimebaseSource = 3,

        /// <summary>Sampling frequency in Hertz. In effect when TimebaseSource = internal.
        /// Valid values from 1 Hz to the maximum speed of the board or cluster.</summary>
        SamplingFrequency = 4,

        /// <summary>Divisor for external clock input. In effect when TimebaseSource = External.
        /// Valid values: 1..65535</summary>
        ExternalClockDivisor = 5,


        /// <summary>External trigger input: Off, PositiveSlope, NegativeSlope. See <see cref="Devices.ExternalTriggerMode"/>.</summary>
        ExternalTriggerMode = 6,

        /// <summary>
        /// Sampling frequency in Hertz that is applied to the external clock input 
        /// when measuring with external clock. In effect when TimebaseSource = external.
        /// Valid values >= 0.
        /// </summary>
        ExternalClockSamplingFrequency = 46,

        /// <summary>
        /// External Timebase frequency which can be used for a external device as a synchronisation
        /// signal. (Example: High Speed Camera)
        /// </summary>
        ExternalTimebaseOutFrequency = 66,

        //--- Parameters for block mode ---

        /// <summary>Number of blocks to record. Valid values from 1 to the memory size
        /// of the board or the cluster divided by the block length.</summary>
        BlkNumberOfBlocks = 7,

        /// <summary>Block length in samples. Valid values are powers of two from 1024 up to the 
        /// memory size of the board or the cluster.</summary>
        BlkBlockLength = 8,

        /// <summary>Trigger delay in percent of the block length. Valid values from -100 to +200%.</summary>
        BlkTriggerDelay = 9,

        //--- Parameters for Continuous Mode ---

        /// <summary>Limits the maximum amount of data in samples to be recorded. 
        /// Valid values from 1 to 1e18.</summary>
        ContMaximumDataLength = 10,

        /// <summary>Number of samples to record after a stop trigger. 
        /// In effect when the stop trigger option for Continuous mode is used.
        /// Valid values from 1 up to the memory size of the board or the cluster.</summary>
        ContStopTrailer = 11,

        /// <summary>
        /// Enable stop trigger in continuous mode.  Valid values: 0: disabled, 1: enabled
        /// </summary>
        ContEnableStopTrigger = 47,

        /// <summary>
        /// Enable recording limit for continuous mode.  Valid values: 0: disabled, 1: enabled
        /// </summary>
        ContEnableRecLengthLimit = 48,


        //--- Parameters for scope mode ---

        /// <summary>Auto trigger for scope mode. 
        /// If this value is 0, waits until the signal triggers.
        /// If this value is 1, automatically fires a trigger after 
        /// some time if the signal does not trigger.</summary>
        ScopeAutoTrigger = 43,


        /// <summary>Single shot mode for scope mode. 
        /// If this value is 0, acquisitions are continuously restarted.
        /// If this value is 1, stops after each acquisition.</summary>
        ScopeSingleShot = 44,

        /// <summary>
        /// Block length in samples. Valid values are powers of two from 1024 up to 
        /// half of the memory size of the board.
        /// </summary>
        ScopeBlockLength = 49,

        /// <summary>
        /// Trigger delay in percent of the block length. Valid values from -100 to +200%.
        /// </summary>
        ScopeTriggerDelay = 50,

        //--- Parameters for Event recorder --- 

        /// <summary>Dual mode for ECR single and multi. 
        /// If this value is 0, Dual Mode is disabled.
        /// If this value is 1, Dual Mode is enabled.</summary>
        EcrDualMode = 60,

        /// <summary>Block length in samples. Valid values &gt;= 0.
        /// Note: If the retrigger option is active, the actually recorded block 
        /// can be longer than this.
        /// Valid values from 1 up to the memory size of the board or the cluster.</summary>
        [Obsolete("Use EcrPre- and PostTrigger instead.")]
        EcrMinimumBlockLength = 12,

        /// <summary>Trigger delay in percent of the block length. Valid values from -100 to 0%.</summary>
        [Obsolete("Use EcrPre- and PostTrigger instead.")]
        EcrTriggerDelay = 13,

        /// <summary> ECR Pretrigger in samples. Valid values >= 0. </summary>
        EcrPreTrigger = 56,

        /// <summary> ECR Posttrigger in Samples. Valid values >= 0. </summary>
        EcrPostTrigger = 57,

        /// <summary>Limits the number of blocks to record. Valid values from 1 to 2e9.</summary>
        EcrMaximumNumberOfBlocks = 14,

        /// <summary>Hold off time in samples.
        /// Valid values from 1 to 1e18.</summary>
        EcrHoldOffTime = 15,

        /// <summary>Limits the length of a block. 
        /// In effect when the retrigger option for Event recorder mode is used.
        /// Valid values from 1 to 1e18.</summary>
        EcrMaximumBlockLength = 16,

        /// <summary>Retrigger time in samples.
        /// In effect when the retrigger option for Event recorder mode is used.
        /// Valid values from 1 to 1e18.</summary>
        EcrRetriggerTime = 17,

        /// <summary>Number of samples to record after a stop trigger. Valid values &gt;= 0. 
        /// In effect when the stop trigger option for Event recorder mode is used.
        /// Valid values from 1 to 1e18.</summary>
        EcrTrailer = 18,

        /// <summary>Divisor for the sample rate of the continuous recording in dual mode. Valid values &gt;= 1.
        /// In effect when the dual mode option for Event recorder mode is used.
        /// Valid values from 1 to 2e9.</summary>
        EcrClockDivisor = 19,


        /// <summary>
        /// Enable hold off.  Valid values: 0: disabled, 1: enabled
        /// </summary>
        EcrEnableHoldOff = 51,

        /// <summary>
        /// Enable trailer in dual mode.  Valid values: 0: disabled, 1: enabled
        /// </summary>
        EcrDualEnableTrailer = 52,

        /// <summary>
        /// Number of samples to record after the trigger of the last ECR block. Valid values >= 0. 
        /// </summary>
        EcrDualTrailer = 53,

        /// <summary>
        /// Enable recording limit in dual mode.  Valid values: 0: disabled, 1: enabled
        /// </summary>
        EcrDualEnableRecLengthLimit = 54,

        /// <summary>
        /// Limits the maximum amount of data in samples to be recorded in dual mode. Valid values >= 1 
        /// </summary>
        EcrDualMaxRecLength = 55,


        /// <summary>
        /// Enable stop trigger in ECR mode.  Valid values: 0: disabled, 1: enabled
        /// </summary>
        EcrEnableStopTrigger = 58,

        /// <summary>
        /// Enable retrigger in ECR mode.  Valid values: 0: disabled, 1: enabled
        /// </summary>
        EcrEnableRetrigger = 59,


        //=== Input parameters, apply per input channel ===

        //--- General ---

        /// <summary>Input mode: Off, Single ended, Differential. See <see cref="Devices.InputMode"/>.</summary>
        InputMode = 20,

        /// <summary>Option to disable data recording for the channel in ECR and Continuous mode. 
        /// Valid values: 0:channel produces data, 1:channel does not produce 
        /// data, but can still cause a trigger.</summary>
        TriggerOnly = 21,


        //--- Input range

        /// <summary>Input coupling: DC, AC, ICP. See <see cref="Devices.InputCoupling"/></summary>
        InputCoupling = 22,

        /// <summary>Input voltage range in Volt. Valid values: 0.1, 0.2, 0.5, 1, 2, 5, 10, 20, 50, 100.</summary>
        Range = 23,

        /// <summary>Input range offset in percent. Valid values from 0 to 100%.</summary>
        Offset = 24,

        /// <summary>Invert input range. Valid values: 0: Off, no inversion, 1: On, invert</summary>
        Invert = 25,


        //--- Filter

        /// <summary>Filter mode: Off, 22kHz RC filter, 200kHz RC filter, Filter Module. See <see cref="Devices.FilterMode"/>.</summary>
        FilterMode = 26,

        /// <summary>Filter frequency in Hertz. Valid values: A set of frequencies between 200Hz and 5MHz.
        /// In effect when filterMode = FilterMode.Module.</summary>
        FilterFreq = 27,

        /// <summary>Input averaging: Off, 14bit, 16bit. See <see cref="Devices.AveragingMode"/>.
        /// Please note: This option cannot be selected individually for each input.
        /// The setting applies to all inputs of a board.</summary>
        AveragingMode = 28,


        //--- Physical unit

        /// <summary>Factor for the conversion from volt to user selected physical unit. 
        /// value in physical unit = value in volt * factor + constant.</summary>
        PhysFactor = 29,

        /// <summary>Constant for the conversion from volt to user selected physical unit. 
        /// value in physical unit = value in volt * factor + constant.</summary>
        PhysConstant = 30,


        //--- Trigger parameters

        /// <summary>Trigger mode.
        /// See enum <see cref="Devices.TriggerModeBits"/>.</summary>
        TrgMode = 31,

        /// <summary>Trigger comparator mode:  Off, PositiveSlope, NegativeSlope, BothSlopes, WindowOut, WindowIn. 
        /// See <see cref="Devices.ComparatorMode"/>.
        /// 'Off' disables the trigger for this channel.</summary>
        TrgComparatorMode = 32,

        /// <summary>First trigger level in percent of full input range. 
        /// Valid values are from -100 to +100%.</summary>
        TrgLevelA = 33,

        /// <summary>Second trigger level in percent of full input range. 
        /// Valid values are from -100 to +100%.</summary>
        TrgLevelB = 34,


        //--- Additional trigger parameters, depending on trigger mode.

        /// <summary>Timeout for trigger timer in samples (t2). Valid values from 1 to 65535.</summary>
        TrgTimerTSamples = 35,

        /// <summary>Timeout for t1 trigger timer in samples. Valid values from 1 to 65535.</summary>
        TrgTimerT1Samples = 64,

        /// <summary>Delta-T in samples for the differentiator. Valid values from 1 to 1024.</summary>
        TrgSlewRateDTSamples = 36,

        /// <summary>Trigger comparator mode:  PositiveSlope, NegativeSlope, BothSlopes. 
        /// See <see cref="Devices.ComparatorMode"/>.
        /// Comparator Mode for Slew Rate Trigger Mode.</summary>
        TrgSlewRateComparatorMode = 61,

        /// <summary>Trigger mode for the second comparator: Off, PositiveSlope, NegativeSlope, BothSlopes, WindowOut, WindowIn. 
        /// See <see cref="Devices.ComparatorMode"/>.</summary>
        TrgComparator2Mode = 37,

        /// <summary>Trigger level for the second comarator in percent of full input range. 
        /// Valid values are from -100 to +100%.</summary>
        TrgLevel2A = 38,

        /// <summary>Trigger level for the second comarator in percent of full input range. 
        /// Valid values are from -100 to +100%.</summary>
        TrgLevel2B = 39,


        /// <summary>Hysteresis for slew rate trigger.</summary>
        TrgSlewRateHysteresis = 45,

        /// <summary>Show also the multiplied signal (only if trigger multiplier is used (see \ref tpc_trgInputMultiplier))</summary>
        TrgShowProduct = 62,

        /// <summary>
        /// Set the AND trigger mask for configuring the AND trigger groups. 
        /// Each of the 4 or 8 board inputs can be assigned to one or several AND trigger groups. 
        /// Trigger events from these inputs are linked together and were forwarded once all triggers of 
        /// the assigned group inputs have triggered. Trigger signals coming from the AND trigger groups 
        /// are linked together by an OR logic. The ANDTriggerMask is a 32 bit variable. Bit 7 to 0 configures
        /// group 1, Bit 15 to 8 configures group 2, bit 23 to 16 configures group 3 and bit 31 to 24 configures 
        /// group 4. A channel is assigned to a group by setting a the corresponding bit to 1. 
        /// Example: 0x0000030C is assigning channel 1 and 2 to group 2 and channel 3 and 4 to group 1.
        /// </summary>
        TrgANDMask = 63,


        //--- Stop trigger in Dual mode

        /// <summary>Comparator mode for stop trigger:  Off, PositiveSlope, NegativeSlope, BothSlopes, WindowOut, WindowIn. 
        /// See <see cref="Devices.ComparatorMode"/>.
        /// 'Off' disables the stop trigger for this channel.
        /// In effect when the stop trigger option for Event recorder mode is used.</summary>
        DualModeStopTrgComparatorMode = 40,

        /// <summary>First level for the stop trigger in percent of full input range. 
        /// Valid values are from -100 to +100%.
        /// In effect when the stop trigger option for Event recorder mode is used 
        /// and the trigger comparator mode is not 'Off'.</summary>
        DualModeStopTrgLevelA = 41,

        /// <summary>Second level for the stop trigger in percent of full input range. 
        /// Valid values are from -100 to +100%.
        /// In effect when the stop trigger option for Event recorder mode is used 
        /// and the trigger comparator mode is not 'Off'.</summary>
        DualModeStopTrgLevelB = 42,

        /// <summary>
        /// Flag to reset the overload state of one single input.
        /// </summary>
        ResetOverload = 65,

        /// <summary>
        /// Flag to enable (1) or disable (0) GPS synchronization between a device compound over the network.
        /// </summary>
        EnableGPSSync = 67,

        /// <summary>
        /// Flag to configure the LAN trigger settings.
        /// - Generate LAN Trigger 0x1
        /// - Receive LAN Trigger 0x2
        /// - Generate and Receive LAN Trigger 0x3
        /// </summary>
        LXIMsgConfig = 68,

        /// <summary>
        /// Enables trigger mask for digital positive slope trigger.
        /// </summary>
        TrgDigPosSlope = 69,

        /// <summary>
        /// Enables trigger mask for digital negative slope trigger.
        /// </summary>
        TrgDigNegSlope = 70,

        /// <summary>
        /// Enable Pulse Width Trigger on digital channel, one per 16 Bit available
        /// </summary>
        TrgDigPls = 73,

        /// GPS Logging Mode
        GPSMode = 74,

        /// GPS Log Intervale
        GPSTimer = 75,

        ///Set Sync Ping Mode on/off
        SyncPingMode = 76,

        /// Set SyncLink 2 Delay
        SyncDelay = 77,

        /// <summary>
        /// Programmable ICP/IEPE current (4-50 mA)
        /// </summary>
        ICPCurrent = 78,

        /// <summary>
        /// Set Charge Amplifier Mode 0 = Measure, 1 = Reset
        /// </summary>
        ChargeMode = 79,

        /// <summary>
        /// Strain Excitation voltage
        /// </summary>
        StrainExcitationVoltage = 80,

        /// <summary>
        /// Quarter or Half-Bridge
        /// </summary>
        StrainHalfQuarter = 81,

        /// <summary>
        /// 4 or 6 wire measurement
        /// </summary>
        StrainExcitationVoltageSensse = 82,

        /// <summary>
        /// Start auto zero process
        /// </summary>
        StrainAutoZero = 83,

    }
}
