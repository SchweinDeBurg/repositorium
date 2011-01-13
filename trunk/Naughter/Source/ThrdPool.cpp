/*
Module : ThrdPool.cpp
Purpose: implementation for an MFC wrapper class for thread pools
Created: PJN / 15-04-2001
History: PJN / 21-07-2001 1. Made destructors of the two classes virtual as both can be used as base classes
         PJN / 25-07-2001 1. Code now uses a Win9x compatible IO completion port if we fail to use the build 
                          in OS one. This IOCP is implemented in the class "CIOCP9x" in IOCP9x.h/cpp.
         PJN / 07-08-2001 1. Added a WaitForThreadsInitInstance. This allows code in the main thread to synchronise
                          with the all the InitInstance calls in the thread pool
         PJN / 23-08-2001 1. Removed unnecessary "#pragma message"
         PJN / 15-04-2002 1. Class now uses new "CDirectedIOCPRequest" class instead of an SDK IOCP or dummy one
                          for Win9x. This allows the thread pool to now support recycling of threads after a 
                          specified interval in the thread pool.
                          2. Tidied up the exposed API to not reflect the exact attributes of IO completion
                          ports and instead be more object oriented.
                          3. Change some of the class API parameters to provide for more flexibility
         PJN / 29-04-2002 1. Fixed a bug in the CDirectedIOCP class which was causing multiple threads in the thread 
                          pool to be released into the depths of the CDirectedIOCP::GetRequest code.
                          2. Fixed a bug which was causing m_Threads array to be accessed from multiple threads
                          when thread recycling was enabled.
         PJN / 16-05-2002 1. Now provides an option to have a Q size different than the thread pool size.
                          2. Also provides a method to post to the IOCP without first checking the limit
         PJN / 18-08-2002 1. Renamed CDirectedIOCP to CDirectedThreadPoolQueue
                          2. Renamed CDirectedIOCPRequest to CThreadPoolRequest
                          3. Now user can decide which queing mechanism to use thro the Start method. 2 pre built
                          classes are provided, namely CDirectedThreadPoolQueue and CIOCPThreadPoolQueue
                          4. Provision of virtual GetNonDirectedRequestIndexToRemove and GetDirectedRequestIndexToRemove
                          methods in the CDirectedThreadPoolQueue class. This allows derived classes to implement 
                          their own schemes as to which requests to prioritize on the thread pool queue
         PJN / 20-08-2002 1. Provided virtual destructors for all the classes which constitute the Thread pool framework
                          2. Removed forward reference of the now defunct class CDirectedIOCP
                          3. Removed unreferenced parameters level 4 warnings in the CThreadPool class declaration
                          4. Fixed usage of 2 int variables in CDirectedThreadPoolQueue::GetNonDirectedRequestIndexToRemove
                          and GetDirectedRequestIndexToRemove which were incorrectly declared as BOOL's. Thanks to 
                          Serhiy Pavlov for these updates.
         PJN / 04-10-2002 1. CThreadPoolClient::Run now has a return value to decide whether or not the worker thread
                          should continue to service requests upon return from handling the current request
         PJN / 08-10-2002 1. Shutting down of the thread pool now uses directed requests instead of undirected requests. 
                          This should improve the speed of shutdown of the thread pool when it contains a lot of requests
                          on the queue.
                          2. Defined enums for m_dwID member of the CThreadPoolRequest class
         PJN / 12-10-2002 1. Removed and replaced the PostRequestWithoutLimitCheck method with the standard PostRequest
                          method. This avoids the problem with TRACE messages appearing along the lines
                          "CDirectedThreadPoolQueue::GetRequest, Failed to release a semaphore". Thanks to Frank Schmidt
                          for reporting this problem.
                          2. Fixed a minor flow in "CDirectedThreadPoolQueue::Create()" where I forgot to call Close() 
                          when the creation of "m_hGetRequestSemaphore" fails. Again thanks to Frank Schmidt for spotting 
                          this.
         PJN / 14-10-2002 1. Reintroduced the function CThreadPoolQueue::PostRequestWithoutLimitCheck as some users of
                          the thread pool class had legit reasons to use this function.
                          2. Changed a VERIFY call into an ASSERT in CThreadPoolServer::RecycleThread
         PJN / 17-10-2002 1. Fixed a problem where CThreadPoolServer::Stop() would hang if an I/O completion port based
                          thread pool is being used. Thanks to Frank Schmidt for spotting this problem.
                          2. Made the thread pool class Win95 compliant by dynamically linking to the waitable timer 
                          API's. Even though the code did gracefully degrade if the waitable timer functions failed, the
                          fact that they did not use GetProcAddress to link to the functions meant that any app / dll
                          which included the thread pool class would fail to load on Win95. Thanks to Frank Schmidt 
                          for this update.
         PJN / 07-11-2002 1. Minor update to the thread pool class to provide a virtual function which gets call when 
                          the m_bRequestToStop is being set. 
         PJN / 13-01-2004 1. Made the m_bRequestToStop member variable "volatile" as it can be modified from
                          multiple threads while possible been read in a loop in another thread. Thanks to Dan Baker
                          for reporting this issue.
         PJN / 25-10-2004 1. Updated to compile cleanly when Detect 64 bit issues and Force conformance in for loop
                          options are enabled in Visual Studio .Net
         PJN / 06-09-2005 1. Reworked the CThreadPoolClient::_Run method so that it can now be customized at runtime. 
                          There is now a non static version which is also called _Run and there is also a virtual Main 
                          function which implements the main loop of the thread pool.
                          2. A CEvent namely "m_RequestToStop" is now also set when the thread pool is being stopped.
         PJN / 13-04-2006 1. Updated copyright details.
                          2. Renamed CThreadPoolClient::m_RequestToStop to m_evtRequestToStop.
         PJN / 31-05-2006 1. Removed the virtual destructor for CThreadPoolRequest. Please note that this class is not
                          meant to be derived from. For further information on its usage please read the comments in 
                          Thrdpool.h
                          2. Reworked thread recycling logic in CThreadPoolServer::Monitor to use a much simpler
                          WaitForSingleObject call.
                          3. Optimized CThreadPoolServer constructor code
                          4. Optimized CThreadPoolRequest constructor code
         PJN / 08-08-2006 1. Provision of a CThreadPoolClient::m_bPumpMessageQueue boolean. This allows you to specify 
                          that internally the thread pool client will use MsgWaitForMultipleObjects to wait for a request
                          to appear on the thread pool queue or a windows message to appear on the thread's system queue. If
                          a windows message is detected, then the thread pool will internally pump the message queue until 
                          no more messages are present. If CThreadPoolClient::m_bPumpMessageQueue is left at the default of 
                          FALSE, then you will get the old thread pool behaviour which is to directly call into 
                          CThreadPoolQueue::GetRequest which will block until a request is available. This new addition prevents 
                          problems if your thread pool initializes and uses COM as an STA. In this case the thread is required 
                          to pump messages since COM uses the message queue for synchronisation when it is initialized as an STA.
                          If you do not pump the message queue when you are using COM as an STA, you will notice unusual behaviour 
                          such as the Windows shell being unresponsive when ShellExecute calls are running. This is because 
                          internally ShellExecute uses DDE which itself broadcasts window messages to all top level windows. When 
                          you call CoInitialize to use an STA, internally a top level window is created, but because there is no 
                          code servicing this windows message queue, the shell's ShellExecute calls will seem to hang for 30 
                          seconds. Boy did it take me some work to find the root of this problem. You have been warned!!!
         PJN / 19-11-2007 1. THREADPOOL_SHUTDOWN_REQUEST and THREADPOOL_USER_DEFINED_REQUEST constants have now been made enums of
                          the CThreadPoolRequest class
         PJN / 31-05-2008 1. Updated copyright details
                          2. Code now compiles cleanly using Code Analysis (/analyze)

Copyright (c) 2002 - 2011 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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
#include "ThrdPool.h"
#include "DirectedThreadPoolQueue.h"


/////////////////// Macros / Defines //////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////// Implementation ////////////////////////////////////////////

CThreadPoolRequest::CThreadPoolRequest() : m_bDirectedRequest(FALSE),
                                           m_nDirectedRequestIndex(-1),
                                           m_dwID(THREADPOOL_USER_DEFINED_REQUEST),
                                           m_pData(NULL)
{
}

CThreadPoolRequest::CThreadPoolRequest(const CThreadPoolRequest& request)
{
  *this = request;
}

CThreadPoolRequest& CThreadPoolRequest::operator=(const CThreadPoolRequest& request)
{
  m_bDirectedRequest = request.m_bDirectedRequest;
  m_nDirectedRequestIndex = request.m_nDirectedRequestIndex;
  m_dwID = request.m_dwID;
  m_pData = request.m_pData;
  
  return *this;
}


IMPLEMENT_DYNCREATE(CThreadPoolClient, CObject)

CThreadPoolClient::CThreadPoolClient() : m_evtRequestToStop(FALSE, TRUE), //we want this event to be a manual reset event
                                         m_pWorkerThread(NULL),
                                         m_pPoolServer(NULL),
                                         m_lInitOK(FALSE),
                                         m_lRequestToStop(FALSE),
                                         m_nStartupThreadPriority(THREAD_PRIORITY_NORMAL),
                                         m_nStackSize(0),
                                         m_nThreadIndex(-1),
                                         m_bPumpMessageQueue(FALSE)
{
}

CThreadPoolClient::~CThreadPoolClient()
{
  ASSERT(m_pWorkerThread == NULL); //Thread should be destroyed by now
}

BOOL CThreadPoolClient::InitInstance()
{
  return TRUE; //Default behaviour is to allow "Run" to be called
}

int CThreadPoolClient::ExitInstance()
{
  return 1; //By default, return 1 as the thread exit code
}

BOOL CThreadPoolClient::Run(const CThreadPoolRequest& /*request*/)
{
  ASSERT(FALSE); //You need to override CThreadPoolClient::Run in your derived class
  return FALSE;
}

