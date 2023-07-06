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
  $Id: DataReceiver.cpp 2 2009-01-13 08:45:52Z roman $
  Stores a list of data requests. Distributes the server data stream to the
  individual requests.
--------------------------------------------------------------------------------*/

#include "DataReceiver.h"

//-------------------------------------------------------------------------------


DataRequest::DataRequest(int boardAddress, int inputNumber, int blockNumber, void* data, TPC_ErrorCode* error)
{
	m_boardAddress = boardAddress;
	m_inputNumber = inputNumber;
	m_blockNumber = blockNumber;
	m_errPtr = error;
	m_dataPtr = data;
	m_scale = false;
}



DataRequest::DataRequest(int boardAddress, int inputNumber, int blockNumber, void* data, TPC_ErrorCode* error,
				uint32_t mask, double voltFactor, double voltConstant)
{

	m_boardAddress = boardAddress;
	m_inputNumber = inputNumber;
	m_blockNumber = blockNumber;
	m_errPtr = error;
	m_dataPtr = data;
	m_scale = true;
	m_mask = mask;
	m_f = voltFactor;
	m_c = voltConstant;
}



void DataRequest::SetData(uint16_t* source, int length) 
{
	if (m_scale) {
		for (int i=0; i<length; i++)
			((double*)m_dataPtr)[i] = (double)((source[i] & m_mask) * m_f + m_c);
	}
	else {
		for (int i=0; i<length; i++)
			((int32_t*)m_dataPtr)[i] = (uint16_t)source[i];
	}
}



void DataRequest::SetData(uint32_t* source, int length) 
{
	if (m_scale) {
		for (int i=0; i<length; i++)
			((double*)m_dataPtr)[i] = (double)((source[i] & m_mask) * m_f + m_c);
	}
	else {
		for (int i=0; i<length; i++)
			((int32_t*)m_dataPtr)[i] = (int32_t)source[i];
	}
}






DataRangeRequest::DataRangeRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, void* data, TPC_ErrorCode* error)
	: DataRequest(boardAddress, inputNumber, blockNumber, data, error)
{
	m_readoutType = 0; // normal
	if (resultLength < dataLength) m_readoutType = 1; // minmax

	m_dataStart = dataStart;
	m_dataLength = dataLength;
	m_resultLength = resultLength;
}



DataRangeRequest::DataRangeRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, void* data, TPC_ErrorCode* error,
				uint32_t mask, double voltFactor, double voltConstant)
	: DataRequest(boardAddress, inputNumber, blockNumber, data, error, mask, voltFactor, voltConstant)
{
	m_readoutType = 0; // normal
	if (resultLength < dataLength) m_readoutType = 1; // minmax

	m_dataStart = dataStart;
	m_dataLength = dataLength;
	m_resultLength = resultLength;
}



ns__DataSpecification* DataRangeRequest::GetSoapSpec(struct soap* soap)
{
	ns__RangeDataSpecification* r = soap_new_ns__RangeDataSpecification(soap, -1);
	r->boardAddress = m_boardAddress;
	r->inputNumber = m_inputNumber;
	r->blockNumber = m_blockNumber;
	r->readoutType = m_readoutType;
	r->dataStart = m_dataStart;
	r->dataLength = m_dataLength;
	r->resultLength = m_resultLength;
	return r;
}





DataMinMaxRequest::DataMinMaxRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, void* data, TPC_ErrorCode* error)
	: DataRequest(boardAddress, inputNumber, blockNumber, data, error)
{
	m_dataStart = dataStart;
	m_dataLength = dataLength;
	m_resultLength = resultLength;
}



DataMinMaxRequest::DataMinMaxRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, void* data, TPC_ErrorCode* error,
				uint32_t mask, double voltFactor, double voltConstant)
	: DataRequest(boardAddress, inputNumber, blockNumber, data, error, mask, voltFactor, voltConstant)
{
	m_dataStart = dataStart;
	m_dataLength = dataLength;
	m_resultLength = resultLength;
}



ns__DataSpecification* DataMinMaxRequest::GetSoapSpec(struct soap* soap)
{
	ns__RangeDataSpecification* r = soap_new_ns__RangeDataSpecification(soap, -1);
	r->boardAddress = m_boardAddress;
	r->inputNumber = m_inputNumber;
	r->blockNumber = m_blockNumber;
	r->readoutType = 1;
	r->dataStart = m_dataStart;
	r->dataLength = m_dataLength;
	r->resultLength = m_resultLength;
	return r;
}



