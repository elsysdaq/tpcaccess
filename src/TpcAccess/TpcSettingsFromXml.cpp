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
// $Id: TpcSettingsFromXml.cpp 13 2012-08-07 08:30:26Z roman $

#include "TpcSettingsFromXml.h"

#include <vector>

#include "StringConversions.h"

#include "xerces_include_start.h"
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include "xerces_include_end.h"

#include "TpcAccess.h"

#include "TpcSettings.h"

//-----------------------------------------------------------------------------


static X::DOMElement* GetElement(X::DOMElement* e, const XMLCh *name)
{
   X::DOMNodeList* nl = e->getElementsByTagName(name);
   if (nl == 0 || nl->getLength() != 1) { return 0; }
   return dynamic_cast<X::DOMElement*>(nl->item(0));
}

static nstring GetElementText(X::DOMElement* e)
{
   return ToNstring(e->getFirstChild()->getNodeValue());
}

static nstring GetAttribute(X::DOMElement* e, const XMLCh *name)
{
   return ToNstring(e->getAttribute(name));
}


#ifdef _LINUX
static X::DOMElement* GetElement(X::DOMElement* e, const wchar_t *name)
{
  return GetElement(e, TO_XMLCH(name));
}

static nstring GetAttribute(X::DOMElement* e, const wchar_t *name)
{
  return GetAttribute(e, TO_XMLCH(name));
}
#endif


namespace {
   class ElemIterator {
      public:
      typedef std::basic_string<wchar_t> xstring;
      void Init(X::DOMElement* parent, const wchar_t *name_filter) {
         if (name_filter != 0) { name=name_filter; }
         required_namespace=TO_WCHAR(parent->getNamespaceURI());
         cur=0;
         next=parent->getFirstChild();
      }
#ifdef _LINUX
      ElemIterator(X::DOMElement* parent, const wchar_t *name_filter) {
         if (name_filter == 0)
            Init(parent, 0);
         else
            Init(parent, name_filter);
      }
      bool MoveNext() {

         while (next != 0 &&
                (next->getNodeType() != X::DOMNode::ELEMENT_NODE ||
                 xstring(TO_WCHAR(next->getNamespaceURI())) != required_namespace ||
                 (!name.empty() && name != xstring(TO_WCHAR(next->getLocalName()))))) {
            next=next->getNextSibling();
         }
         if (next != 0) {
            cur=dynamic_cast<X::DOMElement*>(next);
            next=next->getNextSibling();
            return true;
         } else {
            cur=0;
            return false;
         }
      }
#else
      ElemIterator(X::DOMElement* parent, const XMLCh *name_filter) {
         Init(parent, name_filter);
      }
      bool MoveNext() {

         while (next != 0 &&
                (next->getNodeType() != X::DOMNode::ELEMENT_NODE ||
                 next->getNamespaceURI() != required_namespace ||
                 (!name.empty() && name != next->getLocalName()))) {
            next=next->getNextSibling();
         }
         if (next != 0) {
            cur=dynamic_cast<X::DOMElement*>(next);
            next=next->getNextSibling();
            return true;
         } else {
            cur=0;
            return false;
         }
      }
#endif
      X::DOMElement* operator->() const { return  cur; }
      X::DOMElement& operator* () const { return *cur; }
      operator X::DOMElement*  () const { return  cur; }
      private:
      xstring         name;
      xstring         required_namespace;
      X::DOMElement*  cur;
      X::DOMNode*     next;
   };
};

//-----------------------------------------------------------------------------

static void CheckStatus(TPC_ErrorCode status)
{
    if (status != tpc_noError) {
        throw status;
    }
}

//-----------------------------------------------------------------------------

