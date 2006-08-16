/*
Module : PJNPOP3.CPP
Purpose: Implementation for a MFC class encapsulation of the POP3 protocol
Created: PJN / 04-05-1998
History: PJN / 27-06-1998 1) Fixed a potential buffer overflow problem in Delete
                          and Retrieve functions when a large message number was
                          specified.
                          2) Improve the ReadResponse code by a) passing the 
                          readability check onto the scoket class and b) Sleeping
                          for 100 ms prior to looping around waiting for new 
                          response data
                          3) Classes are now fully Unicode compliant. Unicode
                          build configurations are also now included.
                          4) Now supports the TOP POP3 command which can be
                          issued using the GetHeader function.
         PJN / 04-01-1999 1) Properly UNICODE enabled the code
         PJN / 22-02-1999 1) Improved the reading of responses back from the server by implementing
                          a growable receive buffer
                          2) timeout is now 60 seconds when building for debug
                          3) Code now yields its time slice while waiting for a timeout
                          4) Made most functions virtual to help end user usage
         PJN / 25-03-1999 1) Fixed memory leak in the CPJNPOP3Connection::ReadReturnResponse function.
                          2) Now sleeps for 250 ms instead of yielding the time slice. This helps 
                          reduce CPU usage when waiting for data to arrive in the socket
         PJN / 15-06-1999 1) Added functions to return the message body, header or a particular
                          header field of a message
                          2) Tidied up some of the TRACE messages which the code generates
         PJN / 16-06-1999 1) Fixed a bug in the GetHeaderItem function which was causing a header
                          item which appeared at the begining of the header fields to fail to be 
                          parsed incorrectly.
         PJN / 27-06-1999 1) Fixed a bug in the GetHeaderItem function when a header spanned 
                          multiple lines as is allowed by RFC 822
         PJN / 29-06-1999 1) Another improvement to GetHeaderItem. When will it end <g>. Originally 
                          this was reported as a bug but upon further investigation it turns out that
                          the message which was causing the problems had embedded tabs in the header. 
                          This is discouraged by the RFC which refers to mail headers (RFC 822). 
                          The code has been enhanced to handle this case. Thanks to Chris Bishop 
                          for spotting this.
                          2) Fix for a bug in GetHeaderItem where I accidently was using "=" instead of
                          "==". Thanks to Angelini Fabio for spotting this.
         PJN / 05-07-1999 1) Addition of the following functions:

                          i)   CPJNPOP3Message::GetReplyTo
                          ii)  CPJNPOP3Message::GetRawBody      
                          iii) CPJNPOP3Message::GetSubject                 
	                        iv)  CPJNPOP3Message::GetFrom                    
	                        v)   CPJNPOP3Message::GetDate                    

                          2) GetHeaderItem function now uses case insensitive searching
                          3) GetHeaderItem now allows you to search for the "n'th" header of a specified type
         PJN / 24-08-1999 1) Fixed a bug whereby the function GetHeader was sometimes failing when it
                          was called when the message was retrieved using the "TOP" command.
         PJN / 27-03-2000 1) Fixed a problem where GetBody and GetRawBody will fail if you call it for a 
                          CPJNPOP3Message object that doesn't have a message in it (i.e m_pszMessage == NULL). 
                          This was previously causing a goof old access violation in GetRawBody.
         PJN / 20-09-2000 1) Fixed a bug in CPJNPOP3Connection::UIDL when message id's were returned
                          with embedded dots in them. Thanks to Konstantin Vasserman for finding and resolving
                          this problem.
         PJN / 25-03-2001 1) Updated copyright information
         PJN / 27-07-2001 1) Added two methods namely: GetTo() and GetCC() to the CPJNPOP3Message class
         PJN / 11-08-2001 1) Fixed a bug in CPJNPOP3Connection::ReadResponse where determining the terminator
                          was failing when an embedded NULL was in the message.
         PJN / 27-09-2001 1) Fixed a bug in CPJNPOP3Connection::ReadResponse when handling disconnection errors
                          2) Improved the error handling in Delete, GetMessageSize & GetMessageID methods
         PJN / 29-09-2001 1) Further improved the error handling in CPJNPOP3Connection::ReadResponse
         PJN / 05-07-2003 1) Added copy constructors and operator= methods to the CPJNPOP3Message class. Thanks
                          to "Sten" for pointing out this omission.
                          2) Made destructors of all classes virtual.
         PJN / 21-07-2003 1) Class now preserves last command response across calls to Disconnect. This is 
                          especially important in calls to Connect when for example you get the POP3 error
                          "-Err. Can't lock the mailbox" or something like this when you call Connect. Thanks
                          to "Lev Elbert" for reporting this problem and providing the fix. 
         PJN / 03-11-2003 1) Simplified the code in CPJNPOP3Connection::ReadResponse. Thanks to Clarke Brunt for 
                          reporting this issue.
                          2) Reviewed the various trace statements in the code.
         PJN / 06-11-2003 1) Fixed a problem with an unitialized "m_pszMessage" in the CPJNPOP3Message 
                          constructor. Thanks to Michael Gunlock for reporting this problem.
         PJN / 12-11-2003 1) Fixed a bug in the CPJNPOP3Message operator= method. Thanks to Henrik Grek for 
                          reporting this bug.
         PJN / 05-06-2004 1) Fixed a bug in CPJNPOP3Connection::ReadResponse, where the wrong parameters
                          were being null terminated. Thanks to "caowen" for this update.
         PJN / 04-07-2004 1) Added an IsConnected() method to the CPJNPOP3Connection class. Thanks to Alessandro 
                          Rosa for this addition.
                          2) Added two new helper functions, namely CPJNPOP3Message::GetEmailAddress and 
                          CPJNPOP3Message::GetEmailFriendlyName. Thanks to Alessandro Rosa for this suggestion.
         PJN / 09-08-2004 1) Fixed a number of compiler warnings when the code is compiled with VC.Net 2003
         PJN / 27-03-2005 1). Fixed a number of compiler warnings in the sample app when the code is compiled
                          with VC .NET 2003.
                          2) Timeout now defaults to 5 seconds in debug mode.
                          3) Added support for connecting via Socks4, Socks5 and HTTP proxies
                          4) Added support for POP3 via SSL. Thanks to Serhiy Pavlov and Alexey Kuznetsov for
                          suggesting this update. Please note that because I do not have access to a mail 
                          server which provides POP3 over SSL, you should consider this support preliminary
                          until I get feedback from users of this feature. The standard POP3 support is still
                          fully functional and tested of course.
         PJN / 09-04-2005 1) Updated the initialization code for OpenSSL in the sample app to avoid memory 
                          leaks. Thanks to Alexey Kuznetsov for reporting these issues. You should implement
                          the same initialization and cleanup code in your applications. For details please check
                          the code in "main.cpp" module which is enclosed in #ifndef POP3_NOSSL_SUPPORT / 
                          #endif sections.
                          2) "List" function is now public instead of protected. Thanks to Alexey Kuznetsov for 
                          reporting these issues. 
                          3) Addition of a simple "FindMessageID" which returns the Message index given a message
                          ID as returned via UIDL. Thanks to Alexey Kuznetsov for this addition. 
         PJN / 08-02-2006 1) Fixed a /Wp64 compile issue in CPJNPOP3Connection::FindMessageID. Thanks to Alexey 
                          Kuznetsov for reporting this issue.    
                          2) Updated copyright details.
                          3) Renamed the classes to include a PJN prefix inline with my SMTP class.
                          4) Provided a PJNPOP3_EXT_CLASS to allow the class to be easily added to an extension dll
                          5) Changed the preprocessor macro to exclude SSL support to be CPJNPOP3_NOSSL. This is
                          to maintain consistency with my SMTP class.     
                          6) Changed the default release mode timeout to 60 seconds and the default debug mode 
                          timeout to 90 seconds.
         PJN / 13-02-2006 1) Fixed a graceful disconnect issue in CPJNPOP3Connection::ReadResponse.
         PJN / 09-07-2006 1) Optimized CPJNPOP3Message constructor code.
                          2) Code now uses new C++ style casts rather than old style C casts where necessary.
                          3) Optimized CPJNPOP3Connection constructor code
                          4) All classes now uses exceptions to indicate errors. This means the whole area of 
                          handling errors in the code is a whole lot simpler. You should carefully review your 
                          code as a lot of the return values from methods (especially CPJNPOP3Connection) are now void 
                          and will throw CPJNPOP3Exception's.
                          5) General rework of the class to bring it in line with the author's CPJNSMTPConnection code
                          6) Update the code in the sample app to TRACE out what it is doing
                          7) Updated the code to clean compile on VC 2005

Copyright (c) 1998 - 2006 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/

//////////////// Includes /////////////////////////////////////////////////////

#include "stdafx.h"
#ifndef __AFXPRIV_H__
#pragma message("To avoid this message, please put afxpriv.h in your pre compiled header (normally stdafx.h)")
#include <afxpriv.h>
#endif

#include "pop3.h"
#include "pop3.rh"

#if defined(__INTEL_COMPILER)
// remark #174: expression has no effect
#pragma warning(disable: 174)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
// remark #383: value copied to temporary, reference to temporary used
#pragma warning(disable: 383)
// remark #593: variable was set but never used
#pragma warning(disable: 593)
// remark #981: operands are evaluated in unspecified order
#pragma warning(disable: 981)
#endif	// __INTEL_COMPILER

//////////////// Macros ///////////////////////////////////////////////////////

#if (_MFC_VER < 0x700)
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


//////////////// Implementation ///////////////////////////////////////////////

CPJNPOP3Exception::CPJNPOP3Exception(HRESULT hr, const CString& sLastResponse) : m_hr(hr), 
                                                                                 m_sLastResponse(sLastResponse)
{
}

CPJNPOP3Exception::CPJNPOP3Exception(DWORD dwError, DWORD dwFacility, const CString& sLastResponse) : m_hr(MAKE_HRESULT(SEVERITY_ERROR, dwFacility, dwError)),
                                                                                                      m_sLastResponse(sLastResponse)
{
}

BOOL CPJNPOP3Exception::GetErrorMessage(LPTSTR pstrError, UINT nMaxError, PUINT pnHelpContext)
{
	ASSERT(pstrError != NULL && AfxIsValidString(pstrError, nMaxError));

	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

  BOOL bRet = FALSE;
  if (HRESULT_FACILITY(m_hr) == FACILITY_ITF)
  {
    //Simply load up the string from the string table
    CString sError;
    AfxFormatString1(sError, HRESULT_CODE(m_hr), m_sLastResponse);
    lstrcpyn(pstrError, sError, nMaxError);
    bRet = TRUE;
  }
  else
  {
  	LPTSTR lpBuffer;
	  bRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			                   NULL, HRESULT_CODE(m_hr), MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			                   reinterpret_cast<LPTSTR>(&lpBuffer), 0, NULL);

	  if (bRet == FALSE)
		  *pstrError = '\0';
	  else
	  {
		  lstrcpyn(pstrError, lpBuffer, nMaxError);
		  LocalFree(lpBuffer);
	  }
  }

	return bRet;
}

CString CPJNPOP3Exception::GetErrorMessage()
{
  CString rVal;
  LPTSTR pstrError = rVal.GetBuffer(4096);
  GetErrorMessage(pstrError, 4096, NULL);
  rVal.ReleaseBuffer();
  return rVal;
}

IMPLEMENT_DYNAMIC(CPJNPOP3Exception, CException)

#ifdef _DEBUG
void CPJNPOP3Exception::Dump(CDumpContext& dc) const
{
  //Let the base class do its thing
	CObject::Dump(dc);

	dc << "m_hr = " << m_hr;
}
#endif


CPJNPOP3Message::CPJNPOP3Message() : m_pszMessage(NULL)
{
}

CPJNPOP3Message::CPJNPOP3Message(const CPJNPOP3Message& message) : m_pszMessage(NULL)
{
  *this = message;
}

CPJNPOP3Message::~CPJNPOP3Message()
{
  if (m_pszMessage)
  {
    delete [] m_pszMessage;
    m_pszMessage = NULL;
  }
}

CPJNPOP3Message& CPJNPOP3Message::operator=(const CPJNPOP3Message& message)
{
  //Tidy up any heap memory we may be using
  if (m_pszMessage)
  {
    delete [] m_pszMessage;
    m_pszMessage = NULL;
  }

  if (message.m_pszMessage)
	{
    //Allocate the new heap memory
    size_t nMessageSize = strlen(message.m_pszMessage);
    m_pszMessage = new char[nMessageSize+1];

    //Transfer across its contents
  #if (_MSC_VER >= 1400)
    strcpy_s(m_pszMessage, nMessageSize+1, message.m_pszMessage);
  #else  
    strcpy(m_pszMessage, message.m_pszMessage);
  #endif
  }

  return *this;
}

CString CPJNPOP3Message::GetHeaderItem(const CString& sName, int nItem) const
{
  //Value which will be returned by this function
  CString sField;

  //Get the message header (add an extra "\r\n" at the
  //begining to aid in the parsing)  
	CString sHeader(_T("\r\n"));
  sHeader += GetHeader();
  CString sUpCaseHeader(sHeader);
  sUpCaseHeader.MakeUpper();

  CString sUpCaseName(sName);
  sUpCaseName.MakeUpper();

  //Find the specified line in the header
  CString sFind(CString(_T("\r\n")) + sUpCaseName + _T(":"));
  int nFindLength = sFind.GetLength();
  int nFindStart = sUpCaseHeader.Find(sFind);
  int nFind = nFindStart;
	for (int i=0; i<nItem; i++) 
  {
    //Get ready for the next loop around
    sUpCaseHeader = sUpCaseHeader.Right(sUpCaseHeader.GetLength() - nFind - nFindLength);
    nFind = sUpCaseHeader.Find(sFind);
    
		if (nFind == -1)
			return _T(""); //Not found
    else
      nFindStart += (nFind + nFindLength);
	}

  if (nFindStart != -1)
    nFindStart += (3 + sName.GetLength());
  if (nFindStart != -1)
  {
    BOOL bFoundEnd = FALSE;
    int i = nFindStart;
    int nLength = sHeader.GetLength();
    do
    {
      //Examine the current 3 characters
      TCHAR c1 = _T('\0');
      if (i < nLength)
        c1 = sHeader[i];
      TCHAR c2 = _T('\0');
      if (i < (nLength-1))
        c2 = sHeader[i+1];
      TCHAR c3 = _T('\0');
      if (i < (nLength-2))
        c3 = sHeader[i+2];

      //Have we found the terminator
      if ((c1 == _T('\0')) || ((c1 == _T('\r')) && (c2 == _T('\n')) && (c3 != _T(' ')) && c3 != _T('\t')))
      {
        bFoundEnd = TRUE;
      }
      else
      {
        //Move onto the next character  
        ++i;
      }
    }
    while (!bFoundEnd);
    sField = sHeader.Mid(nFindStart, i - nFindStart);

    //Remove any embedded "\r\n" sequences from the field
    int nEOL = sField.Find(_T("\r\n"));
    while (nEOL != -1)
    {
      sField = sField.Left(nEOL) + sField.Right(sField.GetLength() - nEOL - 2);
      nEOL = sField.Find(_T("\r\n"));
    }

    //Replace any embedded "\t" sequences with spaces
    int nTab = sField.Find(_T('\t'));
    while (nTab != -1)
    {
      sField = sField.Left(nTab) + _T(' ') + sField.Right(sField.GetLength() - nTab - 1);
      nTab = sField.Find(_T('\t'));
    }

    //Remove any leading or trailing white space from the Field Body
    sField.TrimLeft();
    sField.TrimRight();
  }

	return sField;
}

CString CPJNPOP3Message::GetHeader() const
{
  //Value which will be returned by this function
  CString sHeader;

  //Find the divider between the header and body
	CString sMessage(m_pszMessage);
  int nFind = sMessage.Find(_T("\r\n\r\n"));
  if (nFind != -1)
    sHeader = sMessage.Left(nFind);
  else
  {
    //No divider, then assume all the text is the header
    sHeader = sMessage;
  }

  return sHeader;
}

LPCSTR CPJNPOP3Message::GetRawBody() const
{
  if (m_pszMessage == NULL)
    return NULL;
	char* pszStartBody = strstr(m_pszMessage, "\r\n\r\n");
	if (pszStartBody == NULL) 
    return NULL;
	else 
    return pszStartBody + 4;
}

CString CPJNPOP3Message::GetBody() const
{
  CString sBody;
  LPCSTR pszBody = GetRawBody();
  if (pszBody)
    sBody = pszBody;
  return sBody;
}

CString CPJNPOP3Message::GetReplyTo() const
{
  //What will be the return value from this function
	CString sRet(GetHeaderItem(_T("Reply-To")));

  if (sRet.IsEmpty())
  {
    sRet = GetFrom();
    if (sRet.IsEmpty())
    {
      sRet = GetHeaderItem(_T("Sender"));
      if (sRet.IsEmpty())
        sRet = GetHeaderItem(_T("Return-Path"));
    }
  }
	return sRet;
}

CString CPJNPOP3Message::GetEmailAddress(const CString& sNameAndAddress)
{
  //What will be the return value
  CString sEmailAddress;

	//divide the substring into friendly names and e-mail addresses
	int nMark = sNameAndAddress.Find(_T('<'));
	int nMark2 = sNameAndAddress.Find(_T('>'));
	if ((nMark != -1) && (nMark2 != -1) && (nMark2 > (nMark+1)))
		sEmailAddress = sNameAndAddress.Mid(nMark+1, nMark2 - nMark - 1);
	else
		sEmailAddress = sNameAndAddress;

  return sEmailAddress;
}

CString CPJNPOP3Message::GetEmailFriendlyName(const CString& sNameAndAddress)
{
  //What will be the return value
  CString sFriendlyName;

	//divide the substring into friendly names and e-mail addresses
	int nMark = sNameAndAddress.Find(_T('<'));
	int nMark2 = sNameAndAddress.Find(_T('>'));
	if ((nMark != -1) && (nMark2 != -1) && (nMark2 > (nMark+1)))
	{
		sFriendlyName = sNameAndAddress.Left(nMark);
    sFriendlyName.TrimLeft();
    sFriendlyName.TrimRight();

    //Remove any quotes which may appear on the friendly name
    int nLength = sFriendlyName.GetLength();
    if (nLength)
    {
      //Remove the leading quote
      if (sFriendlyName.GetAt(nLength - 1) == _T('"'))
      {
        sFriendlyName = sFriendlyName.Left(nLength - 1);
        nLength--;
      }
      //Remove the trailing quote
      if (nLength)
      {
        if (sFriendlyName.GetAt(0) == _T('"'))
          sFriendlyName = sFriendlyName.Right(nLength - 1);
      }
    }
  }

  return sFriendlyName;
}


CPJNPOP3Connection::CPJNPOP3Connection() : m_nNumberOfMails(0),
                                           m_bListRetrieved(FALSE),
                                           m_bStatRetrieved(FALSE),
                                           m_bUIDLRetrieved(FALSE),
                                           m_bConnected(FALSE),
                                           m_bSSL(FALSE),
                                           m_ProxyType(ptNone),
                                           m_nProxyPort(1080)
{
#ifdef _DEBUG
  m_dwTimeout = 90000; //default timeout of 90 seconds when debugging
#else
 m_dwTimeout = 60000;  //default timeout of 60 seconds for normal release code
#endif
}

CPJNPOP3Connection::~CPJNPOP3Connection()
{
  if (m_bConnected)
  {
    //Deliberately handle exceptions here, so that the destructor does not throw exception
    try
    {
      Disconnect(TRUE);
    }
    catch(CPJNPOP3Exception* pEx)
    {
      pEx->Delete();
    }
  }
}

void CPJNPOP3Connection::ThrowPJNPOP3Exception(DWORD dwError, DWORD dwFacility, const CString& sLastResponse)
{
	if (dwError == 0)
		dwError = ::WSAGetLastError();

	CPJNPOP3Exception* pException = new CPJNPOP3Exception(dwError, dwFacility, sLastResponse);

	TRACE(_T("Warning: throwing CPJNPOP3Exception for error %x\n"), pException->m_hr);
	THROW(pException);
}

void CPJNPOP3Connection::ThrowPJNPOP3Exception(HRESULT hr, const CString& sLastResponse)
{
	CPJNPOP3Exception* pException = new CPJNPOP3Exception(hr, sLastResponse);

	TRACE(_T("Warning: throwing CPJNPOP3Exception for error %x\n"), pException->m_hr);
	THROW(pException);
}

void CPJNPOP3Connection::_CreateSocket()
{
	m_Socket.Create();
#ifndef CPJNPOP3_NOSSL
	if (m_bSSL) 
  {
		m_SSLCtx.Attach(SSL_CTX_new(SSLv23_client_method()));
		if (!m_SSL.Create(m_SSLCtx, m_Socket))
      ThrowPJNPOP3Exception(IDS_PJNPOP3_FAIL_CREATE_SSL_SOCKET, FACILITY_ITF, GetOpenSSLError());
	}
#endif
}

#ifndef CPJNPOP3_NOSSL
CString CPJNPOP3Connection::GetOpenSSLError()
{
  //What will be the return value
  CString sError;

  char szError[1024];
  unsigned long nError;
  do
  { 
    nError = ERR_get_error();
    if (nError)
    {
      ERR_error_string(nError, szError);
      sError += szError;
      sError += _T("\r\n");
    }
  }
  while (nError);

  return sError;
}
#endif

void CPJNPOP3Connection::_ConnectViaSocks4(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszSocksServer, UINT nSocksPort, DWORD dwConnectionTimeout)
{
#ifndef CPJNPOP3_NOSSL
	if (m_bSSL) 
  {
		if(!m_SSL.ConnectViaSocks4(lpszHostAddress, nHostPort, lpszSocksServer, nSocksPort, dwConnectionTimeout)) 
			ThrowPJNPOP3Exception(IDS_PJNPOP3_FAIL_CONNECT_SOCKS4_VIASSL, FACILITY_ITF, GetOpenSSLError());
	}
	else 
#endif
		m_Socket.ConnectViaSocks4(lpszHostAddress, nHostPort, lpszSocksServer, nSocksPort, dwConnectionTimeout);
}

void CPJNPOP3Connection::_ConnectViaSocks5(LPCTSTR lpszHostAddress, UINT nHostPort , LPCTSTR lpszSocksServer, UINT nSocksPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword, DWORD dwConnectionTimeout, BOOL bUDP)
{
#ifndef CPJNPOP3_NOSSL
	if (m_bSSL) 
  {
		if (!m_SSL.ConnectViaSocks5(lpszHostAddress, nHostPort, lpszSocksServer, nSocksPort, lpszUserName, lpszPassword, dwConnectionTimeout, bUDP)) 
			ThrowPJNPOP3Exception(IDS_PJNPOP3_FAIL_CONNECT_SOCKS5_VIASSL, FACILITY_ITF, GetOpenSSLError());
	}
	else 
#endif
		m_Socket.ConnectViaSocks5(lpszHostAddress, nHostPort, lpszSocksServer, nSocksPort, lpszUserName, lpszPassword, dwConnectionTimeout, bUDP);
}

void CPJNPOP3Connection::_ConnectViaHTTPProxy(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszHTTPServer, UINT nHTTPProxyPort, CString & sProxyResponse, 
                                              LPCTSTR lpszUserName, LPCTSTR pszPassword, DWORD dwConnectionTimeout, LPCTSTR lpszUserAgent)
{
#ifndef CPJNPOP3_NOSSL
	if (m_bSSL) 
  {
		if (!m_SSL.ConnectViaHTTPProxy(lpszHostAddress, nHostPort, lpszHTTPServer, nHTTPProxyPort, sProxyResponse, lpszUserName, pszPassword, dwConnectionTimeout, lpszUserAgent))
			ThrowPJNPOP3Exception(IDS_PJNPOP3_FAIL_CONNECT_HTTPPROXY_VIASSL, FACILITY_ITF, GetOpenSSLError());
	}
	else 
#endif
		m_Socket.ConnectViaHTTPProxy(lpszHostAddress, nHostPort, lpszHTTPServer, nHTTPProxyPort, sProxyResponse, lpszUserName, pszPassword, dwConnectionTimeout, lpszUserAgent);
}

void CPJNPOP3Connection::_Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
#ifndef CPJNPOP3_NOSSL
	if (m_bSSL) 
  {
		if (!m_SSL.Connect(lpszHostAddress, nHostPort)) 
			ThrowPJNPOP3Exception(IDS_PJNPOP3_FAIL_CONNECT_VIASSL, FACILITY_ITF, GetOpenSSLError());
	}
	else 
#endif
		m_Socket.Connect(lpszHostAddress, nHostPort);
}

int CPJNPOP3Connection::_Send(const void* pBuffer, int nBuf)
{
#ifndef CPJNPOP3_NOSSL
	if (m_bSSL) 
  {
		int nSSLWrite = m_SSL.Send(pBuffer, nBuf);
    if (nSSLWrite < 1)
      ThrowPJNPOP3Exception(IDS_PJNPOP3_FAIL_SEND_VIASSL, FACILITY_ITF, GetOpenSSLError());
    return nSSLWrite;
  }
	else 
#endif
		return m_Socket.Send(pBuffer, nBuf);
}

int CPJNPOP3Connection::_Receive(void *pBuffer, int nBuf)
{
#ifndef CPJNPOP3_NOSSL
	if (m_bSSL) 
  {
		int nSSLReceive = m_SSL.Receive(pBuffer, nBuf);
    if (nSSLReceive <= 0)
    {
      int nSSLError = SSL_get_error(m_SSL, nSSLReceive);
      if (nSSLError != SSL_ERROR_WANT_READ)
        ThrowPJNPOP3Exception(IDS_PJNPOP_FAIL_RECEIVE_VIASSL, FACILITY_ITF, GetOpenSSLError());
    }
    return nSSLReceive;
  }
	else 
#endif
		return m_Socket.Receive(pBuffer, nBuf);
}

void CPJNPOP3Connection::_Close()
{
#ifndef CPJNPOP3_NOSSL
	if (m_bSSL)
		m_SSL.Close();
	else 
#endif
		m_Socket.Close();
}

BOOL CPJNPOP3Connection::_IsReadible(DWORD dwTimeout)
{
#ifndef CPJNPOP3_NOSSL
	if (m_bSSL) 
		return m_SSL.IsReadible(dwTimeout);
	else 
#endif
		return m_Socket.IsReadible(dwTimeout);
}
#ifndef CPJNPOP3_NOSSL
void CPJNPOP3Connection::Connect(LPCTSTR pszHostName, LPCTSTR pszUsername, LPCTSTR pszPassword, int nPort, BOOL bSSL)
#else
void CPJNPOP3Connection::Connect(LPCTSTR pszHostName, LPCTSTR pszUsername, LPCTSTR pszPassword, int nPort)
#endif
{
	USES_CONVERSION;

  //Validate our parameters
#ifndef CPJNPOP3_NOSSL
  m_bSSL = bSSL;
#else
  m_bSSL = FALSE;
#endif

  //Create the socket
  try
  {
    _CreateSocket();
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();

    ThrowPJNPOP3Exception(dwError, FACILITY_WIN32);
  }

  try
  {
    //Connect to the POP3 server

    //Bind if required
    if (m_sLocalBoundAddress.GetLength())
      m_Socket.Bind(0, m_sLocalBoundAddress);

    switch (m_ProxyType)
    {
      case ptSocks4:
      {
        _ConnectViaSocks4(pszHostName, nPort, m_sProxyServer, m_nProxyPort, m_dwTimeout);
        break;
      }
      case ptSocks5:
      {
        if (m_sProxyUserName.GetLength())
          _ConnectViaSocks5(pszHostName, nPort, m_sProxyServer, m_nProxyPort, m_sProxyUserName, m_sProxyPassword, m_dwTimeout, FALSE);
        else
          _ConnectViaSocks5(pszHostName, nPort, m_sProxyServer, m_nProxyPort, NULL, NULL, m_dwTimeout, FALSE);
        break;
      }
      case ptHTTP:
      {
        CString sProxyResponse;
        if (m_sProxyUserName.GetLength())
        {
          if (m_sUserAgent.GetLength())
            _ConnectViaHTTPProxy(pszHostName, nPort, m_sProxyServer, m_nProxyPort, sProxyResponse, m_sProxyUserName, m_sProxyPassword, m_dwTimeout, m_sUserAgent);
          else
            _ConnectViaHTTPProxy(pszHostName, nPort, m_sProxyServer, m_nProxyPort, sProxyResponse, m_sProxyUserName, m_sProxyPassword, m_dwTimeout, NULL);
        }
        else
        {
          if (m_sUserAgent.GetLength())
            _ConnectViaHTTPProxy(pszHostName, nPort, m_sProxyServer, m_nProxyPort, sProxyResponse, NULL, NULL, m_dwTimeout, m_sUserAgent);
          else
            _ConnectViaHTTPProxy(pszHostName, nPort, m_sProxyServer, m_nProxyPort, sProxyResponse, NULL, NULL, m_dwTimeout, NULL);
        }
        break;
      }
      case ptNone:
      {
        _Connect(pszHostName, nPort);
        break;
      }
      default:
      {
        ASSERT(FALSE);
        break;
      }
    }
  }
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();

    //Disconnect before we rethrow the exception
    try
    {
      Disconnect(TRUE);
    }
    catch(CPJNPOP3Exception* pEx2)
    {
      pEx2->Delete();
    }

    //rethrow the exception
    ThrowPJNPOP3Exception(dwError, FACILITY_WIN32);
  }

  //We're now connected !!
  m_bConnected = TRUE;

  try
  {
    //check the response
    if (!ReadCommandResponse())
      ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_POP3_LOGIN_RESPONSE, FACILITY_ITF, GetLastCommandResponse());

    //Send the POP3 username and check the response
    CString sRequest;
    sRequest.Format(_T("USER %s\r\n"), pszUsername);
    char* pszAsciiRequest = T2A(const_cast<LPTSTR>(sRequest.operator LPCTSTR()));
    int nCmdLength = static_cast<int>(strlen(pszAsciiRequest));
    _Send(pszAsciiRequest, nCmdLength);
    if (!ReadCommandResponse())
      ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_POP3_LOGIN_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 

    //Send the POP3 password and check the response
    sRequest.Format(_T("PASS %s\r\n"), pszPassword);
    pszAsciiRequest = T2A(const_cast<LPTSTR>(sRequest.operator LPCTSTR()));
    nCmdLength = static_cast<int>(strlen(pszAsciiRequest));
    _Send(pszAsciiRequest, nCmdLength);
    if (!ReadCommandResponse())
      ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_POP3_LOGIN_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
  }
  catch(CPJNPOP3Exception* pEx)
  {
    //Disconnect before we rethrow the exception
    try
    {
      Disconnect(TRUE);
    }
    catch(CPJNPOP3Exception* pEx2)
    {
      pEx2->Delete();
    }

    HRESULT hr = pEx->m_hr;
    CString sLastResponse = pEx->m_sLastResponse;
    pEx->Delete();

    //rethrow the exception
    ThrowPJNPOP3Exception(hr, sLastResponse);
  }
}

void CPJNPOP3Connection::Disconnect(BOOL bGracefully)
{          
  //disconnect from the SMTP server if connected 
  HRESULT hr = S_OK;
  CString sLastResponse;
  if (m_bConnected)
  {
    if (bGracefully)
    {
      static char* sBuf= "QUIT\r\n";
      int nCmdLength = static_cast<int>(strlen(sBuf));
      try
      {
        _Send(sBuf, nCmdLength);
      }
      catch(CWSocketException* pEx)
      {
        hr = MAKE_HRESULT(SEVERITY_ERROR, pEx->m_nError, FACILITY_WIN32);
        pEx->Delete();
      }

      //Check the reponse
      if (!ReadCommandResponse())
      {
        hr = MAKE_HRESULT(SEVERITY_ERROR, IDS_PJNPOP3_UNEXPECTED_QUIT_RESPONSE, FACILITY_ITF);
        sLastResponse = GetLastCommandResponse();
      }
    }

    //Reset the connected state
    m_bConnected = FALSE;
  }
 
  //free up our socket
  _Close();

  //Reset all our member variables
  m_bListRetrieved = FALSE;
  m_bStatRetrieved = FALSE;
  m_bUIDLRetrieved = FALSE;
  m_msgSizes.RemoveAll();
  m_msgIDs.RemoveAll();

  //Should we throw an expection
  if (FAILED(hr))
    ThrowPJNPOP3Exception(hr, sLastResponse);
}

void CPJNPOP3Connection::Delete(int nMsg)
{
  USES_CONVERSION;

  //Must be connected to perform a delete
  ASSERT(m_bConnected);

  //if we haven't executed the LIST command then do it now
  if (!m_bListRetrieved)
    List();

  //Send the DELE command along with the message ID
	CString sRequest;
 	sRequest.Format(_T("DELE %d\r\n"), nMsg);
  char* pszAsciiRequest = T2A(const_cast<LPTSTR>(sRequest.operator LPCTSTR()));
  int nCmdLength = static_cast<int>(strlen(pszAsciiRequest));
	_Send(pszAsciiRequest, nCmdLength);

	if (!ReadCommandResponse())
    ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_DELE_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
}

void CPJNPOP3Connection::Statistics(int& nNumberOfMails, int& nTotalMailSize)
{
  //Must be connected to perform a "STAT"
  ASSERT(m_bConnected);

  //Send the STAT command
	static char* sBuf = "STAT\r\n";
  int nCmdLength = static_cast<int>(strlen(sBuf));
	_Send(sBuf, nCmdLength);

	ReadStatResponse(nNumberOfMails, nTotalMailSize);
  m_bStatRetrieved = TRUE;
}

DWORD CPJNPOP3Connection::GetMessageSize(int nMsg)
{
  //if we haven't executed the LIST command then do it now
  if (!m_bListRetrieved)
    List();

  //nMsg must be in the correct range
  ASSERT((nMsg > 0) && (nMsg <= m_msgSizes.GetSize()));

  //retrieve the size from the message size array
  return m_msgSizes.GetAt(nMsg - 1);
}

CString CPJNPOP3Connection::GetMessageID(int nMsg)
{
  //if we haven't executed the UIDL command then do it now
  if (!m_bUIDLRetrieved)
    UIDL();

  //nMsg must be in the correct range
  ASSERT((nMsg > 0) && (nMsg <= m_msgIDs.GetSize()));

  //retrieve the size from the message size array
  return m_msgIDs.GetAt(nMsg - 1);
}

int CPJNPOP3Connection::FindMessageID(const CString& sID)
{
  //Validate our parameters
  ASSERT(!sID.IsEmpty());

  //if we haven't executed the UIDL command then do it now
  if (!m_bUIDLRetrieved)
    UIDL();

  //find message ID
  int nMsgIds = static_cast<int>(m_msgIDs.GetSize());
  for (int i=0; i<nMsgIds; i++)
  {
    if (m_msgIDs.GetAt(i) == sID)
      return i + 1; //nMsg is 1 based
  }

  return -1;
}

void CPJNPOP3Connection::List()
{
  //Must be connected to perform a "LIST"
  ASSERT(m_bConnected);

  //if we haven't executed the STAT command then do it now
  int nNumberOfMails = m_nNumberOfMails;
  int nTotalMailSize;
  if (!m_bStatRetrieved)
    Statistics(nNumberOfMails, nTotalMailSize);

  //Send the LIST command
  static char* sBuf = "LIST\r\n";
  int nCmdLength = static_cast<int>(strlen(sBuf));
	_Send(sBuf, nCmdLength);

  //And check the response
	ReadListResponse(nNumberOfMails);
  m_bListRetrieved = TRUE;
}

void CPJNPOP3Connection::UIDL()
{
  //Must be connected to perform a "UIDL"
  ASSERT(m_bConnected);

  //if we haven't executed the STAT command then do it now
  int nNumberOfMails = m_nNumberOfMails;
  int nTotalMailSize;
  if (!m_bStatRetrieved)
    Statistics(nNumberOfMails, nTotalMailSize);

  //Send the UIDL command
  static char* sBuf = "UIDL\r\n";
  int nCmdLength = static_cast<int>(strlen(sBuf));
	_Send(sBuf, nCmdLength);

  //And check the response
	ReadUIDLResponse(nNumberOfMails);
  m_bUIDLRetrieved = TRUE;
}

void CPJNPOP3Connection::Reset()
{
  //Must be connected to perform a "RSET"
  ASSERT(m_bConnected);

  //Send the RSET command
	static char* sBuf = "RSET\r\n";
  int nCmdLength = static_cast<int>(strlen(sBuf));
	_Send(sBuf, nCmdLength);

  //And check the command
	if (!ReadCommandResponse())
    ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_RSET_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
}

void CPJNPOP3Connection::Noop()
{
  //Must be connected to perform a "NOOP"
  ASSERT(m_bConnected);

  //Send the NOOP command
	static char* sBuf = "NOOP\r\n";
  int nCmdLength = static_cast<int>(strlen(sBuf));
	_Send(sBuf, nCmdLength);

  //And check the response
	if (!ReadCommandResponse())
    ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_POP3_NOOP_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
}

void CPJNPOP3Connection::Retrieve(int nMsg, CPJNPOP3Message& message)
{
  USES_CONVERSION;

  //Must be connected to retrieve a message
  ASSERT(m_bConnected);

  //work out the size of the message to retrieve
  DWORD dwSize = GetMessageSize(nMsg);

  //Send the RETR command
	CString sRequest;
	sRequest.Format(_T("RETR %d\r\n"), nMsg);	
  char* pszAsciiRequest = T2A(const_cast<LPTSTR>(sRequest.operator LPCTSTR()));
  int nCmdLength = static_cast<int>(strlen(pszAsciiRequest));
	_Send(pszAsciiRequest, nCmdLength);
  
	//And check the command
	ReadReturnResponse(message, dwSize);
}

void CPJNPOP3Connection::GetMessageHeader(int nMsg, CPJNPOP3Message& message)
{
  USES_CONVERSION;

  //Must be connected to retrieve a message
  ASSERT(m_bConnected);

  //make sure the message actually exists
  DWORD dwSize = GetMessageSize(nMsg);

  //Send the TOP command
  CString sRequest;
  sRequest.Format(_T("TOP %d 0\r\n"), nMsg);
  char* pszAsciiRequest = T2A(const_cast<LPTSTR>(sRequest.operator LPCTSTR()));
  int nCmdLength = static_cast<int>(strlen(pszAsciiRequest));
  _Send(pszAsciiRequest, nCmdLength);

  //And check the command
  ReadReturnResponse(message, dwSize);
}

BOOL CPJNPOP3Connection::ReadCommandResponse()
{
  LPSTR pszOverFlowBuffer = NULL;
  char sBuf[1000];
  BOOL bSuccess = ReadResponse(sBuf, 1000, "\r\n", &pszOverFlowBuffer);
  if (pszOverFlowBuffer)
    delete [] pszOverFlowBuffer;
  
  //determine if the response is an error
  if (bSuccess)
  {
    CString sUpperCaseLastCommandResponse(m_sLastCommandResponse);
    sUpperCaseLastCommandResponse.MakeUpper();
	  bSuccess = (sUpperCaseLastCommandResponse.Find(_T("+OK")) == 0);
  }

  return bSuccess;
}

LPSTR CPJNPOP3Connection::GetFirstCharInResponse(LPSTR pszData) const
{
  while ((*pszData != '\n') && *pszData)
    ++pszData;

  //skip over the "\n" onto the next line
  if (*pszData)
    ++pszData;

  return pszData;
}

void CPJNPOP3Connection::ReadReturnResponse(CPJNPOP3Message& message, DWORD dwSize)
{
  //Must be connected to perform a "RETR"
  ASSERT(m_bConnected);

  //Retrieve the message body
  LPSTR pszOverFlowBuffer = NULL;
  int nSize = dwSize + 100;
  char* sBuf = new char[nSize];
  char* sMessageBuf = sBuf;

  BOOL bReadResponse = ReadResponse(sBuf, nSize, "\r\n.\r\n", &pszOverFlowBuffer, 32000);
  if (!bReadResponse)
	{
    delete [] sBuf;
    if (pszOverFlowBuffer)
      delete [] pszOverFlowBuffer;

		ThrowPJNPOP3Exception(IDS_PJNPOP3_FAILED_RECEIVE_RETR_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
	}
  if (pszOverFlowBuffer)
    sMessageBuf = pszOverFlowBuffer;

  //determine if the response is an error
  if (_strnicmp(sMessageBuf, "+OK", 3) != 0)
	{
    delete [] sBuf;
    if (pszOverFlowBuffer)
      delete [] pszOverFlowBuffer;

    ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_RETR_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
	}
	else
	{  
    //remove the first line which contains the +OK from the message
    char* pszFirst = GetFirstCharInResponse(sMessageBuf);
		VERIFY(pszFirst);

    //transfer the message contents to the message class
    size_t nMessageSize = (sMessageBuf - pszFirst + strlen(sMessageBuf));

    // Do we already have memory allocated? If so, destroy it!
  	if (message.m_pszMessage)
  	{
  	  delete [] message.m_pszMessage;
  	  message.m_pszMessage = NULL;
    }

    message.m_pszMessage = new char[nMessageSize + 1];
    memcpy(message.m_pszMessage, pszFirst, nMessageSize);
    message.m_pszMessage[nMessageSize] = '\0';
	}
  delete [] sBuf;
  if (pszOverFlowBuffer)
    delete [] pszOverFlowBuffer;
}

void CPJNPOP3Connection::ReadListResponse(int nNumberOfMails)
{
  //Must be connected to perform a "LIST"
  ASSERT(m_bConnected);

  //retrieve the reponse
  LPSTR pszOverFlowBuffer = NULL;
  int nSize = (14 * nNumberOfMails) + 100;
  char* sBuf = new char[nSize];
  char* sMessageBuf = sBuf;
  if (!ReadResponse(sBuf, nSize, "\r\n.\r\n", &pszOverFlowBuffer))
	{
    delete [] sBuf;
    if (pszOverFlowBuffer)
      delete [] pszOverFlowBuffer;

		ThrowPJNPOP3Exception(IDS_PJNPOP3_FAILED_RECEIVE_LIST_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
	}
  if (pszOverFlowBuffer)
    sMessageBuf = pszOverFlowBuffer;

  //determine if the response is an error
  if (_strnicmp(sMessageBuf,"+OK", 3) != 0)
	{
    delete [] sBuf;
    if (pszOverFlowBuffer)
      delete [] pszOverFlowBuffer;

    ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_LIST_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
	}
	else
	{  
    //Retrieve the message sizes and put them
    //into the m_msgSizes array
    m_msgSizes.RemoveAll();
    m_msgSizes.SetSize(0, nNumberOfMails);

    //then parse the LIST response
		char* pszSize = GetFirstCharInResponse(sMessageBuf);
		VERIFY(pszSize);
		for (; *pszSize != '.'; pszSize++)
			if (*pszSize == '\t' || *pszSize == ' ')
				m_msgSizes.Add(atoi(pszSize));
	}
  delete [] sBuf;
  if (pszOverFlowBuffer)
    delete [] pszOverFlowBuffer;
}

void CPJNPOP3Connection::ReadUIDLResponse(int nNumberOfMails)
{
  //Must be connected to perform a "UIDL"
  ASSERT(m_bConnected);

  //retrieve the reponse
  LPSTR pszOverFlowBuffer = NULL;
  int nSize = (14 * nNumberOfMails) + 100;
  char* sBuf = new char[nSize];
  char* sMessageBuf = sBuf;
  if (!ReadResponse(sBuf, nSize, "\r\n.\r\n", &pszOverFlowBuffer))
	{
    delete [] sBuf;
    if (pszOverFlowBuffer)
      delete [] pszOverFlowBuffer;

		ThrowPJNPOP3Exception(IDS_PJNPOP3_FAILED_RECEIVE_UIDL_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
	}
  if (pszOverFlowBuffer)
    sMessageBuf = pszOverFlowBuffer;

  //determine if the response is an error
  if (_strnicmp(sMessageBuf,"+OK", 3) != 0)
	{
    delete [] sBuf;
    if (pszOverFlowBuffer)
      delete [] pszOverFlowBuffer;

    ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_UIDL_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
	}
	else
	{  
    //Retrieve the message ID's and put them
    //into the m_msgIDs array
    m_msgIDs.RemoveAll();
    m_msgIDs.SetSize(0, nNumberOfMails);

		//then parse the UIDL response
		char* pszSize = GetFirstCharInResponse(sMessageBuf);
		
		VERIFY(pszSize);
		for (int iCount=0; iCount<nNumberOfMails; iCount++)
		{
		  //Find the next line, before we attempt to parse the current one
		  char* pszBegin = pszSize;
		  while (*pszSize != '\n' && *pszSize != '\0')
  			++pszSize;
  			
	    //Convert the EOL to a NULL so that we can do easier parsing of the message ids
	    if (*pszSize == '\n')
	      *pszSize = '\0';
	      
	    //Find the first whitespace character in the line (which means that we have skipped over the message number)  
  	  char* pszID = pszBegin;
  	  while (*pszID != '\0' && !isspace(*pszID))
  	    ++pszID;
  	    
  	  //Then find the first non-whitespace character in the line (which is the start of the message id)
  	  while (*pszID != '\0' && isspace(*pszID))
  	    ++pszID;
  	    
  	  //Then find the first whitespace character in the line (which is the end of the message id)
  	  char* pszEndID = pszID;
  	  while (*pszEndID != '\0' && !isspace(*pszEndID))
  	    ++pszEndID;
  	  *pszEndID = '\0';
  	    
  	  //Finally form the string from the ID
  	  CString sID;
  	  if (pszID < pszSize)
  	    sID = pszID;
		  m_msgIDs.Add(sID);
		  pszSize++;
		}
	}
  delete [] sBuf;
  if (pszOverFlowBuffer)
    delete [] pszOverFlowBuffer;
}

void CPJNPOP3Connection::ReadStatResponse(int& nNumberOfMails, int& nTotalMailSize)
{
  //Must be connected to perform a "STAT"
  ASSERT(m_bConnected);

  //retrieve the reponse
  LPSTR pszOverFlowBuffer = NULL;
  char sBuf[100];
  char* sMessageBuf = sBuf;
  if (!ReadResponse(sBuf, 100, "\r\n", &pszOverFlowBuffer))
  {
    if (pszOverFlowBuffer)
      delete [] pszOverFlowBuffer;

  	ThrowPJNPOP3Exception(IDS_PJNPOP3_FAILED_RECEIVE_STAT_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
  }
  if (pszOverFlowBuffer)
    sMessageBuf = pszOverFlowBuffer;

  //determine if the response is an error
  if (strncmp(sMessageBuf,"+OK", 3) != 0)
	{
    if (pszOverFlowBuffer)
      delete [] pszOverFlowBuffer;

		ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_STAT_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
	}
	else
	{                                          
    //Parse out the Number of Mails and Total mail size values
		BOOL bGotNumber = FALSE;
    BOOL bGotTotal = FALSE;
		for (char* pszNum=sMessageBuf; (*pszNum!='\0') && (!bGotNumber || !bGotTotal); pszNum++)
		{
			if (*pszNum == '\t' || *pszNum== ' ')
			{						
				if (!bGotNumber)
				{
					nNumberOfMails = atoi(pszNum);
          m_nNumberOfMails = nNumberOfMails;
					bGotNumber = TRUE;
				}
				else
				{
					nTotalMailSize = atoi(pszNum);
					bGotTotal = TRUE;
				}
			}
		}
	}
  if (pszOverFlowBuffer)
    delete [] pszOverFlowBuffer;
}

BOOL CPJNPOP3Connection::ReadResponse(LPSTR pszBuffer, int nInitialBufSize, LPSTR pszTerminator, LPSTR* ppszOverFlowBuffer, int nGrowBy)
{
  ASSERT(ppszOverFlowBuffer);          //Must have a valid string pointer
  ASSERT(*ppszOverFlowBuffer == NULL); //Initially it must point to a NULL string

  //must have been created first
  ASSERT(m_bConnected);

  size_t nTerminatorLen = strlen(pszTerminator);

  //The local variables which will receive the data
  LPSTR pszRecvBuffer = pszBuffer;
  int nBufSize = nInitialBufSize;

  //retrieve the reponse using until we
	//get the terminator or a timeout occurs
	BOOL bFoundTerminator = FALSE;
	int nReceived = 0;
	while (!bFoundTerminator)
	{
		//check the socket for readability
    try
    {
		  if (!_IsReadible(m_dwTimeout)) //A timeout has occured so fail the function call
		  {
			  pszRecvBuffer[nReceived] = '\0';
			  m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
			  return FALSE;
		  }
    }
    catch(CWSocketException* pEx)
    {
      DWORD dwError = pEx->m_nError;
      pEx->Delete();
      ThrowPJNPOP3Exception(dwError, FACILITY_WIN32);
    }

		//receive the data from the socket
    int nBufRemaining = nBufSize-nReceived-1; //Allows allow one space for the NULL terminator
    if (nBufRemaining<0)
      nBufRemaining = 0;

    int nData = 0;
    try
    {
      nData = _Receive(pszRecvBuffer+nReceived, nBufRemaining);
    }
    catch(CWSocketException* pEx)
    {
      DWORD dwError = pEx->m_nError;
      pEx->Delete();
      ThrowPJNPOP3Exception(dwError, FACILITY_WIN32);
    }

		//Reset the idle timeout if data was received
		if (nData > 0)
		{
			//Increment the count of data received
			nReceived += nData;							   
		}

		//If an error occurred receiving the data
		if (nData < 1)
		{
			//NULL terminate the data received
			if (pszRecvBuffer)
			  pszRecvBuffer[nReceived] = '\0';
			m_sLastCommandResponse = pszRecvBuffer; //Hive away the last command reponse
			return FALSE; 
		}
		else
    {
      //NULL terminate the data received
      if (pszRecvBuffer)
		    pszRecvBuffer[nReceived] = '\0';
      if (nBufRemaining-nData == 0) //No space left in the current buffer
      {
				//Allocate the new receive buffer
				nBufSize += nGrowBy; //Grow the buffer by the specified amount
				LPSTR pszNewBuf = new char[nBufSize];

				//copy the old contents over to the new buffer and assign 
				//the new buffer to the local variable used for retreiving 
				//from the socket
				if (pszRecvBuffer)
				#if (_MSC_VER >= 1400)
				  strcpy_s(pszNewBuf, nBufSize, pszRecvBuffer);
				#else
				  strcpy(pszNewBuf, pszRecvBuffer);
				#endif
				pszRecvBuffer = pszNewBuf;

				//delete the old buffer if it was allocated
				if (*ppszOverFlowBuffer)
				  delete [] *ppszOverFlowBuffer;

				//Remember the overflow buffer for the next time around
				*ppszOverFlowBuffer = pszNewBuf;        
      }

      //Check to see if the terminator character(s) have been found
      bFoundTerminator = (strncmp(&pszRecvBuffer[nReceived - nTerminatorLen], pszTerminator, nTerminatorLen) == 0);
    }
	}

	//Remove the terminator from the response data
  pszRecvBuffer[nReceived - nTerminatorLen] = '\0';

  //Hive away the last command response
  m_sLastCommandResponse = pszRecvBuffer;

  return TRUE;
}

