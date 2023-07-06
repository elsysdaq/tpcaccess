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
  $Id: SystemList.h 2 2009-01-13 08:45:52Z roman $
  This class holds all information and state for the system.
--------------------------------------------------------------------------------*/
#ifndef SystemList_h
#define SystemList_h SystemList_h
//---------------------------------------------------------------------------------

#include "TpcAccess.h"
#include "TpcAccessElsys.h"

#include <vector>
using namespace std;

#include "System.h"

//---------------------------------------------------------------------------------


#define SYSTEM_MULTIPLIER 0x1000; // systemId * SYSTEM_MULTIPLIER + deviceIx

class SystemList
{
public: 
	int NewSystem();
    TPC_ErrorCode DeleteSystem(int deviceIx);
	System* FindSystem(int deviceIx);

public:
	static SystemList* TheSystemList();
	static void PrepareToQuitProgram();

private:
	static SystemList* s_singleton;
	SystemList();
    ~SystemList();

private:
	vector<System*> m_systems;
};


//---------------------------------------------------------------------------------
#endif // SystemList_h

