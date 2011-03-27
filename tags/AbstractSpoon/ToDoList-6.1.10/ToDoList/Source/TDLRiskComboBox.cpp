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
//*****************************************************************************

// TDLRiskComboBox.cpp : implementation file
//

#include "StdAfx.h"
#include "TDLRiskComboBox.h"
#include "Resource.h"
#include "TDCEnum.h"

#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/DlgUnits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT IDS_TDC_SCALE[] =
{
	IDS_TDC_SCALE0,
	IDS_TDC_SCALE1,
	IDS_TDC_SCALE2,
	IDS_TDC_SCALE3,
	IDS_TDC_SCALE4,
	IDS_TDC_SCALE5,
	IDS_TDC_SCALE6,
	IDS_TDC_SCALE7,
	IDS_TDC_SCALE8,
	IDS_TDC_SCALE9,
	IDS_TDC_SCALE10
};

const int TDC_NUMSCALES = sizeof(IDS_TDC_SCALE) / sizeof(UINT);

void AFXAPI DDX_CBRisk(CDataExchange* pDX, int nIDC, int& nRisk)
{
	if (pDX->m_bSaveAndValidate)
	{
		::DDX_CBIndex(pDX, nIDC, nRisk);

		if (nRisk == 0) // NONE
		{
			nRisk = FT_NORISK;
		}
		else
		{
			nRisk--;
		}
	}
	else
	{
		int nTemp = (nRisk == FT_NORISK) ? 0 : nRisk + 1;
		::DDX_CBIndex(pDX, nIDC, nTemp);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTDLRiskComboBox

CTDLRiskComboBox::CTDLRiskComboBox()
{
}

CTDLRiskComboBox::~CTDLRiskComboBox()
{
}

BEGIN_MESSAGE_MAP(CTDLRiskComboBox, CComboBox)
	//{{AFX_MSG_MAP(CTDLRiskComboBox)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLRiskComboBox message handlers

int CTDLRiskComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	BuildCombo();

	return 0;
}

void CTDLRiskComboBox::PreSubclassWindow()
{
	CComboBox::PreSubclassWindow();

	BuildCombo();
}

void CTDLRiskComboBox::BuildCombo()
{
	ASSERT(GetSafeHwnd());

	int nSel = GetCurSel(); // so we can restore it

	ResetContent();

	// first item is 'None' and never has a colour
	AddString(CEnString(IDS_TDC_NONE));

	for (int nLevel = 0; nLevel <= 10; nLevel++)
	{
		CString sRisk;
		sRisk.Format(_T("%d (%s)"), nLevel, CEnString(IDS_TDC_SCALE[nLevel]));

		AddString(sRisk);
	}

	SetCurSel(nSel);
}
