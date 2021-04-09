// $Id: XercesUtils.h 2 2009-01-13 08:45:52Z roman $

#ifndef __XERCESUTILS_H__
#define __XERCESUTILS_H__

#include <xercesc/util/XercesDefs.hpp>

#ifdef _MSC_VER
# include <tchar.h>
#else
# if defined(UNICODE) || defined(_UNICODE)
#   define _TEXT(x) L ## x
    typedef wchar_t TCHAR;
# else
#  define _TEXT(x) x
   typedef char TCHAR;
# endif
#endif

#include <vector>
#include <string>

#include "intTypes.h"

std::vector<uint32_t> to_ucs4(const uint16_t *utf16);
std::vector<uint16_t> to_utf16(const uint32_t *ucs4);

#ifdef _LINUX
#define TO_XMLCH(wc) (XMLCh *)(&to_utf16((const uint32_t *)wc)[0])
#define TO_WCHAR(xc) (wchar_t *)(&to_ucs4((const uint16_t *)xc)[0])
#else
#define TO_XMLCH(wc) wc
#define TO_WCHAR(xc) xc
#endif


#if defined(UNICODE) || defined(_UNICODE)
typedef std::wstring nstring;
inline nstring ToNstring(const XMLCh* const s) { return std::wstring(TO_WCHAR(s)); }
#else
typedef std::string nstring;
nstring ToNstring(const XMLCh* const s);
#endif

//-----------------------------------------------------------------------------

#include "xerces_include_start.h"
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include "xerces_include_end.h"

// Use X as a namespace for all Xerces namespaces.
// It is not possible to use a plain XERCES_CPP_NAMESPACE_USE
// because the names in those namespaces colide with the declarations
// in <windows.h>.
namespace X {
   XERCES_CPP_NAMESPACE_USE
};

class XercesLibraryInit {
   public:
   XercesLibraryInit()  { Init(); }
   ~XercesLibraryInit() { Done(); }
   private:
   static long refcount;
   static void Init();
   static void Done();
};

// initialize the Xerces library at startup and free it at termination
static XercesLibraryInit static_xerces_library_init;

//-----------------------------------------------------------------------------

// An ErrorHandler that collects all errors in a single string
class DOMErrorCollector : public X::ErrorHandler
{
   public:
   DOMErrorCollector() : warnings(0), errors(0) {}
   virtual ~DOMErrorCollector() {}

   unsigned NumWarnings() { return warnings; }
   unsigned NumErrors()   { return errors;   }
   nstring  GetMessages() { return messages; }

   protected:
   // -----------------------------------------------------------------------
   //  Implementation of the error handler interface
   // -----------------------------------------------------------------------
   void warning   (const X::SAXParseException& toCatch);
   void error     (const X::SAXParseException& toCatch);
   void fatalError(const X::SAXParseException& toCatch);
   void resetErrors();

   private:
   unsigned warnings;
   unsigned errors;
   nstring  messages;
};

//-----------------------------------------------------------------------------

// a simple reference counting handle for a DOMDocument
class DOMDocumentHandle {
   public:
   DOMDocumentHandle() : p(0), count(0) {}
   ~DOMDocumentHandle() { Free(); }
   DOMDocumentHandle(const DOMDocumentHandle& rhs) { // transfers ownership
      p=rhs.p;
      count=rhs.count;
      if (count != 0) { (*count)++; }
   }
   DOMDocumentHandle& operator=(const DOMDocumentHandle& rhs) { // transfers ownership
      if (this != &rhs) {
         Free();
         p=rhs.p;
         count=rhs.count;
         if (count != 0) { (*count)++; }
      }
      return *this; //sam
   }
   inline X::DOMDocument* operator->() const { return  p; }
   inline X::DOMDocument& operator* () const { return *p; }
   operator X::DOMDocument*         () const { return  p; }

   private:
   friend class BasicDOMInOut;
   explicit DOMDocumentHandle(X::DOMDocument* doc) : p(doc), count(new unsigned) { *count=1; }
   void Free() {
      if (count != 0) {
         if (--(*count) == 0) {
            delete count;
            // according to the doc, the DOMDucument should be releases by
            // calling p->DOMDocument::release(). However release() is not
            // implemented and the examples simply call delete. So do we.
            delete p;
         }
      }
   }
   X::DOMDocument* p;
   unsigned*       count;
};

//-----------------------------------------------------------------------------

// Utility class to read/write XML files
class BasicDOMInOut {
   public:
   BasicDOMInOut() {}
   virtual ~BasicDOMInOut() {}

   virtual DOMDocumentHandle ParseFile   (const TCHAR* filename);
   virtual DOMDocumentHandle ParseString (const char* xml);
   virtual bool              WriteFile   (const X::DOMDocument* doc, const TCHAR* filename);
   virtual nstring           GetLastError() const { return last_error; }

   protected:
   virtual void SetupParser(X::XercesDOMParser& parser);
   virtual DOMDocumentHandle Parse(const TCHAR* filename, X::InputSource* input);
   nstring last_error;
};

// Utility class to read/write XML files with validation
class ValidatingDOMInOut : public BasicDOMInOut{
   protected:
   virtual void LoadSchema(X::XercesDOMParser& parser) = 0; // must call parser.loadGrammar to set schema
   virtual void SetupParser(X::XercesDOMParser& parser);    // overrides BasicDOMInOut::SetupParser
};

#endif /*__XERCESUTILS_H__*/
