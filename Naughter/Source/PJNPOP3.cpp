/*
Module : PJNPOP3.CPP
Purpose: Implementation for a MFC class encapsulation of the POP3 protocol
Created: PJN / 04-05-1998
History: PJN / 27-06-1998 1. Fixed a potential buffer overflow problem in Delete
                          and Retrieve functions when a large message number was
                          specified.
                          2. Improve the ReadResponse code by a) passing the 
                          readability check onto the scoket class and b) Sleeping
                          for 100 ms prior to looping around waiting for new 
                          response data
                          3. Classes are now fully Unicode compliant. Unicode
                          build configurations are also now included.
                          4. Now supports the TOP POP3 command which can be
                          issued using the GetHeader function.
         PJN / 04-01-1999 1. Properly UNICODE enabled the code
         PJN / 22-02-1999 1. Improved the reading of responses back from the server by implementing
                          a growable receive buffer
                          2. timeout is now 60 seconds when building for debug
                          3. Code now yields its time slice while waiting for a timeout
                          4. Made most functions virtual to help end user usage
         PJN / 25-03-1999 1. Fixed memory leak in the CPJNPOP3Connection::ReadReturnResponse function.
                          2. Now sleeps for 250 ms instead of yielding the time slice. This helps 
                          reduce CPU usage when waiting for data to arrive in the socket
         PJN / 15-06-1999 1. Added functions to return the message body, header or a particular
                          header field of a message
                          2. Tidied up some of the TRACE messages which the code generates
         PJN / 16-06-1999 1. Fixed a bug in the GetHeaderItem function which was causing a header
                          item which appeared at the begining of the header fields to fail to be 
                          parsed incorrectly.
         PJN / 27-06-1999 1. Fixed a bug in the GetHeaderItem function when a header spanned 
                          multiple lines as is allowed by RFC 822
         PJN / 29-06-1999 1. Another improvement to GetHeaderItem. When will it end <g>. Originally 
                          this was reported as a bug but upon further investigation it turns out that
                          the message which was causing the problems had embedded tabs in the header. 
                          This is discouraged by the RFC which refers to mail headers (RFC 822). 
                          The code has been enhanced to handle this case. Thanks to Chris Bishop 
                          for spotting this.
                          2. Fix for a bug in GetHeaderItem where I accidently was using "=" instead of
                          "==". Thanks to Angelini Fabio for spotting this.
         PJN / 05-07-1999 1. Addition of the following functions:

                          i)   CPJNPOP3Message::GetReplyTo
                          ii)  CPJNPOP3Message::GetRawBody      
                          iii) CPJNPOP3Message::GetSubject                 
	                        iv)  CPJNPOP3Message::GetFrom                    
	                        v)   CPJNPOP3Message::GetDate                    

                          2. GetHeaderItem function now uses case insensitive searching
                          3. GetHeaderItem now allows you to search for the "n'th" header of a specified type
         PJN / 24-08-1999 1. Fixed a bug whereby the function GetHeader was sometimes failing when it
                          was called when the message was retrieved using the "TOP" command.
         PJN / 27-03-2000 1. Fixed a problem where GetBody and GetRawBody will fail if you call it for a 
                          CPJNPOP3Message object that doesn't have a message in it (i.e m_pszMessage == NULL). 
                          This was previously causing a good old access violation in GetRawBody.
         PJN / 20-09-2000 1. Fixed a bug in CPJNPOP3Connection::UIDL when message id's were returned
                          with embedded dots in them. Thanks to Konstantin Vasserman for finding and resolving
                          this problem.
         PJN / 25-03-2001 1. Updated copyright information
         PJN / 27-07-2001 1. Added two methods namely: GetTo() and GetCC() to the CPJNPOP3Message class
         PJN / 11-08-2001 1. Fixed a bug in CPJNPOP3Connection::ReadResponse where determining the terminator
                          was failing when an embedded NULL was in the message.
         PJN / 27-09-2001 1. Fixed a bug in CPJNPOP3Connection::ReadResponse when handling disconnection errors
                          2. Improved the error handling in Delete, GetMessageSize & GetMessageID methods
         PJN / 29-09-2001 1. Further improved the error handling in CPJNPOP3Connection::ReadResponse
         PJN / 05-07-2003 1. Added copy constructors and operator= methods to the CPJNPOP3Message class. Thanks
                          to "Sten" for pointing out this omission.
                          2. Made destructors of all classes virtual.
         PJN / 21-07-2003 1. Class now preserves last command response across calls to Disconnect. This is 
                          especially important in calls to Connect when for example you get the POP3 error
                          "-Err. Can't lock the mailbox" or something like this when you call Connect. Thanks
                          to "Lev Elbert" for reporting this problem and providing the fix. 
         PJN / 03-11-2003 1. Simplified the code in CPJNPOP3Connection::ReadResponse. Thanks to Clarke Brunt for 
                          reporting this issue.
                          2. Reviewed the various trace statements in the code.
         PJN / 06-11-2003 1. Fixed a problem with an unitialized "m_pszMessage" in the CPJNPOP3Message 
                          constructor. Thanks to Michael Gunlock for reporting this problem.
         PJN / 12-11-2003 1. Fixed a bug in the CPJNPOP3Message operator= method. Thanks to Henrik Grek for 
                          reporting this bug.
         PJN / 05-06-2004 1. Fixed a bug in CPJNPOP3Connection::ReadResponse, where the wrong parameters
                          were being null terminated. Thanks to "caowen" for this update.
         PJN / 04-07-2004 1. Added an IsConnected() method to the CPJNPOP3Connection class. Thanks to Alessandro 
                          Rosa for this addition.
                          2. Added two new helper functions, namely CPJNPOP3Message::GetEmailAddress and 
                          CPJNPOP3Message::GetEmailFriendlyName. Thanks to Alessandro Rosa for this suggestion.
         PJN / 09-08-2004 1. Fixed a number of compiler warnings when the code is compiled with VC.Net 2003
         PJN / 27-03-2005 1. Fixed a number of compiler warnings in the sample app when the code is compiled
                          with VC .NET 2003.
                          2. Timeout now defaults to 5 seconds in debug mode.
                          3. Added support for connecting via Socks4, Socks5 and HTTP proxies
                          4. Added support for POP3 via SSL. Thanks to Serhiy Pavlov and Alexey Kuznetsov for
                          suggesting this update. Please note that because I do not have access to a mail 
                          server which provides POP3 over SSL, you should consider this support preliminary
                          until I get feedback from users of this feature. The standard POP3 support is still
                          fully functional and tested of course.
         PJN / 09-04-2005 1. Updated the initialization code for OpenSSL in the sample app to avoid memory 
                          leaks. Thanks to Alexey Kuznetsov for reporting these issues. You should implement
                          the same initialization and cleanup code in your applications. For details please check
                          the code in "main.cpp" module which is enclosed in #ifndef POP3_NOSSL_SUPPORT / 
                          #endif sections.
                          2. "List" function is now public instead of protected. Thanks to Alexey Kuznetsov for 
                          reporting these issues. 
                          3. Addition of a simple "FindMessageID" which returns the Message index given a message
                          ID as returned via UIDL. Thanks to Alexey Kuznetsov for this addition. 
         PJN / 08-02-2006 1. Fixed a /Wp64 compile issue in CPJNPOP3Connection::FindMessageID. Thanks to Alexey 
                          Kuznetsov for reporting this issue.    
                          2. Updated copyright details.
                          3. Renamed the classes to include a PJN prefix inline with my SMTP class.
                          4. Provided a PJNPOP3_EXT_CLASS to allow the class to be easily added to an extension dll
                          5. Changed the preprocessor macro to exclude SSL support to be CPJNPOP3_NOSSL. This is
                          to maintain consistency with my SMTP class.     
                          6. Changed the default release mode timeout to 60 seconds and the default debug mode 
                          timeout to 90 seconds.
         PJN / 13-02-2006 1. Fixed a graceful disconnect issue in CPJNPOP3Connection::ReadResponse.
         PJN / 09-07-2006 1. Optimized CPJNPOP3Message constructor code.
                          2. Code now uses new C++ style casts rather than old style C casts where necessary.
                          3. Optimized CPJNPOP3Connection constructor code
                          4. All classes now uses exceptions to indicate errors. This means the whole area of 
                          handling errors in the code is a whole lot simpler. You should carefully review your 
                          code as a lot of the return values from methods (especially CPJNPOP3Connection) are now void 
                          and will throw CPJNPOP3Exception's.
                          5. General rework of the class to bring it in line with the author's CPJNSMTPConnection code
                          6. Update the code in the sample app to TRACE out what it is doing
                          7. Updated the code to clean compile on VC 2005
         PJN / 15-11-2006 1. Minor update to the sample app to allow it to clean compile on VC 2005
                          2. Optimized ReadResponse method so that if an error occurs while reading the "RETR" 
                          response, only the actual command text is set into m_sLastCommandResponse instead of the 
                          whole message. This avoids a great performance hit on Unicode projects because of the
                          m_sLastCommandResponse CString nature (ANSI -> UNICODE conversion). Thanks to Alexey 
                          Kuznetsov for reporting this issue.
         PJN / 21-07-2007 1. Updated copyright details
                          2. CPJNPOP3Connection::Connect, Delete, Statistics, List, UIDL, Reset, Noop and GetMessageHeader
                          methods now catch CWSocketException exceptions and rethrow them as CPJNPOP3Exception exceptions. 
                          This allows client code to only expect CPJNPOP3Exception exceptions to be thrown from the class. 
                          Thanks to Alexey Kuznetsov for reporting this issue.
         PJN / 31-12-2007 1. Updated sample app to clean compile on VC 2008
                          2. CPJNPOP3Exception::GetErrorMessage now uses the FORMAT_MESSAGE_IGNORE_INSERTS flag. For more 
                          information please see Raymond Chen's blog at 
                          http://blogs.msdn.com/oldnewthing/archive/2007/11/28/6564257.aspx. Thanks to Alexey Kuznetsov for 
                          reporting this issue.
                          3. CPJNPOP3Exception::GetErrorMessage now uses Checked::tcsncpy_s if compiled using VC 2005 or 
                          later.
                          4. Sample app shipped with download is now a VC 2005 solution
         PJN / 18-03-2008 1. Updated copyright details
                          2. The code now only supports VC 2005 or later.
                          3. Simplified the response parsing considerably through the usage of CStringA
                          4. Fixed logic which terminates the parsing loop in ReadListResponse
                          5. Improved the robustness of the ReadUIDLResponse parsing logic
                          6. Now includes support for APOP authentication. Thanks to "goodsiwen" for prompting this update.
         PJN / 18-05-2008 1. GetHeaderItem now uses a CStringA parameter instead of CString. Thanks to Alexey Kuznetsov for
                          reporting this issue.
         PJN / 31-05-2008 1. Code now compiles cleanly using Code Analysis (/analyze)
         PJN / 14-06-2008 1. Updated code to compile correctly using _ATL_CSTRING_EXPLICIT_CONSTRUCTORS define
         PJN / 04-05-2009 1. Updated copyright details.
                          2. Updated the sample app's project settings to more modern default values.
                          3. Removed the parsing methods from the CPJNPOP3Message class. For comprehensive support for 
                          message parsing please take a look at CDO for Windows 2000.
                          4. Extensively reworked the parsing code to use CByteArray derived classes to hold parsed 
                          responses instead of CStringA instances. This handles the unusual but legal situation where a 
                          message contains embedded nulls. Thanks to Sergey Lysenko for reporting this issue.
                          5. Fixed a bug in ReadUIDLResponse when parsing the message ID
                          6. The code in Connect now correctly validates that a timestamp has been provided if doing APOP
                          authentication
         PJN / 22-12-2009 1. Optimized the ReadResponse logic to reduce the amount of searching required to find the terminator 
                          in the received buffer. This helps avoid excessive CPU utilization when we receive large responses.
                          Thanks to Alexey Rabets for reporting this issue.
                          2. The size of the read buffer used in the ReadResponse method can now be specified at runtime via 
                          SetReadBufferSize().
                          3. Updated the documentation to fix various out of date details in the API reference section.
         PJN / 13-02-2011 1. Updated copyright details
                          2. Updated sample app to clean compile on VC 2010
                          3. Updated code to support latest SSL and Sockets class from the author. This means that the code now supports IPv6 POP3 servers
                          4. The sample app is now linked against the latest OpenSSL v1.0.0c dlls
                          5. Set/GetBoundAddress have been renamed Set/GetBindAddress for consistency with the sockets class

Copyright (c) 1998 - 2011 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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
#include "PJNPOP3.h"
#include "PJNMD5.h" //If you get a compilation error about this missing header file, then you need to download my MD5 wrapper classes from http://www.naughter.com/smtp.html
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////// Implementation ///////////////////////////////////////////////

//A version of CStringA which supports secure disposal
class CPJNPOP3SecureStringA : public CStringA
{
public:
//Constructors / Destructors
  CPJNPOP3SecureStringA()
  {
  }
  CPJNPOP3SecureStringA(LPCWSTR pszValue) : CStringA(pszValue)
  {
  }
  CPJNPOP3SecureStringA(LPCSTR pszValue) : CStringA(pszValue)
  {
  }
  ~CPJNPOP3SecureStringA()
  {
    SecureEmpty();
  }

//Methods
  __forceinline void SecureEmpty()
  {
    int nLength = GetLength();
    if (nLength)
    {
      LPSTR pszVal = GetBuffer(nLength);
      SecureZeroMemory(pszVal, nLength);
      ReleaseBuffer();
    } 
  }
};


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
  //Validate our parameters
	ASSERT(pstrError != NULL && AfxIsValidString(pstrError, nMaxError));

	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  if (HRESULT_FACILITY(m_hr) == FACILITY_ITF)
  {
    bSuccess = TRUE;
  
    //Simply load up the string from the string table
    CString sError;
    AfxFormatString1(sError, HRESULT_CODE(m_hr), m_sLastResponse);
    Checked::tcsncpy_s(pstrError, nMaxError, sError, _TRUNCATE);  
  }
  else
  {
  	LPTSTR lpBuffer;
	  DWORD dwReturn = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			                             NULL, HRESULT_CODE(m_hr), MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			                             reinterpret_cast<LPTSTR>(&lpBuffer), 0, NULL);

	  if (dwReturn == 0)
		  *pstrError = _T('\0');
	  else
	  {
	    bSuccess = TRUE;
      Checked::tcsncpy_s(pstrError, nMaxError, lpBuffer, _TRUNCATE);  
		  LocalFree(lpBuffer);
	  }
  }

	return bSuccess;
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

	dc << _T("m_hr = ") << m_hr << _T("\n");
}
#endif


CPJNPOP3Connection::CPJNPOP3Connection() : m_nNumberOfMails(0),
                                           m_bListRetrieved(FALSE),
                                           m_bStatRetrieved(FALSE),
                                           m_bUIDLRetrieved(FALSE),
                                           m_bConnected(FALSE),
                                           m_bSSL(FALSE),
                                           m_ProxyType(ptNone),
                                           m_nProxyPort(1080),
                                           m_dwReadBufferSize(1024)
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
    //Deliberately handle exceptions here, so that the destructor does not throw exceptions
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

	TRACE(_T("Warning: throwing CPJNPOP3Exception for error %08X\n"), pException->m_hr);
	THROW(pException);
}

void CPJNPOP3Connection::ThrowPJNPOP3Exception(HRESULT hr, const CString& sLastResponse)
{
	CPJNPOP3Exception* pException = new CPJNPOP3Exception(hr, sLastResponse);

	TRACE(_T("Warning: throwing CPJNPOP3Exception for error %08X\n"), pException->m_hr);
	THROW(pException);
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
	m_Socket.Create();
	m_Socket.SetBindAddress(m_sBindAddress);

#ifndef CPJNPOP3_NOSSL
	if (m_bSSL) 
  {
		if(!m_SSL.ConnectViaSocks4(lpszHostAddress, nHostPort, lpszSocksServer, nSocksPort, dwConnectionTimeout)) 
			ThrowPJNPOP3Exception(IDS_PJNPOP3_FAIL_CONNECT_SOCKS4_VIASSL, FACILITY_ITF, GetOpenSSLError());
	}
	else 
#endif
  {
    m_Socket.SetBindAddress(m_sBindAddress);
		m_Socket.ConnectViaSocks4(lpszHostAddress, nHostPort, lpszSocksServer, nSocksPort, dwConnectionTimeout);
  }
}

void CPJNPOP3Connection::_ConnectViaSocks5(LPCTSTR lpszHostAddress, UINT nHostPort , LPCTSTR lpszSocksServer, UINT nSocksPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword, DWORD dwConnectionTimeout, BOOL bUDP)
{
#ifndef CPJNPOP3_NOSSL
	if (m_bSSL) 
  {
		m_SSLCtx.Attach(SSL_CTX_new(SSLv23_client_method()));
		if (!m_SSL.Create(m_SSLCtx, m_Socket))
      ThrowPJNPOP3Exception(IDS_PJNPOP3_FAIL_CREATE_SSL_SOCKET, FACILITY_ITF, GetOpenSSLError());

		if (!m_SSL.ConnectViaSocks5(lpszHostAddress, nHostPort, lpszSocksServer, nSocksPort, lpszUserName, lpszPassword, dwConnectionTimeout, bUDP)) 
			ThrowPJNPOP3Exception(IDS_PJNPOP3_FAIL_CONNECT_SOCKS5_VIASSL, FACILITY_ITF, GetOpenSSLError());
	}
	else 
#endif
  {
    m_Socket.SetBindAddress(m_sBindAddress);
		m_Socket.ConnectViaSocks5(lpszHostAddress, nHostPort, lpszSocksServer, nSocksPort, lpszUserName, lpszPassword, dwConnectionTimeout, bUDP);
  }
}

void CPJNPOP3Connection::_ConnectViaHTTPProxy(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszHTTPServer, UINT nHTTPProxyPort, CStringA& sProxyResponse, 
                                              LPCTSTR lpszUserName, LPCTSTR pszPassword, DWORD dwConnectionTimeout, LPCTSTR lpszUserAgent)
{
#ifndef CPJNPOP3_NOSSL
	if (m_bSSL) 
  {
		m_SSLCtx.Attach(SSL_CTX_new(SSLv23_client_method()));
		if (!m_SSL.Create(m_SSLCtx, m_Socket))
      ThrowPJNPOP3Exception(IDS_PJNPOP3_FAIL_CREATE_SSL_SOCKET, FACILITY_ITF, GetOpenSSLError());

		if (!m_SSL.ConnectViaHTTPProxy(lpszHostAddress, nHostPort, lpszHTTPServer, nHTTPProxyPort, sProxyResponse, lpszUserName, pszPassword, dwConnectionTimeout, lpszUserAgent))
			ThrowPJNPOP3Exception(IDS_PJNPOP3_FAIL_CONNECT_HTTPPROXY_VIASSL, FACILITY_ITF, GetOpenSSLError());
	}
	else 
#endif
  {
    m_Socket.SetBindAddress(m_sBindAddress);
		m_Socket.ConnectViaHTTPProxy(lpszHostAddress, nHostPort, lpszHTTPServer, nHTTPProxyPort, sProxyResponse, lpszUserName, pszPassword, dwConnectionTimeout, lpszUserAgent);
  }
}

void CPJNPOP3Connection::_Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
#ifndef CPJNPOP3_NOSSL
	if (m_bSSL) 
  {
		m_SSLCtx.Attach(SSL_CTX_new(SSLv23_client_method()));
		if (!m_SSL.Create(m_SSLCtx, m_Socket))
      ThrowPJNPOP3Exception(IDS_PJNPOP3_FAIL_CREATE_SSL_SOCKET, FACILITY_ITF, GetOpenSSLError());

		if (!m_SSL.Connect(lpszHostAddress, nHostPort)) 
			ThrowPJNPOP3Exception(IDS_PJNPOP3_FAIL_CONNECT_VIASSL, FACILITY_ITF, GetOpenSSLError());
	}
	else 
#endif
  {
    m_Socket.SetBindAddress(m_sBindAddress);
		m_Socket.CreateAndConnect(lpszHostAddress, nHostPort);
  }
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
void CPJNPOP3Connection::Connect(LPCTSTR pszHostName, LPCTSTR pszUsername, LPCTSTR pszPassword, int nPort, AuthenticationMethod am, BOOL bSSL)
#else
void CPJNPOP3Connection::Connect(LPCTSTR pszHostName, LPCTSTR pszUsername, LPCTSTR pszPassword, int nPort, AuthenticationMethod am)
#endif
{
  //Validate our parameters
#ifndef CPJNPOP3_NOSSL
  m_bSSL = bSSL;
#else
  m_bSSL = FALSE;
#endif

  try
  {
    //Connect to the POP3 server
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
        CStringA sProxyResponse;
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

  //At this point, we're now connected
  m_bConnected = TRUE;

  try
  {
    //check the response
    CString sTimeStampBanner;
    CString sResponse;
    if (!ReadCommandResponse(sResponse))
      ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_POP3_LOGIN_RESPONSE, FACILITY_ITF, GetLastCommandResponse());
    else
    {
      //Hive away the timestamp banner
      int nFind1 = sResponse.Find(_T("<"));
      int nFind2 = sResponse.Find(_T(">"));
      if ((nFind2 != -1) && (nFind1 != -1) && (nFind2 > nFind1))
        sTimeStampBanner = sResponse.Mid(nFind1, nFind2 - nFind1 + 1);
    }
    
    //Form ascii versions of the username and password parameters
    CPJNPOP3SecureStringA sAsciiUsername(pszUsername);
    CPJNPOP3SecureStringA sAsciiPassword(pszPassword);

    //Issue the correct sequence of commands depending on the authentication scheme 
    if (am == AUTH_PLAIN)
    {
      //Send the POP3 username and check the response
      CPJNPOP3SecureStringA sRequest;
      sRequest.Format("USER %s\r\n", sAsciiUsername.operator LPCSTR());
      _Send(sRequest.operator LPCSTR(), sRequest.GetLength());
      if (!ReadCommandResponse())
        ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_POP3_LOGIN_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 

      //Send the POP3 password and check the response
      sRequest.Format("PASS %s\r\n", sAsciiPassword.operator LPCSTR());
      _Send(sRequest.operator LPCSTR(), sRequest.GetLength());
      if (!ReadCommandResponse())
        ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_POP3_LOGIN_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
    }
    else
    {
      ASSERT(am == AUTH_APOP);

      //Check that the server provided us a timestamp to hash
      if (sTimeStampBanner.IsEmpty())
        ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_POP3_LOGIN_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
      
      //Form the string which we need to MD5 hash
      CPJNPOP3SecureStringA sToHash(sTimeStampBanner);
      sToHash += pszPassword;
      
      //Calculate the MD5 hash
      CPJNMD5Hash hash;
      CPJNMD5 md5;
      if (!md5.Hash(reinterpret_cast<const BYTE*>(sToHash.operator LPCSTR()), sToHash.GetLength(), hash))
        ThrowPJNPOP3Exception(GetLastError(), FACILITY_WIN32);
      
      //Issue the APOP command and check the response
      CPJNPOP3SecureStringA sAPOP(hash.Format(FALSE));
      CPJNPOP3SecureStringA sRequest;
      sRequest.Format("APOP %s %s\r\n", sAsciiUsername.operator LPCSTR(), sAPOP.operator LPCSTR());
      _Send(sRequest, sRequest.GetLength());
      if (!ReadCommandResponse())
        ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_POP3_LOGIN_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
    }
  }
  catch(CWSocketException* pEx)
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
  
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNPOP3Exception(dwError, FACILITY_WIN32);
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
    CString sLastResponse(pEx->m_sLastResponse);
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
      static char* sBuf = "QUIT\r\n";
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
  //Must be connected to perform a delete
  ASSERT(m_bConnected);

  //if we haven't executed the LIST command then do it now
  if (!m_bListRetrieved)
    List();

  //Send the DELE command along with the message ID
	CStringA sRequest;
 	sRequest.Format("DELE %d\r\n", nMsg);
  try
  {
	  _Send(sRequest.operator LPCSTR(), sRequest.GetLength());
	}
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNPOP3Exception(dwError, FACILITY_WIN32);
  }

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
  try
  {
	  _Send(sBuf, nCmdLength);
	}
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNPOP3Exception(dwError, FACILITY_WIN32);
  }

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

INT_PTR CPJNPOP3Connection::FindMessageID(const CString& sID)
{
  //Validate our parameters
  ASSERT(!sID.IsEmpty());

  //if we haven't executed the UIDL command then do it now
  if (!m_bUIDLRetrieved)
    UIDL();

  //find message ID
  INT_PTR nMsgIds = m_msgIDs.GetSize();
  for (INT_PTR i=0; i<nMsgIds; i++)
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
  try
  {
	  _Send(sBuf, nCmdLength);
	}
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNPOP3Exception(dwError, FACILITY_WIN32);
  }

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
  try
  {
	  _Send(sBuf, nCmdLength);
	}
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNPOP3Exception(dwError, FACILITY_WIN32);
  }

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
  try
  {
  	_Send(sBuf, nCmdLength);
	}
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNPOP3Exception(dwError, FACILITY_WIN32);
  }

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
  try
  {
	  _Send(sBuf, nCmdLength);
	}
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNPOP3Exception(dwError, FACILITY_WIN32);
  }

  //And check the response
	if (!ReadCommandResponse())
    ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_POP3_NOOP_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
}

void CPJNPOP3Connection::Retrieve(int nMsg, CPJNPOP3Buffer& message)
{
  //Must be connected to retrieve a message
  ASSERT(m_bConnected);

  //work out the size of the message to retrieve
  DWORD dwSize = GetMessageSize(nMsg);

  //Send the RETR command
	CStringA sRequest;
	sRequest.Format("RETR %d\r\n", nMsg);	
  try
  {
	  _Send(sRequest.operator LPCSTR(), sRequest.GetLength());
	}
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNPOP3Exception(dwError, FACILITY_WIN32);
  }
  
	//And check the command
	ReadReturnResponse(message, dwSize);
}

void CPJNPOP3Connection::GetMessageHeader(int nMsg, CPJNPOP3Buffer& message)
{
  //Must be connected to retrieve a message
  ASSERT(m_bConnected);

  //make sure the message actually exists
  DWORD dwSize = GetMessageSize(nMsg);

  //Send the TOP command
  CStringA sRequest;
  sRequest.Format("TOP %d 0\r\n", nMsg);
  try
  {
    _Send(sRequest.operator LPCSTR(), sRequest.GetLength());
	}
  catch(CWSocketException* pEx)
  {
    DWORD dwError = pEx->m_nError;
    pEx->Delete();
    ThrowPJNPOP3Exception(dwError, FACILITY_WIN32);
  }

  //And check the command
  ReadReturnResponse(message, dwSize);
}

BOOL CPJNPOP3Connection::ReadCommandResponse()
{
  //Delegate to the real implementation
  CString sResponse;
  return ReadCommandResponse(sResponse);
}

BOOL CPJNPOP3Connection::ReadCommandResponse(CString& sResponse)
{
  //Read the response
  CPJNPOP3Buffer response;
  response.SetSize(0, 1024);
  BOOL bSuccess = ReadResponse(response, "\r\n", FALSE);
  sResponse = response.AsString();
  
  //determine if the response is an error
  if (bSuccess)
	  bSuccess = (response.Find("+OK") == 0);

  return bSuccess;
}

CPJNPOP3Buffer CPJNPOP3Connection::GetBodyOfResponse(const CPJNPOP3Buffer& response) const
{
  //What will be the return value from this function
  CPJNPOP3Buffer body;

  INT_PTR nFind = response.Find("\n");
  if (nFind != -1)
  {
    INT_PTR nBodySize = response.GetSize() - nFind - 1;
    if (nBodySize)
    {
      body.SetSize(nBodySize);
      BYTE* pBody = body.GetData();
      ASSERT(pBody);
      const BYTE* pResponse = response.GetData();
      memcpy(pBody, &(pResponse[nFind + 1]), nBodySize);
    }
  }
  
  return body;
}

void CPJNPOP3Connection::ReadReturnResponse(CPJNPOP3Buffer& message, DWORD dwSize)
{
  //Must be connected to perform a "RETR"
  ASSERT(m_bConnected);

  //Retrieve the message body
  CPJNPOP3Buffer response;
  response.SetSize(0, dwSize + 4096);
  BOOL bReadResponse = ReadResponse(response, "\r\n.\r\n", TRUE);
  if (!bReadResponse)
		ThrowPJNPOP3Exception(IDS_PJNPOP3_FAILED_RECEIVE_RETR_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 

  //determine if the response is an error
  if (response.Find("+OK") != 0)
    ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_RETR_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 

  //remove the first line which contains the +OK from the message and put it into the message
  message = GetBodyOfResponse(response);
  message.SetSize(message.GetSize() - 5); //Throw away the last "\r\n.\r\n" part of the response 
}

void CPJNPOP3Connection::ReadListResponse(int nNumberOfMails)
{
  //Must be connected to perform a "LIST"
  ASSERT(m_bConnected);

  //retrieve the reponse
  CPJNPOP3Buffer response;
  response.SetSize(0, (14 * nNumberOfMails) + 100);
  if (!ReadResponse(response, "\r\n.\r\n", FALSE))
		ThrowPJNPOP3Exception(IDS_PJNPOP3_FAILED_RECEIVE_LIST_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 

  //determine if the response is an error
  if (response.Find("+OK") != 0)
    ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_LIST_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 

  //Retrieve the message sizes and put them
  //into the m_msgSizes array
  m_msgSizes.RemoveAll();
  m_msgSizes.SetSize(0, nNumberOfMails);

  //then parse the LIST response
  CString sBody(GetBodyOfResponse(response).AsString());
  if (sBody.IsEmpty())
    ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_LIST_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
	LPCTSTR pszSize = sBody.operator LPCTSTR();
	for (; (*pszSize != '.' && *pszSize != '\0'); pszSize++)
	{
		if (*pszSize == _T('\t') || *pszSize == _T(' '))
			m_msgSizes.Add(_ttoi(pszSize));
  }
}

void CPJNPOP3Connection::ReadUIDLResponse(int nNumberOfMails)
{
  //Must be connected to perform a "UIDL"
  ASSERT(m_bConnected);

  //retrieve the reponse
  CPJNPOP3Buffer response;
  response.SetSize(0, (14 * nNumberOfMails) + 100);
  if (!ReadResponse(response, "\r\n.\r\n", FALSE))
		ThrowPJNPOP3Exception(IDS_PJNPOP3_FAILED_RECEIVE_UIDL_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 

  //determine if the response is an error
  if (response.Find("+OK") != 0)
    ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_UIDL_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 

  //Reset the message id array 
  m_msgIDs.SetSize(0, nNumberOfMails);

	//then parse the UIDL response
	CString sBody(GetBodyOfResponse(response).AsString());
	if (sBody.IsEmpty())
	  ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_UIDL_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
	LPTSTR pszSize = sBody.GetBuffer(sBody.GetLength());
	for (int iCount=0; iCount<nNumberOfMails; iCount++)
	{
	  //Find the next line, before we attempt to parse the current one
	  LPTSTR pszBegin = pszSize;
	  while (*pszSize != _T('\n') && *pszSize != _T('\0'))
			++pszSize;
			
    //Convert the EOL to a NULL so that we can do easier parsing of the message ids
    if (*pszSize == _T('\n'))
      *pszSize = _T('\0');
      
    //Find the first whitespace character in the line (which means that we have skipped over the message number)  
	  TCHAR* pszID = pszBegin;
	  while (*pszID != _T('\0') && *pszID != _T(' '))
	    ++pszID;
	    
	  //Then find the first non-whitespace character in the line (which is the start of the message id)
	  while (*pszID != _T('\0') && *pszID == _T(' '))
	    ++pszID;
	    
	  //Then find the first whitespace or EOL character in the line (which is the end of the message id)
	  TCHAR* pszEndID = pszID;
	  while (*pszEndID != _T('\0') && *pszEndID != _T(' ') && *pszEndID != _T('\r') && *pszEndID != _T('\n'))
	    ++pszEndID;
	  *pszEndID = _T('\0');
	  
	  //If we could not find the begining of the ID or the ID is empty, then we were given a bad resposne
	  if (pszID == _T('\0') || (_tcslen(pszID) == 0))
	    ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_UIDL_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
	    
	  //Finally form the string from the ID
	  m_msgIDs.Add(pszID);
	  pszSize++;
	}
	sBody.ReleaseBuffer();
	
	//If we have not got the same amount of elements in m_msgIDs compared to the nNumberOfMails, then we were given a bad response
	if (m_msgIDs.GetSize() != nNumberOfMails)
	  ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_UIDL_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
}

void CPJNPOP3Connection::ReadStatResponse(int& nNumberOfMails, int& nTotalMailSize)
{
  //Must be connected to perform a "STAT"
  ASSERT(m_bConnected);

  //retrieve the reponse
  CPJNPOP3Buffer response;
  response.SetSize(0, 128);
  if (!ReadResponse(response, "\r\n", FALSE))
  	ThrowPJNPOP3Exception(IDS_PJNPOP3_FAILED_RECEIVE_STAT_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 

  //determine if the response is an error
  if (response.Find("+OK") != 0)
		ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_STAT_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 

  //Parse out the "Number of Mails" and "Total mail size" values
  CString sResponse(response.AsString());
  int nFindIndex = 3;
  CString sNumberOfMails(sResponse.Tokenize(_T(" \t"), nFindIndex));
  if ((nFindIndex == -1) || sNumberOfMails.IsEmpty())
    ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_STAT_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
	nNumberOfMails = _ttoi(sNumberOfMails);
  m_nNumberOfMails = nNumberOfMails;

  CString sTotalMailSize(sResponse.Tokenize(_T(" \t\r\n"), nFindIndex));
  if (sTotalMailSize.IsEmpty())
    ThrowPJNPOP3Exception(IDS_PJNPOP3_UNEXPECTED_STAT_RESPONSE, FACILITY_ITF, GetLastCommandResponse()); 
	nTotalMailSize = _ttoi(sTotalMailSize);
}

BOOL CPJNPOP3Connection::ReadResponse(CPJNPOP3Buffer& response, LPSTR pszTerminator, BOOL bCalledForReturnResponse)
{
  //must have been created first
  ASSERT(m_bConnected);

  //Allocate a buffer to read into from the socket
	ATL::CHeapPtr<BYTE> readBuffer;
	if (!readBuffer.Allocate(m_dwReadBufferSize))
	  ThrowPJNPOP3Exception(ERROR_OUTOFMEMORY, FACILITY_WIN32);

  //retrieve the response using until we get the terminator or a timeout occurs
	BOOL bFoundTerminator = FALSE;
	size_t nTerminatorLen = strlen(pszTerminator);
	while (!bFoundTerminator)
	{
		//check the socket for readability
    try
    {
		  if (!_IsReadible(m_dwTimeout)) //A timeout has occured so fail the function call
		  {
			  //Hive away the last command response
        if (bCalledForReturnResponse)
	        SetReturnLastCommandResponse(response);
        else
	        m_sLastCommandResponse = response.AsString();
	        
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
	  int nData = 0;
    try
    {
      nData = _Receive(readBuffer.m_pData, m_dwReadBufferSize);
    }
    catch(CWSocketException* pEx)
    {
      DWORD dwError = pEx->m_nError;
      pEx->Delete();
      ThrowPJNPOP3Exception(dwError, FACILITY_WIN32);
    }
    
    //Handle a graceful disconnect
    if (nData == 0)
      ThrowPJNPOP3Exception(IDS_PJNPOP3_GRACEFUL_DISCONNECT, FACILITY_ITF, GetLastCommandResponse());

    //Add the data to the complete response
    response.Append(readBuffer.m_pData, nData);    
    
    //Optimize the location where we search for the terminator in the received buffer. This helps 
    //avoid excessive CPU utilization when we receive large responses
    int nStartSearch = static_cast<int>(response.GetSize() - nData - nTerminatorLen + 1);
    if (nStartSearch < 0)
      nStartSearch = 0;

    //Check to see if the terminator character(s) have been found
    bFoundTerminator = (response.Find(pszTerminator, nStartSearch) != -1);
	}

  //Hive away the last command response
  if (bCalledForReturnResponse)
	  SetReturnLastCommandResponse(response);
  else  
	  m_sLastCommandResponse = response.AsString();

  return TRUE;
}

void CPJNPOP3Connection::SetReturnLastCommandResponse(const CPJNPOP3Buffer& response)
{
  //Only copy up to the first EOL if present
  INT_PTR nFind = response.Find("\r");
  if (nFind != -1)
    m_sLastCommandResponse = response.Left(nFind).AsString();
	else 
	  m_sLastCommandResponse.Empty();
}
