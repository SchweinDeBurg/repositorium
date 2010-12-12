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

// TaskSelectionDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TaskSelectionDlg.h"

#include "../../Common/Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskSelectionDlg dialog

CTaskSelectionDlg::CTaskSelectionDlg(LPCTSTR szRegKey, BOOL bShowSubtaskCheckbox, BOOL bVisibleColumnsOnly) :
CDialog(),
m_sRegKey(szRegKey),
m_bShowSubtaskCheckbox(bShowSubtaskCheckbox)
{
	//{{AFX_DATA_INIT(CTaskSelectionDlg)
	//}}AFX_DATA_INIT
	CPreferences prefs;

	m_bCompletedTasks = prefs.GetProfileInt(m_sRegKey, _T("CompletedTasks"), TRUE);
	m_bIncompleteTasks = prefs.GetProfileInt(m_sRegKey, _T("IncompleteTasks"), TRUE);
	m_nWhatTasks = prefs.GetProfileInt(m_sRegKey, _T("WhatTasks"), TSDT_FILTERED);
	m_bSelectedSubtasks = prefs.GetProfileInt(m_sRegKey, _T("SelectedSubtasks"), TRUE);

	if (bVisibleColumnsOnly)
	{
		m_nAttribOption = TSDA_VISIBLE;
	}
	else
	{
		m_nAttribOption = prefs.GetProfileInt(m_sRegKey, _T("AttributeOption"), TSDA_ALL);
	}

	CTDCAttributeArray aAttrib;
	CString sGroup = m_sRegKey + _T("\\AttribVisibility");
	int nAttrib = prefs.GetProfileInt(sGroup, _T("Count"), 0);

	while (nAttrib--)
	{
		CString sKey;
		sKey.Format(_T("att%d"), nAttrib);

		TDC_ATTRIBUTE att = (TDC_ATTRIBUTE)prefs.GetProfileInt(sGroup, sKey, 0);
		aAttrib.Add(att);
	}

	m_lbAttribList.SetVisibleAttributes(aAttrib);
}

void CTaskSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskSelectionDlg)
	DDX_Control(pDX, IDC_CUSTOMATTRIBLIST, m_lbAttribList);
	DDX_Radio(pDX, IDC_ALLATTRIB, m_nAttribOption);
	DDX_Check(pDX, IDC_INCLUDESUBTASKS, m_bSelectedSubtasks);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_INCLUDEDONE, m_bCompletedTasks);
	DDX_Check(pDX, IDC_INCLUDENOTDONE, m_bIncompleteTasks);
	DDX_Radio(pDX, IDC_ALLTASKS, m_nWhatTasks);
}

BEGIN_MESSAGE_MAP(CTaskSelectionDlg, CDialog)
	//{{AFX_MSG_MAP(CTaskSelectionDlg)
	ON_BN_CLICKED(IDC_ALLATTRIB, OnChangeAttribOption)
	ON_BN_CLICKED(IDC_CUSTOMATTRIB, OnChangeAttribOption)
	ON_BN_CLICKED(IDC_VISIBLEATTRIB, OnChangeAttribOption)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ALLTASKS, OnChangetasksOption)
	ON_BN_CLICKED(IDC_FILTERTASKS, OnChangetasksOption)
	ON_BN_CLICKED(IDC_INCLUDEDONE, OnIncludeDone)
	ON_BN_CLICKED(IDC_SELTASK, OnChangetasksOption)
	ON_BN_CLICKED(IDC_INCLUDENOTDONE, OnIncludeNotDone)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskSelectionDlg message handlers

