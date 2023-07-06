/// --------------------------------------------------------------------------------------------------
/// TpcAccess Additional Function needed for Python/SWIG interface which don't support 
/// Structures in parameter lists. 
/// Copyright Elsys AG 2022
/// Author: Roman Bertschi
/// --------------------------------------------------------------------------------------------------


#ifndef TpcAccessA_h
#define TpcAccessA_h TpcAccessA_h

#include "TpcAccess.h"
#include "intTypes.h"




// Calling convention
#ifdef WIN32
#define TPC_CC __stdcall
#else
#define TPC_CC
#endif


// Export/Import qualifier 
#ifdef WIN32
#ifdef BUILDING_TPC_ACCESS
#define TPC_EXP __declspec(dllexport)
#else
#define TPC_EXP __declspec(dllimport)
#endif
#else
#define TPC_EXP
#endif


// declare C linkage for the exported functions if in C++
#ifdef __cplusplus
extern "C" {
#endif

	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetInputRange(int deviceIx, int boardAddress, int inputNumber, int RangeIdx, double* value);
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetChargeInputRange(int deviceIx, int boardAddress, int inputNumber, int RangeIdx, double* value);

	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetBoardStatus(int deviceIx, int boardAddress, struct TPC_BoardStatus* status, int structSize);
	TPC_EXP enum TPC_ErrorCode TPC_CC TPC_GetInputStatus(int deviceIx, int boardAddress, int inputNumber, struct TPC_InputStatus* status, int structSize);


#ifdef __cplusplus
} // extern "C"
#endif
//---------------------------------------------------------------------------------
#endif // TPCAccess_h
