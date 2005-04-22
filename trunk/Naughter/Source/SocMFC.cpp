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

Copyright (c) 2002 - 2005 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

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
#include "Base64.h"



/////////////////// Macros / Defines //////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef __AFXPRIV_H__
#pragma message("To avoid this message please put afxpriv.h in your PCH (normally stdafx.h)")
#include <afxpriv.h>
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



////////// Exception handling code

void AfxThrowWSocketException(int nError /* = 0 */)
{
	if (nError == 0)
		nError = ::WSAGetLastError();

	CWSocketException* pException = new CWSocketException(nError);

	TRACE(_T("Warning: throwing CWSocketException for error %d\n"), nError);
	THROW(pException);
}

BOOL CWSocketException::GetErrorMessage(LPTSTR pstrError, UINT nMaxError, PUINT pnHelpContext)
{
	ASSERT(pstrError != NULL && AfxIsValidString(pstrError, nMaxError));

	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

	LPTSTR lpBuffer;
	BOOL bRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			                      NULL,  m_nError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			                      (LPTSTR) &lpBuffer, 0, NULL);

	if (bRet == FALSE)
		*pstrError = '\0';
	else
	{
		lstrcpyn(pstrError, lpBuffer, nMaxError);
		bRet = TRUE;

		LocalFree(lpBuffer);
	}

	return bRet;
}

CString CWSocketException::GetErrorMessage()
{
  CString rVal;
  LPTSTR pstrError = rVal.GetBuffer(4096);
  GetErrorMessage(pstrError, 4096, NULL);
  rVal.ReleaseBuffer();
  return rVal;
}

CWSocketException::CWSocketException(int nError)
{
	m_nError = nError;
}

CWSocketException::~CWSocketException()
{
}

IMPLEMENT_DYNAMIC(CWSocketException, CException)

#ifdef _DEBUG
void CWSocketException::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << "m_nError = " << m_nError;
}
#endif



////////// The main class /////////////////////////////////////

CWSocket::CWSocket() : m_hSocket(INVALID_SOCKET)
{
}

CWSocket::~CWSocket()
{
  if (m_hSocket != INVALID_SOCKET)
    Close();
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
	GetPeerName((SOCKADDR*)&sockAddr, &nSockAddrLen);
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
	GetSockName((SOCKADDR*)&sockAddr, &nSockAddrLen);
	nSocketPort = ntohs(sockAddr.sin_port);
	sSocketAddress = inet_ntoa(sockAddr.sin_addr);
}

void CWSocket::GetPeerName(SOCKADDR* lpSockAddr, int* lpSockAddrLen)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //Must have been created first

  if (getpeername(m_hSocket, lpSockAddr, lpSockAddrLen) == SOCKET_ERROR)
    AfxThrowWSocketException();
}

void CWSocket::GetSockName(SOCKADDR* lpSockAddr, int* lpSockAddrLen)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //Must have been created first

  if (getsockname(m_hSocket, lpSockAddr, lpSockAddrLen) == SOCKET_ERROR)
    AfxThrowWSocketException();
}

void CWSocket::Accept(CWSocket& connectedSocket, sockaddr_in& clientAddress)
{
  ASSERT(IsCreated());               //must have been created first
  ASSERT(!connectedSocket.IsCreated()); //Must be an unitialized socket

  //Call the SDK accept function  
  int nSize = sizeof(sockaddr_in);
  SOCKET socket = accept(m_hSocket, (sockaddr*) &clientAddress, &nSize);
  if (socket == INVALID_SOCKET)
    AfxThrowWSocketException(); 

  //Wrap the return value up into a C++ instance
  connectedSocket.Attach(socket);
}

void CWSocket::SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //Must have been created first

  if (setsockopt(m_hSocket, nLevel, nOptionName, (LPCSTR)lpOptionValue, nOptionLen) == SOCKET_ERROR)
    AfxThrowWSocketException();
}

void CWSocket::GetSockOpt(int nOptionName, void* lpOptionValue, int* lpOptionLen, int nLevel)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //Must have been created first

  if (getsockopt(m_hSocket, nLevel, nOptionName, (LPSTR)lpOptionValue, lpOptionLen) == SOCKET_ERROR)
    AfxThrowWSocketException();
}

