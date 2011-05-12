// StdAfx.h : include file for standard system include files, or project
// specific include files that are used frequently, but are changed infrequently
//
/////////////////////////////////////////////////////////////////////////////
//
// This file is part of ResizableLib
// http://sourceforge.net/projects/resizablelib
//
// Copyright (C) 2000-2004 by Paolo Messina
// http://www.geocities.com/ppescher - mailto:ppescher@hotmail.com
//
// The contents of this file are subject to the Artistic License (the "License").
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.opensource.org/licenses/artistic-license.html
//
// If you find this code useful, credits would be nice!
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESIZABLESTDAFX_H__INCLUDED_)
#define AFX_RESIZABLESTDAFX_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Set max target Windows platform
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501

// Use target Common Controls version for compatibility
// with CPropertyPageEx, CPropertySheetEx
#define _WIN32_IE 0x0500

// Prevent automatic CRT/STL/MFC/ATL manifest dependencies generation

// See http://www.codeproject.com/KB/cpp/PrivateAssemblyProjects.aspx and
// http://blog.m-ri.de/index.php/2008/05/06/hotfix-fuer-usemsprivateassembliesh-und-vc-2008/
// by by Martin Richter for more information

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

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <shlwapi.h>		// DLL Version support
#if _WIN32_WINNT >= 0x0501
#include <uxtheme.h>		// Windows XP Visual Style API support
#endif

#ifndef WS_EX_LAYOUTRTL
#pragma message("Please update your Windows header files, get the latest SDK")
#pragma message("WinUser.h is out of date!")

#define WS_EX_LAYOUTRTL		0x00400000
#endif

#ifndef WC_BUTTON
#pragma message("Please update your Windows header files, get the latest SDK")
#pragma message("CommCtrl.h is out of date!")

#define WC_BUTTON			TEXT("Button")
#define WC_STATIC			TEXT("Static")
#define WC_EDIT				TEXT("Edit")
#define WC_LISTBOX			TEXT("ListBox")
#define WC_COMBOBOX			TEXT("ComboBox")
#define WC_SCROLLBAR		TEXT("ScrollBar")
#endif

#define RSZLIB_NO_XP_DOUBLE_BUFFER

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESIZABLESTDAFX_H__INCLUDED_)
