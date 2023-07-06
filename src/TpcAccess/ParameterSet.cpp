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
  $Id: ParameterSet.cpp 2 2009-01-13 08:45:52Z roman $
  ParameterSet: Stores a local copy of all parameters
  ParameterModificationList: Keeps a growing list of parameters to be set
--------------------------------------------------------------------------------*/

#include "ParameterSet.h"

//-------------------------------------------------------------------------------



bool operator<(const ParameterSetKey& lhs, const ParameterSetKey& rhs)
{
	if (lhs.boardAddress() < rhs.boardAddress()) return true;
	if (lhs.boardAddress() > rhs.boardAddress()) return false;
	if (lhs.inputNumber() < rhs.inputNumber()) return true;
	if (lhs.inputNumber() > rhs.inputNumber()) return false;
	if (lhs.parameterIndex() < rhs.parameterIndex()) return true;
	return false;
}


bool operator==(const ParameterSetKey& lhs, const ParameterSetKey& rhs)
{
	if (lhs.boardAddress() != rhs.boardAddress()) return false;
	if (lhs.inputNumber() != rhs.inputNumber()) return false;
	if (lhs.parameterIndex() != rhs.parameterIndex()) return false;
	return true;
}


//----------------------------


ParameterSet::ParameterSet()
{
}


ParameterSet::ParameterSet(const ParameterSet& p)
{
	m_dictionary = p.m_dictionary;
}


ParameterSet& ParameterSet::operator=(const ParameterSet& rhs)
{
	m_dictionary = rhs.m_dictionary;
	return *this; 
}


bool ParameterSet::operator==(const ParameterSet& rhs) const
{
	return (m_dictionary == rhs.m_dictionary);
}


void ParameterSet::SetParameter(int boardAddress, int inputNumber, int parameterIndex, double value)
{
	ParameterSetKey key = ParameterSetKey(boardAddress, inputNumber, parameterIndex);
	if (m_dictionary.find(key) != m_dictionary.end()) {
		m_dictionary.erase(key);
	}
	m_dictionary[key] = value;
}


bool ParameterSet::GetParameter(int boardAddress, int inputNumber, int parameterIndex, double* value)
{
	ParameterSetKey key = ParameterSetKey(boardAddress, inputNumber, parameterIndex);

	if (m_dictionary.find(key) != m_dictionary.end()) {
		*value = (*const_cast<map<ParameterSetKey, double>* >(&m_dictionary))[key];
		return true;
	}
	else {
		// not found
		return false;
	}
}


void ParameterSet::Clear()
{
	m_dictionary.clear();
}



//-------------------------------------------------------------------------------


ParameterModificationList::ParameterModificationList()
{
}


void ParameterModificationList::Add(int boardAddress, int inputNumber, int parameterIndex, double value)
{
	Entry e;
	e.boardAddress = boardAddress;
	e.inputNumber = inputNumber;
	e.parameterIndex = parameterIndex;
	e.value = value;
	e.roundedValue = value;
	e.error = 0;
	m_entries.push_back(e);
}


void ParameterModificationList::SetResult(int index, double roundedValue, int error)
{
	m_entries[index].roundedValue = roundedValue;
	m_entries[index].error = error;
}


//-------------------------------------------------------------------------------