void CWSocket::Bind(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //Must have been created first

  if (bind(m_hSocket, lpSockAddr, nSockAddrLen) == SOCKET_ERROR)
    AfxThrowWSocketException();
}

void CWSocket::Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress)
{
	USES_CONVERSION;

  //Setup the structure used in sdk "bind" calls
  SOCKADDR_IN sockAddr;
  memset(&sockAddr, 0, sizeof(sockAddr));

  sockAddr.sin_family = AF_INET;
  sockAddr.sin_port = htons((u_short)nSocketPort);

  //Do we need to bind to a specific IP address?
  if (lpszSocketAddress)
  {
    //Convert to an ASCII string
    LPSTR lpszAscii = T2A((LPTSTR) lpszSocketAddress);
    sockAddr.sin_addr.s_addr = inet_addr(lpszAscii);

	  //If the address is not dotted notation, then do a DNS 
	  //lookup of it.
	  if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	  {
		  LPHOSTENT lphost;
		  lphost = gethostbyname(lpszAscii);
		  if (lphost != NULL)
			  sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		  else
		  {
        AfxThrowWSocketException(WSAEINVAL); 
			  return;
		  }
    }
  }
  else
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY); //Bind to any IP address;

  Bind((SOCKADDR*) &sockAddr, sizeof(sockAddr));
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
    AfxThrowWSocketException();
}

void CWSocket::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
	USES_CONVERSION;

  //Validate our parameters
  ASSERT(IsCreated());             //must have been created first
  ASSERT(lpszHostAddress);          //Must have a valid host

  //Work out the IP address of the machine we want to connect to
	LPSTR lpszAscii = T2A((LPTSTR) lpszHostAddress);

	//Determine if the address is in dotted notation
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons((u_short)nHostPort);
	sockAddr.sin_addr.s_addr = inet_addr(lpszAscii);

	//If the address is not dotted notation, then do a DNS 
	//lookup of it.
	if (sockAddr.sin_addr.s_addr == INADDR_NONE)
	{
		LPHOSTENT lphost = gethostbyname(lpszAscii);
		if (lphost != NULL)
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
      AfxThrowWSocketException(); 
	}

	Connect((SOCKADDR*)&sockAddr, sizeof(sockAddr));
}

int CWSocket::Receive(void* lpBuf, int nBufLen, int nFlags)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  int nReceived = recv(m_hSocket, (LPSTR) lpBuf, nBufLen, nFlags); 
  if (nReceived == SOCKET_ERROR)
    AfxThrowWSocketException();

  return nReceived;
}

int CWSocket::ReceiveFrom(void* lpBuf, int nBufLen, SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  int nReceived = recvfrom(m_hSocket, (LPSTR) lpBuf, nBufLen, nFlags, lpSockAddr, lpSockAddrLen);
  if (nReceived == SOCKET_ERROR)
    AfxThrowWSocketException();

  return nReceived;
}

int CWSocket::ReceiveFrom(void* lpBuf, int nBufLen, CString& sSocketAddress, UINT& nSocketPort, int nFlags)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	int nSockAddrLen = sizeof(sockAddr);
	int nResult = ReceiveFrom(lpBuf, nBufLen, (SOCKADDR*)&sockAddr, &nSockAddrLen, nFlags);
	nSocketPort = ntohs(sockAddr.sin_port);
	sSocketAddress = inet_ntoa(sockAddr.sin_addr);
	return nResult;
}

void CWSocket::Send(void* pBuffer, int nBufLen, int nFlags)
{
  ASSERT(IsCreated()); //must have been created first

  if (send(m_hSocket, (char*) pBuffer, nBufLen, nFlags) == SOCKET_ERROR)
    AfxThrowWSocketException();
}

int CWSocket::SendTo(const void* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  int nSent = sendto(m_hSocket, (LPSTR) lpBuf, nBufLen, nFlags, lpSockAddr, nSockAddrLen);
  if (nSent == SOCKET_ERROR)
    AfxThrowWSocketException();

  return nSent; 
}