void CThreadPoolClient::SetRequestToStop()
{
  //Signal the boolean and the event
  InterlockedExchange(&m_lRequestToStop, TRUE);
  m_evtRequestToStop.SetEvent();
}

UINT CThreadPoolClient::_Run(LPVOID pParam)
{
  //Validate our parameters
  AFXASSUME(pParam);

  //Get back the "this" pointer
  CThreadPoolClient* pClient = static_cast<CThreadPoolClient*>(pParam);
  AFXASSUME(pClient);
  ASSERT(pClient->IsKindOf(RUNTIME_CLASS(CThreadPoolClient)));

  //Delegate the work to the non static version of _Run
  return pClient->_Run();
}

UINT CThreadPoolClient::_Run()
{
  //Call InitInstance to allow thread pool initialization customisation,
  //also store its return value in m_bInitOK
  InterlockedExchange(&m_lInitOK, InitInstance());

  //Signal the event to let the server know that we have completed InitInstance
  m_evtInitCompleted.SetEvent();

  //Should we run the "main" loop
  if (m_lInitOK)
    Main(); //Call the Main function which implements the "main" loop

  //Let the ExitInstance method decide the return code from the thread
  return ExitInstance();
}

void CThreadPoolClient::Main()
{
  //Validate our parameters
  AFXASSUME(m_pPoolServer);
  CThreadPoolQueue* pQueue = m_pPoolServer->GetQueue();
  AFXASSUME(pQueue);
  if (m_bPumpMessageQueue)
  {
    CDirectedThreadPoolQueue* pDirectedQueue = static_cast<CDirectedThreadPoolQueue*>(pQueue);
    AFXASSUME(pDirectedQueue);
    ASSERT(pDirectedQueue->IsKindOf(RUNTIME_CLASS(CDirectedThreadPoolQueue))); //We only supporting pumping messages for a "DirectedThreadPool" Queue
    DBG_UNREFERENCED_LOCAL_VARIABLE(pDirectedQueue);
  }

  //Get the queued request posted to us from the queue class
  CThreadPoolRequest request;
  BOOL bContinue = TRUE;
	while (bContinue && !m_lRequestToStop)
	{
    if (pQueue->GetRequest(request, m_nThreadIndex, INFINITE, TRUE, m_bPumpMessageQueue))
    {
	    if (request.m_dwID == CThreadPoolRequest::THREADPOOL_SHUTDOWN_REQUEST)
		    bContinue = FALSE; //break out of the loop, since a shutdown request was sent to us
	    else
        bContinue = Run(request); //Allow the virtual function to handle the client request
    }
	}
}


