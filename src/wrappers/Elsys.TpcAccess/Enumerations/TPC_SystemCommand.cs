namespace Elsys.TpcAccess.Enumerations {
    /// <summary>Constants for use with ExecuteSystemCommand.</summary>
    public enum TPC_SystemCommand {
        ///<summary> Start the measurement.</summary>
        Start = 0,
        ///<summary> Abort the measurement if it is running.</summary>
        Stop,
        ///<summary> Disable triggers.</summary>
        Disarm,
        ///<summary> Enable triggers.</summary>
        Arm,
        ///<summary> Provoke a trigger from software.</summary>
        Trigger,
        ///<summary> Unknown Command.</summary>
        reserved,
        ///<summary> Start measurement with external Trigger.</summary>
        ExtStart,
        ///<summary> Abort measurement with external Trigger.</summary>
        ExtStop,
        ///<summary> Start measurement with a GPS synced device compound.</summary>
        StartGPSSynced,
    }
}