int CWSocket::SendTo(const void* lpBuf, int nBufLen, UINT nHostPort, LPCTSTR lpszHostAddress, int nFlags)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

	USES_CONVERSION;

  //Determine if the address is in dotted notation
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons((u_short)nHostPort);

	if (lpszHostAddress == NULL)
		sockAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	else
	{
	  //If the address is not dotted notation, then do a DNS 
	  //lookup of it.
	  LPSTR lpszAscii = T2A((LPTSTR)lpszHostAddress);
		sockAddr.sin_addr.s_addr = inet_addr(lpszAscii);
		if (sockAddr.sin_addr.s_addr == INADDR_NONE)
		{
			LPHOSTENT lphost = gethostbyname(lpszAscii);
			if (lphost != NULL)
				sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
			else
				AfxThrowWSocketException();
		}
	}

	return SendTo(lpBuf, nBufLen, (SOCKADDR*)&sockAddr, sizeof(sockAddr), nFlags);
}

void CWSocket::IOCtl(long lCommand, DWORD* lpArgument)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  if (ioctlsocket(m_hSocket, lCommand, lpArgument) == SOCKET_ERROR)
    AfxThrowWSocketException();
}

void CWSocket::Listen(int nConnectionBacklog)
{
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  if (listen(m_hSocket, nConnectionBacklog) == SOCKET_ERROR)
    AfxThrowWSocketException();
}

void CWSocket::ShutDown(int nHow)
{ 
  //Validate our parameters
  ASSERT(IsCreated()); //must have been created first

  if (shutdown(m_hSocket, nHow) == SOCKET_ERROR)
    AfxThrowWSocketException();
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
    AfxThrowWSocketException();
}

void CWSocket::ConnectViaSocks4(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszSocksServer, UINT nSocksPort, DWORD dwConnectionTimeout)
{
	USES_CONVERSION;
  ASSERT(IsCreated()); //must have been created first

  //connect to the proxy
  Connect(lpszSocksServer, nSocksPort);

  try
  {
    //Fill in a connection request packet
    WSOCKET_SOCK4_CONNECT_REQUEST request;
    request.VN = 4;
    request.CD = 1;
    request.DSTPORT = htons((u_short) nHostPort);

	  //Determine if the address is in dotted notation
	  LPSTR lpszAscii = T2A((LPTSTR) lpszHostAddress);
	  request.DSTIP.S_un.S_addr = inet_addr(lpszAscii);

	  //If the address is not dotted notation, then do a DNS 
	  //lookup of it, since Socks 4 does not support DNS proxying
	  if (request.DSTIP.S_un.S_addr == INADDR_NONE)
	  {
		  LPHOSTENT lphost = gethostbyname(lpszAscii);
		  if (lphost != NULL)
			  request.DSTIP.S_un.S_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		  else
        AfxThrowWSocketException(); 
	  }
    request.USERID[0] = 0;
    Send(&request, sizeof(request));

    //Wait for the connection reply
    WSOCKET_SOCKS4_CONNECT_REPLY reply;
    memset(&reply, 0, sizeof(reply));
    int nDataReceived = 0;
    while (nDataReceived < sizeof(reply))
    {
      if (IsReadible(dwConnectionTimeout))
      {
        int nData = Receive(((BYTE*) &reply) + nDataReceived, sizeof(reply) - nDataReceived);
        nDataReceived += nData;
      }
      else
        AfxThrowWSocketException(WSAETIMEDOUT);
    }

    //Validate the response
    if ((reply.VN != 0) || (reply.CD != 90))
      AfxThrowWSocketException(ERROR_BAD_NET_RESP);
  }
  catch(CWSocketException* pEx)
  {
    //Close the socket before we rethrow the exception
    int nError = pEx->m_nError;
    pEx->Delete();
    Close();
    AfxThrowWSocketException(nError);
  }
}

