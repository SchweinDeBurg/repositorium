#ifndef _SYNCOBJECT_H_
#define _SYNCOBJECT_H_
//
// syncobject.h
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//
#include "../standard.h"

// A baseclass for synchronisation classes.
class ClsSyncObject
{
	_NO_COPY( ClsSyncObject );
public:
	// Contruction.
	ClsSyncObject() : m_hHandle( NULL ) {;}

	// Destruction.
	virtual ~ClsSyncObject()
	{
		// Destroy the handle.
		if ( m_hHandle )
		{
			::CloseHandle( m_hHandle );
			m_hHandle = NULL;
		}
	}

	// Pure virtuals. Must be overidden in derived
	// classes.
	virtual BOOL Lock() = 0;
	virtual BOOL Unlock() = 0;

	// Overide when necessary.
	virtual BOOL Lock( DWORD dwTimeout = INFINITE )
	{
		// Lock the handle.
		_ASSERT_VALID( m_hHandle );
		if ( ::WaitForSingleObject( m_hHandle, dwTimeout ) == WAIT_OBJECT_0 )
			return TRUE;
		return FALSE;
	}

	// Operator overloads.
	inline operator HANDLE() const	{ return m_hHandle; }
	inline operator HANDLE()	{ return m_hHandle; }

protected:
	// Data.
	HANDLE	m_hHandle;
};
#endif // _SYNCOBJECT_H_