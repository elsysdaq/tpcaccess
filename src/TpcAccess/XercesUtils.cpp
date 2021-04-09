// $Id: XercesUtils.cpp 2 2009-01-13 08:45:52Z roman $

#include "XercesUtils.h"

#include "xerces_include_start.h"
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include "xerces_include_end.h"

#include <memory>
#include <sstream>
#include <vector>
 #include <iostream>

 using namespace std;
 using namespace xercesc;

unsigned ucs4_length(const uint16_t *utf16)
{
  unsigned length = 0;

  while (*utf16) {
    if (utf16[0] < 0xd800 || utf16[0] > 0xdfff)
      ++length;
    else if (utf16[0] >= 0xd800 && utf16[0] <= 0xdbff &&
	     utf16[1] >= 0xdc00 && utf16[1] <= 0xdfff) {
      ++length;
      ++utf16;
    }

    ++utf16;
  }

  return length;
}


unsigned utf16_length(const uint32_t *ucs4)
{
  unsigned size = 0;

  while (*ucs4) {
    ++size;
    if (*ucs4 >= 0x00010000L && *ucs4 <= 0x0010ffffL)
      ++size;

    ++ucs4;
  }

  return size;
}


unsigned utf16_decodechar(const uint16_t *utf16, uint32_t *ucs4)
{
  uint16_t const *start = utf16;

  for(;;) {
    if (utf16[0] < 0xd800 || utf16[0] > 0xdfff) {
      *ucs4 = utf16[0];
      return utf16 - start + 1;
    }
    else if (utf16[0] >= 0xd800 && utf16[0] <= 0xdbff &&
	     utf16[1] >= 0xdc00 && utf16[1] <= 0xdfff) {
      *ucs4 = (((utf16[0] & 0x03ffL) << 10) |
	       ((utf16[1] & 0x03ffL) <<  0)) + 0x00010000L;
      return utf16 - start + 2;
    }

    ++utf16;
  }
}


unsigned utf16_encodechar(uint16_t *utf16, uint32_t ucs4)
{
  // Wenn ungltig
  if (ucs4 >= 0x00110000L)
  {
    ucs4 = 0;
    return 1;
  }

  if (ucs4 < 0x00010000L) 
  {
    utf16[0] = (uint16_t)ucs4;
    return 1;
  }
  else 
  {
    ucs4 -= 0x00010000L;
    utf16[0] = (uint16_t)(((ucs4 >> 10) & 0x3ff) | 0xd800);
    utf16[1] = (uint16_t)(((ucs4 >>  0) & 0x3ff) | 0xdc00);
    return 2;
  }
}


void utf16_decode(const uint16_t *utf16, uint32_t *ucs4)
{
  do
    utf16 += utf16_decodechar(utf16, ucs4);
  while (*ucs4++);
}


void utf16_encode(uint16_t *utf16, const uint32_t *ucs4)
{
  do
    utf16 += utf16_encodechar(utf16, *ucs4);
  while (*ucs4++);
}


std::vector<uint32_t> to_ucs4(const uint16_t *utf16)
{
  assert(utf16 != 0);
  std::vector<uint32_t> utf32;
  utf32.resize(ucs4_length(utf16) + 1);
  utf16_decode(utf16, &utf32[0]);
  return utf32;
}


std::vector<uint16_t> to_utf16(const uint32_t *ucs4)
{
  assert(ucs4 != 0);
  std::vector<uint16_t> utf16;
  utf16.resize(utf16_length(ucs4) + 1);  
  utf16_encode(&utf16[0], ucs4);
  return utf16;
}


//-----------------------------------------------------------------------------


#ifdef _UNICODE
#else
nstring ToNstring(const XMLCh* const s)
{
   char* n = X::XMLString::transcode(s);
   std::string str(n);
   X::XMLString::release(&n);
   return str;
}
#endif


//-----------------------------------------------------------------------------

long XercesLibraryInit::refcount = 0;

void XercesLibraryInit::Init()
{
   if (refcount++ == 0) {
      // Initialize the XML4C2 system
      try {
         X::XMLPlatformUtils::Initialize();
      }
      catch(const X::XMLException&)
      {
         abort();
      }
   }
}

void XercesLibraryInit::Done()
{
   if (--refcount == 0) {
      // And call the termination method
      X::XMLPlatformUtils::Terminate();
   }
}

//-----------------------------------------------------------------------------

static void AppendExceptionMessage(nstring& messages, const X::SAXParseException& e, const TCHAR* type)
{
   std::wostringstream message;
   message << type << L" at File \"" << TO_WCHAR(e.getSystemId())
           << L"\" line " << e.getLineNumber()
           << L" column " << e.getColumnNumber()
           << L"\n  Message: " << TO_WCHAR(e.getMessage())
           << L"\n";
   std::wstring s(message.str());
   messages += ToNstring(TO_XMLCH(s.c_str()));
}

