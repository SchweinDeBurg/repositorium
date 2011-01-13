/*
Module : W3MFC.h
Purpose: Defines the interface for a simple MFC class encapsulation of a HTTP server

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

#ifndef __W3MFC_H__
#define __W3MFC_H__

#ifndef W3MFC_EXT_CLASS
#define W3MFC_EXT_CLASS
#endif


/////////////////////////////// Includes //////////////////////////////////////

#include "W3MFCMimeManager.h"
#ifndef W3MFC_NO_ISAPI_SUPPORT
#include "W3MFCISAPI.h"
#include "W3MFCISAPIManager.h"
#endif
#ifndef W3MFC_NO_CGI_SUPPORT
#include "W3MFCCGI.h"
#endif
#include "W3MFCDirectory.h"
#include "ThrdPool.h"
#include "DirectedThreadPoolQueue.h"
#include "IOCPThreadPoolQueue.h"

#ifndef W3MFC_NO_SSPI_SUPPORT
  #ifndef __SSPI_H__
  #include <sspi.h>
  #pragma message("To avoid this message, please put SSPI.h in your pre compiled header (normally stdafx.h)")
  #endif
#endif

#ifndef _WINSOCK2API_
#include <winsock2.h>
#pragma message("To avoid this message, please put winsock2.h in your pre compiled header (normally stdafx.h)")
#endif


/////////////////////////////// Classes ///////////////////////////////////////

//The settings which the web server uses in the call to CW3MFCServer::Start
class W3MFC_EXT_CLASS CW3MFCServerSettings
{
public:
//Constructors / Destructors
  CW3MFCServerSettings();
  virtual ~CW3MFCServerSettings();

//Methods
  void FreeDirectoryArray();                                                        //Frees up the memory allocated in m_Directories
                                                                                  
//Member variables                                                                
  unsigned short                              m_nPort;                              //The port on which to run the web server
  CArray<CW3MFCDirectory*, CW3MFCDirectory*&> m_Directories;                        //Directories served up by this server
  BOOL                                        m_bBind;                              //Should the server be bound to an address
  BOOL                                        m_bIPv6;
  CString                                     m_sBindAddress;                       //The IP address to bind to (if m_bBind is set)
  DWORD                                       m_dwIdleClientTimeout;                //Timeout in ms to wait for client requests
  CString                                     m_sServerName;                        //The Web server name to return in HTTP headers
  CRuntimeClass*                              m_pRuntimeClientClass;                //The runtime class of the client class to use, normally is CW3MFCClient
  CString                                     m_sUserName;                          //The account to run the web server under
  CString                                     m_sPassword;                          //The account's password
  int                                         m_nThreadPoolSize;                    //The size of the thread pool to use
  BOOL                                        m_bDNSLookup;                         //Should we do reverse DNS lookups on client addresses when connecting
  DWORD                                       m_dwWritableTimeout;                  //Timeout in ms to wait for client sockets to become writable
  BOOL                                        m_bEnableThreadLifetime;              //TRUE if threads are to be recycled in the thread pool
  DWORD                                       m_dwThreadLifetime;                   //The lifetime for threads (in Minutes) if recycling is enabled
  CW3MFCMimeManager*                          m_pMimeManager;                       //The Mime manager to use
  BOOL                                        m_bUseIOCPQueue;                      //Should CIOCPThreadPoolQueue be used in preference to CDirectedThreadPoolQueue.
                                                                                    //Please note that the IOCP class does not support thread recycling
  BOOL                                        m_bKeepAlives;                        //Should the server observe client "ConnectionL Keep-Alive" requests 
  BOOL                                        m_bAutoExpire;                        //Should all reponses be marked with an immediate "Expires" header
  BOOL                                        m_bAllowDeleteRequest;                //Should the "DELETE" request be allowed
#ifndef W3MFC_NO_CGI_SUPPORT                                                      
  DWORD                                       m_dwCGIResponseBufferSize;            //The size of the buffer to use when reading the data from CGI programs. Note that
                                                                                    //this is important value as the code will only look for Keep Alive headers
                                                                                    //in the first buffer returned. So make sure this value is bigger than any 
                                                                                    //of the headers which any of your CGI programs generate
#endif                                                                            
  BOOL                                        m_bAllowAnonymous;                    //Should we support anonymous access
  BOOL                                        m_bAllowBasicAuthentication;          //Should we support Basic authentication
  BOOL                                        m_bAllowNTLMAuthentication;           //Should we support NTLM authentication
  BOOL                                        m_bPerformPassthroughAuthentication;  //Should the credentials sent to us by the client, result in us calling thro to the OS to login that user
#ifndef W3MFC_NO_ISAPI_SUPPORT
  BOOL                                        m_bCacheISAPI;                        //Should we cache ISAPI extensions
  CW3MFCISAPIManager*                         m_pISAPIManager;                      //The ISAPI Extension manager to use
  CW3MFCISAPI*                                m_pISAPI;                             //The actual ISAPI class to use
#endif                                                                            
#ifndef W3MFC_NO_CGI_SUPPORT                                                      
  CW3MFCCGI*                                  m_pCGI;                               //The actual CGI class to use
#endif
#ifdef W3MFC_SSL_SUPPORT
  enum SSL_PROTOCOL
  {
   SSL_NONE = 0, 
   SSL_V2 = 1,
   SSL_V3 = 2,
   SSL_V2_OR_V3 = 3,
   TLS_V1 = 4
  }                                         m_SSLProtocol;              //The SSL protocol to use
  CString                                   m_sServerCertificateFile;   //The filename of the PEM file which contains the server's certificate
  CString                                   m_sPrivateKeyFile;          //The filename of the PEM file which contains the server's private 
                                                                        //key, if this value is empty, then the private key is expected to 
                                                                        //be stored in the Server's certificate file "m_sServerCertificateFile"
  BOOL                                      m_bReuseSessions;           //Should SSL sessions be reused
  DWORD                                     m_dwSSLSessionTimeout;      //The SSL Session timeout value in seconds. Default value is 300 seconds.
  DWORD                                     m_dwSSLNegotiationTimeout;  //The timeout in milliseconds to allow for SSL negotiation
  CString                                   m_sSSLRandomnessFile;       //The filename to initialize the SSL PRNG with.
#endif //W3MFC_SSL_SUPPORT
};

//Class which handles Directed requests for the web server
class W3MFC_EXT_CLASS CW3MFCDirectedThreadPoolQueue : public CDirectedThreadPoolQueue
{
public:
//Constructors / Destructors
  virtual ~CW3MFCDirectedThreadPoolQueue();

protected:
  DECLARE_DYNCREATE(CW3MFCDirectedThreadPoolQueue)
};

//The actual web server
class W3MFC_EXT_CLASS CW3MFCServer : public CObject
{
public:
//Constructors / Destructors
  CW3MFCServer();
  virtual ~CW3MFCServer();

//Methods
  virtual BOOL         Start(CW3MFCServerSettings* pSettings);
  virtual BOOL         Stop();
  virtual BOOL         Wait();
  virtual void         OnError(const CString& sError);
  static BOOL          ReverseDNSLookup(in_addr sin_addr, CString& sDomainName);
  virtual CStringA     GetFileDeletedHTML();
#ifdef W3MFC_SSL_SUPPORT
  virtual void LogSSLErrors();
#endif
  virtual void     LoadHTMLResources();
  virtual CStringA LoadHTML(int nStatusCode);
  virtual BOOL     LoadHTMLResource(UINT nID, CStringA& sHTML);

//Accessors
  CW3MFCServerSettings* GetSettings() const { return m_pSettings; };
  CThreadPoolServer&    GetThreadPool() { return m_ThreadPool; };
  HANDLE                GetImpersonationHandle() const { return m_hImpersonation; };
#ifndef W3MFC_NO_SSPI_SUPPORT
  CredHandle*           GetCredentialHandle() { return &m_hCredHandle; };
#endif

protected:
  DECLARE_DYNAMIC(CW3MFCServer)

//Methods
  static UINT ListenSocketFunction(LPVOID pParam);
  virtual void ListenSocketFunction();
#ifndef W3MFC_NO_SSPI_SUPPORT
  virtual BOOL AcquireSSPI();
  virtual void ReleaseSSPI();
#endif

//Member variables
  CWinThread*           m_pListenThread;
  CCriticalSection      m_csListenThread;
  CEvent                m_ListenStartEvent;
  CEvent                m_StopEvent;
  CEvent                m_SocketEvent;
  BOOL                  m_bListenerRunningOK;
  CW3MFCServerSettings* m_pSettings;
  CThreadPoolServer     m_ThreadPool;
  ATL::CHandle          m_hImpersonation;
#ifndef W3MFC_NO_SSPI_SUPPORT
  CredHandle            m_hCredHandle;
#endif
  CStringA              m_s302HTML;
  CStringA              m_s400HTML;
  CStringA              m_s401HTML;
  CStringA              m_s404HTML;
  CStringA              m_s500HTML;
  CStringA              m_sDeletedHTML;
};

#endif //__W3MFC_H__
