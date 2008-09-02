/*
Module : PJNPOP3.H
Purpose: Defines the interface for a MFC class encapsulation of the POP3 protocol
Created: PJN / 04-05-1998

Copyright (c) 1998 - 2008 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////// Includes / Defines ////////////////////////////

#pragma once

#ifndef __PJNPOP3_H__
#define __PJNPOP3_H__

#ifndef __AFXTEMPL_H__
#pragma message("To avoid this message, put afxtempl.h in your pre compiled header (usually stdafx.h)")
#include <afxtempl.h>
#endif

#include "SocMFC.h" //If you get a compilation error about this missing header file, then you need to download my CWSocket wrapper classes from http://www.naughter.com/w3mfc.html

#ifndef CPJNPOP3_NOSSL
#include "OpenSSLMfc.h" //If you get a compilation error about this missing header file, then you need to download my CSSLSocket wrapper classes from http://www.naughter.com/w3mfc.html
#endif

#ifndef PJNPOP3_EXT_CLASS
#define PJNPOP3_EXT_CLASS
#endif

#if defined(__INTEL_COMPILER)
// remark #383: value copied to temporary, reference to temporary used
#pragma warning(disable: 383)
// remark #271: trailing comma is nonstandard
#pragma warning(disable: 271)
#endif	// __INTEL_COMPILER

/////////////////////////////// Classes ///////////////////////////////////////

class PJNPOP3_EXT_CLASS CPJNPOP3Exception : public CException
{
public:
//Constructors / Destructors
  CPJNPOP3Exception(HRESULT hr, const CString& sLastResponse = _T("")); 
	CPJNPOP3Exception(DWORD dwError = 0, DWORD dwFacility = FACILITY_WIN32, const CString& sLastResponse = _T(""));

//Methods
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL GetErrorMessage(__out_ecount_z(nMaxError) LPTSTR lpstrError, __in UINT nMaxError, __out_opt PUINT pnHelpContext = NULL);
	CString GetErrorMessage();

//Data members
	HRESULT m_hr;
  CString m_sLastResponse;

protected:
	DECLARE_DYNAMIC(CPJNPOP3Exception)
};

////// forward declaration
class CPJNPOP3Connection;

//Encapsulation of a POP3 message
class PJNPOP3_EXT_CLASS CPJNPOP3Message
{
public:
//Constructors / Destructors
  CPJNPOP3Message();
  CPJNPOP3Message(const CPJNPOP3Message& message);

//Methods
  CPJNPOP3Message& operator=(const CPJNPOP3Message& message);
  CStringA GetHeader() const;
  CStringA GetHeaderItem(const CStringA& sName, int nItem = 0) const;
  CStringA GetBody() const;
	CStringA GetSubject() const { return GetHeaderItem("Subject"); }
	CStringA GetFrom() const		{ return GetHeaderItem("From"); }
	CStringA GetDate() const		{ return GetHeaderItem("Date"); }
  CStringA GetTo() const	    { return GetHeaderItem("To"); }
	CStringA GetCC() const	    { return GetHeaderItem("CC"); }
	CStringA GetReplyTo() const;

//Helper Methods
  static CStringA GetEmailAddress(const CStringA& sNameAndAddress);
  static CStringA GetEmailFriendlyName(const CStringA& sNameAndAddress);

//Member variables
  CStringA m_sMessage;

  friend class CPJNPOP3Connection;
};
  
//The main class which encapsulates the POP3 connection
class PJNPOP3_EXT_CLASS CPJNPOP3Connection
{
public:
//typedefs
  enum AuthenticationMethod
  {
    AUTH_PLAIN = 0, //Username and password are send in the clear to the server (USER / PASS commands)
    AUTH_APOP = 1   //Use APOP support which is a challenge response authentication method (APOP command)
  };

  enum ProxyType
  {
    ptNone = 0,
    ptSocks4 = 1,
    ptSocks5 = 2,
    ptHTTP = 3
  };

//Constructors / Destructors
  CPJNPOP3Connection();
  virtual ~CPJNPOP3Connection();

//Methods
#ifndef CPJNPOP3_NOSSL
  void    Connect(LPCTSTR pszHostName, LPCTSTR pszUsername, LPCTSTR pszPassword, int nPort = 110, AuthenticationMethod am = AUTH_PLAIN, BOOL bSSL = FALSE);
#else
  void    Connect(LPCTSTR pszHostName, LPCTSTR pszUsername, LPCTSTR pszPassword, int nPort = 110, AuthenticationMethod am = AUTH_PLAIN);
#endif
  void    Disconnect(BOOL bGracefully = TRUE);
  BOOL    IsConnected() const	{ return m_bConnected; };
  CString GetLastCommandResponse() const { return m_sLastCommandResponse; };
  DWORD   GetTimeout() const { return m_dwTimeout; };
  void    SetTimeout(DWORD dwTimeout) { m_dwTimeout = dwTimeout; };
  void    Statistics(int& nNumberOfMails, int& nTotalMailSize);
  void    Delete(int nMsg);
  DWORD   GetMessageSize(int nMsg);
  CString GetMessageID(int nMsg);
  INT_PTR FindMessageID(const CString& sID); 
  void    Retrieve(int nMsg, CPJNPOP3Message& message);
  void    GetMessageHeader(int nMsg, CPJNPOP3Message& message);
  void    Reset();
  void    UIDL();
  void    Noop();
  void    List();

//Proxy methods
  void      SetProxyType(ProxyType proxyType) { m_ProxyType = proxyType; };
  ProxyType GetProxyType() const { return m_ProxyType; };
  void      SetProxyServer(const CString& sServer) { m_sProxyServer = sServer; };
  CString   GetProxyServer() const { return m_sProxyServer; };
  void      SetProxyPort(int nPort) { m_nProxyPort = nPort; };
  int       GetProxyPort() { return m_nProxyPort; };
  void      SetBoundAddress(const CString& sLocalBoundAddress) { m_sLocalBoundAddress = sLocalBoundAddress; };
  CString   SetBoundAddress() const { return m_sLocalBoundAddress; };
  void      SetProxyUserName(const CString& sUserName) { m_sProxyUserName = sUserName; };
  CString   GetProxyUserName() const { return m_sProxyUserName; };
  void      SetProxyPassword(const CString& sPassword) { m_sProxyPassword = sPassword; };
  CString   GetProxyPassword() const { return m_sProxyPassword; };
  CString   GetHTTPProxyUserAgent() const { return m_sUserAgent; };
  void      SetHTTPProxyUserAgent(const CString& sUserAgent) { m_sUserAgent = sUserAgent; };

//Static methods
  static void ThrowPJNPOP3Exception(DWORD dwError = 0, DWORD Facility = FACILITY_WIN32, const CString& sLastResponse = _T(""));
  static void ThrowPJNPOP3Exception(HRESULT hr, const CString& sLastResponse = _T(""));

protected:
  virtual void ReadStatResponse(int& nNumberOfMails, int& nTotalMailSize);
	virtual BOOL ReadCommandResponse();
	virtual BOOL ReadCommandResponse(CStringA& sResponse);
  virtual void ReadListResponse(int nNumberOfMails);
  virtual void ReadUIDLResponse(int nNumberOfMails);
  virtual void ReadReturnResponse(CPJNPOP3Message& message, DWORD dwSize);
  virtual BOOL ReadResponse(CStringA& sResponse, LPSTR pszTerminator, BOOL bCalledForReturnResponse);
  CStringA     GetBodyOfResponse(const CStringA& sResponse) const;
#ifndef CPJNPOP3_NOSSL
  virtual CString GetOpenSSLError();
#endif
  void _CreateSocket();
  void _ConnectViaSocks4(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszSocksServer, UINT nSocksPort, DWORD dwConnectionTimeout);
  void _ConnectViaSocks5(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszSocksServer, UINT nSocksPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword, DWORD dwConnectionTimeout, BOOL bUDP);
  void _ConnectViaHTTPProxy(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszHTTPServer, UINT nHTTPProxyPort, CStringA& sProxyResponse, LPCTSTR lpszUserName, LPCTSTR pszPassword, DWORD dwConnectionTimeout, LPCTSTR lpszUserAgent);
  void _Connect(LPCTSTR lpszHostAddress, UINT nHostPort);
  int  _Send(const void *pBuffer, int nBuf);
  int  _Receive(void *pBuffer, int nBuf);
  void _Close();
  BOOL _IsReadible(DWORD dwTimeout);
  void SetReturnLastCommandResponse(const CStringA& sResponse);
  
//Static methods
  __forceinline static void SecureEmptyString(CStringA& sVal);

#ifndef CPJNPOP3_NOSSL
  CSSLContext    m_SSLCtx;               //SSL Context
  CSSLSocket     m_SSL;                  //SSL socket wrapper
#endif
  BOOL           m_bSSL;                 //Are we connecting using SSL?
  CWSocket       m_Socket;               //The socket connection to the POP3 server (if not using SSL) 
  BOOL           m_bConnected;           //Are we currently connected to the server 
  CString        m_sLastCommandResponse; //The full last response the server sent us  
	DWORD          m_dwTimeout;
  ProxyType      m_ProxyType;
  CString        m_sProxyServer;
  int            m_nProxyPort;
  CString        m_sLocalBoundAddress;
  CString        m_sProxyUserName;
  CString        m_sProxyPassword;
  CString        m_sUserAgent;
  int            m_nNumberOfMails;
  BOOL           m_bListRetrieved;
  BOOL           m_bStatRetrieved;
  BOOL           m_bUIDLRetrieved;
  CDWordArray    m_msgSizes;
  CStringArray   m_msgIDs;
};

#endif //__PJNPOP3_H__
