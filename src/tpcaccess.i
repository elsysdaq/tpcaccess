%include <windows.i>
%include <cpointer.i>
%include <carrays.i>
%include "stdint.i"
%include "typemaps.i"

%apply double *OUTPUT { double* value }
%apply int *OUTPUT { int* measurementNumber }
%module TpcAccess
 %{
	/* Put header files here or function declarations like below */
	#include "../TpcAccess/TpcAccessA.h"
	#include "../TpcAccess.h"
 %}

%include "TpcAccess/TpcAccessA.h"
%include "TpcAccess.h"
%pointer_functions(int32_t, int32_tp);
%array_functions(int32_t, int32_tArray);
%array_functions(double, doubleArray);