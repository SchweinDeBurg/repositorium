/*
Module : W3MFCClient.cpp
Purpose: Implementation for the CW3MFCClient class
Created: PJN / 22-04-1999
History  PJN / 19-02-2006 1. Replaced all calls to ZeroMemory and CopyMemory with memset and memcpy
         PJN / 27-06-2006 1. Combined the functionality of the _W3MFC_DATA class into the main CW3MFCClient class.
         PJN / 19-11-2007 1. CHttpClient class has been renamed to CW3MFCClient
         PJN / 18-02-2008 1. Fixed a memory leak in CW3MFCClient::_TransmitFile
         PJN / 31-05-2008 1. Code now compiles cleanly using Code Analysis (/analyze)
         PJN / 23-05-2009 1. Reworked all token parsing code to use CString::Tokenize
         PJN / 07-09-2009 1. Fixed a debug mode ASSERT when calling TRACE in CW3MFCClient::PostLog

Copyright (c) 1999 - 2009 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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
#include "W3MFC.h"
#include "W3MFCResponseHeader.h"
#include "W3MFCClient.h"

#ifndef __ATLENC_H__
#pragma message("To avoid this message, please put atlenc.h in your pre compiled header (usually stdafx.h)")
#include <atlenc.h>
#endif


//////////////// Macros / Defines /////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////// Implementation ///////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CW3MFCClient, CThreadPoolClient)

CW3MFCClient::CW3MFCClient() : m_lpfnTransmitFile(NULL),
                               m_lpfnTransmitPackets(NULL),
                               m_pServer(NULL),
                               m_hMSWsock(NULL),
                             #ifdef W3MFC_SSL_SUPPORT
                               m_pSSLContext(NULL),
                             #endif
                             #ifndef W3MFC_NO_ISAPI_SUPPORT
                               m_dwDataSentViaWriteClient(0),
                               m_nHttpStatusCodeSent(0),
                             #endif
                               m_bResponseKeepAlive(FALSE)
{
  //Only use the TransmitFile and TransmitPacket API's if we are running on Win2K or later
  OSVERSIONINFO osvi;
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  BOOL bNT = (GetVersionEx(&osvi) && osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);
  if (bNT && (osvi.dwMajorVersion >= 5))
  {
    m_hMSWsock = LoadLibrary(_T("MSWSOCK.DLL"));
    if (m_hMSWsock)
    {
      m_lpfnTransmitFile = reinterpret_cast<LPFN_TRANSMITFILE>(GetProcAddress(m_hMSWsock, "TransmitFile"));
      m_lpfnTransmitPackets = reinterpret_cast<LPFN_TRANSMITPACKETS>(GetProcAddress(m_hMSWsock, "TransmitPackets"));
    }
  }
}

CW3MFCClient::~CW3MFCClient()
{
  if (m_hMSWsock)
  {
    FreeLibrary(m_hMSWsock);
    m_hMSWsock = NULL;
  }
}

#ifndef W3MFC_NO_ISAPI_SUPPORT
BOOL CW3MFCClient::_TransmitFile(CW3MFCSocket& socket, CW3MFCResponseHeader& responseHdr, HSE_TF_INFO* pInfo)
{
  //Assume the worst
  BOOL bSuccess = FALSE;

  if (pInfo->pfnHseIO)
  {
    SetLastError(ERROR_INVALID_PARAMETER); //we do not support ascynchronous notifications
    return FALSE;
  }
  if (pInfo->Offset)
  {
    SetLastError(ERROR_INVALID_PARAMETER); //we do not support partials sends thro TransmitFile because that requires overlapped IO
    return FALSE;
  }

  if (pInfo->dwFlags & HSE_IO_SEND_HEADERS)
  {
    if (m_lpfnTransmitPackets && m_lpfnTransmitFile)
    {
      TRANSMIT_PACKETS_ELEMENT tpe[2];
      tpe[0].pBuffer = static_cast<void*>(const_cast<LPSTR>(pInfo->pszStatusCode));
      tpe[0].dwElFlags = TP_ELEMENT_MEMORY;
      tpe[0].cLength = static_cast<ULONG>(strlen(pInfo->pszStatusCode));

      CStringA sHeader(responseHdr.GetData());
      tpe[1].cLength = sHeader.GetLength();
      tpe[1].pBuffer = sHeader.GetBuffer();
      tpe[1].dwElFlags = TP_ELEMENT_MEMORY;

      //Call the TransmitPackets function
      bSuccess = m_lpfnTransmitPackets(socket, tpe, 2, 0, NULL, TF_USE_KERNEL_APC);
      sHeader.ReleaseBuffer();

      //And the TransmitFile function
      if (bSuccess)
        bSuccess = m_lpfnTransmitFile(socket, pInfo->hFile, pInfo->BytesToWrite, 0, NULL, NULL, TF_USE_KERNEL_APC);
    }
    else
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  }
  else
  {
    if (m_lpfnTransmitFile)
    {
      TRANSMIT_FILE_BUFFERS tfb;
      tfb.Head = pInfo->pHead;
      tfb.HeadLength = pInfo->HeadLength;
      tfb.Tail = pInfo->pTail;
      tfb.TailLength = pInfo->TailLength;

      //Call the TransmitFile function
      bSuccess = m_lpfnTransmitFile(socket, pInfo->hFile, pInfo->BytesToWrite, 0, NULL, &tfb, TF_USE_KERNEL_APC);
    }
    else
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  }

  return bSuccess;
}
#endif

BOOL CW3MFCClient::_TransmitFile(CW3MFCSocket& socket, CW3MFCResponseHeader& responseHdr, HANDLE hFile, DWORD dwSize)
{
  //Validate our parameter
  ASSERT(m_lpfnTransmitFile);

  //Setup the TFB ready for the call to the "TransmitFile" function
  TRANSMIT_FILE_BUFFERS tfb;
  CStringA sData(responseHdr.GetData());
  tfb.HeadLength = sData.GetLength();
  tfb.Head = sData.GetBuffer();
  tfb.Tail = NULL;
  tfb.TailLength = 0;

  //Call the TransmitFile function
  BOOL bSuccess = m_lpfnTransmitFile(socket, hFile, dwSize, 0, NULL, &tfb, TF_USE_KERNEL_APC);
  
  sData.ReleaseBuffer();
  
  return bSuccess;
}

BOOL CW3MFCClient::_TransmitBuffer(CW3MFCSocket& socket, CW3MFCResponseHeader& responseHdr, BYTE* byData, DWORD dwSize)
{
  //Validate our parameters
  ASSERT(m_lpfnTransmitPackets);

  //Setup the TFB ready for the call to the "TransmitPackets" function
  TRANSMIT_PACKETS_ELEMENT tpe[2];
  CStringA sData(responseHdr.GetData());
  tpe[0].pBuffer = sData.GetBuffer();
  tpe[0].dwElFlags = TP_ELEMENT_MEMORY;
  tpe[1].pBuffer = reinterpret_cast<void*>(byData);
  tpe[1].cLength = dwSize;
  tpe[0].dwElFlags = TP_ELEMENT_MEMORY;

  //Call the TransmitPackets function
  BOOL bSuccess = m_lpfnTransmitPackets(socket, tpe, 2, 0, NULL, TF_USE_KERNEL_APC);
  
  sData.ReleaseBuffer();
  
  return bSuccess;
}

BOOL CW3MFCClient::AllowThisConnection()
{
  return TRUE;
}

#ifdef W3MFC_SSL_SUPPORT
BOOL CW3MFCClient::InitializeSSLConnection()
{
  SSL* pSSL = SSL_new(m_pSSLContext->operator SSL_CTX*());
  if (pSSL == NULL)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCClient::InitializeSSLConnection, Failed to create create SSL connection object"));
    AFXASSUME(m_pServer);
    m_pServer->OnError(sError);

    m_pServer->LogSSLErrors();

    return FALSE;
  }
  else
    m_SSL.Attach(pSSL);

  //Associate the raw socket with the SSL connection object
  if (SSL_set_fd(m_SSL, m_Socket) != 1)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCClient::InitializeSSLConnection, Failed to create create SSL connection object"));
    AFXASSUME(m_pServer);
    m_pServer->OnError(sError);

    m_pServer->LogSSLErrors();

    return FALSE;
  }

  return TRUE;
}

BOOL CW3MFCClient::DoSSLNegotiation()
{
  //Validate our parameters
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);

  BOOL bNegotiationComplete = FALSE;
  while (!bNegotiationComplete)
  {
    int nSSLAccept = SSL_accept(m_SSL);
    if (nSSLAccept != 1)
    {
      BOOL bRetry = FALSE;
      int nSSL_get_error = SSL_get_error(m_SSL, nSSLAccept);
      if (nSSL_get_error == SSL_ERROR_WANT_READ)
      {
        if (m_Socket.IsReadible(pSettings->m_dwSSLNegotiationTimeout))
          bRetry = TRUE;
      }

      if (!bRetry)
      {
        //Report the error
        CString sError;
        sError.Format(_T("CW3MFCClient::DoSSLNegotiation, Failed to perform SSL handshake, SSL_accept:%d SSL_get_error:%d"), nSSLAccept, nSSL_get_error);
        m_pServer->OnError(sError);

        m_pServer->LogSSLErrors();

        return FALSE;
      }
    }
    else
      bNegotiationComplete = TRUE;
  }

  return TRUE;
}
#endif

BOOL CW3MFCClient::Run(const CThreadPoolRequest& request)
{
  //Validate our parameters
  ASSERT(request.m_pData);
  CW3MFCThreadPoolRequest* pW3MFCRequest = static_cast<CW3MFCThreadPoolRequest*>(request.m_pData);
  AFXASSUME(pW3MFCRequest);

  //Hive away the parameters in member variables
  SOCKET clientSocket = pW3MFCRequest->m_ClientSocket.Detach();
  m_Socket.Attach(clientSocket);
  memcpy(&m_Request.m_ClientAddress, &pW3MFCRequest->m_ClientAddress, sizeof(sockaddr_in));
#ifdef W3MFC_SSL_SUPPORT
  m_pSSLContext = pW3MFCRequest->m_pSSLContext;
#endif

  //Call the helper function which does all of the work
  HandleClient();

#ifdef W3MFC_SSL_SUPPORT
  //Close the SSL connection
  m_SSL.Close();
#endif

  //Close down the connection
  m_Socket.Close();

  //Tidy up our heap memory after ourselves
  delete pW3MFCRequest;

  //Reset the request data
  m_Request = CW3MFCRequest();

  return TRUE;
}

int CW3MFCClient::ExitInstance()
{
  //Tidy up per thread SSL structures if we are using SSL
#ifdef W3MFC_SSL_SUPPORT
  ERR_remove_state(0);
#endif

  //Let the base class do its thing
  return CThreadPoolClient::ExitInstance();
}

void CW3MFCClient::HandleClient()
{
  //Validate our parameters
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);

  //Do the reverse DNS lookup if configured to do so
  m_Request.m_sRemoteHost.Empty();
  if (pSettings->m_bDNSLookup)
  {
    HOSTENT* pHostEnt = gethostbyaddr(reinterpret_cast<const char*>(&m_Request.m_ClientAddress.sin_addr), sizeof(IN_ADDR), AF_INET);
    if (pHostEnt)
      m_Request.m_sRemoteHost = pHostEnt->h_name;
  }

  //Should we allow this client to connect
  if (!AllowThisConnection())
  {
    ReturnErrorMessage(400); //Bad Request
    return;    
  }

  //Create the SSL connection if required
#ifdef W3MFC_SSL_SUPPORT
  if (pSettings->m_SSLProtocol != CW3MFCServerSettings::SSL_NONE)
  {
    if (!InitializeSSLConnection())
      return;

    //Do the SSL negotiation
    if (!DoSSLNegotiation())
      return;
  }
#endif

  //Use a Win32 event notification on the socket
  CEvent dataEvent;
  int nError = WSAEventSelect(m_Socket, dataEvent, FD_READ | FD_CLOSE);
  if (nError == SOCKET_ERROR)
  {
    DWORD dwError = ::GetLastError();

    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCClient::HandleClient, Failed in call to WSAEventSelect, GetLastError:%d"), dwError);
    m_pServer->OnError(sError);

    return;
  }

  //Also create a waitable timer if we can
  CWaitableTimer dataTimer;
  if (dataTimer.Create(TRUE))
    dataTimer.SetOnceOffRelative(pSettings->m_dwIdleClientTimeout);

  BOOL bMoreRequests = FALSE;
#ifndef W3MFC_NO_SSPI_SUPPORT
  BOOL bImpersonatedUsingSSPI = FALSE;
#endif
  do
  {
    m_bResponseKeepAlive = FALSE;

  #ifdef W3MFC_SSL_SUPPORT
    //Read the client request
    BOOL bReadRequest = FALSE;
    if (dataTimer)
      bReadRequest = m_Socket.ReadRequest(m_Request, pSettings->m_dwIdleClientTimeout, 4096, m_SSL, *this, dataTimer, m_StopEvent, dataEvent);
    else
      bReadRequest = m_Socket.ReadRequest(m_Request, pSettings->m_dwIdleClientTimeout, 4096, m_SSL, *this);
  #else
    BOOL bReadRequest = FALSE;
    if (dataTimer)
      bReadRequest = m_Socket.ReadRequest(m_Request, pSettings->m_dwIdleClientTimeout, 4096, *this, dataTimer, m_StopEvent, dataEvent);
    else
      bReadRequest = m_Socket.ReadRequest(m_Request, pSettings->m_dwIdleClientTimeout, 4096, *this);
  #endif
    if (bReadRequest)
    {
      //Parse the client request
      if (ParseRequest())
      {
        m_bResponseKeepAlive = pSettings->m_bKeepAlives && m_Request.m_bKeepAlive;

        //Impersonate the client credentials if authorization type is PLAINTEXT
        ATL::CHandle hImpersonation;
        BOOL bLoggedOn = FALSE;
        if (m_Request.m_AuthorizationType == CW3MFCRequest::HTTP_AUTHORIZATION_PLAINTEXT && pSettings->m_bPerformPassthroughAuthentication)
        {
          LPTSTR pszUser = m_Request.m_sUserName.GetBuffer(m_Request.m_sUserName.GetLength());
          LPTSTR pszPassword = m_Request.m_sPassword.GetBuffer(m_Request.m_sPassword.GetLength());
          bLoggedOn = LogonUser(pszUser, NULL, pszPassword, LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &hImpersonation.m_h);
          if (bLoggedOn)
          {
            m_Request.m_bLoggedOn = ImpersonateLoggedOnUser(hImpersonation);
            if (!m_Request.m_bLoggedOn)
            {
              //Report the error
              CString sError;
              sError.Format(_T("CW3MFCClient::HandleClient, Failed to impersonate using user name: %s, GetLastError:%d"), pszUser, ::GetLastError());
              m_pServer->OnError(sError);
            }
          }
          else
          {
            //Report the error
            CString sError;
            sError.Format(_T("CW3MFCClient::HandleClient, Failed to logon using user name: %s, GetLastError:%d"), pszUser, ::GetLastError());
            m_pServer->OnError(sError);
          }
          m_Request.m_sUserName.ReleaseBuffer();
          m_Request.m_sPassword.ReleaseBuffer();
        } 
      #ifndef W3MFC_NO_SSPI_SUPPORT
        else if (m_Request.m_AuthorizationType == CW3MFCRequest::HTTP_AUTHORIZATION_NTLM && !bImpersonatedUsingSSPI)
        {
          bImpersonatedUsingSSPI = TRUE;
          SECURITY_STATUS ss = ImpersonateSecurityContext(&m_Request.m_ContentHandle);
          bLoggedOn = (ss == SEC_E_OK);
          if (bLoggedOn)
          {
            //Pull out some values from the SSPI context handle and stuff them in the request object
            SecPkgContext_Names names;
            if (QueryContextAttributes(&m_Request.m_ContentHandle, SECPKG_ATTR_NAMES, &names) == SEC_E_OK)
            {
              m_Request.m_sUserName = names.sUserName;
              FreeContextBuffer(names.sUserName);
            }

            SecPkgContext_Authority authority;
            if (QueryContextAttributes(&m_Request.m_ContentHandle, SECPKG_ATTR_AUTHORITY, &authority) == SEC_E_OK)
            {
              m_Request.m_sAuthorityName = authority.sAuthorityName;
              FreeContextBuffer(authority.sAuthorityName);
            }
          }
          else
          {
            //Report the error
            CString sError;
            sError.Format(_T("CW3MFCClient::HandleClient, Failed to impersonate client credentials using SSPI, Error:%d"), ss);
            m_pServer->OnError(sError);
          }
        }
      #endif

        m_Request.m_hImpersonation = hImpersonation;

        if (m_Request.m_AuthorizationType == CW3MFCRequest::HTTP_AUTHORIZATION_ANONYMOUS && !pSettings->m_bAllowAnonymous)
        {
          //Return an unauthorized message if some form of authentication is enabled
          if (pSettings->m_bAllowBasicAuthentication || pSettings->m_bAllowNTLMAuthentication)
            ReturnUnauthorizedMessage(m_Request.m_sURL);
          else
          {
            //Report the error
            m_pServer->OnError(_T("CW3MFCClient::HandleClient, Anonymous access is disabled in addition to all authentication mechanisms, All requests will Fail!!"));

            ReturnErrorMessage(500); //Internal server error
          }
        }
        else
        {
          if (m_Request.m_Verb == CW3MFCRequest::HTTP_VERB_GET || m_Request.m_Verb == CW3MFCRequest::HTTP_VERB_HEAD || m_Request.m_Verb == CW3MFCRequest::HTTP_VERB_POST)
          {
            if (!PreHandleGetPostHead()) //Allow derived classes to handle GET, HEAD or POST
            {
              BOOL bDirectory = FALSE;
              CW3MFCDirectory* pDirectory = NULL;
              DWORD dwMatchingURL = 0;
              DWORD dwMatchingPath = 0;
              if (MapURLToLocalFilename(m_Request.m_sURL, m_Request.m_sLocalFile, m_Request.m_sPathInfo, bDirectory, pDirectory, dwMatchingURL, dwMatchingPath))
              {
                AFXASSUME(pDirectory);
                pDirectory->HandleDirectory(this, bDirectory);
              }
              else
                ReturnErrorMessage(404); //Not Found
            }
          }
          else if (pSettings->m_bAllowDeleteRequest && m_Request.m_Verb == CW3MFCRequest::HTTP_VERB_DELETE)
          {
            //By default, only allow deletion of a file if we are using authorization
            if (m_Request.m_AuthorizationType != CW3MFCRequest::HTTP_AUTHORIZATION_ANONYMOUS)
            {
              CString sLocalFile;
              BOOL bDirectory = FALSE;
              CW3MFCDirectory* pDirectory = NULL;
              DWORD dwMatchingURL = 0;
              DWORD dwMatchingPath = 0;
              if (MapURLToLocalFilename(m_Request.m_sURL, m_Request.m_sLocalFile, m_Request.m_sPathInfo, bDirectory, pDirectory, dwMatchingURL, dwMatchingPath) && !bDirectory && pDirectory->GetWritable())
              {
                if (DeleteFile(m_Request.m_sLocalFile))
                  ReturnFileDeletedOkMessage(m_Request.m_sLocalFile);
                else
                {
                  if (::GetLastError() == ERROR_ACCESS_DENIED && !bDirectory)
                    ReturnUnauthorizedMessage(m_Request.m_sURL);
                  else 
                    ReturnErrorMessage(500); //Internal server error
                }
              }
              else
                ReturnErrorMessage(404); //Not Found
            }
            else if (pSettings->m_bAllowBasicAuthentication || pSettings->m_bAllowNTLMAuthentication)
              ReturnUnauthorizedMessage(m_Request.m_sURL); //Not authorized
            else
              ReturnErrorMessage(404); //Not Found
          }
          else
            ReturnErrorMessage(501); //Not implemented
        }

        //Restore our usual security priviledges
        if (m_Request.m_AuthorizationType == CW3MFCRequest::HTTP_AUTHORIZATION_PLAINTEXT)
        {
          //Revert to the usual security settings
          RevertToSelf();
        }
      }
      else
        ReturnErrorMessage(400); //Bad Request
    }

    //Should we service another request
    bMoreRequests = m_bResponseKeepAlive;

    //Remember some values from the old request
    BOOL bAuthenticationCompleted = m_Request.m_bAuthenticationComplete;
    CW3MFCRequest::HttpAuthorization Authorization = m_Request.m_AuthorizationType;
    CString sRemoteHost = m_Request.m_sRemoteHost;
    CString sUserName = m_Request.m_sUserName;
    CString sAuthorityName = m_Request.m_sAuthorityName;
    sockaddr_in clientAddress;
    memcpy(&clientAddress, &m_Request.m_ClientAddress, sizeof(sockaddr_in));

    //Reset the request data before we (potentially) loop around or exit
    m_Request = CW3MFCRequest();
  
    //Reinstate the Authentication completed flag. This prevents the need for reauthentications if the connection is keep alive
    m_Request.m_bAuthenticationComplete = bAuthenticationCompleted;

    //Reinstate the Authentication type and the Authority type and username (if it requires keep alives, which is only NTLM currently)
    if (Authorization == CW3MFCRequest::HTTP_AUTHORIZATION_NTLM)
    {
      m_Request.m_AuthorizationType = Authorization;
      m_Request.m_sUserName = sUserName;
      m_Request.m_sAuthorityName = sAuthorityName;
    }

    //Reinstate the client connection details
    memcpy(&m_Request.m_ClientAddress, &clientAddress, sizeof(sockaddr_in));
    m_Request.m_sRemoteHost = sRemoteHost;
  }
  while (bMoreRequests);

  //Undo the SSPI impersonation
#ifndef W3MFC_NO_SSPI_SUPPORT
  if (bImpersonatedUsingSSPI)
    RevertSecurityContext(&m_Request.m_ContentHandle);

  //Free up the context handle
  DeleteSecurityContext(&m_Request.m_ContentHandle);
#endif
}

BOOL CW3MFCClient::ParseSimpleRequestLine(const CString& sLine)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //First parse out the VERB
  static TCHAR* pszTokens = _T(" ");
  int nTokenPosition = 0;
  CString sVerb(sLine.Tokenize(pszTokens, nTokenPosition));
  if (sVerb.GetLength())
  {
    m_Request.m_sVerb = sVerb;
    if (sVerb.CompareNoCase(_T("GET")) == 0)
      m_Request.m_Verb = CW3MFCRequest::HTTP_VERB_GET;
    else if (sVerb.CompareNoCase(_T("POST")) == 0)
      m_Request.m_Verb = CW3MFCRequest::HTTP_VERB_POST;
    else if (sVerb.CompareNoCase(_T("HEAD")) == 0)
      m_Request.m_Verb = CW3MFCRequest::HTTP_VERB_HEAD;
    else if (sVerb.CompareNoCase(_T("PUT")) == 0)
      m_Request.m_Verb = CW3MFCRequest::HTTP_VERB_PUT;
    else if (sVerb.CompareNoCase(_T("LINK")) == 0)
      m_Request.m_Verb = CW3MFCRequest::HTTP_VERB_LINK;
    else if (sVerb.CompareNoCase(_T("DELETE")) == 0)
      m_Request.m_Verb = CW3MFCRequest::HTTP_VERB_DELETE;
    else if (sVerb.CompareNoCase(_T("UNLINK")) == 0)
      m_Request.m_Verb = CW3MFCRequest::HTTP_VERB_UNLINK;
    else
      m_Request.m_Verb = CW3MFCRequest::HTTP_VERB_UNKNOWN;

    //Parse out the URL
    CString sURL(sLine.Tokenize(pszTokens, nTokenPosition));
    if (sURL.GetLength())
    {
      m_Request.m_sRawURL = sURL;
      m_Request.m_sURL = UTF8ToCString(URLDecode(sURL)); //Convert any embedded escape sequences to their unencoded format 
                                                         //as well as handling UTF8 input data

      //Handle the Search path i.e everything after the ?
      int nQuestion = m_Request.m_sURL.Find(_T('?'));
      if (nQuestion != -1)
      {
        m_Request.m_sExtra = m_Request.m_sURL.Right(m_Request.m_sURL.GetLength() - nQuestion - 1);
        m_Request.m_sURL = m_Request.m_sURL.Left(nQuestion);
      }
      nQuestion = m_Request.m_sRawURL.Find(_T('?'));
      if (nQuestion != -1)
        m_Request.m_sRawExtra = m_Request.m_sRawURL.Right(m_Request.m_sRawURL.GetLength() - nQuestion - 1);

      //Parse out the HTTP version
      CString sVersion(sLine.Tokenize(pszTokens, nTokenPosition));
      if (sVersion.GetLength())
      {
        if (sVersion.Find(_T("HTTP/")) == 0)
        {
          static TCHAR* pszVersionTokens = _T(".");
          CString sVersionToTokenize(sVersion.Right(sVersion.GetLength() - 5));
          int nVersionTokenPosition = 0;
          CString sMajorVersion(sVersionToTokenize.Tokenize(pszVersionTokens, nVersionTokenPosition));
          if (sMajorVersion.GetLength())
          {
            WORD wMajorVersion = static_cast<WORD>(_ttoi(sMajorVersion));
            CString sMinorVersion(sVersionToTokenize.Tokenize(pszVersionTokens, nVersionTokenPosition));
            if (sMinorVersion.GetLength())
            {
              WORD wMinorVersion = static_cast<WORD>(_ttoi(sMinorVersion));
              m_Request.m_dwHttpVersion = MAKELONG(wMinorVersion, wMajorVersion);
              bSuccess = TRUE;
            }
          }
        }
      }
      else
      {
        //No version included in the request, so set it to HTTP v0.9
        m_Request.m_dwHttpVersion = MAKELONG(9, 0);
        bSuccess = m_Request.m_Verb == CW3MFCRequest::HTTP_VERB_GET; //"GET" is only allowed with HTTP v0.9
      }  
    }
  }

  return bSuccess;
}

BOOL CW3MFCClient::ParseWeekDay(LPCTSTR pszToken, WORD& nWeekDay)
{
  BOOL bSuccess = TRUE;
  if (_tcsicmp(pszToken, _T("Sun")) == 0 || _tcsicmp(pszToken, _T("Sunday")) == 0)
    nWeekDay = 0;
  else if (_tcsicmp(pszToken, _T("Mon")) == 0 || _tcsicmp(pszToken, _T("Monday")) == 0)
    nWeekDay = 1;
  else if (_tcsicmp(pszToken, _T("Tue")) == 0 || _tcsicmp(pszToken, _T("Tuesday")) == 0)
    nWeekDay = 2;
  else if (_tcsicmp(pszToken, _T("Wed")) == 0 || _tcsicmp(pszToken, _T("Wednesday")) == 0)
    nWeekDay = 3;
  else if (_tcsicmp(pszToken, _T("Thu")) == 0 || _tcsicmp(pszToken, _T("Thursday")) == 0)
    nWeekDay = 4;
  else if (_tcsicmp(pszToken, _T("Fri")) == 0 || _tcsicmp(pszToken, _T("Friday")) == 0)
    nWeekDay = 5;
  else if (_tcsicmp(pszToken, _T("Sat")) == 0 || _tcsicmp(pszToken, _T("Saturday")) == 0)
    nWeekDay = 6;
  else
    bSuccess = FALSE;
  return bSuccess;
}

BOOL CW3MFCClient::ParseMonth(LPCTSTR pszToken, WORD& nMonth)
{
  BOOL bSuccess = TRUE;
  if (_tcsicmp(pszToken, _T("Jan")) == 0)
    nMonth = 1;
  else if (_tcsicmp(pszToken, _T("Feb")) == 0)
    nMonth = 2;
  else if (_tcsicmp(pszToken, _T("Mar")) == 0)
    nMonth = 3;
  else if (_tcsicmp(pszToken, _T("Apr")) == 0)
    nMonth = 4;
  else if (_tcsicmp(pszToken, _T("May")) == 0)
    nMonth = 5;
  else if (_tcsicmp(pszToken, _T("Jun")) == 0)
    nMonth = 6;
  else if (_tcsicmp(pszToken, _T("Jul")) == 0)
    nMonth = 7;
  else if (_tcsicmp(pszToken, _T("Aug")) == 0)
    nMonth = 8;
  else if (_tcsicmp(pszToken, _T("Sep")) == 0)
    nMonth = 9;
  else if (_tcsicmp(pszToken, _T("Oct")) == 0)
    nMonth = 10;
  else if (_tcsicmp(pszToken, _T("Nov")) == 0)
    nMonth = 11;
  else if (_tcsicmp(pszToken, _T("Dec")) == 0)
    nMonth = 12;
  else
    bSuccess = FALSE;
  return bSuccess;
}

BOOL CW3MFCClient::ParseDate(const CString& sField, SYSTEMTIME& time)
{
  //This method understands RFC 1123, RFC 850 and asctime formats

  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Http times never include a millisecond field, so just set it to zero
  time.wMilliseconds = 0;

  int nLength = sField.GetLength();
  if (nLength > 5)
  {
    TCHAR cThirdCharacter = sField.GetAt(3);
    if (cThirdCharacter == _T(',')) //Parsing a RFC 1123 format date
    {
      //First the weekday
      static TCHAR* pszTokens = _T(", :");
      int nTokenPosition = 0;
      CString sToken(sField.Tokenize(pszTokens, nTokenPosition));
      if (nTokenPosition == -1)
        return FALSE;
      bSuccess = ParseWeekDay(sToken, time.wDayOfWeek);

      //Then the day of the month
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wDay = static_cast<WORD>(_ttoi(sToken));

      //Then the month
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      bSuccess = bSuccess && ParseMonth(sToken, time.wMonth);

      //And the year
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wYear = static_cast<WORD>(_ttoi(sToken));

      //And the hour
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wHour = static_cast<WORD>(_ttoi(sToken));

      //And the minute
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wMinute = static_cast<WORD>(_ttoi(sToken));

      //And the second
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wSecond = static_cast<WORD>(_ttoi(sToken));
    }
    else if (cThirdCharacter == _T(' ')) //Parsing an asctime format date
    {
      //First the weekday
      static TCHAR* pszTokens = _T(", :");
      int nTokenPosition = 0;
      CString sToken(sField.Tokenize(pszTokens, nTokenPosition));
      if (nTokenPosition == -1)
        return FALSE;
      bSuccess = ParseWeekDay(sToken, time.wDayOfWeek);

      //Then the month
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      bSuccess = bSuccess && ParseMonth(sToken, time.wMonth);

      //Then the day of the month
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wDay = static_cast<WORD>(_ttoi(sToken));

      //And the hour
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wHour = static_cast<WORD>(_ttoi(sToken));

      //And the minute
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wMinute = static_cast<WORD>(_ttoi(sToken));

      //And the second
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wSecond = static_cast<WORD>(_ttoi(sToken));

      //And the year
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wYear = static_cast<WORD>(_ttoi(sToken));
    }
    else //Must be a RFC 850 format date
    {
      //First the weekday
      static TCHAR* pszTokens = _T(", :-");
      int nTokenPosition = 0;
      CString sToken(sField.Tokenize(pszTokens, nTokenPosition));
      if (nTokenPosition == -1)
        return FALSE;
      bSuccess = ParseWeekDay(sToken, time.wDayOfWeek);

      //Then the day of the month
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wDay = static_cast<WORD>(_ttoi(sToken));

      //Then the month
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      bSuccess = bSuccess && ParseMonth(sToken, time.wMonth);

      //And the year (2 Digits only, so make some intelligent assumptions)
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wYear = static_cast<WORD>(_ttoi(sToken));
      if (time.wYear < 50)
        time.wYear += 2000;
      else if (time.wYear < 100)
        time.wYear += 1900; 

      //And the hour
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wHour = static_cast<WORD>(_ttoi(sToken));

      //And the minute
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wMinute = static_cast<WORD>(_ttoi(sToken));

      //And the second
      sToken = sField.Tokenize(pszTokens, nTokenPosition);
      if (nTokenPosition == -1)
        return FALSE;
      time.wSecond = static_cast<WORD>(_ttoi(sToken));
    }
  }

  return bSuccess;
}

BOOL CW3MFCClient::ParseAuthorizationLine(const CString& sField)
{
  //Validate our parameters
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);

  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  static char* pszTokens = " ";
  int nTokenPosition = 0;
  CStringA sAsciiField(sField);
  CStringA sToken(sAsciiField.Tokenize(pszTokens, nTokenPosition));
  if (sToken.GetLength())
  {
    //Parse out the base64 encoded username and password if we are doing Basic authentication
    if ((sToken.CompareNoCase("basic") == 0) && pSettings->m_bAllowBasicAuthentication)
    {
      //Assume the worst
      bSuccess = FALSE;
    
      //Move to the base64 encoded data after the text "Basic"
      sToken = sAsciiField.Tokenize(pszTokens, nTokenPosition);
      if (sToken.GetLength())
      {
        //Decode the base64 string passed to us
        int nEncodedLength = sToken.GetLength();
        int nDecodedLength = ATL::Base64DecodeGetRequiredLength(nEncodedLength);
        ATL::CHeapPtr<BYTE> szOutput;
        if (szOutput.Allocate(nDecodedLength + 1))
        {
          int nOutputLength = nDecodedLength;
          if (ATL::Base64Decode(sToken, nEncodedLength, szOutput.m_pData, &nOutputLength))
          {
            //NULL terminate the decoded data
            szOutput.m_pData[nOutputLength] = '\0';

            //Extract the username and password from the decoded data
            CString sOutput(szOutput);
            int nColon = sOutput.Find(_T(":"));
            if (nColon != -1)
            {
              m_Request.m_AuthorizationType = CW3MFCRequest::HTTP_AUTHORIZATION_PLAINTEXT;
              m_Request.m_sUserName = sOutput.Left(nColon);
              m_Request.m_sPassword = sOutput.Right(sOutput.GetLength() - nColon - 1);
              bSuccess = TRUE;
            }
          }
        }
      }
    }
  #ifndef W3MFC_NO_SSPI_SUPPORT
    else if ((sToken.CompareNoCase("ntlm") == 0) && pSettings->m_bAllowNTLMAuthentication && !m_Request.m_bAuthenticationComplete)
    {
      //Assume the worst
      bSuccess = FALSE;
    
      //Move to the encoded data after the text "NTLM"
      sToken = sAsciiField.Tokenize(pszTokens, nTokenPosition);
      if (sToken.GetLength())
      {
        int nEncodedLength = sToken.GetLength();
        int nDecodedLength = ATL::Base64DecodeGetRequiredLength(nEncodedLength);
        ATL::CHeapPtr<BYTE> szInput;
        if (szInput.Allocate(nDecodedLength + 1))
        {
          int nInputLength = nDecodedLength;
          if (ATL::Base64Decode(sToken, nEncodedLength, szInput.m_pData, &nInputLength))
          {
            //Get SSPI to act on the received data

            //First setup the buffers
            SecBuffer secBufferIn[1];
            secBufferIn[0].BufferType = SECBUFFER_TOKEN;
            secBufferIn[0].cbBuffer = nInputLength;
            secBufferIn[0].pvBuffer = szInput.m_pData; 

            SecBufferDesc secbufDescriptorIn;
            secbufDescriptorIn.ulVersion = SECBUFFER_VERSION;
            secbufDescriptorIn.cBuffers = 1;
            secbufDescriptorIn.pBuffers = secBufferIn;

            SecBuffer secBufferOut[1];
            secBufferOut[0].BufferType = SECBUFFER_TOKEN;
            secBufferOut[0].cbBuffer = 0;
            secBufferOut[0].pvBuffer = NULL;

            SecBufferDesc secbufDescriptorOut;
            secbufDescriptorOut.ulVersion = SECBUFFER_VERSION;
            secbufDescriptorOut.cBuffers = 1;
            secbufDescriptorOut.pBuffers = secBufferOut;

            //Call SSPI
            ULONG nContextAttributes;
            SECURITY_STATUS ss = AcceptSecurityContext(m_pServer->GetCredentialHandle(), m_Request.m_bFirstAuthenticationRequest ? NULL : &m_Request.m_ContentHandle, 
                                                       &secbufDescriptorIn, ASC_REQ_STREAM | ASC_REQ_ALLOCATE_MEMORY, SECURITY_NETWORK_DREP, 
                                                       &m_Request.m_ContentHandle, &secbufDescriptorOut, &nContextAttributes, NULL);

            //It's no longer the first request
            m_Request.m_bFirstAuthenticationRequest = FALSE;

            //Setup the output data (if any)
            BOOL bAllocateOK = TRUE;
            if (secBufferOut[0].cbBuffer)
            {
              nEncodedLength = ATL::Base64EncodeGetRequiredLength(secBufferOut[0].cbBuffer, ATL_BASE64_FLAG_NOCRLF);
              
              ATL::CHeapPtr<char> szOutput;
              if (szOutput.Allocate(nEncodedLength + 1))
              {
                //Base 64 encode the data to be sent
                ATL::Base64Encode(static_cast<const BYTE*>(secBufferOut[0].pvBuffer), secBufferOut[0].cbBuffer, szOutput.m_pData, &nEncodedLength, ATL_BASE64_FLAG_NOCRLF);

                //Null terminate the data
                szOutput.m_pData[nEncodedLength] = '\0';

                //Set the data to return to the client
                m_Request.m_sAuthenticationResponse = szOutput;
              }
              else
                bAllocateOK = FALSE;
              
              //Also free the data allocated by SSPI
              FreeContextBuffer(secBufferOut[0].pvBuffer);
            }

            //Do we need more data from the client?
            m_Request.m_bAuthenticationComplete = (ss != SEC_I_CONTINUE_NEEDED);

            //Is the authentication handshake completed
            if (ss == SEC_E_OK)
              m_Request.m_AuthorizationType = CW3MFCRequest::HTTP_AUTHORIZATION_NTLM;

            bSuccess = (ss == SEC_E_OK || ss == SEC_I_CONTINUE_NEEDED);
            if (bSuccess && !bAllocateOK)
              bSuccess = FALSE;
          }
        }
      }
    }
  #endif //W3MFC_NO_SSPI_SUPPORT
  }

  return bSuccess;
}

BOOL CW3MFCClient::ParseRequestLine(const CString& sCurrentLine, BOOL bFirstLine, const CString& sField, const CString& sValue)
{
  //Assume the worst
  BOOL bSuccess = FALSE;

  if (bFirstLine)
  {
    //Handle the first line
    m_Request.m_sRequest = sCurrentLine;
    bSuccess = ParseSimpleRequestLine(sCurrentLine);
  }
  else
  {
    bSuccess = TRUE;

    //Also add the header line to the header map
    CString sKey(sField);
    sKey.MakeUpper();
    m_Request.m_HeaderMap.SetAt(sKey, sValue);

    //Handle any other request headers  
    if (sField.CompareNoCase(_T("If-Modified-Since")) == 0)
    {
      //Handle the If-Modified-Since header
      SYSTEMTIME time;
      if (ParseDate(sValue, time))
      {
        m_Request.m_bIfModifiedSincePresent = TRUE; 
        memcpy(&m_Request.m_IfModifiedSince, &time, sizeof(SYSTEMTIME));
      }
    }
    else if (sField.CompareNoCase(_T("Authorization")) == 0)
    {
      //Handle the Authorization header
      bSuccess = ParseAuthorizationLine(sValue);
    }
    else if (sField.CompareNoCase(_T("Content-Type")) == 0)
      m_Request.m_sContentType = sValue;
    else if (sField.CompareNoCase(_T("Content-Length")) == 0)
      m_Request.m_nContentLength = _ttoi(sValue);
    else if ((sField.CompareNoCase(_T("Connection")) == 0) && (sValue.CompareNoCase(_T("Keep-Alive")) == 0))
      m_Request.m_bKeepAlive = TRUE;
  }

  return bSuccess;
}

LPSTR CW3MFCClient::FindNextTerminatorFromRequest(LPSTR pszLine)
{
  LPSTR pszCurrentLine = pszLine;
  while (TRUE)
  {
    ++pszCurrentLine;
    if (pszCurrentLine >= ((LPSTR) m_Request.m_pRawEntity))
      return pszCurrentLine;
    else if (*pszCurrentLine == '\n')
      return pszCurrentLine;
  }
  
  return NULL;
}

BOOL CW3MFCClient::ParseRequest()
{
  //By default assume the worst 
  BOOL bSuccess = FALSE;

  //Also store a pointer to entity body.
  LPSTR pszEntityTerminator = strstr((LPSTR) m_Request.m_pRawRequest, "\r\n\r\n");
  if (pszEntityTerminator)
  {
    m_Request.m_pRawEntity = reinterpret_cast<BYTE*>(pszEntityTerminator+4);
    m_Request.m_dwRawEntitySize = static_cast<DWORD>(m_Request.m_dwRawRequestSize - (m_Request.m_pRawEntity - m_Request.m_pRawRequest));
  }
  else
  {
    m_Request.m_pRawEntity = NULL;
    m_Request.m_dwRawEntitySize = 0;
  }

  //Process each line
  BOOL bFirstLine = TRUE;
  LPSTR pszLine = reinterpret_cast<LPSTR>(m_Request.m_pRawRequest);
  LPSTR pszTerminator = strstr(pszLine, "\n");
  BOOL bMoreLines = TRUE;
  do 
  {
    CString sLine;
    if (pszTerminator)
    {
      //Form the current line
      size_t nCurSize = pszTerminator - pszLine;
      char* pszCurrentLine = new char[nCurSize];
      strncpy_s(pszCurrentLine, nCurSize, pszLine, nCurSize-1);
      pszCurrentLine[nCurSize-1] = '\0'; 

      //Transfer to the CString variable
      sLine = pszCurrentLine;

      //Tidy up the heap memory now that we are finished with it
      delete [] pszCurrentLine; 

      //Parse each line using the virtual ParseRequestLine method
      if (sLine.GetLength())
      {
        CString sField;
        CString sValue;
        if (!bFirstLine)
        {
          m_Socket.SplitRequestLine(sLine, sField, sValue);
          bSuccess = ParseRequestLine(sLine, FALSE, sField, sValue);
        }
        else
        {
          bSuccess = ParseRequestLine(sLine, TRUE, sField, sValue);
          bFirstLine = FALSE;
        }
      }

      //Move onto the next line
      if (pszTerminator)
      {
        pszLine = pszTerminator+1;
        if (pszLine >= (LPSTR) m_Request.m_pRawEntity)
          bMoreLines = FALSE;
        else 
          pszTerminator = FindNextTerminatorFromRequest(pszLine);
      }
    }  
    else
      bMoreLines = FALSE;
  }
  while (bMoreLines && bSuccess);

  m_Request.m_hImpersonation = m_pServer->GetImpersonationHandle();

  return bSuccess;
}

DWORD CW3MFCClient::ReturnErrorMessage(int nStatusCode)
{
  //Validate our parameters
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);

  //Form the body of the response
  CStringA sBody(m_pServer->LoadHTML(nStatusCode));
  int nBodyLength = sBody.GetLength();

  if (m_Request.m_dwHttpVersion > MAKELONG(9, 0))
  {
    //Form the header of the response
    CW3MFCResponseHeader responseHdr;
    responseHdr.AddStatusCode(nStatusCode);
    SYSTEMTIME st;
    GetSystemTime(&st);
    responseHdr.AddDate(st);
    responseHdr.AddServer(pSettings->m_sServerName);
    responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
    if (m_bResponseKeepAlive)
      responseHdr.AddKeepAlive();
    responseHdr.AddContentLength(nBodyLength);
    responseHdr.AddContentType(_T("text/html"));

    //Send the header and body all in one
    TransmitBuffer(m_Socket, responseHdr, sBody, pSettings->m_dwWritableTimeout);
  }
  else
  {
    //No header sent for HTTP v0.9
    //so just send the body
    try
    {
    #ifdef W3MFC_SSL_SUPPORT
      m_Socket.SendWithRetry(sBody, nBodyLength, pSettings->m_dwWritableTimeout, m_SSL);
    #else    
      m_Socket.SendWithRetry(sBody, nBodyLength, pSettings->m_dwWritableTimeout);
    #endif
    }
    catch(CWSocketException* pEx)
    {
      //Report the error
      CString sError;
      sError.Format(_T("CW3MFCClient::ReturnErrorMessage, Failed to send to socket, Error:%d"), pEx->m_nError);
      ASSERT(m_pServer);
      m_pServer->OnError(sError);

      pEx->Delete();  
    }
  }

  //Log the information
  PostLog(nStatusCode, nBodyLength);

  //Return the body length
  return nBodyLength;
}

DWORD CW3MFCClient::ReturnRedirectMessage(const CString& sURL)
{
  //Validate our parameters
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);

  //Form the body of the response
  CStringA sBody(m_pServer->LoadHTML(302));
  int nBodyLength = sBody.GetLength();

  //Form the header of the response
  CW3MFCResponseHeader responseHdr;
  responseHdr.AddStatusCode(302);
  SYSTEMTIME st;
  GetSystemTime(&st);
  responseHdr.AddDate(st);
  responseHdr.AddServer(pSettings->m_sServerName);
  responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
  responseHdr.AddLocation(sURL);
  if (m_bResponseKeepAlive)
    responseHdr.AddKeepAlive();
  responseHdr.AddContentLength(nBodyLength);
  responseHdr.AddContentType(_T("text/html"));

  //Send the header and body all in one
  TransmitBuffer(m_Socket, responseHdr, sBody, pSettings->m_dwWritableTimeout);

  //Log the information
  PostLog(302, nBodyLength);

  return nBodyLength;
}

DWORD CW3MFCClient::ReturnUnauthorizedMessage(const CString& sRealm)
{
  //Validate our parameters
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);
  ASSERT(pSettings->m_bAllowBasicAuthentication || pSettings->m_bAllowNTLMAuthentication);

  //Form the body of the response
  CStringA sBody(m_pServer->LoadHTML(401));
  int nBodyLength = sBody.GetLength();

  //Form the header of the response
  CW3MFCResponseHeader responseHdr;
  responseHdr.AddStatusCode(401);
  SYSTEMTIME st;
  GetSystemTime(&st);
  responseHdr.AddDate(st);
  responseHdr.AddServer(pSettings->m_sServerName);
  responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);

  if (pSettings->m_bAllowNTLMAuthentication)
    responseHdr.AddWWWAuthenticateNTLM(m_Request.m_sAuthenticationResponse);
  if (pSettings->m_bAllowBasicAuthentication)
    responseHdr.AddWWWAuthenticateBasic(sRealm);
  if (m_bResponseKeepAlive)
    responseHdr.AddKeepAlive();
  responseHdr.AddContentLength(nBodyLength);
  responseHdr.AddContentType(_T("text/html"));

  //Send the header and body all in one
  TransmitBuffer(m_Socket, responseHdr, sBody, pSettings->m_dwWritableTimeout);

  //Log the information
  PostLog(401, nBodyLength);

  return nBodyLength;
}

DWORD CW3MFCClient::ReturnFileDeletedOkMessage(const CString& /*sFile*/)
{
  //Validate our parameters
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);

  //Form the body of the response
  CStringA sBody(m_pServer->GetFileDeletedHTML());
  int nBodyLength = sBody.GetLength();

  //Form the header of the response
  CW3MFCResponseHeader responseHdr;
  responseHdr.AddStatusCode(200);
  SYSTEMTIME st;
  GetSystemTime(&st);
  responseHdr.AddDate(st);
  responseHdr.AddServer(pSettings->m_sServerName);
  responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
  if (m_bResponseKeepAlive)
    responseHdr.AddKeepAlive();
  responseHdr.AddContentLength(nBodyLength);
  responseHdr.AddContentType(_T("text/html"));

  //Send the header and body all in one
  TransmitBuffer(m_Socket, responseHdr, sBody, pSettings->m_dwWritableTimeout);

  //Log the information
  PostLog(200, nBodyLength);

  return nBodyLength;
}

