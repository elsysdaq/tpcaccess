namespace Elsys.TpcAccess.Enumerations {
    /// <summary>
    /// Determines the basic time base operation mode.
    /// </summary>
    public enum TimeBaseMode {
        /// <summary>You can choose this otion if no device is available</summary>
        NoTimeBaseMode = -1,

        /// <summary>Scope mode.</summary>
        Scope = OperationModeBits.Scope,

        /// <summary>Block mode.</summary>
        Block = OperationModeBits.Block,

        /// <summary>Block mode where <see cref="TimeBaseSettings.BlkNumberOfBlocks"/> 
        /// has no effect (taken as 1).</summary>
        SingleBlock = OperationModeBits.SingleBlock,

        /// <summary>Continuous mode.</summary>
        Continuous = OperationModeBits.Continuous,

        /// <summary>Event recorder single channel mode. 
        /// Each channel records data individually when a trigger is detected.</summary>
        EcrSingle = OperationModeBits.EventRecorder,

        /// <summary>Event recorder multi channel mode.
        /// All channels record data simultaneously when a trigger is detected.</summary>
        EcrMulti = OperationModeBits.EventRecorder | OperationModeBits.MultiChannel,

        /// <summary>Event recorder single channel dual mode.
        /// Each channel records data individually when a trigger is detected.
        /// Continuous data is also recorded with a lower sample rate.</summary>
        EcrSingleDual = OperationModeBits.EventRecorder | OperationModeBits.Dual,

        /// <summary>Event recorder multi channel dual mode.
        /// All channels record data simultaneously when a trigger is detected.
        /// Continuous data is also recorded with a lower sample rate.</summary>
        EcrMultiDual = OperationModeBits.EventRecorder | OperationModeBits.Dual | OperationModeBits.MultiChannel,
    }
}
