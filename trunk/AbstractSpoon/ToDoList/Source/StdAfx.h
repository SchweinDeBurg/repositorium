// Copyright (C) 2003-2005 AbstractSpoon Software.
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
// - added AbstractSpoon Software copyright notice and licenese information
// - adjusted #include's paths
//*****************************************************************************

// StdAfx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BEB85A7E_10B8_410B_BC4B_56649653A624__INCLUDED_)
#define AFX_STDAFX_H__BEB85A7E_10B8_410B_BC4B_56649653A624__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Valik - WINVER needs defined for VC 7.1 will complain and define it to 0x0501 (Windows XP)
#ifndef WINVER
#define WINVER 0x0600	// Windows Vista
#endif	// WINVER

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600	// Windows Vista
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components

#include <afxext.h>         // MFC extensions
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#include <afxole.h>
#include <afxtempl.h>
#include <afxpriv.h>
#include <afxinet.h>

#pragma warning(disable:4239)
#pragma warning(disable:4505)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BEB85A7E_10B8_410B_BC4B_56649653A624__INCLUDED_)