void DOMErrorCollector::warning(const X::SAXParseException& toCatch)
{
   ++warnings;
   AppendExceptionMessage(messages,toCatch,_TEXT("Warning"));
}

void DOMErrorCollector::error(const X::SAXParseException& toCatch)
{
   ++errors;
   AppendExceptionMessage(messages,toCatch,_TEXT("Error"));
}

void DOMErrorCollector::fatalError(const X::SAXParseException& toCatch)
{
   ++errors;
   AppendExceptionMessage(messages,toCatch,_TEXT("Fatal Error"));
}

void DOMErrorCollector::resetErrors()
{
   warnings=0;
   errors=0;
   messages.erase();
}

//-----------------------------------------------------------------------------

void BasicDOMInOut::SetupParser(X::XercesDOMParser& parser)
{
   parser.setValidationScheme(X::XercesDOMParser::Val_Never);
   parser.setDoNamespaces(true);
   parser.setExitOnFirstFatalError(true);
}


DOMDocumentHandle BasicDOMInOut::Parse(const TCHAR* filename, X::InputSource* input)
{
   DOMErrorCollector error_collector;
   X::XercesDOMParser parser;
   
    parser.setErrorHandler(&error_collector);
   SetupParser(parser);

   // parse file
   unsigned errors = 0;
   last_error.erase();
   try {
      if (input != 0) {
         parser.parse(*input);
      } else {
         parser.parse(filename);
      }
      errors=parser.getErrorCount();
      last_error=error_collector.GetMessages();
   }
   catch (const X::OutOfMemoryException&)
   {
      ++errors;
      last_error += _TEXT("Out of memory\n");
   }
   catch (const X::XMLException& e)
   {
      ++errors;
      last_error += _TEXT("XML parse error: ")+ToNstring(e.getMessage())+_TEXT("\n");
   }
   catch (const X::DOMException&)
   {
      ++errors;
      last_error += _TEXT("DOM parse error\n");
   }

   return errors ? DOMDocumentHandle() : DOMDocumentHandle(parser.adoptDocument());
}

DOMDocumentHandle BasicDOMInOut::ParseFile(const TCHAR* filename)
{
   return Parse(filename, 0);
}

DOMDocumentHandle BasicDOMInOut::ParseString(const char *xml)
{
	X::MemBufInputSource input(reinterpret_cast<const XMLByte*>(xml), // srcDocBytes,
                              unsigned(strlen(xml)),                 // byteCount
                              TO_XMLCH(L"built in xml data"));       // bufId
   
	return Parse(0, &input);
}


bool BasicDOMInOut::WriteFile(const X::DOMDocument* doc, const TCHAR* filename){
//   DOMErrorCollector error_collector;
   
   unsigned errors = 0;
   
   last_error.erase();
   XMLCh tempStr[100];
   XMLString::transcode("LS", tempStr, 99);
  
   try{   
	   DOMImplementation *impl				= DOMImplementationRegistry::getDOMImplementation(tempStr);
	   DOMLSSerializer   *theSerializer		= ((DOMImplementationLS*)impl)->createLSSerializer();
	   DOMLSOutput       *theOutputDesc		= ((DOMImplementationLS*)impl)->createLSOutput();

		if (theSerializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
             theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
			
		// convert DOM to XML 
		
		//XMLFormatTarget *myFormTarget = new StdOutFormatTarget();
		XMLFormatTarget *myFormTarget = new LocalFileFormatTarget(filename);
		
		theOutputDesc->setByteStream(myFormTarget);
		
		// do the serialization through DOMLSSerializer::write();
        theSerializer->write(doc, theOutputDesc);
        
		theOutputDesc->release();
        theSerializer->release();

		 delete myFormTarget;
   }
   catch (const OutOfMemoryException&){
        ++errors;
		last_error += _TEXT("Out of memory\n");
   }
   catch (XMLException& e){
           ++errors;
		last_error += _TEXT("XML creation error: ")+ToNstring(e.getMessage())+_TEXT("\n");
   }
   return errors == 0;
}

void ValidatingDOMInOut::SetupParser(X::XercesDOMParser& parser)
{
   BasicDOMInOut::SetupParser(parser);
   parser.setValidationScheme(X::XercesDOMParser::Val_Always);
   parser.setDoSchema(true);
   parser.setValidationSchemaFullChecking(true);
   parser.setValidationConstraintFatal(true);
   parser.useCachedGrammarInParse(true);
   LoadSchema(parser);
}
