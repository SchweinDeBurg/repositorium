/*
Module : SocMFC.H
Purpose: Interface for an MFC wrapper class for sockets
Created: PJN / 05-08-1998

Copyright (c) 2002 - 2011 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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

#ifndef __SOCMFC_H__
#define __SOCMFC_H__

#ifndef SOCKMFC_EXT_CLASS
#define SOCKMFC_EXT_CLASS
#endif

__if_not_exists(ADDRESS_FAMILY)
{
  typedef USHORT ADDRESS_FAMILY;
}

__if_not_exists(SOCKADDR_INET)
{
  typedef union _SOCKADDR_INET {
      SOCKADDR_IN Ipv4;
      SOCKADDR_IN6 Ipv6;
      ADDRESS_FAMILY si_family;    
  } SOCKADDR_INET, *PSOCKADDR_INET;
}


////////////////////////////// Includes ///////////////////////////////////////

#ifndef _WINSOCK2API_
#pragma message("To avoid this message, please put winsock2.h in your pre compiled header (usually stdafx.h)")
#include <winsock2.h>
#endif

#include <sal.h>


////////////////////////////// Classes ////////////////////////////////////////

class SOCKMFC_EXT_CLASS CWSocketException : public CException
{
public:
//Constructors / Destructors
	CWSocketException(int nError);

//Methods
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL GetErrorMessage(__out_ecount_z(nMaxError) LPTSTR lpstrError, __in UINT nMaxError, __out_opt PUINT pnHelpContext = NULL);
	CString GetErrorMessage();

//Data members
	int m_nError;

protected:
	DECLARE_DYNAMIC(CWSocketException)
};

class SOCKMFC_EXT_CLASS CWSocket
{
public:
//Constructors / Destructors
  CWSocket();
  virtual ~CWSocket();

//Attributes
  void    Attach(SOCKET hSocket);
  SOCKET  Detach(); 
  void    GetPeerName(CString& sPeerAddress, UINT& nPeerPort);
  void    GetPeerName(SOCKADDR* pSockaddr, int* pSockAddrLen);
  void    GetSockName(CString& sSocketAddress, UINT& nSocketPort);
  void    GetSockName(SOCKADDR* pSockAddr, int* pSockAddrLen);
  void    SetSockOpt(int nOptionName, const void* pOptionValue, int nOptionLen, int nLevel = SOL_SOCKET);
  void    GetSockOpt(int nOptionName, void* pOptionValue, int* pOptionLen, int nLevel = SOL_SOCKET);
  BOOL    IsCreated() const; 
  BOOL    IsReadible(DWORD dwTimeout);
  BOOL    IsWritable(DWORD dwTimeout);

//Methods
  void    Create(BOOL bUDP = FALSE, BOOL bIPv6 = FALSE);
  void    Create(int nSocketType, int nProtocolType, int nAddressFormat);
  __declspec(deprecated) void Accept(CWSocket& connectedSocket, SOCKADDR_IN& clientAddress);
  void    Accept(CWSocket& connectedSocket, SOCKADDR* pSockAddr = NULL, int* pSockAddrLen = NULL);
  __declspec(deprecated) void Bind(UINT nSocketPort, LPCTSTR pszSocketAddress = NULL);
  void    CreateAndBind(UINT nSocketPort, LPCTSTR pszSocketAddress = NULL, int nSocketType = SOCK_STREAM, int nDefaultAddressFormat = AF_INET);
  void    Bind(const SOCKADDR* pSockAddr, int nSockAddrLen);
  void    Close();
  void    Connect(const SOCKADDR* pSockAddr, int nSockAddrLen);
  __declspec(deprecated) void Connect(LPCTSTR pszHostAddress, UINT nHostPort);
  void    CreateAndConnect(LPCTSTR pszHostAddress, UINT nHostPort, int nSocketType = SOCK_STREAM);
  void    CreateAndConnect(LPCTSTR pszHostAddress, LPCTSTR pszPortOrServiceName, int nSocketType = SOCK_STREAM);
  void    Connect(const SOCKADDR* pSockAddr, int nSockAddrLen, DWORD dwTimeout, BOOL bResetToBlockingMode = TRUE);
  void    CreateAndConnect(LPCTSTR pszHostAddress, UINT nHostPort, DWORD dwTimeout, BOOL bResetToBlockingMode, int nSocketType = SOCK_STREAM);
  void    CreateAndConnect(LPCTSTR pszHostAddress, LPCTSTR pszPortOrServiceName, DWORD dwTimeout, BOOL bResetToBlockingMode, int nSocketType = SOCK_STREAM);
  void    ConnectViaSocks4(LPCTSTR pszHostAddress, UINT nHostPort, LPCTSTR pszSocksServer, UINT nSocksPort = 1080, DWORD dwTimeout = 5000);
  void    ConnectViaSocks5(LPCTSTR pszHostAddress, UINT nHostPort, LPCTSTR pszSocksServer, UINT nSocksPort = 1080, LPCTSTR pszUserName = NULL, LPCTSTR pszPassword = NULL, DWORD dwTimeout = 5000, BOOL bUDP = FALSE);
  void    ConnectViaHTTPProxy(LPCTSTR pszHostAddress, UINT nHostPort, LPCTSTR pszHTTPServer, UINT nHTTPProxyPort, CStringA& sProxyResponse, LPCTSTR pszUserName = NULL, LPCTSTR pszPassword = NULL, DWORD dwTimeout = 5000, LPCTSTR pszUserAgent = NULL);
  void    IOCtl(long lCommand, DWORD* pArgument);
  void    Listen(int nConnectionBacklog = SOMAXCONN);
  int     Receive(void* pBuf, int nBufLen, int nFlags = 0);
  int     ReceiveFrom(void* pBuf, int nBufLen, SOCKADDR* pSockAddr, int* pSockAddrLen, int nFlags = 0);
  int     ReceiveFrom(void* pBuf, int nBufLen, CString& sSocketAddress, UINT& nSocketPort, int nFlags = 0);
  int     Send(const void* pBuffer, int nBufLen, int nFlags = 0);
  int     SendTo(const void* pBuf, int nBufLen, const SOCKADDR* pSockAddr, int nSockAddrLen, int nFlags = 0);
  int     SendTo(const void* pBuf, int nBufLen, UINT nHostPort, LPCTSTR pszHostAddress = NULL, int nFlags = 0);
  enum { receives = 0, sends = 1, both = 2 };
  void    ShutDown(int nHow = sends);

//Operators
  operator SOCKET();

//Static methods
  static void ThrowWSocketException(int nError = 0);
  static CString AddressToString(const SOCKADDR_INET& sockAddr);
  __forceinline static void SecureEmptyString(CStringA& sVal)
  {
    int nLength = sVal.GetLength();
    if (nLength)
    {
      LPSTR pszVal = sVal.GetBuffer(nLength);
      SecureZeroMemory(pszVal, nLength);
      sVal.ReleaseBuffer();
    }
  }

  __forceinline static void SecureEmptyString(CStringW& sVal)
  {
    int nLength = sVal.GetLength();
    if (nLength)
    {
      LPWSTR pszVal = sVal.GetBuffer(nLength);
      SecureZeroMemory(pszVal, nLength*sizeof(wchar_t));
      sVal.ReleaseBuffer();
    }
  }

  __forceinline static void SecureEmptyString(LPSTR sVal)
  {
    size_t nLen = strlen(sVal);
    if (nLen)
      SecureZeroMemory(sVal, nLen);
  }

protected:
//Methods
  void _Connect(LPCTSTR pszHostAddress, LPCTSTR pszPortOrServiceName);
  void _Bind(UINT nSocketPort, LPCTSTR pszSocketAddress);
  void ReadHTTPProxyResponse(DWORD dwTimeout, CStringA& sResponse);
  void ReadSocks5ConnectReply(DWORD dwTimeout);

//Member variables
  SOCKET m_hSocket;
};

#endif //__SOCMFC_H__
