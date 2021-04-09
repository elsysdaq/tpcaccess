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
  $Id: DeviceInfo.h 2 2009-01-13 08:45:52Z roman $
  Keeps a copy of the device info (device, board, input).
--------------------------------------------------------------------------------*/
#ifndef DeviceInfo_h
#define DeviceInfo_h DeviceInfo_h
//---------------------------------------------------------------------------------

#include "TpcAccess.h"

//---------------------------------------------------------------------------------

struct BoardRestrictions
{
	int maxMemory;
	int maxSpeed;
	int maxAdcSpeed; 
	int boardClock; 
	int version;
};

BoardRestrictions Combine(const BoardRestrictions& a, const BoardRestrictions& b);

bool CanCombine(const BoardRestrictions& a, const BoardRestrictions& b);





class DeviceInfo
{
public:
	DeviceInfo();
	~DeviceInfo();

public:
	bool BoardExists(int boardAddress);
	bool InputExists(int boardAddress, int inputNumber);
	int NumberOfInputs(int boardAddress);

	const TPC_DeviceInfo& GetDeviceInfo();
	const TPC_BoardInfo& GetBoardInfo(int boardAddress);
	const BoardRestrictions& GetBoardRestrictions(int boardAddress);
	const TPC_InputInfo& GetInputInfo(int boardAddress, int inputNumber);

public:
	void Clear();
	void SetDeviceInfo(const TPC_DeviceInfo& deviceInfo);
	void SetBoardInfo(int boardAddress, const TPC_BoardInfo& boardInfo);
	void SetBoardRestrictions(int boardAddress, const BoardRestrictions& boardRestrictions);
	void SetInputInfo(int boardAddress, int inputNumber, const TPC_InputInfo& inputInfo);

private:
	TPC_DeviceInfo m_deviceInfo;
	TPC_BoardInfo m_boardInfo[tpc_maxBoards];
	BoardRestrictions m_boardRestrictions[tpc_maxBoards];
	TPC_InputInfo m_inputInfo[tpc_maxBoards][tpc_maxInputs];
};


//---------------------------------------------------------------------------------
#endif // DeviceInfo_h