void CWSocket::ConnectViaSocks5(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszSocksServer, UINT nSocksPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword, DWORD dwConnectionTimeout, BOOL bUDP)
{
  USES_CONVERSION;
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

    Send(&request, bAuthenticate ? 4 : 3);

    //Wait for the connection reply
    WSOCKET_SOCKS5_METHODSELECTION_MESSAGE reply;
    memset(&reply, 0, sizeof(reply));
    int nDataReceived = 0;
    while (nDataReceived < sizeof(reply))
    {
      if (IsReadible(dwConnectionTimeout))
      {
        int nData = Receive(((BYTE*) &reply) + nDataReceived, sizeof(reply) - nDataReceived);
        nDataReceived += nData;
      }
      else
        AfxThrowWSocketException(WSAETIMEDOUT);
    }

    //Validate the response
    if ((bAuthenticate && ((reply.METHOD != 0) && (reply.METHOD != 2))) || (!bAuthenticate && (reply.METHOD != 0)))
      AfxThrowWSocketException(WSAECONNREFUSED);

    if (bAuthenticate && reply.METHOD == 2)
    {
      LPSTR pszAsciiUserName = T2A((LPTSTR) lpszUserName);
      LPSTR pszAsciiPassword = T2A((LPTSTR) lpszPassword);
      int nUserNameLength = (int) strlen(pszAsciiUserName);
      int nPasswordLength = 0;
      if (pszAsciiPassword)
        nPasswordLength = (int) strlen(pszAsciiPassword);

      if ((nUserNameLength > 255) || (nPasswordLength > 255))
        AfxThrowWSocketException(ERROR_INVALID_PARAMETER);

      int nUserRequestLength = 3 + nUserNameLength + nPasswordLength;
      BYTE* pUserRequest = new BYTE[nUserRequestLength];
      pUserRequest[0] = 1;
      pUserRequest[1] = (BYTE) nUserNameLength;
      CopyMemory(&(pUserRequest[2]), pszAsciiUserName, nUserNameLength);
      pUserRequest[2 + nUserNameLength] = (BYTE) nPasswordLength;
      CopyMemory(pUserRequest + 3 + nUserNameLength, pszAsciiPassword, nPasswordLength);
      Send(pUserRequest, nUserRequestLength);

      //Wait for the login reply
      WSOCKET_SOCKS5_USERNAME_AUTHENTICATION_REPLY reply;
      memset(&reply, 0, sizeof(reply));
      int nDataReceived = 0;
      while (nDataReceived < sizeof(reply))
      {
        if (IsReadible(dwConnectionTimeout))
        {
          int nData = Receive(((BYTE*) &reply) + nDataReceived, sizeof(reply) - nDataReceived);
          nDataReceived += nData;
        }
        else
          AfxThrowWSocketException(WSAETIMEDOUT);
      }

      if (reply.STATUS != 0)
        AfxThrowWSocketException(ERROR_ACCESS_DENIED);
    }

	  //Determine if the address is in dotted notation
	  LPSTR lpszAscii = T2A((LPTSTR) lpszHostAddress);
	  unsigned long nAddr = inet_addr(lpszAscii);
	  if (nAddr == INADDR_NONE)
	  {
      //verify that the host name is less than 256 bytes which is the limit of the hostname which Socks5 can accomadate
      int nHostLength = (int) strlen(lpszAscii);
      if (nHostLength > 255)
        AfxThrowWSocketException(ERROR_INVALID_PARAMETER);

      WSOCKET_SOCKS5_HOSTNAME_REQUEST_DETAILS requestDetails;
      memset(&requestDetails, 0, sizeof(requestDetails));
      requestDetails.Base.VER = 5;
      if (bUDP)
        requestDetails.Base.CMD = 3;
      else
        requestDetails.Base.CMD = 1;
      requestDetails.Base.ATYP = 3;
      requestDetails.DST_HOST.LENGTH = (BYTE) nHostLength;
      memcpy(requestDetails.DST_HOST.HOST, lpszAscii, nHostLength);
      WORD* pPort = (WORD*) (requestDetails.DST_HOST.HOST + nHostLength);
      *pPort = htons((u_short) nHostPort);
      int nRequestDetailsSize = sizeof(requestDetails) - 256 + nHostLength + 1;

      Send(&requestDetails, nRequestDetailsSize);
	  }
    else
    {
      WSOCKET_SOCKS5_IP4_REQUEST_DETAILS requestDetails;
      memset(&requestDetails, 0, sizeof(requestDetails));
      requestDetails.Base.VER = 5;
      requestDetails.Base.CMD = 1;
      requestDetails.Base.ATYP = 1;
      requestDetails.DST_IP.S_un.S_addr = nAddr;
      requestDetails.DSTPORT = htons((u_short) nHostPort);
      Send(&requestDetails, sizeof(requestDetails));
    }

    ReadSocks5ConnectReply(dwConnectionTimeout);
  }
  catch(CWSocketException* pEx)
  {
    //Close the socket before we rethrow the exception
    int nError = pEx->m_nError;
    pEx->Delete();
    Close();
    AfxThrowWSocketException(nError);
  }
}

