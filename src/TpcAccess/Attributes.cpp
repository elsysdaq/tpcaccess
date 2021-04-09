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
  $Id: Attributes.cpp 2 2009-01-13 08:45:52Z roman $
  Attribute container.
--------------------------------------------------------------------------------*/

#include "Attributes.h"

#include <map>
using namespace std;

//-------------------------------------------------------------------------------


bool operator<(const AttributeKey& lhs, const AttributeKey& rhs)
{
	if (lhs.boardAddress() < rhs.boardAddress()) return true;
	if (lhs.boardAddress() > rhs.boardAddress()) return false;
	if (lhs.inputNumber() < rhs.inputNumber()) return true;
	if (lhs.inputNumber() > rhs.inputNumber()) return false;
	if (lhs.name() < rhs.name()) return true;
	return false;
}


bool operator==(const AttributeKey& lhs, const AttributeKey& rhs)
{
	if (lhs.boardAddress() != rhs.boardAddress()) return false;
	if (lhs.inputNumber() != rhs.inputNumber()) return false;
	if (lhs.name() != rhs.name()) return false;
	return true;
}


//----------------------------


Attributes::Attributes() 
{
}


Attributes::Attributes(const Attributes& a)
{
	m_dictionary = a.m_dictionary;
}


Attributes& Attributes::operator=(const Attributes& rhs)
{
	m_dictionary = rhs.m_dictionary;

	return *this; 
}


void Attributes::Clear()
{
	m_dictionary.clear();
}


void Attributes::SetAttribute(int boardAddress, int inputNumber, const string& name, const string& value)
{
	AttributeKey key = AttributeKey(boardAddress, inputNumber, name);
	if (m_dictionary.find(key) != m_dictionary.end()) {
		m_dictionary.erase(key);
	}

	if (value != "") {
		m_dictionary[key] = value;
	}
}


void Attributes::SetAttributeE(int boardAddress, int inputNumber, const string& name, const string& value)
{
	AttributeKey key = AttributeKey(boardAddress, inputNumber, name);
	if (m_dictionary.find(key) != m_dictionary.end()) {
		m_dictionary.erase(key);
	}

	m_dictionary[key] = value;
}



string Attributes::GetAttribute(int boardAddress, int inputNumber, const string& name) const
{
	AttributeKey key = AttributeKey(boardAddress, inputNumber, name);

	if (m_dictionary.find(key) != m_dictionary.end()) {
		return (*const_cast<map<AttributeKey, string>* >(&m_dictionary))[key];
	}
	else {
		// not found
		return "";
	}
}


void Attributes::EnumerateAttributes(AttributeEnumerator* callback)
{
	callback->SetTotal(m_dictionary.size());
	map<AttributeKey, string>::const_iterator i = m_dictionary.begin();
	int n = 0;
	while (i != m_dictionary.end()) {
		bool goOn = callback->Callback(n, i->first.boardAddress(), i->first.inputNumber(), i->first.name(), i->second);
		if (!goOn) break;
		n++;
		++i;
	}
}


bool Attributes::operator==(const Attributes& rhs) const
{
	return (m_dictionary == rhs.m_dictionary);
}


//-------------------------------------------------------------------------------