void CW3MFCClient::TransmitBuffer(BYTE* byData, DWORD dwSize, CW3MFCDirectory* /*pDirectory*/, BOOL bForceExpire)
{
  //validate our settings
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);
  AFXASSUME(pSettings->m_pMimeManager);

  CW3MFCResponseHeader responseHdr;

  //Get the current system time in UTC
  SYSTEMTIME stCurTime;
  ::GetSystemTime(&stCurTime);

  if (m_Request.m_dwHttpVersion > MAKELONG(9, 0)) //No header sent for Http 0.9
  {
    //Get the mime type for extension we are about to return
    CString sMime(pSettings->m_pMimeManager->GetMimeType(m_Request));

    //Form the header of the response
    responseHdr.AddStatusCode(200);
    responseHdr.AddDate(stCurTime);
    responseHdr.AddServer(pSettings->m_sServerName);
    responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
	  if (bForceExpire)
		  responseHdr.AddExpires(stCurTime);		
    if (m_bResponseKeepAlive)
      responseHdr.AddKeepAlive();
    responseHdr.AddContentLength(dwSize);
    responseHdr.AddContentType(sMime);

    //Send the header and body all in one
    TransmitBuffer(m_Socket, responseHdr, byData, dwSize, pSettings->m_dwWritableTimeout);
  }
  else
  {
    //Send back the file contents (if not a HEAD request)
    if (m_Request.m_Verb != CW3MFCRequest::HTTP_VERB_HEAD)
    {
      try
      {
        m_Socket.SendWithRetry(byData, dwSize, pSettings->m_dwWritableTimeout);
      }
      catch(CWSocketException* pEx)
      {
        //Report the error
        CString sError;
        sError.Format(_T("CW3MFCClient::TransmitBuffer, Failed to send to socket, Error:%d"), pEx->m_nError);
        m_pServer->OnError(sError);

        pEx->Delete();  
      }
    }
  }

  //Log the information
  PostLog(200, dwSize);
}
 
