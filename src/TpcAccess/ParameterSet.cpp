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
  $Id: ParameterSet.cpp 46 2025-03-21 10:42:51Z philipp $
  ParameterSet: Stores a local copy of all parameters
  ParameterModificationList: Keeps a growing list of parameters to be set
--------------------------------------------------------------------------------*/

#include "ParameterSet.h"

//-------------------------------------------------------------------------------

// Lexical ordering
bool operator<(const ParameterSetKey& lhs, const ParameterSetKey& rhs) {
    if (lhs.boardAddress() != rhs.boardAddress()) return lhs.boardAddress() < rhs.boardAddress();
    if (lhs.inputNumber() != rhs.inputNumber()) return lhs.inputNumber() < rhs.inputNumber();
    return lhs.parameterIndex() < rhs.parameterIndex();
}

bool operator==(const ParameterSetKey& lhs, const ParameterSetKey& rhs) {
    return lhs.boardAddress() == rhs.boardAddress() && lhs.inputNumber() == rhs.inputNumber() &&
           lhs.parameterIndex() == rhs.parameterIndex();
}

//----------------------------

bool ParameterSet::operator==(const ParameterSet& rhs) const {
    return (m_parameters_dict == rhs.m_parameters_dict);
}

void ParameterSet::SetParameter(int boardAddress, int inputNumber, int parameterIndex, double value) {
    ParameterSetKey key(boardAddress, inputNumber, parameterIndex);
    m_parameters_dict[key] = value;
}

void ParameterSet::SetParameterAvailableValues(int boardAddress, int inputNumber, int parameterIndex,
                                               std::vector<double> values, TPC_ParameterAvailableValuesType type) {
    ParameterSetKey key(boardAddress, inputNumber, parameterIndex);
    m_parameters_available_vals_dict[key] = {values, type};
}

// TODO change to std::optional
bool ParameterSet::GetParameter(int boardAddress, int inputNumber, int parameterIndex, double* value) {
    ParameterSetKey key = ParameterSetKey(boardAddress, inputNumber, parameterIndex);

    if (auto it = m_parameters_dict.find(key); it != m_parameters_dict.end()) {
        *value = it->second;
        return true;
    }
    else {
        // not found
        return false;
    }
}

std::optional<ParameterAvailableValues> ParameterSet::GetParameterAvailableValues(int boardAddress, int inputNumber,
                                                                                  int parameterIndex) {
    ParameterSetKey key = ParameterSetKey(boardAddress, inputNumber, parameterIndex);

    if (auto it = m_parameters_available_vals_dict.find(key); it != m_parameters_available_vals_dict.end()) {
        return it->second;
    }
    else {
        return {};
    }
}

void ParameterSet::ClearParameters() {
    m_parameters_dict.clear();
}

void ParameterSet::ClearAvailableValues() {
    m_parameters_available_vals_dict.clear();
}

//-------------------------------------------------------------------------------

void ParameterModificationList::Add(int boardAddress, int inputNumber, int parameterIndex, double value) {
    Entry e;
    e.boardAddress   = boardAddress;
    e.inputNumber    = inputNumber;
    e.parameterIndex = parameterIndex;
    e.value          = value;
    e.roundedValue   = value;
    e.error          = 0;
    m_entries.push_back(e);
}

void ParameterModificationList::SetResult(int index, double roundedValue, int error) {
    m_entries[index].roundedValue = roundedValue;
    m_entries[index].error        = error;
}

//-------------------------------------------------------------------------------
