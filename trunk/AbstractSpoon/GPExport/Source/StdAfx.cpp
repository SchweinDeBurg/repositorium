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

// StdAfx.cpp : source file that includes just the standard includes
//	GPExport.pch will be the pre-compiled header
//	StdAfx.obj will contain the pre-compiled type information

#include "StdAfx.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// manifest dependencies

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

// VC-2008 RTM                       9.0.21022.8
// VC-2008 Feature Pack              9.0.30411.0
// VC-2008 SP1                       9.0.30729.1
// VC-2008 SP1 ATL Security Update   9.0.30729.4148
// VC-2008 SP1 MFC Security Update   9.0.30729.5570

#if defined(_DEBUG)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.VC90.DebugCRT' version='9.0.30729.5570' processorArchitecture='x86' publicKeyToken='1fc8b3b9a1e18e3b'\"")
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.VC90.DebugMFC' version='9.0.30729.5570' processorArchitecture='x86' publicKeyToken='1fc8b3b9a1e18e3b'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.VC90.PrivateCRT' version='9.0.30729.5570' processorArchitecture='x86' publicKeyToken='1fc8b3b9a1e18e3b'\"")
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.VC90.PrivateMFC' version='9.0.30729.5570' processorArchitecture='x86' publicKeyToken='1fc8b3b9a1e18e3b'\"")
#endif   // _DEBUG
