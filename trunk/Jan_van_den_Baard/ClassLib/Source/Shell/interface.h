#ifndef _INTERFACE_H_
#define _INTERFACE_H_
//
// interface.h
//
// (C) Copyright 2002-2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include <shlobj.h>

template <typename TYPE>
class ClsInterface
{
public:
	// Constructor.
	ClsInterface() {;}

	// Destructor, releases the interface.
	virtual ~ClsInterface() 
	{
		// Release the pointer and if we were the last
		// user reset the pointer.
		if ( s_pInterface->Release() == 0 )
			s_pInterface = NULL;
	}

	// Valid?
	inline BOOL IsValid() { return ( BOOL )( s_pInterface ? TRUE : FALSE ); }

	// Operator overloads.
	TYPE *operator->() { return s_pInterface; }
	operator TYPE*() { return s_pInterface; }

protected:
	// Data.
	static TYPE *s_pInterface;
};

// Initialize interface storage pointer.
template <typename TYPE>
TYPE *ClsInterface<TYPE>::s_pInterface = NULL;

#endif