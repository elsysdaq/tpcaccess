//---------------------------------------------------------------------------
/*
 *
 * ELSYS EXPRESSLY DISCLAIMS ALL WARRANITIES OR REPRESENTATIONS WITH RESPECT TO
 * THIS SOFTWARE AND DOCUMENTATION AND ANY SUPPORT OR MAINTENANCE SERVICES THAT
 * ELSYS MAY CHOOSE TO PROVIDE IN RELATION THERETO (INCLUDING, WITHOUT
 * LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE OR THAT THE SOFTWARE: WILL BE
 * ERROR-FREE, WILL OPERATE WITHOUT INTERUPTION, WILL NOT INFRINGE THE RIGHTS OF A
 * THIRD PARTY, OR WILL PRODUCE RESULTS IN CONNECTION WITH ITS USE). FURTHER,
 * ELSYS EXPRESSLY DISCLAIMS ALL EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * (C) Copyright 2005 - 2019 Elsys AG. All rights reserved.
*/

//---------------------------------------------------------------------------
/*--------------------------------------------------------------------------------
  $Id: TpcAccess.h 363 2018-01-15 14:43:51Z roman $
  TransPC TPCX API.
--------------------------------------------------------------------------------*/
#ifndef TpcAccess_h
#define TpcAccess_h TpcAccess_h
//---------------------------------------------------------------------------------

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "intTypes.h"

//---------------------------------------------------------------------------------


// Calling convention
#ifdef WIN32
#define TPC_CC __stdcall
#else
#define TPC_CC
#endif


// Export/Import qualifier 
#ifdef WIN32
	#ifdef BUILDING_TPC_ACCESS
		#define TPC_EXP __declspec(dllexport)
	#else
		#define TPC_EXP __declspec(dllimport)
	#endif
#else
#define TPC_EXP
#endif


