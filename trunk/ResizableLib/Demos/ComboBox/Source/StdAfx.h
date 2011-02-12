// StdAfx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F6DCB1E4_4878_43E3_8B61_F929F62F7A6F__INCLUDED_)
#define AFX_STDAFX_H__F6DCB1E4_4878_43E3_8B61_F929F62F7A6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Set max target Windows platform
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501

// Use target Common Controls version for compatibility
// with CPropertyPageEx, CPropertySheetEx
#define _WIN32_IE 0x0500

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F6DCB1E4_4878_43E3_8B61_F929F62F7A6F__INCLUDED_)
