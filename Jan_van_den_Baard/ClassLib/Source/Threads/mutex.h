#ifndef _MUTEX_H_
#define _MUTEX_H_
//
// mutex.h
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//
#include "syncobject.h"

// Mutex handle wrapper.
class ClsMutex : public ClsSyncObject
{
	_NO_COPY( ClsMutex );
public:
	// Contruction.
	ClsMutex( BOOL bInitialHolder, LPCTSTR pszName = NULL, LPSECURITY_ATTRIBUTES lpAttributes = NULL ) 
	{
		// Create the handle.
		m_hHandle = ::CreateMutex( lpAttributes, bInitialHolder, pszName );
		_ASSERT_VALID( m_hHandle );
	}

	// Destruction.
	virtual ~ClsMutex()
	{
	}

	// Lock the event handle.
	inline virtual BOOL Unlock() { _ASSERT_VALID( m_hHandle ); return ::ReleaseMutex( m_hHandle ); }
	inline virtual BOOL Lock() { return FALSE; }
};
#endif // _MUTEX_H_