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

// EnColorDialog.cpp : implementation file
//

#include "StdAfx.h"
#include "EnColorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnColorDialog

IMPLEMENT_DYNAMIC(CEnColorDialog, CColorDialog)

CEnColorDialog::CEnColorDialog(COLORREF clrInit, DWORD dwFlags, CWnd* pParentWnd) :
CColorDialog(clrInit, dwFlags, pParentWnd)
{
	// restore previously saved custom colors
	for (int nColor = 0; nColor < 16; nColor++)
	{
		CString sKey;
		sKey.Format("CustomColor%d", nColor);

		COLORREF color = (COLORREF)AfxGetApp()->GetProfileInt("ColorDialog", sKey, (int)RGB(255, 255, 255));
		m_cc.lpCustColors[nColor] = color;
	}
}

CEnColorDialog::~CEnColorDialog()
{
	// save any custom colors
	COLORREF* pColors = GetSavedCustomColors();

	for (int nColor = 0; nColor < 16; nColor++)
	{
		CString sKey;
		sKey.Format("CustomColor%d", nColor);

		int nColorVal = (int)pColors[nColor];
		AfxGetApp()->WriteProfileInt("ColorDialog", sKey, nColorVal);
	}
}

BEGIN_MESSAGE_MAP(CEnColorDialog, CColorDialog)
	//{{AFX_MSG_MAP(CEnColorDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
