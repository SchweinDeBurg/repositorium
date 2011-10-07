// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__25D88527_2F22_4EDF_AE89_D4EF9AB307EC__INCLUDED_)
#define AFX_STDAFX_H__25D88527_2F22_4EDF_AE89_D4EF9AB307EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////////////////////////
// unwanted warnings

// unreferenced inline/local function has been removed
#pragma warning(disable: 4514)
// function not inlined
#pragma warning(disable: 4710)
// identifier was truncated in the debug information
#pragma warning(disable: 4786)

// C4996: function or variable may be unsafe
#if (_MSC_VER > 1310)
#define _CRT_SECURE_NO_WARNINGS
#endif   // _MSC_VER

//////////////////////////////////////////////////////////////////////////////////////////////
// use WinXP/IE6 features

#define WINVER 0x0501
#define _WIN32_WINDOWS 0x0500
#define _WIN32_IE 0x0600
#define _WIN32_WINNT 0x0501

//////////////////////////////////////////////////////////////////////////////////////////////
// exclude rarely-used stuff from Windows headers

#define VC_EXTRALEAN

//////////////////////////////////////////////////////////////////////////////////////////////
// prevent automatic CRT/STL/MFC/ATL manifest dependencies generation

// see http://www.codeproject.com/KB/cpp/PrivateAssemblyProjects.aspx and
// http://blog.m-ri.de/index.php/2008/05/06/hotfix-fuer-usemsprivateassembliesh-und-vc-2008/
// by by Martin Richter for more information

#include "NoForceManifest.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// MFC headers

#if (_MSC_VER >= 1300)
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#endif	// _MSC_VER

#include <afxwin.h>
#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxext.h>
#include <afxpriv.h>
#include <afxadv.h>
#include <afxdtctl.h>
#include <afxconv.h>
#include <afxtempl.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// ATL headers

#include <atlbase.h>
#include <ATLComTime.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// PSDK headers

#include <shlobj.h>

// STL headers
#pragma warning(push, 3)
#include <utility>
#if defined(_CPPLIB_VER) && defined(_DEBUG)
#pragma warning(disable: 4702)
#endif	// _CPPLIB_VER && _DEBUG
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#if defined(_CPPLIB_VER) && defined(_DEBUG)
#pragma warning(default: 4702)
#endif	// _CPPLIB_VER && _DEBUG
#pragma warning(pop)

#if !defined(min)
using std::min;
#endif	// min

#if !defined(max)
using std::max;
#endif	// max

#pragma hdrstop

// TODO: reference additional headers your program requires here

#include "XTrace.h"

///////////////////////////////////////////////////////////////////////////////
// uncomment following line to remove support for comboboxes
//#define DO_NOT_INCLUDE_XCOMBOLIST

///////////////////////////////////////////////////////////////////////////////
// uncomment following line to remove support for tooltips
//#define NO_XLISTCTRL_TOOL_TIPS

///////////////////////////////////////////////////////////////////////////////
// comment the following line to disable TRACE statements within the 
// XListCtrl library
#define ENABLE_XLISTCTRL_TRACE

#ifdef ENABLE_XLISTCTRL_TRACE
	#define XLISTCTRL_TRACE TRACE
#else
	#define XLISTCTRL_TRACE __noop
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
// some tricks

// force ISO/IEC 14882 conformance in for loop scope
#if (_MSC_VER < 1300)
#define for if (false); else for
#else
#pragma conform(forScope, on)
#endif   // _MSC_VER

#endif // !defined(AFX_STDAFX_H__25D88527_2F22_4EDF_AE89_D4EF9AB307EC__INCLUDED_)
