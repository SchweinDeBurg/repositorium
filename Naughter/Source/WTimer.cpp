/*
Module : WTimer.cpp
Purpose: Implementation for an MFC wrapper class for waitable timers
Created: PJN / 06-08-2000
History: PJN / 08-11-2002 1. Removed some unnecessary comments in the module
                          2. Updated copyright details in the module
         PJN / 08-02-2004 1. Completion routine can now be passed an LPVOID parameter just like 
                          the SDK equivalent. Thanks to Erenst F. Wepener for suggesting this 
                          addition.
                          2. Minor updates to the code to allow the code to compile on VC 5 
                          without the requirement for the Platform SDK to be installed.
         PJN / 26-06-2006 1. Updated copyright details.
                          2. Removed various defines in code which were taken from the Platform SDK. 
                          This does mean that the code now requires the Platform SDK if compiled using VC 6.
                          3. Optimized CWaitableTimer constructor code.
                          4. Combined the functionality of the _WAITABLE_TIMER_DATA class into the main 
                          CWaitableTimer class.
                          5. Code now uses new C++ style casts rather than old style C casts where necessary. 
                          6. Reviewed all TRACE statements for correctness
         PJN / 01-06-2008 1. Code now compiles cleanly using Code Analysis (/analyze)
                          2. The code has now been updated to support VC 2005 or later only.  
                          3. Updated the sample app to clean compile on VC 2008

Copyright (c) 2000 - 2008 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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
#include "WTimer.h"


//////////////// Macros / Locals /////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////// Implementation //////////////////////////////////////

CWaitableTimer::CWaitableTimer() : m_hTimer(NULL),
                                   m_bSignalWhenSuspended(FALSE),
                                   m_lpfnCompletionRoutine(NULL),
                                   m_lpArgToCompletionRoutine(NULL),
                                   m_lpfnOpenWaitableTimer(NULL),
                                   m_lpfnCreateWaitableTimer(NULL),
                                   m_lpfnSetWaitableTimer(NULL),
                                   m_lpfnCancelWaitableTimer(NULL)
{
  m_hKernel32 = GetModuleHandle(_T("KERNEL32.DLL"));
  if (m_hKernel32 != NULL)
  {
  #ifdef _UNICODE
    m_lpfnOpenWaitableTimer = reinterpret_cast<LPOPENWAITABLETIMER>(GetProcAddress(m_hKernel32, "OpenWaitableTimerW"));
    m_lpfnCreateWaitableTimer = reinterpret_cast<LPCREATEWAITABLETIMER>(GetProcAddress(m_hKernel32, "CreateWaitableTimerW"));
  #else
    m_lpfnOpenWaitableTimer = reinterpret_cast<LPOPENWAITABLETIMER>(GetProcAddress(m_hKernel32, "OpenWaitableTimerA"));
    m_lpfnCreateWaitableTimer = reinterpret_cast<LPCREATEWAITABLETIMER>(GetProcAddress(m_hKernel32, "CreateWaitableTimerA"));
  #endif
    m_lpfnSetWaitableTimer = reinterpret_cast<LPSETWAITABLETIMER>(GetProcAddress(m_hKernel32, "SetWaitableTimer"));
    m_lpfnCancelWaitableTimer = reinterpret_cast<LPCANCELWAITABLETIMER>(GetProcAddress(m_hKernel32, "CancelWaitableTimer"));
  }
}

CWaitableTimer::~CWaitableTimer()
{
  Close();
}

BOOL CWaitableTimer::Attach(HANDLE hTimer)
{
  Close();
  m_hTimer = hTimer;
  return TRUE;
}

HANDLE CWaitableTimer::Detach()
{
  HANDLE hReturn = m_hTimer;
  m_hTimer = NULL;
  return hReturn;
}

BOOL CWaitableTimer::Open(LPCTSTR pszName, BOOL bInherit, DWORD dwAccess)
{
  //Free up any existant timer
  Close();

  //Call the SDK function
  if (m_lpfnOpenWaitableTimer)
  {
    m_hTimer = m_lpfnOpenWaitableTimer(dwAccess, bInherit, pszName);
    return (m_hTimer != NULL);
  }
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
}

BOOL CWaitableTimer::Create(BOOL bManualReset, LPCTSTR pszName, LPSECURITY_ATTRIBUTES lpTimerAttributes)
{
  //Free up any existant timer
  Close();

  //Call the SDK function
  if (m_lpfnCreateWaitableTimer)
  {
    m_hTimer = m_lpfnCreateWaitableTimer(lpTimerAttributes, bManualReset, pszName);
    return (m_hTimer != NULL);
  }
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
}

void CWaitableTimer::Close()
{
  if (m_hTimer)
  {
    //Free up any existant timer
    CloseHandle(m_hTimer);
    m_hTimer = NULL;
    m_bSignalWhenSuspended = FALSE;
  }
}

BOOL CWaitableTimer::SetOnceOffAbsolute(const SYSTEMTIME& DueTime)
{
  ASSERT(m_hTimer); //Timer must be valid

  //Call the SDK function
  if (m_lpfnSetWaitableTimer)
  {
    //Convert from the SystemTime to the filetime struct
    FILETIME ft;
    if (!SystemTimeToFileTime(&DueTime, &ft))
    {
      TRACE(_T("CWaitableTimer::SetOnceOffAbsolute, Failed in call to SystemTimeToFileTime in SetOnceOffAbsolute\n"));
      return FALSE;
    }

    LARGE_INTEGER li;
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime; 
    return m_lpfnSetWaitableTimer(m_hTimer, &li, 0, m_lpfnCompletionRoutine ? _CompletionRoutine : NULL, this, m_bSignalWhenSuspended);
  }
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
}

BOOL CWaitableTimer::SetOnceOffRelative(DWORD dwMilliseconds)
{
  ASSERT(m_hTimer); //Timer must be valid

  //Call the SDK function
  if (m_lpfnSetWaitableTimer)
  {
    //Convert from the milliseconds to FILETIME representation (100-nanoseconds)
    //We also need to make it negative
    __int64 nFileTimes = static_cast<__int64>(-10000) * static_cast<__int64>(dwMilliseconds);
    LARGE_INTEGER li;
    li.LowPart = static_cast<DWORD>(nFileTimes & 0xFFFFFFFF);
    li.HighPart = static_cast<LONG>(nFileTimes >> 32); 
    return m_lpfnSetWaitableTimer(m_hTimer, &li, 0, m_lpfnCompletionRoutine ? _CompletionRoutine : NULL, this, m_bSignalWhenSuspended);
  }
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
}

BOOL CWaitableTimer::SetPeriodicAbsolute(const SYSTEMTIME& DueTime, DWORD dwPeriod)
{
  ASSERT(m_hTimer); //Timer must be valid

  //Call the SDK function
  if (m_lpfnSetWaitableTimer)
  {
    //Convert from the SystemTime to the filetime struct
    FILETIME ft;
    if (!SystemTimeToFileTime(&DueTime, &ft))
    {
      TRACE(_T("CWaitableTimer::SetPeriodicAbsolute, Failed in call to SystemTimeToFileTime in SetPeriodicAbsolute\n"));
      return FALSE;
    }

    LARGE_INTEGER li;
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime; 
    return m_lpfnSetWaitableTimer(m_hTimer, &li, dwPeriod, m_lpfnCompletionRoutine ? _CompletionRoutine : NULL, this, m_bSignalWhenSuspended);
  }
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
}

BOOL CWaitableTimer::SetPeriodicRelative(DWORD dwMilliseconds, DWORD dwPeriod)
{
  ASSERT(m_hTimer); //Timer must be valid

  //Call the SDK function
  if (m_lpfnSetWaitableTimer)
  {
    //Convert from the milliseconds to FILETIME representation (100-nanoseconds)
    //We also need to make it negative
    __int64 nFileTimes = static_cast<__int64>(-10000) * static_cast<__int64>(dwMilliseconds);
    LARGE_INTEGER li;
    li.LowPart = static_cast<DWORD>(nFileTimes & 0xFFFFFFFF);
    li.HighPart = static_cast<LONG>(nFileTimes >> 32); 
    return m_lpfnSetWaitableTimer(m_hTimer, &li, dwPeriod, m_lpfnCompletionRoutine ? _CompletionRoutine : NULL, this, m_bSignalWhenSuspended);
  }
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
}

BOOL CWaitableTimer::Cancel()
{
  ASSERT(m_hTimer); //Timer must be valid

  //Call the SDK function
  if (m_lpfnCancelWaitableTimer)
    return m_lpfnCancelWaitableTimer(m_hTimer);
  else
  {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
}

VOID CALLBACK CWaitableTimer::_CompletionRoutine(LPVOID lpArgToCompletionRoutine, DWORD dwTimerLowValue, DWORD dwTimerHighValue)
{
  //Convert from the SDK world to the C++ world
  CWaitableTimer* pTimer = static_cast<CWaitableTimer*>(lpArgToCompletionRoutine);
  AFXASSUME(pTimer);

  //Call the C++ callback method if there is one
  if (pTimer->m_lpfnCompletionRoutine)
  {
    FILETIME ft;
    ft.dwLowDateTime = dwTimerLowValue;
    ft.dwHighDateTime = dwTimerHighValue;

    //Call the C++ callback method
    pTimer->m_lpfnCompletionRoutine(pTimer->m_lpArgToCompletionRoutine, ft);
  }
}
