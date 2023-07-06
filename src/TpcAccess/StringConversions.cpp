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
// $Id: StringConversions.cpp 21 2014-01-14 16:12:52Z roman $

#include "StringConversions.h"

#include <stdio.h>
#include <stdexcept>
#include <sstream>

#ifdef _LINUX
  #include "errno.h"
#endif
using namespace std;

#if _MSC_VER >= 1400
#pragma warning(push)
#pragma warning(disable: 4996) /* 4996: 'xxx' was declared deprecated */
#endif


nstring IntToString(int x)
{
   TCHAR s[80];
#if defined(UNICODE) || defined(_UNICODE)
   _itow(x, s, 10);
#else
   sprintf(s,"%20d",x);
#endif
   return s;
}

int StringToInt(const nstring& s)
{
   TCHAR* end;
   errno=0;
#if defined(UNICODE) || defined(_UNICODE)
   long x = wcstol(s.c_str(), &end, 10);
#else
   long x = strtol(s.c_str(), &end, 10);
#endif
   int xint = int(x);
   if (errno || end == s.c_str() || xint != x) {
      throw std::invalid_argument("invalid int syntax");
   }
   while (*end != _TEXT('\0')) {
      if (*end != _TEXT(' ') && *end != _TEXT('\t')) {
         throw std::invalid_argument("invalid int syntax");
      }
      ++end;
   }
   return x;
}

nstring DoubleToString(double x)
{
   TCHAR s[80];
#if defined(UNICODE) || defined(_UNICODE)
   swprintf(s,_TEXT("%.20G"),x);
#else
   sprintf(s,_TEXT("%.20G"),x);
#endif
   return s;
}

double StringToDouble(const nstring& s)
{
   TCHAR* end;
   errno=0;
#if defined(UNICODE) || defined(_UNICODE)
   double x = wcstod(s.c_str(), &end);
#else
   double x = strtod(s.c_str(), &end);
#endif
   if (errno || end == s.c_str()) {
      throw std::invalid_argument("invalid double syntax");
   }
   while (*end != _TEXT('\0')) {
      if (*end != _TEXT(' ') && *end != _TEXT('\t')) {
         throw std::invalid_argument("invalid double syntax");
      }
      ++end;
   }
   return x;
}


#if _MSC_VER >= 1400
#pragma warning(default: 4996)
#pragma warning(pop)
#endif
