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
  $Id: DeviceInfo.cpp 2 2009-01-13 08:45:52Z roman $
  Keeps a copy of the device info (device, board, input).
--------------------------------------------------------------------------------*/

#include "DeviceInfo.h"

#include <memory.h>
#include <assert.h>

//-------------------------------------------------------------------------------


unsigned gcd(unsigned a, unsigned b)
{
	// Euklid's algorithm
	while (b != 0) {
		unsigned t = a % b;
		a = b;
		b = t;
	}
	return a;
}


BoardRestrictions Combine(const BoardRestrictions& a, const BoardRestrictions& b)
{
	BoardRestrictions r;
	r.maxMemory = a.maxMemory < b.maxMemory ? a.maxMemory : b.maxMemory;
	r.maxSpeed = a.maxSpeed < b.maxSpeed ? a.maxSpeed : b.maxSpeed;
	r.maxAdcSpeed = gcd(a.maxAdcSpeed, b.maxAdcSpeed);
	return r;
}



bool CanCombine(const BoardRestrictions& a, const BoardRestrictions& b)
{
	if (a.boardClock != b.boardClock) return false;
	//### version auch noch pr�fen
	return true;
}


//-------------------------------------------------------------------------------


static TPC_BoardInfo nullBoardInfo;
static BoardRestrictions nullBoardRestrictions;
static TPC_InputInfo nullInputInfo;


DeviceInfo::DeviceInfo()
{
	memset(&nullBoardInfo, 0, sizeof(nullBoardInfo));
	memset(&nullBoardRestrictions, 0, sizeof(nullBoardRestrictions));
	memset(&nullInputInfo, 0, sizeof(nullInputInfo));

	Clear();
}



DeviceInfo::~DeviceInfo()
{
}



bool DeviceInfo::BoardExists(int boardAddress)
{
	if (boardAddress < 0 || boardAddress >= tpc_maxBoards) return false;
	return (m_boardInfo[boardAddress].numberOfInputs > 0);
}



bool DeviceInfo::InputExists(int boardAddress, int inputNumber)
{
	if (!BoardExists(boardAddress)) return false;
	if (inputNumber < 0 || inputNumber >= m_boardInfo[boardAddress].numberOfInputs) return false;
	return true;
}



int DeviceInfo::NumberOfInputs(int boardAddress)
{
	if (!BoardExists(boardAddress)) return 0;
	return m_boardInfo[boardAddress].numberOfInputs;
}



const TPC_DeviceInfo& DeviceInfo::GetDeviceInfo()
{
	return m_deviceInfo;
}



const TPC_BoardInfo& DeviceInfo::GetBoardInfo(int boardAddress)
{
	if (!BoardExists(boardAddress)) return nullBoardInfo;
	return m_boardInfo[boardAddress];
}



const BoardRestrictions& DeviceInfo::GetBoardRestrictions(int boardAddress)
{
	if (!BoardExists(boardAddress)) return nullBoardRestrictions;
	return m_boardRestrictions[boardAddress];
}



const TPC_InputInfo& DeviceInfo::GetInputInfo(int boardAddress, int inputNumber)
{
	if (!InputExists(boardAddress, inputNumber)) return nullInputInfo;
	return m_inputInfo[boardAddress][inputNumber];
}



void DeviceInfo::Clear()
{
	memset(&m_deviceInfo, 0, sizeof(m_deviceInfo));
	memset(&m_boardInfo, 0, sizeof(m_boardInfo));
	memset(&m_boardRestrictions, 0, sizeof(m_boardRestrictions));
	memset(&m_inputInfo, 0, sizeof(m_inputInfo));
}



void DeviceInfo::SetDeviceInfo(const TPC_DeviceInfo& deviceInfo)
{
	m_deviceInfo = deviceInfo;
}



void DeviceInfo::SetBoardInfo(int boardAddress, const TPC_BoardInfo& boardInfo)
{
	assert(boardAddress >= 0 && boardAddress < tpc_maxBoards);
	m_boardInfo[boardAddress] = boardInfo;
}



void DeviceInfo::SetBoardRestrictions(int boardAddress, const BoardRestrictions& boardRestrictions)
{
	assert(boardAddress >= 0 && boardAddress < tpc_maxBoards);
	m_boardRestrictions[boardAddress] = boardRestrictions;
}



void DeviceInfo::SetInputInfo(int boardAddress, int inputNumber, const TPC_InputInfo& inputInfo)
{
	assert(boardAddress >= 0 && boardAddress < tpc_maxBoards);
	assert(inputNumber >= 0 && inputNumber < tpc_maxInputs);
	m_inputInfo[boardAddress][inputNumber] = inputInfo;
}


//-------------------------------------------------------------------------------


