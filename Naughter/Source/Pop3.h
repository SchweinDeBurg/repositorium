/*
Module : POP3.H
Purpose: Defines the interface for a MFC class encapsulation of the POP3 protocol
Created: PJN / 04-05-1998

Copyright (c) 1998 - 2004 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

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

#ifndef _WINSOCKAPI_
#pragma message("To avoid this message, put afxsock.h or winsock.h in your PCH (usually stdafx.h)")
#include <winsock.h>
#endif
  


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

//protected:
  char* m_pszMessage;

  friend class CPop3Connection;
};

//Simple Socket wrapper class
class CPop3Socket
{
public:
//Constructors / Destructors
  CPop3Socket();
  virtual ~CPop3Socket();

//methods
  BOOL  Create();
  BOOL  Connect(LPCTSTR pszHostAddress, int nPort = 110);
  BOOL  Send(LPCSTR pszBuf, int nBuf);
  void  Close();
  int   Receive(LPSTR pszBuf, int nBuf);
  BOOL  IsReadible(BOOL& bReadible, DWORD dwTimeout);

protected:
  BOOL   Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen);
  SOCKET m_hSocket;

	friend class CPop3Connection;
};
  
//The main class which encapsulates the POP3 connection
class CPop3Connection
{
public:
//Constructors / Destructors
  CPop3Connection();
  virtual ~CPop3Connection();

//Methods
  BOOL    Connect(LPCTSTR pszHostName, LPCTSTR pszUser, LPCTSTR pszPassword, int nPort = 110);
  BOOL    Disconnect();
  BOOL    Statistics(int& nNumberOfMails, int& nTotalMailSize);
  BOOL    Delete(int nMsg);
  BOOL    GetMessageSize(int nMsg, DWORD& dwSize);
  BOOL    GetMessageID(int nMsg, CString& sID);
  BOOL    Retrieve(int nMsg, CPop3Message& message);
  BOOL    GetMessageHeader(int nMsg, CPop3Message& message);
  BOOL    Reset();
  BOOL    UIDL();
  BOOL    Noop();
  CString GetLastCommandResponse() const { return m_sLastCommandResponse; };
  DWORD   GetTimeout() const { return m_dwTimeout; };
  void    SetTimeout(DWORD dwTimeout) { m_dwTimeout = dwTimeout; };

protected:
  virtual BOOL ReadStatResponse(int& nNumberOfMails, int& nTotalMailSize);
	virtual BOOL ReadCommandResponse();
  virtual BOOL ReadListResponse(int nNumberOfMails);
  virtual BOOL ReadUIDLResponse(int nNumberOfMails);
  virtual BOOL ReadReturnResponse(CPop3Message& message, DWORD dwSize);
  virtual BOOL ReadResponse(LPSTR pszBuffer, int nInitialBufSize, LPSTR pszTerminator, 
                            LPSTR* ppszOverFlowBuffer, int nGrowBy=4096);
  BOOL  List();
  LPSTR GetFirstCharInResponse(LPSTR pszData) const;

  CPop3Socket  m_Pop;
  int          m_nNumberOfMails;
  BOOL         m_bListRetrieved;
  BOOL         m_bStatRetrieved;
  BOOL         m_bUIDLRetrieved;
  CDWordArray  m_msgSizes;
  CStringArray m_msgIDs;
  BOOL         m_bConnected;
  CString      m_sLastCommandResponse;
	DWORD        m_dwTimeout;
};

#endif //__POP3_H__
