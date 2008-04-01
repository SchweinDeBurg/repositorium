#ifndef _SHELLMALLOC_H_
#define _SHELLMALLOC_H_
//
// shellmalloc.h
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//

#include "interface.h"
#include "../exceptions/shellexception.h"
#include <shlobj.h>

// Simple wrapper for the shell IMalloc
// interface.
class ClsShellMalloc : public ClsInterface<IMalloc>
{
public:
	// Constructor.
	ClsShellMalloc()
	{
		// Get the interface if it does not yet exists.
		// Otherwise add a reference to it.
		if ( s_pInterface == NULL ) 
		{
			if ( ::SHGetMalloc( &s_pInterface ) != NOERROR )
				throw( ClsShellException());
		}
		else 			    
			s_pInterface->AddRef();
	}

	// Destructor.
	virtual ~ClsShellMalloc() {;}
};
#endif