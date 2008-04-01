#ifndef _MENUITEMINFO_H_
#define _MENUITEMINFO_H_
//
// menuiteminfo.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../application.h"

// A simple self-initializing MENUITEMINFO
// structure class.
class ClsMenuItemInfo : public MENUITEMINFO
{
public:
	// Constructor initializes the structure.
	ClsMenuItemInfo()
	{
		// Zero structure and setup the
		// cbSize field.
		memset(( void * )this, 0, sizeof( MENUITEMINFO ));
		cbSize = sizeof( MENUITEMINFO );

		// For some reason a larger structure does not
		// work on Win95 and NT4...
#if(WINVER >= 0x0500)
		DWORD dwMajor, dwMinor, dwPlatf;
		dwMajor = ClsGetApp()->GetMajorVersion();
		dwMinor = ClsGetApp()->GetMinorVersion();
		dwPlatf = ClsGetApp()->GetPlatformID();

		// Are we running Win95, NT3.5 or NT4?
		if (( dwPlatf == VER_PLATFORM_WIN32_NT && dwMajor <= 4 ) ||
		    ( dwPlatf == VER_PLATFORM_WIN32_WINDOWS && dwMinor == 0 ))
			cbSize -= sizeof( HBITMAP );
#endif
	}

	inline operator LPMENUITEMINFO()
	{ return this; }
};

#endif // _MENUITEMINFO_H_