IMPLEMENT_DYNCREATE(CThreadPoolQueue, CObject)

CThreadPoolQueue::CThreadPoolQueue()
{
}


CThreadPoolServer::CThreadPoolServer() : m_bMaxLifetime(FALSE),
                                         m_dwMaxLifetime(0),
                                         m_pLifetimeMonitorThread(NULL),
                                         m_nLifetimeThreadIndex(0),
                                         m_pQueue(NULL)
{
}

CThreadPoolServer::~CThreadPoolServer()
{
  Stop();
}

BOOL CThreadPoolServer::Start(CRuntimeClass* pRuntimeClient, CRuntimeClass* pRuntimeQueue, int nPoolSize, int nQueueSize, BOOL bSuspended, int nPriority, UINT nStackSize)
{
  //Validate our parameters
  AFXASSUME(pRuntimeClient);
  ASSERT(pRuntimeClient->IsDerivedFrom(RUNTIME_CLASS(CThreadPoolClient))); //Your class needs to be derived from CThreadPoolClient
  ASSERT(nPoolSize); //You must have at least 1 thread in the pool
  AFXASSUME(pRuntimeQueue);
  ASSERT(pRuntimeQueue->IsDerivedFrom(RUNTIME_CLASS(CThreadPoolQueue)));

  //Stop if currently started
  Stop();

  //Try to create the queue
  ASSERT(m_pQueue == NULL);
  m_pQueue = static_cast<CThreadPoolQueue*>(pRuntimeQueue->CreateObject());
  if (m_pQueue == NULL)
  {
    TRACE(_T("CThreadPoolServer::Start, Failed to create the queue\n"));
    return FALSE;
  }

  //Serialize access to the threads array
  CSingleLock sl(&m_csThreads, TRUE);

  ASSERT(m_Threads.GetSize() == 0); //Should be empty by now

  //Create the thread pool queue
  if (!m_pQueue->Create(nQueueSize))
  {
    TRACE(_T("CThreadPoolServer::Start, Failed to create the queue, Error:%d\n"), ::GetLastError());
    return FALSE;
  }

  //Assume the best
  BOOL bSuccess = TRUE;

  //Setup the worker threads in the thread pool
  m_Threads.SetSize(0, nPoolSize);
  for (int i=0; i<nPoolSize && bSuccess; i++)
  {
    //Create the thread pool client object
    CThreadPoolClient* pClient = static_cast<CThreadPoolClient*>(pRuntimeClient->CreateObject());
    if (pClient == NULL)
    {
      TRACE(_T("CThreadPoolServer::Start, Failed to create client thread pool object for thread pool at index %d\n"), i);
      bSuccess = FALSE;
    }

    if (pClient)
    {
      //Setup its member variables
      pClient->m_pPoolServer = this;
      pClient->m_nStartupThreadPriority = nPriority;
      pClient->m_nStackSize = nStackSize;
      pClient->m_nThreadIndex = i;

      //Spin of a worker thread for it (initially suspened so that we can setup it correctly!)
      pClient->m_pWorkerThread = AfxBeginThread(CThreadPoolClient::_Run, pClient, nPriority, nStackSize, CREATE_SUSPENDED);
      if (pClient->m_pWorkerThread == NULL)
      {
        TRACE(_T("CThreadPoolServer::Start, Failed to create worker thread for thread pool at index %d\n"), i);
        delete pClient;
        bSuccess = FALSE;
      }
      else
      {
        pClient->m_pWorkerThread->m_bAutoDelete = FALSE; //We are in charge of closing the thread

        //Add it to the thread pool
        m_Threads.SetAtGrow(i, pClient);
      }
    }
  }

  //Tidy up if anything failed
  if (!bSuccess)
    Stop();
  else
  {
    //Now that everything is setup we can resume the threads in the thread pool (if need be)
    if (!bSuspended)
    {
      for (int i=0; i<m_Threads.GetSize(); i++)
      {
        CThreadPoolClient* pClient = m_Threads.GetAt(i);
        AFXASSUME(pClient);
        AFXASSUME(pClient->m_pWorkerThread);
        pClient->m_pWorkerThread->ResumeThread();
      }
    }
  }

  return bSuccess;
}