void ProcessXmlTpcSettings(const DOMDocumentHandle& d)
{
    CheckStatus(TPC_ResetConfiguration());

    X::DOMElement* e_settings = d->getDocumentElement();
    //Version not used yet
    //nstring settings_version(GetAttribute(e_settings,L"Version"));

    CheckStatus(TPC_BeginSet());
    try {
        int num_devices = TPC_NumDevices();
        int dev_ix = 0;

        // devices
        ElemIterator device(GetElement(e_settings, L"Devices"),L"Device");
        while (device.MoveNext()) {
            ElemIterator syncClock(device, L"SyncClockOut");
			ElemIterator board(GetElement(device, L"Boards"),L"Board");

            if (dev_ix >= num_devices) break;
     
            // boards
            while (board.MoveNext()) {

                int boardAddr = StringToInt(GetAttribute(board,L"Address"));
                int cluster = StringToInt(GetElementText(GetElement(board, L"ClusterNumber")));
                int masterBoardAddress = -1;
                TPC_ClusterNumberToBoardAddress(dev_ix, cluster, &masterBoardAddress);
          
                // timebase parameters
                ElemIterator timebase_param(GetElement(GetElement(board, L"Timebase"), L"Parameters"), L"Parameter");
                while (timebase_param.MoveNext()) {
                    if (masterBoardAddress >= 0)
                    {
                        nstring name(GetAttribute(timebase_param,L"Name"));
                        double value(StringToDouble(GetElementText(timebase_param)));

                        int ix = SearchParamName(name);
                        if (ix < 0) {
                            // Be tolerant with setting files from newer versions.
                        }
                        else {
                            TPC_ErrorCode err = TPC_SetParameter(dev_ix, masterBoardAddress, 0, paramTable[ix].par, value);
                            if (err == tpc_errInvalidParameter) {
                                // Be tolerant with older hardware.
                            }
                            else {
                                CheckStatus(err);
                            }
                        }
                    }
                }
          
                // inputs
                ElemIterator input(GetElement(board, L"Inputs"), L"Input");
                while (input.MoveNext()) {
                    nstring number(GetAttribute(input,L"Number"));
                    int inputNr = StringToInt(number);
            
                    // input parameters
                    ElemIterator input_param(GetElement(input, L"Parameters") ,L"Parameter");
                    while (input_param.MoveNext()) {
                        nstring name(GetAttribute(input_param,L"Name"));
                        double  value(StringToDouble(GetElementText(input_param)));
                        
                        int ix = SearchParamName(name);
                        if (ix < 0) {
                            // Be tolerant with setting files from newer versions.
                        }
                        else {
                            TPC_ErrorCode err = TPC_SetParameter(dev_ix, boardAddr, inputNr, paramTable[ix].par, value);
                            if (err == tpc_errInvalidParameter) {
                                // Be tolerant with older hardware.
                            }
                            else {
                                CheckStatus(err);
                            }
                        }
                    }
					//ElemIterator RealDiff(GetElement(board, L"HasRealDifferentialInputs"), L"HasRealDifferentialInputs");
                    // associations
                    TPC_AssociatedChannel assoc_buf[tpc_maxBoards * tpc_maxInputs];
                    int assoc_count = 0;
                    ElemIterator association(GetElement(input, L"Associations") ,L"Association");
                    while (association.MoveNext()) {
                        int board(StringToInt(GetAttribute(association,L"Board")));
                        int input(StringToInt(GetAttribute(association,L"Input")));

                        assoc_buf[assoc_count].boardAddress = board;
                        assoc_buf[assoc_count].inputNumber = input;
                        assoc_count++;
                    }
                    CheckStatus(TPC_SetAssociatedChannels(dev_ix, boardAddr, inputNr, assoc_buf, assoc_count));
                }
            }
  
            // attributes
            ElemIterator attribute(GetElement(device, L"Attributes"),L"Attribute");
            while (attribute.MoveNext()) {
                int     board_address(StringToInt(GetAttribute(attribute,L"BoardAddress")));
                int     input_number (StringToInt(GetAttribute(attribute,L"InputNumber")));
                nstring name         (GetAttribute(attribute,L"Name"));
                nstring value        (GetElementText(attribute));
                CheckStatus(TPC_SetAttribute(dev_ix, board_address, input_number, name.c_str(), value.c_str()));
            }

            dev_ix++;
        }

        CheckStatus(TPC_EndSet());
    }
    catch(...) {
        TPC_EndSet();
    }
}
