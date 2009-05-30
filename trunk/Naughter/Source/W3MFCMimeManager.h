/*
Module : W3MFCMimeManager.h
Purpose: Defines the interface for the Mime Manager classes

Copyright (c) 1999 - 2009 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////// Macros / Defines //////////////////////////////

#pragma once

#ifndef __W3MFCMIMEMANAGER_H__
#define __W3MFCMIMEMANAGER_H__

#ifndef W3MFC_EXT_CLASS
#define W3MFC_EXT_CLASS
#endif


/////////////////////////////// Includes //////////////////////////////////////

#include "W3MFCRequest.h"


/////////////////////////////// Classes ///////////////////////////////////////

//Base class which you derive from to implement a Mime Lookup manager i.e.
//a class which encapsulates retrieval of MIME types given a filename 
//extension
class W3MFC_EXT_CLASS CW3MFCMimeManager
{
public:
  virtual CString GetMimeType(const CW3MFCRequest& request) = 0;
};

//Class which does mime lookups based on the Win32 registry mime settings
class W3MFC_EXT_CLASS CW3MFCRegistryMimeManager : public CW3MFCMimeManager
{
public:
//Methods
  virtual CString GetMimeType(const CW3MFCRequest& request);

protected:
  CMapStringToString m_sMimeMap;
  CCriticalSection m_CS;
};

//Class which does mime lookups based on a section in an ini file e.g. "W3MFC.ini"
class W3MFC_EXT_CLASS CW3MFCIniMimeManager : public CW3MFCMimeManager
{
public:
//Methods
  int Initialize(const CString& sIniPath, const CString& sSection);
  virtual CString GetMimeType(const CW3MFCRequest& request);

protected:
//Methods
  void FullArraysFromMultiSZString(LPTSTR pszString);

//Member variables
  CMapStringToString m_sMimeMap;
  CCriticalSection   m_CS;
};

#endif //__W3MFCMIMEMANAGER_H__
