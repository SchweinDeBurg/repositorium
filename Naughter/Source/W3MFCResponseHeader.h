/*
Module : W3MFCResponseHeader.h
Purpose: Defines the interface for a class to simplify sending Http response headers

Copyright (c) 1999 - 2011 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////// Defines ///////////////////////////////////////

#pragma once

#ifndef __W3MFCRESPONSEHEADER_H__
#define __W3MFCRESPONSEHEADER_H__

#ifndef W3MFC_EXT_CLASS
#define W3MFC_EXT_CLASS
#endif


/////////////////////////////// Includes //////////////////////////////////////

#include "W3MFCSocket.h"


/////////////////////////////// Classes ///////////////////////////////////////

//Class which is used to simplify forming and returning a header to the client
class W3MFC_EXT_CLASS CW3MFCResponseHeader
{
public:
//Constructors / Destructors
  CW3MFCResponseHeader();

//Methods
  void     AddStatusCode(int nStatusCode);
  void     AddStatus(LPCSTR pszStatusString);
  void     AddContentLength(int nSize);
  void     AddContentType(LPCTSTR pszMediaType);
  void     AddKeepAlive();
  void     AddDate(const SYSTEMTIME& st);
  void     AddLastModified(const SYSTEMTIME& st);
  void     AddWWWAuthenticateBasic(LPCTSTR pszRealm);
  void     AddWWWAuthenticateNTLM(LPCTSTR pszMessage);
  void     AddExpires(const SYSTEMTIME& st);
  void     AddLocation(LPCTSTR pszLocation);
  void     AddServer(LPCTSTR pszServer);
  void     AddW3MfcAllowFields(BOOL bAllowDeleteRequest);
  void     AddExtraHeaders(LPCSTR pszHeaders);
  void     SetAddEntitySeparator(BOOL bSeparator);

  CStringA DateToStr(const SYSTEMTIME& st);
#ifdef W3MFC_SSL_SUPPORT
  BOOL     Send(CW3MFCSocket& socket, DWORD dwTimeout, CSSL& ssl);
#endif
  BOOL     Send(CW3MFCSocket& socket, DWORD dwTimeout);
  CStringA GetData();

protected:
  CStringA m_sHeader;
  BOOL     m_bEntitySeparator;
};

#endif //__W3MFCRESPONSEHEADER_H__