void CThreadPoolServer::Stop()
{
  //Serialize access to the threads array
  CSingleLock sl(&m_csThreads, TRUE);

  int nThreads = static_cast<int>(m_Threads.GetSize());
  if (nThreads)
  {
    //Set the m_bRequestToStop in each thread to ask them to exit if they are
    //currently processing a request in CThreadPoolClient::Run
    int i;
    for (i=0; i<nThreads; i++)
    {
      CThreadPoolClient* pClient = m_Threads.GetAt(i);
      pClient->SetRequestToStop();
    }

    //Now post enough requests to get each thread in the thread pool to stop via
    //a "special" request with an ID of THREADPOOL_SHUTDOWN_REQUEST
    for (i=0; i<nThreads; i++)
    {
      CThreadPoolRequest killRequest;
      killRequest.m_dwID = CThreadPoolRequest::THREADPOOL_SHUTDOWN_REQUEST;

      //Only use directed requests to shut down the thread pool if the pool supports
      //directed requests
      if (m_pQueue->SupportsDirectedRequests())
      {
        killRequest.m_bDirectedRequest = TRUE;
        killRequest.m_nDirectedRequestIndex = i;
      }
      ASSERT(m_pQueue);
      m_pQueue->PostRequest(killRequest);
    }

    //Wait for all the threads to exit in the thread pool
    BOOL bMoreThreads = TRUE;
    int nCurrentThreadIndex = 0;
    while (bMoreThreads)
    {
      //Wait for as many threads at once as possible
      int nCurrentThreadsToStop = min(MAXIMUM_WAIT_OBJECTS, nThreads - nCurrentThreadIndex);

      //Setup the array of threads to wait on to exit
      HANDLE hThreads[MAXIMUM_WAIT_OBJECTS];
      for (int j=0; j<nCurrentThreadsToStop; j++)
      {
        CThreadPoolClient* pClient = m_Threads.GetAt(j + nCurrentThreadIndex);
        AFXASSUME(pClient);
        AFXASSUME(pClient->m_pWorkerThread);
        hThreads[j] = pClient->m_pWorkerThread->m_hThread;
      }

      //Wait for the threads to exit
      WaitForMultipleObjects(nCurrentThreadsToStop, hThreads, TRUE, INFINITE);

      //Get ready for the next time around
      nCurrentThreadIndex += nCurrentThreadsToStop;
      bMoreThreads = (nCurrentThreadIndex < nThreads);
    }

    //Now free up all the memory associated with each thread
    for (i=0; i<nThreads; i++)
    {
      CThreadPoolClient* pClient = m_Threads.GetAt(i);
      AFXASSUME(pClient);
      delete pClient->m_pWorkerThread;
      pClient->m_pWorkerThread = NULL;
      delete pClient;
    }
    m_Threads.RemoveAll();

    //Close our queue object
    delete m_pQueue;
    m_pQueue = NULL;
  }

  //Bring down the monitoring thread if any
  SetMaxThreadClientLifetime(FALSE, 0);  
}

