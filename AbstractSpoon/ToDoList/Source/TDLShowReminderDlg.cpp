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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// TDLShowReminderDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLShowReminderDlg.h"
#include "FilteredToDoCtrl.h"

#include <math.h>

#pragma warning(disable: 4201)
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLShowReminderDlg dialog

CTDLShowReminderDlg::CTDLShowReminderDlg(CWnd* pParent /*=NULL*/):
CDialog(CTDLShowReminderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTDLShowReminderDlg)
	m_sWhen = _T("");
	m_sTaskTitle = _T("");
	m_nSnoozeIndex = 0;
	//}}AFX_DATA_INIT
}

void CTDLShowReminderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLShowReminderDlg)
	DDX_Text(pDX, IDC_WHENTEXT, m_sWhen);
	DDX_Text(pDX, IDC_TASKTITLE, m_sTaskTitle);
	DDX_CBIndex(pDX, IDC_SNOOZE, m_nSnoozeIndex);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTDLShowReminderDlg, CDialog)
	//{{AFX_MSG_MAP(CTDLShowReminderDlg)
	ON_BN_CLICKED(IDSNOOZE, OnSnooze)
	ON_BN_CLICKED(IDGOTOTASK, OnGototask)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLShowReminderDlg message handlers

int CTDLShowReminderDlg::DoModal(const TDCREMINDER& rem)
{
	CString sFormat;
	COleDateTime date;

	if (rem.bRelative)
	{
		double dWhen = 0;

		if (rem.nRelativeFromWhen == TDCR_DUEDATE)
		{
			date = rem.pTDC->GetTaskDate(rem.dwTaskID, TDCD_DUE);
			dWhen = date - COleDateTime::GetCurrentTime();

			if (dWhen < 1.0)
			{
				sFormat.LoadString(IDS_DUEWHENREMINDERNOW);
			}

			else if (fabs(dWhen) < 1.0)
			{
				dWhen *= 24 * 60; // convert to minutes
				sFormat.LoadString(IDS_DUEWHENREMINDERMINS);
			}
			else
			{
				sFormat.LoadString(IDS_DUEWHENREMINDERHOURS);
			}
		}
		else
		{
			date = rem.pTDC->GetTaskDate(rem.dwTaskID, TDCD_START);
			dWhen = date - COleDateTime::GetCurrentTime();

			if (dWhen < 1.0)
			{
				sFormat.LoadString(IDS_BEGINWHENREMINDERNOW);
			}

			else if (fabs(dWhen) < 1.0)
			{
				dWhen *= 24 * 60; // convert to minutes
				sFormat.LoadString(IDS_BEGINWHENREMINDERMINS);
			}
			else
			{
				sFormat.LoadString(IDS_BEGINWHENREMINDERHOURS);
			}
		}

		CString sDateTime = CDateHelper::FormatDate(date, DHFD_DOW | DHFD_NOSEC | DHFD_TIME);

		if (dWhen < 1.0)
		{
			m_sWhen.Format(sFormat, sDateTime);
		}
		else
		{
			m_sWhen.Format(sFormat, dWhen, sDateTime);
		}
	}

	m_sTaskTitle = rem.pTDC->GetTaskTitle(rem.dwTaskID);
	m_sSoundFile = rem.sSoundFile;

	return CDialog::DoModal();
}

BOOL CTDLShowReminderDlg::OnInitDialog()
{
	BOOL bRes = CDialog::OnInitDialog();

	// do we need to play a sound?
	if (!m_sSoundFile.IsEmpty())
	{
		PlaySound(m_sSoundFile, NULL, SND_FILENAME);
	}

	return bRes;
}

void CTDLShowReminderDlg::OnSnooze()
{
	UpdateData();
	EndDialog(IDSNOOZE);
}

int CTDLShowReminderDlg::GetSnoozeMinutes() const
{
	const UINT SNOOZE[] = { 5, 10, 15, 20, 30, 45, 60, 120, 180, 240, 300, 360 };
	const UINT SIZESNOOZE = sizeof(SNOOZE) / sizeof(UINT);

	ASSERT(m_nSnoozeIndex < SIZESNOOZE);

	if (m_nSnoozeIndex >= SIZESNOOZE)
	{
		return 60;
	}
	else
	{
		return SNOOZE[m_nSnoozeIndex];
	}
}

void CTDLShowReminderDlg::OnGototask()
{
	EndDialog(IDGOTOTASK);
}