void CWSocket::ReadSocks5ConnectReply(DWORD dwTimeout)
{
  //The local variables which will receive the data
  int nBufSize = max(sizeof(WSOCKET_SOCKS5_IP4_REQUEST_DETAILS), sizeof(WSOCKET_SOCKS5_HOSTNAME_REQUEST_DETAILS));
  BYTE* pRawRequest = new BYTE[nBufSize];
  
  //retrieve the reponse
  DWORD dwCurrentReadOffset = 0;
  BOOL bMoreDataToRead = TRUE;
	while (bMoreDataToRead)
	{
    if (IsReadible(dwTimeout))
    {
      int nBufRemaining = nBufSize - dwCurrentReadOffset;
      int nData = Receive(pRawRequest + dwCurrentReadOffset, nBufRemaining);

      //Increment the count of data received
		  dwCurrentReadOffset += nData;							   

      //Try to parse out what we received
      if (dwCurrentReadOffset >= sizeof(WSOCKET_SOCKS5_BASE_REQUEST_DETAILS)) 
      {
        WSOCKET_SOCKS5_BASE_REQUEST_DETAILS* pBaseRequest = (WSOCKET_SOCKS5_BASE_REQUEST_DETAILS*) pRawRequest;
        if (pBaseRequest->ATYP == 1)
        {
          //An IP 4 address type destination
          bMoreDataToRead = (dwCurrentReadOffset < sizeof(WSOCKET_SOCKS5_IP4_REQUEST_DETAILS));
          if (!bMoreDataToRead)
          {
            if (pBaseRequest->CMD != 0)
            {
              delete [] pRawRequest;
              AfxThrowWSocketException(ERROR_BAD_NET_RESP);
            }
          }
        }
        else if (pBaseRequest->ATYP == 3)
        {
          //A domain name type destination
          if (dwCurrentReadOffset > sizeof(WSOCKET_SOCKS5_BASE_REQUEST_DETAILS))
          {
            WSOCKET_SOCKS5_HOSTNAME_REQUEST_DETAILS* pHostnameRequest = (WSOCKET_SOCKS5_HOSTNAME_REQUEST_DETAILS*) pRawRequest;
            bMoreDataToRead = (dwCurrentReadOffset < ((sizeof(WSOCKET_SOCKS5_HOSTNAME_REQUEST_DETAILS) - 256) + pHostnameRequest->DST_HOST.LENGTH));
            if (!bMoreDataToRead)
            {
              if (pBaseRequest->CMD != 0)
              {
                delete [] pRawRequest;
                AfxThrowWSocketException(ERROR_BAD_NET_RESP);
              }
            }
          }
        }
        else
        {
          delete [] pRawRequest;
          AfxThrowWSocketException(ERROR_INVALID_PARAMETER);
        }
      }
    }
    else
    {
      delete [] pRawRequest;
      AfxThrowWSocketException(WSAETIMEDOUT);
    }
  }

  delete [] pRawRequest;
}

BOOL CWSocket::IsReadible(DWORD dwTimeout)
{
  ASSERT(IsCreated()); //must have been created first

  timeval timeout;
  timeout.tv_sec = dwTimeout/1000;
  timeout.tv_usec = (dwTimeout%1000)*1000;
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(m_hSocket, &fds);
  int nStatus = select(0, &fds, NULL, NULL, &timeout);
  if (nStatus == SOCKET_ERROR)
    AfxThrowWSocketException();

  return !(nStatus == 0);
}

