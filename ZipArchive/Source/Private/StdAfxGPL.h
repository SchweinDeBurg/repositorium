////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive library source distribution and
// is Copyrighted 2000 - 2012 by Artpol Software - Tadeusz Dracz
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
* \file stdafx.h
*	Contains global definitions.
*
*/

#if !defined(ZIPARCHIVE_STDAFX_DOT_H)
#define ZIPARCHIVE_STDAFX_DOT_H

// avoid C4121: alignment of a member was sensitive to packing
#pragma warning(disable: 4121)

#if defined(ZIPARCHIVE_LIB_BUILD)

// prevent automatic CRT/STL/MFC/ATL manifest dependencies generation

// see http://www.codeproject.com/KB/cpp/PrivateAssemblyProjects.aspx and
// http://blog.m-ri.de/index.php/2008/05/06/hotfix-fuer-usemsprivateassembliesh-und-vc-2008/
// by by Martin Richter for more information

#if defined(_MSC_VER) && (_MSC_VER >= 1400) && !defined(UNDER_CE)
// VC++ 2005 (8.0) or greater

#define _CRT_NOFORCE_MANIFEST
#define _STL_NOFORCE_MANIFEST
#define _AFX_NOFORCE_MANIFEST
#define _ATL_NOFORCE_MANIFEST

#ifdef __cplusplus
extern "C"
{
#endif

__declspec(selectany) int _forceCRTManifest;
__declspec(selectany) int _forceMFCManifest;
__declspec(selectany) int _forceAtlDllManifest;

// the next symbols are used by the several versions of VC++ 2008 (9.0)
__declspec(selectany) int _forceCRTManifestRTM;
__declspec(selectany) int _forceMFCManifestRTM;
__declspec(selectany) int _forceMFCManifestCUR;

#ifdef __cplusplus
}   // extern "C"
#endif

#endif   // _MSC_VER

#endif   // ZIPARCHIVE_LIB_BUILD

#include "../_features.h"
#include "../_platform.h"

#if _MSC_VER > 1000
	#pragma once
#endif

#if (_MSC_VER > 1310) && !defined(_WIN32_IE)
	#define _WIN32_IE 0x0601
#endif   // _MSC_VER

// uncomment to disable compiling standard error messages into the library
#define _ZIP_ENABLE_ERROR_DESCRIPTION

#if _MSC_VER < 1300 && !defined __BORLANDC__ && !defined (__GNUC__)
	#define ZIPINLINE inline
#else
	#define ZIPINLINE
#endif

#if defined(_ZIP_IMPL_STL)
	#include "../std_stl.h"
#elif defined(_ZIP_IMPL_ATL)
	#include "../std_atl.h"
#else
	#include "../std_mfc.h"
#endif

// force ISO/IEC 14882 conformance in for loop scope
#if _MSC_VER < 1300
#define for if(false); else for
#else
#pragma conform(forScope, on)
#endif	// _MSC_VER

#ifdef TRACE
	#if defined(UNDER_CE)
		#define ZIPTRACE(f) TRACE(_T(f),__FILE__,__LINE__)
	#elif _MSC_VER >= 1300
		#define ZIPTRACE(f) TRACE(f,__FILE__,__LINE__)
	#else
		#define ZIPTRACE(f) TRACE(_T(f))
	#endif
#else
	#define ZIPTRACE(f)
	#define NOZIPTRACE
#endif

	#ifdef _ZIP_STRICT_U16
		#define ZIP_INDEX_TYPE WORD
		#define ZIP_VOLUME_TYPE WORD		
	#else
		#define ZIP_INDEX_TYPE int
		#define ZIP_VOLUME_TYPE int
	#endif	

	#define ZIP_SIZE_TYPE DWORD
	#define ZIP_ZLIB_TYPE int

#if !defined(_INTPTR_T_DEFINED) && !defined(__GNUC__)	
	#if __BORLANDC__ > 0x551
		#include <stdint.h>
	#elif _MSC_VER <= 1200 || !defined _MSC_VER
		typedef long intptr_t;
	#endif
#endif

#ifndef UNICODE
	#ifdef _ZIP_UNICODE_NORMALIZE
		#undef _ZIP_UNICODE_NORMALIZE
	#endif
	#ifdef _ZIP_UNICODE_CUSTOM
		#undef _ZIP_UNICODE_CUSTOM
	#endif
	#if defined _ZIP_UNICODE && defined _ZIP_SYSTEM_WIN
		#undef _ZIP_UNICODE
	#endif
#endif

#define ZIP_FILE_INDEX_NOT_FOUND ZIP_INDEX_TYPE(-1)
#define ZIP_FILE_INDEX_UNSPECIFIED ZIP_FILE_INDEX_NOT_FOUND
#define ZIP_VOLUME_NUMBER_UNSPECIFIED ZIP_VOLUME_TYPE(-1)

#ifdef __MINGW32__
	#define SUFFIX_I64(v) v##LL
#else
	#define SUFFIX_I64(v) v##i64
#endif

#ifdef _ZIP_SYSTEM_WIN
	#define ZIP_SYSTEM_ERROR_TYPE DWORD
#else
	#define ZIP_SYSTEM_ERROR_TYPE int
#endif

#endif // !defined(ZIPARCHIVE_STDAFX_DOT_H)
 
