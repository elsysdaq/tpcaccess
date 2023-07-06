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
// $Id: TpcSettingsToXml.cpp 2 2009-01-13 08:45:52Z roman $

#include "TpcSettingsToXml.h"

#include "StringConversions.h"

#include "xerces_include_start.h"
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include "xerces_include_end.h"

#include "TpcAccess.h"
#include "System.h"

#include "TpcSettings.h"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// Note: the TpcSettings Version below should match the schema that correponds to
// the XML data this module writes.
// This enables humans as well as read routines to recognize the exact format
// of the file. (Version 1.0 never contains a certain <Extension> element
// whereas version 1.1 contains this extension.)
static const char* const Empty_TcpSettings_XML =
"<?xml version='1.0' encoding='utf-8'?>"
"<TpcSettings xmlns='http://www.elsys.ch/schema/transas/tpcsettings-1.x.xsd' Version='1.0' >" // <<<--- TpcSettings Version 
"</TpcSettings>";


#ifndef _LINUX
static const XMLCh* TPCXNAMESPACE = L"http://www.elsys.ch/schema/transas/tpcsettings-1.x.xsd";
#endif

namespace {

   class XSTR {
#if defined(UNICODE) || defined(_UNICODE)
      public:
      XSTR(const TCHAR* s) : x(s) {} // s must be valid for the entire lifetime of XSTR
      operator const XMLCh*() { return x; }
      private:
      const XMLCh* x;
#else
      public:
      XSTR(const TCHAR* s) {
         x=X::XMLString::transcode(s);
      }
      ~XSTR() {
         X::XMLString::release(&x);
      }
      operator const XMLCh*() { return x; }
      private:
      XMLCh* x;
#endif
   };
}

static X::DOMElement* AddContainer(X::DOMElement* parent, const XMLCh *element_name)
{
#ifdef _LINUX
 X::DOMElement* e = parent->getOwnerDocument()->createElementNS(
			TO_XMLCH(L"http://www.elsys.ch/schema/transas/tpcsettings-1.x.xsd"),element_name);
#else
 X::DOMElement* e = parent->getOwnerDocument()->createElementNS(TPCXNAMESPACE,element_name);
#endif
  
   parent->appendChild(e);
   return e;
}

static inline X::DOMElement* AddEmptyElement(X::DOMElement* parent, const XMLCh *element_name)
{
   return AddContainer(parent, element_name);
}

static X::DOMElement* AddLeaf(X::DOMElement* parent, const XMLCh *element_name, const nstring& text)
{
#ifdef _LINUX
 X::DOMElement* e = parent->getOwnerDocument()->createElementNS(
			TO_XMLCH(L"http://www.elsys.ch/schema/transas/tpcsettings-1.x.xsd"),element_name);
#else
   X::DOMElement* e = parent->getOwnerDocument()->createElementNS(TPCXNAMESPACE,element_name);
#endif 
   XSTR t(text.c_str());
   e->setTextContent(t);
   parent->appendChild(e);
   return e;
}

static X::DOMElement* AddAttribute(X::DOMElement* elem, const XMLCh *name, const nstring& value)
{
   XSTR v(value.c_str());
   elem->setAttribute(name, v);
   return elem;
}


#ifdef _LINUX
static X::DOMElement* AddContainer(X::DOMElement* parent, const wchar_t *element_name)
{
   return AddContainer(parent, TO_XMLCH(element_name));
}

static inline X::DOMElement* AddEmptyElement(X::DOMElement* parent, const wchar_t *element_name)
{
   return AddEmptyElement(parent, TO_XMLCH(element_name));
}

static X::DOMElement* AddLeaf(X::DOMElement* parent, const wchar_t *element_name, const nstring& text)
{
   return AddLeaf(parent, TO_XMLCH(element_name), text);
}

static X::DOMElement* AddAttribute(X::DOMElement* elem, const wchar_t *name, const nstring& value)
{
  return AddAttribute(elem, TO_XMLCH(name), value);
}
#endif



namespace {
    template<class T, size_t defsize>
    class GetBuffer {
        public:
        GetBuffer() : buf(statbuf) {}
        ~GetBuffer() { if (buf != statbuf) { delete[] buf; } }

        void     Allocate (size_t size) // does not preserve buffer contents
        {
            if (size <= defsize) {
                if (buf != statbuf) { delete[] buf; buf=statbuf; }
            } else {
                if (buf == statbuf || dynsize < size) {
                    if (buf != statbuf) { delete[] buf; buf=0; }
                    buf=new T[size]; dynsize=size;
                }
            }
        }

        size_t   GetSize() const { return (buf != statbuf) ? dynsize : defsize; }
        const T* GetPtr()  const { return buf; }
              T* GetPtr()        { return buf; }

        private:
        GetBuffer(const GetBuffer& rhs);            // not allowed
        GetBuffer& operator=(const GetBuffer& rhs); // not allowed

        T      statbuf[defsize];
        T*     buf;
        size_t dynsize;
    };
}

static void CheckStatus(TPC_ErrorCode status)
{
    if (status != tpc_noError) {
        throw status;
    }
}

static std::string GetDeviceUrl(int index)
{
    GetBuffer<char,256> buf;
    for(;;) {
        TPC_ErrorCode status = TPC_GetDeviceUrl(index, buf.GetPtr(), buf.GetSize());
        if (status == tpc_noError) {
            return buf.GetPtr();
        } else if (status != tpc_errBufferTooShort) {
            CheckStatus(status);
            abort(); // should not be reachable
        }
        buf.Allocate(buf.GetSize()*2);
    }
}

