/*
Module : PJJNSMTP.CPP
Purpose: Implementation for a simple wrapper class for client NTLM authentication via SSPI
Created: PJN / 22-05-1998
History: None


Copyright (c) 2005 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


//////////////// Includes ////////////////////////////////////////////

#include "stdafx.h"
#include "PJNNTLMAuth.h"

#if defined(__INTEL_COMPILER)
// remark #177: controlling expression is constant
#pragma warning(disable: 177)
#endif	// __INTEL_COMPILER

//////////////// Macros / Locals /////////////////////////////////////

#if (_MFC_VER < 0x700)
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifndef SEC_SUCCESS
#define SEC_SUCCESS(Status) ((Status) >= 0)
#endif
#pragma comment(lib, "Secur32.lib") //Automatically link to the security dll



//////////////// Implementation //////////////////////////////////////

SECURITY_STATUS CNTLMClientAuth::NTLMAuthenticate()
{
  CredHandle hCred;
  ZeroMemory(&hCred, sizeof(hCred));
  SecHandle hContext;
  ZeroMemory(&hContext, sizeof(hContext));

  //Call the helper function which does all the work
  SECURITY_STATUS ss = DoNTLMAuthentication(hCred, hContext);

  //Free up the security context if valid
  if (hContext.dwLower != 0 || hContext.dwUpper != 0) 
    DeleteSecurityContext(&hContext);

  //Free up the credentials handle if valid
  if (hCred.dwLower != 0 || hCred.dwUpper != 0) 
    FreeCredentialHandle(&hCred);

  return ss;
}

SECURITY_STATUS CNTLMClientAuth::DoNTLMAuthentication(CredHandle& hCred, SecHandle& hContext)
{
  BOOL fDone = FALSE;
  DWORD cbIn = 0;
  BYTE InBuf[12000];
  BYTE OutBuf[12000];
  DWORD cbMaxMessage = sizeof(InBuf);
  DWORD cbOut = cbMaxMessage;

  SECURITY_STATUS ss = GenClientContext(NULL, 0, OutBuf, &cbOut, &fDone, hCred, hContext);
  if (!SEC_SUCCESS(ss))
    return ss;

  ss = NTLMAuthPhase1(OutBuf, cbOut);
  if (!SEC_SUCCESS(ss))
    return ss;

  while (!fDone) 
  {
    ss = NTLMAuthPhase2(InBuf, sizeof(InBuf), &cbIn);
    if (!SEC_SUCCESS(ss))
      return ss;

    cbOut = cbMaxMessage;

    ss = GenClientContext(InBuf, cbIn, OutBuf, &cbOut, &fDone, hCred, hContext);
    if (!SEC_SUCCESS(ss))
      return ss;

    ss = NTLMAuthPhase3(OutBuf, cbOut);
    if (!SEC_SUCCESS(ss))
      return ss;
  }

  return ss;
}

SECURITY_STATUS CNTLMClientAuth::GenClientContext(BYTE* pIn, DWORD cbIn, BYTE* pOut, DWORD* pcbOut, BOOL* pfDone, CredHandle& hCred, SecHandle& hContext)
{
  TimeStamp Lifetime;
  SECURITY_STATUS ss;
  if (NULL == pIn)  
  {   
    ss = AcquireCredentialsHandle(NULL, _T("NTLM"), SECPKG_CRED_OUTBOUND, NULL, NULL, NULL, NULL, &hCred, &Lifetime);
    if (!SEC_SUCCESS(ss))
      return ss;
  }

  //Prepare the buffers
  SecBufferDesc OutBuffDesc;
  OutBuffDesc.ulVersion = 0;
  OutBuffDesc.cBuffers  = 1;
  SecBuffer OutSecBuff;
  OutBuffDesc.pBuffers  = &OutSecBuff;
  OutSecBuff.cbBuffer   = *pcbOut;
  OutSecBuff.BufferType = SECBUFFER_TOKEN;
  OutSecBuff.pvBuffer   = pOut;

  //The input buffer is created only if a message has been received from the server.
  SecBufferDesc InBuffDesc;
  SecBuffer InSecBuff;
  if (pIn)   
  {
    InBuffDesc.ulVersion = 0;
    InBuffDesc.cBuffers  = 1;
    InBuffDesc.pBuffers  = &InSecBuff;
    InSecBuff.cbBuffer   = cbIn;
    InSecBuff.BufferType = SECBUFFER_TOKEN;
    InSecBuff.pvBuffer   = pIn;

    ULONG ContextAttributes;
    ss = InitializeSecurityContext(&hCred, &hContext, NULL, 0, 0, SECURITY_NATIVE_DREP, &InBuffDesc, 0, &hContext, &OutBuffDesc, &ContextAttributes, &Lifetime);
  }
  else
  {
    ULONG ContextAttributes;
    ss = InitializeSecurityContext(&hCred, NULL, NULL, 0, 0, SECURITY_NATIVE_DREP, NULL, 0, &hContext, &OutBuffDesc, &ContextAttributes, &Lifetime);
  }

  if (!SEC_SUCCESS(ss))
    return ss;

  //If necessary, complete the token.
  if ((SEC_I_COMPLETE_NEEDED == ss) || (SEC_I_COMPLETE_AND_CONTINUE == ss))  
  {
    ss = CompleteAuthToken(&hContext, &OutBuffDesc);
    if (!SEC_SUCCESS(ss))  
      return ss;
  }

  *pcbOut = OutSecBuff.cbBuffer;
  *pfDone = !((SEC_I_CONTINUE_NEEDED == ss) || (SEC_I_COMPLETE_AND_CONTINUE == ss));

  return ss;
}
