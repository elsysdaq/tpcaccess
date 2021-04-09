// $Id: ResourceFile.h 2 2009-01-13 08:45:52Z roman $

#ifndef __RESOURCEFILE_H__
#define __RESOURCEFILE_H__

#include <windows.h>

// Reads a file from the resources stored in an executable file
class ResourceFile {
   public:
   ResourceFile(HINSTANCE hinstance, LPCTSTR lpName, LPCTSTR lpType);
   ~ResourceFile();

   const void* data() { return p; } // returns 0 if the file does not exist
   DWORD       size() { return s; } // returns 0 if the file does not exist

   private:
   HGLOBAL     hg;
   const void* p;
   DWORD       s;
};

#endif /*__RESOURCEFILE_H__*/