static bool TPC_CC AttributeEnumeratorCallback
   (void* userData, 
	int boardAddress, int inputNumber, 
	const char* key, const char* value)
{
    X::DOMElement* e_attributes = reinterpret_cast<X::DOMElement*>(userData);
    AddAttribute(AddAttribute(AddAttribute(AddLeaf(e_attributes,
        L"Attribute", value),
        L"BoardAddress", IntToString(boardAddress)),
        L"InputNumber", IntToString(inputNumber)),
        L"Name", key);
    return true;
}



DOMDocumentHandle CreateXmlTpcSettings()
{
    DOMDocumentHandle doc(BasicDOMInOut().ParseString(Empty_TcpSettings_XML));
    X::DOMElement* e_settings = doc->getDocumentElement();
    X::DOMElement* e_devices = AddContainer(e_settings, L"Devices");
    
    // loop devices
    // The following line ...
    // int num_devices = TPC_NumDevices();
    // ... causes this warning:
    // TpcSettingsToXml.obj : warning LNK4049: locally defined symbol _TPC_NumDevices@0 imported
    // I cannot see why...
    int num_devices = System::TheSystem()->NumDevices();

    for(int dev_no = 0; dev_no < num_devices; ++dev_no) {
        // get per device info
        TPC_DeviceInfo dev_info;
        CheckStatus(TPC_GetDeviceInfo(dev_no, &dev_info, sizeof(dev_info)));

        X::DOMElement* e_device = AddContainer(e_devices, L"Device");
        AddAttribute(e_device, L"Url", GetDeviceUrl(dev_no));

        // loop boards
        X::DOMElement* e_boards = AddContainer(e_device, L"Boards");
        for(unsigned board_addr = 0; board_addr < tpc_maxBoards; ++board_addr) {
            if (((dev_info.installedBoards >> board_addr) & 1) != 0) {
                // get per board info
                TPC_BoardInfo board_info;
                CheckStatus(TPC_GetBoardInfo(dev_no, board_addr, &board_info, sizeof(board_info)));

                X::DOMElement* e_board = AddContainer(e_boards, L"Board");
                AddAttribute(e_board, L"Address", IntToString(board_addr));

                // Cluster numbers
                int clusterNumbers[tpc_maxBoards];
                TPC_GetClusterNumbers(dev_no, clusterNumbers);
                AddLeaf(e_board, L"ClusterNumber", IntToString(clusterNumbers[board_addr]));

                // timebase
                X::DOMElement* e_timebase = AddContainer(e_board, L"Timebase");
                X::DOMElement* e_tb_params = AddContainer(e_timebase, L"Parameters");
                // loop timebase parameters
                for(int p = 0; p < numKnownParameters; p++) {
                    if (paramTable[p].timebase) {
                        double v;
                        TPC_ErrorCode err = TPC_GetParameter(dev_no, board_addr, 0, paramTable[p].par, &v);
                        if (err == tpc_errInvalidParameter) { 
                            // Be tolerant with older servers or smaller hardware
                            err = err;
                        }
                        else {
                            CheckStatus(err);
                            AddAttribute(AddLeaf(e_tb_params, L"Parameter", DoubleToString(v)), L"Name", paramTable[p].name);
                        }
                    }
                }

                // inputs
                X::DOMElement* e_inputs = AddContainer(e_board, L"Inputs");
                for(int input_no = 0; input_no < board_info.numberOfInputs; ++input_no) {
                    X::DOMElement* e_input = AddContainer(e_inputs, L"Input");
                    AddAttribute(e_input, L"Number", IntToString(input_no));

                    // input parameters
                    X::DOMElement* e_i_params = AddContainer(e_input, L"Parameters");
                    // loop input parameters
                    for(int p = 0; p < numKnownParameters; p++) {
                        if (!paramTable[p].timebase) {
                            double v;
                            TPC_ErrorCode err = TPC_GetParameter(dev_no, board_addr, input_no, paramTable[p].par, &v);
                            if (err == tpc_errInvalidParameter) { 
                                // Be tolerant with older servers or smaller hardware
                                err = err;
                            }
                            else {
                                CheckStatus(err);
                                AddAttribute(AddLeaf(e_i_params, L"Parameter", DoubleToString(v)), L"Name", paramTable[p].name);
                            }
                        }
                    }

                    // associations
                    TPC_AssociatedChannel assoc_buf[tpc_maxBoards * tpc_maxInputs];
                    int assoc_count = tpc_maxBoards * tpc_maxInputs;
                    CheckStatus(TPC_GetAssociatedChannels(dev_no, board_addr, input_no, assoc_buf, &assoc_count));
                    X::DOMElement* e_assocs = AddContainer(e_input, L"Associations");
                    for(int i = 0; i < assoc_count; ++i) {
                       AddAttribute(AddAttribute(AddEmptyElement(e_assocs, L"Association"),
                           L"Board", IntToString(assoc_buf[i].boardAddress)),
                           L"Input", IntToString(assoc_buf[i].inputNumber));
                    }
                }
            }
        }

        // attributes
        X::DOMElement* e_attributes = AddContainer(e_device, L"Attributes");
        CheckStatus(TPC_GetAllAttributes(dev_no, AttributeEnumeratorCallback, reinterpret_cast<void*>(e_attributes)));
    }

    return doc;
}