CW3MFCDirectory* CW3MFCClient::GetVirtualDirectory(const CString& sDirectory)
{
  //Validate our parameters
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);

  CW3MFCDirectory* pDirectory = NULL;
  for (INT_PTR i=0; i<pSettings->m_Directories.GetSize() && pDirectory == NULL; i++)  
  {
    CW3MFCDirectory* pDir = pSettings->m_Directories.GetAt(i);
    if (sDirectory.CompareNoCase(pDir->GetAlias()) == 0)
      pDirectory = pDir;
  }

  return pDirectory;  
}

int CW3MFCClient::HexToInt(TCHAR ch)
{
  //character is in range of 0-9 subtract ascii value of '0' to get integer
  if ((ch >= _T('0')) && (ch <= _T('9')))
    return ch - _T('0');

  //character is in range of a-z or A-Z subtract ascii value of 'a' and add 10 to get it converted into int
  if ((ch >= _T('a')) && (ch <= _T('f')))
    return ch - _T('a') + 10;
  else if ((ch >= _T('A')) && (ch <= _T('F')))
    return ch - _T('A') + 10;

  //Character is not a Hex Digit
  return -1;
}

TCHAR CW3MFCClient::IntToHex(int Character)
{
  //This function only expects nibbles
  ASSERT(Character >= 0 && Character <= 15);
  
  if (Character <= 9)
    return static_cast<TCHAR>(Character + _T('0'));
  else
    return static_cast<TCHAR>(Character - 10 + _T('A'));
}

