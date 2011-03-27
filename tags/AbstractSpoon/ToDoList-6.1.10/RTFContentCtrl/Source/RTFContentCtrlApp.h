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
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// RTFContentCtrlApp.h : main header file for the RTFCONTENTCTRL DLL
//

#if !defined(AFX_RTFCONTENTCTRL_H__2C6426B5_72CA_4C0F_9CE1_A03CD5C833C7__INCLUDED_)
#define AFX_RTFCONTENTCTRL_H__2C6426B5_72CA_4C0F_9CE1_A03CD5C833C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'StdAfx.h' before including this file for PCH
#endif

#include "Resource.h"       // main symbols

#include "../../Common/IContentControl.h"

/////////////////////////////////////////////////////////////////////////////
// CRTFContentCtrlApp
// See RTFContentCtrlApp.cpp for the implementation of this class
//

// {849CF988-79FE-418A-A40D-01FE3AFCAB2C}
static const GUID RTF_TYPEID =
{ 0x849cf988, 0x79fe, 0x418a, { 0xa4, 0xd, 0x1, 0xfe, 0x3a, 0xfc, 0xab, 0x2c } };

class CRTFContentCtrlApp : public IContent, public CWinApp
{
public:
	CRTFContentCtrlApp();

	// IContent implementation
	const char* GetTypeID() const;
	const char* GetTypeDescription() const;

	IContentControl* CreateCtrl(unsigned short nCtrlID, unsigned long nStyle,
		long nLeft, long nTop, long nWidth, long nHeight, HWND hwndParent);
	void Release();

	int ConvertToHtml(const unsigned char* pContent, int nLength,
		const TCHAR* szCharSet, char*& szHtml);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RTFCONTENTCTRL_H__2C6426B5_72CA_4C0F_9CE1_A03CD5C833C7__INCLUDED_)