CThreadPoolClient* CThreadPoolServer::GetAtClient(int nIndex)
{
  return m_Threads.GetAt(nIndex);
}

BOOL CThreadPoolServer::WaitForThreadsInitInstance()
{
  //Assume the worst
  BOOL bInitOK = TRUE;

  int nThreads = static_cast<int>(m_Threads.GetSize());
  if (nThreads)
  {
    BOOL bMoreThreads = TRUE;
    int nCurrentThreadIndex = 0;
    while (bMoreThreads)
    {
      //Wait for as many threads at once as possible
      int nEventsToWaitOn = min(MAXIMUM_WAIT_OBJECTS, nThreads - nCurrentThreadIndex);

      //Setup the array of threads to wait on to exit
      HANDLE hEvents[MAXIMUM_WAIT_OBJECTS];
      int j;
      for (j=0; j<nEventsToWaitOn; j++)
      {
        CThreadPoolClient* pClient = m_Threads.GetAt(j + nCurrentThreadIndex);
        AFXASSUME(pClient);
        hEvents[j] = pClient->m_evtInitCompleted;
      }

      //Wait for the threads to to complete their InitInstance code
      WaitForMultipleObjects(nEventsToWaitOn, hEvents, TRUE, INFINITE);

      //Update the Init completed which is the logical "And" of all the InitInstances
      for (j=0; j<nEventsToWaitOn && bInitOK; j++)
      {
        CThreadPoolClient* pClient = m_Threads.GetAt(j + nCurrentThreadIndex);
        AFXASSUME(pClient);
        bInitOK &= pClient->m_lInitOK;
      }

      //Get ready for the next time around
      nCurrentThreadIndex += nEventsToWaitOn;
      bMoreThreads = (nCurrentThreadIndex < nThreads);
    }
  }
  else
    bInitOK = FALSE;

  return bInitOK;
}