CString CW3MFCClient::UTF8ToCString(const CString& sURL)
{
  //Get the length of the string to convert from UTF
  int nInputLen = sURL.GetLength();

  //Allocate memory to hold the Unicode string
  LPWSTR lpwszURL = new WCHAR[nInputLen * 4];

  //Convert the UTF input string to Unicode
  int nInputIndex = 0;
  int nOutputIndex = 0;
  while (nInputIndex < nInputLen)
  {
    BYTE cInputChar1 = static_cast<BYTE>(sURL.GetAt(nInputIndex));
    if (cInputChar1 <= 0x7f)
    {
      //Form the current output character
      lpwszURL[nOutputIndex] = cInputChar1;

      //Move onto the next input character
      nInputIndex++;
    }
    else if (nInputIndex < nInputLen-1 && cInputChar1 <= 0xdf)
    {
      //Form the current output character
      lpwszURL[nOutputIndex] = static_cast<wchar_t>(((cInputChar1 & 0x1f) << 6) + (sURL.GetAt(nInputIndex+1) & 0x3f));

      //Move onto the next input character
      nInputIndex += 2;
    }
    else if (nInputIndex < nInputLen-2)
    {

      //Form the current output character
      lpwszURL[nOutputIndex] = static_cast<wchar_t>(((cInputChar1 & 0x0f) << 12) + ((sURL.GetAt(nInputIndex+1) & 0x3f) << 6) + (sURL.GetAt(nInputIndex+2) & 0x3f));

      //Move onto the next input character
      nInputIndex += 3;
    }
    else
    {
      //skip illegal sequence
      nInputIndex++;
    }

    //Move onto the next output character
    nOutputIndex++;
  }

  //Don't forget to NULL terminate
  lpwszURL[nOutputIndex] = _T('\0');

  //Form a CString version of the Unicode string
  CString sDecodedURL(lpwszURL);

  //Tidy up the heap memory we have been using
  delete [] lpwszURL;

  //Return the string
  return sDecodedURL;
}

