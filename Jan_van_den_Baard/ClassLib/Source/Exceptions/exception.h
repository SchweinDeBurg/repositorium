#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_
//
// exception.h
//
// (C) Copyright 2000-2002 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../strings/string.h"

// A general exception class.
class ClsException
{
public:
	// Constructor. Initializes m_nCause and m_nDescription to the
	// passed values.
	ClsException() : m_nCause( 0 ) {;}
	ClsException( int nCause, LPCTSTR pszDescription = NULL )
	{ 
		// Setup values.
		m_nCause = nCause; 
		if ( pszDescription ) m_sDescription = pszDescription;
	}

	// Destructor. Does nothing.
	virtual ~ClsException()
	{;}

	// Cause of the exception. Error codes described
	// in here are defined in the specific derived
	// exception classes or in the class throwing the
	// exception. The description string can contain
	// a descriptive error.
	int		m_nCause;
	ClsString	m_sDescription;
};

#endif // _EXCEPTION_H_