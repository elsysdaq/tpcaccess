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
// $Id: StringConversions.h 2 2009-01-13 08:45:52Z roman $

#ifndef __STRINGCONVERSIONS_H__
#define __STRINGCONVERSIONS_H__

#include "XercesUtils.h"

nstring IntToString   (int x);
int     StringToInt   (const nstring& s);
nstring DoubleToString(double x);
double  StringToDouble(const nstring& s);

#endif /*__STRINGCONVERSIONS_H__*/
