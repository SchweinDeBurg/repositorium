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
// - reformatted using Artistic Style 2.02 with the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --style=allman
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

// ContentTypeComboBox.cpp : implementation file
//

#include "StdAfx.h"
#include "ContentTypeComboBox.h"

#include "ContentMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CContentTypeComboBox

CContentTypeComboBox::CContentTypeComboBox(const CContentMgr* pContentMgr) :
m_pContentMgr(pContentMgr), m_nInitSel(0)
{
}

CContentTypeComboBox::~CContentTypeComboBox()
{
}


BEGIN_MESSAGE_MAP(CContentTypeComboBox, CComboBox)
	//{{AFX_MSG_MAP(CContentTypeComboBox)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CContentTypeComboBox message handlers

int CContentTypeComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	if (!m_pContentMgr)
	{
		return -1;
	}

	if (!GetCount())
	{
		FillCombo();
	}

	return 0;
}

void CContentTypeComboBox::FillCombo()
{
	ASSERT(CComboBox::GetCount() == 0);
	ASSERT(m_pContentMgr);

	if (m_pContentMgr && !CComboBox::GetCount())
	{
		for (int nItem = 0; nItem < m_pContentMgr->GetNumContent(); nItem++)
		{
			CString sItem = m_pContentMgr->GetContentTypeDescription(nItem);
			AddString(sItem);
		}
	}

	if (m_nInitSel < GetCount())
	{
		SetCurSel(m_nInitSel);
	}
}

void CContentTypeComboBox::PreSubclassWindow()
{
	if (!CComboBox::GetCount() && m_pContentMgr)
	{
		FillCombo();
	}

	CComboBox::PreSubclassWindow();
}

int CContentTypeComboBox::GetSelectedFormat(CONTENTFORMAT& cf) const
{
	int nSel = GetCurSel();

	if (nSel != CB_ERR)
	{
		cf = m_pContentMgr->GetContentFormat(nSel);
	}

	return nSel;
}

int CContentTypeComboBox::SetSelectedFormat(LPCTSTR szTypeID)
{
	int nSel = m_pContentMgr ? m_pContentMgr->FindContent(szTypeID) : CB_ERR;

	SetCurSel(nSel);

	return nSel;
}

int CContentTypeComboBox::GetCount() const
{
	if (m_pContentMgr)
	{
		return m_pContentMgr->GetNumContent();
	}

	// else
	return CComboBox::GetCount();
}

void CContentTypeComboBox::SetCurSel(int nSel)
{
	if (GetSafeHwnd())
	{
		CComboBox::SetCurSel(nSel);
	}
	else
	{
		m_nInitSel = nSel;
	}
}

int CContentTypeComboBox::GetCurSel() const
{
	if (GetSafeHwnd())
	{
		return CComboBox::GetCurSel();
	}

	// else
	return m_nInitSel;
}