CString CW3MFCClient::URLEncode(const CString& sURL)
{
  CString sEncodedURL;
  int nLength = sURL.GetLength();
  LPTSTR pszEncodedURL = sEncodedURL.GetBufferSetLength((nLength*3) + 1);
  int nOutputIndex = 0;
  for (int i=0; i<nLength; i++)
  {
    //Pull out the current character to evaluate
    BYTE CurrentChar = (BYTE) sURL.GetAt(i);

    //Should we encode the character or not? See RFC 1738 for the details.
    if ((CurrentChar >= '0' && CurrentChar <= '9') ||
        (CurrentChar >= 'a' && CurrentChar <= 'z') ||
        (CurrentChar >= 'A' && CurrentChar <= 'Z') ||
        (CurrentChar == '$') ||
        (CurrentChar == '-') ||
        (CurrentChar == '_') ||
        (CurrentChar == '.') ||
        (CurrentChar == '+') ||
        (CurrentChar == '!') ||
        (CurrentChar == '*') ||
        (CurrentChar == '\'') ||
        (CurrentChar == '(') ||
        (CurrentChar == ')') ||
        (CurrentChar == ','))
    {
      pszEncodedURL[nOutputIndex] = CurrentChar;
      ++nOutputIndex;
    }
    else 
    {
      pszEncodedURL[nOutputIndex] = _T('%');
      ++nOutputIndex;

      TCHAR nNibble = IntToHex((CurrentChar & 0xF0) >> 4);
      pszEncodedURL[nOutputIndex] = nNibble;
      ++nOutputIndex;

      nNibble = IntToHex(CurrentChar & 0x0F);  
      pszEncodedURL[nOutputIndex] = nNibble;
      ++nOutputIndex;
    }
  }
  //Don't forget to NULL terminate
  pszEncodedURL[nOutputIndex] = _T('\0');
  sEncodedURL.ReleaseBuffer();

  return sEncodedURL;
}

