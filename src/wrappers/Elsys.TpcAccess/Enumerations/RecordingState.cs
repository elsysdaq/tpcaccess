namespace Elsys.TpcAccess.Enumerations {
    /// <summary>Recording state enumeration.</summary>
    public enum RecordingState {
        /// <summary>The measurement was aborted with a stop command.</summary>
        Aborted,

        /// <summary>The device has been prepared for a new start command.</summary>
        Starting,

        /// <summary>The recording is in progress.</summary>
        Recording,

        /// <summary>The measurement has finished normally.</summary>
        Stopped,

        /// <summary>The device did not start properly (problem with SyncLink or StarHub).</summary>
        StartError
    }
}
