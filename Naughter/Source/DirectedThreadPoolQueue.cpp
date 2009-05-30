/*
Module : DirectedThreadPoolQueue.cpp
Purpose: Implementation for an MFC class which implements a Pseudo IOCP like class which also 
         supports a directed post to the queue. i.e. the request is for a specific thread in the 
         thread pool (which is the normal use of a Directed IOCP). This can be used by a thread pool
         server (see the author's CThreadPoolServer class) to implement a thread pool which implements
         recycling.
Created: PJN / 16-04-2002
History: PJN / 11-11-2004 1. Provided a GetRequestArray() method which allows access to the internal
                          array used to hold the thread pool requests. Can prove handy to have access
                          to this in certain circumstances.
                          2. Updated to compile cleanly when Detect 64 bit issues and Force conformance in for loop
                          options are enabled in Visual Studio .Net
         PJN / 30-11-2004 1. Updated the macro which detects if arrays use INT_PTR for index rather than int.
                          2. Also removed some ASSERTS which were overly restrictive in light of the queue now 
                          being externally modifiable via CThreadPoolServer::GetQueue
         PJN / 13-04-2006 1. Updated copyright details.
                          2. Provision of Lock and Unlock methods along the lines of the author's CSyncCollection class
                          3. CDirectedThreadPoolQueue::PostRequest, PostRequestWithoutLimitCheck, Close and GetRequest now 
                          includes a parameter which decides if internal locking should be done. Again this allows 
                          more fine tuned external synchronisation of the class
                          4. Made timeout value in PostRequest default to "INFINITE"
                          5. Made GetDirectedRequestIndexToRemove method virtual
                          6. Optimized CDirectedThreadPoolQueue constructor code
         PJN / 08-08-2006 1. Addition of a m_evtDataAvailable member variable which is signalled when an item is available
                          on the queue
         PJN / 19-11-2007 1. Updated copyright details
         PJN / 01-05-2008 1. Addition of a GetCurrentQueueSize method.

Copyright (c) 2002 - 2009 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


//////////////////// Includes /////////////////////////////////////////////////

#include "stdafx.h"
#include "DirectedThreadPoolQueue.h"


/////////////////// Macros / Defines //////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


///////////////// Implementation //////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDirectedThreadPoolQueue, CThreadPoolQueue)

CDirectedThreadPoolQueue::CDirectedThreadPoolQueue() : m_hPostRequestSemaphore(NULL),
                                                       m_hGetRequestSemaphore(NULL)
{
}

CDirectedThreadPoolQueue::~CDirectedThreadPoolQueue()
{
  Close();
}

BOOL CDirectedThreadPoolQueue::Create(DWORD dwMaxQSize)
{
  //Close if already created (this will empty out the request Q for us)
  Close(TRUE);

  //Create the PostRequest semaphores
  m_hPostRequestSemaphore = CreateSemaphore(NULL, dwMaxQSize, dwMaxQSize, NULL);
  if (m_hPostRequestSemaphore == NULL)
  {
    TRACE(_T("CDirectedThreadPoolQueue::Create, Failed to create a PostRequest semaphore, Error:%d\n"), GetLastError());
    Close(TRUE);
    return FALSE;
  }
  m_PostRequestSemaphores.SetSize(0, dwMaxQSize);
  DWORD i;
  for (i=0; i<dwMaxQSize; i++)
  {
    HANDLE hSemaphore = CreateSemaphore(NULL, dwMaxQSize, dwMaxQSize, NULL);
    if (hSemaphore == NULL)
    {
      TRACE(_T("CDirectedThreadPoolQueue::Create, Failed to create a PostRequest semaphore, Error:%d\n"), GetLastError());
      Close(TRUE);
      return FALSE;
    }

    m_PostRequestSemaphores.Add(hSemaphore);
  }

  //Create the GetRequest semaphores
  m_hGetRequestSemaphore = CreateSemaphore(NULL, 0, dwMaxQSize, NULL);
  if (m_hGetRequestSemaphore == NULL)
  {
    TRACE(_T("CDirectedThreadPoolQueue::Create, Failed to create a GetRequest semaphore, Error:%d\n"), GetLastError());
    Close(TRUE);
    return FALSE;
  }
  m_GetRequestSemaphores.SetSize(0, dwMaxQSize);
  for (i=0; i<dwMaxQSize; i++)
  {
    HANDLE hSemaphore = CreateSemaphore(NULL, 0, dwMaxQSize, NULL);
    if (hSemaphore == NULL)
    {
      TRACE(_T("CDirectedThreadPoolQueue::Create, Failed to create a GetRequest semaphore, Error:%d\n"), GetLastError());
      Close(TRUE);
      return FALSE;
    }

    m_GetRequestSemaphores.Add(hSemaphore);
  }

  return TRUE;
}

BOOL CDirectedThreadPoolQueue::PostRequestWithoutLimitCheck(const CThreadPoolRequest& request, BOOL bLock)
{
  ASSERT(IsCreated()); //Must have been created

  //By default assume the best
  BOOL bSuccess = TRUE;

  //Add the request to the request queue
  CSingleLock sl(&m_csRequests, FALSE);
  if (bLock)
    sl.Lock();
  CThreadPoolRequest CopyOfRequest(request);
  INT_PTR nRequestIndex = m_Requests.Add(CopyOfRequest);

  //Release the semaphore
  if (request.m_bDirectedRequest)
  {
    if (!ReleaseSemaphore(m_GetRequestSemaphores.GetAt(request.m_nDirectedRequestIndex), 1, NULL))
    {
      bSuccess = FALSE;

      //Remove the item from the Q since we could not update the "Get" semaphores
      m_Requests.RemoveAt(nRequestIndex);

      TRACE(_T("CDirectedThreadPoolQueue::PostRequestWithoutLimitCheck, Failed to release a semaphore, Error:%d\n"), GetLastError());
    }
  }
  else
  {
    if (!ReleaseSemaphore(m_hGetRequestSemaphore, 1, NULL))
    {
      bSuccess = FALSE;

      //Remove the item from the Q since we could not update the "Get" semaphores
      m_Requests.RemoveAt(nRequestIndex);

      TRACE(_T("CDirectedThreadPoolQueue::PostRequestWithoutLimitCheck, Failed to release a semaphore, Error:%d\n"), GetLastError());
    }
  }

  //set the event to signify that data is now available on the collection
  if (!m_evtDataAvailable.SetEvent())
    TRACE(_T("CDirectedThreadPoolQueue::PostRequestWithoutLimitCheck, Failed to set event, Error:%d\n"), GetLastError());

  return bSuccess;
}

BOOL CDirectedThreadPoolQueue::PostRequest(const CThreadPoolRequest& request, DWORD dwMilliseconds, BOOL bLock)
{
  ASSERT(IsCreated()); //Must have been created

  //Wait for the post request semaphore
  DWORD dwWait;
  if (request.m_bDirectedRequest)
    dwWait = WaitForSingleObject(m_PostRequestSemaphores.GetAt(request.m_nDirectedRequestIndex), dwMilliseconds);
  else
    dwWait = WaitForSingleObject(m_hPostRequestSemaphore, dwMilliseconds);
  if (dwWait != WAIT_OBJECT_0)
  {
    TRACE(_T("CDirectedThreadPoolQueue::PostRequest, Failed while waiting for the Q to free up, Error:%d\n"), GetLastError());
    return FALSE;
  }

  //Pass the buck to the other PostRequest method
  return PostRequestWithoutLimitCheck(request, bLock);
}

INT_PTR CDirectedThreadPoolQueue::GetNonDirectedRequestIndexToRemove()
{
  //Work out the item to remove from the Q, by default we pick
  //the first non directed request starting from the tail of the queue
  INT_PTR nIndexToRemoveAt = -1;
  INT_PTR nRequestSize = m_Requests.GetSize();
  INT_PTR i;
  for (i=0; (i<nRequestSize) && (nIndexToRemoveAt == -1); i++)
  {
    CThreadPoolRequest& tempRequest = m_Requests.ElementAt(i);
    if (!tempRequest.m_bDirectedRequest)
      nIndexToRemoveAt = i;
  }

  return nIndexToRemoveAt;
}

INT_PTR CDirectedThreadPoolQueue::GetDirectedRequestIndexToRemove(int nThreadIndexForDirectedRequest)
{
  //Work out the item to remove from the Q, by default we pick the first 
  //directed request for this thread starting from the tail of the queue
  INT_PTR nIndexToRemoveAt = -1;
  INT_PTR nRequestSize = m_Requests.GetSize();
  INT_PTR i;
  ASSERT(nRequestSize);
  for (i=0; (i<nRequestSize) && (nIndexToRemoveAt == -1); i++)
  {
    CThreadPoolRequest& tempRequest = m_Requests.ElementAt(i);
    if (tempRequest.m_bDirectedRequest && nThreadIndexForDirectedRequest == tempRequest.m_nDirectedRequestIndex)
      nIndexToRemoveAt = i;
  }

  return nIndexToRemoveAt;
}

BOOL CDirectedThreadPoolQueue::GetRequest(CThreadPoolRequest& request, int nThreadIndexForDirectedRequest, DWORD dwMilliseconds, BOOL bLock)
{
  ASSERT(IsCreated()); //Must have been created

  //Wait for either a non directed request or a directed request for this thread to become available on the Q
  HANDLE hWaitHandles[2];
  hWaitHandles[0] = m_GetRequestSemaphores.GetAt(nThreadIndexForDirectedRequest);
  hWaitHandles[1] = m_hGetRequestSemaphore;
  DWORD dwWait = WaitForMultipleObjects(2, hWaitHandles, FALSE, dwMilliseconds);
  int nSignaledHandle = dwWait - WAIT_OBJECT_0;

  //Work out what the return value from WFMO means!
  BOOL bRemoveDirected = FALSE;
  if (nSignaledHandle == 0)
    bRemoveDirected = TRUE;
  else if (nSignaledHandle != 1)
  {
    TRACE(_T("CDirectedThreadPoolQueue::GetRequest, Failed while waiting for the item on the Q, Error:%d\n"), GetLastError());
    return FALSE;
  }

  //What will be the return value from the function (assume the best)
  BOOL bSuccess = TRUE;

  //Remove some item from the request Q  
  CSingleLock sl(&m_csRequests, FALSE);
  if (bLock)
    sl.Lock(); //Lock down access to the Q

  if (bRemoveDirected)
  {
    //Work out the item to remove from the Q
    INT_PTR nIndexToRemoveAt = GetDirectedRequestIndexToRemove(nThreadIndexForDirectedRequest);
    ASSERT(nIndexToRemoveAt != -1); //something has gone badly wrong if we could not find a request to remove
    request = m_Requests.GetAt(nIndexToRemoveAt);
    ASSERT(request.m_bDirectedRequest); //the GetDirectedRequestIndexToRemove call above has returned an incorrect index
    m_Requests.RemoveAt(nIndexToRemoveAt);

    //Release the PostRequest semaphore
    ReleaseSemaphore(m_PostRequestSemaphores.GetAt(nThreadIndexForDirectedRequest), 1, NULL);
  }
  else
  {
    //Work out the item to remove from the Q
    INT_PTR nIndexToRemoveAt = GetNonDirectedRequestIndexToRemove();
    if (nIndexToRemoveAt != -1)
    {
      request = m_Requests.GetAt(nIndexToRemoveAt);
      ASSERT(!request.m_bDirectedRequest); //the GetNonDirectedRequestIndexToRemove call above has returned an incorrect index
      m_Requests.RemoveAt(nIndexToRemoveAt);

      //Release the PostRequest semaphore
      ReleaseSemaphore(m_hPostRequestSemaphore, 1, NULL);
    }
    else
      bSuccess = FALSE;
  }
  
  //Now check to see if there is an outstanding item on the queue after we have removed the current item
  //and if so signal the data available event
  INT_PTR nItems = m_Requests.GetSize();
  if (nItems)
  {
    if (!m_evtDataAvailable.SetEvent())
      TRACE(_T("CDirectedThreadPoolQueue::GetRequest, Failed to set event, Error:%d\n"), GetLastError());
  }

  return bSuccess;
}

BOOL CDirectedThreadPoolQueue::Close(BOOL bLock)
{
  //Empty out the request queue
  CSingleLock sl(&m_csRequests, FALSE);
  if (bLock)
    sl.Lock();
  m_Requests.RemoveAll();
  
  //Free up the PostRequest semaphores
  if (m_hPostRequestSemaphore)
  {
    CloseHandle(m_hPostRequestSemaphore);
    m_hPostRequestSemaphore = NULL;
  }
  INT_PTR i;
  for (i=0; i<m_PostRequestSemaphores.GetSize(); i++)
  {
    HANDLE hSemaphore = m_PostRequestSemaphores.GetAt(i);
    CloseHandle(hSemaphore);
  }
  m_PostRequestSemaphores.SetSize(0);

  //Free up the GetRequest semaphores
  if (m_hGetRequestSemaphore)
  {
    CloseHandle(m_hGetRequestSemaphore);
    m_hGetRequestSemaphore = NULL;
  }
  for (i=0; i<m_GetRequestSemaphores.GetSize(); i++)
  {
    HANDLE hSemaphore = m_GetRequestSemaphores.GetAt(i);
    CloseHandle(hSemaphore);
  }
  m_GetRequestSemaphores.SetSize(0);

  return TRUE;
}

BOOL CDirectedThreadPoolQueue::IsCreated() const
{
  return (m_hPostRequestSemaphore != NULL);
}

INT_PTR CDirectedThreadPoolQueue::GetCurrentQueueSize()
{
  //Serialize access to the requests array
  CSingleLock sl(&m_csRequests, TRUE);

  return m_Requests.GetSize();
}