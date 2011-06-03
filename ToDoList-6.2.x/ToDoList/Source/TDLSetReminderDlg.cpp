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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// TDLSetReminderDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLSetReminderDlg.h"
#include "FilteredToDoCtrl.h"

#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/FileMisc.h"

#pragma warning(disable: 4201)
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLSetReminderDlg dialog

#define ID_PLAYSOUNDBTN 0xfff0

CTDLSetReminderDlg::CTDLSetReminderDlg(CWnd* pParent /*=NULL*/):
CDialog(CTDLSetReminderDlg::IDD, pParent),
m_cbAbsoluteTime(TCB_HALFHOURS)
{
	//{{AFX_DATA_INIT(CTDLSetReminderDlg)
	m_bRelativeFromDueDate = 0;
	m_sTaskTitle = _T("");
	m_dRelativeLeadIn = 0.25;
	m_sSoundFile = _T("");
	m_bRelative = TRUE;
	m_dtAbsoluteDate = COleDateTime::GetCurrentTime();
	//}}AFX_DATA_INIT
	m_dAbsoluteTime = CDateHelper::GetTimeOnly(m_dtAbsoluteDate);

	m_ePlaySound.SetFilter(CEnString(IDS_SOUNDFILEFILTER));
	m_ePlaySound.AddButton(ID_PLAYSOUNDBTN, _T(" > "), CEnString(IDS_PLAYSOUNDBTNTIP), CALC_BTNWIDTH);
	m_ePlaySound.SetButtonTip(FEBTN_BROWSE, CEnString(IDS_BROWSE));
}

void CTDLSetReminderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLSetReminderDlg)
	DDX_Control(pDX, IDC_ABSOLUTETIME, m_cbAbsoluteTime);
	DDX_Control(pDX, IDC_PLAYSOUND, m_ePlaySound);
	DDX_CBIndex(pDX, IDC_RELATIVEFROMWHERE, m_bRelativeFromDueDate);
	DDX_Text(pDX, IDC_TASKTITLE, m_sTaskTitle);
	DDX_Control(pDX, IDC_RELATIVELEADIN, m_cbLeadIn);
	DDX_CBIndex(pDX, IDC_RELATIVELEADIN, m_nLeadIn);
	DDX_Text(pDX, IDC_PLAYSOUND, m_sSoundFile);
	DDX_Radio(pDX, IDC_ABSOLUTE, m_bRelative);
	DDX_DateTimeCtrl(pDX, IDC_ABSOLUTEDATE, m_dtAbsoluteDate);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		m_dAbsoluteTime = m_cbAbsoluteTime.GetOleTime();
	}
	else
	{
		m_cbAbsoluteTime.SetOleTime(m_dAbsoluteTime);
	}
}

BEGIN_MESSAGE_MAP(CTDLSetReminderDlg, CDialog)
	//{{AFX_MSG_MAP(CTDLSetReminderDlg)
	ON_CBN_SELCHANGE(IDC_RELATIVELEADIN, OnSelchangeLeadin)
	ON_BN_CLICKED(IDC_RELATIVE, OnChangeRelative)
	ON_BN_CLICKED(IDC_ABSOLUTE, OnChangeRelative)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_EE_BTNCLICK, OnPlaySound)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLSetReminderDlg message handlers

int CTDLSetReminderDlg::DoModal(TDCREMINDER& rem)
{
	m_sTaskTitle = rem.pTDC->GetTaskTitle(rem.dwTaskID);
	m_bRelative = rem.bRelative;

	if (m_bRelative)
	{
		m_dRelativeLeadIn = rem.dRelativeDaysLeadIn * 24;
		m_bRelativeFromDueDate = (rem.nRelativeFromWhen == TDCR_DUEDATE);

		// init absolute date and time to now
		m_dtAbsoluteDate = COleDateTime::GetCurrentTime();
		m_dAbsoluteTime = CDateHelper::GetTimeOnly(m_dtAbsoluteDate);
	}
	else
	{
		m_dtAbsoluteDate = rem.dtAbsolute;
		m_dAbsoluteTime = CDateHelper::GetTimeOnly(m_dtAbsoluteDate);
	}

	m_sSoundFile = rem.sSoundFile;

	// if the sound file is empty then use last hint
	if (m_sSoundFile.IsEmpty())
	{
		m_sSoundFile = AfxGetApp()->GetProfileString(_T("Reminders"), _T("SoundFile"));

		// if that's empty too then use C:\Windows\Media\tada.wav
		if (m_sSoundFile.IsEmpty())
		{
			m_sSoundFile = FileMisc::GetWindowsFolder() + _T("\\Media\\tada.wav");
		}
	}

	int nRes = CDialog::DoModal();

	// save off last specified sound file to be a hint next time
	if (nRes == IDOK)
	{
		AfxGetApp()->WriteProfileString(_T("Reminders"), _T("SoundFile"), m_sSoundFile);

		// update reminder
		rem.sSoundFile = m_sSoundFile;
		rem.bRelative = m_bRelative;
		rem.dDaysSnooze = 0; // always

		if (m_bRelative)
		{
			rem.nRelativeFromWhen = m_bRelativeFromDueDate ? TDCR_DUEDATE : TDCR_STARTDATE;
			rem.dRelativeDaysLeadIn = m_dRelativeLeadIn / 24;
		}
		else
		{
			rem.dtAbsolute = CDateHelper::MakeDate(m_dAbsoluteTime, m_dtAbsoluteDate);
		}
	}

	return nRes;
}