DataEnvelopeRequest::DataEnvelopeRequest(int boardAddress, int inputNumber, int blockNumber, 
										 uint64_t* indices, int numIndices, void* data, TPC_ErrorCode* error)
	: DataRequest(boardAddress, inputNumber, blockNumber, data, error)
{
	m_indices = new uint64_t[numIndices];
	for (int i=0; i<numIndices; i++) m_indices[i] = indices[i];
	m_numIndices = numIndices;
}



DataEnvelopeRequest::DataEnvelopeRequest(int boardAddress, int inputNumber, int blockNumber, 
										 uint64_t* indices, int numIndices, void* data, TPC_ErrorCode* error, 
										 uint32_t mask, double voltFactor, double voltConstant)
	: DataRequest(boardAddress, inputNumber, blockNumber, data, error, mask, voltFactor, voltConstant)
{
	m_indices = new uint64_t[numIndices];
	for (int i=0; i<numIndices; i++) m_indices[i] = indices[i];
	m_numIndices = numIndices;
}



DataEnvelopeRequest::~DataEnvelopeRequest()
{
	delete[] m_indices;
}



ns__DataSpecification* DataEnvelopeRequest::GetSoapSpec(struct soap* soap)
{
	ns__EnvelopeDataSpecification* r = soap_new_ns__EnvelopeDataSpecification(soap, -1);
	r->boardAddress = m_boardAddress;
	r->inputNumber = m_inputNumber;
	r->blockNumber = m_blockNumber;
	r->indices.__size = m_numIndices;
	r->indices.__ptr = (uint64_t*)soap_malloc(soap, sizeof(uint64_t)*m_numIndices);
	for (int i=0; i<m_numIndices; i++) r->indices.__ptr[i] = m_indices[i];
	return r;
}



//-------------------------------------------------------------------------------


DataReceiver::DataReceiver()
{
}



DataReceiver::~DataReceiver()
{
	Clear();
}



void DataReceiver::Clear()
{
	while (!m_requests.empty()) {
		DataRequest* d = m_requests.back();
		m_requests.pop_back();
		delete d;
	}
}



void DataReceiver::AddDataRangeRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, double* data, TPC_ErrorCode* error, 
				uint32_t mask, double factor, double constant)
{
	DataRangeRequest* r = new DataRangeRequest(boardAddress, inputNumber, blockNumber, 
				dataStart, dataLength, 
			    resultLength, data, error, mask, factor, constant);
	m_requests.push_back(r);
}



void DataReceiver::AddRawDataRangeRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, int32_t* data, TPC_ErrorCode* error)
{
	DataRangeRequest* r = new DataRangeRequest(boardAddress, inputNumber, blockNumber, 
				dataStart, dataLength, 
			    resultLength, data, error);
	m_requests.push_back(r);
}



void DataReceiver::AddDataMinMaxRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, double* data, TPC_ErrorCode* error, 
				uint32_t mask, double factor, double constant)
{
	DataMinMaxRequest* r = new DataMinMaxRequest(boardAddress, inputNumber, blockNumber, 
				dataStart, dataLength, 
			    resultLength, data, error, mask, factor, constant);
	m_requests.push_back(r);
}



void DataReceiver::AddRawDataMinMaxRequest(int boardAddress, int inputNumber, int blockNumber, 
				uint64_t dataStart, uint64_t dataLength, 
			    int resultLength, int32_t* data, TPC_ErrorCode* error)
{
	DataMinMaxRequest* r = new DataMinMaxRequest(boardAddress, inputNumber, blockNumber, 
				dataStart, dataLength, 
			    resultLength, data, error);
	m_requests.push_back(r);
}




ns__DataSpecificationArray DataReceiver::PrepareForSoap(struct soap* soap)
{
	int n = m_requests.size();
	ns__DataSpecificationArray r;
	r.__size = n;
	r.__ptr = (ns__DataSpecification**)soap_malloc(soap, sizeof(ns__DataSpecification*)*n);
	for (int i=0; i<n; i++) {
		DataRequest* req = m_requests[i];
		ns__DataSpecification* spec = req->GetSoapSpec(soap);
		r.__ptr[i] = spec;
	}

	m_totalRecs = n;
	m_recsToGo = n;
	m_hdrToGo = 0;
	m_dataToGo = 0;
	m_hdrPtr = NULL;
	m_dataPtr = NULL;
	m_recNr = 0;
	m_dataBlock = NULL;

	// Prepare for first record
	if (m_recsToGo > 0) {
		m_hdrPtr = (uint8_t*)&m_hdr;
		m_hdrToGo = sizeof(ns__DataBlockHeader);
	}

	return r;
}



