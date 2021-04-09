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
 * (C) Copyright 2005 - 2019 Elsys AG. All rights reserved.
*/

//---------------------------------------------------------------------------
/*--------------------------------------------------------------------------------
  $Id: YMetaData.h 2 2009-01-13 08:45:52Z roman $
  Keeps y meta data for each input
--------------------------------------------------------------------------------*/
#ifndef YMetaData_h
#define YMetaData_h YMetaData_h
//---------------------------------------------------------------------------------

#include "TpcAccess.h"

//---------------------------------------------------------------------------------

struct RawYMetaData
{
	bool isActive;
	int resolutionInBits;
	unsigned int analogMask;
	unsigned int markerMask;
	int numberOfMarkerBits;
	int bytesPerSample;
	double binToVoltFactor;
	double binToVoltConst;
	double voltToPhysicalFactor;		
	double voltToPhysicalConstant;	
};


class YMetaDataList
{
public:
	YMetaDataList();

	void SetMetaData(int boardAddress, int inputNumber, const RawYMetaData& metadata);
	bool GetMetaData(int boardAddress, int inputNumber, RawYMetaData* metadata);

	void Clear();

private:
	RawYMetaData m_data[tpc_maxBoards][tpc_maxInputs];
};


//---------------------------------------------------------------------------------
#endif // YMetaData_h

