/*
Module : IOCPThreadPoolQueue.h
Purpose: Interface for an MFC class which implements a queue class for CThreadPoolServer using an SDK IO Completion port
Created: PJN / 18-08-2002

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

#ifndef __IOCPTHREADPOOLQUEUE_H__
#define __IOCPTHREADPOOLQUEUE_H__

#ifndef THRDPOOL_EXT_CLASS
#define THRDPOOL_EXT_CLASS
#endif


///////////////////// Includes ////////////////////////////////////////////////

#include "ThrdPool.h"


///////////////////// Classes /////////////////////////////////////////////////

//Class which implements a queue for CThreadPoolServer using an IO Completion Port
class THRDPOOL_EXT_CLASS CIOCPThreadPoolQueue : public CThreadPoolQueue
{
public:
//Constructors / Destructors
  CIOCPThreadPoolQueue();
  virtual ~CIOCPThreadPoolQueue(); 

//Methods
  virtual BOOL Create(DWORD dwMaxQSize);
  virtual BOOL Close();
  virtual BOOL PostRequest(const CThreadPoolRequest& request, DWORD dwMilliseconds = INFINITE, BOOL bLock = TRUE);
  virtual BOOL PostRequestWithoutLimitCheck(const CThreadPoolRequest& request, BOOL bLock = TRUE) { return PostRequest(request, 0, bLock); };
  virtual BOOL GetRequest(CThreadPoolRequest& request, int nThreadIndexForDirectedRequest, DWORD dwMilliseconds, BOOL bLock, BOOL bPumpMessage);
  virtual BOOL IsCreated() const;
  virtual BOOL SupportsDirectedRequests() const { return FALSE; };

protected:
  DECLARE_DYNCREATE(CIOCPThreadPoolQueue)

//Member variables
  HANDLE m_hIOCP;   //Handle to the SDK IOCP
};

#endif //__IOCPTHREADPOOLQUEUE_H__
