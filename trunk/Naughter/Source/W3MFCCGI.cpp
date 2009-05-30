/*
Module : W3MFCCGI.cpp
Purpose: Implementation for the CW3MFCCGI class
Created: PJN / 26-02-2003
History: PJN / 03-01-2006 1. Fixed an issue in the CGI implementation so that HTTP headers such as User-Agent get mapped to HTTP_USER_AGENT 
                          environment variables. Basically all HTTP_... values should have any hyphens in their names replaced by underscores.
                          2. Now includes support for the DOCUMENT_ROOT CGI environment variable
                          3. Now returns a HTTP status code of 302 if a "Content:" header is found but no status code is present. Thanks to 
                          Hector Santos for reporting this issue.
                          4. Now CGI implementation supports Complete (Non-Parsed) CGI scripts. If you prefix your CGI script with "NPR-" then 
                          the data as received from the script will be sent to the client unmodified. Thanks to Hector Santos for suggesting this
                          nice addition.
                          5. Parsing of the HTTP status code line e.g. "HTTP/1.0 200 Ok" is now done using the CGI "Status:" header rather than 
                          looking for a "HTTP/.." line. Again thanks to Hector Santos for reporting this.
         PJN / 19-02-2006 1. Replaced all calls to ZeroMemory and CopyMemory with memset and memcpy
         PJN / 19-11-2007 1. CHttpCCGI class has been renamed to CW3MFCCGI
         PJN / 31-05-2008 1. Code now compiles cleanly using Code Analysis (/analyze)
                          2. Replaced use of CW32Handle class with ATL::CHandle

Copyright (c) 2003 - 2009 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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
#include "W3MfcRes.rh"
#include "W3MFCCGI.h"
#include "W3MFCClient.h"


//////////////// Macros ///////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////// Implementation ///////////////////////////////////////////////

CW3MFCCGI::CW3MFCCGI() : m_pszPendingHeader(NULL)
{
}

void CW3MFCCGI::TransmitCGIResponse(CW3MFCClient* pClient)
{
  //Validate our parameters
  AFXASSUME(pClient);
  AFXASSUME(pClient->m_pServer);

  //Check to make sure the file exists become we try to run it
  if (GetFileAttributes(pClient->m_Request.m_sLocalFile) == INVALID_FILE_ATTRIBUTES)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCCGI::TransmitCGIResponse, Requested file %s does not exist"), pClient->m_Request.m_sLocalFile.operator LPCTSTR());
    pClient->m_pServer->OnError(sError);

    //Handle the error
    pClient->ReturnErrorMessage(500);
    return;
  }

  //First need to create an anonymous pipe for the child process's redirected 
  //STDOUT
  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = NULL;
  sa.bInheritHandle = TRUE;
  ATL::CHandle ChildStdoutReadTemp;
  ATL::CHandle ChildStdoutWrite;
  if (!CreatePipe(&ChildStdoutReadTemp.m_h, &ChildStdoutWrite.m_h, &sa, 0)) 
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCCGI::TransmitCGIResponse, Failed to create anonymous pipes for STDOUT for CGI child process, Error:%d"), GetLastError());
    pClient->m_pServer->OnError(sError);

    //Handle the error
    pClient->ReturnErrorMessage(500);
    return;
  }

  //Next need to create an anonymous pipe for the child process's redirected 
  //STDIN
  ATL::CHandle ChildStdinRead;
  ATL::CHandle ChildStdinWriteTemp;
  if (pClient->m_Request.m_dwRawEntitySize)
  {
    if (!CreatePipe(&ChildStdinRead.m_h, &ChildStdinWriteTemp.m_h, &sa, 0)) 
    {
      //Report the error
      CString sError;
      sError.Format(_T("CW3MFCCGI::TransmitCGIResponse, Failed to create anonymous pipes for STDIN for CGI child process, Error:%d"), GetLastError());
      pClient->m_pServer->OnError(sError);

      //Handle the error
      pClient->ReturnErrorMessage(500);
      return;
    }
  }

  //Create a duplicate of the child's STDOUT write handle for the STDERR write handle.
  //This is necessary in case the child application closes one of its std output handles
  ATL::CHandle ChildStderrWrite;
  if (!DuplicateHandle(GetCurrentProcess(), ChildStdoutWrite, GetCurrentProcess(), &ChildStderrWrite.m_h, 0, TRUE, DUPLICATE_SAME_ACCESS))
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCCGI::TransmitCGIResponse, Failed to duplicate STDOUT handle for STDERR handle CGI child process, Error:%d"), GetLastError());
    pClient->m_pServer->OnError(sError);

    //Handle the error
    pClient->ReturnErrorMessage(500);
    return;
  }

  //Create new STDOUT read handle (which is non-inheritable). if we did not do this then the child inherits the 
  //properties and, as a result non-closeable handles to the pipes are created
  ATL::CHandle ChildStdoutRead;
  if (!DuplicateHandle(GetCurrentProcess(), ChildStdoutReadTemp, GetCurrentProcess(), &ChildStdoutRead.m_h, 0, FALSE, DUPLICATE_SAME_ACCESS))
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCCGI::TransmitCGIResponse, Failed to duplicate STDOUT read handle, Error:%d"), GetLastError());
    pClient->m_pServer->OnError(sError);

    //Handle the error
    pClient->ReturnErrorMessage(500);
    return;
  }

  //Close the temp STDOUT read handle now that we have duplicated it successfully
  ChildStdoutReadTemp.Close();

  //Create new write handle (if we need to which is non-inheritable). if we did not do this then the child inherits the 
  //properties and, as a result non-closeable handles to the pipes are created
  ATL::CHandle ChildStdinWrite;
  if (pClient->m_Request.m_dwRawEntitySize)
  {
    if (!DuplicateHandle(GetCurrentProcess(), ChildStdinWriteTemp, GetCurrentProcess(), &ChildStdinWrite.m_h, 0, FALSE, DUPLICATE_SAME_ACCESS))
    {
      //Report the error
      CString sError;
      sError.Format(_T("CW3MFCCGI::TransmitCGIResponse, Failed to duplicate STDIN write handle, Error:%d"), GetLastError());
      pClient->m_pServer->OnError(sError);

      //Handle the error
      pClient->ReturnErrorMessage(500);
      return;
    }

    //Close the temp STDIN write handle now that we have duplicated it successfully
    ChildStdinWriteTemp.Close();
  }

  //Now setup the structures for a call to CreateProcess
  PROCESS_INFORMATION pi;
  STARTUPINFO si;
  memset(&si, 0, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.hStdOutput = ChildStdoutWrite;
  si.hStdInput  = ChildStdinRead;
  si.hStdError  = ChildStderrWrite;
  si.wShowWindow = SW_HIDE;

  //Get the environment variables we will be sending down to the CGI process
  CString sEnvironment = FormCGIEnvironment(pClient);

  //Convert the "sEnvironment" CString into a MULTI_SZ suitable for calling CreateProcess with
  DWORD dwEnvironmentSize = sEnvironment.GetLength();
  DWORD dwNewEnvironmentSize = (dwEnvironmentSize + 2);
  TCHAR* pszEnvironment = new TCHAR[dwNewEnvironmentSize];
  _tcscpy_s(pszEnvironment, dwNewEnvironmentSize, sEnvironment);
  pszEnvironment[dwNewEnvironmentSize-1] = _T('\0'); //Double NULL terminate the data
  //Replace all '\n' with '\0'
  for (DWORD i=0; i<dwEnvironmentSize; i++)
  {
    if (pszEnvironment[i] == _T('\n'))
      pszEnvironment[i] = _T('\0');
  }
 
  //Setup the creation flags
  DWORD dwCreationFlags = CREATE_NEW_CONSOLE;
#ifdef _UNICODE
  dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
#endif

  //Get the working directory of the script
  TCHAR sDrive[_MAX_DRIVE];
  TCHAR sDir[_MAX_DIR];
  TCHAR sCWD[_MAX_PATH];
  _tsplitpath_s(pClient->m_Request.m_sLocalFile, sDrive, sizeof(sDrive)/sizeof(TCHAR), sDir, sizeof(sDir)/sizeof(TCHAR), NULL, 0, NULL, 0);
  _tmakepath_s(sCWD, sizeof(sCWD)/sizeof(TCHAR), sDrive, sDir, NULL, NULL);

  //Form the correct command line for the call to CreateProcess
  CString sCommandLine(GetCGICommandLine(pClient));

  //Launch the process that we want to redirect
  BOOL bSuccess = CreateProcess(NULL, sCommandLine.GetBuffer(sCommandLine.GetLength()), NULL, NULL, TRUE, dwCreationFlags, pszEnvironment, sCWD, &si, &pi);
  DWORD dwError = GetLastError();
  sCommandLine.ReleaseBuffer();

  //Tidy up the temp heap memory we have used
  delete [] pszEnvironment;

  //Close the STDOUT write handle as we have no use for it now
  ChildStdoutWrite.Close();

  //Close the STDIN read handle as we have no use for it now
  if (pClient->m_Request.m_dwRawEntitySize)
    ChildStdinRead.Close();

  //Close the STDERR write handle as we have no use for it now
  ChildStderrWrite.Close();

  //Handle the error if we could not run the child process
  if (!bSuccess)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCCGI::TransmitCGIResponse, Failed to run the CGI child process, Error:%d"), GetLastError());
    pClient->m_pServer->OnError(sError);

    //Return an appropriate error page
    if (dwError == ERROR_FILE_NOT_FOUND)
      pClient->ReturnErrorMessage(404);
    else
      pClient->ReturnErrorMessage(500);
    return;
  }

  //Write to stdin of the client
  if (pClient->m_Request.m_dwRawEntitySize)
  {
    WriteToChildStdin(pClient, ChildStdinWrite);

    //Close the pipe handle now that we are finished with it
    ChildStdinWrite.Close();
  }

  //Read from stdout of the client and send the data back down the socket
  TCHAR sFname[_MAX_FNAME];
  _tsplitpath_s(pClient->m_Request.m_sLocalFile, NULL, 0, NULL, 0, sFname, sizeof(sFname)/sizeof(TCHAR), NULL, 0);
  CString sNPRFile(sFname);
  sNPRFile.MakeUpper();
  BOOL bNPH = sNPRFile.Find(_T("NPR-")) == 0;
  DWORD dwDataSent = ReadFromClientStdout(pClient, ChildStdoutRead, pClient->m_bResponseKeepAlive, bNPH);
  if (dwDataSent == 0)
    pClient->ReturnErrorMessage(500);
  else
    pClient->PostLog(200, dwDataSent);

  //Close the anonymous pipe handles
  ChildStdoutRead.Close();

  //Wait until the child process exits
  WaitForSingleObject(pi.hProcess, INFINITE);

  // Close process and thread handles.     
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}

CString CW3MFCCGI::FormCGIEnvironment(CW3MFCClient* pClient)
{
  //Validate our parameters
  AFXASSUME(pClient);
  AFXASSUME(pClient->m_pServer);
  CW3MFCServerSettings* pSettings = pClient->m_pServer->GetSettings();
  AFXASSUME(pSettings);

  //Form the environment string to send to the child process
  CString sEnvironment;
  CString sLine;

  //Add all the environment variables W3MFC has before we add the custom ones
  for (TCHAR** szVariable = _tenviron; *szVariable; szVariable++ )
  {
    sEnvironment += *szVariable;
    sEnvironment += _T("\n");
  }    

  if (pClient->m_Request.m_AuthorizationType == CW3MFCRequest::HTTP_AUTHORIZATION_PLAINTEXT)
  {
    sEnvironment += _T("AUTH_TYPE=Basic\n");

    sLine.Format(_T("AUTH_REALM=%s\n"), pClient->m_Request.m_sURL.operator LPCTSTR());
    sEnvironment += sLine;

    sLine.Format(_T("REMOTE_USER=%s\n"), pClient->m_Request.m_sUserName.operator LPCTSTR());
    sEnvironment += sLine;

    sLine.Format(_T("AUTH_USER=%s\n"), pClient->m_Request.m_sUserName.operator LPCTSTR());
    sEnvironment += sLine;

    sLine.Format(_T("REMOTE_PASSWORD=%s\n"), pClient->m_Request.m_sPassword.operator LPCTSTR());
    sEnvironment += sLine;

    sLine.Format(_T("AUTH_PASSWORD=%s\n"), pClient->m_Request.m_sPassword.operator LPCTSTR());
    sEnvironment += sLine;
  }
  else if (pClient->m_Request.m_AuthorizationType == CW3MFCRequest::HTTP_AUTHORIZATION_NTLM)
  {
    sEnvironment += _T("AUTH_TYPE=NTLM\n");

    sLine.Format(_T("REMOTE_USER=%s\n"), pClient->m_Request.m_sUserName.operator LPCTSTR());
    sEnvironment += sLine;

    sLine.Format(_T("AUTH_USER=%s\n"), pClient->m_Request.m_sUserName.operator LPCTSTR());
    sEnvironment += sLine;
  }

  sEnvironment += _T("GATEWAY_INTERFACE=CGI/1.1\n");

  if (pSettings->m_sBindAddress.GetLength())
    sLine.Format(_T("SERVER_NAME=%s\n"), pSettings->m_sBindAddress.operator LPCTSTR());
  else
  {
    char pszHostName[256];
    if (gethostname(pszHostName, 256) == 0)
    {
      CString sName(pszHostName);
      sLine.Format(_T("SERVER_NAME=%s\n"), sName.operator LPCTSTR());
    }
    else
      sLine = (_T("SERVER_NAME=\n"));
  }
  sEnvironment += sLine;

  if (pClient->m_Request.m_nContentLength)
  {
    sLine.Format(_T("CONTENT_LENGTH=%d\n"), pClient->m_Request.m_nContentLength);
    sEnvironment += sLine;
  }

  if (pClient->m_Request.m_sContentType.GetLength())
  {
    sLine.Format(_T("CONTENT_TYPE=%s\n"), pClient->m_Request.m_sContentType.operator LPCTSTR());
    sEnvironment += sLine;
  }

  sLine.Format(_T("SERVER_PORT=%d\n"), pSettings->m_nPort);
  sEnvironment += sLine;

#ifdef W3MFC_SSL_SUPPORT
  if (pSettings->m_SSLProtocol == CW3MFCServerSettings::SSL_NONE)
    sEnvironment += _T("SERVER_PROTOCOL=HTTP/1.0\n");
  else
    sEnvironment += _T("SERVER_PROTOCOL=HTTPS/1.0\n");
#else
  sEnvironment += _T("SERVER_PROTOCOL=HTTP/1.0\n");
#endif

  sLine.Format(_T("SERVER_SOFTWARE=%s\n"), pSettings->m_sServerName.operator LPCTSTR());
  sEnvironment += sLine;

  CString sScriptName = pClient->m_Request.m_sLocalFile;
  sScriptName.Replace(_T('\\'), _T('/')); //Ensure we use unix directory separators
  if (sScriptName.GetLength() && sScriptName.GetAt(0) != _T('/'))
    sScriptName = _T("/") + sScriptName;
  sLine.Format(_T("SCRIPT_NAME=%s\n"), sScriptName.operator LPCTSTR());
  sEnvironment += sLine;

  //Form the absolute path for PATH_TRANSLATED
  LPTSTR pszFilePart;
  TCHAR pszAbsolutePath[_MAX_PATH];
  GetFullPathName(pClient->m_Request.m_sLocalFile, _MAX_PATH, pszAbsolutePath, &pszFilePart);
  sLine.Format(_T("PATH_TRANSLATED=%s\n"), pszAbsolutePath);
  sEnvironment += sLine;

  sLine.Format(_T("REQUEST_LINE=%s\n"), pClient->m_Request.m_sRequest.operator LPCTSTR());
  sEnvironment += sLine;

  sLine.Format(_T("QUERY_STRING=%s\n"), pClient->m_Request.m_sRawExtra.operator LPCTSTR());
  sEnvironment += sLine;
  
  if (!pClient->m_Request.m_sRemoteHost.IsEmpty())
  {
    sLine.Format(_T("REMOTE_HOST=%s\n"), pClient->m_Request.m_sRemoteHost.operator LPCTSTR());
    sEnvironment += sLine;
  }
  else
  {
    sLine.Format(_T("REMOTE_HOST=%d.%d.%d.%d\n"), pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b1,
                 pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b2, pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b3,
                 pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b4);
    sEnvironment += sLine;
  }

  sLine.Format(_T("REMOTE_ADDR=%d.%d.%d.%d\n"), pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b1,
               pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b2, pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b3,
               pClient->m_Request.m_ClientAddress.sin_addr.S_un.S_un_b.s_b4);
  sEnvironment += sLine;

  sLine.Format(_T("PATH_INFO=%s\n"), pClient->m_Request.m_sPathInfo.operator LPCTSTR());
  sEnvironment += sLine;

  sLine.Format(_T("REQUEST_METHOD=%s\n"), pClient->m_Request.m_sVerb.operator LPCTSTR());
  sEnvironment += sLine;

  CString sURL(_T("/"));
  CString sLocalFile;
  CString sPathInfo;
  BOOL bDirectory;
  CW3MFCDirectory* pDirectory;
  DWORD dwMatchingURL;
  DWORD dwMatchingPath;
  if (pClient->MapURLToLocalFilename(sURL, sLocalFile, sPathInfo, bDirectory, pDirectory, dwMatchingURL, dwMatchingPath))
  {
    ASSERT(pDirectory);

    sLine.Format(_T("DOCUMENT_ROOT=%s\n"), pDirectory->GetDirectory().operator LPCTSTR());
    sEnvironment += sLine;
  }

  int nKeySize = 0;
  if (pClient->GetKeySizeServerVariable(nKeySize))
  {
    sLine.Format(_T("HTTPS_KEYSIZE=%d\nCERT_KEYSIZE=%d\n"), nKeySize, nKeySize);
    sEnvironment += sLine;
  }

  if (pClient->GetServerKeySizeServerVariable(nKeySize))
  {
    sLine.Format(_T("HTTPS_SECRETKEYSIZE=%d\nCERT_SECRETKEYSIZE=%d\n"), nKeySize, nKeySize);
    sEnvironment += sLine;
  }

  sEnvironment += sLine;

#ifdef W3MFC_SSL_SUPPORT
  if (pSettings->m_SSLProtocol == CW3MFCServerSettings::SSL_NONE)
    sLine = _T("HTTPS=off\n");
  else
    sLine = _T("HTTPS=on\n");
#else
  sLine = _T("HTTPS=off\n");
#endif
  sEnvironment += sLine;

#ifdef W3MFC_SSL_SUPPORT
  if (pSettings->m_SSLProtocol != CW3MFCServerSettings::SSL_NONE)
  {
    sLine.Format(_T("SERVER_PORT_SECURE=%d\n"), pSettings->m_nPort);
    sEnvironment += sLine;
  }
#endif

  long nSerialNumber = 0;
  if (pClient->GetCertSerialNumberServerVariable(nSerialNumber))
  {
    sLine.Format(_T("CERT_SERIALNUMBER=%d\n"), nSerialNumber);
    sEnvironment += sLine;
  }

  sLine.Format(_T("REQUEST_URI=%s\n"), pClient->m_Request.m_sURL.operator LPCTSTR());
  sEnvironment += sLine;

  sLine.Format(_T("URL=%s\n"), pClient->m_Request.m_sURL.operator LPCTSTR());
  sEnvironment += sLine;

  if (pClient->m_Request.m_bLoggedOn)
  {
    sLine.Format(_T("LOGON_USER=%s\n"), pClient->m_Request.m_sUserName.operator LPCTSTR());
    sEnvironment += sLine;
  }
  else if (pSettings->m_sUserName.GetLength())
  {
    sLine.Format(_T("LOGON_USER=%s\n"), pSettings->m_sUserName.operator LPCTSTR());
    sEnvironment += sLine;
  }

  char* szIssuer = NULL;
  if (pClient->GetCertIssuerServerVariable(szIssuer))
  { 
    CString sIssuer(szIssuer);
    sLine.Format(_T("CERT_SERVER_ISSUER=%s\nHTTPS_SERVER_ISSUER=%s\n"), sIssuer.operator LPCTSTR(), sIssuer.operator LPCTSTR());
    sEnvironment += sLine;
  }

  char* szSubject = NULL;
  if (pClient->GetCertSubjectServerVariable(szSubject))
  { 
    CString sSubject(szSubject);
    sLine.Format(_T("CERT_SERVER_SUBJECT=%s\nHTTPS_SERVER_SUBJECT=%s\n"), sSubject.operator LPCTSTR(), sSubject.operator LPCTSTR());
    sEnvironment += sLine;
  }

  sLine.Format(_T("HTTP_VERSION=%d.%d\n"), HIWORD(pClient->m_Request.m_dwHttpVersion), LOWORD(pClient->m_Request.m_dwHttpVersion));
  sEnvironment += sLine;

  POSITION posMap = pClient->m_Request.m_HeaderMap.GetStartPosition();
  while (posMap)
  {
    CString sKey;
    CString sValue;
    pClient->m_Request.m_HeaderMap.GetNextAssoc(posMap, sKey, sValue);

    //Replace any hyphens in the key by underscores
    sKey.Replace(_T("-"), _T("_"));

    sLine.Format(_T("HTTP_%s=%s\n"), sKey.operator LPCTSTR(), sValue.operator LPCTSTR());
    sEnvironment += sLine;
  }

  return sEnvironment;
}

CString CW3MFCCGI::GetCGICommandLine(CW3MFCClient* pClient)
{
  //What will be the return value
  CString sCommandLine(pClient->m_Request.m_sLocalFile);

  //First get the extension of the file
  TCHAR sExt[_MAX_EXT];
  _tsplitpath_s(pClient->m_Request.m_sLocalFile, NULL, 0, NULL, 0, NULL, 0, sExt, sizeof(sExt)/sizeof(TCHAR));

  //Now get the command line for this extension from the registry
  TCHAR pszValue[1024/sizeof(TCHAR)];
  DWORD nData = 1024;
  HKEY hExtKey = NULL;
  if (RegOpenKeyEx(HKEY_CLASSES_ROOT, sExt, 0, KEY_READ, &hExtKey) == ERROR_SUCCESS)
  {
	  if (RegQueryValueEx(hExtKey, NULL, NULL, NULL, reinterpret_cast<LPBYTE>(pszValue), &nData) == ERROR_SUCCESS)
	  {
      //Close the registry key now that we are finished with it
      RegCloseKey(hExtKey);

		  //pszValue is now the Extension File Subkey to use.
		  CString sValue;
		  sValue.Format(_T("%s\\Shell\\Open\\Command"), pszValue);
		  nData = 1024; //reset the size parameter

      HKEY hExtensionFileKey = NULL;
      if (RegOpenKeyEx(HKEY_CLASSES_ROOT, sValue, 0, KEY_READ, &hExtensionFileKey) == ERROR_SUCCESS)
		  {
 			  if (RegQueryValueEx(hExtensionFileKey, NULL, NULL, NULL, reinterpret_cast<LPBYTE>(pszValue), &nData) == ERROR_SUCCESS)
			  {
          //Expand the string with any environment variables it may contain
          nData = 1024;
					LPTSTR szBuf = sCommandLine.GetBuffer(nData);
					DWORD  dwRet = ExpandEnvironmentStrings(pszValue, szBuf, nData); 
					sCommandLine.ReleaseBuffer();

          if (dwRet != 0)
          {
			      sCommandLine.Replace(_T("%L"), pClient->m_Request.m_sLocalFile);
			      sCommandLine.Replace(_T("%1"), pClient->m_Request.m_sLocalFile);
			      sCommandLine.Replace(_T("%*"), _T("")); 
          }
			  }

        //Close the registry key now that we are finished with it
			  RegCloseKey(hExtensionFileKey);
		  }
	  }
  }

  return sCommandLine;
}

void CW3MFCCGI::WriteToChildStdin(CW3MFCClient* pClient, HANDLE hChildStdin)
{
  //Write the entity body to STDIN of the CGI child process
  DWORD dwWritten;
  ASSERT(pClient->m_Request.m_pRawEntity);
  WriteFile(hChildStdin, pClient->m_Request.m_pRawEntity, pClient->m_Request.m_dwRawEntitySize, &dwWritten, NULL);
}

LPSTR CW3MFCCGI::FindNextTerminatorFromRequest(LPSTR pszLine, LPSTR psRawEntity)
{
  LPSTR pszCurrentLine = pszLine;
  while (TRUE)
  {
    ++pszCurrentLine;
    if (pszCurrentLine >= psRawEntity)
      return pszCurrentLine;
    else if (*pszCurrentLine == '\n')
      return pszCurrentLine;
  }
  
  return NULL;
}

DWORD CW3MFCCGI::ReadFromClientStdout(CW3MFCClient* pClient, HANDLE hChildStdout, BOOL& bFoundKeepAlive, BOOL bNPH) 
{ 
  //Validate our parameters
  AFXASSUME(pClient);
  AFXASSUME(pClient->m_pServer);
  CW3MFCServerSettings* pSettings = pClient->m_pServer->GetSettings();
  AFXASSUME(pSettings);

  //Initialize the out parameters
  bFoundKeepAlive = FALSE;
  DWORD dwDataSent = 0;

  //Allocate the receive / transmit buffer
  ATL::CHeapPtr<char> receiveBuffer;
  if (!receiveBuffer.Allocate(pSettings->m_dwCGIResponseBufferSize))
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCCGI::ReadFromClientStdout, Failed to allocate transmit buffer, Error:%d"), ::GetLastError());
    pClient->m_pServer->OnError(sError);

    return 0;
  }
  char* pbyBufferToXmit = receiveBuffer.m_pData; 

  //Read output from the child process, and write to parent's STDOUT. 
  BOOL bMore = TRUE;
  BOOL bFirstBuffer = TRUE;
  while (bMore) 
  { 
    DWORD dwRead;
    if (!ReadFile(hChildStdout, receiveBuffer.m_pData, pSettings->m_dwCGIResponseBufferSize-1, &dwRead, NULL) || dwRead == 0) 
      bMore = FALSE; 
    else
    {
      //Used as a return value to indicate how much was sent to the client
      dwDataSent += dwRead;
      
      BOOL bGotFullHeader = FALSE;
      CW3MFCResponseHeader responseHdr;
      responseHdr.SetAddEntitySeparator(TRUE);
      BOOL bSendHeader = FALSE;

      if (bFirstBuffer && !bNPH) //Only do parsing if not in NPH mode
      {
        bSendHeader = TRUE;  

        //We only do this parse on the first buffer
        bFirstBuffer = FALSE;

        //NULL terminate the data (so that we can use strstr)
        receiveBuffer.m_pData[dwRead] = '\0';

        //Have we found the header / body delimiter
        char* pszEOH = strstr(receiveBuffer.m_pData, "\r\n\r\n");
        LPSTR psRawEntity = pszEOH + 4;
        bGotFullHeader = (pszEOH != NULL);
        if (bGotFullHeader)
        {
          BOOL bAddedServer = FALSE;  
          int nToSendStatusCode = 0;
          BOOL bFoundConnectionKeepAlive = FALSE;

          //Process each header line one by one
          LPSTR pszLine = receiveBuffer.m_pData;
          LPSTR pszTerminator = strstr(receiveBuffer.m_pData, "\n");
          BOOL bMoreLines = TRUE;
          CStringArray sAdditionalHeaderLines;
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

              //Parse each line of the header
              if (sLine.GetLength())
              {
                CString sField;
                CString sValue;
                if (CW3MFCSocket::SplitRequestLine(sLine, sField, sValue))
                {
                  if ((sField.CompareNoCase(_T("Connection")) == 0) && (sValue.CompareNoCase(_T("Keep-Alive")) == 0))
                  {
                    bFoundConnectionKeepAlive = TRUE;
                    sAdditionalHeaderLines.Add(sLine);
                  }
                  else if (bFoundConnectionKeepAlive && sField.CompareNoCase(_T("Content-Length")) == 0)
                  {
                    bFoundKeepAlive = TRUE;
                    sAdditionalHeaderLines.Add(sLine);
                  }
                  else if (sField.CompareNoCase(_T("Status")) == 0)
                    nToSendStatusCode = _ttoi(sValue);
                  else if (sField.CompareNoCase(_T("Location")) == 0)
                  {
                    //Update the HTTP status code if not set already
                    if (nToSendStatusCode == 0)
                      nToSendStatusCode = 302;

                    sAdditionalHeaderLines.Add(sLine);
                  }  
                  else if (sField.CompareNoCase(_T("Server")) == 0)
                  {
                    bAddedServer = TRUE;
                    sAdditionalHeaderLines.Add(sLine);
                  }
                  else
                    sAdditionalHeaderLines.Add(sLine);
                }
              }

              //Move onto the next line
              if (pszTerminator)
              {
                pszLine = pszTerminator+1;
                if (pszLine >= pszEOH)
                  bMoreLines = FALSE;
                else 
                  pszTerminator = FindNextTerminatorFromRequest(pszLine, psRawEntity);
              }
            }  
            else
              bMoreLines = FALSE;
          }
          while (bMoreLines);

          //Now that we have done the parsing, form the header response
          if (nToSendStatusCode == 0) //Could not find a status code, then default it to 200
            nToSendStatusCode = 200;
          responseHdr.AddStatusCode(nToSendStatusCode);
          if (!bAddedServer)
            responseHdr.AddServer(pSettings->m_sServerName);
          for (INT_PTR i=0; i<sAdditionalHeaderLines.GetSize(); i++)
            responseHdr.AddExtraHeaders(CStringA(sAdditionalHeaderLines.GetAt(i) + _T("\r\n")));

          //Finally patch up the buffer to point to the body of the response
          dwRead -= static_cast<DWORD>(pszEOH + 4 - receiveBuffer.m_pData);
          pbyBufferToXmit = pszEOH + 4;
        }
        else
        {
          //Form our own complete header
          responseHdr.AddStatusCode(200);
          SYSTEMTIME st;
          GetSystemTime(&st);
          responseHdr.AddDate(st);
          responseHdr.AddServer(pSettings->m_sServerName);
          responseHdr.AddW3MfcAllowFields(pSettings->m_bAllowDeleteRequest);
          responseHdr.AddContentType(_T("text/plain"));
        }
      }

      //Send the data back down the socket
      try
      {
      #ifdef W3MFC_SSL_SUPPORT
        if (bSendHeader)
			    responseHdr.Send(pClient->m_Socket, pSettings->m_dwWritableTimeout, pClient->m_SSL);
        if (dwRead)
          pClient->m_Socket.SendWithRetry(pbyBufferToXmit, dwRead, pSettings->m_dwWritableTimeout, pClient->m_SSL);
      #else
        if (bSendHeader)
			    responseHdr.Send(pClient->m_Socket, pSettings->m_dwWritableTimeout);
        if (dwRead)
          pClient->m_Socket.SendWithRetry(pbyBufferToXmit, dwRead, pSettings->m_dwWritableTimeout);
      #endif
      }
      catch(CWSocketException* pEx)
      {
        //Report the error
        CString sError;
        sError.Format(_T("CW3MFCCGI::ReadFromClientStdout, Failed to send to socket, Error:%d"), pEx->m_nError);
        pClient->m_pServer->OnError(sError);

        pEx->Delete();  

        bMore = FALSE;
      }
    }
  } 

  return dwDataSent;
} 
