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
  $Id: AssocRow.h 2 2009-01-13 08:45:52Z roman $
  AssocRow: Stores a local copy of association rows
--------------------------------------------------------------------------------*/
#ifndef AssocRow_h
#define AssocRow_h AssocRow_h
//---------------------------------------------------------------------------------

#include <vector>
#include <map>
using namespace std;

#include "TpcAccess.h"

//---------------------------------------------------------------------------------

class AssociationRow
{
public:
	AssociationRow(int boardAddress, int inputNumber);

	int Count() const { return m_entries.size(); }

	int GetBoardAddress() const { return m_boardAddress; }
	int GetInputNumber() const { return m_inputNumber; }

	void GetAssociatedChannels(TPC_AssociatedChannel* list, int count) const;
	void SetAssociatedChannels(TPC_AssociatedChannel* list, int count);

	bool operator==(const AssociationRow& rhs) const;
	bool operator!=(const AssociationRow& rhs) const { return !(*this == rhs); }

	const TPC_AssociatedChannel& operator[](int index) const { return m_entries[index]; }

private:
	int m_boardAddress;
	int m_inputNumber;
	vector<TPC_AssociatedChannel> m_entries;
};



class AssociationRowList
{
public:
	int Count() const { return m_entries.size(); }

	void Clear() { m_entries.clear(); }

	TPC_ErrorCode GetAssociatedChannels(int boardAddress, int inputNumber, 
										TPC_AssociatedChannel* list, int* count) const;

	TPC_ErrorCode SetAssociatedChannels(int boardAddress, int inputNumber, 
										TPC_AssociatedChannel* list, int count);

	bool operator==(const AssociationRowList& rhs) const;
	bool operator!=(const AssociationRowList& rhs) const { return !(*this == rhs); }

	const AssociationRow& operator[](int index) const { return m_entries[index]; }

private:
	vector<AssociationRow> m_entries;

	int FindEntry(int boardAddress, int inputNumber) const;
};






//---------------------------------------------------------------------------------
#endif // AssocRow_h

