/*
Module : ThrdPool.h
Purpose: Interface for an MFC wrapper class for thread pools
Created: PJN / 15-04-2001

Copyright (c) 2002 - 2009 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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

#ifndef __THRDPOOL_H__
#define __THRDPOOL_H__

#ifndef THRDPOOL_EXT_CLASS
#define THRDPOOL_EXT_CLASS
#endif


/////////////////////////////// Classes ///////////////////////////////////////

//Forward declaration
class CThreadPoolServer;

//Represents an instances of a request as stored on the thread pools queue
class THRDPOOL_EXT_CLASS CThreadPoolRequest
{
public:
//Enums
  enum ThreadPoolRequestType
  {
    THREADPOOL_SHUTDOWN_REQUEST = 0,
    THREADPOOL_USER_DEFINED_REQUEST = 1
  };

//Constructors / Destructors
  CThreadPoolRequest();
  CThreadPoolRequest(const CThreadPoolRequest& request);

//methods
  CThreadPoolRequest& operator=(const CThreadPoolRequest& request);

//Member variables
  BOOL  m_bDirectedRequest;        //If this to be a directed request i.e. targetted at a specific thread
  INT_PTR m_nDirectedRequestIndex; //The index of the associated thread if this is a directed request
  void* m_pData;                   //Any opaque data you want to associate with the requested
  DWORD m_dwID;                    //Any DWORD item data you want to associate with the request, Please note that some values sent down in m_dwID, specifically 
                                   //THREADPOOL_SHUTDOWN_REQUEST as declared above are reserved for use by the thread pool framework itself. If you want to 
                                   //place you own value / enum here, then offset it from THREADPOOL_USER_DEFINED_REQUEST. i.e. use code like the following:
                                   //
                                   //CThreadPoolRequest request;
                                   //request.m_dwID = THREADPOOL_USER_DEFINED_REQUEST + dwMyValue;
                                   //if (!ThreadPool.GetQueue()->PostRequest(request))
                                   //...
                                   //
                                   //Then in your thread pool client class you would use something like the following code:
                                   //
                                   //CMyThreadPoolClient::Run(const CThreadPoolRequest& request)
                                   //{
                                   //  DWORD dwMyValue = request.m_dwID - THREADPOOL_USER_DEFINES_REQUEST;
                                   //  ...
                                   // 
                                   //If I update the REQUEST enum values in the future, I will ensure that any built in values are less than THREADPOOL_USER_DEFINED_REQUEST. 
                                   //This will ensure that client code will continue to operate as expected.
                                   //Also note that if you want to send anything more than a simple integer to the thread pool for each request, then you can use the 
                                   //m_pData member variable. The canonical use of this would be to store the address of a heap allocated struct in it, which you can then 
                                   //easily dot into in your thread pools Run method, but don't forget to delete it before you return from Run. Here's an example:
                                   //
                                   //
                                   // struct CMyStruct
                                   // {
                                   //   int nSomeValue;
                                   //   DWORD nSomeOtherValue;
                                   //   CString sStringValue;
                                   //   ...
                                   // }; 
                                   //
                                   // CMyStruct* pStruct = new CMyStruct;
                                   // pStruct->nSomeValue = something;
                                   // pStruct->nSomeOtherValue = somethingElse;
                                   // pStruct->sStringValue = someString;
                                   // CThreadPoolRequest request;
                                   // request.m_pData = pStruct;
                                   // request.m_dwID = THREADPOOL_USER_DEFINED_REQUEST;
                                   // if (!ThreadPool.GetQueue()->PostRequest(request))
                                   //   delete pStruct;
                                   // ...
                                   //Note the code above which deletes the struct if it is not delivered to the thread pool. Then in our thread pool client code we have 
                                   //the following:
                                   //
                                   //CMyThreadPoolClient::Run(const CThreadPoolRequest& request)
                                   //{
                                   //  CMyStruct* pStruct = (CMyStruct*) request->m_pData;
                                   //  ...
                                   //  use pStruct->nSomeValue, pStruct->nSomeOtherValue or pStruct->sStringValue
                                   //  ...
                                   //  //Finally now that we are finished with it, tidy up the heap memory in the request
                                   //  delete pStruct;
                                   //  ...
                                   //  return ...
                                   //}
                                   //
                                   //Please note that CThreadPoolRequest is not meant to be derived from, has no virtual destructor and should be used as described above.
};

//Class which you derive from to implement your own thread pool behaviour
class THRDPOOL_EXT_CLASS CThreadPoolClient : public CObject
{
public:
//Constructors / Destructors
  CThreadPoolClient();
  virtual ~CThreadPoolClient(); 

//Methods
  virtual void SetRequestToStop();                     //Called by the manager when CThreadPoolServer::Stop is called. You can use this or 
                                                       //the member variables m_bRequestToStop or m_RequestToStop to effect a premature exit 
                                                       //from your Run method, rather than waiting for a stop request to arrive via the 
                                                       //thread pool queue

protected:
//Virtual methods
  virtual BOOL InitInstance();                         //Similiar to the MFC function of the same name
  virtual int  ExitInstance();                         //Similiar to the MFC function of the same name
  virtual BOOL Run(const CThreadPoolRequest& request); //The function which is called to handle client requests
  virtual void Main();                                 //Implements the "main" loop

  DECLARE_DYNCREATE(CThreadPoolClient)
  
//Member variables
  CWinThread*        m_pWorkerThread;          //The actual worker thread object
  CThreadPoolServer* m_pPoolServer;            //Reference to this objects manager 
  volatile LONG      m_lRequestToStop;         //Set to TRUE by the manager when CThreadPoolServer::Stop is called. You can use this inside of
                                               //your derived Run method to effect a premature exit from it, rather than waiting for a Stop
                                               //request to arrive via the thread pool queue
  CEvent             m_evtRequestToStop;       //synchronizable version of "m_bRequestToStop"
  CEvent             m_evtInitCompleted;       //Signalled when InitInstance has completed, Use by CThreadPoolServer::WaitForThreadsInitInstance
  LONG               m_lInitOK;                //Has InitInstance completed successfully
  int                m_nStartupThreadPriority; //The thread priority which this thread started with
  UINT               m_nStackSize;             //The size of the stack this thread started with
  int                m_nThreadIndex;           //The thread index of this thread in the thread pool
  BOOL               m_bPumpMessageQueue;      //Should we pump the message queue

//The worker thread
  static UINT _Run(LPVOID pParam);             //Standard SDK style thread function
  virtual UINT _Run();                         //Non static thread function called by the static version         
};

//Base class which implements a Queue for the thread pool
class THRDPOOL_EXT_CLASS CThreadPoolQueue : public CObject
{
public:
//Constructors / Destructors
  CThreadPoolQueue();
  virtual ~CThreadPoolQueue() {};  

//Methods
  virtual BOOL Create(DWORD /*dwMaxQSize*/) { return FALSE; };
  virtual BOOL PostRequest(const CThreadPoolRequest& /*request*/, DWORD /*dwMilliseconds*/ = INFINITE, BOOL /*bLock*/ = TRUE) { return FALSE; };
  virtual BOOL PostRequestWithoutLimitCheck(const CThreadPoolRequest& /*request*/, BOOL /*bLock*/ = TRUE) { return FALSE; };
  virtual BOOL GetRequest(CThreadPoolRequest& /*request*/, int /*nThreadIndexForDirectedRequest*/, DWORD /*dwMilliseconds*/ = INFINITE, BOOL /*bLock*/ = TRUE) { return FALSE; };
  virtual BOOL IsCreated() const { return FALSE; };
  virtual BOOL SupportsDirectedRequests() const { return FALSE; };