#ifdef TPC_BIG_ENDIAN

void DataReceiver::EndianSwap(int& x)
{
	uint8_t t;
	uint8_t* p = (uint8_t*)&x;
	t = p[0];
	p[0] = p[3];
	p[3] = t;
	t = p[1];
	p[1] = p[2];
	p[2] = t;
}

void DataReceiver::EndianSwap(unsigned int& x)
{
	uint8_t t;
	uint8_t* p = (uint8_t*)&x;
	t = p[0];
	p[0] = p[3];
	p[3] = t;
	t = p[1];
	p[1] = p[2];
	p[2] = t;
}

void DataReceiver::EndianSwap(uint8_t* x, int bytes, int bytesPerSample)
{
	uint8_t t;
	if (bytesPerSample == 2) {
		for (int i=0; i<bytes; i+=2) {
			t = x[0];
			x[0] = x[1];
			x[1] = t;
			x += 2;
		}
	}
	else if (bytesPerSample == 4) {
		for (int i=0; i<bytes; i+=4) {
			t = x[0];
			x[0] = x[3];
			x[3] = t;
			t = x[1];
			x[1] = x[2];
			x[2] = t;
			x += 4;
		}
	}
}

#endif


void DataReceiver::PushBytes(uint8_t* bytes, int count)
{


// 2 Blocks: 4096 + 16 Bytes: Beim zweiten Block wird wieder der Header gefllt

	while (count > 0 && m_recsToGo > 0) {
		if (m_hdrToGo > 0) { 
			// Continue receiving header
			// Determine number of bytes to copy.
			int n = m_hdrToGo;
			if (n > count) n = count;
			// Copy n bytes
			for (int i=0; i<n; i++) {
				*m_hdrPtr = *bytes;
				m_hdrPtr++;
				bytes++;
			}
			m_hdrToGo -= n;
			count -= n;

			if (m_hdrToGo == 0) {
				#ifdef TPC_BIG_ENDIAN
					// Endian swap
					EndianSwap(m_hdr.error);
					EndianSwap(m_hdr.numberOfSamples);
					EndianSwap(m_hdr.sampleFormat);
					EndianSwap(m_hdr.totalBytes);
				#endif
				// Get data size from header
				m_dataToGo = m_hdr.totalBytes; 
				// Allocate space for the data
				if (m_dataToGo == 0) m_dataBlock = NULL; else m_dataBlock = new uint8_t[m_dataToGo];
				m_dataPtr = m_dataBlock;
			}
		}

		if (m_dataToGo > 0) {
			// Continue receiving data

			// Determine number of bytes to copy.
			int n = m_dataToGo;
			if (n > count) n = count;
			// Copy n bytes
			for (int i=0; i<n; i++) {
				*m_dataPtr = *bytes;
				m_dataPtr++;
				bytes++;
			}
			m_dataToGo -= n;
			count -= n;
		}

		if (m_hdrToGo == 0 && m_dataToGo == 0) {
			#ifdef TPC_BIG_ENDIAN
				// Endian swap
				int bytesPerSample = (m_hdr.sampleFormat == 0) ? 2 : 4;
				EndianSwap(m_dataBlock, n, bytesPerSample);
			#endif

			// Deliver the data
			DataRequest* req = m_requests[m_recNr];
			req->SetError((TPC_ErrorCode)m_hdr.error);
			
			if ((unsigned int)req->DataLength() != m_hdr.numberOfSamples) {
				if (m_hdr.error == 0) {
					req->SetError(tpc_errInternalError);
				}
			}
			else {
				req->SetError(tpc_noError);
				if (m_dataBlock != NULL && m_hdr.error == 0 && m_hdr.numberOfSamples > 0) {
					switch (m_hdr.sampleFormat) {
					case 0: {
						req->SetData((uint16_t*)m_dataBlock, m_hdr.numberOfSamples);
						break;
					}
					case 1: {
						req->SetData((uint32_t*)m_dataBlock, m_hdr.numberOfSamples);
						break;
					}
					default:
						req->SetError(tpc_errInternalError);
						break;
					}
				}
			}
			if (m_dataBlock != NULL) {
				delete[] m_dataBlock;
				m_dataBlock = NULL;
			}

			// Next record
			m_recNr++;
			m_recsToGo--;
			if (m_recsToGo) {
				m_hdrPtr = (uint8_t*)&m_hdr;
				m_hdrToGo = sizeof(ns__DataBlockHeader);
				m_dataPtr = NULL;
			}
		}
	}

}


//-------------------------------------------------------------------------------


