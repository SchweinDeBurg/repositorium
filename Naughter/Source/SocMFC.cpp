/*
Module : SocMFC.cpp
Purpose: Implementation for an MFC wrapper class for sockets
Created: PJN / 05-08-1998
History: 03-03-2003 1. Addition of a number of preprocessor defines, namely W3MFC_EXT_CLASS, 
                    THRDPOOL_EXT_CLASS and SOCKMFC_EXT_CLASS. This allows the classes to easily 
                    be added and exported from a MFC extension dll.
                    2. Now implements support for connecting via Socks 4 and Socks 5 proxies
         21-09-2003 1. Now supports UDP sockets.
                    2. Now supports UDP relaying via Socks 5 proxy.
         26-09-2003 1. Now supports connection via HTTP proxies which support the CONNECT verb
         13-01-2004 1. Used newer form of #pragma pack to avoid problems with non standard 
                    packing sizes.
         25-10-2004 1. Updated to compile cleanly when Detect 64 bit issues and Force conformance 
                    in for loop options are enabled in Visual Studio .NET
         29-12-2004 Almost all of the following updates were to match the functionality provided
                    by the MFC CAsyncSocket class but without the overhead of hidden windows and 
                    its async behaviour.
                    1. Now automatically links to Winsock via #pragma comment
                    2. Addition of a GetPeerName method.
                    3. Replaced all calls to ZeroMemory to memset.
                    4. Addtion of a GetSockName method.
                    5. Addition of a SetSockOpt method.
                    6. Addition of a Flags parameter to Receive method.
                    7. Addition of a IOCtl method.
                    8. Optimized the code in Listen.
                    9. Addition of a ReceiveFrom method.
                    10. Addition of a ShutDown method.
                    11. Optimized the code in Close.
                    12. Remove of pszLocalBoundAddress parameter from Connect methods to make it 
                    consistent with CAsyncSocket.
                    13. Addition of a Flags parameter to Send method.
                    14. Optimized code in CWSocket destructor
                    15. Addition of an overloaded Create method which allows all of the socket
                    parameters to be set
                    16. Use of _tcslen has been minimized when NULL string parameters can be passed
                    to various CWSocket methods.
                    17. Change of various parameter names to be consistent with names as used in
                    CAsyncSocket.
         31-01-2005 1. Fixed a bug in CWSocket::Receive where it throws an error when a graceful 
                    disconnect occurs. Now the code only throws an exception if the return value
                    from recv is SOCKET_ERROR
         01-05-2005 1. Send method now uses a const void* parameter.
         21-06-2005 1. Provision of connect methods which allows a timeout to be specified. Please note
                    that if you use a host name in these calls as opposed to an IP address, the DNS
                    lookup is still done using the OS supplied timeout. Only the actual connection
                    to the server is implemented using a timeout after the DNS lookup is done (if it
                    is necessary).
         04-11-2005 1. Send method now returns the number of bytes written. Thanks to Owen O'Flaherty
                    for pointing out this omission.
         19-02-2006 1. Replaced all calls to ZeroMemory and CopyMemory with memset and memcpy
         27-06-2006 1. Updated copyright details.
                    2. Made ThrowWSocketException part of CWSocket class and renamed to 
                    ThrowWSocketException.
                    3. CWSocketException::GetErrorMessage now uses safestring functionality.
                    4. Optimized CWSocketException constructor code.
                    5. Removed unnecessary CWSocketException destructor
                    6. Code now uses new C++ style casts rather than old style C casts where necessary. 
         19-11-2007 1. Updated copyright details.
         26-12-2007 1. CWSocketException::GetErrorMessage now uses the FORMAT_MESSAGE_IGNORE_INSERTS flag. 
                    For more information please see Raymond Chen's blog at 
                    http://blogs.msdn.com/oldnewthing/archive/2007/11/28/6564257.aspx. Thanks to Alexey 
                    Kuznetsov for reporting this issue.
                    2. All username and password temp strings are now securely destroyed using 
                    SecureZeroMemory. This version of the code and onwards will be supported only
                    on VC 2005 or later.
         27-12-2007 1. CWSocketException::GetErrorMessage now uses Checked::tcsncpy_s similiar to the 
                    built in MFC exception classes
         31-12-2007 1. Minor coding updates to CWSocketException::GetErrorMessage
         02-02-2008 1. Updated copyright details.
                    2. Fixed potential heap memory leaks in CWSocket::ReadHTTPProxyResponse.Thanks to 
                    Michal Urbanczyk for reporting this bug.
                    3. Fixed a memory leak in CWSocket::ConnectViaSocks5
                    4. Restructured CWSocket::ReadSocks5ConnectReply to avoid the need to allocate 
                    heap memory
         01-03-2008 1. Since the code is now for VC 2005 or later only, the code now uses the Base64 
                    encoding support from the ATL atlenc.h header file. Thanks to Mat Berchtold for 
                    reporting this optimization. This means that client projects no longer need to include 
                    Base64.cpp/h in their projects.
         31-05-2008 1. Code now compiles cleanly using Code Analysis (/analyze)
                    2. Tidied up the CWSocket::ReadHTTPProxyResponse implementation
         23-05-2009 1. Removed use of CT2A throughout the code

Copyright (c) 2002 - 2009 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////// Includes //////////////////////////////////////////////////

#include "stdafx.h"
#include "SocMFC.h"

#ifndef __ATLENC_H__
#pragma message("To avoid this message, please put atlenc.h in your pre compiled header (usually stdafx.h)")
#include <atlenc.h>
#endif

#if defined(__INTEL_COMPILER)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
// remark #174: expression has no effect
#pragma warning(disable: 174)
// remark #593: variable was set but never used
#pragma warning(disable: 593)
// remark #383: value copied to temporary, reference to temporary used
#pragma warning(disable: 383)
// remark #981: operands are evaluated in unspecified order
#pragma warning(disable: 981)
#endif	// __INTEL_COMPILER

/////////////////// Macros / Defines //////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "wsock32.lib")


///////////////// Implementation //////////////////////////////////////////////

#pragma pack(push, 1)
struct WSOCKET_SOCK4_CONNECT_REQUEST
{
  BYTE    VN;
  BYTE    CD;
  WORD    DSTPORT;
  in_addr DSTIP;
  BYTE    USERID[1];
};

struct WSOCKET_SOCKS4_CONNECT_REPLY
{
  BYTE    VN;
  BYTE    CD;
  WORD    DSTPORT;
  in_addr DSTIP;
};

struct WSOCKET_SOCKS5_IDENTIFIER_PACKET
{
  BYTE VER;
  BYTE NMETHODS;
  BYTE METHODS[255];
};

struct WSOCKET_SOCKS5_METHODSELECTION_MESSAGE
{
  BYTE VER;
  BYTE METHOD;
};

struct WSOCKET_SOCKS5_BASE_REQUEST_DETAILS
{
  BYTE VER;
  BYTE CMD;
  BYTE RSV;
  BYTE ATYP;
};

struct WSOCKET_SOCKS5_IP4_REQUEST_DETAILS
{
  WSOCKET_SOCKS5_BASE_REQUEST_DETAILS Base;
  in_addr                     DST_IP;
  WORD                        DSTPORT;
};

struct WSOCKET_SOCKS5_HOST_DETAILS
{
  BYTE LENGTH;
  BYTE HOST[255];
};

struct WSOCKET_SOCKS5_HOSTNAME_REQUEST_DETAILS
{
  WSOCKET_SOCKS5_BASE_REQUEST_DETAILS Base;
  WSOCKET_SOCKS5_HOST_DETAILS         DST_HOST;
  WORD                                DSTPORT;
};

struct WSOCKET_SOCKS5_USERNAME_AUTHENTICATION_REPLY
{
  BYTE VER;
  BYTE STATUS;
};
#pragma pack(pop)


//////////////////////////////// Implementation ///////////////////////////////

BOOL CWSocketException::GetErrorMessage(LPTSTR pstrError, UINT nMaxError, PUINT pnHelpContext)
{
  //Validate our parameters
	ASSERT(pstrError != NULL && AfxIsValidString(pstrError, nMaxError));

	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

	LPTSTR lpBuffer;
	DWORD dwReturn = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			                           NULL,  m_nError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			                           reinterpret_cast<LPTSTR>(&lpBuffer), 0, NULL);
	if (dwReturn == 0)
		*pstrError = _T('\0');
	else
	{
    bSuccess = TRUE;
	  Checked::tcsncpy_s(pstrError, nMaxError, lpBuffer, _TRUNCATE);
		LocalFree(lpBuffer);
	}

	return bSuccess;
}

CString CWSocketException::GetErrorMessage()
{
  CString rVal;
  LPTSTR pstrError = rVal.GetBuffer(4096);
  GetErrorMessage(pstrError, 4096, NULL);
  rVal.ReleaseBuffer();
  return rVal;
}

CWSocketException::CWSocketException(int nError) : m_nError(nError)
{
}

IMPLEMENT_DYNAMIC(CWSocketException, CException)

#ifdef _DEBUG
void CWSocketException::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << _T("m_nError = ") << m_nError << _T("\n");
}
#endif


CWSocket::CWSocket() : m_hSocket(INVALID_SOCKET)
{
}

CWSocket::~CWSocket()
{
  if (m_hSocket != INVALID_SOCKET)
    Close();
}

void CWSocket::ThrowWSocketException(int nError)
{
	if (nError == 0)
		nError = ::WSAGetLastError();

	CWSocketException* pException = new CWSocketException(nError);

	TRACE(_T("Warning: throwing CWSocketException for error %d\n"), nError);
	THROW(pException);
}

void CWSocket::Attach(SOCKET hSocket)
{
  //Validate our parameters
  ASSERT(hSocket != INVALID_SOCKET);

  if (m_hSocket != INVALID_SOCKET)
    Close();

  m_hSocket = hSocket;
}

SOCKET CWSocket::Detach()
{
  SOCKET socket = m_hSocket;
  m_hSocket = INVALID_SOCKET;
  return socket;
}

void CWSocket::GetPeerName(CString& sPeerAddress, UINT& nPeerPort)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	int nSockAddrLen = sizeof(sockAddr);
	GetPeerName(reinterpret_cast<SOCKADDR*>(&sockAddr), &nSockAddrLen);
	nPeerPort = ntohs(sockAddr.sin_port);
	sPeerAddress = inet_ntoa(sockAddr.sin_addr);
}

void CWSocket::GetSockName(CString& sSocketAddress, UINT& nSocketPort)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	int nSockAddrLen = sizeof(sockAddr);
	GetSockName(reinterpret_cast<SOCKADDR*>(&sockAddr), &nSockAddrLen);
	nSocketPort = ntohs(sockAddr.sin_port);
	sSocketAddress = inet_ntoa(sockAddr.sin_addr);
}

void CWSocket::GetPeerName(SOCKADDR* lpSockAddr, int* lpSockAddrLen)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //Must have been created first

  if (getpeername(m_hSocket, lpSockAddr, lpSockAddrLen) == SOCKET_ERROR)
    ThrowWSocketException();
}

void CWSocket::GetSockName(SOCKADDR* lpSockAddr, int* lpSockAddrLen)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //Must have been created first

  if (getsockname(m_hSocket, lpSockAddr, lpSockAddrLen) == SOCKET_ERROR)
    ThrowWSocketException();
}

void CWSocket::Accept(CWSocket& connectedSocket, sockaddr_in& clientAddress)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first
  ASSERT(!connectedSocket.IsCreated()); //Must be an unitialized socket

  //Call the SDK accept function  
  int nSize = sizeof(sockaddr_in);
  SOCKET socket = accept(m_hSocket, reinterpret_cast<sockaddr*>(&clientAddress), &nSize);
  if (socket == INVALID_SOCKET)
    ThrowWSocketException(); 

  //Wrap the return value up into a C++ instance
  connectedSocket.Attach(socket);
}

void CWSocket::SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //Must have been created first

  if (setsockopt(m_hSocket, nLevel, nOptionName, static_cast<LPCSTR>(lpOptionValue), nOptionLen) == SOCKET_ERROR)
    ThrowWSocketException();
}

void CWSocket::GetSockOpt(int nOptionName, void* lpOptionValue, int* lpOptionLen, int nLevel)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //Must have been created first

  if (getsockopt(m_hSocket, nLevel, nOptionName, static_cast<LPSTR>(lpOptionValue), lpOptionLen) == SOCKET_ERROR)
    ThrowWSocketException();
}

void CWSocket::Bind(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //Must have been created first

  if (bind(m_hSocket, lpSockAddr, nSockAddrLen) == SOCKET_ERROR)
    ThrowWSocketException();
}

void CWSocket::Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress)
{
  //Setup the structure used in sdk "bind" calls
  SOCKADDR_IN sockAddr;
  memset(&sockAddr, 0, sizeof(sockAddr));

  sockAddr.sin_family = AF_INET;
  sockAddr.sin_port = htons(static_cast<u_short>(nSocketPort));

  //Do we need to bind to a specific IP address?
  if (lpszSocketAddress)
  {
    //Convert to an ASCII string
    CStringA sAsciiSocketAddress(lpszSocketAddress);
    sockAddr.sin_addr.s_addr = inet_addr(sAsciiSocketAddress);

	  //If the address is not dotted notation, then do a DNS 
	  //lookup of it.
	  if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	  {
		  LPHOSTENT lphost;
		  lphost = gethostbyname(sAsciiSocketAddress);
		  if (lphost != NULL)
			  sockAddr.sin_addr.s_addr = (reinterpret_cast<LPIN_ADDR>(lphost->h_addr))->s_addr;
		  else
		  {
        ThrowWSocketException(WSAEINVAL); 
			  return;
		  }
    }
  }
  else
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY); //Bind to any IP address;

  Bind(reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr));
}

void CWSocket::Close()
{
	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
}

void CWSocket::Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

	if (connect(m_hSocket, lpSockAddr, nSockAddrLen) == SOCKET_ERROR)
    ThrowWSocketException();
}

void CWSocket::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first
  ASSERT(lpszHostAddress); //must have a valid host

  //Work out the IP address of the machine we want to connect to
	CStringA sAsciiHostAddress(lpszHostAddress);

	//Determine if the address is in dotted notation
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(static_cast<u_short>(nHostPort));
	sockAddr.sin_addr.s_addr = inet_addr(sAsciiHostAddress);

	//If the address is not dotted notation, then do a DNS 
	//lookup of it.
	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost = gethostbyname(sAsciiHostAddress);
		if (lphost != NULL)
			sockAddr.sin_addr.s_addr = (reinterpret_cast<LPIN_ADDR>(lphost->h_addr))->s_addr;
		else
      ThrowWSocketException(); 
	}

  //Call the other version of Connect which does the actual work
	Connect(reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr));
}

#ifdef _WINSOCK2API_
void CWSocket::Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen, DWORD dwTimeout, BOOL bResetToBlockingMode)
{
  //Create an event to wait on
  WSAEVENT hConnectedEvent = WSACreateEvent();
  if (hConnectedEvent == WSA_INVALID_EVENT)
    ThrowWSocketException();

  //Setup event selection on the socket
  if (WSAEventSelect(m_hSocket, hConnectedEvent, FD_CONNECT) == SOCKET_ERROR)
  {
    //Hive away the last error
    DWORD dwLastError = GetLastError();

    //Close the event before we return
    WSACloseEvent(hConnectedEvent);

    //Throw the exception that we could not setup event selection
    ThrowWSocketException(dwLastError);
  }

  //Call the SDK "connect" function
  int nConnected = connect(m_hSocket, lpSockAddr, nSockAddrLen);
  if (nConnected == SOCKET_ERROR)
  {
    //Check to see if the call should be completed by waiting for the event to be signalled
    DWORD dwLastError = GetLastError();
    if (dwLastError == WSAEWOULDBLOCK)
    {
      DWORD dwWait = WaitForSingleObject(hConnectedEvent, dwTimeout);
      if (dwWait == WAIT_OBJECT_0)
      {
        //Get the error value returned using WSAEnumNetworkEvents
        WSANETWORKEVENTS networkEvents;
        int nEvents = WSAEnumNetworkEvents(m_hSocket, hConnectedEvent, &networkEvents);
        if (nEvents == SOCKET_ERROR)
        {
          //Hive away the last error
          dwLastError = GetLastError();

          //Close the event before we return
          WSACloseEvent(hConnectedEvent);

          //Throw the exception that we could not call WSAEnumNetworkEvents
          ThrowWSocketException(dwLastError);
        }
        else
        {
          ASSERT(networkEvents.lNetworkEvents & FD_CONNECT);

          //Has an error occured in the connect call
          if (networkEvents.iErrorCode[FD_CONNECT_BIT] != ERROR_SUCCESS)
          {
            //Close the event before we return
            WSACloseEvent(hConnectedEvent);

            //Throw the exception that an error has occurred in calling connect
            ThrowWSocketException(networkEvents.iErrorCode[FD_CONNECT_BIT]);
          }
        }
      }
      else
      {
        //Close the event before we return
        WSACloseEvent(hConnectedEvent);

        //Throw the exception that we could not connect in a timely fashion
        ThrowWSocketException(WSAETIMEDOUT);
      }
    }
    else
    {
      //Close the event before we return
      WSACloseEvent(hConnectedEvent);

      //Throw the exception that the connect call failed unexpectedly
      ThrowWSocketException(dwLastError);
    }
  }

  //Remove the event notification on the socket
  WSAEventSelect(m_hSocket, hConnectedEvent, 0);

  //Destroy the event now that we are finished with it
  WSACloseEvent(hConnectedEvent);

  //Reset the socket to blocking mode if required
  if (bResetToBlockingMode)
  {
    DWORD dwNonBlocking = 0;
    if (ioctlsocket(m_hSocket, FIONBIO, &dwNonBlocking) == SOCKET_ERROR)
    {
      //Throw the exception that we could not reset the socket to blocking mode
      ThrowWSocketException();
    }
  }    
}

void CWSocket::Connect(LPCTSTR lpszHostAddress, UINT nHostPort, DWORD dwTimeout, BOOL bResetToBlockingMode)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first
  ASSERT(lpszHostAddress); //must have a valid host

  //Work out the IP address of the machine we want to connect to
	CStringA sAsciiHostAddress(lpszHostAddress);

	//Determine if the address is in dotted notation
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(static_cast<u_short>(nHostPort));
	sockAddr.sin_addr.s_addr = inet_addr(sAsciiHostAddress);

	//If the address is not dotted notation, then do a DNS lookup of it.
	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost = gethostbyname(sAsciiHostAddress);
		if (lphost != NULL)
			sockAddr.sin_addr.s_addr = (reinterpret_cast<LPIN_ADDR>(lphost->h_addr))->s_addr;
		else
      ThrowWSocketException(); 
	}

  //Call the other version of Connect which does the actual work
  Connect(reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), dwTimeout, bResetToBlockingMode);
}
#endif //_WINSOCK2API_

int CWSocket::Receive(void* lpBuf, int nBufLen, int nFlags)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  int nReceived = recv(m_hSocket, static_cast<LPSTR>(lpBuf), nBufLen, nFlags); 
  if (nReceived == SOCKET_ERROR)
    ThrowWSocketException();

  return nReceived;
}

int CWSocket::ReceiveFrom(void* lpBuf, int nBufLen, SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  int nReceived = recvfrom(m_hSocket, static_cast<LPSTR>(lpBuf), nBufLen, nFlags, lpSockAddr, lpSockAddrLen);
  if (nReceived == SOCKET_ERROR)
    ThrowWSocketException();

  return nReceived;
}

int CWSocket::ReceiveFrom(void* lpBuf, int nBufLen, CString& sSocketAddress, UINT& nSocketPort, int nFlags)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	int nSockAddrLen = sizeof(sockAddr);
	int nResult = ReceiveFrom(lpBuf, nBufLen, reinterpret_cast<SOCKADDR*>(&sockAddr), &nSockAddrLen, nFlags);
	nSocketPort = ntohs(sockAddr.sin_port);
	sSocketAddress = inet_ntoa(sockAddr.sin_addr);
	return nResult;
}

int CWSocket::Send(const void* pBuffer, int nBufLen, int nFlags)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  int nSent = send(m_hSocket, static_cast<const char*>(pBuffer), nBufLen, nFlags);
  if (nSent == SOCKET_ERROR)
    ThrowWSocketException();

  return nSent;
}

int CWSocket::SendTo(const void* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  int nSent = sendto(m_hSocket, static_cast<const char*>(lpBuf), nBufLen, nFlags, lpSockAddr, nSockAddrLen);
  if (nSent == SOCKET_ERROR)
    ThrowWSocketException();

  return nSent; 
}

int CWSocket::SendTo(const void* lpBuf, int nBufLen, UINT nHostPort, LPCTSTR lpszHostAddress, int nFlags)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  //Determine if the address is in dotted notation
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(static_cast<u_short>(nHostPort));

	if (lpszHostAddress == NULL)
		sockAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	else
	{
	  //If the address is not dotted notation, then do a DNS lookup of it.
	  CStringA sAsciiHostAddress(lpszHostAddress);
		sockAddr.sin_addr.s_addr = inet_addr(sAsciiHostAddress);
		if (sockAddr.sin_addr.s_addr == INADDR_NONE)
		{
			LPHOSTENT lphost = gethostbyname(sAsciiHostAddress);
			if (lphost != NULL)
				sockAddr.sin_addr.s_addr = (reinterpret_cast<LPIN_ADDR>(lphost->h_addr))->s_addr;
			else
				ThrowWSocketException();
		}
	}

	return SendTo(lpBuf, nBufLen, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nFlags);
}

void CWSocket::IOCtl(long lCommand, DWORD* lpArgument)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  if (ioctlsocket(m_hSocket, lCommand, lpArgument) == SOCKET_ERROR)
    ThrowWSocketException();
}

void CWSocket::Listen(int nConnectionBacklog)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  if (listen(m_hSocket, nConnectionBacklog) == SOCKET_ERROR)
    ThrowWSocketException();
}

void CWSocket::ShutDown(int nHow)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  if (shutdown(m_hSocket, nHow) == SOCKET_ERROR)
    ThrowWSocketException();
}

void CWSocket::Create(BOOL bUDP)
{
  //Validate our parameters
  ASSERT(!IsCreated()); //must not have been already created

  if (bUDP)
    Create(SOCK_DGRAM, 0, PF_INET);
  else
    Create(SOCK_STREAM, 0, PF_INET);
}

void CWSocket::Create(int nSocketType, int nProtocolType, int nAddressFormat)
{
  //Validate our parameters
  ASSERT(!IsCreated()); //must not have been already created

	m_hSocket = socket(nAddressFormat, nSocketType, nProtocolType);
  if (m_hSocket == INVALID_SOCKET)
    ThrowWSocketException();
}

void CWSocket::ConnectViaSocks4(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszSocksServer, UINT nSocksPort, DWORD dwTimeout)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  //connect to the proxy
  Connect(lpszSocksServer, nSocksPort);

  try
  {
    //Fill in a connection request packet
    WSOCKET_SOCK4_CONNECT_REQUEST request;
    request.VN = 4;
    request.CD = 1;
    request.DSTPORT = htons(static_cast<u_short>(nHostPort));

	  //Determine if the address is in dotted notation
	  CStringA sAsciiHostAddress(lpszHostAddress);
	  request.DSTIP.S_un.S_addr = inet_addr(sAsciiHostAddress);

	  //If the address is not dotted notation, then do a DNS 
	  //lookup of it, since Socks 4 does not support DNS proxying
	  if (request.DSTIP.S_un.S_addr == INADDR_NONE)
	  {
		  LPHOSTENT lphost = gethostbyname(sAsciiHostAddress);
		  if (lphost != NULL)
			  request.DSTIP.S_un.S_addr = (reinterpret_cast<LPIN_ADDR>(lphost->h_addr))->s_addr;
		  else
        ThrowWSocketException(); 
	  }
    request.USERID[0] = 0;
    Send(&request, sizeof(request));

    //Wait for the connection reply
    WSOCKET_SOCKS4_CONNECT_REPLY reply;
    memset(&reply, 0, sizeof(reply));
    int nDataReceived = 0;
    while (nDataReceived < sizeof(reply))
    {
      if (IsReadible(dwTimeout))
      {
        int nData = Receive((reinterpret_cast<BYTE*>(&reply)) + nDataReceived, sizeof(reply) - nDataReceived);
        nDataReceived += nData;
      }
      else
        ThrowWSocketException(ERROR_TIMEOUT);
    }

    //Validate the response
    if ((reply.VN != 0) || (reply.CD != 90))
      ThrowWSocketException(ERROR_BAD_NET_RESP);
  }
  catch(CWSocketException* pEx)
  {
    //Close the socket before we rethrow the exception
    int nError = pEx->m_nError;
    pEx->Delete();
    Close();
    ThrowWSocketException(nError);
  }
}

void CWSocket::ConnectViaSocks5(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszSocksServer, UINT nSocksPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword, DWORD dwTimeout, BOOL bUDP)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  //connect to the proxy
  Connect(lpszSocksServer, nSocksPort);

  try
  {  
    BOOL bAuthenticate = (lpszUserName != NULL);

    //Fill in a connection request packet
    WSOCKET_SOCKS5_IDENTIFIER_PACKET request;
    request.VER = 5;
    request.NMETHODS = 1;
    request.METHODS[0] = 0;
    if (bAuthenticate)
    {
      request.NMETHODS++;
      request.METHODS[1] = 2;
    }

    //Send the request down the socket
    Send(&request, bAuthenticate ? 4 : 3);

    //Wait for the connection reply
    WSOCKET_SOCKS5_METHODSELECTION_MESSAGE reply;
    memset(&reply, 0, sizeof(reply));
    int nDataReceived = 0;
    while (nDataReceived < sizeof(reply))
    {
      //check the socket for readability
      if (!IsReadible(dwTimeout))
      {
        TRACE(_T("CWSocket::ConnectViaSocks5, Timed out waiting for response from socket\n"));  
        ThrowWSocketException(ERROR_TIMEOUT);
      }
    
      //Read the data from the socket
      int nData = Receive((reinterpret_cast<BYTE*>(&reply)) + nDataReceived, sizeof(reply) - nDataReceived);
      
      //Handle a graceful disconnect
      if (nData == 0)
        ThrowWSocketException(ERROR_GRACEFUL_DISCONNECT);
      
      //Increment the count of data received
      nDataReceived += nData;
    }

    //Validate the response
    if ((bAuthenticate && ((reply.METHOD != 0) && (reply.METHOD != 2))) || (!bAuthenticate && (reply.METHOD != 0)))
      ThrowWSocketException(WSAECONNREFUSED);

    if (bAuthenticate && reply.METHOD == 2)
    {
      CStringA sAsciiUserName(lpszUserName);
      CStringA sAsciiPassword(lpszPassword);
      int nUserNameLength = sAsciiUserName.GetLength();
      int nPasswordLength = sAsciiPassword.GetLength();

      //Verify the username and password are not longer that the protocol allows
      if ((nUserNameLength > 255) || (nPasswordLength > 255))
      {
        SecureEmptyString(sAsciiUserName);
        SecureEmptyString(sAsciiPassword);
        ThrowWSocketException(ERROR_INVALID_PARAMETER);
      }

      int nUserRequestLength = 3 + nUserNameLength + nPasswordLength;
      BYTE* pUserRequest = reinterpret_cast<BYTE*>(_alloca(nUserRequestLength));
      pUserRequest[0] = 1;
      pUserRequest[1] = static_cast<BYTE>(nUserNameLength);
      memcpy(&(pUserRequest[2]), sAsciiUserName.operator LPCSTR(), nUserNameLength);
      SecureEmptyString(sAsciiUserName);
      pUserRequest[2 + nUserNameLength] = static_cast<BYTE>(nPasswordLength);
      memcpy(pUserRequest + 3 + nUserNameLength, sAsciiPassword.operator LPCSTR(), nPasswordLength);
      SecureEmptyString(sAsciiPassword);
      Send(pUserRequest, static_cast<int>(nUserRequestLength));
      SecureZeroMemory(pUserRequest, nUserRequestLength);

      //Wait for the login reply
      WSOCKET_SOCKS5_USERNAME_AUTHENTICATION_REPLY reply2;
      memset(&reply2, 0, sizeof(reply2));
      nDataReceived = 0;
      while (nDataReceived < sizeof(reply2))
      {
        //check the socket for readability
        if (!IsReadible(dwTimeout))
        {
          TRACE(_T("CWSocket::ConnectViaSocks5, Timed out waiting for response from socket\n"));  
          ThrowWSocketException(ERROR_TIMEOUT);
        }
      
        //Read the data from the socket
        int nData = Receive((reinterpret_cast<BYTE*>(&reply2)) + nDataReceived, sizeof(reply2) - nDataReceived);
        
        //Handle a graceful disconnect
        if (nData == 0)
          ThrowWSocketException(ERROR_GRACEFUL_DISCONNECT);
        
        //Increment the count of data received
        nDataReceived += nData;
      }

      if (reply2.STATUS != 0)
        ThrowWSocketException(ERROR_ACCESS_DENIED);
    }

	  //Determine if the address is in dotted notation
	  CStringA sAsciiHostAddress(lpszHostAddress);
	  unsigned long nAddr = inet_addr(sAsciiHostAddress);
	  if (nAddr == INADDR_NONE)
	  {
      //verify that the host name is less than 256 bytes which is the limit of the hostname which Socks5 can accomadate
      int nHostLength = sAsciiHostAddress.GetLength();
      if (nHostLength > 255)
        ThrowWSocketException(ERROR_INVALID_PARAMETER);

      WSOCKET_SOCKS5_HOSTNAME_REQUEST_DETAILS requestDetails;
      memset(&requestDetails, 0, sizeof(requestDetails));
      requestDetails.Base.VER = 5;
      if (bUDP)
        requestDetails.Base.CMD = 3;
      else
        requestDetails.Base.CMD = 1;
      requestDetails.Base.ATYP = 3;
      requestDetails.DST_HOST.LENGTH = static_cast<BYTE>(nHostLength);
      memcpy(requestDetails.DST_HOST.HOST, sAsciiHostAddress.operator LPCSTR(), nHostLength);
      WORD* pPort = reinterpret_cast<WORD*>(requestDetails.DST_HOST.HOST + nHostLength);
      *pPort = htons(static_cast<u_short>(nHostPort));
      size_t nRequestDetailsSize = sizeof(requestDetails) - 256 + nHostLength + 1;

      //Send the request down the socket
      Send(&requestDetails, static_cast<int>(nRequestDetailsSize));
	  }
    else
    {
      WSOCKET_SOCKS5_IP4_REQUEST_DETAILS requestDetails;
      memset(&requestDetails, 0, sizeof(requestDetails));
      requestDetails.Base.VER = 5;
      requestDetails.Base.CMD = 1;
      requestDetails.Base.ATYP = 1;
      requestDetails.DST_IP.S_un.S_addr = nAddr;
      requestDetails.DSTPORT = htons(static_cast<u_short>(nHostPort));
      Send(&requestDetails, sizeof(requestDetails));
    }

    //Read the reply
    ReadSocks5ConnectReply(dwTimeout);
  }
  catch(CWSocketException* pEx)
  {
    //Close the socket before we rethrow the exception
    int nError = pEx->m_nError;
    pEx->Delete();
    Close();
    ThrowWSocketException(nError);
  }
}

void CWSocket::ReadSocks5ConnectReply(DWORD dwTimeout)
{
  //Allocate some heap memory to contain the response in
  int nBufSize = max(sizeof(WSOCKET_SOCKS5_IP4_REQUEST_DETAILS), sizeof(WSOCKET_SOCKS5_HOSTNAME_REQUEST_DETAILS));
  ATL::CHeapPtr<BYTE> response;
  if (!response.Allocate(nBufSize))
    ThrowWSocketException(ERROR_OUTOFMEMORY);
  
  //retrieve the reponse
  DWORD dwCurrentReadOffset = 0;
  BOOL bMoreDataToRead = TRUE;
  while (bMoreDataToRead)
  {
    //check the socket for readability
    if (!IsReadible(dwTimeout))
    {
      TRACE(_T("CWSocket::ReadSocks5ConnectReply, Timed out waiting for response from socket\n"));  
      ThrowWSocketException(ERROR_TIMEOUT);
    }

    //Read data from the socket    
    int nData = Receive(response.m_pData + dwCurrentReadOffset, nBufSize - dwCurrentReadOffset);
    
    //Handle a graceful disconnect
    if (nData == 0)
      ThrowWSocketException(ERROR_GRACEFUL_DISCONNECT);

    //Increment the count of data received
    dwCurrentReadOffset += nData;							   

    //Try to parse out what we received
    if (dwCurrentReadOffset >= sizeof(WSOCKET_SOCKS5_BASE_REQUEST_DETAILS)) 
    {
      WSOCKET_SOCKS5_BASE_REQUEST_DETAILS* pBaseRequest = reinterpret_cast<WSOCKET_SOCKS5_BASE_REQUEST_DETAILS*>(response.m_pData);
      if (pBaseRequest->ATYP == 1)
      {
        //An IP 4 address type destination
        bMoreDataToRead = (dwCurrentReadOffset < sizeof(WSOCKET_SOCKS5_IP4_REQUEST_DETAILS));
        if (!bMoreDataToRead)
        {
          if (pBaseRequest->CMD != 0)
            ThrowWSocketException(ERROR_BAD_NET_RESP);
        }
      }
      else if (pBaseRequest->ATYP == 3)
      {
        //A domain name type destination
        if (dwCurrentReadOffset > sizeof(WSOCKET_SOCKS5_BASE_REQUEST_DETAILS))
        {
          WSOCKET_SOCKS5_HOSTNAME_REQUEST_DETAILS* pHostnameRequest = reinterpret_cast<WSOCKET_SOCKS5_HOSTNAME_REQUEST_DETAILS*>(response.m_pData);
          bMoreDataToRead = (dwCurrentReadOffset < ((sizeof(WSOCKET_SOCKS5_HOSTNAME_REQUEST_DETAILS) - 256) + pHostnameRequest->DST_HOST.LENGTH));
          if (!bMoreDataToRead)
          {
            if (pBaseRequest->CMD != 0)
              ThrowWSocketException(ERROR_BAD_NET_RESP);
          }
        }
      }
      else
        ThrowWSocketException(ERROR_INVALID_PARAMETER);
    }
  }
}

BOOL CWSocket::IsReadible(DWORD dwTimeout)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  timeval timeout;
  timeout.tv_sec = dwTimeout/1000;
  timeout.tv_usec = (dwTimeout%1000)*1000;
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(m_hSocket, &fds);
  int nStatus = select(0, &fds, NULL, NULL, &timeout);
  if (nStatus == SOCKET_ERROR)
    ThrowWSocketException();

  return !(nStatus == 0);
}

BOOL CWSocket::IsWritable(DWORD dwTimeout)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  timeval timeout;
  timeout.tv_sec = dwTimeout/1000;
  timeout.tv_usec = (dwTimeout%1000)*1000;
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(m_hSocket, &fds);
  int nStatus = select(0, NULL, &fds, NULL, &timeout);
  if (nStatus == SOCKET_ERROR)
    ThrowWSocketException();

  return !(nStatus == 0);
}

BOOL CWSocket::IsCreated() const
{
  return (m_hSocket != INVALID_SOCKET);
}

CWSocket::operator SOCKET()
{
  return m_hSocket;
}

void CWSocket::ConnectViaHTTPProxy(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszHTTPProxy, UINT nHTTPProxyPort, CStringA& sProxyResponse, LPCTSTR lpszUserName, LPCTSTR lpszPassword, DWORD dwConnectionTimeout, LPCTSTR lpszUserAgent)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  //connect to the proxy
  Connect(lpszHTTPProxy, nHTTPProxyPort);

  try
  {  
    //Form the HTTP CONNECT request header
    CString sLine;
    sLine.Format(_T("CONNECT %s:%d HTTP/1.0\r\n"), lpszHostAddress, nHostPort);
    CString sRequest(sLine);
    
    //Form the authorization line if required  
    if (lpszUserName != NULL)
    {
      //Base64 encode the username password combination
      CString sUserNamePassword;
      sUserNamePassword.Format(_T("%s:%s"), lpszUserName, lpszPassword);
      CStringA sAsciiUserNamePassword(sUserNamePassword);
      int nUserNamePasswordLength = sAsciiUserNamePassword.GetLength();
      int nEncodedLength = ATL::Base64EncodeGetRequiredLength(nUserNamePasswordLength, ATL_BASE64_FLAG_NOCRLF);
      LPSTR pszEncoded = static_cast<LPSTR>(_alloca(nEncodedLength + 1));
      ATL::Base64Encode(reinterpret_cast<const BYTE*>(sAsciiUserNamePassword.operator LPCSTR()), nUserNamePasswordLength, pszEncoded, &nEncodedLength);
      pszEncoded[nEncodedLength] = '\0';
      SecureEmptyString(sAsciiUserNamePassword);
      SecureEmptyString(sUserNamePassword);

      //Form the Authorization header line and add it to the request
      sLine.Format(_T("Proxy-authorization: Basic %s\r\n"), CString(pszEncoded).operator LPCTSTR());
      SecureZeroMemory(pszEncoded, nEncodedLength + 1);
      sRequest += sLine;
    }

    //Form the user agent line if required
    if (lpszUserAgent != NULL)
    {
      //Add the User Agent line to the request
      sLine.Format(_T("User-Agent: %s\r\n"), lpszUserAgent);
      sRequest += sLine;
    }

    //Add the final line feed to the request
    sRequest += _T("\r\n");

    //Finally send the request to the HTTP proxy
    CStringA sAsciiRequest(sRequest);
    Send(sAsciiRequest.operator LPCSTR(), sAsciiRequest.GetLength());

    //Read the proxy response
    sProxyResponse.Preallocate(4096);
    ReadHTTPProxyResponse(dwConnectionTimeout, sProxyResponse);

    //Next make sure that we got a HTTP code of 200 to indicate success
    int nFirstSpace = sProxyResponse.Find(" ");
    if (nFirstSpace != -1)
    {
      CStringA sResponseCode(sProxyResponse.Right(sProxyResponse.GetLength() - nFirstSpace - 1));
      int nResponseCode = atoi(sResponseCode);
      if (nResponseCode != 200)
        ThrowWSocketException(ERROR_CONNECTION_REFUSED);
    }
  }
  catch(CWSocketException* pEx)
  {
    //Close the socket before we rethrow the exception
    int nError = pEx->m_nError;
    pEx->Delete();
    Close();
    ThrowWSocketException(nError);
  }
}

void CWSocket::ReadHTTPProxyResponse(DWORD dwTimeout, CStringA& sResponse)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  //retrieve the reponse
  BOOL bMoreDataToRead = TRUE;
  while (bMoreDataToRead)
  {
    //check the socket for readability
    if (!IsReadible(dwTimeout))
    {
      TRACE(_T("CWSocket::ReadHTTPProxyResponse, Timed out waiting for response from socket\n"));
		  ThrowWSocketException(ERROR_TIMEOUT);
    }

	  //receive the data from the socket (1 byte at a time, so that we can look for the separators between the HTTP response headers and the response entity body)
	  char szResponse[2];
    int nData = Receive(szResponse, 1);
    
    //Handle a graceful disconnect
    if (nData == 0)
      ThrowWSocketException(ERROR_GRACEFUL_DISCONNECT);
    
    //NULL terminate the data received
    szResponse[1] = '\0';
    
    //Accumulate into the response output parameter
    sResponse += szResponse;

    //Check to see if the terminator character(s) have been found
    int nLength = sResponse.GetLength();
    if (nLength >= 4)
    {
      bMoreDataToRead = !((sResponse.GetAt(nLength-4) == '\r') && (sResponse.GetAt(nLength-3) == '\n') &&
                          (sResponse.GetAt(nLength-2) == '\r') && (sResponse.GetAt(nLength-1) == '\n'));
    }
  }
}
