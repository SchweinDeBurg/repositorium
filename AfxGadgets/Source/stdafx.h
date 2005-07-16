// AfxGadgets library.
// Copyright (c) 2003-2005 by Elijah Zarezky,
// All rights reserved.

// stdafx.h - include file for standard system include files

#if !defined(__stdafx_h)
#define __stdafx_h

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

// unreferenced inline/local function has been removed
#pragma warning(disable: 4514)
// function not inlined
#pragma warning(disable: 4710)
// identifier was truncated in the debug information
#pragma warning(disable: 4786)

// use Win98/NT4 features
#define _WIN32_WINNT 0x0400

#if (_MSC_VER < 1300)
#else
#define _WIN32_WINDOWS 0x0490
#endif	// _MSC_VER

// MFC headers
#include <afxwin.h>			// MFC core and standard components
#include <afxtempl.h>		// MFC collection template classes
#include <afxcmn.h>			// MFC common control classes

// RSDK headers
#include <wincrypt.h>		// cryptographic API prototypes and definitions
#include <oleauto.h>			// OLE automation support
#include <shlobj.h>
#include <shlwapi.h>			// light-weight utility APIs

// STL headers
#pragma warning(push, 3)
#include <iostream>
#include <memory>
#pragma warning(pop)

// for compatibility with Dinkumware STL
#if !defined(_IOSTREAM_)
#define _IOSTREAM_
#endif	// _IOSTREAM_
#define _FSTREAM_

// additional headers
#include "../../CodeProject/Source/PugXML.h"

// force ISO/IEC 14882 conformance in for loop scope
#if (_MSC_VER < 1300)
#define for if (false); else for
#else
#pragma conform(forScope, on)
#endif	// _MSC_VER

#endif	// __stdafx_h

// end of file
