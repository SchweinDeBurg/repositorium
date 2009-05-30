/*
Module : W3MFCResponseHeader.cpp
Purpose: Implementation for a class to simplify sending Http response headers
Created: PJN / 22-04-1999
History: PJN / 27-06-2006 1. Optimized CHttpResponseHeader constructor code                    
         PJN / 12-01-2007 1. Fixed a bug in CHttpPResponseHeader::GetData to do with the calculation of
                          the ASCII header size when the code is compiled for Unicode. Thanks to 
                          Alexey V. Leonov for reporting this bug.
         PJN / 20-11-2007 1. CHttpResponseHeader class has been renamed CW3MFCResponseHeader
         PJN / 31-05-2008 1. Code now compiles cleanly using Code Analysis (/analyze)

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
#include "W3MFCResponseHeader.h"


//////////////// Macros ///////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////// Implementation ///////////////////////////////////////////////

CW3MFCResponseHeader::CW3MFCResponseHeader() : m_bEntitySeparator(TRUE)
{
}

void CW3MFCResponseHeader::SetAddEntitySeparator(BOOL bSeparator)
{
  m_bEntitySeparator = bSeparator;
}

void CW3MFCResponseHeader::AddStatus(LPCSTR pszStatusString)
{
  CStringA sLine;
  sLine.Format("HTTP/1.0 %s\r\n", pszStatusString);
  m_sHeader += sLine;
}

void CW3MFCResponseHeader::AddExtraHeaders(LPCSTR pszHeaders)
{
  m_sHeader += pszHeaders;
}

void CW3MFCResponseHeader::AddStatusCode(int nStatusCode)
{
  CStringA sLine;
  switch (nStatusCode)
  {
    case 200: sLine = "HTTP/1.0 200 OK\r\n";                    break;
    case 201: sLine = "HTTP/1.0 201 Created\r\n";               break;
    case 202: sLine = "HTTP/1.0 202 Accepted\r\n";              break;
    case 204: sLine = "HTTP/1.0 204 No Content\r\n";            break;
    case 300: sLine = "HTTP/1.0 300 Multiple Choices\r\n";      break;
    case 301: sLine = "HTTP/1.0 301 Moved Permanently\r\n";     break;
    case 302: sLine = "HTTP/1.0 302 Moved Temporarily\r\n";     break;
    case 304: sLine = "HTTP/1.0 304 Not Modified\r\n";          break;
    case 400: sLine = "HTTP/1.0 400 Bad Request\r\n";           break;
    case 401: sLine = "HTTP/1.0 401 Unauthorized\r\n";          break;
    case 403: sLine = "HTTP/1.0 403 Forbidden\r\n";             break;
    case 404: sLine = "HTTP/1.0 404 Not Found\r\n";             break;
    case 500: sLine = "HTTP/1.0 500 Internal Server Error\r\n"; break;
    case 501: sLine = "HTTP/1.0 501 Not Implemented\r\n";       break;
    case 502: sLine = "HTTP/1.0 502 Bad Gateway\r\n";           break;
    case 503: sLine = "HTTP/1.0 503 Service Unavailable\r\n";   break;
    default: sLine.Format("HTTP/1.0 %d\r\n", nStatusCode);      break;
  }
  m_sHeader += sLine;
}

void CW3MFCResponseHeader::AddContentLength(int nSize)
{
  CStringA sLine;
  sLine.Format("Content-Length: %d\r\n", nSize);
  m_sHeader += sLine;
}

void CW3MFCResponseHeader::AddContentType(LPCTSTR pszMediaType)
{
  CStringA sLine;
  sLine.Format("Content-Type: %s\r\n", CStringA(pszMediaType).operator LPCSTR());
  m_sHeader += sLine;
}

CStringA CW3MFCResponseHeader::DateToStr(const SYSTEMTIME& st)
{
  static char* sMonth[] =  
  {
    "Jan",
    "Feb",
    "Mar", 
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
  };

  static char* sDay[] =
  {
    "Sun",
    "Mon",
    "Tue", 
    "Wed",
    "Thu",
    "Fri",
    "Sat",
  };

  CStringA sDate;
  sDate.Format("%s, %02d %s %04d %02d:%02d:%02d GMT", sDay[st.wDayOfWeek], 
               st.wDay, sMonth[st.wMonth-1], st.wYear, st.wHour, st.wMinute, st.wSecond);
  return sDate;
}

void CW3MFCResponseHeader::AddW3MfcAllowFields(BOOL bAllowDeleteRequest)
{
  if (bAllowDeleteRequest)
    m_sHeader += "Allow: GET, POST, HEAD, DELETE\r\n";
  else
    m_sHeader += "Allow: GET, POST, HEAD\r\n";
}

void CW3MFCResponseHeader::AddDate(const SYSTEMTIME& st)
{
  CStringA sDate(DateToStr(st));
  CStringA sLine;
  sLine.Format("Date: %s\r\n", sDate.operator LPCSTR());
  m_sHeader += sLine;
}

void CW3MFCResponseHeader::AddLastModified(const SYSTEMTIME& st)
{
  CStringA sDate(DateToStr(st));
  CStringA sLine;
  sLine.Format("Last-Modified: %s\r\n", sDate.operator LPCSTR());
  m_sHeader += sLine;
}

void CW3MFCResponseHeader::AddExpires(const SYSTEMTIME& st)
{
  CStringA sDate(DateToStr(st));
  CStringA sLine;
  sLine.Format("Expires: %s\r\n", sDate.operator LPCSTR());
  m_sHeader += sLine;
}

void CW3MFCResponseHeader::AddKeepAlive()
{
  m_sHeader += _T("Connection: Keep-Alive\r\n");
}

void CW3MFCResponseHeader::AddWWWAuthenticateBasic(LPCTSTR pszRealm)
{
  CStringA sLine;
  sLine.Format("WWW-Authenticate: Basic realm=%s\r\n", CStringA(pszRealm).operator LPCSTR());
  m_sHeader += sLine;
}

void CW3MFCResponseHeader::AddWWWAuthenticateNTLM(LPCTSTR pszMessage)
{
  CStringA sLine;
  if (_tcslen(pszMessage))
    sLine.Format("WWW-Authenticate: NTLM %s\r\n", CStringA(pszMessage).operator LPCSTR());
  else
    sLine = _T("WWW-Authenticate: NTLM\r\n");
  m_sHeader += sLine;
}

void CW3MFCResponseHeader::AddLocation(LPCTSTR pszLocation)
{
  CStringA sLine;
  sLine.Format("Location: %s\r\n", CStringA(pszLocation).operator LPCSTR());
  m_sHeader += sLine;
}

void CW3MFCResponseHeader::AddServer(LPCTSTR pszServer)
{
  CStringA sLine;
  sLine.Format("Server: %s\r\n", CStringA(pszServer).operator LPCSTR());
  m_sHeader += sLine;    
}

BOOL CW3MFCResponseHeader::Send(CW3MFCSocket& socket, DWORD dwTimeout)
{
  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  //Get the ascii version of the data
  CStringA sData(GetData());

  try
  {
    socket.SendWithRetry(sData.operator LPCSTR(), sData.GetLength(), dwTimeout);
  }
  catch(CWSocketException* pEx)
  {
    TRACE(_T("CW3MFCResponseHeader::Send, Error:%d\n"), pEx->m_nError);

    pEx->Delete();  
    bSuccess = FALSE;
  }

  return bSuccess;
}

#ifdef W3MFC_SSL_SUPPORT
BOOL CW3MFCResponseHeader::Send(CW3MFCSocket& socket, DWORD dwTimeout, CSSL& ssl)
{
  //What will be the return value fom this function (assume the best)
  BOOL bSuccess = TRUE;

  if (ssl.operator SSL*())
  {
    //Get the ascii version of the data
    CStringA sData(GetData());

    //Send it down the socket  
    try
    {
      socket.SendWithRetry(sData.operator LPCSTR(), sData.GetLength(), dwTimeout, ssl);
    }
    catch(CWSocketException* pEx)
    {
      TRACE(_T("CW3MFCResponseHeader::Send, Error:%d\n"), pEx->m_nError);

      pEx->Delete();  
      bSuccess = FALSE;
    }
  }
  else
    bSuccess = Send(socket, dwTimeout);

  return bSuccess;
}
#endif

CStringA CW3MFCResponseHeader::GetData()
{
  //What will be the return value from this function
  CStringA sHeader(m_sHeader);
  
  //Add the entity separator if required
  if (m_bEntitySeparator)
    sHeader += "\r\n";
    
  return sHeader;
}
