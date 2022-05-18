// ********************************************************************************/
/* TpcAccess API Example
/*
/* Copyright: 2009 ELSYS AG
/* Programmer: Roman Bertschi
/*
/* Contact: email: info@elsys.ch
/*
// ********************************************************************************/
#pragma once
#include "TpcAccess.h"

// globals
TPC_StatusCallbackFunc myCallbackFunctionPointer;
volatile int m_iMeasurementNumber;
volatile bool m_recordingStopped;
int m_iBlockSize;

// Function declaration
void __stdcall StatusCallback(void* userData, int deviceIx, struct TPC_DeviceStatus* status);
bool CheckErrorCode(TPC_ErrorCode err);