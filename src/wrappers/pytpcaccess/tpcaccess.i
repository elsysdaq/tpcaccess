%module TpcAccess

%include <windows.i>
%include <cpointer.i>
%include <carrays.i>
%include <stdint.i>
%include <typemaps.i>

%apply double *OUTPUT { double* value }
%apply int *OUTPUT { int* measurementNumber }

%{
#include "TpcAccess.h"
#include "TpcAccessA.h"
%}

%include "../../../include/TpcAccess.h"
%include "../tpcaccess/TpcAccessA.h"

%pointer_functions(int32_t, int32_tp);
%array_functions(int32_t, int32_tArray);
%array_functions(double, doubleArray);
