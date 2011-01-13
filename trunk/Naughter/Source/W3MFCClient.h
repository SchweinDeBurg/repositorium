/*
Module : W3MFCClient.h
Purpose: Defines the interface for the CW3MFCClient classes

Copyright (c) 1999 - 2011 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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

#ifndef __W3MFCCLIENT_H__
#define __W3MFCCLIENT_H__

#ifndef W3MFC_EXT_CLASS
#define W3MFC_EXT_CLASS
#endif


/////////////////////////////// Includes //////////////////////////////////////

#include "W3MFCSocket.h"
#include "ThrdPool.h"
#include "W3MFC.h"
#include "W3MFCResponseHeader.h"
#ifndef _MSWSOCK_
#pragma message("To avoid this message, please put Mswsock.h in your pre compiled header (normally stdafx.h)")
#include <Mswsock.h>
#endif


/////////////////////////////// Classes ///////////////////////////////////////

//Class which represents the thread pool request
class W3MFC_EXT_CLASS CW3MFCThreadPoolRequest
{
public:
//Constructors / Destructors
  CW3MFCThreadPoolRequest() 
#ifdef W3MFC_SSL_SUPPORT
                            : m_pSSLContext(NULL)
#endif
  {
    memset(&m_ClientAddress, 0, sizeof(m_ClientAddress));
  }

//Member variables
  SOCKADDR_INET m_ClientAddress;
  CW3MFCSocket m_ClientSocket;
#ifdef W3MFC_SSL_SUPPORT
  CSSLContext* m_pSSLContext;
#endif
};

//forward declarations
class CW3MFCServer;

//Class which handles the HTTP client connection
class W3MFC_EXT_CLASS CW3MFCClient : public CThreadPoolClient
{
public:
//Constructors / Destructors
  CW3MFCClient();
  virtual ~CW3MFCClient();

//Methods
  virtual BOOL  Run(const CThreadPoolRequest& request);
  virtual int   ExitInstance();
  virtual DWORD ReturnErrorMessage(int nStatusCode);
  virtual DWORD ReturnRedirectMessage(const CString& sURL);
  virtual DWORD ReturnFileDeletedOkMessage(const CString& sFile);
  virtual DWORD ReturnUnauthorizedMessage(const CString& sRealm);
  virtual void  PostLog(int nHTTPStatusCode, DWORD dwBodyLength);
  virtual BOOL  GetKeySizeServerVariable(int& nKeySize);
  virtual BOOL  GetServerKeySizeServerVariable(int& nKeySize);
  virtual BOOL  GetCertSerialNumberServerVariable(long& nSerialNumber);
  virtual BOOL  GetCertIssuerServerVariable(char*& szIssuer);
  virtual BOOL  GetCertSubjectServerVariable(char*& szSubject);
  virtual BOOL  TransmitBuffer(CW3MFCSocket& socket, CW3MFCResponseHeader& responseHdr, BYTE* byData, DWORD dwSize, DWORD dwTimeout);
  virtual BOOL  TransmitBuffer(CW3MFCSocket& socket, CW3MFCResponseHeader& responseHdr, CStringA& sData, DWORD dwTimeout);
  virtual BOOL  TransmitFile(CW3MFCSocket& socket, CW3MFCResponseHeader& responseHdr, HANDLE hFile, DWORD dwSize, DWORD dwTimeout);
#ifndef W3MFC_NO_ISAPI_SUPPORT
  virtual BOOL  TransmitFile(CW3MFCSocket& socket, CW3MFCResponseHeader& responseHdr, HSE_TF_INFO* pInfo);
#endif
  virtual BOOL  MapURLToLocalFilename(CString& sURL, CString& sLocalFile, CString& sPathInfo, BOOL& bDirectory, CW3MFCDirectory*& pDirectory, DWORD& dwMatchingURL, DWORD& dwMatchingPath);
  virtual void  SetRequestToStop();
  virtual BOOL  PreHandleGetPostHead();

//Static Methods
  static int     HexToInt(TCHAR ch);
  static TCHAR   IntToHex(int Character);
  static CString URLDecode(const CString& sURL);
  static CString URLEncode(const CString& sURL);
  static CString UTF8ToCString(const CString& sURL);
  static BOOL    ParseDate(const CString& sField, SYSTEMTIME& time);
  static BOOL    ParseWeekDay(LPCTSTR pszToken, WORD& nWeekDay);
  static BOOL    ParseMonth(LPCTSTR pszToken, WORD& nMonth);

//Member variables
  CW3MFCServer* m_pServer;
  CW3MFCRequest m_Request;
  BOOL          m_bResponseKeepAlive;
  CW3MFCSocket  m_Socket;
#ifndef W3MFC_NO_ISAPI_SUPPORT
  DWORD         m_dwDataSentViaWriteClient;
  int           m_nHttpStatusCodeSent;
#endif
#ifdef W3MFC_SSL_SUPPORT
  CSSLContext*  m_pSSLContext;
  CSSL          m_SSL;
#endif

protected:
//Methods
  BOOL                     _TransmitFile(CW3MFCSocket& socket, CW3MFCResponseHeader& responseHdr, HANDLE hFile, DWORD dwSize);
  BOOL                     _TransmitBuffer(CW3MFCSocket& socket, CW3MFCResponseHeader& responseHdr, BYTE* byData, DWORD dwSize);
#ifndef W3MFC_NO_ISAPI_SUPPORT
  BOOL                     _TransmitFile(CW3MFCSocket& socket, CW3MFCResponseHeader& responseHdr, HSE_TF_INFO* pInfo);
#endif
  virtual BOOL             ParseRequest();
  virtual BOOL             ParseSimpleRequestLine(const CString& sLine);
  virtual BOOL             ParseAuthorizationLine(const CString& sLine);
  virtual BOOL             ParseRequestLine(const CString& sCurrentLine, BOOL bFirstLine, const CString& sField, const CString& sValue);
  virtual void             TransmitBuffer(BYTE* byData, DWORD dwSize, CW3MFCDirectory* pDirectory, BOOL bForceExpire);
  virtual BOOL             AllowThisConnection();
  virtual CW3MFCDirectory* GetVirtualDirectory(const CString& sDirectory);
  virtual void             HandleClient();
  virtual LPSTR            FindNextTerminatorFromRequest(LPSTR pszLine);

#ifdef W3MFC_SSL_SUPPORT
  virtual BOOL             DoSSLNegotiation();
  virtual BOOL             InitializeSSLConnection();
#endif

//Member variables
  CEvent               m_StopEvent;
  HMODULE              m_hMSWsock;
  LPFN_TRANSMITFILE    m_lpfnTransmitFile;
  LPFN_TRANSMITPACKETS m_lpfnTransmitPackets;

  DECLARE_DYNCREATE(CW3MFCClient)
};

#endif //__W3MFCCLIENT_H__