BOOL CWSocket::IsWritable(DWORD dwTimeout)
{
  ASSERT(IsCreated()); //must have been created first

  timeval timeout;
  timeout.tv_sec = dwTimeout/1000;
  timeout.tv_usec = (dwTimeout%1000)*1000;
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(m_hSocket, &fds);
  int nStatus = select(0, NULL, &fds, NULL, &timeout);
  if (nStatus == SOCKET_ERROR)
    AfxThrowWSocketException();

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

void CWSocket::ConnectViaHTTPProxy(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszHTTPProxy, UINT nHTTPProxyPort, CString& sProxyResponse, LPCTSTR lpszUserName, LPCTSTR lpszPassword, DWORD dwConnectionTimeout, LPCTSTR lpszUserAgent)
{
  USES_CONVERSION;
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
      CBase64 base64;
      CString sUserNamePassword;
      sUserNamePassword.Format(_T("%s:%s"), lpszUserName, lpszPassword);
      char* pszUserNamePassword = T2A((LPTSTR) (LPCTSTR) sUserNamePassword);
      int nUserNamePasswordLength = (int) strlen(pszUserNamePassword);
      int nEncodedLength = base64.EncodeGetRequiredLength(nUserNamePasswordLength);
      LPSTR pszEncoded = (LPSTR) _alloca(nEncodedLength + 1);
      base64.Encode((const BYTE*) pszUserNamePassword, nUserNamePasswordLength, pszEncoded, &nEncodedLength);
      pszEncoded[nEncodedLength] = '\0';

      //Form the Authorization header line and add it to the request
      sLine.Format(_T("Proxy-authorization: Basic %s\r\n"), A2T(pszEncoded));
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
    LPSTR pszRequest = T2A((LPTSTR) (LPCTSTR) sRequest);
    Send(pszRequest, (int) strlen(pszRequest));

    //Read the proxy response
    ReadHTTPProxyResponse(dwConnectionTimeout, sProxyResponse);

    //Next make sure that we got a HTTP code of 200 to indicate success
    int nFirstSpace = sProxyResponse.Find(_T(" "));
    if (nFirstSpace != -1)
    {
      CString sResponseCode = sProxyResponse.Right(sProxyResponse.GetLength() - nFirstSpace - 1);
      int nResponseCode = _ttoi(sResponseCode);
      if (nResponseCode != 200)
        AfxThrowWSocketException(ERROR_CONNECTION_REFUSED);
    }
  }
  catch(CWSocketException* pEx)
  {
    //Close the socket before we rethrow the exception
    int nError = pEx->m_nError;
    pEx->Delete();
    Close();
    AfxThrowWSocketException(nError);
  }
}

void CWSocket::ReadHTTPProxyResponse(DWORD dwTimeout, CString& sResponse)
{
  USES_CONVERSION;
  ASSERT(IsCreated()); //must have been created first

  //The local variables which will receive the data
  DWORD dwGrowBy = 4096;
  BYTE* pRawRequest = new BYTE[dwGrowBy];
  DWORD dwBufSize = dwGrowBy;
  DWORD dwRawRequestSize = 0;
  
  //retrieve the reponse
  BOOL bMoreDataToRead = TRUE;
	while (bMoreDataToRead)
	{
    //check the socket for readability
    if (!IsReadible(dwTimeout))
    {
      //Null terminate the data
  	  pRawRequest[dwRawRequestSize] = '\0';
      TRACE(_T("CWSocket::ReadHTTPProxyResponse, Timed out waiting for response from socket\n"));
			AfxThrowWSocketException(WSAETIMEDOUT);
    }

		//receive the data from the socket
    dwRawRequestSize += Receive(pRawRequest + dwRawRequestSize, 1);

    //NULL terminate the data received
	  pRawRequest[dwRawRequestSize] = '\0';

    //Check to see if the terminator character(s) have been found
    if (dwRawRequestSize >= 4)
    {
      bMoreDataToRead = !((pRawRequest[dwRawRequestSize-4] == '\r') && (pRawRequest[dwRawRequestSize-3] == '\n') &&
                          (pRawRequest[dwRawRequestSize-2] == '\r') && (pRawRequest[dwRawRequestSize-1] == '\n'));
    }

    if (dwRawRequestSize >= dwBufSize) //No space left in the current buffer
    {
      //Allocate the new receive buffer
      dwBufSize += dwGrowBy; //Grow the buffer by the specified amount
      LPBYTE pNewBuf = new BYTE[dwBufSize];

      //copy the old contents over to the new buffer and assign 
      //the new buffer to the local variable used for retreiving 
      //from the socket
      CopyMemory(pNewBuf, pRawRequest, dwRawRequestSize);
      delete [] pRawRequest;
      pRawRequest = pNewBuf;
    }
	}

  //Form the CString out parameter
  sResponse = A2T((char*) pRawRequest);
}
