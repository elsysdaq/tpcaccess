#ifndef TPCACCESS_EXAMPLES_SCOPE_EXAMPLE_H
#define TPCACCESS_EXAMPLES_SCOPE_EXAMPLE_H

#include "TpcAccess.h"

TPC_StatusCallbackFunc myCallbackFunctionPointer;
volatile int m_iMeasurementNumber;
volatile bool m_recordingStopped;
int m_iBlockSize;

void TPC_CC StatusCallback(void* userData, int deviceIx, struct TPC_DeviceStatus* status);
bool CheckErrorCode(TPC_ErrorCode err);

#endif  // TPCACCESS_EXAMPLES_SCOPE_EXAMPLE_H
