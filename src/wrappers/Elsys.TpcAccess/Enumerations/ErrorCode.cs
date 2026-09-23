namespace Elsys.TpcAccess.Enumerations {
    /* ############ ErrorCode ############
     * 
    Revisit this enum.
        - Should everything be a TransPCException?

    Some of these codes represent a device status:
          - NotYetConnected (after entering it into the system definition -> installed hardware unknown)
          - Connected and ok (-> installed hardware properties (nr of boards) known)
          - Connection lost (-> installed hardware properties still known)
          
          - Calibrating (lasts for a few seconds)
          
          - ServerClientVersionConflict ( -> serious problem)
          - DeviceInitializationFailure ( -> serious problem)
          - HardwareFailure              ( -> serious problem)

          - Transmission error (problem with last soap command. Should that be -> connection lost??)
          - StartError (indicates problem with syncLink -> serious problem)
         * 
    Others represent results for a specific command    
    */

    /// <summary>
    /// Error codes used by the TpcAccess functions.
    /// </summary>
    public enum ErrorCode {
        /// <summary>Code for 'no error'.</summary>
        NoError = 0,

        /// <summary>The device is perfoming an auto calibration and cannot carry 
        /// out the requested operation. </summary>
        Calibrating,


        /// <summary>The network connection to the device is broken.</summary>
        NoConnection,

        /// <summary>The software version of the device does not match this version of TpcAccess.</summary>
        ServerClientVersionConflict,

        /// <summary>The device could not be initialized properly. 
        /// This is probably a problem with the software installation on the device. </summary>
        DeviceInitializationFailure,

        /// <summary>There is a problem with the hardware or the driver installation.
        /// Access the device with a web browser to get more information.</summary>
        HardwareFailure,


        /// <summary>A network error occurred while communicating with the device.</summary>
        TransmissionError,

        /// <summary>The device did not confirm the measurement start.
        /// This indicates a problem with the SyncLink or the StarHub.</summary>
        StartError,


        /// <summary>(reserved) The boards are incompatible for clustering.</summary>
        IncompatibleBoards,


        /// <summary>An invalid device index was passed to a function.</summary>
        InvalidDeviceIx,

        /// <summary>An invalid board address of a non-existent board was passed to a function.</summary>
        InvalidBoardAddress,

        /// <summary>An invalid input number of a non-existent input was passed to a function.</summary>
        InvalidInputNumber,

        /// <summary>An invalid index (out of range) was passed to a function.</summary>
        InvalidIndex,

        /// <summary>The returned string had to be truncated because the given buffer was too 
        /// short. (This error code appears only in the native DLL. The .NET wrapper handles this
        /// autmatically.) </summary>
        BufferTooShort,

        /// <summary>The parameter index is not one of the possible values defined in <see cref="Parameter"/>.</summary>
        InvalidParameter,

        /// <summary>This error is returned for mode parameters on an attempt to set an invalid mode.<br/>
        /// - The number is not an integer or not one of the possible options.<br/>
        /// - The hardware does not have the required option installed.</summary>
        InvalidParameterValue,

        /// <summary>The command index is not one of the possible values defined in <see cref="TPC_SystemCommand"/>.</summary>
        InvalidCommand,


        /// <summary>The command cannot be performed because the system is not in the correct state.
        /// For example, the start command cannot be given if a measurement is already in progress.</summary>
        WrongSystemState,


        /// <summary>
        /// An invalid block number was passed to a function. The specified block
        /// does not exist or has not been recorded yet.
        /// </summary>
        InvalidBlockNumber,

        /// <summary>
        /// Trying to read data from an inactive input. An input is inactive when its mode
        /// is set to 'Off', or when it is used as partner for a differential input.
        /// This error is also returned when trying to read data when no measurement has 
        /// been made after device initialization.
        /// </summary>
        NoData,

        /// <summary>
        /// The data does no longer exist because a new measurement has been started.
        /// This happens when a client is trying to read out data, but has not noticed
        /// at that time that already a new measurement has been started (probably by 
        /// another client), because the update of the system state was delayed in
        /// the network. This problem can be detected because the client still uses the
        /// old measurementNumber parameter. The application should prepare itself for 
        /// using new data from a fresh measurement, get the new measurementNumber from
        /// the device status and use that for retrieving the new data.
        /// </summary>
        NewMeasurement,

        /// <summary>
        /// The configuration file could not be loaded.
        /// </summary>
        LoadSettingFile,

        /// <summary>
        /// The configuration file could not be saved.
        /// </summary>
        WriteSettingFile,


        /// <summary>The feature or function is not implemented yet.</summary>
        NotImplemented,

        /// <summary>An unexpected internal error occurred.</summary>
        InternalError
    }
}
