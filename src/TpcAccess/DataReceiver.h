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
  $Id: DataReceiver.h 2 2009-01-13 08:45:52Z roman $
  Stores a list of data requests. Distributes the server data stream to the
  individual requests.
--------------------------------------------------------------------------------*/
#ifndef DataReceiver_h
#define DataReceiver_h DataReceiver_h
//---------------------------------------------------------------------------------

#include "intTypes.h"

#include "soapH.h"

#include "TpcAccess.h"

#include <vector>
using namespace std;

//---------------------------------------------------------------------------------


class DataRequest
{
public:
	// Constructor for "raw data requests"
	DataRequest(int boardAddress, int inputNumber, int blockNumber, void* data, TPC_ErrorCode* error);

	// Constructor for "volt requests"
	DataRequest(int boardAddress, int inputNumber, int blockNumber, void* data, TPC_ErrorCode* error, 
				uint32_t mask, double voltFactor, double voltConstant);

	virtual ~DataRequest() {}

	virtual ns__DataSpecification* GetSoapSpec(struct soap* soap) = 0;

	void SetError(TPC_ErrorCode error) { *m_errPtr = error; }

	void SetData(uint16_t* source, int length);
	void SetData(uint32_t* source, int length);

	virtual int DataLength() = 0;

protected:
	// Result pointers
	TPC_ErrorCode* m_errPtr;
	void* m_dataPtr;
	bool m_scale;
	uint32_t m_mask;
	double m_c, m_f;

	int m_boardAddress;
	int m_inputNumber;
	int m_blockNumber;
};


class DataRangeRequest : public DataRequest
{
public:
	// Constructor for "raw data requests"
	DataRangeRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, void* data, TPC_ErrorCode* error);

	// Constructor for "volt requests"
	DataRangeRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, void* data, TPC_ErrorCode* error,
				uint32_t mask, double voltFactor, double voltConstant);

	virtual ns__DataSpecification* GetSoapSpec(struct soap* soap);
	virtual int DataLength() { return m_resultLength; }

protected:
	int m_readoutType;  // 0:range - normal, resultLength = dataLength
					  // 1:range - minmax, resultLength <= 2*dataLength (pairs of min and max)
					  // 2:range - average, resultLength <= dataLength
	unsigned long long m_dataStart;
	unsigned long long m_dataLength;
	unsigned int m_resultLength;
};


class DataMinMaxRequest : public DataRequest
{
public:
	// Constructor for "raw data requests"
	DataMinMaxRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, void* data, TPC_ErrorCode* error);

	// Constructor for "volt requests"
	DataMinMaxRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, void* data, TPC_ErrorCode* error,
				uint32_t mask, double voltFactor, double voltConstant);

	virtual ns__DataSpecification* GetSoapSpec(struct soap* soap);
	virtual int DataLength() { return m_resultLength; }

protected:
	unsigned long long m_dataStart;
	unsigned long long m_dataLength;
	unsigned int m_resultLength;
};



class DataEnvelopeRequest : public DataRequest
{
public:
	// Constructor for "raw data requests"
	DataEnvelopeRequest(int boardAddress, int inputNumber, int blockNumber, 
										 uint64_t* indices, int numIndices, 
										 void* data, TPC_ErrorCode* error);

	// Constructor for "volt requests"
	DataEnvelopeRequest(int boardAddress, int inputNumber, int blockNumber, 
										 uint64_t* indices, int numIndices, 
										 void* data, TPC_ErrorCode* error,
										uint32_t mask, double voltFactor, double voltConstant);

	virtual ~DataEnvelopeRequest();

	virtual ns__DataSpecification* GetSoapSpec(struct soap* soap);
	virtual int DataLength() { return m_numIndices-1; }

protected:
	uint64_t* m_indices;
	int m_numIndices;
};


//----------------


class DataReceiver
{
public: // Client side
	DataReceiver();
	~DataReceiver();

	void Clear();
	void AddDataRangeRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, double* data, TPC_ErrorCode* error,
				uint32_t mask, double factor, double constant);

	void AddRawDataRangeRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, int32_t* data, TPC_ErrorCode* error);

	void AddDataMinMaxRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, double* data, TPC_ErrorCode* error,
				uint32_t mask, double factor, double constant);

	void AddRawDataMinMaxRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, int32_t* data, TPC_ErrorCode* error);

	ns__DataSpecificationArray PrepareForSoap(struct soap* soap);

public: // Soap side
	void PushBytes(uint8_t* bytes, int count);

private:
	vector<DataRequest*> m_requests;

private:
	int m_totalRecs;
	int m_recsToGo;
	int m_hdrToGo;
	int m_dataToGo;
	uint8_t* m_hdrPtr;
	uint8_t* m_dataPtr;
	int m_recNr;

	ns__DataBlockHeader m_hdr;
	uint8_t* m_dataBlock;
	
private:
	void EndianSwap(int& x);
	void EndianSwap(unsigned int& x);
	void EndianSwap(uint8_t* x, int bytes);
};



//---------------------------------------------------------------------------------
#endif // DataReceiver_h

