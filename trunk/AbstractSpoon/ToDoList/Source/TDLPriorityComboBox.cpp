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

// TDLPriorityComboBox.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLPriorityComboBox.h"
#include "TDCEnum.h"

#include "../../../CodeProject/Source/EnString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT IDS_TDC_SCALE[] = { IDS_TDC_SCALE0,
								IDS_TDC_SCALE1,
								IDS_TDC_SCALE2,
								IDS_TDC_SCALE3,
								IDS_TDC_SCALE4,
								IDS_TDC_SCALE5,
								IDS_TDC_SCALE6,
								IDS_TDC_SCALE7,
								IDS_TDC_SCALE8,
								IDS_TDC_SCALE9,
								IDS_TDC_SCALE10 };


const int TDC_NUMSCALES = sizeof(IDS_TDC_SCALE) / sizeof(UINT);

void AFXAPI DDX_CBPriority(CDataExchange* pDX, int nIDC, int& nPriority)
{
	if (pDX->m_bSaveAndValidate)
	{
		::DDX_CBIndex(pDX, nIDC, nPriority);

		if (nPriority == 0) // NONE
			nPriority = FT_NOPRIORITY;
		else
			nPriority--;
	}
	else
	{
		int nTemp = (nPriority == FT_NOPRIORITY) ? 0 : nPriority + 1;
		::DDX_CBIndex(pDX, nIDC, nTemp);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTDLPriorityComboBox

CTDLPriorityComboBox::CTDLPriorityComboBox() : m_bReverse(FALSE)
{
}

CTDLPriorityComboBox::~CTDLPriorityComboBox()
{
}


BEGIN_MESSAGE_MAP(CTDLPriorityComboBox, CColorComboBox)
	//{{AFX_MSG_MAP(CTDLPriorityComboBox)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLPriorityComboBox message handlers

int CTDLPriorityComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CColorComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	BuildCombo();
	
	return 0;
}

void CTDLPriorityComboBox::PreSubclassWindow() 
{
	CColorComboBox::PreSubclassWindow();
	
	BuildCombo();
}

int CTDLPriorityComboBox::IncrementPriority(int nAmount)
{
	int nPrevPriority = GetCurSel();
	int nPriority = nPrevPriority + nAmount;
	nPriority = max(0, min(nPriority, 11));	

	if (nPriority != nPrevPriority)
		SetCurSel(nPriority);

	return GetPriority();
}

int CTDLPriorityComboBox::GetPriority() const
{
	int nPriority = GetCurSel();
   
   if (nPriority == 0)
      nPriority = FT_NOPRIORITY;

   else if (nPriority > 0)
      nPriority--; // to take account of <none>

   return nPriority;
}

void CTDLPriorityComboBox::SetPriority(int nPriority) // -2 -> 10
{
   if (nPriority == FT_NOPRIORITY)
      nPriority = 0;

   else if (nPriority >= 0 && nPriority <= 10)
      nPriority++; // to take account of <none>

   else
      nPriority = CB_ERR;

   SetCurSel(nPriority);
}

BOOL CTDLPriorityComboBox::SetColors(const CDWordArray& aColors)
{
	if (aColors.GetSize() < 11)
		return FALSE;
	
	m_aColors.Copy(aColors);
	
	if (GetSafeHwnd())
		BuildCombo();
	
	return TRUE;
}

void CTDLPriorityComboBox::SetReverseOrder(BOOL bReverse)
{
	if (m_bReverse != bReverse)
	{
		m_bReverse = bReverse;
		
		if (GetSafeHwnd())
			BuildCombo();
	}
}

void CTDLPriorityComboBox::BuildCombo()
{
	ASSERT(GetSafeHwnd());
	
	int nSel = GetCurSel(); // so we can restore it
	
	ResetContent();
	BOOL bHasColors = m_aColors.GetSize();

	// first item is 'None' and never has a colour
	AddColor((COLORREF)-1, CEnString(IDS_TDC_NONE));
	
	for (int nLevel = 0; nLevel <= 10; nLevel++)
	{
		COLORREF color = bHasColors ? m_aColors[nLevel] : -1;
		int nPriority = m_bReverse ? 11 - nLevel : nLevel;
		
		CString sPriority;
		sPriority.Format(_T("%d (%s)"), nPriority, (LPCTSTR)CEnString(IDS_TDC_SCALE[nLevel]));
		
		AddColor(color, sPriority);
	}
	
	SetCurSel(nSel);
}
