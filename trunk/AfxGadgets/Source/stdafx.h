// AfxGadgets library.
// Copyright (c) 2004-2008 by Elijah Zarezky,
// All rights reserved.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// stdafx.h - include file for standard system include files

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if !defined(__stdafx_h)
#define __stdafx_h

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

#if (_MSC_VER > 1310)
// use WinXP/IE6 features
#define _WIN32_WINNT 0x0501
#define _WIN32_IE 0x0601
#else
// use Win98/NT4 features
#define _WIN32_WINNT 0x0400
#endif   // _MSC_VER

#if (_MSC_VER >= 1200)
#define _WIN32_WINDOWS 0x0490
#endif	// _MSC_VER

// MFC headers
#include <afxwin.h>			// MFC core and standard components
#include <afxtempl.h>		// MFC collection template classes
#include <afxcmn.h>			// MFC common control classes
#include <afxpriv.h>       // MFC private classes

#include <../src/mfc/afximpl.h>

// ATL headers
#include <atlbase.h>			// for ATL::CComBSTR
#include <ATLComTime.h>		// for ATL::CTime and ATL::COleDateTime

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
