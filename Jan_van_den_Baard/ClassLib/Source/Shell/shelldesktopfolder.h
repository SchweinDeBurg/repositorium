#ifndef _SHELLDESKTOPFOLDER_H_
#define _SHELLDESKTOPFOLDER_H_
//
// shelldesktopfolder.h
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//

#include "interface.h"
#include "../exceptions/shellexception.h"
#include <shlobj.h>

// Simple wrapper for the shell IShellFolder
// interface.
class ClsShellDesktopFolder : public ClsInterface<IShellFolder>
{
public:
	// Constructor.
	ClsShellDesktopFolder()
	{
		// Get the interface if it does not yet exists.
		// Otherwise add a reference to it.
		if ( s_pInterface == NULL ) 
		{
			if ( ::SHGetDesktopFolder( &s_pInterface ) != NOERROR )
				throw( ClsShellException());
		}
		else 			    
			s_pInterface->AddRef();
	}

	// Destructor.
	virtual ~ClsShellDesktopFolder() {;}
};
#endif