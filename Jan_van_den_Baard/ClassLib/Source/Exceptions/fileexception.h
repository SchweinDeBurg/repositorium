#ifndef _FILEEXCEPTION_H_
#define _FILEEXCEPTION_H_
//
// fileexception.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "exception.h"

// An exception class thrown when a IO
// error in a "ClsFile" object occures.
class ClsFileException : public ClsException
{
public:
	// Constructor sets the cause to the
	// result of the GetLastError() call.
	ClsFileException()
	{
		m_nCause = ( int )::GetLastError();
		m_sDescription.FormatMessage( m_nCause );
	}

	// Destructor. Does nothing.
	virtual ~ClsFileException()
	{;}
};

#endif // _FILEEXCEPTION_H_