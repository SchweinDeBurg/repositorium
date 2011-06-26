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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

// ToDoCtrlReminders.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "ToDoCtrlReminders.h"
#include "FilteredToDoCtrl.h"
#include "../../Common/Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToDoCtrlReminders

CToDoCtrlReminders::CToDoCtrlReminders():
m_pWndNotify(NULL)
{
}

CToDoCtrlReminders::~CToDoCtrlReminders()
{
}

BEGIN_MESSAGE_MAP(CToDoCtrlReminders, CWnd)
	//{{AFX_MSG_MAP(CToDoCtrlReminders)
	ON_WM_TIMER()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToDoCtrlReminders message handlers

BOOL CToDoCtrlReminders::Initialize(CWnd* pNotify)
{
	ASSERT_VALID(pNotify);

	if (!pNotify || !pNotify->GetSafeHwnd())
	{
		return FALSE;
	}

	m_pWndNotify = pNotify;

	// create ourselves so that we can receive timer messages
	if (!Create(_T("STATIC"), _T(""), WS_CHILD, CRect(0, 0, 0, 0), pNotify, 0xffff))
	{
		m_pWndNotify = NULL;
		return FALSE;
	}

	// else
	return TRUE;
}

void CToDoCtrlReminders::AddToDoCtrl(const CFilteredToDoCtrl& tdc)
{
	LoadReminders(tdc);
}

void CToDoCtrlReminders::CloseToDoCtrl(const CFilteredToDoCtrl& tdc)
{
	SaveAndRemoveReminders(tdc);
}

void CToDoCtrlReminders::SetReminder(const TDCREMINDER& rem)
{
	ASSERT(m_pWndNotify);

	int nRem = FindReminder(rem);
	TDCREMINDER temp = rem; // to get around constness

	if (nRem != -1) // already exists
	{
		m_aReminders[nRem] = rem;   // replace
	}
	else
	{
		m_aReminders.Add(temp);
	}

	// start time if we have any reminders
	if (m_aReminders.GetSize())
	{
#ifdef _DEBUG
		SetTimer(1, 10000, NULL);
#else
		SetTimer(1, 60000, NULL); // every minute
#endif
	}
}

void CToDoCtrlReminders::RemoveReminder(const TDCREMINDER& rem)
{
	RemoveReminder(rem.dwTaskID, rem.pTDC);
}

void CToDoCtrlReminders::RemoveReminder(DWORD dwTaskID, const CFilteredToDoCtrl* pTDC)
{
	ASSERT(m_pWndNotify);

	int nRem = FindReminder(dwTaskID, pTDC);

	if (nRem != -1) // already exists
	{
		m_aReminders.RemoveAt(nRem);

		// kill time if we have no reminders
		if (m_aReminders.GetSize() == 0)
		{
			KillTimer(1);
		}
	}
}

BOOL CToDoCtrlReminders::GetReminder(int nRem, TDCREMINDER& rem) const
{
	if (nRem < 0 || nRem >= m_aReminders.GetSize())
	{
		return FALSE;
	}

	rem = m_aReminders[nRem];
	return TRUE;
}

int CToDoCtrlReminders::FindReminder(const TDCREMINDER& rem) const
{
	return FindReminder(rem.dwTaskID, rem.pTDC);
}

int CToDoCtrlReminders::FindReminder(DWORD dwTaskID, const CFilteredToDoCtrl* pTDC) const
{
	int nRem = m_aReminders.GetSize();

	while (nRem--)
	{
		TDCREMINDER rem = m_aReminders[nRem];

		if (dwTaskID == rem.dwTaskID && pTDC == rem.pTDC)
		{
			return nRem;
		}
	}

	return -1;
}

