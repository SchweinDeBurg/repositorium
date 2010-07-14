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
// - taken out from the original ToDoList package for better sharing
//*****************************************************************************

// MaskEdit.cpp : implementation file
//

#include "stdafx.h"
#include "MaskEdit.h"
#include "WinClasses.h"
#include "Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMaskEdit

CMaskEdit::CMaskEdit(LPCTSTR szMask, DWORD dwFlags) : m_sMask(szMask), m_dwFlags(dwFlags)
{
	if (dwFlags & ME_LOCALIZEDECIMAL)
	{
		if (m_sMask.Find('.') != -1)
			m_sMask += Misc::GetDecimalSeparator();
	}
}

CMaskEdit::~CMaskEdit()
{
}

BEGIN_MESSAGE_MAP(CMaskEdit, CEdit)
	//{{AFX_MSG_MAP(CMaskEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMaskEdit message handlers

void CMaskEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
	case VK_BACK:
		break;

	// something wierd going on with edit control accelerators
	case 3:   // c
	case 22:  // v
	case 24:  // x
		if (CWinClasses::IsEditControl(*this))
		{
			ASSERT (GetKeyState(VK_CONTROL) & 0x8000);
			break;
		}
		// else drop thru
		
	case 'c':
	case 'C':
	case 'v':
	case 'V':
	case 'x':
	case 'X':
		if (GetKeyState(VK_CONTROL) & 0x8000)
			break;
		// else drop thru
		
	default:
		if (!m_sMask.IsEmpty())
		{
			if ((m_dwFlags & ME_EXCLUDE) && m_sMask.Find((char)nChar) != -1)
				return;

			else if (!(m_dwFlags & ME_EXCLUDE) && m_sMask.Find((char)nChar) == -1)
				return;
		}
		break;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CMaskEdit::SetMask(LPCTSTR szMask, DWORD dwFlags) 
{ 
	m_sMask = szMask; 
	m_dwFlags = dwFlags; 

	if (dwFlags & ME_LOCALIZEDECIMAL)
	{
		if (m_sMask.Find('.') != -1)
			m_sMask += Misc::GetDecimalSeparator();
	}
}
