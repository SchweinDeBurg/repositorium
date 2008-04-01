#ifndef _SHELLEXCEPTION_H_
#define _SHELLEXCEPTION_H_
//
// shellexception.h
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//

#include "exception.h"

// An exception class thrown by the shell wrapper
// classes.
class ClsShellException : public ClsException
{
public:
	// Constructor. Set's up the cause of the
	// exception..
	ClsShellException()
	{
		// Setup the only possible cause.
		m_nCause = ::GetLastError();
		m_sDescription.FormatMessage( m_nCause );
	}

	// Destructor. Does nothing.
	virtual ~ClsShellException()
	{;}
};

#endif // _SHELLEXCEPTION_H_