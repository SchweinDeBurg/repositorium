#ifndef _CRITICALSECTION_H_
#define _CRITICALSECTION_H_
//
// criticalsection.h
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//
#include "syncobject.h"

// CRITICAL_SECTION wrapper.
class ClsCriticalSection : public ClsSyncObject
{
	_NO_COPY( ClsCriticalSection );
public:
	// Contruction.
	ClsCriticalSection() 
	{
		// Setup the critical section.
		::InitializeCriticalSection( &m_Critical );
	}

	// Destruction.
	virtual ~ClsCriticalSection()
	{
		// Delete the critical section.
		::DeleteCriticalSection( &m_Critical );
	}

	// Lock the critical section.
	inline virtual BOOL Lock()
	{
		// Enter critical section.
		::EnterCriticalSection( &m_Critical );
		return TRUE;
	}

	// Unlock the critical section.
	inline virtual BOOL Unlock()
	{
		// Leave the critical section.
		::LeaveCriticalSection( &m_Critical );
		return TRUE;
	}

	// Operator overloads.
	inline operator LPCRITICAL_SECTION() { return &m_Critical; }

protected:
	// Data.
	CRITICAL_SECTION	m_Critical;
};
#endif // _CRITICALSECTION_H_