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
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

// TDLAttributeListBox.cpp : implementation file
//

#include "StdAfx.h"
#include "Resource.h"
#include "TDLAttribListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_INITLISTBOX (WM_APP+1)

/////////////////////////////////////////////////////////////////////////////
// CTDLAttributeListBox

CTDLAttributeListBox::CTDLAttributeListBox()
{
	// same order as enum
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_POS,           TDCA_POSITION,     TRUE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_TASKPARENTIDS, TDCA_ID,           FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_PRIORITY,      TDCA_PRIORITY,     TRUE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_PERCENT,       TDCA_PERCENT,      TRUE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_TIMEEST,       TDCA_TIMEEST,      FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_TIMESPENT,     TDCA_TIMESPENT,    FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_CREATEDBY,     TDCA_CREATEDBY,    FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_CREATEDATE,    TDCA_CREATIONDATE, FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_STARTDATE,     TDCA_STARTDATE,    FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_DUEDATE,       TDCA_DUEDATE,      TRUE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_DONEDATE,      TDCA_DONEDATE,     FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_ALLOCTO,       TDCA_ALLOCTO,      TRUE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_ALLOCBY,       TDCA_ALLOCBY,      FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_STATUS,        TDCA_STATUS,       FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_CATEGORY,      TDCA_CATEGORY,     FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_FILEREF,       TDCA_FILEREF,      FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_FLAG,          TDCA_FLAG,         FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_MODIFYDATE,    TDCA_LASTMOD,      FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_RISK,          TDCA_RISK,         FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_EXTERNALID,    TDCA_EXTERNALID,   FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_COST,          TDCA_COST,         FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_DEPENDS,       TDCA_DEPENDENCY,   FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_RECURRENCE,    TDCA_RECURRENCE,   FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_VERSION,       TDCA_VERSION,      FALSE));
	m_aAttribs.Add(ATTRIBVIS(IDS_TDLBC_COMMENTS,      TDCA_COMMENTS,     FALSE));
}

CTDLAttributeListBox::~CTDLAttributeListBox()
{
}

BEGIN_MESSAGE_MAP(CTDLAttributeListBox, CCheckListBoxEx)
	//{{AFX_MSG_MAP(CTDLAttributeListBox)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT_EX(CLBN_CHKCHANGE, OnReflectCheckChange)
	ON_MESSAGE(WM_INITLISTBOX, OnInitListBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLAttributeListBox message handlers

void CTDLAttributeListBox::PreSubclassWindow()
{
	CCheckListBoxEx::PreSubclassWindow();

	// too early to initialize the list box so we post a message
	PostMessage(WM_INITLISTBOX);
}

int CTDLAttributeListBox::FindAttribute(TDC_ATTRIBUTE nAttrib) const
{
	int nIndex = m_aAttribs.GetSize();

	while (nIndex--)
	{
		if (m_aAttribs[nIndex].nTDCAttrib == nAttrib)
		{
			return nIndex;
		}
	}

	ASSERT(0);
	return -1;
}

void CTDLAttributeListBox::SetAllAttributesVisible(BOOL bVisible)
{
	int nIndex = m_aAttribs.GetSize();

	while (nIndex--)
	{
		m_aAttribs[nIndex].bVisible = bVisible;

		if (GetSafeHwnd())
		{
			SetCheck(nIndex, bVisible ? 1 : 0);
		}
	}
}

int CTDLAttributeListBox::GetAllAttributes(CTDCAttributeArray& aAttributes) const
{
	int nIndex = m_aAttribs.GetSize();

	while (nIndex--)
	{
		TDC_ATTRIBUTE col = m_aAttribs[nIndex].nTDCAttrib;
		aAttributes.Add(col);
	}

	return aAttributes.GetSize();
}

BOOL CTDLAttributeListBox::OnReflectCheckChange()
{
	// update all check states because we don't know which one changed
	int nIndex = m_aAttribs.GetSize();

	while (nIndex--)
	{
		TDC_ATTRIBUTE col = (TDC_ATTRIBUTE)GetItemData(nIndex);
		int nAttrib = FindAttribute(col);

		if (nAttrib != -1)
		{
			m_aAttribs[nAttrib].bVisible = GetCheck(nIndex);
		}
	}

	// continue routing
	return FALSE;
}

LRESULT CTDLAttributeListBox::OnInitListBox(WPARAM /*wp*/, LPARAM /*lp*/)
{
	ASSERT(GetCount() == 0);

	for (int nIndex = 0; nIndex < (int)m_aAttribs.GetSize(); nIndex++)
	{
		const ATTRIBVIS& cs = m_aAttribs[nIndex];

		int nPos = AddString(cs.sName); // same order as enum
		SetCheck(nPos, cs.bVisible ? 1 : 0);
		SetItemData(nPos, (DWORD)cs.nTDCAttrib);
	}

	return 0L;
}

void CTDLAttributeListBox::SetVisibleAttributes(const CTDCAttributeArray& aAttributes)
{
	SetAllAttributesVisible(FALSE);

	int nAttrib = (int)aAttributes.GetSize();

	while (nAttrib--)
	{
		int nIndex = FindAttribute(aAttributes[nAttrib]);

		if (nIndex != -1)
		{
			m_aAttribs[nIndex].bVisible = TRUE;

			if (GetSafeHwnd())
			{
				SetCheck(nIndex, 1);
			}
		}
	}
}

int CTDLAttributeListBox::GetVisibleAttributes(CTDCAttributeArray& aAttributes) const
{
	aAttributes.RemoveAll();

	int nIndex = m_aAttribs.GetSize();

	while (nIndex--)
	{
		ATTRIBVIS cs = m_aAttribs[nIndex];

		if (cs.bVisible)
		{
			aAttributes.Add(cs.nTDCAttrib);
		}
	}

	return aAttributes.GetSize();
}
