////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive library source distribution and
// is Copyrighted 2000 - 2011 by Artpol Software - Tadeusz Dracz
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// For the licensing details refer to the License.txt file.
//
// Web Site: http://www.artpol-software.com
////////////////////////////////////////////////////////////////////////////////
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - reformatted using Artistic Style 2.02 with the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --min-conditional-indent=0
//      --max-instatement-indent=2
//      --style=allman
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - implemented support for the Windows Mobile/CE tragets
// - added possibility to seamless usage in the ATL-based projects
////////////////////////////////////////////////////////////////////////////////

/**
* \file _platform.h
*	Contains definitions that determine the target compilation platform.
*
*/

#if !defined(ZIPARCHIVE_PLATFORM_DOT_H)
#define ZIPARCHIVE_PLATFORM_DOT_H

#if (_MSC_VER > 1000)
#pragma once
#endif

/************ Feel free to adjust the definitions in the following block ************/
/************************************ BLOCK START ***********************************/

//#define _ZIP_IMPL_ATL
#define _ZIP_IMPL_MFC
//#define _ZIP_SYSTEM_LINUX

#if defined(UNDER_CE)

#if !defined(_ZIP_IMPL_MFC) && !defined(_ZIP_IMPL_ATL)

#define _ZIP_IMPL_STL

#pragma warning(push, 3)
#include <windows.h>
#pragma warning(pop)
#include <altcecrt.h>

#endif   // !_ZIP_IMPL_MFC && !_ZIP_IMPL_ATL

#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcd.lib")
#if (_WIN32_WCE < 0x500)
#pragma comment(lib, "ccrtrtti.lib")
#endif   // _WIN32_WCE

#endif   // UNDER_CE

// simplified endianess detection
#ifdef __APPLE__
	#if  __BIG_ENDIAN__ == 1
		#define _ZIP_BIG_ENDIAN
	#endif
#endif

/************************************* BLOCK END ***********************************/
/********* The contents below this line are not intended for modification **********/

#if !defined(_ZIP_IMPL_MFC) && !defined(_ZIP_IMPL_ATL)
	#define _ZIP_IMPL_STL
#else
	#ifdef _ZIP_IMPL_STL
		#undef _ZIP_IMPL_STL
	#endif
#endif

#ifndef _ZIP_SYSTEM_LINUX
	#define _ZIP_SYSTEM_WIN
#else
	#ifdef _ZIP_SYSTEM_WIN
		#undef _ZIP_SYSTEM_WIN
	#endif
#endif

#if defined (_ZIP_SYSTEM_LINUX)
	#if defined (_ZIP_IMPL_MFC)
		#undef _ZIP_IMPL_MFC
		#define _ZIP_IMPL_STL
		#error Using MFC under a non-Windows platform is not supported
	#elif defined (_ZIP_IMPL_ATL)
		#undef _ZIP_IMPL_ATL
		#define _ZIP_IMPL_STL
		#error Using ATL under a non-Windows platform is not supported
	#endif   // _ZIP_IMPL_*
#endif

#endif // !defined(ZIPARCHIVE_PLATFORM_DOT_H)