CString CW3MFCClient::URLDecode(const CString& sURL)
{
  CString sDecodedURL;
  int nLength = sURL.GetLength();
  LPTSTR pszDecodedURL = sDecodedURL.GetBufferSetLength(nLength + 1);
  int nOutputIndex = 0;
  for (int i=0; i<nLength; i++)
  {
    TCHAR c1 = sURL[i];
    if (c1 != _T('%'))
    {
      if (c1 == '+')
        pszDecodedURL[nOutputIndex] = ' ';  
      else
        pszDecodedURL[nOutputIndex] = c1;
      nOutputIndex++;
    }
    else
    {
      if (i < nLength-2)
      {
        int msb = HexToInt(sURL[i+1]);
        int lsb = HexToInt(sURL[i+2]);
        if (msb != -1 && lsb != -1)
        {
          int nChar = (msb << 4) + lsb;
          pszDecodedURL[nOutputIndex] = TCHAR(nChar);
          nOutputIndex++;
          i += 2;
        }
        else
        {
          pszDecodedURL[nOutputIndex] = c1;
          nOutputIndex++;
        }
      }
      else
      {
        pszDecodedURL[nOutputIndex] = c1;
        nOutputIndex++;
      }
    }
  }
  //Don't forget to NULL terminate
  pszDecodedURL[nOutputIndex] = _T('\0');
  sDecodedURL.ReleaseBuffer();

  return sDecodedURL;
}