protected:
  DECLARE_DYNCREATE(CThreadPoolQueue);
};

//The class which manages the thread pool
class THRDPOOL_EXT_CLASS CThreadPoolServer
{
public:
//Constructors / Destructors
  CThreadPoolServer();
  virtual ~CThreadPoolServer();

//Methods  
  virtual BOOL               Start(CRuntimeClass* pRuntimeClient, CRuntimeClass* pRuntimeQueue, int nPoolSize, int nQueueSize, BOOL bSuspended = FALSE, 
                                   int nPriority = THREAD_PRIORITY_NORMAL, UINT nStackSize = 0); //Starts up the thread pool
  virtual void               Stop(); //Closes down the thread pool
  virtual CThreadPoolClient* GetAtClient(int nIndex);
  virtual BOOL               WaitForThreadsInitInstance();
  BOOL                       SetMaxThreadClientLifetime(BOOL bEnableThreadLifetime, DWORD dwMinutes);
  BOOL                       GetEnabledThreadClientLifetime() const { return m_bMaxLifetime; };
  DWORD                      GetMaxThreadClientLifetime() const { return m_dwMaxLifetime; };
  int                        GetPoolSize() const { return (int) m_Threads.GetSize(); };
  CThreadPoolQueue*          GetQueue() const { return m_pQueue; };

protected:
//Methods
  static UINT _Monitor(LPVOID pParam);
  virtual UINT Monitor();
  virtual BOOL RecycleThread();

//Member variables
  CArray<CThreadPoolClient*, CThreadPoolClient*&> m_Threads;                  //The actual thread pool
  CThreadPoolQueue*                               m_pQueue;                   //The queue class instance
  CCriticalSection                                m_csThreads;                //Serializes access to "m_Threads"
  BOOL                                            m_bMaxLifetime;             //Should threads be limited to a certain lifetime
  DWORD                                           m_dwMaxLifetime;            //Lifetime of threads if m_bMaxLifetime is TRUE
  CWinThread*                                     m_pLifetimeMonitorThread;   //The thread which recycles the client threads
  CEvent                                          m_evtRequestLifetimeThread; //Event which gets set to request the lifetime monitoring thread to exit
  int                                             m_nLifetimeThreadIndex;     //The index of the next thread to be recycled
};

#endif //__THRDPOOL_H__