BOOL CThreadPoolServer::SetMaxThreadClientLifetime(BOOL bEnableThreadLifetime, DWORD dwMinutes)
{
  //Kill the monitoring thread if currently active
  if (m_pLifetimeMonitorThread)
  {
    m_evtRequestLifetimeThread.SetEvent();
    WaitForSingleObject(m_pLifetimeMonitorThread->m_hThread, INFINITE);
    delete m_pLifetimeMonitorThread;
    m_pLifetimeMonitorThread = NULL;
  }

  //Hive away the member variables
  m_bMaxLifetime = bEnableThreadLifetime;
  m_dwMaxLifetime = dwMinutes;

  //Recreate the monitoring thread if required
  if (m_bMaxLifetime)
  {
    if (!m_pQueue->SupportsDirectedRequests())
    {
      TRACE(_T("Recyclying of threads in the thread pool is not supported because the Queue does not support directed requests\n"));
      return FALSE;
    }

    ASSERT(m_pLifetimeMonitorThread == NULL);

    m_pLifetimeMonitorThread = AfxBeginThread(CThreadPoolServer::_Monitor, this, THREAD_PRIORITY_IDLE, 0, CREATE_SUSPENDED);
    if (m_pLifetimeMonitorThread == NULL)
    {
      TRACE(_T("CThreadPoolServer::SetMaxThreadClientLifetime, Failed to create worker thread for monitoring thread in thread pool\n"));
      m_bMaxLifetime = FALSE;
      m_dwMaxLifetime = 0;
      return FALSE;
    }
    else
    {
      m_pLifetimeMonitorThread->m_bAutoDelete = FALSE; //We are in charge of closing the thread
      m_pLifetimeMonitorThread->ResumeThread();
    }
  }

  return TRUE;
}

