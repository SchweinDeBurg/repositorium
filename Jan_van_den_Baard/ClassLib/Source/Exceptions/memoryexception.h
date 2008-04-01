#ifndef _MEMORYEXCEPTION_H_
#define _MEMORYEXCEPTION_H_
//
// memoryexception.h
//
// (C) Copyright 2000-2002 Jan van den Baard.
//     All Rights Reserved.
//

#include "exception.h"

// An exception class thrown when out of
// memory conditions occur using new and new[].
class ClsMemoryException : public ClsException
{
public:
	// Constructor. Set's up the cause of the
	// exception..
	ClsMemoryException()
	{
		// Setup the only possible cause.
		m_nCause = ERROR_NOT_ENOUGH_MEMORY;
		m_sDescription.FormatMessage( ERROR_NOT_ENOUGH_MEMORY );
	}

	// Destructor. Does nothing.
	virtual ~ClsMemoryException()
	{;}
};

#endif // _MEMORYEXCEPTION_H_