BOOL CTDLSetReminderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// set item data to be time in minutes
	m_cbLeadIn.SetItemData(0, 0);  // 0 minutes
	m_cbLeadIn.SetItemData(1, 5);  // 5 minutes
	m_cbLeadIn.SetItemData(2, 10); // 10 minutes
	m_cbLeadIn.SetItemData(3, 15); // 15 minutes
	m_cbLeadIn.SetItemData(4, 20); // 20 minutes
	m_cbLeadIn.SetItemData(5, 30); // 30 minutes
	m_cbLeadIn.SetItemData(6, 45); // 45 minutes
	m_cbLeadIn.SetItemData(7, 60); // 1 hour
	m_cbLeadIn.SetItemData(8, 120); //  2 hours
	m_cbLeadIn.SetItemData(9, 180); //  3 hours
	m_cbLeadIn.SetItemData(10, 240); //  4 hours
	m_cbLeadIn.SetItemData(11, 300); //  5 hours
	m_cbLeadIn.SetItemData(12, 360); //  6 hours
	m_cbLeadIn.SetItemData(13, 720); //  12 hours
	m_cbLeadIn.SetItemData(14, 1440); //  1 day
	m_cbLeadIn.SetItemData(15, 2880); //  2 days
	m_cbLeadIn.SetItemData(16, 4320); //  3 days
	m_cbLeadIn.SetItemData(17, 5760); //  4 days
	m_cbLeadIn.SetItemData(18, 7200); //  5 days
	m_cbLeadIn.SetItemData(19, 8640); //  6 days
	m_cbLeadIn.SetItemData(20, 10080); //  1 week
	m_cbLeadIn.SetItemData(21, 20160); //  2 weeks
	m_cbLeadIn.SetItemData(22, 30240); //  3 weeks
	m_cbLeadIn.SetItemData(23, 40320); //  4 weeks

	// work out the index to select
	for (int nTime = 0; nTime < m_cbLeadIn.GetCount(); nTime++)
	{
		double dHours = m_cbLeadIn.GetItemData(nTime) / 60.0;

		if (dHours >= m_dRelativeLeadIn)
		{
			m_cbLeadIn.SetCurSel(nTime);
			m_nLeadIn = nTime;
			break;
		}
	}

	// init radio button ctrls
	GetDlgItem(IDC_ABSOLUTEDATE)->EnableWindow(!m_bRelative);
	GetDlgItem(IDC_ABSOLUTETIME)->EnableWindow(!m_bRelative);
	GetDlgItem(IDC_RELATIVEFROMWHERE)->EnableWindow(m_bRelative);
	GetDlgItem(IDC_RELATIVELEADIN)->EnableWindow(m_bRelative);

	return TRUE;
}

void CTDLSetReminderDlg::OnSelchangeLeadin()
{
	UpdateData();

	m_dRelativeLeadIn = m_cbLeadIn.GetItemData(m_nLeadIn) / 60.0; // in hours
}

LRESULT CTDLSetReminderDlg::OnPlaySound(WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDC_PLAYSOUND && lParam == ID_PLAYSOUNDBTN)
	{
		UpdateData();

		if (!m_sSoundFile.IsEmpty())
		{
			PlaySound(m_sSoundFile, NULL, SND_FILENAME);
		}

		return TRUE;
	}

	return 0;
}

void CTDLSetReminderDlg::OnChangeRelative()
{
	UpdateData();

	GetDlgItem(IDC_ABSOLUTEDATE)->EnableWindow(!m_bRelative);
	GetDlgItem(IDC_ABSOLUTETIME)->EnableWindow(!m_bRelative);
	GetDlgItem(IDC_RELATIVEFROMWHERE)->EnableWindow(m_bRelative);
	GetDlgItem(IDC_RELATIVELEADIN)->EnableWindow(m_bRelative);
}
