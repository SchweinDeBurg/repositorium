#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_
//
// semaphore.h
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//
#include "syncobject.h"

// Semaphore handle wrapper.
class ClsSemaphore : public ClsSyncObject
{
	_NO_COPY( ClsSemaphore );
public:
	// Contruction.
	ClsSemaphore( LONG lInitialCount = 1, LONG lMaximumCount = 1, LPCTSTR pszName = NULL, LPSECURITY_ATTRIBUTES lpAttributes = NULL ) 
	{
		// Create the handle.
		m_hHandle = ::CreateSemaphore( lpAttributes, lInitialCount, lMaximumCount, pszName );
		_ASSERT_VALID( m_hHandle );
	}

	// Destruction.
	virtual ~ClsSemaphore()
	{
	}

	// Lock the event handle.
	inline virtual BOOL Unlock() { _ASSERT_VALID( m_hHandle ); return ::ReleaseSemaphore( m_hHandle, 1, NULL ); }
	inline virtual BOOL Lock() { return FALSE; }
};
#endif // _SEMAPHORE_H_