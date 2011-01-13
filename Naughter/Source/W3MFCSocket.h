/*
Module : W3MFCSocket.h
Purpose: Defines the interface for a simple MFC socket wrapper class
Created: PJN / 22-04-1999

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

#ifndef __W3MFCSOCKET_H__
#define __W3MFCSOCKET_H__

#ifndef W3MFC_EXT_CLASS
#define W3MFC_EXT_CLASS
#endif


/////////////////////////////// Includes //////////////////////////////////////

#include "SocMFC.h"
#include "W3MFCRequest.h"
#include "W3MFC.h"
#ifdef W3MFC_SSL_SUPPORT
#include "OpenSSLMfc.h"
#endif
#include "WTimer.h" //If the code fails to compile because of this missing include then you need to download 
                    //the CWaitableTimer class from my web site and copy the relavent files to your project directory


/////////////////////////////// Classes ///////////////////////////////////////

//forward declaration
class CW3MFCClient;

class W3MFC_EXT_CLASS CW3MFCSocket : public CWSocket
{
public:
//methods
  static BOOL SplitRequestLine(LPSTR pszLine, CString& sField, CString& sValue);
  static BOOL SplitRequestLine(const CString& sLine, CString& sField, CString& sValue);
  BOOL ReadRequest(CW3MFCRequest& request, DWORD dwTimeout, int nGrowBy, CW3MFCClient& client);
  BOOL ReadRequest(CW3MFCRequest& request, DWORD dwTimeout, int nGrowBy, CW3MFCClient& client, CWaitableTimer& timer, HANDLE hStopEvent, HANDLE hDataEvent);
  void SendWithRetry(const void* pBuffer, int nBuf, DWORD dwTimeout);
#ifdef W3MFC_SSL_SUPPORT
  BOOL ReadRequest(CW3MFCRequest& request, DWORD dwTimeout, int nGrowBy, CSSL& ssl, CW3MFCClient& client);
  BOOL ReadRequest(CW3MFCRequest& request, DWORD dwTimeout, int nGrowBy, CSSL& ssl, CW3MFCClient& client, CWaitableTimer& timer, HANDLE hStopEvent, HANDLE hDataEvent);
  void SendWithRetry(const void* pBuffer, int nBuf, DWORD dwTimeout, CSSL& ssl);
#endif

protected:
  CEvent m_WSABlockEvent;  //Used to implemented SocketSendWithRetry
};

#endif //__W3MFCSOCKET_H__
