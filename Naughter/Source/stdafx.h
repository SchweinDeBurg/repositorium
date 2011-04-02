// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#if !defined(AFX_STDAFX_H__483FB9A9_908F_4150_90A7_4EEAF73AD80C__INCLUDED_)
#define AFX_STDAFX_H__483FB9A9_908F_4150_90A7_4EEAF73AD80C__INCLUDED_

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

// use the Secure C Runtime in ATL
#define _SECURE_ATL 1
// exclude rarely-used stuff from Windows headers
#define VC_EXTRALEAN
// use Windows XP features
#define _WIN32_WINNT 0x0501

//////////////////////////////////////////////////////////////////////////////////////////////
// remove automatic CRT/MFC dependencies generation

// see http://www.codeproject.com/KB/cpp/PrivateAssemblyProjects.aspx and
// http://blog.m-ri.de/index.php/2008/05/06/hotfix-fuer-usemsprivateassembliesh-und-vc-2008/
// by by Martin Richter for more information

#define _STL_NOFORCE_MANIFEST
#define _CRT_NOFORCE_MANIFEST
#define _AFX_NOFORCE_MANIFEST
#define _ATL_NOFORCE_MANIFEST

#ifdef __cplusplus
extern "C"
{
#endif

__declspec(selectany) int _forceCRTManifest;
__declspec(selectany) int _forceMFCManifest;
__declspec(selectany) int _forceAtlDllManifest;

// the next symbols are used by the several versions of VC++ 9.0
__declspec(selectany) int _forceCRTManifestRTM;
__declspec(selectany) int _forceMFCManifestRTM;
__declspec(selectany) int _forceMFCManifestCUR;

#ifdef __cplusplus
}   // extern "C"
#endif

//////////////////////////////////////////////////////////////////////////////////////////////
// MFC headers

#include <afxwin.h>        // MFC core and standard components
#include <afxtempl.h>      // MFC collection template classes
#include <afxext.h>        // MFC advanced extensions
#include <afxpriv.h>       // MFC private classes
#include <afxmt.h>         // MFC multithreaded extensions
#include <afxcmn.h>        // MFC common control classes
#include <afxdtctl.h>      // MFC date/time control classes

//////////////////////////////////////////////////////////////////////////////////////////////
// PSDK headers

#include <wininet.h>       // Microsoft Windows Internet Extensions
#include <windns.h>        // DNS definitions and DNS API
#include <wincrypt.h>      // cryptographic API prototypes and definitions
#include <lm.h>            // LAN Manager
#include <shlobj.h>        // Windows shell COM interfaces
#include <winsock2.h>      // Windows Sockets version 2
#include <mswsock.h>       // Microsoft-specific extensions to the Windows Sockets API
#include <uxtheme.h>       // theming API header file
#include <tmschema.h>      // Theme Manager schema (properties, parts, etc)
#include <MAPI.h>          // Messaging Applications Programming Interface

#if !defined(W3MFC_NO_ISAPI_SUPPORT)
#include <HttpExt.h>
#endif   // W3MFC_NO_ISAPI_SUPPORT
#if !defined(W3MFC_NO_SSPI_SUPPORT)
#include <sspi.h>
#endif   // W3MFC_NO_SSPI_SUPPORT

//////////////////////////////////////////////////////////////////////////////////////////////
// ATL headers

#include <atlbase.h>
#include <atlenc.h>
#include <atlsocket.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atltypes.h>
#include <atlfile.h>
#include <atlimage.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// OpenSSL headers

#include <openssl/ssl.h>
#include <openssl/err.h>

//////////////////////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////////////////
// CRT headers

#include <locale.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// Scintilla headers

#include <Platform.h>
#include <Scintilla.h>
#include <SciLexer.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// some tricks

// force ISO/IEC 14882 conformance in for loop scope
#if (_MSC_VER < 1300)
#define for if (false); else for
#else
#pragma conform(forScope, on)
#endif   // _MSC_VER

#endif // !defined(AFX_STDAFX_H__483FB9A9_908F_4150_90A7_4EEAF73AD80C__INCLUDED_)
