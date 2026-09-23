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
// $Id: StringConversions.h 36 2023-07-06 15:16:35Z roman $

#ifndef __STRINGCONVERSIONS_H__
#define __STRINGCONVERSIONS_H__

#include "XercesUtils.h"

nstring IntToString(int x);
int StringToInt(const nstring& s);
nstring DoubleToString(double x);
double StringToDouble(const nstring& s);

// Convert std::string to nstring (handles UTF-8->wide conversion on Windows with UNICODE)
#if defined(UNICODE) || defined(_UNICODE)
#include <common/utils/win_utils.h>
inline nstring StringToNstring(const std::string& s) {
    return utils::utf8ToWide(s);
}
inline std::string NstringToString(const nstring& ws) {
    return utils::wideToUtf8(ws);
}
#else
inline nstring StringToNstring(const std::string& s) {
    return s;
}
inline std::string NstringToString(const nstring& s) {
    return s;
}
#endif

#endif /*__STRINGCONVERSIONS_H__*/
