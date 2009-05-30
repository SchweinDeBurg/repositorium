/*
Module : WTimer.h
Purpose: Defines the interface for an MFC wrapper class for waitable timers
Created: PJN / 06-08-2000

Copyright (c) 2000 - 2008 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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

#ifndef __WTIMER_H__
#define __WTIMER_H__

#ifndef CWAITABLETIMER_EXT_CLASS
#define CWAITABLETIMER_EXT_CLASS
#endif


/////////////////////////////// Classes ///////////////////////////////////////

class CWAITABLETIMER_EXT_CLASS CWaitableTimer
{
public:
//Constructors / Destructors
  CWaitableTimer();
  virtual ~CWaitableTimer();

//typedefs
  typedef void (COMPLETION_ROUTINE)(LPVOID, const FILETIME&);
  typedef COMPLETION_ROUTINE* LPCOMPLETION_ROUTINE;

//General methods
  BOOL     Open(LPCTSTR pszName, BOOL bInherit = FALSE, DWORD dwAccess = TIMER_ALL_ACCESS);
  BOOL     Create(BOOL bManualReset, LPCTSTR pszName = NULL, LPSECURITY_ATTRIBUTES lpTimerAttributes = NULL);
  void     Close();
  operator HANDLE() { return m_hTimer; };
  BOOL     Attach(HANDLE hTimer);
  HANDLE   Detach();

//Timer specific methods
  BOOL     SetOnceOffAbsolute(const SYSTEMTIME& DueTime);
  BOOL     SetOnceOffRelative(DWORD dwMilliseconds);
  BOOL     SetPeriodicAbsolute(const SYSTEMTIME& DueTime, DWORD dwPeriod);
  BOOL     SetPeriodicRelative(DWORD dwMilliseconds, DWORD dwPeriod);
  void     SetSignalWhenSuspended(BOOL bSignal) { m_bSignalWhenSuspended = bSignal; };
  BOOL     Cancel();
  void     SetCompletionRoutine(LPCOMPLETION_ROUTINE lpCompletionRoutine, LPVOID lpArgToCompletionRoutine = NULL) { m_lpfnCompletionRoutine = lpCompletionRoutine; m_lpArgToCompletionRoutine = lpArgToCompletionRoutine; };

protected:
//Methods
  static VOID CALLBACK _CompletionRoutine(LPVOID lpArgToCompletionRoutine, DWORD dwTimerLowValue, DWORD dwTimerHighValue);

//typedefs
  typedef HANDLE (WINAPI OPENWAITABLETIMER)(DWORD, BOOL, LPCTSTR);
  typedef OPENWAITABLETIMER* LPOPENWAITABLETIMER;

  typedef HANDLE (WINAPI CREATEWAITABLETIMER)(LPSECURITY_ATTRIBUTES, BOOL, LPCTSTR);
  typedef CREATEWAITABLETIMER* LPCREATEWAITABLETIMER;

  typedef VOID (APIENTRY *PTIMERAPCROUTINE)(LPVOID, DWORD, DWORD);
  typedef BOOL (WINAPI SETWAITABLETIMER)(HANDLE, const LARGE_INTEGER*, LONG, PTIMERAPCROUTINE, LPVOID, BOOL);
  typedef SETWAITABLETIMER* LPSETWAITABLETIMER;

  typedef BOOL (WINAPI CANCELWAITABLETIMER)(HANDLE);
  typedef CANCELWAITABLETIMER* LPCANCELWAITABLETIMER;

//Member variables
  HANDLE                m_hTimer;
  BOOL                  m_bSignalWhenSuspended;
  LPCOMPLETION_ROUTINE  m_lpfnCompletionRoutine;
  LPVOID                m_lpArgToCompletionRoutine;
  HMODULE               m_hKernel32;
  LPOPENWAITABLETIMER   m_lpfnOpenWaitableTimer;
  LPCREATEWAITABLETIMER m_lpfnCreateWaitableTimer;
  LPSETWAITABLETIMER    m_lpfnSetWaitableTimer;
  LPCANCELWAITABLETIMER m_lpfnCancelWaitableTimer;
};

#endif //__WTIMER_H__