BOOL CW3MFCClient::MapURLToLocalFilename(CString& sURL, CString& sLocalFile, CString& sPathInfo, BOOL& bDirectory, CW3MFCDirectory*& pDirectory, DWORD& dwMatchingURL, DWORD& dwMatchingPath)
{
  //Setup the default return value from this function
  BOOL bSuccess = FALSE;
  sLocalFile.Empty();
  sPathInfo.Empty();
  bDirectory = FALSE;
  pDirectory = NULL;
  dwMatchingURL = 0;
  dwMatchingPath = 0;

  //Convert from Unix to Windows format
  CString sClientURL(sURL);
  sClientURL.Replace(_T("/"), _T("\\"));

  //As a security precaution do not allow any URL's which contains any relative parts in it 
  //(as an example trying to request a file outside of the directories we are serving)
  //We also exclude URL's with a ":" in them as this is how NTFS streams are accessed
  if (sClientURL.Find(_T("..")) == -1 && sClientURL.Find(_T(":")) == -1)
  {
    pDirectory = NULL;
    TCHAR sDrive[_MAX_DRIVE];
    TCHAR sDir[_MAX_DIR];
    TCHAR sFname[_MAX_FNAME];
    TCHAR sExt[_MAX_EXT];
    CString sVirtualDir(sClientURL);
    sVirtualDir += _T("\\"); //Initially try a default directory
    do
    {
      _tsplitpath_s(sVirtualDir, sDrive, sizeof(sDrive)/sizeof(TCHAR), sDir, sizeof(sDir)/sizeof(TCHAR), sFname, sizeof(sFname)/sizeof(TCHAR), sExt, sizeof(sExt)/sizeof(TCHAR));
      if (_tcslen(sDir))
      {
        pDirectory = GetVirtualDirectory(sDir);
        if (pDirectory == NULL)
        {
          sVirtualDir = sDir;
          sVirtualDir = sVirtualDir.Left(sVirtualDir.GetLength()-1);
        }
      }
    }
    while (pDirectory == NULL && _tcslen(sDir));

    if (pDirectory)
    {
      ASSERT(pDirectory->GetDirectory().GetLength());
      ASSERT(pDirectory->GetAlias().GetLength());

      //Ignore the alias part of the URL now that we have got the virtual directory
      CString sAlias = pDirectory->GetAlias();
      CString sRelativeFile(sClientURL);
      sRelativeFile = sRelativeFile.Right(sRelativeFile.GetLength() - sAlias.GetLength());

      //Form the local filename from the requested URL
      CString sDirectory = pDirectory->GetDirectory();
      int nLength = sDirectory.GetLength();
      if (sDirectory.GetAt(nLength-1) != _T('\\'))
        sDirectory += _T("\\");
      sLocalFile = sDirectory; 

      //Asking for the default filename
      if (sRelativeFile.IsEmpty())
      {
        bDirectory = pDirectory->GetDirectoryListing();
        if (!bDirectory)
          sLocalFile += pDirectory->GetDefaultFile(); 

        dwMatchingURL = sURL.GetLength();
        dwMatchingPath = sLocalFile.GetLength();
      }
      else
      {
        //Ensure that we don't have two "\" separating the filename from the directory
        if (sClientURL.Find(_T('\\')) == 0)
          sLocalFile += sRelativeFile.Right(sRelativeFile.GetLength());
        else
          sLocalFile += sRelativeFile; 

        dwMatchingURL = sURL.GetLength();
        dwMatchingPath = sLocalFile.GetLength();

        //Keep parsing left to right until we find a piece of the path which is a file.
        //This is used to work out the PathInfo value
        CString sTemp(sRelativeFile);
        int nLocalFileData = 0;
        BOOL bContinueParse = TRUE;
        while (bContinueParse)
        {
          int nSlash = sTemp.Find(_T('\\'));
          if (nSlash != -1)
          {
            nLocalFileData += nSlash;
            CString sFile(sDirectory + sRelativeFile.Left(nLocalFileData));
            sTemp = sTemp.Right(sTemp.GetLength() - nSlash - 1);

            DWORD dwAttributes = GetFileAttributes(sFile);
            if (dwAttributes == INVALID_FILE_ATTRIBUTES)
            {
              bContinueParse = FALSE;
              sPathInfo = sLocalFile.Right(sLocalFile.GetLength() - sFile.GetLength() + nSlash);
              sPathInfo.Replace(_T("\\"), _T("/"));
              sLocalFile = sFile.Left(sFile.GetLength() - nSlash - 1);

              //Remove the PathInfo from the incoming parameter
              sURL = sURL.Left(sURL.GetLength() - sPathInfo.GetLength());

              dwMatchingURL = sClientURL.GetLength() - sPathInfo.GetLength();
              dwMatchingPath = sLocalFile.GetLength();
            }
            else if ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
              bContinueParse = FALSE;
              sPathInfo = sLocalFile.Right(sLocalFile.GetLength() - sFile.GetLength() - 1);
              sPathInfo.Replace(_T("\\"), _T("/"));
              sLocalFile = sFile;

              //Remove the PathInfo from the incoming parameter
              sURL = sURL.Left(sURL.GetLength() - sPathInfo.GetLength() - 1);

              dwMatchingURL = sClientURL.GetLength() - sPathInfo.GetLength();
              dwMatchingPath = sLocalFile.GetLength();
            }

            nLocalFileData++; //Move over the directory separator
          }
          else
          {
            bContinueParse = FALSE;

            CString sFile(sDirectory + sRelativeFile.Left(nLocalFileData) + sTemp);

            DWORD dwAttributes = GetFileAttributes(sFile);
            if (dwAttributes == INVALID_FILE_ATTRIBUTES)
            {
              sPathInfo = sTemp;
              sPathInfo.Replace(_T("\\"), _T("/"));
              sLocalFile = sDirectory + sRelativeFile.Left(nLocalFileData);

              //Remove the PathInfo from the incoming parameter
              sURL = sURL.Left(sURL.GetLength() - sPathInfo.GetLength());

              dwMatchingURL = sClientURL.GetLength() - sPathInfo.GetLength();
              dwMatchingPath = sLocalFile.GetLength();
            }
          }
        }

        bDirectory = pDirectory->GetDirectoryListing() && (((GetFileAttributes(sLocalFile) & FILE_ATTRIBUTE_DIRECTORY) != 0));
        if (bDirectory)
        {
          int nURLLength = sURL.GetLength();
          if (nURLLength && sURL.GetAt(nURLLength-1) != _T('/'))
            sURL += _T("/");
        }

      }

      bSuccess = TRUE;
    }
  }

  return bSuccess;
}

#ifdef _DEBUG
void CW3MFCClient::PostLog(int nHTTPStatusCode, DWORD dwBodyLength)
#else
void CW3MFCClient::PostLog(int /*nHTTPStatusCode*/, DWORD /*dwBodyLength*/)
#endif
{
#ifdef _DEBUG
  //The default is to just TRACE the details

  //Get the current date and time
  time_t now = time(NULL);
  tm localtm;
  localtime_s(&localtm, &now);
  tm* pNow = &localtm;  

  //Get the time zone information
  TIME_ZONE_INFORMATION tzi;
  memset(&tzi, 0, sizeof(tzi));
  int nTZBias = 0;
  if (GetTimeZoneInformation(&tzi) == TIME_ZONE_ID_DAYLIGHT)
    nTZBias = tzi.Bias + tzi.DaylightBias;
  else
    nTZBias = tzi.Bias;

  //Format the date and time appropiately
  TCHAR sDateTime[64];
  sDateTime[0] = _T('\0');
  _tcsftime(sDateTime, 64, _T("[%d/%b/%Y:%H:%M:%S"), pNow);

  //Display the connections to the console window
  CString sUser(m_Request.m_sUserName);
  if (sUser.IsEmpty())
    sUser = _T("-");
    
  TRACE(_T("%d.%d.%d.%d - %s [%s %+.2d%.2d] \"%s\" %d %d\n"), 
        static_cast<int>(m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b1),
        static_cast<int>(m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b2), 
        static_cast<int>(m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b3), 
        static_cast<int>(m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b4), 
        sUser.operator LPCTSTR(), sDateTime, -nTZBias/60, abs(nTZBias)%60, m_Request.m_sRequest.operator LPCTSTR(), nHTTPStatusCode, dwBodyLength);
#endif        
}

