/*
Module : PJNNTLMAuth.H
Purpose: Defines the interface for a simple wrapper class for client NTLM authentication via SSPI
Created: PJN / 07-03-2005

Copyright (c) 2005 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

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

#ifndef __PJNNTLMAUTH_H__
#define __PJNNTLMAUTH_H__

#ifndef SECURITY_WIN32
#define SECURITY_WIN32
#endif
#include <security.h>

#ifndef PJNNTLMAUTH_EXT_CLASS
#define PJNNTLMAUTH_EXT_CLASS
#endif


/////////////////////////////// Classes ///////////////////////////////////////

//////// A class which performs NTLM authentication ///////////////////////////

class PJNNTLMAUTH_EXT_CLASS CNTLMClientAuth
{
public:
//Methods
  SECURITY_STATUS NTLMAuthenticate();

//Virtual methods
  virtual SECURITY_STATUS NTLMAuthPhase1(PBYTE pBuf, DWORD cbBuf) = 0;
  virtual SECURITY_STATUS NTLMAuthPhase2(PBYTE pBuf, DWORD cbBuf, DWORD* pcbRead) = 0;
  virtual SECURITY_STATUS NTLMAuthPhase3(PBYTE pBuf, DWORD cbBuf) = 0;

protected:
//Misc Methods
  SECURITY_STATUS GenClientContext(BYTE* pIn, DWORD cbIn, BYTE* pOut, DWORD* pcbOut, BOOL* pfDone, CredHandle& hCred, SecHandle& hContext);
  SECURITY_STATUS DoNTLMAuthentication(CredHandle& hCred, SecHandle& hContext);
};

#endif //__PJNNTLMAUTH_H__
