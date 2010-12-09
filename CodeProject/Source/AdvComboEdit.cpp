/////////////////////////////////////////////////////////////////////////////
// AdvComboEdit.cpp : implementation file
//
// CAdvComboEdit Control
// Version: 2.1
// Date: September 2002
// Author: Mathias Tunared
// Email: Mathias@inorbit.com
// Copyright (c) 2002. All Rights Reserved.
//
// This code, in compiled form or as source code, may be redistributed
// unmodified PROVIDING it is not sold for profit without the authors
// written consent, and providing that this notice and the authors name
// and all copyright notices remains intact.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AdvComboEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvComboEdit

BEGIN_MESSAGE_MAP(CAdvComboEdit, CEdit)
	//{{AFX_MSG_MAP(CAdvComboEdit)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CAdvComboEdit::CAdvComboEdit()
{
	XLISTCTRL_TRACE(_T("in CAdvComboEdit::CAdvComboEdit\n"));
}

CAdvComboEdit::~CAdvComboEdit()
{
	XLISTCTRL_TRACE(_T("in CAdvComboEdit::~CAdvComboEdit\n"));
}

/////////////////////////////////////////////////////////////////////////////
// CAdvComboEdit message handlers

void CAdvComboEdit::OnTimer(UINT nIDEvent)
{
	KillTimer(nIDEvent);
	SetForegroundWindow();

	CEdit::OnTimer(nIDEvent);
}

void CAdvComboEdit::PreSubclassWindow()
{
	XLISTCTRL_TRACE(_T("in CAdvComboEdit::PreSubclassWindow\n"));
	SetTimer(1, 100, NULL);
	CEdit::PreSubclassWindow();
}
