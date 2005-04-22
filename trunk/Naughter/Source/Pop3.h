/*
Module : POP3.H
Purpose: Defines the interface for a MFC class encapsulation of the POP3 protocol
Created: PJN / 04-05-1998

Copyright (c) 1998 - 2005 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////// Includes / Defines ////////////////////////////
#ifndef __POP3_H__
#define __POP3_H__

#ifndef __AFXTEMPL_H__
#pragma message("To avoid this message, put afxtempl.h in your PCH (usually stdafx.h)")
#include <afxtempl.h>
#endif

#include "SocMFC.h" //If you get a compilation error about this missing header file, then you need to download my CWSocket wrapper classes from http://www.naughter.com/w3mfc.html

#ifndef POP3_NOSSL_SUPPORT
#include "OpenSSLMfc.h" //If you get a compilation error about this missing header file, then you need to download my CSSLSocket wrapper classes from http://www.naughter.com/w3mfc.html
#include <openssl\rand.h>
#endif

#if defined(__INTEL_COMPILER)
// remark #383: value copied to temporary, reference to temporary used
#pragma warning(disable: 383)
#endif	// __INTEL_COMPILER

/////////////////////////////// Classes ///////////////////////////////////////

////// forward declaration
class CPop3Connection;

//Encapsulation of a POP3 message
class CPop3Message
{
public:
//Constructors / Destructors
  CPop3Message();
  CPop3Message(const CPop3Message& message);
  virtual ~CPop3Message();

//Methods
  CPop3Message& operator=(const CPop3Message& message);
  LPCSTR  GetMessageText() const { return m_pszMessage; };
  CString GetHeader() const;
  CString GetHeaderItem(const CString& sName, int nItem = 0) const;
  CString GetBody() const;
	LPCSTR  GetRawBody() const;
	CString GetSubject() const { return GetHeaderItem(_T("Subject")); }
	CString GetFrom() const		 { return GetHeaderItem(_T("From")); }
	CString GetDate() const		 { return GetHeaderItem(_T("Date")); }
  CString GetTo() const	     { return GetHeaderItem(_T("To")); }
	CString GetCC() const	     { return GetHeaderItem(_T("CC")); }
	CString GetReplyTo() const;

//Helper Methods
  static CString GetEmailAddress(const CString& sNameAndAddress);
  static CString GetEmailFriendlyName(const CString& sNameAndAddress);

//protected:
  char* m_pszMessage;

  friend class CPop3Connection;
};
  
//The main class which encapsulates the POP3 connection
class CPop3Connection
{
public:
  typedef enum ConnectionType
  {
    ctNormal = 0,
#ifndef POP3_NOSSL_SUPPORT
    ctSSL3 = 1,
#endif
  }; 

  typedef enum ProxyType
  {
    ptNone = 0,
    ptSocks4 = 1,
    ptSocks5 = 2,
    ptHTTP = 3
  };

//Constructors / Destructors
  CPop3Connection();
  virtual ~CPop3Connection();

//Methods
  BOOL           Connect(LPCTSTR pszHostName, LPCTSTR pszUser, LPCTSTR pszPassword, int nPort = 110);
  BOOL           Disconnect();
  BOOL           Statistics(int& nNumberOfMails, int& nTotalMailSize);
  BOOL           Delete(int nMsg);
  BOOL           GetMessageSize(int nMsg, DWORD& dwSize);
  BOOL           GetMessageID(int nMsg, CString& sID);
  BOOL           FindMessageID(const CString& sID, int& nMsg); 
  BOOL           Retrieve(int nMsg, CPop3Message& message);
  BOOL           GetMessageHeader(int nMsg, CPop3Message& message);
  BOOL           Reset();
  BOOL           UIDL();
  BOOL           Noop();
  BOOL           List();

  BOOL           IsConnected() const	{ return m_bConnected; };
  CString        GetLastCommandResponse() const { return m_sLastCommandResponse; };
  DWORD          GetTimeout() const { return m_dwTimeout; };
  void           SetTimeout(DWORD dwTimeout) { m_dwTimeout = dwTimeout; };
  ConnectionType GetConnectionType() const { return m_ConnectionType; };
  void           SetConnectionType(ConnectionType connectionType) { m_ConnectionType = connectionType; };
  void           SetProxyType(ProxyType proxyType) { m_ProxyType = proxyType; };
  ProxyType      GetProxyType() const { return m_ProxyType; };
  void           SetProxyServer(const CString& sServer) { m_sProxyServer = sServer; };
  CString        GetProxyServer() const { return m_sProxyServer; };
  void           SetProxyPort(int nPort) { m_nProxyPort = nPort; };
  int            GetProxyPort() { return m_nProxyPort; };
  void           SetBoundAddress(const CString& sLocalBoundAddress) { m_sLocalBoundAddress = sLocalBoundAddress; };
  CString        SetBoundAddress() const { return m_sLocalBoundAddress; };
  void           SetProxyUserName(const CString& sUserName) { m_sProxyUserName = sUserName; };
  CString        GetProxyUserName() const { return m_sProxyUserName; };
  void           SetProxyPassword(const CString& sPassword) { m_sProxyPassword = sPassword; };
  CString        GetProxyPassword() const { return m_sProxyPassword; };
  CString        GetHTTPProxyUserAgent() const { return m_sUserAgent; };
  void           SetHTTPProxyUserAgent(const CString& sUserAgent) { m_sUserAgent = sUserAgent; };

protected:
  virtual BOOL ReadStatResponse(int& nNumberOfMails, int& nTotalMailSize);
	virtual BOOL ReadCommandResponse();
  virtual BOOL ReadListResponse(int nNumberOfMails);
  virtual BOOL ReadUIDLResponse(int nNumberOfMails);
  virtual BOOL ReadReturnResponse(CPop3Message& message, DWORD dwSize);
  virtual BOOL ReadResponse(LPSTR pszBuffer, int nInitialBufSize, LPSTR pszTerminator, LPSTR* ppszOverFlowBuffer, int nGrowBy=4096);
  BOOL         Send(void* pBuffer, int nBufLen);
  LPSTR        GetFirstCharInResponse(LPSTR pszData) const;

  CWSocket       m_Socket;
#ifndef POP3_NOSSL_SUPPORT
  CSSLSocket     m_SSLSocket;
#endif
  int            m_nNumberOfMails;
  BOOL           m_bListRetrieved;
  BOOL           m_bStatRetrieved;
  BOOL           m_bUIDLRetrieved;
  CDWordArray    m_msgSizes;
  CStringArray   m_msgIDs;
  BOOL           m_bConnected;
  CString        m_sLastCommandResponse;
	DWORD          m_dwTimeout;
  ConnectionType m_ConnectionType;
  ProxyType      m_ProxyType;
  CString        m_sProxyServer;
  int            m_nProxyPort;
  CString        m_sLocalBoundAddress;
  CString        m_sProxyUserName;
  CString        m_sProxyPassword;
  CString        m_sUserAgent;
};

#endif //__POP3_H__
