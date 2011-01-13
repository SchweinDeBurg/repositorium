/*
Module : W3MFCRequest.cpp
Purpose: Implementation for the CW3MFCRequest class
Created: PJN / 30-09-1999
History: PJN / 19-02-2006 1. Replaced all calls to ZeroMemory and CopyMemory with memset and memcpy
         PJN / 12-01-2007 1. Optimized CW3MFCRequest constructor code
         PJN / 19-11-2007 1. Updated copyright details.
                          2. CHttpRequest class has been renamed CW3MFCRequest

Copyright (c) 1999 - 2011 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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
#include "W3MFCRequest.h"


//////////////// Macros ///////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////// Implementation ///////////////////////////////////////////////

CW3MFCRequest::CW3MFCRequest(int nHeaderHashTable) : m_Verb(HTTP_VERB_UNKNOWN),
                                                   m_dwHttpVersion(0),
                                                   m_bIfModifiedSincePresent(FALSE),
                                                   m_AuthorizationType(HTTP_AUTHORIZATION_ANONYMOUS),
                                                   m_pRawRequest(NULL),
                                                   m_dwRawRequestSize(0),
                                                   m_pRawEntity(NULL),
                                                   m_dwRawEntitySize(0),
                                                   m_nContentLength(0),
                                                   m_bKeepAlive(FALSE),
                                                   m_bFirstAuthenticationRequest(TRUE),
                                                   m_bAuthenticationComplete(FALSE),
                                                   m_bLoggedOn(FALSE),
                                                   m_hImpersonation(NULL)
{
  memset(&m_IfModifiedSince, 0, sizeof(SYSTEMTIME));
  memset(&m_ClientAddress, 0, sizeof(m_ClientAddress));
#ifndef W3MFC_NO_SSPI_SUPPORT
  memset(&m_ContentHandle, 0, sizeof(CtxtHandle));
#endif

  m_HeaderMap.InitHashTable(nHeaderHashTable);
}

CW3MFCRequest& CW3MFCRequest::operator=(const CW3MFCRequest& request)
{
  //Free up any existing memory before we copy over
  if (m_pRawRequest)
  {
    delete [] m_pRawRequest;
    m_pRawRequest = NULL;
  }

  m_nContentLength          = request.m_nContentLength;
  m_sRequest                = request.m_sRequest;
  m_Verb                    = request.m_Verb;
  m_sVerb                   = request.m_sVerb;
  m_ClientAddress           = request.m_ClientAddress;
  m_sURL                    = request.m_sURL;
  m_sRawURL                 = request.m_sRawURL;
  m_sPathInfo               = request.m_sPathInfo;
  m_sExtra                  = request.m_sExtra;
  m_sRawExtra               = request.m_sRawExtra;
  m_dwHttpVersion           = request.m_dwHttpVersion;
  m_bIfModifiedSincePresent = request.m_bIfModifiedSincePresent; 
  memcpy(&m_IfModifiedSince, &request.m_IfModifiedSince, sizeof(SYSTEMTIME));
  m_AuthorizationType       = request.m_AuthorizationType;
  m_sUserName               = request.m_sUserName;
  m_sPassword               = request.m_sPassword;
  m_sRemoteHost             = request.m_sRemoteHost;
  m_sContentType            = request.m_sContentType;
  m_bKeepAlive              = request.m_bKeepAlive;
  m_sAuthenticationResponse = request.m_sAuthenticationResponse;
  //Note: we deliberatly do not copy across context handle or the first authentication request boolean
  m_bAuthenticationComplete = request.m_bAuthenticationComplete;
  m_sAuthorityName          = request.m_sAuthorityName;
  m_sLocalFile              = request.m_sLocalFile;

  if (request.m_pRawRequest)
  {
    m_pRawRequest = new BYTE[request.m_dwRawRequestSize];
    m_dwRawRequestSize  = request.m_dwRawRequestSize;
    m_dwRawEntitySize   = request.m_dwRawEntitySize;
    memcpy(m_pRawRequest, request.m_pRawRequest, m_dwRawRequestSize);
    if (m_dwRawEntitySize)
      m_pRawEntity = m_pRawRequest + (m_dwRawRequestSize - m_dwRawEntitySize);
    else
      m_pRawEntity = NULL;
  }
  else
  {
    m_pRawRequest = NULL;
    m_dwRawRequestSize = 0;
    m_pRawEntity = NULL;
    m_dwRawEntitySize = 0;
  }

  m_HeaderMap.RemoveAll();
  m_HeaderMap.InitHashTable(request.m_HeaderMap.GetHashTableSize());
  POSITION posMap = request.m_HeaderMap.GetStartPosition();
  while (posMap)
  {
    CString sKey;
    CString sValue;
    request.m_HeaderMap.GetNextAssoc(posMap, sKey, sValue);
    m_HeaderMap.SetAt(sKey, sValue);
  }

  m_hImpersonation = request.m_hImpersonation;

  return *this;
}
