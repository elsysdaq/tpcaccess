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
// $Id: TpcSettings.h 29 2019-09-26 10:22:37Z roman $

#ifndef __TPCSETTINGS_H__
#define __TPCSETTINGS_H__

#include <string>
#include "XercesUtils.h"

#include "TpcAccess.h"

TPC_ErrorCode WriteSettingFile(const std::string& filename, std::string& errormsg);
TPC_ErrorCode LoadSettingFile (const std::string& filename, std::string& errormsg);


// Table of all known parameters
struct Param
{
    TPC_Parameter par;  // Parameter.
    TCHAR* name;        // Name of the parameter in the xml setting file.
    bool timebase;      // True if it is a time base parameter (and need to be saved only once per board)
};

const int numKnownParameters = 69;		
extern const Param paramTable[numKnownParameters];

int SearchParamName(const nstring& name);



#endif /*__TPCSETTINGS_H__*/
