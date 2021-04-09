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
  $Id: TpcAccessElsys.h 54 2010-03-12 13:58:54Z roman $
  TransPC TPCX API - Definitions for Elsys usage only.
--------------------------------------------------------------------------------*/


/*
#####################################################
#####################################################
#####################################################
########                                     ########
########      This is a DRAFT version.       ########
########  Subject to change without notice!  ########
########                                     ########
#####################################################
#####################################################
#####################################################
*/



#ifndef TpcAccessElsys_h
#define TpcAccessElsys_h TpcAccessElsys_h
//---------------------------------------------------------------------------------

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
	
	\ref AutoSequences\n
	Function to load, start and stop autosequences. 
	
	
	
	*/

	/**
	 * \defgroup AutoSequences Auto Sequences
	 * \defgroup ElsysInternal Elsys Internal Functions
	 */


	//======================================================================================

	/**
	 * \addtogroup ElsysInternal
	 *
	 * Function used for internal purpose only
	 *
	  * @{
	 */
	/// Constants for use with TPC_ExecuteSystemCommand.
	enum TPC_Elsys_SystemCommand {
		/// Used internally by TpcAccess.dll (Primary device mutex)
		tpc_cmdStartLock = -1,		
	
		/// Used internally by TpcAccess.dll (Primary device mutex)
		tpc_cmdStartUnlock = -2,	
	};

	/// Type of Calibration
	enum TPC_CalibrationType {
		/// Autocalibration (without external instruments)
		tpc_calAuto,
		/// Falst Calibration
		tpc_calFast,
		/// Full Calibration
		tpc_calFull,
	};

	/// Start the calibration command
	/**
	* \param deviceIx Device Index
	* \param command
	* \returns \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP TPC_ErrorCode TPC_CC TPC_Elsys_StartCalibration(int deviceIx, TPC_CalibrationType command);

	/// Write to device registers
	/** The parameter 'type' takes values from enum eDevIndex. 
	*  Conversion hwconst union -> parameters
	* 
	* <table>
	*	<tr><th>Type</th>			<th>count</th>		<th>data</th> <th>aux1 </th>	<th> aux2 </th> </tr>
	*	<tr><td>sMemoryData</td>	<td>Count</td>		<td>Data</td> <td>Address</td>	<td>Bank</td>	</tr>
	*  <tr><td>sTriggerData</td>	<td>Count</td>		<td>Data</td> <td>0</td>		<td>0</td>		</tr>
	*	<tr><td>sEepromData</td>	<td>Count</td>		<td>Data</td> <td>0</td>		<td>0</td>		</tr>
	*	<tr><td>sCalVoltageData</td><td>NumVals*4</td>  <td>Data</td> <td>0</td>		<td>0</td>		</tr>
	*	<tr><td>sRegisterData</td>	<td>4</td>			<td>&Data</td><td>Dest</td>		<td>Address</td></tr> 
	* </table>
	*
	* \param deviceIx Device Index
	* \param boardAddress
	* \param type
	* \param count
	* \param data
	* \param aux1
	* \param aux2
	* \return \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_WriteDev(int deviceIx, int boardAddress, int type, unsigned count, void *data, unsigned aux1, unsigned aux2);


	/// Read from a device register
	/** The parameter 'type' takes values from enum eDevIndex. 
	*  Conversion hwconst union -> parameters
	* 
	* <table>
	*	<tr><th>Type</th>			<th>count</th>		<th>data</th> <th>aux1 </th>	<th> aux2 </th> </tr>
	*	<tr><td>sMemoryData</td>	<td>Count</td>		<td>Data</td> <td>Address</td>	<td>Bank</td>	</tr>
	*  <tr><td>sTriggerData</td>	<td>Count</td>		<td>Data</td> <td>0</td>		<td>0</td>		</tr>
	*	<tr><td>sEepromData</td>	<td>Count</td>		<td>Data</td> <td>0</td>		<td>0</td>		</tr>
	*	<tr><td>sCalVoltageData</td><td>NumVals*4</td>  <td>Data</td> <td>0</td>		<td>0</td>		</tr>
	*	<tr><td>sRegisterData</td>	<td>4</td>			<td>&Data</td><td>Dest</td>		<td>Address</td></tr> 
	* </table>
	*
	* \param deviceIx Device Index
	* \param boardAddress
	* \param type
	* \param count
	* \param data
	* \param aux1
	* \param aux2
	* \return \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ReadDev(int deviceIx, int boardAddress, int type, unsigned count, void *data, unsigned aux1, unsigned aux2);

	/// Read or Write to the Two Wire (I2C) interface
	/**
	* \param deviceIx Device Index
	* \param boardAddress
	* \param amplifier
	* \param count
	* \param data
	* \return \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ReadWriteTwi(int deviceIx, int boardAddress, int amplifier, int count, char *data);

	/// Elsys Parameter for internal calibration
	/**
	* Stellt die Kalibriersannungen ein, die Einstellungen werden sofort uebernommen.
	* Soll die Referenzspannung aufgezeichnet werden, muss cplCAL fuer den entsprechenden 
	* Kanal eingeschaltet werden. Diese Einstellung gilt fuer den gesammten Verstaerker, 
	* die Kanalnummer selektiert den Verstaerker.
	* - Bit 0-1 Kalibrierspannung [0.4V, 0.8V, 1.83V, 4.1V]
	* - Bit 2-3 Quelle fuer Kanal 1+3 [GND, +Kalibrierspannung,  -Kalibrierspannung, Referenz]
	* - Bit 4-5 Quelle fuer Kanal 2+4 [GND, +Kalibrierspannung,  -Kalibrierspannung, +4.5V]
	*/
	enum TPC_Elsys_Parameter {
		/// Calibration voltage
		tpc_parKalibVoltageDirect = 3008
	};


	/// Elsys Parameter for internal calibration
	enum TPC_Elsys_InputCouplings {
		/// cplCAL
		cplCAL = 2,
	};


	/// Operation Mode
	enum TPC_Elsys_OperationModes {
		/// tpc_omScope
		tpc_omScope = 0,
	};


	/*
	// Multy-System Extension:
	//    Each system gets a systemId. The default system has systemId = 0. 
	//    The deviceIx is regarded as  systemId * 4096 + deviceIx for all those methods that have deviceIx parameters
	*/


	///  TPC_BeginSetSystem for multi systems 
	/**
	*\param id System ID
	* \return \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_BeginSetSystem(int id);

	///  TPC_EndSetSystem for multi systems 
	/**
	*\param id System ID
	* \return \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_EndSetSystem(int id);

	///  TPC_EndSetSystem for multi systems 
	/**
	*\param id System ID
	* \return \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_CancelSetSystem(int id);

	/// TPC_ProcessDeferredDataRequestsSystem
	/**
	*\param id System ID
	*\param measurementNumber
	* \return \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_ProcessDeferredDataRequestsSystem(int id, int measurementNumber);

	/// TPC_CancelDeferredDataRequestsSystem
	/**
	*\param id System ID
	* \return \ref tpc_noError if successful, otherwise an error code indicating the problem.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_CancelDeferredDataRequestsSystem(int id);

	/*
	##### Implement GetMulti-T-Metadata!
	##### Implement Clusters!
	
	*/

	/*@}*/
	//=== Auto Sequenz Functinos ======================================================

	/**
	 * \addtogroup AutoSequences
	 *
	 *  The AutoSequence module is a script interpreter which allows to automate 
	 *	a sequence of commands like start recording, trigger etc. 
	 *
	 *  The autosequence script must have the same syntax like auto sequences  
	 *  generated with TransAS 3.x
	 *
	 *  The following auto sequence commands are supported:
	 *  - Start :	Start the recording
	 *  - Stop  :   Stop the recording
	 *  - Trigger:  Software trigger
	 *  - EOR: Wait on End Of Record
	 *  - Repeat/ Next: loop generator
	 *  - Delay: wait command
	 *  - Autocalibration
	 *  - Save: saves to a TPC5 file
	 *
	 * @{
	 */
	/// Load an auto sequence into the device and store it in a file
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param cData Pointer to the autsequence string
	* \param iLength Length of the autsequence string
	* \returns \ref tpc_noError if successful, an error code otherwise.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_LoadAutosequence(	int deviceIx, char *cData, int iLength);

	/// Read out the stored auto sequence from the device
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \param cData Pointer to the buffer for storing the autosequence
	* \param iLength Length of the autsequence string
	* \returns \ref tpc_noError if successful, an error code otherwise.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetAutoSequence(	int deviceIx, char *cData, int iLength);

	/// Start the auto sequence 
	/**
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \returns \ref tpc_noError if successful, an error code otherwise.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_StartAutoSequence(int deviceIx);

	/// Stop the auto sequence 
	/**
	* Remark: The auto sequence cannot been interrupted during file save
	* \param deviceIx The device index as returned by \ref TPC_AddDevice.
	* \returns \ref tpc_noError if successful, an error code otherwise.
	*/
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_StopAutoSequence(	int deviceIx);

	/*@}*/

	//======================================================================================
#ifdef __cplusplus
} // extern "C"
#endif
//---------------------------------------------------------------------------------
#endif // TPCAccess_h

