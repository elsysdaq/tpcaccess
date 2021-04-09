// $Id: ResourceFile.cpp 2 2009-01-13 08:45:52Z roman $

#include "ResourceFile.h"

ResourceFile::ResourceFile(HINSTANCE hinstance, LPCTSTR lpName, LPCTSTR lpType)
{
   hg=NULL; p=0; s=0;

   HMODULE module = (HMODULE)hinstance;
   HRSRC r = FindResource(module, lpName, lpType);
   if (r == NULL) { return; } // not found
   hg=LoadResource(module, r);
   if (hg == NULL) { return; } // not found
   p=LockResource(hg);
   s=SizeofResource(module, r);
}

ResourceFile::~ResourceFile()
{
   if (hg != NULL) { FreeResource(hg); }
}