UINT CThreadPoolServer::_Monitor(LPVOID pParam)
{
  //Validate our parameters
  AFXASSUME(pParam);

  //Convert from the SDK world to the C++ world
  CThreadPoolServer* pServer = static_cast<CThreadPoolServer*>(pParam);
  AFXASSUME(pServer);
  return pServer->Monitor();
}

UINT CThreadPoolServer::Monitor()
{
  //Work out the time interval (in ms) at which threads in the thread pool need to be recycled
  CSingleLock sl(&m_csThreads, TRUE);
  DWORD dwWaitInterval = m_dwMaxLifetime * 60000 / static_cast<DWORD>(m_Threads.GetSize()); 
  sl.Unlock();

  BOOL bWantStop = FALSE;
  while (!bWantStop)
  {
    //Check to see if the m_evtRequestLifetimeThread event is signaled
    DWORD dwWait = WaitForSingleObject(m_evtRequestLifetimeThread, dwWaitInterval);
    if (dwWait == WAIT_OBJECT_0)
      bWantStop = TRUE;
    else
      RecycleThread();
  }

  return 0L;
}

BOOL CThreadPoolServer::RecycleThread()
{
  //Serialize access to the threads array
  CSingleLock sl(&m_csThreads, TRUE);

  //Assume the best
  BOOL bSuccess = TRUE;

  CThreadPoolClient* pClient = GetAtClient(m_nLifetimeThreadIndex);

  //Get the specified thread to kill

  //Set the m_bRequestToStop to ask it to exit if they are
  //currently processing a request in CThreadPoolClient::Run
  pClient->SetRequestToStop();

   //Check to see if we need to post a directed request to the thread pool to get it to exit
  BOOL bAlreadyExited = (WaitForSingleObject(pClient->m_pWorkerThread->m_hThread, 0) == WAIT_OBJECT_0);
  if (!bAlreadyExited)
  {
    //Also post a directed request to the thread pool directly
    TRACE(_T("CThreadPoolServer::RecycleThread, Killing thread at index %d\n"), m_nLifetimeThreadIndex);
    CThreadPoolRequest killRequest;
    killRequest.m_dwID = CThreadPoolRequest::THREADPOOL_SHUTDOWN_REQUEST;
    killRequest.m_bDirectedRequest = TRUE;
    killRequest.m_nDirectedRequestIndex = m_nLifetimeThreadIndex;
    ASSERT(m_pQueue);
    BOOL bPostedOK = m_pQueue->PostRequest(killRequest); 
    bPostedOK; //To get rid of unreferrenced variable warning
    ASSERT(bPostedOK); //If this call fails then you may be using a CThreadPoolQueue derived 
                       //class which does not support directed requests, e.g. CIOCPThreadPoolQueue
  }

  //Wait for the thread to exit
  WaitForSingleObject(pClient->m_pWorkerThread->m_hThread, INFINITE);
  delete pClient->m_pWorkerThread;


  //Now recreate the thread

  //Spin of a worker thread for it (initially suspened so that we can setup it correctly!)
  pClient->m_pWorkerThread = AfxBeginThread(CThreadPoolClient::_Run, pClient, pClient->m_nStartupThreadPriority, pClient->m_nStackSize, CREATE_SUSPENDED);
  if (pClient->m_pWorkerThread == NULL)
  {
    TRACE(_T("CThreadPoolServer::RecycleThread, Failed to create worker thread for thread pool at index %d\n"), m_nLifetimeThreadIndex);
    bSuccess = FALSE;
  }
  else
  {
    pClient->m_pWorkerThread->m_bAutoDelete = FALSE; //We are in charge of closing the thread
    pClient->m_pWorkerThread->ResumeThread(); //Resume the thread now that we have set it up correctly
  }

  //increment the thread index, ready for the next call into RecyleThread at a later date
  m_nLifetimeThreadIndex = (m_nLifetimeThreadIndex + 1) % (static_cast<int>(m_Threads.GetSize()));

  return bSuccess;
}
