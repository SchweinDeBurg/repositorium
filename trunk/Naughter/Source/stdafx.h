// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#if !defined(AFX_STDAFX_H__483FB9A9_908F_4150_90A7_4EEAF73AD80C__INCLUDED_)
#define AFX_STDAFX_H__483FB9A9_908F_4150_90A7_4EEAF73AD80C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// unreferenced inline/local function has been removed
#pragma warning(disable: 4514)
// function not inlined
#pragma warning(disable: 4710)
// identifier was truncated in the debug information
#pragma warning(disable: 4786)

#define _SECURE_ATL 1   // use the Secure C Runtime in ATL

#define VC_EXTRALEAN   // exclude rarely-used stuff from Windows headers

#define _WIN32_WINNT 0x0501   // use Windows XP features

// MFC headers
#include <afxwin.h>		// MFC core and standard components
#include <afxtempl.h>
#include <afxext.h> 
#include <afxsock.h>
#include <afxpriv.h>
#include <afxmt.h>
#include <afxcmn.h>
#include <afxdtctl.h>

// ATL headers
#include <atlbase.h>
#include <atlenc.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atltypes.h>

// PSDK headers
#include <wininet.h>
#include <windns.h>		// DNS definitions and DNS API
#include <wincrypt.h>	// cryptographic API prototypes and definitions
#include <lm.h>
#include <shlobj.h>
#include <winsock2.h>
#include <mswsock.h>
#include <shlobj.h>

#if !defined(W3MFC_NO_ISAPI_SUPPORT)
#include <HttpExt.h>
#endif   // W3MFC_NO_ISAPI_SUPPORT
#if !defined(W3MFC_NO_SSPI_SUPPORT)
#include <sspi.h>
#endif   // W3MFC_NO_SSPI_SUPPORT

// OpenSSL headers
#include <openssl/ssl.h>
#include <openssl/err.h>

// STL headers
#pragma warning(push, 3)
#include <string>
#include <algorithm>
#pragma warning(pop)

#if !defined(_STRING_)
#define _STRING_   // for compatibility with Dinkumware STL
#endif

#if !defined(min)
using std::min;
#endif	// min

#if !defined(max)
using std::max;
#endif	// max

// CRT headers
#include <locale.h>

#pragma hdrstop

// force ISO/IEC 14882 conformance in for loop scope
#if (_MSC_VER < 1300)
#define for if (false); else for
#else
#pragma conform(forScope, on)
#endif   // _MSC_VER

#endif // !defined(AFX_STDAFX_H__483FB9A9_908F_4150_90A7_4EEAF73AD80C__INCLUDED_)
