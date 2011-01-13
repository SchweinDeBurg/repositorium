/*
Module : OpenSSLMfc.H
Purpose: Defines the interface for wrapper classes for the OpenSSL C variable types
Created: PJN / 24-05-2002

Copyright (c) 2002 - 2011 by PJ Naughter.  

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

#ifndef __OPENSSLMFC_H__
#define __OPENSSLMFC_H__

#ifndef SOCKMFC_EXT_CLASS
#define SOCKMFC_EXT_CLASS
#endif


/////////////////////////////// Includes //////////////////////////////////////

#include <openssl\ssl.h> //If you get a compilation error about this missing header file, then you need to download OpenSSL from http://www.openssl.org or http://www.slproweb.com/products/Win32OpenSSL.html and incorporate it into your project
#include <openssl\err.h> //If you get a compilation error about this missing header file, then you need to download OpenSSL from http://www.openssl.org or http://www.slproweb.com/products/Win32OpenSSL.html and incorporate it into your project
#include "SocMFC.h"


/////////////////////////////// Classes ///////////////////////////////////////

//class which encapsulates the OpenSSL SSL_CTX* variable
class SOCKMFC_EXT_CLASS CSSLContext 
{
public:
//Constructors / Destructors
  CSSLContext();
  CSSLContext(SSL_CTX* pSSLContext);
  ~CSSLContext();

//Methods
  void Close();
  void Attach(SSL_CTX* pSSLContext);
  SSL_CTX* Detach();

//operators
  operator SSL_CTX*() const;

//Member variables
  SSL_CTX* m_pSSLContext; //The underlying OpenSSL context variable
  
private:
//Methods
  CSSLContext(const CSSLContext& sslContext); //Implemented as private to ensure that instances of this class are non-copyable
  CSSLContext& operator=(const CSSLContext& sslContext); //Implemented as private to ensure that instances of this class are non-copyable
};

//class which encapsulates the OpenSSL SSL* variable
class SOCKMFC_EXT_CLASS CSSL 
{
public:
//Constructors / Destructors
  CSSL();
  CSSL(SSL* pSSLContext);
  ~CSSL();

//Methods
  void Close();
  void Attach(SSL* pSSL);
  SSL* Detach();

//operators
  operator SSL*() const;

//Member variables
  SSL* m_pSSL; //The underlying OpenSSL "SSL" variable
  
private:
//Methods
  CSSL(const CSSL& ssl); //Implemented as private to ensure that instances of this class are non-copyable
  CSSL& operator=(const CSSL& ssl); //Implemented as private to ensure that instances of this class are non-copyable
};

//Class which encapsulates a SSL socket, in OpenSSL parlance, this is an SSL* struct (aka CSSL) doing file IO over a socket (aka CWSocket)
class SOCKMFC_EXT_CLASS CSSLSocket
{
public:
//Constructors / Destructors
  CSSLSocket();
  virtual ~CSSLSocket();

//General Methods
  BOOL    Create(CSSLContext& sslContext, CWSocket& socket);
  BOOL    Connect(LPCTSTR pszHostAddress, UINT nHostPort, int nSocketType = SOCK_STREAM);
  BOOL    Connect(LPCTSTR pszHostAddress, LPCTSTR pszPortOrServiceName, int nSocketType = SOCK_STREAM);
  BOOL    Accept(DWORD dwSSLNegotiationTimeout);
  void    Close();
  int     Send(const void* pBuffer, int nBuf);
  int     Receive(void* pBuffer, int nBuf);
  BOOL    IsReadible(DWORD dwTimeout);

//operators
  operator SSL*() const;
  operator CWSocket&() const;

protected:
//Member variables
  CSSL      m_SSL;
  CWSocket* m_pSocket;
  
private:
//Methods
  CSSLSocket(const CSSLSocket& socket); //Implemented as private to ensure that instances of this class are non-copyable
  CSSLSocket& operator=(const CSSLSocket& socket); //Implemented as private to ensure that instances of this class are non-copyable
};

#endif //__OPENSSLMFC_H__
