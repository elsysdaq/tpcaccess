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
  $Id: main.cpp 24 2015-01-05 10:58:24Z roman $
  TpcAccess Main file  
--------------------------------------------------------------------------------

Module:
-------

main.cpp 
	DLL Entry und exit points.

TpcAccess.h
	Public Header fr TpcAccess.dll

TpcAccess.cpp
	Implementation der Funktionen aus TpcAccess.h.
	Die allermeisten Funktionen werden zur Klasse System weritergeleitet.

System.h, System.cpp
	Die Klasse System beinhaltet alle Daten des Systems. Im Moment wird nur
	ein System untersttzt, es w�e aber problemlos m�lich durch instantiieren
	von mehreren System-Klassen mehrere Systeme gleichzeitig zu bedienen.
	Hier werden Koordinations-Aufgaben ber die Devices ausgefhrt. Was lokal
	fr einen Device behandelt werden kann, wird an die Klasse Device weitergeleitet.

Device.h, Device.cpp
	Die Klasse Device verwaltet einen einzelnen Device des Systems.

SoapDevice.h, SoapDevice.cpp
	Layer ber die Soap-Verbindung zu einem Device. Verwaltet die Verbindung,
	macht Memory management, konvertiert Parameter und analysiert Fehlermeldungen. 

DataReader.h, DataReader.cpp
	Speichert eine Liste von Daten-Abfragen. Zerlegt den Bytestrom vom 
	Server fr die Daten-Abfragen in die einzelnen Datenpakete.

BlockInfoCache.h, BlockInfoCache.cpp
	H�t Kopien der EOR-Info fr eine begrenzte Anzahl Bl�ke.

DeviceInfo.h, DeviceInfo.cpp
	Keeps a copy of the device info (device, board, input).

ParameterSet.h, ParameterSet.cpp
	Enth�t zwei Klassen, eine zum Speichern einer Kopie des kompletten
	Parameter-Sets, und eine zum Speichern von einzelnen Set-Commands.

Attributes.h, Attributes.cpp
	Container-Klasse fr Attribute.

*/

//-------------------------------------------------------------------------------

#ifdef WIN32 


#include <windows.h>
#include "TpcAccessInstance.h"
#endif
#include "System.h"

#include "TransPC_Server.nsmap" // obtain the namespace mapping table



//-------------------------------------------------------------------------------

#ifndef _PYTHON
void DllInit()
{
}

void DllDone()
{
	System::PrepareToQuitProgram();
}

#endif

//=== Main entry point =================================================================


#ifdef WIN32 

BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	// The DllMain function is called whenever the DLL is loaded and unloaded.
	// Place Initialization code for the DLL in this function.
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// Place any initialization which needs to be done when the DLL is loaded here.
        TpcAccessInstance=hinstDLL;
		DllInit();	
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		// Place any finalization which needs to be done when the DLL is unloaded here.
		// DllDone(); *** Not possible, because at this point it is no longer possible
        //                to communicate with threads -> TPC_TerminateTpcAccess().	
	}

	// return FALSE to abort if initialization fails.
	return TRUE;
}

#elif _PYTHON

#elif _LINUX

static void __attribute__ ((__constructor__)) init()
{
	DllInit();
}


static void __attribute__ ((__destructor__)) fini()
{
	DllDone();
}

#else

#warning not implemented

#endif

//-------------------------------------------------------------------------------


