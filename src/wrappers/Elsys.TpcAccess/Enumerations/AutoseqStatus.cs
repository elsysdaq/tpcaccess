namespace Elsys.TpcAccess.Enumerations {
    /// <summary>Autosequence status enumeration.</summary>
    public enum AutoseqStatus {
        /// <summary>No Autosequence File is loaded or present.</summary>
        No_Sequence,

        /// <summary>Autosequence is loaded.</summary>
        Loaded,

        /// <summary>Autosequence load settings.</summary>
        Running_LoadSettings,

        /// <summary>Autosequence measuring is running.</summary>
        Running_Measurement,

        /// <summary>Autosequence wait for end of recording.</summary>
        Running_WEOR,

        /// <summary>Autosequence Delay.</summary>
        Running_Delay,

        /// <summary>Autosequence Calibration.</summary>
        Running_Calibration,

        /// <summary>Autosequence saving.</summary>
        Running_Save,

        /// <summary>Autosequence stopped.</summary>
        Stopped,
    }
}
