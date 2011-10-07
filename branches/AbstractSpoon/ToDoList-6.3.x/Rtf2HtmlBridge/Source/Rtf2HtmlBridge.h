// Copyright (C) 2003-2011 AbstractSpoon Software.
//
// This license applies to everything in the ToDoList package, except where
// otherwise noted.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in a
// product, an acknowledgment in the product documentation would be appreciated
// but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - added AbstractSpoon Software copyright notice and license information
// - adjusted #include's paths
//*****************************************************************************

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the RTF2HTMLBRIDGE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// RTF2HTMLBRIDGE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef RTF2HTMLBRIDGE_EXPORTS
#define RTF2HTMLBRIDGE_API __declspec(dllexport)
#else
#define RTF2HTMLBRIDGE_API __declspec(dllimport)
#endif

extern "C" RTF2HTMLBRIDGE_API int fnRtf2Html(LPCTSTR rtfFile, LPCTSTR htmlFile, LPCTSTR arg1, LPCTSTR arg2,
	LPCTSTR arg3, LPCTSTR arg4, LPCTSTR arg5, LPCTSTR arg6, LPCTSTR arg7, LPCTSTR arg8, LPCTSTR arg9);
