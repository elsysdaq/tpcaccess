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
  $Id: ParameterSet.h 2 2009-01-13 08:45:52Z roman $
  ParameterSet: Stores a local copy of all parameters
  ParameterModificationList: Keeps a growing list of parameters to be set
--------------------------------------------------------------------------------*/
#ifndef ParameterSet_h
#define ParameterSet_h ParameterSet_h
//---------------------------------------------------------------------------------

#include <vector>
#include <map>
using namespace std;

//---------------------------------------------------------------------------------


class ParameterSetKey
{
public:
	ParameterSetKey() 
	{}

	ParameterSetKey(int boardAddress, int inputNumber, int parameterIndex)
	{
		m_boardAddress = boardAddress;
		m_inputNumber = inputNumber;
		m_parameterIndex = parameterIndex;
	}
	
	int boardAddress() const  { return m_boardAddress; }
	int inputNumber() const { return m_inputNumber; }
	int parameterIndex() const { return m_parameterIndex; }

private:
	int m_boardAddress;
	int m_inputNumber;
	int m_parameterIndex;
};


bool operator<(const ParameterSetKey& lhs, const ParameterSetKey& rhs);
bool operator==(const ParameterSetKey& lhs, const ParameterSetKey& rhs);


class ParameterSet
{
public:
	ParameterSet();
	ParameterSet(const ParameterSet&);
	ParameterSet& operator=(const ParameterSet&);

	bool operator==(const ParameterSet& rhs) const;
	bool operator!=(const ParameterSet& rhs) const { return !(*this == rhs); }

	void SetParameter(int boardAddress, int inputNumber, int parameterIndex, double value);
	bool GetParameter(int boardAddress, int inputNumber, int parameterIndex, double* value);

	void Clear();

private:
	map<ParameterSetKey, double> m_dictionary;
};


//---------------------------------------------------------------------------------

class ParameterModificationList
{
public:
	ParameterModificationList();

	void Clear() { m_entries.clear(); }

	void Add(int boardAddress, int inputNumber, int parameterIndex, double value);
	int Count() { return m_entries.size(); }

	struct Entry
	{
		// For input
		int boardAddress;
		int inputNumber;
		int parameterIndex;
		double value;
		// After setting
		double roundedValue;
		int error;
	};

	const Entry& operator[](int index) { return m_entries[index]; }
	void SetResult(int index, double roundedValue, int error);

private:
	vector<Entry> m_entries;
};


//---------------------------------------------------------------------------------
#endif // ParameterSet_h

