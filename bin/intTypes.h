/*--------------------------------------------------------------------------------
$Id: intTypes.h 416 2019-10-12 09:23:25Z roman $
Integer types.
--------------------------------------------------------------------------------*/
#ifndef intTypes_h
#define intTypes_h intTypes_h
//---------------------------------------------------------------------------------


// Linux
#ifdef _LINUX
#include <stdint.h>
//typedef unsigned long long  uint64_t;
//typedef signed long	long	int64_t;
#endif


// Win32
#ifdef WIN32
#include <stdint.h>
/*
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;	
typedef unsigned short uint16_t;
typedef signed long int32_t;
typedef unsigned long uint32_t;
typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;*/
#endif


//---------------------------------------------------------------------------------
#endif // intTypes_h