void CToDoCtrlReminders::SaveAndRemoveReminders(const CFilteredToDoCtrl& tdc)
{
	CPreferences prefs;

	// nRem is the total number of reminders for all tasklists
	// nRemCount is the number of reminders for 'tdc'
	int nRemCount = 0, nRem = m_aReminders.GetSize();
	CString sFileKey = tdc.GetPreferencesKey(_T("Reminders"));

	while (nRem--)
	{
		TDCREMINDER rem = m_aReminders[nRem];

		if (rem.pTDC == &tdc)
		{
			CString sKey;
			sKey.Format(_T("%s\\Reminder%d"), (LPCTSTR)sFileKey, nRem);

			// note: we don't save the snooze value, this gets reset each time
			prefs.WriteProfileInt(sKey, _T("TaskID"), rem.dwTaskID);
			prefs.WriteProfileInt(sKey, _T("Relative"), rem.bRelative);

			if (rem.bRelative)
			{
				prefs.WriteProfileDouble(sKey, _T("LeadIn"), rem.dRelativeDaysLeadIn * 24 * 60); // save as minutes
				prefs.WriteProfileInt(sKey, _T("FromWhen"), rem.nRelativeFromWhen);
			}
			else
			{
				prefs.WriteProfileDouble(sKey, _T("AbsoluteDate"), rem.dtAbsolute.m_dt);
			}

			prefs.WriteProfileInt(sKey, _T("Enabled"), rem.bEnabled);
			prefs.WriteProfileString(sKey, _T("SoundFile"), rem.sSoundFile);

			nRemCount++;
			m_aReminders.RemoveAt(nRem);
		}
	}

	prefs.WriteProfileInt(sFileKey, _T("NumReminders"), nRemCount);

	// kill timer if no reminders
	if (m_aReminders.GetSize() == 0)
	{
		KillTimer(1);
	}
}

void CToDoCtrlReminders::LoadReminders(const CFilteredToDoCtrl& tdc)
{
	CPreferences prefs;
	CString sFileKey = tdc.GetPreferencesKey(_T("Reminders"));
	int nRemCount = prefs.GetProfileInt(sFileKey, _T("NumReminders"));

	for (int nRem = 0; nRem < nRemCount; nRem++)
	{
		CString sKey;
		sKey.Format(_T("%s\\Reminder%d"), (LPCTSTR)sFileKey, nRem);

		TDCREMINDER rem;
		rem.dwTaskID = prefs.GetProfileInt(sKey, _T("TaskID"));
		rem.bRelative = prefs.GetProfileInt(sKey, _T("Relative"));

		if (rem.bRelative)
		{
			rem.dRelativeDaysLeadIn = prefs.GetProfileDouble(sKey, _T("LeadIn")) / (24 * 60);
			rem.nRelativeFromWhen = (TDC_REMINDER)prefs.GetProfileInt(sKey, _T("FromWhen"));
		}
		else
		{
			rem.dtAbsolute = prefs.GetProfileDouble(sKey, _T("AbsoluteDate"));
		}

		rem.bEnabled = prefs.GetProfileInt(sKey, _T("Enabled"));
		rem.sSoundFile = prefs.GetProfileString(sKey, _T("SoundFile"));
		rem.pTDC = &tdc;

		m_aReminders.Add(rem);
	}

	// start timer if some reminders
	if (m_aReminders.GetSize())
	{
#ifdef _DEBUG
		SetTimer(1, 10000, NULL);
#else
		SetTimer(1, 60000, NULL); // every minute
#endif
	}
}

void CToDoCtrlReminders::OnTimer(UINT nIDEvent)
{
	// iterate all the reminders looking for matches
	int nRem = m_aReminders.GetSize();

	while (nRem--)
	{
		TDCREMINDER rem = m_aReminders[nRem];

		// check for disabled reminders
		if (!rem.bEnabled)
		{
			continue;
		}

		// check for completed tasks
		if (rem.pTDC->IsTaskDone(rem.dwTaskID, TDCCHECKALL))
		{
			m_aReminders.RemoveAt(nRem);
			continue;
		}

		// else
		BOOL bNotify = FALSE;
		COleDateTime dateNow = COleDateTime::GetCurrentTime();

		if (rem.bRelative)
		{
			COleDateTime dateRem;

			if (rem.nRelativeFromWhen == TDCR_DUEDATE)
			{
				dateRem = rem.pTDC->GetTaskDate(rem.dwTaskID, TDCD_DUE);
			}
			else // start date
			{
				dateRem = rem.pTDC->GetTaskDate(rem.dwTaskID, TDCD_START);
			}

			bNotify = (dateRem > 0 && (dateNow.m_dt + rem.dRelativeDaysLeadIn - rem.dDaysSnooze > dateRem.m_dt));
		}
		else // absolute
		{
			bNotify = (dateNow.m_dt - rem.dDaysSnooze > rem.dtAbsolute);
		}

		if (bNotify)
		{
			ASSERT(m_pWndNotify);

			if (m_pWndNotify->SendMessage(WM_TD_REMINDER, 0, (LPARAM)&rem) == 0L)
			{
				m_aReminders[nRem] = rem;
			}
			else
			{
				m_aReminders.RemoveAt(nRem);
				rem.pTDC->RedrawReminders();
			}
		}
	}

	CWnd::OnTimer(nIDEvent);
}