// declare C linkage for the exported functions if in C++
#ifdef __cplusplus
extern "C" {
#endif


	//======================================================================================

	/** 
	\mainpage TpcAccess API Documentation
	
	\section intro_sec Introduction
	This documentation describes the application programming interface (API) for controling and 
	configuring a TransPC system and read out their measurement data.  A TransPC system has a server/client architecture where the 
	host holding the hardware (TPCX PCI cards) represents the server and the host on which runs the
	user application represents the client. The communication between the server and client is TCP/IP based, so it is
	possible to have the server and client software on different hosts which are connected over the 
	network. 
	
	This API is the interface to the user application and runs on the client side. All parameters and settings
	are managed on the server side. When different clients where connected to one server all clients get synchronized.     
	
	When a TransPC system consist on several servers they must be connected to a SyncLink for synchronization.
	
	\section changelog API and Documentation Change Log

	\subsection version124 Version 1.24
	<table>
		<tr>
			<th><b>API Version (DLL)</b></th>
			<th><b>Date</b></td>
		</tr>
		<tr>
			<td>1.1.3.24</td>
			<td>2014-12-29</td>
		</tr>
	</table>
	SyncLink Detection added to \ref TPC_DeviceInfo and unsynced device operation added. 

	\subsection version116 Version 1.16
	<table>
		<tr>
			<th><b>API Version (DLL)</b></th>
			<th><b>Date</b></td>
		</tr>
		<tr>
			<td>1.1.3.23</td>
			<td>2014-03-12</td>
		</tr>
	</table>
	Supported input ranges added in \ref TPC_InputInfo structure

	\subsection version115 Version 1.15
	<table>
		<tr>
			<th><b>API Version (DLL)</b></th>
			<th><b>Date</b></td>
		</tr>
		<tr>
			<td>1.1.2.19</td>
			<td>2013-10-28</td>
		</tr>
	</table>
	New parameter and commands added for GPS synchronization and LXI Trigger message generation.
	- \ref tpc_parGPSSync and \ref tpc_parLXIMsgConfig added
	- \ref TPC_MakeMeasurement added
	- \ref tpc_cmdStartGPSSynced command added

	\subsection version114 Version 1.14
	<table>
		<tr>
			<th><b>API Version (DLL)</b></th>
			<th><b>Date</b></td>
		</tr>
		<tr>
			<td>1.1.2.14</td>
			<td>2013-03-22</td>
		</tr>
	</table>
	- \ref TPC_SetTrigger and \ref TPC_GetTrigger added
	- \ref TPC_MakeMeasurement added

	\subsection version113 Version 1.13
	<table>
		<tr>
			<th><b>API Version (DLL)</b></th>
			<th><b>Date</b></td>
		</tr>
		<tr>
			<td>1.1.1.13</td>
			<td>2012-08-07</td>
		</tr>
	</table>
	- Bugfix Load and Write settings with files from TransAS
	- BugFix External Timebase in TMetaData

	\subsection version112 Version 1.12
	<table>
		<tr>
			<th><b>API Version (DLL)</b></th>
			<th><b>Date</b></td>
		</tr>
		<tr>
			<td>1.1.1.12</td>
			<td>2011-08-11</td>
		</tr>
	</table>

	- New API Version request function \ref TPC_GetVersion added.
	- New parameter: Pulse Trigger >< and <> \ref tpc_parTrgPreTimerTSamples 
	- New Trigger Feature added: \ref tpc_trgEqualPeriod and \ref tpc_trgNotEqualPeriod
	- New parameter: ResetOverflow: \ref tpc_parResetOverflow
	- New parameter: External Timebase output frequency: \ref tpc_parExternalTimebaseOutFreq
	- Set parameter bugfix after reset command. 

	\subsection version111 Version 1.11
	<table>
		<tr>
			<th><b>API Version (DLL)</b></th>
			<th><b>Date</b></td>
		</tr>
		<tr>
			<td>1.1.1.5 & 1.1.1.6</td>
			<td>2010-09-27</td>
		</tr>
	</table>
	- 50 Ohm input coupling for TPCX-2 cards added, see \ref TPC_InputCoupling and \ref TPC_InputCouplingOptions
	- Input Overload Status added, see \ref TPC_BoardStatus and \ref TPC_InputStatus. 
      
      Important: If you use the TpcAccess.dll without header inport (As in LabView for example), you must
	  update your code for reading correctly the \ref TPC_DeviceStatus structure. As the size of TPC_BoardStatus
	  has changed, the operationModes, TPC_AutoSeq_Status etc. has new positions in the structure.
    - Marker invertion added. Each marker bit can be inverted seperatly by using the allready existing \ref tpc_parInvert 
	  parameter. The invertion mode is set by a bit mask, see \ref  TPC_InverterModeMask .

	\subsection version110 Version 1.10
	<table>
		<tr>
			<th><b>API Version (DLL)</b></th>
			<th><b>Date</b></td>
		</tr>
		<tr>
			<td>1.1.1.4</td>
			<td>2010-01-18</td>
		</tr>
	</table>
	- new parameter \ref tpc_parANDTriggerMask added for writing and reading the AND trigger mask settings.
	- new trigger comparator mode added: \ref tpc_trgCompStateWindowOut and \ref tpc_trgCompStateWindowIn
	
	\subsection version19 Version 1.9
	<table>
		<tr>
			<th><b>API Version (DLL)</b></th>
			<th><b>Date</b></td>
		</tr>
		<tr>
			<td>1.1.1.3</td>
			<td>2009-10-07</td>
		</tr>
	</table>
	- \ref TPC_DeviceInfo enhanced with ModelType number 
	
	\subsection version18 Version 1.8
	<table>
		<tr>
			<th><b>API Version (DLL)</b></th>
			<th><b>Date</b></td>
		</tr>
		<tr>
			<td>1.1.1.2</td>
			<td>2009-09-02</td>
		</tr>
	</table>
	- \ref TPC_GetFreeDiskSpace added
	- \ref TPC_SetDeviceSettings added
	- \ref TPC_DeviceInfo enhanced
	
	
	\subsection version17 Version 1.7
	<table>
		<tr>
			<th><b>API Version (DLL)</b></th>
			<th><b>Date</b></td>
		</tr>
		<tr>
			<td>1.1.1.1</td>
			<td>2009-07-08</td>
		</tr>
	</table>
	- \ref TPC_AddDeviceEx and \ref TPC_AddDeviceSystemEx added for specifing user specific timeout values. 
	
	\subsection version16 Version 1.6
	<table>
		<tr>
			<th><b>API Version (DLL)</b></th>
			<th><b>Date</b></th>
		</tr>
		<tr>
			<td>1.1.1.0</td>
			<td>2009-04-23</td>
		</tr>
	</table>
	- Time Meta can be readout for several blocs at once.
	
	\subsection version15 Version 1.5
	<table>
		<tr>
			<th><b>API Version (DLL)</b></th>
			<th><b>Date</b></th>
		</tr>
		<tr>
			<td>1.1.0.0</td>
			<td>2009-01-08</td>
		</tr>
	</table>	
	- ExtIO Start Record functionality added. New DeviceState Variable extStartState added, new SystemCommand
	\ref tpc_cmdArmExtCommands and  \ref tpc_cmdDisarmExtCommands for activating the external start signal. 
	
	- New Library gSoap 2.7.12 and Xerces 3.0.0.0 implemented
	
	
	\section api_sec The TpcAccess API
	The API is divided into modules, described here approximately 
	in the order as you might need them in a custom program. The term <i>device</i> has the same 
	meaning as the term <i>server</i> in the description above. 
	
	\ref Devices \n
	Setting up the system definition is done as the first step before 
	working with the system. The system definition is a list of devices that belong to
	the system and take part in the common measurement.  All devices should be connected to 
	the network and should also be interconnected via the SyncLink. Once the system definition is completed, TpcAccess
	will try to establish a connection to the devices and will query their properties and status, and will forward your
	commands to them. 
	
	\ref DeviceInfo \n
	Once the system definition is set up, TpcAccess will try to connect
	to each of the devices and get their information. With the functions in 
	this section you can query the properties of the hardware that was 
	found on the devices.
	
	\ref Configuration\n
	With the functions in this section you can set various recording 
	parameters for the next measurement, like the sample rate or input
	voltage range.
	
	\ref Cluster\n
	If there are several TPCX modules (boards) in the system it usefull to group them together (forming clusters)
	which simplifies the configuration. 
	
	
	\ref SystemCommands\n
	With the functions in this section you can send commands per system, e.g.
	start and stop measurements.
	
	\ref DeviceCommands\n
	With the functions in this section you can send commands per device, e.g.
	start calibration.
	
	\ref DeviceStatus\n
	Using the functions in this section you can query the status
	of the devices. This includes things like for example whether 
	a signal has already triggered, or the number of blocks that 
	the devices have recorded so far. Since the TransPC system is 
	capable of supporting multiple clients, observing clients can 
	detect changes caused by other clients that operate on the system.
	
	\ref DataReadout\n
	These are the functions that you need to read out the data 
	after a measurement has completed.
	
	\ref SettingFiles\n
	Functions to store the configuration to a file and load it again.
	
	
	
	\ref Misc\n
	Error codes, API Version, etc.
	*/

	/**
	 * \defgroup Devices System Definition
	 * \defgroup DeviceInfo Device Information
	 * \defgroup Configuration Configuration
	 * \defgroup Cluster Cluster Configuration
	 * \defgroup SystemCommands System Commands
	 * \defgroup DeviceCommands Device Commands
	 * \defgroup DeviceStatus Device Status
	 * \defgroup DataReadout Data Readout 
	 * \defgroup SettingFiles Setting Files 
	 * \defgroup Misc Miscellaneous
	 */

	//======================================================================================

	/**
	 * \addtogroup Misc
	 *
	 * @{
	 */

	/// Error codes returned by the functions.
	enum TPC_ErrorCode {
		/// Code for 'no error'
		tpc_noError = 0,

		/// The device is perfoming an auto calibration and cannot carry 
		/// out the requested operation.
		tpc_errCalibrating,


		/// The network connection to the device is broken.
		tpc_errNoConnection,

		/// The software version of the device does not match this version of TpcAccess.
		tpc_errServerClientVersionConflict,

		/// The device could not be initialized properly. 
		/// This is probably a problem with the software installation on the device.
		tpc_errDeviceInitializationFailure,

		/// There is a problem with the hardware or the driver installation.
		/// Access the device with a web browser to get more information.
		tpc_errHardwareFailure,


		/// A network error occurred while communicating with the device.
		tpc_errTransmissionError,

		/// The device did not confirm the measurement start.
		/// This indicates a problem with the SyncLink or the StarHub.
		tpc_errStartError,


		/// (reserved) The boards are incompatible for clustering.
		tpc_errIncompatibleBoards,


		/// An invalid device index was passed to a function.
		tpc_errInvalidDeviceIx,

		/// An invalid board address of a non-existent board was passed to a function.
		tpc_errInvalidBoardAddress,
	
		/// An invalid input number of a non-existent input was passed to a function.
		tpc_errInvalidInputNumber,

		/// An invalid index (out of range) was passed to a function.
		tpc_errInvalidIndex,

		/// The returned string had to be truncated because the given buffer was too short.
		tpc_errBufferTooShort,

		/// The parameter index is not one of the possible values defined in enum TPC_Parameter.
		tpc_errInvalidParameter,

		/// This error is returned for mode parameters on an attempt to set an invalid mode.
		/// - The number is not an integer or not one of the possible options.
		/// - The hardware does not have the required option installed.
		tpc_errInvalidParameterValue,

		/// The command index is not one of the possible values defined in enum TPC_Command.
		tpc_errInvalidCommand,


		/// The command cannot be performed because the system is not in the correct state.
		/// For example, the start command cannot be given if a measurement is already in progress.
		tpc_errWrongSystemState,


		/// An invalid block number was passed to a function. The specified block
		/// does not exist or has not been recorded yet.
		tpc_errInvalidBlockNumber,

		/// Trying to read data from an inactive input. 
		/// An input is inactive when its mode
		/// is set to 'Off', or when it is used as partner for a differential input.
		/// This error is also returned when trying to read data when no measurement has 
		/// been made after device initialization.
		tpc_errNoData,

		/// The data does no longer exist because a new measurement has been started.
		/// This happens when a client is trying to read out data, but has not noticed
		/// at that time that already a new measurement has been started (probably by 
		/// another client), because the update of the system state was delayed in
		/// the network. This problem can be detected because the client still uses the
		/// old measurementNumber parameter. The application should prepare itself for 
		/// using new data from a fresh measurement, get the new measurementNumber via 
		/// TPC_GetDeviceStatus() and use that for retrieving the new data.
		tpc_errNewMeasurement,

		/// The configuration file could not be loaded.
		tpc_errLoadSettingFile,

		/// The configuration file could not be saved.
		tpc_errWriteSettingFile,

		/// The feature or function is not implemented yet.
		tpc_errNotImplemented,

		/// An unexpected internal error occurred.
		tpc_errInternalError,

		/// Auto Sequence Error
		tpc_errAutoSequence,

		tpc_noHardwarePresent,
		
		/// The action could not be finished within the specified time.
		tpc_errTimeout,

		/// The measurement was aborted due to an error.
		tpc_errMeasurementAborted
	};



	/// Convert an error code into a readable string.
	/** \param errorCode The error number as returned by any function from this API.
	*  \param errorString A buffer that receives the error string.
	*  \param maxLen The length of the buffer.
	*  \returns 
	* - \ref tpc_noError if successful.
	* - \ref tpc_errBufferTooShort if the string was truncated because the buffer was too short.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ErrorToString(int errorCode, char *errorString, int maxLen);

	/*@}*/



	/**
	 * \addtogroup Misc 
	 *
	 * @{
	 */


	/// Maximum number of boards in a device. 
#define tpc_maxBoards 16

	/// Maximum number of inputs on a board. 
#define tpc_maxInputs  8

	/// Max path length for bdf file path
#define tpc_maxPathLength 128

	/// Max number of input ranges
#define tpc_maxInputRanges 11

	/// Get the TpcAccess API version number. 
	/** This function is obsolete, please use \ref TPC_GetVersion 
	*  
	*/
	TPC_EXP int TPC_CC TPC_GetApiVersion();
	
	/// Version Structure
	struct TPC_Version{
		/// Major Version		
		int major;		
		/// Minor Version
		int	minor;
		/// Build Number
		int build;
		/// Revision Number
		int revision;
	};

	/// Get the TpcAccess API version struct
	/** 
	* \returns \ref TPC_Version API Version
	*/
	TPC_EXP TPC_Version TPC_CC TPC_GetVersion();

	/// Used to store absolute time values.
	struct TPC_DateTime {
		int year;           ///< The year
		int month;          ///< The month
		int day;            ///< The day
		int hour;           ///< The hours
		int minute;         ///< The minutes
		int second;         ///< The seconds
		int milliSecond;    ///< The milli seconds
	};

	/*@}*/

	//=== Devices ==========================================================================

	/**
	 * \addtogroup Devices
	 * A TransPC system is a group of TransPC devices that perform a measurement togehter. 
	 * Each device has a unique URL that is used to connect to it over the network. 
	 * 
	 * TpcAccess stores a list of urls to devices that belong to the system. 
	 * It tries to connect to them or reestablish connection if lost.
	 *
	 * Prior to working with the system, the list of devices that make up the system
	 * (and that are linked together via the SyncLink) must be given to TpcAccess. 
	 * To do this, call the \ref TPC_BeginSystemDefinition function first, then add
	 * all devices that belong to the system via \ref TPC_AddDevice and call
	 * \ref TPC_EndSystemDefinition to finish the list. It may take a short 
	 * while until connection to the device is established.
	 *
	 * The order in which the URLs are added via \ref TPC_AddDevice is important, because
	 * for simpler access, devices are addressed via their index in this device list
	 * in all functions that refer to a device. The index starts with 0 for the first 
	 * device added via \ref TPC_AddDevice, 1 for the next one and so on.
	 *  
	 * Switching systems is possible by calling again \ref TPC_BeginSystemDefinition,
	 * \ref TPC_AddDevice and \ref TPC_EndSystemDefinition but this should not be used
	 * for working with several system in parallel. For this case use the commands \ref TPC_NewSystem, 
	 * \ref TPC_BeginSystemDefinitionSystem, \ref TPC_AddDeviceSystem and
	 * \ref TPC_EndSystemDefinitionSystem. The \ref TPC_NewSystem generates
	 * a new system which can be accessed over the system id. 
	 *
	 * \section Example
	 * Working with one system: <br>
	 *
	 * <TT>
	 * TPC_BeginSystemDefinition(); <br>
	 * int iDeviceId = TPC_AddDevice("192.168.0.100:10010"); <br>
	 * TPC_EndSystemDefinition(5000); <br>
	 *
	 * </TT>
	 *
	 * Working with several systems <br>
	 * <TT>
	 * int iSystemId; <br>
	 * int iDeviceId; <br>
	 * TPC_NewSystem(&iSystemID); <br>
	 * TPC_BeginSystemDefinitionSystem(iSystemID); <br>
	 * TPC_AddDeviceSystem(iSystemID,"192.168.0.100:10010",&iDeviceId); <br>
	 * TPC_EndSystemDefinitionSystem(iSystemID,5000); <br>
	 * iDeviceId = iDeviceId + iSystemID; <br>
	 
	 * </TT>
	 * @{
	 */



	/// \brief Clear the list of devices that make up the TransPC system and start entering
	/// a new list with calls to TPC_AddDevice and TPC_EndSystemDefinition.
	/** 
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_BeginSystemDefinition();

	/// Add a device to the list of devices that make up the TransPC system.
	/** The order in which the URLs are added via \ref TPC_AddDevice is important, because
	* for simpler access, devices are addressed via their index in this device list
	* in all functions that refer to a device. The index starts with 0 for the first 
	* device added via \ref TPC_AddDevice, 1 for the next one and so on.
	*
	* There is no problem if the device cannot be connected immediately. The URL is still 
	* stored in the list and further attempts to connect to the device will be made 
	* periodically. The problem is shown in the deviceError field of the 
	* \ref TPC_DeviceStatus device status struct that can be queried 
	* by \ref TPC_GetDeviceStatus.
	*
	* \param url The URL (including the port) to connect to the device. 
	*               (e.g. "192.168.99.22:10010" or "localhost:10010")
	* \returns The index number of the device. (0 for the first device added, 1 for the next, 
	*       and so on.) If the same URL is added twice it is stored only once in the list. 
	*       The index of the previous entry is returned.
	*/
	TPC_EXP int TPC_CC TPC_AddDevice(const char *url);

	/// Add a device to the list of devices that make up the TransPC system.
	/** The order in which the URLs are added via \ref TPC_AddDevice is important, because
	* for simpler access, devices are addressed via their index in this device list
	* in all functions that refer to a device. The index starts with 0 for the first 
	* device added via \ref TPC_AddDevice, 1 for the next one and so on.
	*
	* There is no problem if the device cannot be connected immediately. The URL is still 
	* stored in the list and further attempts to connect to the device will be made 
	* periodically. The problem is shown in the deviceError field of the 
	* \ref TPC_DeviceStatus device status struct that can be queried 
	* by \ref TPC_GetDeviceStatus.
	*
	* \param url The URL (including the port) to connect to the device. 
	*               (e.g. "192.168.99.22:10010" or "localhost:10010")
	* \param recvTimeOut Periode in seconds till a timeout error occurse while requesting data from the device
	* \param sendTimeOut Periode in seconds till a timeout error occurse after sending a request to the device
	*
	* A value of zero for one of the timeout parameter disables timeout errors. 
	* \returns The index number of the device. (0 for the first device added, 1 for the next, 
	*       and so on.) If the same URL is added twice it is stored only once in the list. 
	*       The index of the previous entry is returned.
	*/
	TPC_EXP int TPC_CC TPC_AddDeviceEx(const char *url, int recvTimeOut, int sendTimeOut);

	/// End the entry of the list of devices and try to connect each one of them.
	/** If all devices could
	* be connected and returned no error, the return value is \ref tpc_noError.
	* If one of the devices did not respond within \a connectionTimeoutMilliseconds
	* the result is \ref tpc_errNoConnection. Otherwise, the result is
	* an error code indicating the problem with one of the devices that returned an error.
	*
	* \param connectionTimeoutMilliseconds Timeout for trying to connect the devices in milliseconds.
	* \returns \ref tpc_noError if all devices are connected successfully and return no error,  
	*					otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_EndSystemDefinition(int connectionTimeoutMilliseconds);



	/// Return the number of devices that make up the TransPC system.
	/** The device indices that are used in other 
	* functions range from 0..\ref TPC_NumDevices()-1.
	* \returns The number of devices. 
	*/
	TPC_EXP int TPC_CC TPC_NumDevices();


	/// Return the URL of a specified device in the list of devices that make up the TransPC system.
	/** 
	* \param index The index of the device (as returned by \ref TPC_AddDevice). Range 0..\ref TPC_NumDevices()-1.
	* \param buffer A buffer to receive the url of the device.
	* \param maxLen The capacity of the buffer.
	* \returns 
	*  - \ref tpc_noError if successful.
	*  - \ref tpc_errInvalidIndex if the index was not in the range 0..\ref TPC_NumDevices()-1. 	
	*  - \ref tpc_errBufferTooShort if the string was truncated because the buffer was too short.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetDeviceUrl(int index, char *buffer, int maxLen);


	/// Create a new system
	/** 
	*  \param id A pointer to the system identifier ID 
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_NewSystem(int *id);

	/// Delete a system
	/** 
	*  \param id System identifier ID generated by \ref TPC_NewSystem
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_DeleteSystem(int id);

	/// \brief Clear the list of devices that make up the TransPC system identified by id and start entering
	/// a new list with calls to \ref TPC_AddDeviceSystem and \ref TPC_EndSystemDefinitionSystem.
	/** 
	*  \param id System identifier ID generated by \ref TPC_NewSystem
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_BeginSystemDefinitionSystem(int id);

	/// Add a device to the list of devices that make up the TransPC system.
	/** The order in which the URLs are added via \ref TPC_AddDeviceSystem is important, because
	* for simpler access, devices are addressed via their index in this device list
	* in all functions that refer to a device. 
	*
	* There is no problem if the device cannot be connected immediately. The URL is still 
	* stored in the list and further attempts to connect to the device will be made 
	* periodically. The problem is shown in the deviceError field of the 
	* \ref TPC_DeviceStatus device status struct that can be queried 
	* by \ref TPC_GetDeviceStatus.
	* \param id System identifier ID generated by \ref TPC_NewSystem
	* \param url The URL (including the port) to connect to the device. 
	*               (e.g. "192.168.99.22:10010" or "localhost:10010")
	* \param deviceIx The index number of the device. If the same URL is added twice it is 
	*        stored only once in the list. 
	*
	*        <b>Attention The deviceIx used in other commands must be calculate as follow:
	*        deviceIx = SystemId (from NewSystem(..)) + deviceIx (from this command)</b>
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_AddDeviceSystem(int id, const char *url, int *deviceIx);


	/// Add a device to the list of devices that make up the TransPC system.
	/** The order in which the URLs are added via \ref TPC_AddDeviceSystem is important, because
	* for simpler access, devices are addressed via their index in this device list
	* in all functions that refer to a device. 
	*
	* There is no problem if the device cannot be connected immediately. The URL is still 
	* stored in the list and further attempts to connect to the device will be made 
	* periodically. The problem is shown in the deviceError field of the 
	* \ref TPC_DeviceStatus device status struct that can be queried 
	* by \ref TPC_GetDeviceStatus.
	* \param id System identifier ID generated by \ref TPC_NewSystem
	* \param url The URL (including the port) to connect to the device. 
	*               (e.g. "192.168.99.22:10010" or "localhost:10010")
	
	* \param recvTimeout Periode in seconds till a timeout error occurse while requesting data from the device
	* \param sendTimeout Periode in seconds till a timeout error occurse after sending a request to the device
	* \param deviceIx The index number of the device. If the same URL is added twice it is 
	*        stored only once in the list. 
	*
	*        <b>Attention The deviceIx used in other commands must be calculate as follow:
	*        deviceIx = SystemId (from NewSystem(..)) + deviceIx (from this command)</b>
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_AddDeviceSystemEx(int id, const char *url, int recvTimeout, int sendTimeout, int *deviceIx);

	/// Remove a device from the current TransPC system
	/**
	* \param id System identifier ID generated by \ref TPC_NewSystem
	* \param url The URL (including the port) to connect to the device. 
	*               (e.g. "192.168.99.22:10010" or "localhost:10010")
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_RemoveDeviceSystem(int id, const char *url);

	/// End the entry of the list of devices and try to connect each one of them.
	/** If all devices could
	* be connected and returned no error, the return value is \ref tpc_noError.
	* If one of the devices did not respond within \a connectionTimeoutMilliseconds
	* the result is \ref tpc_errNoConnection. Otherwise, the result is
	* an error code indicating the problem with one of the devices that returned an error.
	*
	* \param id System identifier ID generated by \ref TPC_NewSystem
	* \param connectionTimeoutMilliseconds Timeout for trying to connect the devices in milliseconds.
	* \returns \ref tpc_noError if all devices are connected successfully and return no error,  
	*					otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_EndSystemDefinitionSystem(int id, int connectionTimeoutMilliseconds);

	/// Return the number of devices that make up the TransPC system.
	/** The device indices that are used in other 
	* functions range from 0..\ref TPC_NumDevices()-1.
	* \param   id System identifier ID generated by \ref TPC_NewSystem
	* \param   n Number of devices defined in the system 
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_NumDevicesSystem(int id, int *n);


	/// Return the URL of a specified device in the list of devices that make up the TransPC system.
	/** 
	*  \param id System identifier ID generated by \ref TPC_NewSystem
	* \param index The index of the device (as returned by \ref TPC_AddDevice). Range 0..\ref TPC_NumDevices()-1.
	* \param buffer A buffer to receive the url of the device.
	* \param maxLen The capacity of the buffer.
	* \returns 
	*  - \ref tpc_noError if successful.
	*  - \ref tpc_errInvalidIndex if the index was not in the range 0..\ref TPC_NumDevices()-1. 	
	*  - \ref tpc_errBufferTooShort if the string was truncated because the buffer was too short.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetDeviceUrlSystem(int id, int index, char *buffer, int maxLen);

	/// Reset the configuration of a system
	/**
	* - stop the measurement
	* - reset the cluster configuration
	* - set all parameters to default values
	* - clear all attributes or set them to default values
	* 
	* \param id System identifier ID generated by \ref TPC_NewSystem
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ResetConfigurationSystem(int id);
	/*@}*/


	//=== Device information ===============================================================

	/**
	 * \addtogroup DeviceInfo
	 *
	 * Once the system definition is set up, TpcAccess will try to connect
	 * to each of the devices and get their information. With the functions in 
	 * this section you can query the properties of the hardware that was found.
	 *
	 * @{
	 */


	/// Holds information about a device and its installed boards.
	struct TPC_DeviceInfo {
		/// Device id number. This number is intended to be unique for each device. (read only)
		uint64_t		deviceID;

		/// Software version of the server. Format decimal 9999.99.99 (read only)
		int				serverSoftwareVersion;

		/// True if the hardware is simulated, false if the hardware is real. (read and write)
		bool			simulatedHardware;

		/// Bitmask, contains a 1 bit for each installed board (0..15) (read only)
		unsigned int	installedBoards;

		/// Device name. (incl. 0 termination) (read and write)
		char deviceName[9];

		// New Parameter  28.08.2009 ----

		/// User given device description (incl. 0 termination) (read and write)
		char deviceDescription[65];

		/// MAC-Address (Media Access Control - Address) (read only)
		uint64_t		deviceMACAddress;

		/// Use Local or remote time (read and write)
		bool			UseLocalTime;

		/// Autostart measurement (read and write)
		bool			AutoStartMeasurement;

		/// Autostart autosequence (read and write)
		bool			AutoStartAutoSequence;

		/// Server Port Device/Server 1 (read and write)
		int				ServerPort1;

		/// Server Port Device/Server 2 (read and write)
		int				ServerPort2;

		/// TargetPort (read and write)
		int				TargetPort;

		/// Enable 2in1 (read and write)
		bool			TwoinOneEnabled;

		/// 2in1 Password (incl. 0 termination) (read and write)
		char TwoInOnepassword[16];

		/// Temp data file path in continous mode (read and write)
		char DataFileName[tpc_maxPathLength];

		/// Number of backup data files in continous mode (read and write)   
		int				NumberOfBackupFiles;

		/// Time interval between harddisk flush commands in s (read and write)
		int				HdFlushInterval;

		/// Use server cache access in continous and ECR mode. (for higher data troughput) (read and write)
		bool			WriteTroughCache;
	
		/// Model Type of the measurement instrument in which the TPCX cards are installed to. 0 = Standard, 1 = TraNET-FE (read only)
		int				ModelType;

		int				SyncLinkDetected;
	};


	/// Get information about a device.
	/** 
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param deviceInfo A variable to receive information about the device
	* \param structSize The size of the deviceInfo struct in bytes.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetDeviceInfo(int deviceIx, struct TPC_DeviceInfo *deviceInfo, int structSize);

	/// Set device settings. Some of the settings need a server restart for getting valid. 
	/** 
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param deviceSettings A variable with the new settings. Only read and write classified struct members will be written.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_SetDeviceSettings(int deviceIx, TPC_DeviceInfo deviceSettings);


	/// Get free disk space.
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param freeDiskSpace Free disk space available for continous or ECR recording in byte
	* \param DiskSize Total disk size
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetFreeDiskSpace(int deviceIx, uint64_t *freeDiskSpace,
		uint64_t *DiskSize);

	/// Operaion mode options
	enum TPC_OperationModeOptions {
		/// Event recorder extensions (Retrigger, Stop trigger)
		tpc_optEcrExtensions = 0x01
	};


	/// Averaging options
	enum TPC_AveragingOptions {
		/// 16 Bit averaging mode
		tpc_optAvg16 = 0x01
	};


	/// Trigger extension options
	enum TPC_TriggerExtensionOptions {
		/// Slew rate trigger
		tpc_optTrgSlewRate		= 0x01,

		// Reference band trigger
		//tpc_optTrgReferenceBand = 0x02,

		/// Power trigger
		tpc_optTrgPower			= 0x04
	};

	enum TPC_SyncLinkMode{
		tpc_sync_oldserver		= 0x00,
		tpc_sync_notdetected	= 0x01,
		tpc_sync_detected		= 0x02
	};

	/// Holds information about a board.
	struct TPC_BoardInfo {
		/// Class indicator of the board. Currently 0 for TPCX cards
		int					boardClass;			
	
		/// Hardware version of the board. Format decimal 9999.99.99
		int					hardwareVersion;		

		/// Serial number of the board.
		int					serialNr;				

		/// Driver version of the software serving this board. Format decimal 9999.99.99
		int					driverVersion;			
								
		/// Firmware version loaded into this board. Format decimal 9999.99.99
		int					firmwareVersion;		
								
		/// Number of inputs.
		int					numberOfInputs;			
	
		/// Maximum block length in samples.
		int					maxMemory;				

		/// Maximum sample rate in Hertz.
		int					maxSpeed;				

		/// Operation mode options bit field. See \ref TPC_OperationModeOptions for the meaning of the bits.
		int					operationModeOptions;	

		/// Averaging options bit field. See \ref TPC_AveragingOptions for the meaning of the bits.
		int					averageOptions;			

		/// Trigger extension options bit field. See \ref TPC_TriggerExtensionOptions for the meaning of the bits. 
		int					triggerOptions;			

		/// Date and time of the last factory calibration
		struct TPC_DateTime lastFactoryCalibration;	

		/// Date and time of the last user calibration
		struct TPC_DateTime lastUserCalibration;		
	};


	/// Get information about a board.
	/** 
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param boardInfo A variable to receive information about the board
	* \param structSize The size of the boardInfo struct in bytes.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetBoardInfo(int deviceIx, int boardAddress,
		struct TPC_BoardInfo *boardInfo, int structSize);



	/// Offset options
	enum TPC_OffsetOptions {
		/// Full offset range (0..100%) instead of just 0% and 50%.
		tpc_optOfsFullRange = 0x01
	};

	/// Filter module options.
	enum TPC_FilterOptions {
		/// Filter module type 1 (5 MHz is installed).
		tpc_optFilterModule1 = 0x01,

		/// Filter module tpye 2 (200kHz is installed)
		tpc_optFilterModule2 = 0x03,
	};


	/// Differential input module options
	enum TPC_DifferentialInputOptions {
		/// Differential input module type 1 is installed.
		tpc_optDiffModule1 = 0x01
	};


	/// Input coupling options
	enum TPC_InputCouplingOptions {
		/// ICP (Integrated Current Power, 4 mA constant current power supply for piezo sensors).
		tpc_optIcpSource = 0x01,

		/// 50 Ohm input coupling
		tpc_opt50Ohm = 0x02
	};




	/// Holds information about an input channel.
	struct TPC_InputInfo {
		/// Class indicator for the input. 0 = TPCX Amplifier, 1 = TPCX-2 Amplifier (100 MHz)
		int inputClass;			

		/// Hardware version of the input. Format decimal 9999.99.99
		int hardwareVersion;	

		/// Max. speed of the ADC in Hertz
		int maxAdcSpeed;		

		/// Resolution of the ADC in Bits
		int adcResolution;		

		/// Offset options bit field.
		/// See OffsetOptions for the meaning of the bits.
		int offsetOptions;		

		/// Filter module options bit field.
		/// See FilterOptions for the meaning of the bits.
		int filterOptions;		

		/// Differential input module options bit field.
		/// See DifferentialInputOptions for the meaning of the bits.
		int diffOptions;		

		/// Input coupling options bit field.
		/// See \ref TPC_InputCouplingOptions for the meaning of the bits.
		int inputCouplingOptions;

		/// Input Range Array
		/// List of supported input ranges
		double inputRanges[tpc_maxInputRanges];

		/// Max Marker Signals
		int maxMarkerMask;
	};

	/// Get information about an input.
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param inputInfo A variable to receive information about the input
	* \param structSize The size of the inputInfo struct in bytes.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetInputInfo(int deviceIx, int boardAddress, int inputNumber,
		struct TPC_InputInfo *inputInfo, int structSize);


	/*@}*/




	//=== Cluster configuration ============================================================

	/**
	 * \addtogroup Cluster
	 *  
	 * A cluster is a group of one or more TPCX boards that have a common operation mode
	 * and a common time base for the measurement. There can be multiple clusters in the 
	 * system and they can contain boards from different devices.
	
	 * Clusters are defined by freely assigning a cluster number >= 0 to each board of 
	 * the system. All boards that have the same cluster number form one cluster.
	
	 * For clearing the cluster configuration, assign the cluster number 0 to all boards.
	
	 * To set time base parameters for the cluster it is sufficient to set the time base
	 * parameter of one board of the cluster. The system will automatically copy them to 
	 * the other devices and boards.
	 * @{
	 */
	

	/// Get the current cluster numbers on one device
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param clusterNumbers An array of dimension 'tpc_maxBoards' to receive the cluster numbers. 
	*		   Boards that are not installed are set to -1.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetClusterNumbers(int deviceIx, int *clusterNumbers);


	/// Set a new group configuration. Warning: Parameter settings may get lost as a result 
	//			of this operation.
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param clusterNumbers An array of dimension 'tpc_maxBoards' with cluster numbers. Not installed boards have the
	*		  cluster number -1.
	* \returns \ref tpc_noError if successful. tpc_errIncompatibleBoards if the new clustering has assigned two or more 
	*		incompatible boards to the same cluster.
	*      Another error code indicating a problem with the devices or the network connection.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_SetClusterNumbers(int deviceIx, int *clusterNumbers);

	/// Returns the first board number of the device which is in the given cluster
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param clusterNumber Cluster number as returnd by \ref TPC_GetClusterNumbers
	* \param boardAddress Returns the first board number from this device which is in this cluster
	* \returns \ref tpc_noError if successful, tpc_errInvalidBoardAddress if the cluster number doesn't exist.
	*   Another error code indicating a problem with the devices or the network connection.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ClusterNumberToBoardAddress(int deviceIx, int clusterNumber, int *boardAddress);

	/*
	Clusters can contain boards with different capabilities. The function GetClusterInfo
	determines the capabilities that can be fulfilled by all boards of the cluster.
	*/


	/*######## NOT IMPLEMENTED YET ###########
	/// Holds information about a cluster. This information is derived from the
	/// capabilities and states of the devices and boards in the cluster.
	struct TPC_ClusterInfo
	{
		/// The maximum block length of the cluster.
		int maxMemory;  
	
		/// The maximum sample rate of the cluster.
		int maxSpeed;   
	};
	
	// Get information about a cluster. The cluster is selected by specifying one
	//		board from that cluster.
	// deviceIx - The device index as returned by TPC_AddDevice() that contains one board 
	//					of the cluster.
	// boardAddress - The address of one board of the cluster.
	// clusterInfo - A variable to receive information about the cluster
	// structSize - size of the clusterInfo struct in bytes.
	// returns 
	// - tpc_noError if successful.
	// - tpc_errIncompatibleBoards if some of the boards in the cluster are incompatible.
	// - an error code indicating a problem with one of the devices or the network connection.
	TPC_EXP TPC_ErrorCode TPC_CC TPC_GetClusterInfo(int deviceIx, int boardAddress,	
						   TPC_ClusterInfo* clusterInfo, int structSize);
	
	
	
	
	// Check if two boards can be combined
	// deviceIx1 - The device index as returned by TPC_AddDevice() that contains the 
	//					first board.
	// boardAddress1 - The address of the first board.
	// deviceIx2 - The device index as returned by TPC_AddDevice() that contains the 
	//					second board.
	// boardAddress2 - The address of the second board.
	// returns 
	// - tpc_noError if successful.
	// - tpc_errIncompatibleBoards if the two boards cannot be in the same cluster.
	// - another error code indicating a problem with one of the devices or the network connection.
	// remarks - This function works independently of the current clusters that the two 
	//				boards are in. It can be used to make a check before the clusters are 
	//				assigned, or to find out which boards do not match if the cluster info 
	//				shows an error after the clusters are assigned.
	TPC_EXP TPC_ErrorCode TPC_CC CanBeClustered(int deviceIx1, int boardAddress1, int deviceIx2, int boardAddress2);
	
	*/
	/*@}*/
	//=== Configuration ====================================================================

	/**
	* \addtogroup Configuration 
	*
	* There are several elements of configuration that can be set before the next recording
	* is started: Parameters, Attributes and associated channels (for ECR mode only).
	*
	* <i>Parameter</i> is just a collective name for settings in the form of double numbers that 
	* are assigned with a single input (channel parameters, e.g. input voltage range), or 
	* a cluster of the system (timebase parameters, e.g. sample rate).
	*
	* There are two kinds of parameters: Mode parameters and value parameters.
	* Mode parameters set a certain mode of operation, e.g. the trigger mode or the
	* filter mode. They use integer constants to specify the mode. Value parameters 
	* set a numerical value like the sample rate or the input range. The main difference
	* between mode and value parameters is in the error handling when trying to set
	* invalid values for the parameter. Mode parameters return an error if a invalid 
	* number is given. Value parameters just restrict the value to the closest possible 
	* one and no error is returned. 
	*
	* <i>Attributes</i> are key/value pairs (string/string) that can be assigned to each input.
	* They are ignored by the driver and free to use to store any kind of information.
	* Predefined attributes are (key/default value):
	* - "ChName"        "A1", "A2", .., "B1", ...
	* - "ChPhysUnit"    "V"
	*
	* <i>Associated channels</i> are used in ECR mode only. In ECR mode each channel
	* triggers and records data individually. Associated channels are a way to define
	* that other channels should also record data when a specified channel triggers.
	* See \ref TPC_GetAssociatedChannels and \ref TPC_SetAssociatedChannels.
	*
	\section Parameters
	
	There are two kinds of parameters: Mode parameters and value parameters.
	Mode parameters set a certain mode of operation, e.g. the trigger mode or the
	filter mode. They use integer constants to specify the mode. Value parameters 
	set a numerical value like the sample rate or the input range. The main difference
	between mode and value parameters is in the error handling when trying to set
	invalid values for the parameter. Mode parameters return an error if a invalid 
	number is given. Value parameters just restrict the value to the closest possible 
	one and no error is returned. 
	
	The most important parameters are these:
	\subsection paramsTB Time base parameters
	Timebase parameters cannot be set individually per channel. They are common per cluster. 
	- tpc_parOperationMode - sets the basic time base mode. See enum \ref TPC_OperationModes.
	- tpc_parSamplingFrequency - sets the sampling frequency in Hertz.
	\subsubsection paramsBlockMode Block Mode
	These parameters apply when tpc_parOperationMode is set to block mode.
	- tpc_parBlkNumberOfBlocks - The number of blocks to record. The recording will stop 
	automatically if this many blocks have been recorded.
	- tpc_parBlkBlockLength - The block length in samples.
	- tpc_parBlkTriggerDelay - Trigger delay in percent of the block length.
	\subsubsection paramsContinuousMode Continuous Mode
	These parameters apply when tpc_parOperationMode is set to continuous mode.
	- tpc_parContMaximumDataLength - Limits the maximum amount of data in samples to be 
	recorded. The recording will stop automatically if this many samples have been recorded. 
	\subsubsection paramsScopeMode Scope Mode
	These parameters apply when tpc_parOperationMode is set to scope mode.
	- tpc_parScopeAutoTrigger - Automatically fire a trigger after some time if the signal does not trigger.
	- tpc_parScopeSingleShot - Single shot mode, stops after each acquisition.
	\subsubsection paramsEventMode Event Recorder Mode
	These parameters apply when tpc_parOperationMode is set to event recorder mode.
	- tpc_parEcrMinimumBlockLength - The block length in samples.
	- tpc_parEcrTriggerDelay - Trigger delay in percent of the block length.
	- tpc_parEcrMaximumNumberOfBlocks - The number of blocks to record. The recording will stop 
	automatically if this many blocks have been recorded.
	
	\subsection inputTB Input parameters
	Input parameters can be set individually per channel.
	
	\subsubsection paramsInputRange Input Range
	These parameters define the input voltage range.	
	- tpc_parInputMode - Input mode: Off, Single ended, Differential. See enum \ref TPC_InputMode
	- tpc_parRange - Input voltage range in Volt. 
	- tpc_parOffset - Input range offset in percent. 
	
	The effective input range can be calculated as follows:\n
	min = -range * offset/100\n
	max = min + range\n
	Example: When setting tpc_parRange to 5 Volt and tpc_parOffset to 40%, the 
	effective input range will be -2 .. +3 Volt.
	
	\subsubsection paramsTrigger Trigger Parameters 
	These parameters define whether and how a channel triggers.
	- tpc_parTrgMode - Main trigger mode. See enum \ref TPC_TriggerModes.
	- tpc_parTrgComparatorMode - Trigger comparator mode:  Slope or window. See enum \ref TPC_TriggerComparatorMode.
	- tpc_parTrgLevelA - The first trigger level in percent of the full input range. 
	- tpc_parTrgLevelB - The second trigger level in percent of the full input range. 
	
	The effective trigger levels in Volt can be calculated as follows:\n
	level = tpc_parRange * tpc_parTrgLevelA / 100\n 
	Note: tpc_parOffset has no effect on the trigger level.\n
	Example: When setting the trigger and comparator mode to +Slope trigger, the
	first level will be the trigger level and the second level will be used as
	hysteresis. If tpc_parRange is set to 5Volt, tpc_parTrgLevelA is set to 10% 
	and tpc_parTrgLevelB to 9%, the channel will trigger if the signal falls 
	below 0.45 Volt (hysteresis) and then raises above 0.5 Volt.

	* @{
	*/

	/// \brief Reset the complete device configuration. This will clear all attributes and 
	/// set all parameters to their default values. (Does not clear the system definition.)
	/**
	* \returns \ref tpc_noError if all devices could be successfully reset,
	*           otherwise an error code indicating a problem with one the 
	*           devices or the network connection.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ResetConfiguration();


	//=== Parameters and Attributes ========================================================


	/// This enum lists all recording parameters that can be set.
	enum TPC_Parameter {

		//=== Timebase parameters, apply per cluster ===

		//--- Common parameters ---

		/// Block, Continuous, Event Recorder and option flags. See enum \ref TPC_OperationModes.
		tpc_parOperationMode = 1,	

		/// Channel Multiplexer, 4 or 8.
		/// Please note: In mux 8 mode the maximum sample rate, maximum block length and 
		/// maximum number of blocks are limited to half of the values in mux 4.
		/// If the sample frequency, block length or number of blocks was set to the
		/// maximum when switching from mux 4 to mux 8, these parameters will be changed.
		tpc_parMultiplexerMode = 2,


		/// Sample rate clock source: Internal or External. See enum \ref TPC_TimeBaseSource.
		tpc_parTimebaseSource = 3,

		/// Sampling frequency in Hertz. In effect when TimebaseSource = internal.
		/// Valid values from 1Hz to the maximum speed of the board or cluster.
		tpc_parSamplingFrequency = 4,

		/// Divisor for external clock input. In effect when TimebaseSource = external.
		/// Valid values: 1..65535
		tpc_parExternalClockDivisor = 5,


		/// External trigger input: Off, PosSlope, NegSlope. See enum \ref TPC_ExternalTriggerMode.
		tpc_parExternalTriggerMode = 6,

		/// Sampling frequency in Hertz that is applied to the external clock input 
		/// when measuring with external clock. In effect when TimebaseSource = external.
		/// Valid values >= 0.
		tpc_parExtClockSamplingFrequency = 46,


		//--- Parameters for block mode ---
	
		/// Number of blocks to record. Valid values from 1 to the memory size
		/// of the board or cluster divided by the block length.
		tpc_parBlkNumberOfBlocks = 7,

		/// Block length in samples. Valid values are powers of two from 1024 up to the 
		/// memory size of the board.
		tpc_parBlkBlockLength = 8,

		/// Trigger delay in percent of the block length. Valid values from -100 to +200%.
		tpc_parBlkTriggerDelay = 9,

		//--- Parameters for Continuous Mode ---

		/// Limits the maximum amount of data in samples to be recorded. Valid values >= 1 
		tpc_parContMaximumDataLength = 10,

		/// Number of samples to record after a stop trigger. Valid values >= 0. 
		/// In effect when the stop trigger option for Continuous mode is used.
		tpc_parContStopTrailer = 11,

		/// Enable stop trigger in continuous mode.  Valid values: 0: disabled, 1: enabled
		tpc_parContEnableStopTrigger = 47,

		/// Enable recording limit for continuous mode.  Valid values: 0: disabled, 1: enabled
		tpc_parContEnableRecLengthLimit = 48,
  	
		//--- Parameters for scope mode ---

		/// Auto trigger for scope mode. 
		/// If this value is 0, waits until the signal triggers.
		/// If this value is 1, automatically fires a trigger after 
		/// some time if the signal does not trigger.
		tpc_parScopeAutoTrigger = 43,

		/// Single shot mode for scope mode. 
		/// If this value is 0, acquisitions are continuously restarted.
		/// If this value is 1, stops after each acquisition.
		tpc_parScopeSingleShot = 44,

		/// Block length in samples. Valid values are powers of two from 1024 up to 
		/// half of the memory size of the board.
		tpc_parScopeBlockLength = 49,

		/// Trigger delay in percent of the block length. Valid values from -100 to +200%.
		tpc_parScopeTriggerDelay = 50,


		//--- Parameters for Event recorder --- 


		/// Dual Mode for ECR Single and Multi 
		/// If this value is 0, Dual Mode was disabled
		/// If this value is 1, Dual Mode was enabled
		tpc_parEcrDualMode = 60,	//MU

		/// Block length in samples. Valid values >= 0.
		/// Note: If the retrigger option is active, the actually recorded block 
		/// can be longer than this.
		/// **** Obsolete. Use tpc_parEcrPreTrigger/tpc_parEcrPostTrigger instead
		tpc_parEcrMinimumBlockLength = 12,

		/// Trigger delay in percent of the block length. Valid values from -100 to 0%.
		/// **** Obsolete. Use tpc_parEcrPreTrigger/tpc_parEcrPostTrigger instead
		tpc_parEcrTriggerDelay = 13,

		/// ECR Pretrigger in samples. Valid values >= 0.
		tpc_parEcrPreTrigger = 56,

		/// ECR Posttrigger in Samples. Valid values >= 0.
		tpc_parEcrPostTrigger = 57,

		/// Limits the number of blocks to record. Valid values >= 1.
		tpc_parEcrMaximumNumberOfBlocks = 14,

		/// Enable hold off.  Valid values: 0: disabled, 1: enabled
		tpc_parEcrEnableHoldOff = 51,

		/// Hold off time in samples.
		tpc_parEcrHoldOffTime = 15,

		/// Limits the length of a block. 
		/// In effect when the retrigger option for Event recorder mode is used.
		tpc_parEcrMaximumBlockLength = 16,

		/// Retrigger time in samples.
		/// In effect when the retrigger option for Event recorder mode is used.
		tpc_parEcrRetriggerTime = 17,

		/// Number of samples to record after a stop trigger. Valid values >= 0. 
		/// In effect when the stop trigger option for Event recorder mode is used.
		tpc_parEcrTrailer = 18,

		/// Divisor for the sample rate of the continuous recording in dual mode. Valid values >= 1.
		/// In effect when the dual mode option for Event recorder mode is used.
		tpc_parEcrClockDivisor = 19,
	
		/// Enable trailer in dual mode.  Valid values: 0: disabled, 1: enabled
		tpc_parEcrDualEnableTrailer = 52,

		/// Number of samples to record after the trigger of the last ECR block. Valid values >= 0. 
		tpc_parEcrDualTrailer = 53,

		/// Enable recording limit in dual mode.  Valid values: 0: disabled, 1: enabled
		tpc_parEcrDualEnableRecLengthLimit = 54,
	
		/// Limits the maximum amount of data in samples to be recorded in dual mode. Valid values >= 1 
		tpc_parEcrDualMaxRecLength = 55,

		/// Enable stop trigger in ECR mode.  Valid values: 0: disabled, 1: enabled
		tpc_parEcrEnableStopTrigger = 58,

		/// Enable retrigger in ECR mode.  Valid values: 0: disabled, 1: enabled
		tpc_parEcrEnableRetrigger = 59,


		//=== Input parameters, apply per input channel ===

		//--- General ---
	
		/// Input mode: Off, Single ended, Differential. See enum \ref TPC_InputMode
		tpc_parInputMode = 20,	

		/// Option to disable data recording for the channel in ECR and Continuous mode. 
		/// Valid values: 0:channel produces data, 1:channel does not produce 
		/// data, but can still cause a trigger.
		tpc_parTriggerOnly = 21,

	
		//--- Input range
	
		/// Input coupling: DC, AC, ICP. 50 Ohm, See enum \ref TPC_InputCoupling
		tpc_parInputCoupling = 22,

		/// Input voltage range in Volt. Valid values: 0.1, 0.2, 0.5, 1, 2, 5, 10, 20, 50, 100.
		tpc_parRange = 23,

		/// Input range offset in percent. Valid values from 0 to 100%.
		tpc_parOffset = 24,

		/// Set the invertion mode by a bit mask, see \ref TPC_InverterModeMask
		tpc_parInvert = 25,

	
		//--- Filter
	
		/// Filter mode: Off, 22kHz RC filter, 200kHz RC filter, Filter Module. See enum \ref TPC_FilterMode
		tpc_parFilterMode = 26,

		/// Filter frequency in Hertz. Valid values: A set of frequencies between 200Hz and 5MHz.
		/// In effect when FilterMode = tpc_fltModule.
		tpc_parFilterFreq = 27,

		/// Input averaging: Off, 14bit, 16bit  
		/// See enum \ref TPC_AveragingMode.
		/// Please note: This option cannot be selected individually for each input.
		/// The setting applies to all inputs of a board.
		tpc_parAveragingMode = 28,			


		//--- Physical unit
	
		/// Factor for the conversion from volt to user selected physical unit. 
		/// value in physical unit = value in volt * factor + constant.
		tpc_parPhysFactor = 29,		

		/// Constant for the conversion from volt to user selected physical unit. 
		/// value in physical unit = value in volt * factor + constant.
		tpc_parPhysConstant = 30,


		//--- Trigger parameters

		/// Trigger mode.
		/// See enum \ref TPC_TriggerModes.
		tpc_parTrgMode = 31,

		/// Trigger comparator mode:  Off, PosSlope, NegSlope, PosNegSlope, WinOut, WinIn, StateWinIn, StateWinOut. 
		/// See enum \ref TPC_TriggerComparatorMode.
		/// 'trgCompOff ' disables the trigger for this channel.
		tpc_parTrgComparatorMode = 32,

		/// First trigger level in percent of full input range. 
		/// Valid values are from -100 to +100%.
		tpc_parTrgLevelA = 33,

		/// Second trigger level in percent of full input range. 
		/// Valid values are from -100 to +100%.
		tpc_parTrgLevelB = 34,


		//--- Additional trigger parameters, depending on trigger mode.

		/// Timeout for trigger timer in samples. Valid values from 0 to 65535.
		tpc_parTrgTimerTSamples = 35,

		/// Pulse Trigger <> and >< Trigger Windows Start. Valid values from 0 to 65535.
		tpc_parTrgPreTimerTSamples = 64,

		/// Delta-T in samples for the differentiator. Valid values 1-65535.
		tpc_parTrgSlewRateDTSamples = 36,

		/// Hysteresis for slew rate trigger.
		tpc_parTrgSlewRateHysteresis = 45,

		/// Trigger comparator mode:  PosSlope, NegSlope, PosNegSlope.
		/// Comparator for Slew Rate Trigger Mode
		/// 
		tpc_parTrgSlewRateComparatorMode = 61,

		/// Trigger mode for the second comparator: Off, PosSlope, NegSlope, PosNegSlope, WinOut, WinIn. 
		/// See enum \ref TPC_TriggerComparatorMode.
		tpc_parTrgComparator2Mode = 37,

		/// Trigger level for the second comarator in percent of full input range. 
		/// Valid values are from -100 to +100%.
		tpc_parTrgLevel2A = 38,

		/// Trigger level for the second comarator in percent of full input range. 
		/// Valid values are from -100 to +100%.
		tpc_parTrgLevel2B = 39,

		/// Show also the multiplied signal (only if trigger multiplier is used (see \ref tpc_trgInputMultiplier))
		tpc_parShowTrgProduct = 62,

		/// Set the AND trigger mask for configuring the AND trigger groups. 
		/// Each of the 4 or 8 board inputs can be assigned to one or several AND trigger groups. 
		/// Trigger events from these inputs are linked together and were forwarded once all triggers of 
		/// the assigned group inputs have triggered. Trigger signals coming from the AND trigger groups 
		/// are linked together by an OR logic. The ANDTriggerMask is a 32 bit variable. Bit 7 to 0 configures
		/// group 1, Bit 15 to 8 configures group 2, bit 23 to 16 configures group 3 and bit 31 to 24 configures 
		/// group 4. A channel is assigned to a group by setting a the corresponding bit to 1. 
		/// Example: 0x0000030C is assigning channel 1 and 2 to group 2 and channel 3 and 4 to group 1.
		tpc_parANDTriggerMask = 63,

		//--- Stop trigger in Dual mode

		/// Comparator mode for stop trigger:  Off, PosSlope, NegSlope, PosNegSlope, WinOut, WinIn. 
		/// See enum \ref TPC_TriggerComparatorMode.
		/// 'Off' disables the stop trigger for this channel.
		/// In effect when the stop trigger option for Event recorder mode is used.
		tpc_parDualModeStopTrgComparatorMode = 40,

		/// First level for the stop trigger in percent of full input range. 
		/// Valid values are from -100 to +100%.
		/// In effect when the stop trigger option for Event recorder mode is used 
		/// and the trigger comparator mode is not 'Off'.
		tpc_parDualModeStopTrgLevelA = 41,

		/// Second level for the stop trigger in percent of full input range. 
		/// Valid values are from -100 to +100%.
		/// In effect when the stop trigger option for Event recorder mode is used 
		/// and the trigger comparator mode is not 'Off'.
		tpc_parDualModeStopTrgLevelB = 42,

		/// Reset the overflow indicator 
		tpc_parResetOverflow = 65,

		/// Enable external Timebase output frequency, 0 = Timebase output is used for Armed signal
		tpc_parExternalTimebaseOutFreq = 66,
		
		/// Enable or disable the GPS/IRIG Pulse per Second synchronization mode 0 = off, 1 = on
		tpc_parGPSSync = 67,

		/// Configure LXI Event Message generation and reception Bit 0: on/off generation, Bit 1: on/off reception
		tpc_parLXIMsgConfig = 68,
		
		/// Enable Trigger Mask for digital positive  Slope Trigger
		tpc_parTrgDigPosSlope = 69,
		/// Enable Trigger Mask for digital negative Slope Trigger
		tpc_parTrgDigNegSlope = 70,
		
		/// Reserved Parameters for customer projects
		tpc_ReservedParam1 = 71,
		tpc_ReservedParam2 = 72,
		tpc_parTrgDigPls	= 73,
		/// GPS Logging Mode
		tpc_parGPSMode = 74,
		/// GPS Log Intervale
		tpc_parGPSTimer = 75,
		///Set Sync Ping Mode on/off
		tpc_parSyncPingMode = 76,
		/// Set SyncLink 2 Delay
		tpc_parSyncDelay = 77
	};

	

	/// GPS Logging Mode
	enum TPC_GPS_CAPTURE_MODE{
		tpc_gps_onTrigger,
		tpc_gps_Continuous,
	};

	/// Operation modes
	enum TPC_OperationModes {
		/// Scope mode.
		tpc_opModeScope         = 0,

		/// Block mode.
		tpc_opModeBlock			= 1,

		/// Continuous mode.
		tpc_opModeContinuous	= 2,

		/// Block basic mode where Parameter.BlkNumberOfBlocks has no effect (taken as 1).
		tpc_opModeSingleBlock	= 4,

		/// Event recorder mode - single channel.
		tpc_opModeEventRecorderSingle = 3,

		/// Event recorder mode - single channel, dual mode.
		tpc_opModeEventRecorderSingleDual = 0x043,

		/// Event recorder mode - multi channel.
		tpc_opModeEventRecorderMulti = 0x083,

		/// Event recorder mode - multi channel, dual mode.
		tpc_opModeEventRecorderMultiDual = 0x0C3,
	};


	/// Time base source
	enum TPC_TimeBaseSource {
		/// Time base is generated from the crystal oscillator.
		tpc_tbSrcInternal = 0,

		/// Time base is taken from external input.
		tpc_tbSrcExternal = 1,
	};


	/// External trigger input modes
	enum TPC_ExternalTriggerMode {
		/// External trigger input does not trigger
		tpc_extTrgOff = 0,

		/// Triggers on a positive slope
		tpc_extTrgPosSlope = 1,

		/// Triggers on a negative slope
		tpc_extTrgNegSlope = 2,
	};


	/// Input modes
	enum TPC_InputMode {
		/// The input does not record data.
		tpc_inpOff = 0,

		/// Single ended input.
		tpc_inpSingleEnded = 1,

		/// Differential input. 
		tpc_inpDifferential = 2,
	};


	/// Input couplings
	enum TPC_InputCoupling {
		/// DC coupling 1 MOhm input impedance
		tpc_inpCouplingDC = 0,

		/// AC coupling, 1 MOhm input impedance
		tpc_inpCouplingAC = 1,

		/// ICP coupling (Integrated Current Power, 4 mA constant current power supply for piezo sensors).
		tpc_inpCouplingICP = 3,

		/// DC coupling 50 Ohm input impedance (Attention, 5V limit!)
		tpc_inpCouplingDC50 = 4,

		/// AC coupling 50 Ohm input impedance (Attention, 5V limit!)
		tpc_inpCouplingAC50 = 5,
	};


	/// Averaging modes
	enum TPC_AveragingMode {
		/// No averaging
		tpc_avgOff = 0,

		/// Averaging with 14 bit result (and 2 marker bits) 
		tpc_avg14bit = 1,

		/// Averaging with 16 bit result (no marker bits) 
		tpc_avg16bit = 2,
	};


	/// Filter modes
	enum TPC_FilterMode {
		/// No filter is used.
		tpc_inpFilterOff = 0,

		/// The first RC filter is used.
		tpc_inpFilterRC1 = 1,

		/// The second RC filter is used.
		tpc_inpFilterRC2 = 2,

		/// The filter module is used. 
		tpc_inpFilterModule = 3
	};



	/// Trigger mode
	/**
	* The trigger mode is composed of the options or modes of
	* the components of the trigger system. The basic mode
	* is 0, which is a simple comparator trigger.
	* The modes for each of the trigger system components
	* can be or-ed together to build the final trigger mode value.
	* Note: Not all components can be combined!
	*
	* Possible combinations:
	* \verbatim
	Multiplier   Slew Rate     Timer       2nd Comparator
	---------------------------------------------------------
	X            0           0                0
	X            1           X                -
	X            X           1                -
	X            X           -                1
	-            -           1(delay)         - 

	1/0 : Option used / not used
	-  : Option not available
	X  : Option available
	\endverbatim
	*/
	enum TPC_TriggerModes {
		/// If this bit is 1, the trigger is off regardless of the other bits.
		tpc_trgMasterDisable = 0x8000,

		/// 0 is the basic mode (simple comparator trigger)
		tpc_trgBasic = 0,

		/// Input multiplier on
		tpc_trgInputMultiplier = 0x01,

		/// Slew rate
		tpc_trgSlewRate		= 0x02,

		// Timer 
		/// Timer off
		tpc_trgTimerOff     = 0x00,
		/// Detects long pulses, Pulse>T
		tpc_trgLongPulse	= 0x04,
		/// Detects short pulses, Pulse<T
		tpc_trgShortPulse	= 0x08,
		/// Detects long periods, Period>T
		tpc_trgLongPeriod	= 0x0C,
		/// Detects short periods, Period<T
		tpc_trgShortPeriod	= 0x10,
		/// Detects pulses (T - preTimer) < Pulse < T
		tpc_trgEqualPulse   = 0x20,
		/// Detects pulses (T - preTimer) > Pulse || Pulse > T
		tpc_trgNotEqualPulse = 0x24,

		/// Detect delays between 2 channels > T
		tpc_trgLongDelay		= 0x28,
		/// Detect delays between 2 channels < T
		tpc_trgShortDelay		= 0x2C,
		/// Detect delays between 2 channels T1 < Delay < T
		tpc_trgEqualDelay		= 0x30,
		/// Detect delays between 2 channels T1 > Delay > T
		tpc_trgNotEqualDelay	= 0x34,
		/// Detect periodes (T - preTimer) < Period < T
		tpc_trgEqualPeriod		= 0x38,
		/// Detect periodes (T - preTimer) > Period || Period > T
		tpc_trgNotEqualPeriod	= 0x3C,
		/// Mask to filter out timer mode bits
		tpc_trgTimerMask		= 0x3C,

		// Reference band 
		//tpc_trgRefBandOff       = 0x00,
		//tpc_trgRefBandSingle    = 0x20,  ///< Reference band, not retriggerable
		//tpc_trgRefBandRetrigger = 0x40,  ///< Reference band, retriggerable

		//tpc_trgRefBandMask		= 0x60,
		//-------

		/// Special pulse width timer modes with two independent comparators 
		/// for the rising and falling edge. Detects long pulses, Pulse>T.
		tpc_trgLongPulseTwoComp	 = 0x14,

		/// Special pulse width timer modes with two independent comparators 
		/// for the rising and falling edge. Detects short pulses, Pulse<T.
		tpc_trgShortPulseTwoComp = 0x18,

		/// Special mode using two comparators. Either comparator can trigger.
		tpc_trgTwoComparatorsOr  = 0x80,

		/// Special mode using two comparators. 
		/// To detect a trigger, comparator one must first detect a trigger condition 
		/// followed by comparator two.
		tpc_trgTwoComparatorsAnd = 0x100,

		/// Mask to filter out twoComparator mode bits
		tpc_trgTwoComparatorsMask = 0x180,
	};



	/// Trigger comparator modes
	enum TPC_TriggerComparatorMode {
		/// Trigger disabled
		tpc_trgCompOff = 0,

		/// Triggers on positive slope
		tpc_trgCompPositiveSlope = 1,

		/// Triggers on negative slope
		tpc_trgCompNegativeSlope = 2,

		/// Triggers on positive and negative slope
		tpc_trgCompBothSlopes = 3,

		/// Triggers when the signal goes outside a window
		tpc_trgCompWindowOut = 4,

		/// Triggers when the signal goes inside a window
		tpc_trgCompWindowIn = 5,

		/// Triggers when state is outside a window or enables other triggers when is used in a AND trigger group, can only used for trigger comparator 1
		tpc_trgCompStateWindowOut = 6,

		/// Triggers when state is inside a window or enables other triggers when is used in a AND trigger group, can only used for trigger comparator 1
		tpc_trgCompStateWindowIn = 7,


	};

	/// Trigger modes for the \ref TPC_SetTrigger function
	enum TPC_EasyTriggerMode {
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
		tpc_etrgLongPulse	= 0x04,
		/// Detects short pulses, Pulse<T; Time2 parameter is ignored
		tpc_etrgShortPulse	= 0x08,
		/// Detects long periods, Period>T; Time2 parameter is ignored
		tpc_etrgLongPeriod	= 0x0C,
		/// Detects short periods, Period<T; Time2 parameter is ignored
		tpc_etrgShortPeriod	= 0x10,
		/// Detect delays between 2 channels > T; Time2 parameter is ignored
		tpc_etrgLongDelay		= 0x28,
		/// Detect delays between 2 channels < T; Time2 parameter is ignored
		tpc_etrgShortDelay		= 0x2C,

		/// Detects pulses Time2 < Pulse < Time
		tpc_etrgEqualPulse   = 0x20,
		/// Detects pulses Time2 > Pulse || Pulse > Time
		tpc_etrgNotEqualPulse = 0x24,
		/// Detect periodes Time2 < Period < Time
		tpc_etrgEqualPeriod		= 0x38,
		/// Detect periodes Time2 > Period || Period > Time
		tpc_etrgNotEqualPeriod	= 0x3C,
		/// Detect delays between 2 channels Time2 < Delay < Time
		tpc_etrgEqualDelay		= 0x30,
		/// Detect delays between 2 channels Time2 > Delay > Time
		tpc_etrgNotEqualDelay	= 0x34,
	};


	/// Trigger comperator modes for the \ref TPC_SetTrigger function
	enum TPC_EasyTriggerComperatorMode {
		/// Triggers on positive slope; Triggers when signal goes inside window in Window mode; Triggers when signal above level in State mode 
		tpc_etrgCompPositive = 1,
		/// Triggers on negative slope; Triggers when signal goes outside window in Window mode; Triggers when signal below level in State mode
		tpc_etrgCompNegative = 2,
		/// Triggers on positive and negative slope
		tpc_etrgCompBoth = 3,
	};


	/// Trigger flags for the \ref TPC_SetTrigger function, flags can be combined with the or-operator
	enum TPC_EasyTriggerFlags {
		/// Disables Input Multiplier and all AND trigger assignments
		tpc_etrgNone = 0x00,
		/// Adds Input to AND Group 1
		tpc_etrgANDGroup1 = 0x01,
		/// Adds Input to AND Group 2
		tpc_etrgANDGroup2 = 0x02,
		/// Adds Input to AND Group 3
		tpc_etrgANDGroup3 = 0x04,
		/// Adds Input to AND Group 4
		tpc_etrgANDGroup4 = 0x08,
		/// Input multiplier on
		tpc_etrgInputMultiplier = 0x10,
		/// Show also multiplied signal (use in conjuction with tpc_etrgInputMultiplier)
		tpc_etrgShowMultipliedSignal = 0x20,
		/// Set the master disable trigger flag
		tpc_etrgSetMasterDisable = 0x40,
		/// Only set trigger and comperator modes. Level, Hysteresis and Time Parameters are not changed.
		tpc_etrgIgnoreLevelAndTime = 0x80,
	};

    
	/// Bit Mask for analog and marker invertion
	enum TPC_InverterModeMask{
		/// 0x00 no invertion 
		tpc_invOff = 0,

		/// 0x01 : analog invertion
		tpc_invAnalog = 1,

		/// 0x02 : Marker 1 invertion
		tpc_invM1 = 2,

		/// 0x04 : Marker 2 invertion
		tpc_invM2 = 4,
	};

	/// Specifies which protocol is currently loaded into the serial protocol trigger module.
	enum TPC_SerialTriggerProtocol {
		/// No protocol is loaded and therefore no trigger events are generated.
		tpc_serTrgProtNone,

		/// The I2C protocol is loaded.
		tpc_serTrgProtI2C,

		/// The CAN bus protocol is loaded.
		tpc_serTrgProtCAN
	};


	/// Specifies which signal from a channel should be taken to decode a serial protocol.
	enum TPC_SerialTriggerInput {
		/// Use digital data from marker bit 1 for decoding (only possible in 14 bit mode).
		tpc_serTrgMarker1,

		/// Use digital data from marker bit 2 for decoding (only possible in 14 bit mode).
		tpc_serTrgMarker2,

		/// Use analog data for decoding. 
		tpc_serTrgAnalog,
	};


	/// Trigger modes for the I2C serial protocol trigger. Also see \ref TPC_SetI2CTrigger.
	enum TPC_I2CTriggerMode {
		/// Triggers on every start condition.
		tpc_i2cEveryStart = 1,

		/// Triggers on every stop condition.
		tpc_i2cEveryStop = 4,

		/// Triggers on every restart condition.
		tpc_i2cEveryRestart = 2,

		/// Triggers on every not acknowledged byte.
		tpc_i2cEveryNack = 8,

		/// Triggers on every message with an address within the specified range.
		tpc_i2cAddress = 16,

		/// Triggers on every message with the address and data within the specified range.
		tpc_i2cAddressAndData = 48
	};


	/// Specifies on which state of the R/W bit in a I2C message should be triggered.
	enum TPC_I2CRWBit {
		/// Ignores the R/W bit. Triggers are generated for write and request for read messages.
		tpc_i2cRWIgnore,

		/// Only triggers on request for read messages (R/W bit set to '1').
		tpc_i2cRead,

		/// Only triggers on write messages (R/W bit set to '0').
		tpc_i2cWrite
	};


	/// Specifies wether to trigger on messages with 7 or 10 bit addressing.
	enum TPC_I2CAdrType {
		/// Triggers on messages with 7 bit addressing only.
		tpc_i2cAdr7Bit,

		/// Triggers on messages with 10 bit addressing only.
		tpc_i2cAdr10Bit
	};


	/// Specifies under which acknowledge circumstances trigger events should be generated.
	enum TPC_I2CAck {
		/// Triggers on all messages regardless wether data bytes are acknowledged or not.
		tpc_i2cAckIgnore,

		/// Only triggers on messages with all bytes acknowledged.
		tpc_i2cAck,

		/// Only triggers on messages with at least one byte not acknowledged.
		tpc_i2cNack
	};


	/// Trigger modes for the CAN serial protocol trigger. Also see \ref TPC_SetCANTrigger.
	enum TPC_CANTriggerMode {
		/// Triggers on every start of frame bit.
		tpc_canEveryFrame = 1,

		/// Triggers on every remote frame with an id within the specified range.
		tpc_canRemote = 16,

		/// Triggers on every frame with an id within the specified range.
		tpc_canId = 32,

		/// Triggers on data frames with an id and the correct number of data bytes within the specified range.
		tpc_canIdAndData = 2,

		/// Triggers on each error frame.
		tpc_canError = 64
	};


	/// Specifies for which message frame formats a trigger event should be generated.
	enum TPC_CANIdFormat {
		/// Triggers on messages in standard format (11 bit ID) only.
		tpc_canIdStandard = 1,

		/// Triggers on messages in extended format (29 bit ID) only.
		tpc_canIdExtended = 2,

		/// Triggers on messages in standard and extended format.
		tpc_canIdBoth = 3
	};



	/// \brief Configuration for the I2C serial protocol trigger.
	/**
	* The I2C serial protocol trigger fully supports the Standard-mode, Fast-mode, Fast-mode Plus and Ultra Fast-mode I2C protocol specifications.
	* Only one serial protocol trigger can be specified per board.
	* Refer to \ref TPC_I2CTriggerMode for an overview over all trigger possibilities. The fields starting with "adr" and the "ack" field
	* are only validated in Address or AddressAndData mode.  The fields starting with "data" are only validated in AddressAndData mode.
	*/
	struct TPC_I2CTriggerSettings {
		/// Specifies on which events should be triggered.
		TPC_I2CTriggerMode mode;

		/// Input number (0-based) of the channel the data signal is connected to.
		int chDataInputNumber;
		/// Defines from which analog or digital signal the data signal is taken from.
		TPC_SerialTriggerInput chDataInputSignal;
		/// Analog level in volts. All analog input values from the data signal below this value are interpreted as logically 0. This parameter is ignored if chDataInputSignal is not set to Analog.
		double chDataLevelL;
		/// Analog level in volts. All analog input values from the data signal above this value are interpreted as logically 1. This parameter is ignored if chDataInputSignal is not set to Analog.
		double chDataLevelH;

		/// Input number (0-based) of the channel the clock signal is connected to.
		int chClockInputNumber;
		/// Defines from which analog or digital signal the clock signal is taken from.
		TPC_SerialTriggerInput chClockInputSignal;
		/// Analog level in volts. All analog input values from the clock signal below this value are interpreted as logically 0. This parameter is ignored if chClockInputSignal is not set to Analog.
		double chClockLevelL;
		/// Analog level in volts. All analog input values from the clock signal above this value are interpreted as logically 1. This parameter is ignored if chClockInputSignal is not set to Analog.
		double chClockLevelH;

		/// Defines the acknowledge condition. This condition is only evaluated in address or address and data trigger mode.
		TPC_I2CAck ack;

		/// Defines wether to trigger on write or request for read messages only or on both message types. This condition is only evaluated in address or address and data trigger mode.
		TPC_I2CRWBit adrRWBit;
		/// Defines wether to trigger on 7 bit or 11 bit addressing messages. This condition is only evaluated in address or address and data trigger mode.
		TPC_I2CAdrType adrType;
		/// If true, trigger is generated when adrLow <= address <= adrHigh. If false, trigger is generated when address > adrHigh or address < adrLow.
		bool adrInRange;
		/// Lower end of the address range.
		int adrLow;
		/// Upper end of the address range.
		int adrHigh;
		/// All bits marked with zero in this mask are ignored for the address range check. If you do not want to make use of any address masking set this parameter to 0xffff.
		int adrMask;

		/// Specifies from which data byte onward the data should be compared (0 based). The data byte at this position is compared to the array entry at index 0. Maximum value is 253.
		int dataStartByte;
		/// Specifies the number of data bytes to compare. Valid range: 1 to 16.
		int dataLength;
		/// If true, trigger is generated when dataLow <= data <= dataHigh. If false, trigger is generated when data > dataHigh or data < dataLow.
		bool dataInRange;
		/// Lower end of the data range.
		char dataLow[16];
		/// Upper end of the data range.
		char dataHigh[16];
		/// All bits marked with zero in this mask are ignored for the data range check. If you do not want to make use of any data masking set all data bytes in this mask to 0xff.
		char dataMask[16];
	};



	/// \brief Configuration for the CAN serial protocol trigger.
	/**
	* The CAN serial protocol trigger fully supports the CAN bus 2.0B specification.
	* Only one serial protocol trigger can be specified per board.
	* Refer to \ref TPC_CANTriggerMode for an overview over all trigger possibilities. The fields starting with "id"
	* are only validated in Remote, Id or IdAndData mode.  The fields starting with "data" are only validated in IdAndData mode.
	*/
	struct TPC_CANTriggerSettings {
		/// Specifies on which events should be triggered.
		TPC_CANTriggerMode mode;

		/// Input number (0-based) of the channel the data signal is connected to.
		int chDataInputNumber;
		/// Defines from which analog or digital signal the data signal is taken from.
		TPC_SerialTriggerInput chDataInputSignal;
		/// Analog level in volts. All analog input values from the data signal below this value are interpreted as logically 0. This parameter is ignored if chDataInputSignal is not set to Analog.
		double chDataLevelL;
		/// Analog level in volts. All analog input values from the data signal above this value are interpreted as logically 1. This parameter is ignored if chDataInputSignal is not set to Analog.
		double chDataLevelH;

		/// The bit rate (bits / s) the CAN bus is running with. Valid values from 2'000 to 1'000'000.
		int bitRate;
		/// The position of the sample point within the nominal bit time. Valid values from 0 (start of bit time) to 1 (end of bit time). Typically 0.7
		double bitSamplePoint;

		/// Defines on which frame formats should be triggered. This condition is only evaluated in Id or IdAndData trigger mode.
		TPC_CANIdFormat idFormat;
		/// If true, trigger is generated when idLow <= id <= idHigh. If false, trigger is generated when id > idHigh or id < idLow.
		bool idInRange;
		/// Lower end of the id range.
		int idLow;
		/// Upper end of the id range.
		int idHigh;
		/// All bits marked with zero in this mask are ignored for the id range check. If you do not want to make use of any id masking set this parameter to 0xffffffff.
		int idMask;

		/// Specifies the number of data bytes in the message. Valid range: 1 to 8.
		int dataLength;
		/// If true, trigger is generated when dataLow <= data <= dataHigh. If false, trigger is generated when data > dataHigh or data < dataLow.
		bool dataInRange;
		/// Lower end of the data range.
		char dataLow[8];
		/// Upper end of the data range.
		char dataHigh[8];
		/// All bits marked with zero in this mask are ignored for the data range check. If you do not want to make use of any data masking set all data bytes in this mask to 0xff.
		char dataMask[8];
	};


	/// Set a parameter value to be used for the next data recording.
	/**
	* Remarks: The function sends the value immediately to the device, unless it is called between
	* \ref TPC_BeginSet and \ref TPC_EndSet.
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param parameter The parameter to be set.
	* \param value On entry: the value of the parameter to be set. On exit: the actual value 
	*          that was set (rounded to nearest possibe value that the hardware allows)
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_SetParameter(int deviceIx, int boardAddress, int inputNumber,
		enum TPC_Parameter parameter, double value);


	/// Get a parameter value. 
	/**
	* Remarks: TpcAccess keeps a local copy of all parameters of the device. Due to the network
	* there may be a little delay before a change in the parameters initiated by
	* another client is updated locally. 
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param parameter The parameter to be retrieved.
	* \param value A variable to receive the value of the parameter.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetParameter(int deviceIx, int boardAddress, int inputNumber,
		enum TPC_Parameter parameter, double *value);


	/// Set an attribute.
	/**
	* Remarks: 
	* There is no check made for valid board addresses and input numbers. You can set 
	* attributes to for any board addresses or input numbers. \n
	* To clear an attribute, set it to "".\n
	* The function sends the value immediately to the device, unless it is called between
	* \ref TPC_BeginSet and \ref TPC_EndSet.
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param key The name of the attribute.
	* \param value The value of the attribute to set.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_SetAttribute(int deviceIx, int boardAddress, int inputNumber,
		const char *key, const char *value);



	/// Get an attribute.
	/**
	* Remarks: Queries for non-existent attributes return "" without an error code.\n
	* TpcAccess keeps a local copy of all attributes of the device. Due to the network
	* there may be a little delay before a change in the attributes initiated by
	* another client is updated locally. 
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param key The name of the attribute.
	* \param buffer A variable to receive the attribute value.
	* \param maxLen The length of the buffer.
	* \returns 
	* - \ref tpc_noError if successful.
	* - \ref tpc_errBufferTooShort if the string was truncated because the buffer was too short.
	* - another error code indicating a problem with the parameters, the devices or 
	*		the network connection.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetAttribute(int deviceIx, int boardAddress, int inputNumber,
		const char *key, char *buffer, int maxLen);



	/// Callback used for enumerating all attributes stored on a device.
	/**
	* \param userData A pointer to user data as it was passed to \ref TPC_GetAllAttributes.
	* \param boardAddress The address of the board that the attribute is stored under.
	* \param inputNumber The input number that the attribute is stored under (0-based).
	* \param key The name that the attribute is stored under.
	* \param value The value of the attribute.
	* \returns The function should return true if it wishes to proceed. If the function
	*           returns false, the enumeration is aborted.
	*/
	typedef bool (TPC_CC *TPC_AttributeEnumeratorCallback)(void *userData,
		int boardAddress, int inputNumber,
		const char *key, const char *value);

	/// Get all attributes.
	/**
	* Remarks: TpcAccess keeps a local copy of all attributes of the device. Due to the network
	* there may be a little delay before a change in the attributes initiated by
	* another client is updated locally. 
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param callback Callback function to be called for every attribute stored for the device.
	* \param userData A pointer to user data that is passed to the callback function.
	* \returns - tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetAllAttributes(int deviceIx,
		TPC_AttributeEnumeratorCallback callback,
		void *userData);



	/// Used for getting or setting the associated channels in event mode. 
	/** See \ref TPC_GetAssociatedChannels and \ref TPC_SetAssociatedChannels.
	*
	* Note: Associated channels must be from the same device and from the same cluster. 
	*/
	struct TPC_AssociatedChannel {
		/// Board address of an associated channel.
		int boardAddress;
		/// Input number of an associated channel (0-based).
		int inputNumber;
	};


	/// Query the associated channels of the given input. 
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param list Receives the associated channels.
	* \param count Before entry, the maximum number of entries that \a list can accept, 
	*        after exit the actual number of entries that were stored into \a list.
	* \returns - \ref tpc_noError if successful.
	*          - \ref tpc_errBufferTooShort if there were more entries than could be stored in the list.
	*          - another error code indicating a problem with the parameters, the devices or 
	*              the network connection.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetAssociatedChannels(int deviceIx, int boardAddress, int inputNumber,
		struct TPC_AssociatedChannel *list, int *count);

	/// Set the associated channels for the given input. 
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param list Contains the associated channels from the same device.
	* \param count Number of entries in \a list.
	* \returns - tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_SetAssociatedChannels(int deviceIx, int boardAddress, int inputNumber,
		struct TPC_AssociatedChannel *list, int count);


	/// Start a compound set operation. 
	/**
	* Setting many parameters or attributes with calls to \ref TPC_SetParameter and 
	* \ref TPC_SetAttribute can cause a lot of network traffic.
	* To reduce this and increase throughput you can use a "compound set operation"
	* where a group of set commands are sent to the devices in a single block.
	* To do this surround a group of \ref TPC_SetParameter and/or \ref TPC_SetAttribute calls 
	* with \ref TPC_BeginSet and \ref TPC_EndSet. 
	*
	* After calling \ref TPC_BeginSet, all calls to \ref TPC_SetParameter and
	* \ref TPC_SetAttribute will no longer be carried out immediately. 
	* They will be deferred and stored internally in a buffer until \ref TPC_EndSet
	* or \ref TPC_CancelSet is called.
	*
	* \ref TPC_BeginSet may be called several times, but an equal number of \ref TPC_EndSet 
	* calls must be given in order to send off the values.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_BeginSet();


	/// \brief End a compound set operation. All deferred TPC_SetParameter and TPC_SetAttribute 
	/// calls are sent off to the devices.
	/**
	* Remarks: If the function returns an error code, some of the set commands may
	* and some may not have been carried out successfully. 
	* \returns 
	* - \ref tpc_noError if all of the stored set commands were carried out successful.
	* - an error code indicating a problem with some of the parameters, devices or 
	*          the network connection.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_EndSet();


	/// Cancel a compound set operation that was started with TPC_BeginSet. 
	/** None of the deferred \ref TPC_SetParameter or \ref TPC_SetAttribute calls will be carried out.
	* \returns - tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_CancelSet();


	/// \brief Sets the desired trigger mode for the specified channel.
	/**
	* Refer to \ref TPC_EasyTriggerMode and \ref TPC_EasyTriggerFlags documentation for an overview
	* over all posibilities. The trigger level and hysteresis depend on the input range. Hence, set the
	* input range before calling this function.
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param mode Trigger Mode, all coming parameters depend on this trigger mode
	* \param compMode Comperator Mode
	* \param flags Flags, used to enable AND mask or trigger multiplier
	* \param level Trigger Level in Volt
	* \param hysteresis Trigger Hysteresis in Volt
	* \param time Time used for time triggers in Samples
	* \param time2 Time2 used for time triggers in Samples
	* \returns - tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_SetTrigger(int deviceIx, int boardAddress, int inputNumber, TPC_EasyTriggerMode mode, TPC_EasyTriggerComperatorMode compMode, TPC_EasyTriggerFlags flags, double level, double hysteresis, int time = 0, int time2 = 0);


	/// \brief Reads the currently set trigger settings for the specified channel.
	/**
	* Refer to \ref TPC_EasyTriggerMode and \ref TPC_EasyTriggerFlags documentation for an overview
	* over all possibilities.
	* If flags is set to tpc_etrgSetMasterDisable, mode never holds tpc_etrgOff.
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param mode Trigger Mode, only parameters used in this mode are set
	* \param compMode Comperator Mode
	* \param flags Flags
	* \param level Trigger Level in Volt
	* \param hysteresis Trigger Hysteresis in Volt
	* \param time Time used for time triggers in Samples
	* \param time2 Time2 used for time triggers in Samples
	* \returns - tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetTrigger(int deviceIx, int boardAddress, int inputNumber, TPC_EasyTriggerMode *mode, TPC_EasyTriggerComperatorMode *compMode, TPC_EasyTriggerFlags *flags, double *level, double *hysteresis, int *time, int *time2);

	/// \brief Resets the serial protocol trigger for the specified board. After this command the serial protocol trigger is no more generating any triggers.
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \returns - tpc_noError if successful, otherwise an error code indicating the problem.
	**/	
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ResetSerTrg(int deviceIx, int boardAddress);


	/// \brief Queries the currently loaded protocol in the serial protocol trigger module.
	/**
	* Refer to \ref TPC_SerialTriggerProtocol for a list of all available protocols.
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param protocol The currently loaded protocol.
	* \returns - tpc_noError if successful, otherwise an error code indicating the problem.
	**/	
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetSerTrgProtocol(int deviceIx, int boardAddress, TPC_SerialTriggerProtocol *protocol);


	/// \brief Configures the specified board to decode the I2C protocol as specified with the settings parameter.
	/**
	* Only one serial protocol trigger can be specified per board. For further details see \ref TPC_I2CTriggerSettings.
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param settings The trigger specification settings.
	* \returns - tpc_noError if successful, otherwise an error code indicating the problem.
	**/	
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_SetI2CTrigger(int deviceIx, int boardAddress, TPC_I2CTriggerSettings *settings);


	/// \brief Returns the current configuration of the I2C protocol trigger for the specified board. Check with \ref TPC_GetSerTrgProtocol beforehand if this function is applicable.
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param settings The trigger specification settings.
	* \returns - tpc_noError if successful, otherwise an error code indicating the problem.
	**/	
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetI2CTrigger(int deviceIx, int boardAddress, TPC_I2CTriggerSettings *settings);


	/// \brief Configures the specified board to decode the CAN protocol as specified with the settings parameter.
	/**
	* Only one serial protocol trigger can be specified per board. For further details see \ref TPC_CANTriggerSettings.
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param settings The trigger specification settings.
	* \returns - tpc_noError if successful, otherwise an error code indicating the problem.
	**/	
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_SetCANTrigger(int deviceIx, int boardAddress, TPC_CANTriggerSettings *settings);


	/// \brief Returns the current configuration of the CAN trigger for the specified board. Check with \ref TPC_GetSerTrgProtocol beforehand if this function is applicable.
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param settings The trigger specification settings.
	* \returns - tpc_noError if successful, otherwise an error code indicating the problem.
	**/	
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetCANTrigger(int deviceIx, int boardAddress, TPC_CANTriggerSettings *settings);




	/*@}*/



	//=== Commands =========================================================================

	/**
	* \addtogroup SystemCommands
	*
	* Commands to start and stop a measurement.
	* The commands are given to the system as a whole.
	*
	*
	* @{
	*/


	/// Constants for use with \ref TPC_ExecuteSystemCommand.
	enum TPC_SystemCommand {
		tpc_cmdStart = 0,		 ///< Start the measurement.
		tpc_cmdStop,			 ///< Abort the measurement if it is running.
		tpc_cmdDisarm,			 ///< Disable triggers.
		tpc_cmdArm,				 ///< Enable triggers.
		tpc_cmdTrigger,			 ///< Provoke a trigger from software.
		tpc_cmdReserved1,		 ///< Reserved command
		tpc_cmdArmExtCommands,	 ///< Enable external commands.
		tpc_cmdDisarmExtCommands,///< Disable external commands.
		tpc_cmdStartGPSSynced,	 ///< Start the measurement synced to a GPS PPS signal
		tpc_cmdInitSync			/// ReInit Device Synchronisation after cabling changes
	};

	/// Execute a system command.
	/**
	* A system command can only be executed if all 
	* devices of the system are connected, initialized and are responding. 
	* If there is a problem with one or more devices the system command 
	* cannot be given.
	* \param command The command to be executed.
	* \returns 
	* - \ref tpc_noError if all devices of the system are connected and accepting commands.
	* - \ref tpc_errWrongSystemState if the current system status does not allow the 
	*              specified command.
	* - another error code indicating a problem with some of the devices or the 
	*              network connection. 
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ExecuteSystemCommand(enum TPC_SystemCommand command);

	/// Execute a system command.
	/**
	* A system command can only be executed if all 
	* devices of the system are connected, initialized and are responding. 
	* If there is a problem with one or more devices the system command 
	* cannot be given.
	* \param id System identifier ID generated by \ref TPC_NewSystem 
	* \param command The command to be executed.
	* \returns 
	* - \ref tpc_noError if all devices of the system are connected and accepting commands.
	* - \ref tpc_errWrongSystemState if the current system status does not allow the 
	*              specified command.
	* - another error code indicating a problem with some of the devices or the 
	*              network connection. 
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ExecuteSystemCommandSystem(int id, enum TPC_SystemCommand command);


	/// Makes a measurement.
	/**
	* Sends a start command to the system and waits until the measurement is finished or the
	* specified timeout is over.
	* \param timeout Timeout in milliseconds.
	* \param measurementNumber New measurement number.
	* \returns 
	* - tpc_noError if the measurement could be made.
	* - or an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_MakeMeasurement(int timeout, int *measurementNumber);



	/*@}*/
	/**
	 * \addtogroup DeviceCommands
	 *
	 *
	 * In this section you will find functions to start
	 * an auto calibration. .
	 * @{
	 */


	/// Execute a calibration command on one device.
	/**
	* Remarks: This function returns immediately after the device has
	*          been given the command to perform the calibration. The
	*          calibration may take a few seconds. The field 'deviceError'
	*          in the device state information (see \ref TPC_GetDeviceStatus)
	*          will be set to tpc_errCalibrating during this time and return 
	*          to tpc_noError when the calibration is finished.
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_StartCalibration(int deviceIx);

	/*@}*/

	/**
	* \addtogroup DeviceStatus
	 * @{
	*/

	/// Auto Sequence State
	enum TPC_AUTOSEQ_STATUS {
		/// No Autosequence File is loaded or present
		AUT_NO_SEQUENCE,
	
		/// Autosequence is loaded
		AUT_LOADED,
	
		/// Autosequence  load settings
		AUT_RUNNING_LS,

		/// Autosequence measuring is running
		AUT_RUNNING_MEAS,

		/// Autosequence wait for end of recording
		AUT_RUNNING_WEOR,

		/// Autosequence Delay
		AUTO_RUNNING_DELAY,

		/// Autosequence Calibration
		AUTO_RUNNING_CALIB,

		/// Autosequence saving
		AUTO_RUNNING_SAVE,

		// Autosequence stoped or finished
		AUTO_STOP
	};

	/*@}*/

	//=== Device status ====================================================================

	/**
	 * \addtogroup DeviceStatus
	 *
	 * TpcAccess keeps a copy of the status of each device. This comprises:
	 *	- Parameters
	 *	- Attributes
	 *	- Recording state
	 *
	 * An application program has several methods to find out about changes in the status:
	 * - It can poll periodically. For each part of the status there is a counter 
	 *   that is incremented with each change, so the application can compare with the last 
	 *   counter values to detect changes and then react accordingly.
	 * - The application can install callbacks. (See \ref TPC_SetStatusCallback)
	 *   Please note: The callbacks are called from a different thread than the application 
	 *   main thread. The callbacks should return quickly. They are intended to send some kind 
	 *   of notification to your application, e.g. waking up another thread, setting a flag 
	 *   or posting a message into the message queue of a window. Do not do extensive data 
	 *   processing in this callback.
	 *
	 * @{
	 */

	/*#####
	 *	- Cluster configuration
	
	Cluster numbers can be queried via TPC_GetClusterNumbers(). Parameters and Attributes 
	can be queried via TPC_GetParameter() and TPC_GetAttribute(). The recording state can be 
	queried with TPC_GetRecordState().
	
	
	*/

	/// Recording state
	enum TPC_RecordingState {
		/// The measurement was aborted with a stop command.
		tpc_recAborted,

		/// The device has been prepared for a new start command.
		tpc_recStarting,

		/// The recording is in progress
		tpc_recRecording,

		/// The measurement has finished normally.
		tpc_recStopped,

		/// The device did not start properly (problem with SyncLink or StarHub).
		tpc_recStartError

	};



	/// Trigger state
	enum TPC_TriggerState {
		/// The system is disarmed due to pretrigger delay or a disarm command.
		tpc_recDisarmed,

		/// The system is ready to detect a trigger condition.
		tpc_recArmed,

		/// A trigger has been detected, the block will be finished.
		tpc_recTriggered,
	};

	/// External Start Recording State
	enum TPC_ExtStartState {
	
		/// External start signal is not active
		tpc_extStartDisarmed,

		/// External start signal is active
		tpc_extStartArmed,
	};
    /// ADC Input Overload State Bit Mask
	enum TPC_OverloadState {
		/// Input signal is inside the input range
		tpc_overloadNone = 0 ,	
		/// Input signal is over the higher input limit
		tpc_overloadPositive = 1,
		/// Input signal is under the lower input limit
		tpc_overloadNegative = 2,
	};
    /// Contains information about the input status, see bit mask \ref TPC_OverloadState
	struct TPC_InputStatus{
		/// Actual (live) overload status
		int OverloadLive;
		/// Overload status during this measurement.
		int OverloadMeas;
	};

	/// Contains information about the status of a board. 
	/** This status is identical
	* for all members of a cluster, so it is sufficient to inspect the statue of
	* one board of each cluster.
	*/
	struct TPC_BoardStatus {
		/// Current recording status. See TPC_RecordingState.
		/// Please note: This field reflects the recording state of the cluster that
		/// this board is a member of. 
		enum TPC_RecordingState recordingState;

		/// Current trigger status. See TPC_TriggerState.
		/// In block mode this field reflects the trigger state of the current block.
		/// In continuous mode this field reflects the stop trigger state.
		enum TPC_TriggerState	triggerState;

		/// Block mode and event recorder: Number of blocks with valid data, blockCounter >= 0.
		/// Continuous and Dual Mode: This includes the growing block 0, so blockCounter >= 1.
		int						blockCounter;

		/// Continuous and Dual mode: Number of samples recorded so far (available in block 0).
		int64_t					dataCounter;

		/// Total number of samples lost in Continuous or Dual mode 
		/// (this indicates that the sampling frequency is too high).
		int						dataLostCounter;

		/// Number of times that samples were lost in Continuous or Dual mode
		/// (this indicates that the sampling frequency is too high).
		int						blockLostCounter;
		
		/// Input status information
		struct TPC_InputStatus	inputs[tpc_maxInputs];
	};

	/// GPS Receiver Status
	struct TPC_GPSStatus{
		bool		locked;
		int			NrOfSatVisible;
		char		gpsTime[128];
		char		lastFrame[128];
		int			nrOfloggedPositions;
	};

	/// Temperature Status
	struct TPC_TempStatus{
		int	CPUTemp;
		int BoardTemp[tpc_maxBoards];
		int Res1Temp;
		int Res2Temp;
	};
	
	/// Contains information about the status of a device.
	struct TPC_DeviceStatus {
		/// This field has the value tpc_noError if the device was initialized correctly,
		/// the network connection could be established and the device is responding.
		/// During the auto calibration phase of a device this field has the value
		/// tpc_errCalibrating. If the connection to the device could not be established
		/// or is lost at present, this field has the value tpc_errNoConnection. If the 
		/// device has an internal problem (software or hardware failure), this field 
		/// contains the corresponding error code.
		enum TPC_ErrorCode		deviceError;

		/// This number changes with each start command. This number is also used to retrieve data or meta data.
		int						measurementNumber;

		/// Time stamp of measurement start
		struct TPC_DateTime		measurementStartTime;

		/// True during the time between preparation of the start of the measurement and the actual start.
		bool					startInProgress;

		/// Status info about the individual boards.
		struct TPC_BoardStatus boards[tpc_maxBoards];

		/// Counter that increments with parameter, attribute or cluster configuration changes.
		/// The intention if this counter is to allow for easy detection of changes in the 
		/// settings without comparing all the settings values.
		int						settingsChangesCounter;


		/// Cluster number that each board is recording with. (Not implemented yet. Is always zero.).
		int clusterNrs[tpc_maxBoards];
	
		/// Operation mode that each board is recording with.
		int operationModes[tpc_maxBoards];

		/// Autosequence Status
		enum TPC_AUTOSEQ_STATUS autosequenceState;

		/// External Start State
		enum TPC_ExtStartState	extStartState;

		/// GPS Status
		struct TPC_GPSStatus gpsStatus;
		
		/// Temp Status
		struct TPC_TempStatus tempStatus;
	};


	/// Return the current status of a device.
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param status A variable to receive the status information.
	* \param structSize The size of the \a inputInfo struct in bytes.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetDeviceStatus(int deviceIx, struct TPC_DeviceStatus *status, int structSize);


	//
	//struct TPC_SystemStatus
	//{
	//	/// This field is non-zero if one of the devices of the system is having 
	//	/// problems with the connection, hardware or driver initialization 
	//	/// problems. If this field is not tpc_errNoError, no system command can 
	//	/// be given.
	//	int systemError;	 
	//};


	//// Get the system status.
	//// status - a variable to receive the status information.
	//// structSize - size of the inputInfo struct in bytes.
	//// returns - tpc_noError if successful, otherwise an error code indicating the problem.
	//TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetSystemStatus(struct TPC_SystemStatus* status, int structSize);
	//

	/// Signature for a callback function that can be installed with \ref TPC_SetStatusCallback.
	/**
	*  Example: 
	*  - Declare a function pointer of type TPC_StatusCallbackFunc: <br>
	*    <TT>TPC_StatusCallbackFunc myCallbackFunctionPointer; </TT> <br>
	*  - Define a handle function which handle the callback:  <br>
	*    <TT>void au StatusCallback(void* userData, int deviceIx, struct TPC_DeviceStatus* status){ <br>
	*    } </TT> <br>
	*  - Define the the function pointer: <br>
	*    <TT>myCallbackFunctionPointer = (TPC_StatusCallbackFunc)StatusCallback; </TT> <br>
	*
	*  - Call the \ref TPC_SetStatusCallback function with callback = myCallbackFunctionPointer
	*
	* \param deviceIx The device index of the device whose status has changed.
	* \param status The new status of the device.
	* \param userData A pointer to user data as it was passed to \ref TPC_SetStatusCallback.
	*/
	typedef void (TPC_CC *TPC_StatusCallbackFunc)(void *userData, int deviceIx, struct TPC_DeviceStatus *status);


	/// Install a status change callback function. 
	/**
	* The function will be called for every status change for every device individually.
	* \param callback A callback function that will be called when status changes are detected.
	* \param userData A pointer to user data that will be passed to the callback function.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_SetStatusCallback(TPC_StatusCallbackFunc callback, void *userData);

	/// Install a status change callback function. 
	/**
	* The function will be called for every status change for every device individually.
	* \param id System identifier ID generated by \ref TPC_NewSystem
	* \param callback A callback function that will be called when status changes are detected.
	* \param userData A pointer to user data that will be passed to the callback function.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_SetStatusCallbackSystem(int id, TPC_StatusCallbackFunc callback, void *userData);

	/*@}*/


	//=== Data readout =====================================================================

	/**
	 * \addtogroup DataReadout
	 *
	The data consists of two parts:
	- The measured data samples, either as binary words or as floating point numbers.
	- Meta data which contains information about the interpretation of the data samples.
	
	The meta data can be useful in determining what part of the trace should be
	read out (e.g. desired time range -> actual sample index range), and how to 
	interpret the data (e.g. binary sample values -> values in Volt).
	
	The meta data is divided in two parts:
	- information about the amplitude (y meta data)
	- information about the time axis (t meta data)
	
	y meta data can be different for each input channel, but it stays the same for all 
	recorded blocks of that input during one acquisition series. t meta data can be 
	different for each recorded block and for each cluster, but it is the same for all 
	boards of the cluster. Exception: In single channel event recorder mode, each device
	that belongs to the cluster has a different set of t meta data records!
	
	The y meta data is available right after the start command has been given. However, some
	parts of the t meta data cannot be determined until the recording of a block has been
	completed. Depending on the recording mode these are: the absolute time of the trigger 
	(expressed as time since the start command), the block length and the channel reference 
	(in event recorder mode). It is possible to read the t meta data before the
	block is completed for block zero in Continuous and Dual mode. In Multi block
	mode or Event recorder mode, the t meta data should only be read after the
	corresponding block has been finished.
	
	TpcAccess allows two scaling modes for reading out data: raw data and scaled data.
	Raw data consists of the binary words as they are measured by the ADC, including the
	digital marker bits. Scaled data come as floating point numbers, scaled to Volt.
	
	You can also choose between reading out a data range or an envelope. When reading
	a data range you define which part of the measured block you want to read by 
	giving a start sample index and the length in samples and then you get all samples
	in that range. When reading an envelope you also specify which part of the block you 
	want to read by giving a start sample index and the length in samples, but you also
	give the number of envelope packets you want to get. The number of envelope packets is
	always smaller than the number of samples in the range you want to read. TpcAccess 
	then computes the envelope by dividing the specified data range into packets of 
	equal size and finds the minimum and maximum amplitude of each of them. This is 
	done very efficiently and allows to draw envelopes on the screen very quickly.
	
	Reading many small pieces of data with calls to TPC_GetXxxData can cause a lot of network 
	traffic. To increase throughput you can use a "compound get data operation" where
	a group of data requests and replies are exchanged with the devices in a single block.
	To do this use the TPC_DeferredGetXxxData functions instead of the TPC_GetXxxData calls. 
	These calls are not processed immediately but stored in an internal list. After all 
	requests are placed, the stored calls can be sent off to the devices collectively with 
	a call to TPC_ProcessDeferredDataRequests.
	 *
	 * @{
	 */


	/// Y-Axis meta data.
	/** Y-Axis meta data describes properties of the recorded data
	*  related to the amplitude axis.
	*/
	struct TPC_YMetaData {
		//--- status information
		/// True if the input is active and produces data.
		bool		inputActive;			

		//--- data information
		/// Mask to blind out marker bits from the binary raw data.
		uint32_t	analogMask;		

		/// Mask to blind out analog bits from the binary raw data.
		uint32_t	markerMask;		

		/// Number of marker bits. Markers are the rightmost bits in the sample word.
		int			numberOfMarkerBits;

		/// Effective resolution of the signal in bits. 
		/** (This can be higher than the ADC resolution if averaging is switched on.)*/
		int			resolutionInBits;		

		/// 2 or 4. Important for reading out raw binary data (used internally).
		int			bytesPerSample;			

		/// Conversion factor binary ADC values to volt. 
		/** (volt = (binary&analogMask)*factor + constant.) */
		double		binToVoltFactor;		

		/// Offset for conversion binary ADC values to volt. 
		double		binToVoltConstant;	

		/// Conversion factor binary ADC values to physical unit. 
		/** (physical unit = (binary&analogMask)*factor + constant.) */
		double		binToPhysicalFactor;		

		/// Offset for conversion binary ADC values to physical unit. 
		double		binToPhysicalConstant;	

		/// Conversion factor Volt to physical unit. 
		/** (physical unit = volt*factor + constant.) */
		double		voltToPhysicalFactor;		

		/// Offset for conversion Volt to physical unit. 
		double		voltToPhysicalConstant;	

		/* 
		In principle a copy of all attributes, made at the time of the start command,
		belongs here, too. To place that into this structure is not well practicable, 
		however, but you can query the attributes with the function TPC_GetMetaDataAttribute().
		
		The same is true for the channel associations in event recorder mode. To
		query them use the function TPC_GetMetaDataAssociatedChannels().
		*/
	};



	/// Get y meta data information. 
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param measurementNumber The number of the measurement. This number is found in the device status.
	* \param metaData A variable to receive the meta data information for the requested block.
	* \param structSize Size of the metaData struct in bytes.
	* \returns 
	* - \ref tpc_noError if successful.
	* - \ref tpc_errNewMeasurement if the data is no longer available because a new measurement 
	*               has been started.
	* - another error code indicating a problem with the device, the network connection or 
	*               the function parameters.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetYMetaData(int deviceIx, int boardAddress, int inputNumber,
		int measurementNumber,
		struct TPC_YMetaData *metaData, int structSize);



	/// Get a recording parameter value from the copy made at the time of the start command.
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param measurementNumber The number of the measurement. This number is found in the device status.
	* \param parameter The parameter to be retrieved.
	* \param value A variable to receive the value of the parameter.
	* \returns 
	* - \ref tpc_noError if successful.
	* - \ref tpc_errNewMeasurement if the data is no longer available because a new measurement 
	*               has been started.
	* - another error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetMetaDataParameter(int deviceIx, int boardAddress, int inputNumber,
		int measurementNumber, enum TPC_Parameter parameter, double *value);

	/// Get an attribute value from the copy made at the time of the start command.
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param measurementNumber The number of the measurement. This number is found in the device status.
	* \param key The name of the attribute
	* \param buffer A variable to receive the attribute value
	* \param maxLen The length of the buffer.
	* \returns 
	*  - \ref tpc_noError if successful.
	*  - \ref tpc_errNewMeasurement if the data is no longer available because a new measurement 
	has been started.
	*  - \ref tpc_errBufferTooShort if the string was truncated because the buffer was too short.
	*  - another error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetMetaDataAttribute(int deviceIx, int boardAddress, int inputNumber,
		int measurementNumber,
		const char *key, char *buffer, int maxLen);



	/// Get all attributes from the copy made at the time of the start command.
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param measurementNumber The number of the measurement. This number is found in the device status.
	* \param callback A callback function to be called for every attribute stored on the device.
	* \param userData A pointer to user data that is passed to the callback function.
	* \returns - \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetAllMetaDataAttributes(int deviceIx,
		int measurementNumber,
		TPC_AttributeEnumeratorCallback callback, void *userData);


	/// Query the channels that were associated to the given input at the time of the start command. 
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param measurementNumber The number of the measurement. This number is found in the device status.
	* \param list Receives the associated channels.
	* \param count Before entry, the maximum number of entries that \a list can accept, 
	*             after exit the actual number of entries that were stored into \a list.
	* \returns 
	* - \ref tpc_noError if successful.
	* - \ref tpc_errBufferTooShort if there were more entries than could be stored in the list.
	* - another error code indicating a problem with the parameters, the devices or 
	*		the network connection.  
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetMetaDataAssociatedChannels(int deviceIx, int boardAddress, int inputNumber,
		int measurementNumber, struct TPC_AssociatedChannel *list, int *count);


	/// T-Axis meta data.
	/** T-Axis meta data describes properties of the recorded data
	*  related to the time axis.
	*/
	struct TPC_TMetaData {
		//--- status information

		/// Indication if the blockLength field is still changing.
		/** If this field is true, then the block contains valid data and can be 
		*  read out, but the recording is not completed yet. The block size is still
		*  growing and additional data will be appended to the block at a later time.
		*  (Block 0 in Continuous and Dual mode).
		*  If this field is false, the block is finished and its block size will
		*  not change any more.
		*/
		bool				growing;


		//--- time axis information

		/// Time base source, 0: internal, 1: external
		int					timeBaseSource;

		/// Sample rate in Hertz, valid when timeBaseSource == 0
		double				sampleRate;	

		/// Divisor for the external clock, valid when timeBaseSource == 1
		int					externalClockDivisor;

		/// Date and time of the start command.
		struct TPC_DateTime startTime;

		/// The absolute time of the trigger sample expressed as pico seconds since the start command. 
		/*  This field does not have a valid value if the block is not
		 *  completed (except for the continuous mode, where the trigger is considered
		 *  to be at sample zero).
		    */
		uint64_t			triggerTime;

		/// The index of the trigger sample. 
		/** The trigger sample is the sample that is associated with time zero.*/
		int64_t				triggerSample;

		/// The size of the recorded data.
		/** This field continuously grows in Continuous mode while the measurement is in 
		 *  progress. Use this field to determine how much of the data can already be 
		 *  read out.
		 */
		uint64_t			blockLength;

		/// The index of the stop trigger sample, for those modes that have a stop trigger.
		uint64_t			stopTriggerSample;


		//--- information for event recorder mode.

		/// Indication if only one channel or all channels have data for this block.
		/** If this field is true, data is available only for a single channel
		 *  which is indicated by the fields \a boardAddress and \a inputNumber.
		 *  If this field is false, data is available on all input channels in the
		 *  same cluster.
		    */
		bool				singleChannel;

		/// Used in event recorder mode: The board that recorded the event.
		int					boardAddress;

		/// Used in event recorder mode: The number of the input (0-based) that recorded the event.
		int					inputNumber;
	};


	/// Get t meta data information about a recorded block. 
	/**
	* remarks: The t meta data can be read for any block that is completed. It can be read
	*       for incomplete blocks in the following cases:
	*       - Block zero in Continuous mode or Dual mode (with blockLength continuously growing)
	*       - In MultiBlock and Scope mode. The 'triggerTime' field does not have a value 
	*       until the recording of the block is completed.
	*
	*    Please note: In sincle channel event recorder mode, each device that belongs to 
	*    the same cluster has a different set of t meta data records! 
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param blockNumber The number of the block.
	* \param measurementNumber The number of the measurement. This number is found in the device status.
	* \param metaData A variable to receive the meta data information for the requested block.
	* \param structSize Size of the metaData struct in bytes.
	* \returns 
	* - \ref tpc_noError if successful.
	* - \ref tpc_errNewMeasurement if the data is no longer available because a new measurement 
	*      has been started.
	* - another error code indicating a problem with the device, the network connection or 
	*      The function parameters.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetTMetaData(int deviceIx, int boardAddress, int blockNumber,
		int measurementNumber,
		struct TPC_TMetaData *metaData, int structSize);

	/// Get t meta data information about several blocks. 
	/**
	* remarks: The t meta data can be read for any block that is completed. It can be read
	*       for incomplete blocks in the following cases:
	*       - Block zero in Continuous mode or Dual mode (with blockLength continuously growing)
	*       - In MultiBlock and Scope mode. The 'triggerTime' field does not have a value 
	*       until the recording of the block is completed.
	*
	*    Please note: In sincle channel event recorder mode, each device that belongs to 
	*    the same cluster has a different set of t meta data records! 
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param blockNumberFrom The number of the first block.
	* \param blockNumberTo	  The number of the last block
	* \param measurementNumber The number of the measurement. This number is found in the device status.
	* \param metaData A variable to receive the meta data information for the requested block.
	* \param structSize Size of the metaData struct in bytes.
	* \returns 
	* - \ref tpc_noError if successful.
	* - \ref tpc_errNewMeasurement if the data is no longer available because a new measurement 
	*      has been started.
	* - another error code indicating a problem with the device, the network connection or 
	*      The function parameters.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetAllTMetaData(int deviceIx, int boardAddress, int blockNumberFrom,
		int blockNumberTo, int measurementNumber,
		struct TPC_TMetaData *metaData, int structSize);

	/// Read out data from a recorded block. The data is scaled to volts.
	/**
	* Remarks:
	* To convert to volts to physical user units use the scaling 
	* factor and constant provided in the y meta data.
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param blockNumber The number of the block.
	* \param measurementNumber The number of the measurement. This number is found in the device status.
	* \param dataStart Sample index of the first sample of the part to be read
	* \param dataLength The length of the part to be read
	* \param data The array to receive the data. 
	* \returns 
	* - \ref tpc_noError if successful.
	* - \ref tpc_errNewMeasurement if the data is no longer available because a new measurement 
	*          has been started.
	* - \ref tpc_errNoData if an attempt is made to read data from an inactive input.
	* - another error code indicating a problem with the device, the network connection or 
	*          the function parameters.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetData(int deviceIx, int boardAddress, int inputNumber, int blockNumber,
		int measurementNumber, uint64_t dataStart, int dataLength, double *data);


	/// Read out raw ADC data from a recorded block. 
	/**
	* Remarks:
	* The raw ADC data can be converted to volts by masking off the marker bits and using the scaling
	* factor and constant: v = (x & mask)*f+c. These values can be obtained from the y meta data.
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param blockNumber The number of the block.
	* \param measurementNumber The number of the measurement. This number is found in the device status.
	* \param dataStart Sample index of the first sample of the part to be read
	* \param dataLength The length of the part to be read
	* \param data The array to receive the data. 
	* \returns 
	* - \ref tpc_noError if successful.
	* - \ref tpc_errNewMeasurement if the data is no longer available because a new measurement 
	*          has been started.
	* - \ref tpc_errNoData if an attempt is made to read data from an inactive input.
	* - another error code indicating a problem with the device, the network connection or 
	*          the function parameters. 
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetRawData(int deviceIx, int boardAddress, int inputNumber, int blockNumber,
		int measurementNumber, uint64_t dataStart, int dataLength, int32_t *data);


	/// Read out envelope data from a recorded block. The data is scaled to volts.
	/**
	* Remarks:
	* To convert to volts to physical user units use the scaling factor 
	* and constant provided in the y meta data.
	*
	* Set the \a resultLength to twice the number of min/max pairs you want to read. 
	* The data length must not be smaller than the number of min/max pairs to read. 
	* The data from \a dataStart to \a dataStart + \a dataLength - 1 will be divided 
	* into (\a resultLength / 2) segments of nearly equal size and the result 
	* will contain min and max for each semgent alternately. 'Nearly equal size' 
	* means that the segments are of size (\a dataLength / numSegments) or 
	* (\a dataLength / numSegments)+1 each. 
	*
	* Example: Read out the data from 100 to 152 and fill 5 min/max pairs. The 
	* \a resultLength parameter will be given as 5x2=10. The segments will be 
	* 100..109, 110..120, 121..130, 131..141, 142..152. The array contains 
	* [min seg1, max seg1, min seg2, max seg2, ...]
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param blockNumber The number of the block.
	* \param measurementNumber The number of the measurement. This number is found in the device status.
	* \param dataStart Sample index of the first sample of the part to be read
	* \param dataLength The length of the part to be read
	* \param resultLength The length of the \a data array in samples. Twice the number of min/max pairs to read.
	* \param data The array to receive the data. 
	* \returns 
	*  - \ref tpc_noError if successful.
	*  - \ref tpc_errNewMeasurement if the data is no longer available because a new measurement 
	*          has been started.
	*  - \ref tpc_errNoData if an attempt is made to read data from an inactive input.
	*  - another error code indicating a problem with the device, the network connection or 
	*          the function parameters.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetMinMaxData(int deviceIx, int boardAddress, int inputNumber, int blockNumber,
		int measurementNumber, uint64_t dataStart, uint64_t dataLength,
		int resultLength, double *data);


	/// Read out envelope raw ADC data from a recorded block. 
	/**
	* Remarks:
	* The raw ADC data can be converted to volts by masking off the marker bits and using the scaling 
	* factor and constant: v = (x & mask)*c+f. These values can be obtained from the y meta data.
	*
	* Set the \a resultLength to twice the number of min/max pairs you want to read. 
	* The data length must not be smaller than the number of min/max pairs to read. 
	* The data from \a dataStart to \a dataStart + \a dataLength - 1 will be divided 
	* into (\a resultLength / 2) segments of nearly equal size and the result 
	* will contain min and max for each semgent alternately. 'Nearly equal size' 
	* means that the segments are of size (\a dataLength / numSegments) or 
	* (\a dataLength / numSegments)+1 each. 
	*
	* Example: Read out the data from 100 to 152 and fill 5 min/max pairs. The 
	* \a resultLength parameter will be given as 5x2=10. The segments will be 
	* 100..109, 110..120, 121..130, 131..141, 142..152. The array contains 
	* [min seg1, max seg1, min seg2, max seg2, ...]
	*
	* For the analog part, min and max contain the least and greatest values.
	* For the digital part, min contains the bit wise AND of all samples, max the OR.
	*
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param blockNumber The number of the block.
	* \param measurementNumber The number of the measurement. This number is found in the device status.
	* \param dataStart Sample index of the first sample of the part to be read
	* \param dataLength The length of the part to be read
	* \param resultLength The length of the \a data array in samples. Twice the number of min/max pairs to read.
	* \param data The array to receive the data. 
	* \returns 
	*  - \ref tpc_noError if successful.
	*  - \ref tpc_errNewMeasurement if the data is no longer available because a new measurement 
	*          has been started.
	*  - \ref tpc_errNoData if an attempt is made to read data from an inactive input.
	*  - another error code indicating a problem with the device, the network connection or 
	*          the function parameters.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetRawMinMaxData(int deviceIx, int boardAddress, int inputNumber, int blockNumber,
		int measurementNumber, uint64_t dataStart, uint64_t dataLength,
		int resultLength, int32_t *data);


	/// Place a deferred request to read out data from a recorded block. The data is scaled to volts.
	/** The actual reading is deferred until \ref TPC_ProcessDeferredDataRequests is called.
	* This allows to use the network more efficiently by sending several requests in one group.
	*
	* To convert to volts to physical user units use the scaling factor and constant 
	* provided in the y meta data.
	*
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param blockNumber The number of the block.
	* \param dataStart Sample index of the first sample of the part to be read
	* \param dataLength The length of the part to be read
	* \param data The array to receive the data when the deferred request is carried out. 
	* \param error A variable to receive the error code when the deferred request is carried out.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_DeferredGetData(int deviceIx, int boardAddress, int inputNumber, int blockNumber,
		uint64_t dataStart, int dataLength, double *data, enum TPC_ErrorCode *error);



	/// Place a deferred request to read out raw ADC data from a recorded block. 
	/** The actual reading is deferred until \ref TPC_ProcessDeferredDataRequests is called.
	* This allows to use the network more efficiently by sending several requests in one group.
	*
	* The raw ADC data can be converted to volts by masking off the marker bits and using the scaling
	* factor and constant: v = (x & mask)*c+f. These values can be obtained from the y meta data.
	*
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param blockNumber The number of the block.
	* \param dataStart Sample index of the first sample of the part to be read
	* \param dataLength The length of the part to be read
	* \param data The array to receive the data when the deferred request is carried out. 
	* \param error A variable to receive the error code when the deferred request is carried out.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_DeferredGetRawData(int deviceIx, int boardAddress, int inputNumber, int blockNumber,
		uint64_t dataStart, int dataLength, int32_t *data, enum TPC_ErrorCode *error);



	/// Place a deferred request to read out envelope data from a recorded block. The data is scaled to volts.
	/**
	* The actual reading is deferred until \ref TPC_ProcessDeferredDataRequests is called.
	* This allows to use the network more efficiently by sending several requests in one group.
	*
	* To convert the volts to physical user units use the scaling factor 
	* and constant provided in the y meta data.
	*
	* Set the \a resultLength to twice the number of min/max pairs you want to read. 
	* The data length must not be smaller than the number of min/max pairs to read. 
	* The data from \a dataStart to \a dataStart + \a dataLength - 1 will be divided 
	* into (\a resultLength / 2) segments of nearly equal size and the result 
	* will contain min and max for each semgent alternately. 'Nearly equal size' 
	* means that the segments are of size (\a dataLength / numSegments) or 
	* (\a dataLength / numSegments)+1 each. 
	*
	* Example: Read out the data from 100 to 152 and fill 5 min/max pairs. The 
	* \a resultLength parameter will be given as 5x2=10. The segments will be 
	* 100..109, 110..120, 121..130, 131..141, 142..152. The array contains 
	* [min seg1, max seg1, min seg2, max seg2, ...]
	*
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param blockNumber The number of the block.
	* \param dataStart Sample index of the first sample of the part to be read
	* \param dataLength The length of the part to be read
	* \param resultLength The length of the \a data array in samples. Twice the number of min/max pairs to read.
	* \param data The array to receive the data when the deferred request is carried out. 
	* \param error A variable to receive the error code when the deferred request is carried out.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_DeferredGetMinMaxData(int deviceIx, int boardAddress, int inputNumber, int blockNumber,
		uint64_t dataStart, uint64_t dataLength,
		int resultLength, double *data, enum TPC_ErrorCode *error);




	/// Place a deferred request to read out envelope raw ADC data from a recorded block. 
	/**
	* The actual reading is deferred until \ref TPC_ProcessDeferredDataRequests is called.
	* This allows to use the network more efficiently by sending several requests in one group.
	*
	* The raw ADC data can be converted to volts by masking off the marker bits and using the scaling
	* factor and constant: v = (x & mask)*c+f. These values can be obtained from the y meta data.
	*
	* Set the \a resultLength to twice the number of min/max pairs you want to read. 
	* The data length must not be smaller than the number of min/max pairs to read. 
	* The data from \a dataStart to \a dataStart + \a dataLength - 1 will be divided 
	* into (\a resultLength / 2) segments of nearly equal size and the result 
	* will contain min and max for each semgent alternately. 'Nearly equal size' 
	* means that the segments are of size (\a dataLength / numSegments) or 
	* (\a dataLength / numSegments)+1 each. 
	*
	* Example: Read out the data from 100 to 152 and fill 5 min/max pairs. The 
	* \a resultLength parameter will be given as 5x2=10. The segments will be 
	* 100..109, 110..120, 121..130, 131..141, 142..152. The array contains 
	* [min seg1, max seg1, min seg2, max seg2, ...]
	*
	* For the analog part, min and max contain the least and greatest values.
	* For the digital part, min contains the bit wise AND of all samples, max the OR.
	*
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param boardAddress The address of the board.
	* \param inputNumber The number of the input (0-based).
	* \param blockNumber The number of the block.
	* \param dataStart Sample index of the first sample of the part to be read
	* \param dataLength The length of the part to be read
	* \param resultLength The length of the \a data array in samples. Twice the number of min/max pairs to read.
	* \param data The array to receive the data when the deferred request is carried out. 
	* \param error A variable to receive the error code when the deferred request is carried out.
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_DeferredGetRawMinMaxData(int deviceIx, int boardAddress, int inputNumber, int blockNumber,
		uint64_t dataStart, uint64_t dataLength,
		int resultLength, int32_t *data, enum TPC_ErrorCode *error);



	/// Finish a "compound get data" operation and execute all deferred TPC_DeferredGetXxxData calls.
	/**
	* Remarks: The arrays that were passed to TPC_DeferredGetXxxData are filled with data from the device.
	*
	* The function works synchronously, that means it does not return before all
	* the data is read from the devices. The error result for each of the deferred 
	* calls is passed to the \a error variables that were given with each call. 
	* Even though one or more of the deferred TPC_DeferredGetXxxData calls could 
	* fail, the other ones can have valid data. Passing different error variables 
	* for each deferred call allows to find out which ones were successful.
	*
	* \param measurementNumber The number of the measurement. This number is found in the device status.
	* \returns 
	*  - \ref tpc_noError if all deferred TPC_DeferredGetXxxData calls can be carried out successfully.
	*  - \ref tpc_errNewMeasurement if the data is no longer available because a new measurement 
	*          has been started.
	*  - another error code indicating a problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ProcessDeferredDataRequests(int measurementNumber);


	/// Cancel a compound get data operation that was started with TPC_DeferredGetXxxData calls.
	/**
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_CancelDeferredDataRequests();

	/*@}*/



	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_SendServerCustomCommand(int deviceIx, char *command, char *result, int length);


	//=== Setting files ====================================================================

	/**
	 * \addtogroup SettingFiles
	 *
	 * Configuration files store all parameters and attributes
	 * of all devices. They do NOT store the device list.
	 *
	 * @{
	 */


	/// Stores the configuration into a file (all parameters and attributes of all devices).
	/**
	* Remarks: Does NOT store the device list into the file. 
	* \param filename A valid filename.
	* \returns \ref tpc_noError if successful, an error code otherwise.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_WriteSettingFile(const char *filename);


	/// Read a file written by \ref TPC_WriteSettingFile and restore the configuration.
	/**
	* Remarks: The device list is NOT stored in a setting file. Set up the list
	* of devices (see \ref TPC_BeginSystemDefinition) before calling this function.
	* \param filename A valid filename.
	* \returns \ref tpc_noError if successful, an error code otherwise.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_LoadSettingFile(const char *filename);

	/*@}*/



	//=== Termination =================================================================

	/**
	 * \addtogroup Misc
	 * @{
	 */

	/// Terminate all connections and prepare for program exit.
	TPC_EXP void TPC_CC TPC_TerminateTpcAccess();

	/*@}*/

	//=== GPS Log =================================================================

	/**
	* \addtogroup Misc
	* @{
	*/

	/// Get GPS logs
	/**
	* \param from	first log entry, index starts at 0
	* \param to		last log entry
	* \param char*	buffer with size 128*(to-from+1)
	* \returns \ref tpc_noError if successful, an error code otherwise.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetGPSLogList(int deviceIx, int from, int to, char *log);

	/*@}*/

	//---------------------------------------------------------------------------------
#ifdef __cplusplus
} // extern "C"
#endif
//---------------------------------------------------------------------------------
#endif // TPCAccess_h


