// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 24/05/02 22:43 $
 * $Revision: 3 $
 * $Archive: /Projects/Libraries/CJLibrary/CJLibrary/StdAfx.h $
 *
 * $History: StdAfx.h $
 * 
 * *****************  Version 3  *****************
 * User: Andy         Date: 24/05/02   Time: 22:43
 * Updated in $/Projects/Libraries/CJLibrary/CJLibrary
 * Modifications for compatibility with Visual Studio.NET
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 10/05/02   Time: 15:54
 * Updated in $/Projects/Libraries/CJLibrary/CJLibrary
 * Changes for compatibility with VC 7.0
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/CJLibrary
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:41p
 * Updated in $/CodeJock/CJLibrary
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_STDAFX_H__F235A078_2921_11D3_8733_000000000000__INCLUDED_)
#define AFX_STDAFX_H__F235A078_2921_11D3_8733_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifdef _UNICODE
	#ifndef UNICODE
		#define UNICODE
	#endif
#endif

// C4996: function or variable may be unsafe
#if (_MSC_VER > 1310)
#define _CRT_SECURE_NO_WARNINGS
#endif   // _MSC_VER

#if (_MSC_VER > 1310)
// use WinXP/IE6 features
#define _WIN32_WINNT 0x0501
#define _WIN32_IE 0x0601
#else
// use Win2000 features
#define _WIN32_WINNT 0x0500
#endif   // _MSC_VER

#if (_MSC_VER >= 1200)
#define _WIN32_WINDOWS 0x0490
#endif	// _MSC_VER

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#if _MSC_VER < 1200
#define _VC_VERSION_5
#endif  // _VC_VERSION_5

#ifndef _CJXLIB_INLINE
#define _CJXLIB_INLINE inline
#endif // _AFXCMN_INLINE

#ifndef _CJX_EXT_CLASS
#ifdef _LIB
#define _CJX_EXT_CLASS
#else
#define _CJX_EXT_CLASS AFX_EXT_CLASS
#endif // _LIB
#endif // _CJX_EXT_CLASS

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif

#ifndef __AFXPRIV_H__
#include <afxpriv.h>
#endif

#if _MSC_VER >= 1300
	#include <..\src\mfc\afximpl.h>		// VC.NET
#else
	#include <..\src\afximpl.h>			// VC 5/6
#endif


#define _delete(p){if(p){delete p;p=NULL;}}
#define _destroyIcon(h){if(h){::DestroyIcon(h);h=NULL;}}

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F235A078_2921_11D3_8733_000000000000__INCLUDED_)
