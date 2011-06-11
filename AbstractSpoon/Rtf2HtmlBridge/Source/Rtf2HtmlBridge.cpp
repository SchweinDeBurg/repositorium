// Copyright (C) 2003-2011 AbstractSpoon Software.
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
// - added AbstractSpoon Software copyright notice and license information
// - adjusted #include's paths
//*****************************************************************************

// Rtf2HtmlBridge.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"

#include "Rtf2HtmlBridge.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain(HANDLE /*hModule*/, DWORD /*ul_reason_for_call*/, LPVOID /*lpReserved*/)
{
	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

#using "Itenso.Rtf2Html.dll"

extern "C" RTF2HTMLBRIDGE_API int fnRtf2Html(LPCTSTR rtfFile, LPCTSTR htmlFile, LPCTSTR arg1, LPCTSTR arg2,
	LPCTSTR arg3, LPCTSTR arg4, LPCTSTR arg5, LPCTSTR arg6, LPCTSTR arg7, LPCTSTR arg8, LPCTSTR arg9)
{
	System::String^ _rtfFile = gcnew System::String(rtfFile);
	System::String^ _htmlFile = gcnew System::String(htmlFile);
	System::String^ _arg1 = gcnew System::String(arg1);
	System::String^ _arg2 = gcnew System::String(arg2);
	System::String^ _arg3 = gcnew System::String(arg3);
	System::String^ _arg4 = gcnew System::String(arg4);
	System::String^ _arg5 = gcnew System::String(arg5);
	System::String^ _arg6 = gcnew System::String(arg6);
	System::String^ _arg7 = gcnew System::String(arg7);
	System::String^ _arg8 = gcnew System::String(arg8);
	System::String^ _arg9 = gcnew System::String(arg9);

	Itenso::Solutions::Community::Rtf2Html::Program prog(_rtfFile, _htmlFile, _arg1, _arg2, _arg3, _arg4, _arg5,
		_arg6, _arg7, _arg8, _arg9);
	return (prog.Execute() ? 1 : 0);
}