BOOL CW3MFCClient::TransmitFile(CW3MFCSocket& socket, CW3MFCResponseHeader& responseHdr, HANDLE hFile, DWORD dwSize, DWORD dwTimeout)
{
  //Validate our parameters
#ifdef W3MFC_SSL_SUPPORT  
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);
#endif  

  //Assume the worst
  BOOL bSuccess = FALSE;

#ifdef W3MFC_SSL_SUPPORT
  BOOL bTryUsingSendExtension = (pSettings->m_SSLProtocol == CW3MFCServerSettings::SSL_NONE);
#else
  BOOL bTryUsingSendExtension = TRUE;
#endif

  if (bTryUsingSendExtension && m_lpfnTransmitFile)
  {
    bSuccess = _TransmitFile(socket, responseHdr, hFile, dwSize);

    //Handle the error
    if (!bSuccess)
    {
      //Report the error
      CString sError;
      sError.Format(_T("CW3MFCClient::TransmitFile, Failed to send response via TransmitFile, Error:%d"), ::GetLastError());
      m_pServer->OnError(sError);
    }
  }
  else
  {
    //Send the header
  #ifdef W3MFC_SSL_SUPPORT
    bSuccess = responseHdr.Send(m_Socket, dwTimeout, m_SSL);
  #else
    bSuccess = responseHdr.Send(socket, dwTimeout);
  #endif
    if (!bSuccess)
    {
      //Report the error
      CString sError;
      sError.Format(_T("CW3MFCClient::TransmitFile, Failed to send to response header, Error:%d"), ::GetLastError());
      m_pServer->OnError(sError);
    }

    //Send the body
    if (bSuccess)
    {
      try
      {
        char sBuf[4096];
        DWORD dwBytesRead = 0;
        do 
        {
          if (::ReadFile(hFile, sBuf, 4096, &dwBytesRead, NULL) && dwBytesRead)
          #ifdef W3MFC_SSL_SUPPORT
            socket.SendWithRetry(sBuf, dwBytesRead, dwTimeout, m_SSL);
          #else
            socket.SendWithRetry(sBuf, dwBytesRead, dwTimeout);
          #endif
        } 
        while (dwBytesRead);
        bSuccess = TRUE;
      }
      catch(CWSocketException* pEx)
      {
        //Report the error
        CString sError;
        sError.Format(_T("CW3MFCClient::TransmitFile, Failed to send response file data, Error:%d"), pEx->m_nError);
        m_pServer->OnError(sError);

        pEx->Delete();  
      }
    }
  }

  return bSuccess;
}

BOOL CW3MFCClient::TransmitBuffer(CW3MFCSocket& socket, CW3MFCResponseHeader& responseHdr, CStringA& sData, DWORD dwTimeout)
{
  int nDataLength = sData.GetLength();
  LPSTR pszData = sData.GetBuffer();
  BOOL bSuccess = TransmitBuffer(socket, responseHdr, reinterpret_cast<BYTE*>(pszData), nDataLength, dwTimeout);
  sData.ReleaseBuffer();
  return bSuccess;
}

BOOL CW3MFCClient::TransmitBuffer(CW3MFCSocket& socket, CW3MFCResponseHeader& responseHdr, BYTE* byData, DWORD dwSize, DWORD dwTimeout)
{
  //Assume the worst
  BOOL bSuccess = FALSE;

#ifdef W3MFC_SSL_SUPPORT
  //Validate our parameters
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);

  BOOL bTryUsingSendExtension = (pSettings->m_SSLProtocol == CW3MFCServerSettings::SSL_NONE);
#else
  BOOL bTryUsingSendExtension = TRUE;
#endif

  if (bTryUsingSendExtension && m_lpfnTransmitPackets)
  {
    bSuccess = _TransmitBuffer(socket, responseHdr, byData, dwSize);
    if (!bSuccess)
    {
      //Report the error
      CString sError;
      sError.Format(_T("CW3MFCClient::TransmitBuffer, Failed to send response via TransmitPackets, Error:%d"), ::GetLastError());
      m_pServer->OnError(sError);
    }
  }
  else
  {
    //Send the header
  #ifdef W3MFC_SSL_SUPPORT
    bSuccess = responseHdr.Send(socket, dwTimeout, m_SSL);
  #else
    bSuccess = responseHdr.Send(socket, dwTimeout);
  #endif
    if (!bSuccess)
    {
      //Report the error
      CString sError;
      sError.Format(_T("CW3MFCClient::TransmitBuffer, Failed to send to response header, Error:%d"), ::GetLastError());
      m_pServer->OnError(sError);
    }

    //Send the body
    if (bSuccess && dwSize)
    {
      try
      {
      #ifdef W3MFC_SSL_SUPPORT
        socket.SendWithRetry(byData, dwSize, dwTimeout, m_SSL);
      #else
        socket.SendWithRetry(byData, dwSize, dwTimeout);
      #endif
        bSuccess = TRUE;
      }
      catch(CWSocketException* pEx)
      {
        //Report the error
        CString sError;
        sError.Format(_T("CW3MFCClient::TransmitBuffer, Failed to send to response body, Error:%d"), pEx->m_nError);
        m_pServer->OnError(sError);

        pEx->Delete();  
      }
    }
  }

  return bSuccess;
}

void CW3MFCClient::SetRequestToStop()
{
  //Let the base class do its thing
  CThreadPoolClient::SetRequestToStop();

  //Set the event which signals that we want this worker thread to exit
  m_StopEvent.SetEvent();
}

BOOL CW3MFCClient::GetKeySizeServerVariable(int& nKeySize)
{
  //What will be the return value
  BOOL bSuccess = FALSE;
  nKeySize = 0;

#ifdef W3MFC_SSL_SUPPORT
  //Validate our parameters
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);

  if (pSettings->m_SSLProtocol != CW3MFCServerSettings::SSL_NONE)
  {
    SSL_get_cipher_bits(m_SSL, &nKeySize);
    bSuccess = TRUE;
  }
#endif

  return bSuccess;
}

BOOL CW3MFCClient::GetServerKeySizeServerVariable(int& nKeySize)
{
  //What will be the return value
  BOOL bSuccess = FALSE;
  nKeySize = 0;

#ifdef W3MFC_SSL_SUPPORT
  //Validate our parameters
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);

  if (pSettings->m_SSLProtocol != CW3MFCServerSettings::SSL_NONE)
  {
    EVP_PKEY* pKey = SSL_get_privatekey(m_SSL);
    if (pKey)
    {
      nKeySize = EVP_PKEY_bits(pKey);
      bSuccess = TRUE;
    }
  }
#endif

  return bSuccess;
}

BOOL CW3MFCClient::GetCertSerialNumberServerVariable(long& nSerialNumber)
{
  //What will be the return value
  BOOL bSuccess = FALSE;
  nSerialNumber = 0;

#ifdef W3MFC_SSL_SUPPORT
  //Validate our parameters
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);

  if (pSettings->m_SSLProtocol != CW3MFCServerSettings::SSL_NONE)
  {
    X509* pCert = SSL_get_certificate(m_SSL);
    if (pCert)
    {
      ASN1_INTEGER* pSerialNumber = X509_get_serialNumber(pCert);
      if (pSerialNumber)
      {
        nSerialNumber = ASN1_INTEGER_get(pSerialNumber);
        bSuccess = TRUE;
      }
    }
  }
#endif

  return bSuccess;
}

BOOL CW3MFCClient::GetCertIssuerServerVariable(char*& szIssuer)
{
  //What will be the return value
  BOOL bSuccess = FALSE;
  szIssuer = NULL;

#ifdef W3MFC_SSL_SUPPORT
  //Validate our parameters
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);

  if (pSettings->m_SSLProtocol != CW3MFCServerSettings::SSL_NONE)
  {
    X509* pCert = SSL_get_certificate(m_SSL);
    if (pCert)
    {
      szIssuer = X509_NAME_oneline(X509_get_issuer_name(pCert), NULL, NULL);
      bSuccess = TRUE;
    }
  }
#endif

  return bSuccess;  
}

BOOL CW3MFCClient::GetCertSubjectServerVariable(char*& szSubject)
{
  //What will be the return value
  BOOL bSuccess = FALSE;
  szSubject = NULL;

#ifdef W3MFC_SSL_SUPPORT
  //Validate our parameters
  AFXASSUME(m_pServer);
  CW3MFCServerSettings* pSettings = m_pServer->GetSettings();
  AFXASSUME(pSettings);

  if (pSettings->m_SSLProtocol != CW3MFCServerSettings::SSL_NONE)
  {
    X509* pCert = SSL_get_certificate(m_SSL);
    if (pCert)
    {
      szSubject = X509_NAME_oneline(X509_get_subject_name(pCert), NULL, NULL);
      bSuccess = TRUE;
    }
  }
#endif

  return bSuccess;
}

#ifndef W3MFC_NO_ISAPI_SUPPORT
BOOL CW3MFCClient::TransmitFile(CW3MFCSocket& socket, CW3MFCResponseHeader& responseHdr, HSE_TF_INFO* pInfo)
{
  return _TransmitFile(socket, responseHdr, pInfo);
}
#endif

BOOL CW3MFCClient::PreHandleGetPostHead()
{
  //This allows derived classes to internally handle GET, POST or HEAD requests.
  //Return TRUE in your derived classes to tell the framework that the request
  //has been handled and stops the framework from implementing its default processing
  //for a URL

  //In this base class we of course allow default processing to occur
  return FALSE;
}
