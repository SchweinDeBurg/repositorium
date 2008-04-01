#ifndef _EVENT_H_
#define _EVENT_H_
//
// event.h
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//
#include "syncobject.h"

// Event handle wrapper.
class ClsEvent : public ClsSyncObject
{
	_NO_COPY( ClsEvent );
public:
	// Contruction.
	ClsEvent( BOOL bManualReset, BOOL bInitialState, LPCTSTR pszName = NULL, LPSECURITY_ATTRIBUTES lpAttributes = NULL ) 
	{
		// Create the handle.
		m_hHandle = ::CreateEvent( lpAttributes, bManualReset, bInitialState, pszName );
		_ASSERT_VALID( m_hHandle );
	}

	// Destruction.
	virtual ~ClsEvent()
	{
	}

	// Lock the event handle.
	inline virtual BOOL Unlock() { return TRUE; }
	inline virtual BOOL Lock() { return TRUE; }
	inline virtual BOOL Lock( DWORD dwTimeout = INFINITE )
	{
		// Lock the event.
		_ASSERT_VALID( m_hHandle );
		if ( ::WaitForSingleObject( m_hHandle, dwTimeout ) == WAIT_OBJECT_0 )
			return TRUE;
		return FALSE;
	}

	// Event functions.
	inline BOOL SetEvent()   { _ASSERT_VALID( m_hHandle ); return ::SetEvent(   m_hHandle ); }
	inline BOOL ResetEvent() { _ASSERT_VALID( m_hHandle ); return ::ResetEvent( m_hHandle ); }
	inline BOOL PulseEvent() { _ASSERT_VALID( m_hHandle ); return ::PulseEvent( m_hHandle ); }
};
#endif // _EVENT_H_