BOOL CTaskSelectionDlg::Create(UINT nIDRefFrame, CWnd* pParent, UINT nID)
{
	ASSERT(nIDRefFrame && pParent);

	if (CDialog::Create(IDD_TASKSELECTION_DIALOG, pParent))
	{
		if (nID != IDC_STATIC)
		{
			SetDlgCtrlID(nID);
		}

		CWnd* pFrame = pParent->GetDlgItem(nIDRefFrame);

		if (pFrame)
		{
			CRect rFrame;
			pFrame->GetWindowRect(rFrame);
			pParent->ScreenToClient(rFrame);

			MoveWindow(rFrame);

			// insert ourselves after this item in the Z-order
			SetWindowPos(pFrame, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

			// and then hide it
			pFrame->ShowWindow(SW_HIDE);
		}

		return TRUE;
	}

	return FALSE;
}

void CTaskSelectionDlg::OnDestroy()
{
	UpdateData();

	CDialog::OnDestroy();

	// save settings
	CPreferences prefs;

	prefs.WriteProfileInt(m_sRegKey, _T("CompletedTasks"), m_bCompletedTasks);
	prefs.WriteProfileInt(m_sRegKey, _T("IncompleteTasks"), m_bIncompleteTasks);
	prefs.WriteProfileInt(m_sRegKey, _T("SelectedTasks"), m_nWhatTasks);
	prefs.WriteProfileInt(m_sRegKey, _T("SelectedSubtasks"), m_bSelectedSubtasks);

	prefs.WriteProfileInt(m_sRegKey, _T("AttributeOption"), m_nAttribOption);

	CTDCAttributeArray aAttrib;
	int nAttrib = m_lbAttribList.GetVisibleAttributes(aAttrib);
	CString sGroup = m_sRegKey + _T("\\AttribVisibility");

	prefs.WriteProfileInt(sGroup, _T("Count"), nAttrib);

	while (nAttrib--)
	{
		CString sKey;
		sKey.Format(_T("att%d"), nAttrib);

		prefs.WriteProfileInt(sGroup, sKey, aAttrib[nAttrib]);
	}
}

void CTaskSelectionDlg::OnChangetasksOption()
{
	UpdateData();

	GetDlgItem(IDC_INCLUDESUBTASKS)->EnableWindow(GetWantSelectedTasks());

	GetParent()->SendMessage(WM_TASKSELDLG_CHANGE);
}

void CTaskSelectionDlg::OnIncludeDone()
{
	UpdateData();

	// prevent the user unchecking both tick boxes
	if (!m_bCompletedTasks && !m_bIncompleteTasks)
	{
		m_bIncompleteTasks = TRUE;
		UpdateData(FALSE);
	}

	GetParent()->SendMessage(WM_TASKSELDLG_CHANGE);
}
void CTaskSelectionDlg::OnIncludeNotDone()
{
	UpdateData();

	// prevent the user unchecking both tick boxes
	if (!m_bCompletedTasks && !m_bIncompleteTasks)
	{
		m_bCompletedTasks = TRUE;
		UpdateData(FALSE);
	}

	GetParent()->SendMessage(WM_TASKSELDLG_CHANGE);
}

BOOL CTaskSelectionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_INCLUDESUBTASKS)->EnableWindow(GetWantSelectedTasks());
	GetDlgItem(IDC_CUSTOMATTRIBLIST)->EnableWindow(m_nAttribOption == TSDA_CUSTOM);

	if (!m_bShowSubtaskCheckbox)
	{
		GetDlgItem(IDC_INCLUDESUBTASKS)->ShowWindow(SW_HIDE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTaskSelectionDlg::SetWantWhatTasks(TSD_TASKS nWhat)
{
	m_nWhatTasks = nWhat;
	UpdateData(FALSE);

	GetDlgItem(IDC_INCLUDEDONE)->EnableWindow(!GetWantSelectedTasks());
	GetDlgItem(IDC_INCLUDENOTDONE)->EnableWindow(!GetWantSelectedTasks());

	if (m_bShowSubtaskCheckbox)
	{
		GetDlgItem(IDC_INCLUDESUBTASKS)->EnableWindow(GetWantSelectedTasks());
	}
}

void CTaskSelectionDlg::SetWantCompletedTasks(BOOL bWant)
{
	// prevent the user unchecking both tick boxes
	if (!bWant && !m_bIncompleteTasks)
	{
		m_bIncompleteTasks = TRUE;
	}

	m_bCompletedTasks = bWant;
	UpdateData(FALSE);
}

void CTaskSelectionDlg::SetWantInCompleteTasks(BOOL bWant)
{
	// prevent the user unchecking both tick boxes
	if (!bWant && !m_bCompletedTasks)
	{
		m_bCompletedTasks = TRUE;
	}

	m_bIncompleteTasks = bWant;
	UpdateData(FALSE);
}

void CTaskSelectionDlg::OnChangeAttribOption()
{
	UpdateData();

	GetDlgItem(IDC_CUSTOMATTRIBLIST)->EnableWindow(m_nAttribOption == TSDA_CUSTOM);
}

int CTaskSelectionDlg::GetCustomAttributes(CTDCAttributeArray& aAttrib) const
{
	aAttrib.RemoveAll();

	if (m_nAttribOption == TSDA_CUSTOM)
	{
		m_lbAttribList.GetVisibleAttributes(aAttrib);
	}
	else
	{
		ASSERT(0);
	}

	return aAttrib.GetSize();
}
