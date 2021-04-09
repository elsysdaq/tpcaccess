/*
$Id: xerces_link.cpp 2 2009-01-13 08:45:52Z roman $

The TpcAccess project needs the XERCES library for XML processing.

Two steps are necessary to build TpcAccess with the static Xerces library on Windows:
- Download and build the Xerces static library.
- Let TpcAccess use it.

Building Xerces:
 1. Download the source distribution of Xerces (see http://xml.apache.org/xerces-c/)
 2. Unzip xerces-c-src_2_7_0.zip to a new directory <Dir> on your machine.
 3. Load <Dir>\Projects\Win32\VC7.1\xerces-all\xerces-all.sln into the Visual
    Studio version used to compile TpcAccess. (Convert project files as necessary)
 4. For the project XercesLib in the "Static Debug":
    Change  "C++ / Output Files / Program Database File Name" to be the
    same as "Librarian / General /Output File" except to replace the
    extension .lib by .pdb.
 5. For the Win32 platform add "\x86" after "StaticDebug" or "StaticRelease"
    for all directories. (In General, C++/Output Files, Librarian)
 6. Create a x64 Platform for the "Static Debug" and "Static Release"
    configurations of the XercesLib project.
    (Configuration-Manager for "Static Debug": on Platform of XercesLib
    <New...> x64 with Create new solution platforms checked.)
 7. For all directories (In General, C++/Output Files, Librarian) on the x64
    platform: use the same values as in the Win32 platform but replace
    "\x86" by "\x64".
 8. Add "WIN64" the the preprocessor definitions of the "Static Debug" and
    "Static Release" configuration of the x64 platform.
 9. Build the project XercesLib in the "Static Debug" and "Static Release"
    configurations for the platforms Win32 and x64.
10. create directory <Dir>\lib\<platform> and copy
    <Dir>\Build\Win32\VC7.1\StaticDebug\<platform>\Xerces-c_static_2D.lib
    <Dir>\Build\Win32\VC7.1\StaticDebug\<platform>\Xerces-c_static_2D.pdb
    <Dir>\Build\Win32\VC7.1\StaticRelease\<platform>\Xerces-c_static_2.lib
    to it.

Setting up TpcAccess to use it:
 1. Add an environement variable XERCES that points to the directory where you 
    unzipped the xerces source code. e.g. if you unzipped the contents of 
    "xerces-c-src_2_7_0" from the zip into c:\xerces\xerces-c-src_2_7_0, 
    set XERCES = c:\xerces\xerces-c-src_2_7_0  (no backslash at the end)
 2. Add $(XERCES)\src to the additional include directories.
 3. Add $(XERCES)\lib\<platform>\Xerces-c_static_2D.lib and
    $(XERCES)\lib\<platform>\Xerces-c_static_2.lib to the linker input for
    debug and release configurations respectively.
    (Alternatively uncomment the "pragma comment(lib ... Xerces-c_static...") below
    if the path is static or relative. Node $(XERCES) does not work in this context!)
 4. Add XML_LIBRARY to the preprocessor definitions.
 5. Use the Multi-threaded Debug (/MTd)" and "Multi-threaded (/MT)" for
    debug and release configurations respectively.
 6. Include this file to the project.

Instructions for Linux:
{tbd}

*/

#ifndef XML_LIBRARY
# error "The symbol XML_LIBRARY must be defined in the project settings."
#endif

#ifdef _MSC_VER
# pragma comment(lib, "ws2_32.lib")
# pragma comment(lib, "advapi32.lib")
# ifdef NDEBUG
// #  pragma comment(lib, "../../xerces/lib/Xerces-c_static_2.lib")
# endif
# ifdef _DEBUG
// #  pragma comment(lib, "../../xerces/lib/Xerces-c_static_2D.lib")
# endif
#endif /*_MSC_VER*/
