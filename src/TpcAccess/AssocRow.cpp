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
  $Id: AssocRow.cpp 2 2009-01-13 08:45:52Z roman $
  AssocRow: Stores a local copy of association rows
--------------------------------------------------------------------------------*/

#include "AssocRow.h"

#include <algorithm>
using namespace std;

//-------------------------------------------------------------------------------

bool operator<(const TPC_AssociatedChannel& lhs, const TPC_AssociatedChannel& rhs) 
{
	if (lhs.boardAddress < rhs.boardAddress) return true;
	if (lhs.boardAddress > rhs.boardAddress) return false;
	if (lhs.inputNumber < rhs.inputNumber) return true;
	return false;
}


bool operator==(const TPC_AssociatedChannel& lhs, const TPC_AssociatedChannel& rhs) 
{
	if (lhs.boardAddress != rhs.boardAddress) return false;
	if (lhs.inputNumber != rhs.inputNumber) return false;
	return true;
}


bool operator!=(const TPC_AssociatedChannel& lhs, const TPC_AssociatedChannel& rhs)
{ 
	return !(lhs == rhs); 
}


AssociationRow::AssociationRow(int boardAddress, int inputNumber)
{
	m_boardAddress = boardAddress;
	m_inputNumber = inputNumber;
}


void AssociationRow::GetAssociatedChannels(TPC_AssociatedChannel* list, int count) const
{
	for (int i=0; i<count; i++) {
		list[i] = m_entries[i];
	}
}


void AssociationRow::SetAssociatedChannels(TPC_AssociatedChannel* list, int count)
{
	m_entries.clear(); 
	for (int i=0; i<count; i++) {
		m_entries.push_back(list[i]);
	}

	// Sort
	sort(m_entries.begin(), m_entries.end());
}


bool AssociationRow::operator==(const AssociationRow& rhs) const
{
	if (Count() != rhs.Count()) return false;
	for (int i=0; i<Count(); i++) {
		if (m_entries[i] != rhs.m_entries[i]) return false;
	}
	return true;
}


//-------------------------------------------------------------------------------

//##### keep empty rows as indication to clear the row

TPC_ErrorCode AssociationRowList::GetAssociatedChannels(int boardAddress, int inputNumber, 
									TPC_AssociatedChannel* list, int* count) const
{
	int ix = FindEntry(boardAddress, inputNumber);
	if (ix < 0) {
		*count = 0;
		return tpc_noError;
	}
	else {
		TPC_ErrorCode err = tpc_noError;
		if (*count < m_entries[ix].Count()) 
			err = tpc_errBufferTooShort;
		else 
			*count = m_entries[ix].Count();
		m_entries[ix].GetAssociatedChannels(list, *count);

		return err;
	}
}


TPC_ErrorCode AssociationRowList::SetAssociatedChannels(int boardAddress, int inputNumber, 
									TPC_AssociatedChannel* list, int count)
{
	int ix = FindEntry(boardAddress, inputNumber);
	if (ix < 0) {
		// Add a new entry
		AssociationRow row(boardAddress, inputNumber);
		row.SetAssociatedChannels(list, count);
		m_entries.push_back(row);
	}
	else {
		// Use the existing entry
		m_entries[ix].SetAssociatedChannels(list, count);
	}
	return tpc_noError;
}


bool AssociationRowList::operator==(const AssociationRowList& rhs) const
{
	if (Count() != rhs.Count()) return false;

	for (int i=0; i<Count(); i++) {
		int ix = FindEntry(m_entries[i].GetBoardAddress(), m_entries[i].GetInputNumber());
		if (ix < 0) return false;
		if (m_entries[i] != rhs.m_entries[ix]) return false;
	}
	return true;
}


int AssociationRowList::FindEntry(int boardAddress, int inputNumber) const
{
	for (unsigned int i = 0; i<m_entries.size(); i++) {
		if (m_entries[i].GetBoardAddress() == boardAddress
			&&m_entries[i].GetInputNumber() == inputNumber) {
				return i;
		}
	}
	return -1;
}


//-------------------------------------------------------------------------------
