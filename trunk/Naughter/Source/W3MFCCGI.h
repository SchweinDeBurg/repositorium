/*
Module : W3MFCCGI.h
Purpose: Defines the interface for the CW3MFCCGI class
Created: PJN / 26-02-2003

Copyright (c) 2003 - 2011 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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

#ifndef __W3MFCCGI_H__
#define __W3MFCCGI_H__

#ifndef W3MFC_EXT_CLASS
#define W3MFC_EXT_CLASS
#endif


/////////////////////////////// Classes ///////////////////////////////////////

//Forward declaration
class CW3MFCClient;

class W3MFC_EXT_CLASS CW3MFCCGI
{
public:
//Constructors / Destructors
  CW3MFCCGI();

//Virtual methods
  virtual void    TransmitCGIResponse(CW3MFCClient* pClient);
  virtual CString FormCGIEnvironment(CW3MFCClient* pClient);
  virtual CString GetCGICommandLine(CW3MFCClient* pClient);
  virtual DWORD   ReadFromClientStdout(CW3MFCClient* pClient, HANDLE hChildStdout, BOOL& bFoundKeepAlive, BOOL bNPH);
  virtual void    WriteToChildStdin(CW3MFCClient* pClient, HANDLE hChildStdin);
  virtual LPSTR   FindNextTerminatorFromRequest(LPSTR pszLine, LPSTR psRawEntity);

//member variables
  char* m_pszPendingHeader; //The pending HTTP header to send to the client (NULL terminated)
  DWORD m_dwPendingheaderPosCurrentPos;  
};

#endif //__W3MFCCGI_H__
