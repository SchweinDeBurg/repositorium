////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive library source distribution and
// is Copyrighted 2000 - 2011 by Artpol Software - Tadeusz Dracz
//
// This source code is licensed as closed source and its use is 
// strictly subject to the terms and conditions of the 
// The ZipArchive Library Commercial License.
// The license is distributed with the source code in the License.txt file.
//
// Web Site: http://www.artpol-software.com
////////////////////////////////////////////////////////////////////////////////

#ifndef ZIPARCHIVE_STDAFX_DOT_H
	#error Do not include this file directly. Include stdafx.h instead
#endif

#if !defined(UNDER_CE)

#if _MSC_VER > 1000	
	#if _MSC_VER < 1500		
		#if !defined WINVER && !defined _WIN32_WINNT
			#if _MSC_VER < 1300		
				#define WINVER 0x0400
			#else
				#define WINVER 0x0501
			#endif
		#endif
	#else
		// Including this header for earlier versions of Visual Studio will cause 
		// warning messages with Platform SDK, but is safe otherwise.
		#include "sdkddkver.h"
	#endif
#pragma once
#endif

#else

#if !defined(WINVER)
#define WINVER _WIN32_WCE
#endif   // WINVER

#define _SECURE_ATL 1

// CE COM has no single threaded apartment (everything runs in the MTA).
// Hence we declare we're not concerned about thread safety issues.
#define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA

#endif   // UNDER_CE

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#ifndef VC_EXTRALEAN
	#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <atlbase.h>
#include <atlstr.h>
#include <atlfile.h>

#define TRACE ATLTRACE

typedef BOOL ZBOOL;

#if _MSC_VER >= 1300 || _ZIP_FILE_IMPLEMENTATION == ZIP_ZFI_WIN || _ZIP_FILE_IMPLEMENTATION == ZIP_ZFI_STL
	#define ZIP_FILE_USIZE ULONGLONG
	#define ZIP_FILE_SIZE LONGLONG
	#define ZIP_FILE_SIZEMAX _I64_MAX	
#else
	#define ZIP_FILE_USIZE DWORD
	#define ZIP_FILE_SIZE LONG
	#define ZIP_FILE_SIZEMAX MAXLONG		
#endif