namespace Elsys.TpcAccess.Enumerations {

    /// <summary>Trigger state.</summary>
    public enum TriggerState {
        /// <summary>The system is disarmed due to pretrigger delay or a disarm command.</summary>
        Disarmed,

        /// <summary>The system is ready to detect a trigger condition.</summary>
        Armed,

        /// <summary>A trigger has been detected, the block will be finished.</summary>
        Triggered,
    }

}
