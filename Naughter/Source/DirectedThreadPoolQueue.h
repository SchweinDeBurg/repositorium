/*
Module : DirectedThreadPoolQueue.h
Purpose: Interface for an MFC class which implements a Pseudo IOCP like class which also 
         supports a "Directed" request to the IOCP. i.e. the request is for a specific thread 
         in the thread pool (which is the normal use of a Directed IOCP) instead of a 
         "Non-Directed" request which can be picked up by any thread in the thread pool.
Created: PJN / 16-04-2002

Copyright (c) 2002 - 2011 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


///////////////////// Defines /////////////////////////////////////////////////

#pragma once

#ifndef __DIRECTEDTHREADPOOLQUEUE_H__
#define __DIRECTEDTHREADPOOLQUEUE_H__

#ifndef THRDPOOL_EXT_CLASS
#define THRDPOOL_EXT_CLASS
#endif


///////////////////// Includes ////////////////////////////////////////////////

#include "ThrdPool.h"


///////////////////// Classes /////////////////////////////////////////////////

typedef CArray<CThreadPoolRequest, CThreadPoolRequest&> CDirectedThreadPoolRequestArray;

//Class which implements a "Directed" IO Completion Port like queue
class THRDPOOL_EXT_CLASS CDirectedThreadPoolQueue : public CThreadPoolQueue
{
public:
//Constructors / Destructors
  CDirectedThreadPoolQueue();
  virtual ~CDirectedThreadPoolQueue(); 

//Methods
  virtual BOOL Create(DWORD dwMaxQSize);
  virtual BOOL Close(BOOL bLock = TRUE);
  virtual BOOL PostRequest(const CThreadPoolRequest& request, DWORD dwMilliseconds = INFINITE, BOOL bLock = TRUE);
  virtual BOOL PostRequestWithoutLimitCheck(const CThreadPoolRequest& request, BOOL bLock = TRUE);
  virtual BOOL GetRequest(CThreadPoolRequest& request, int nThreadIndexForDirectedRequest, DWORD dwMilliseconds, BOOL bLock, BOOL bPumpMessage);
  virtual BOOL IsCreated() const;
  virtual INT_PTR GetDirectedRequestIndexToRemove(int nThreadIndexForDirectedRequest);
  virtual INT_PTR GetNonDirectedRequestIndexToRemove();
  virtual BOOL SupportsDirectedRequests() const { return TRUE; };
  CDirectedThreadPoolRequestArray& GetRequestArray() { return m_Requests; };  
  virtual INT_PTR GetCurrentQueueSize();

//Member variables
  CEvent m_evtDataAvailable; //Signalled when a item is available on the queue

protected:
  DECLARE_DYNCREATE(CDirectedThreadPoolQueue)

//Member variables
  HANDLE                          m_hPostRequestSemaphore; //Semaphore which is used to implement synchronisation in "PostRequest"
  HANDLE                          m_hGetRequestSemaphore;  //Semaphore which is used to implement synchronisation in "GetRequest"
  CCriticalSection                m_csRequests;            //Serializes access to the Q
  CDirectedThreadPoolRequestArray m_Requests;              //The request list
  CArray<HANDLE, HANDLE>          m_PostRequestSemaphores; //Array of semaphores used for signalling when a directed request is available
  CArray<HANDLE, HANDLE>          m_GetRequestSemaphores;  //Array of semaphores used for signalling when a directed request is available
};

#endif //__DIRECTEDTHREADPOOLQUEUE_H__
