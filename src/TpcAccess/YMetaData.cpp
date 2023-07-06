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
 * (C) Copyright 2005 - 2023 Elsys AG. All rights reserved.
*/
//---------------------------------------------------------------------------
/*--------------------------------------------------------------------------------
  $Id: YMetaData.cpp 2 2009-01-13 08:45:52Z roman $
  Keeps y meta data for each input
--------------------------------------------------------------------------------*/

#include "YMetaData.h"

#include <memory.h>

//-------------------------------------------------------------------------------


YMetaDataList::YMetaDataList()
{
	Clear();
}

	
void YMetaDataList::SetMetaData(int boardAddress, int inputNumber, const RawYMetaData& metadata)
{
	if (boardAddress < 0 || boardAddress >= tpc_maxBoards) return;
	if (inputNumber < 0 || inputNumber >= tpc_maxInputs) return;
	m_data[boardAddress][inputNumber] = metadata;
}


bool YMetaDataList::GetMetaData(int boardAddress, int inputNumber, RawYMetaData* metadata)
{
	if (boardAddress < 0 || boardAddress >= tpc_maxBoards) return false;
	if (inputNumber < 0 || inputNumber >= tpc_maxInputs) return false;
	*metadata = m_data[boardAddress][inputNumber];
	return true;
}


void YMetaDataList::Clear()
{
	memset(m_data, 0, sizeof(m_data));
}



//-------------------------------------------------------------------------------


