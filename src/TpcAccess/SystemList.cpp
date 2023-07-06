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
  $Id: SystemList.cpp 2 2009-01-13 08:45:52Z roman $
  This class holds all information and state for the system.
--------------------------------------------------------------------------------*/

#include "System.h"
#include "SystemList.h"

#include <time.h>

//-------------------------------------------------------------------------------

SystemList* SystemList::s_singleton = NULL;


SystemList* SystemList::TheSystemList()
{
	if (s_singleton == NULL) {
		s_singleton = new SystemList();
	}
	return s_singleton;
}

SystemList::SystemList()
{
    m_systems.push_back(System::TheSystem());
}


SystemList::~SystemList()
{
    // Clear all entries except slot zero.
    for (int i = 1; i<(int)m_systems.size(); i++) {
	    System* s = m_systems[i];
        if (s != NULL) {
            m_systems[i] = NULL;
            delete s;
        }
    }
}



void SystemList::PrepareToQuitProgram()
{
	if (s_singleton != NULL) {
        delete s_singleton;
		s_singleton = NULL;
	}

    // Delete the system singleton.
    System::PrepareToQuitProgram();
}



int SystemList::NewSystem()
{
    // Try reusing an empty slot.
    for (int i = 1; i<(int)m_systems.size(); i++) {
        if (m_systems[i] == NULL) {
		    System* s = new System();
            m_systems[i] = s;
            return i*SYSTEM_MULTIPLIER;
        }
	}
    
    // Append a new slot.
    int ix = m_systems.size();
    System* s = new System();
  	m_systems.push_back(s);
	return ix*SYSTEM_MULTIPLIER;;
}


TPC_ErrorCode SystemList::DeleteSystem(int deviceIx)
{
    // Calculate slot
    int sysIx = deviceIx / SYSTEM_MULTIPLIER;

    // Check index range
    if (sysIx < 0) return tpc_errInvalidDeviceIx;
    if (sysIx >= (int)m_systems.size()) return tpc_errInvalidDeviceIx;

    // Do not allow to delete the default system.
    if (sysIx == 0) return tpc_errInvalidDeviceIx;

    // Clear the slot.
	System* s = m_systems[sysIx];
    m_systems[sysIx] = NULL;
    delete s;
    return tpc_noError;
}


System* SystemList::FindSystem(int deviceIx)
{
    // Calculate slot
    int sysIx = deviceIx / SYSTEM_MULTIPLIER;

    // Check index range
    if (sysIx < 0) return NULL;
    if (sysIx >= (int)m_systems.size()) return NULL;

    if (sysIx == 0) return System::TheSystem();
	return m_systems[sysIx];
}

//-------------------------------------------------------------------------------

