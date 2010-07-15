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

// OffsetDatesDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "OffsetDatesDlg.h"

#include "../../Common/Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COffsetDatesDlg dialog


COffsetDatesDlg::COffsetDatesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COffsetDatesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COffsetDatesDlg)
	m_bOffsetSubtasks = FALSE;
	//}}AFX_DATA_INIT

	// restore state
	CPreferences prefs;

	m_bOffsetStartDate = prefs.GetProfileInt("OffsetDates", "StartDate", FALSE);
	m_bOffsetDueDate = prefs.GetProfileInt("OffsetDates", "DueDate", FALSE);
	m_bOffsetDoneDate = prefs.GetProfileInt("OffsetDates", "DoneDate", FALSE);
	m_bForward = prefs.GetProfileInt("OffsetDates", "Forward", 1);
	m_nOffsetBy = prefs.GetProfileInt("OffsetDates", "Amount", 1);
	m_nOffsetByUnits = prefs.GetProfileInt("OffsetDates", "AmountUnits", 0);
	m_bOffsetSubtasks = prefs.GetProfileInt("OffsetDates", "Subtasks", 1);
}


void COffsetDatesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COffsetDatesDlg)
	DDX_Check(pDX, IDC_OFFSETSTARTDATE, m_bOffsetStartDate);
	DDX_Check(pDX, IDC_OFFSETDUEDATE, m_bOffsetDueDate);
	DDX_Check(pDX, IDC_OFFSETDONEDATE, m_bOffsetDoneDate);
	DDX_CBIndex(pDX, IDC_DIRECTION, m_bForward);
	DDX_Text(pDX, IDC_BY, m_nOffsetBy);
	DDX_CBIndex(pDX, IDC_BYUNITS, m_nOffsetByUnits);
	DDX_Check(pDX, IDC_OFFSETSUBTASKS, m_bOffsetSubtasks);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COffsetDatesDlg, CDialog)
	//{{AFX_MSG_MAP(COffsetDatesDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COffsetDatesDlg message handlers

DWORD COffsetDatesDlg::GetOffsetWhat() const
{
	DWORD dwWhat = 0;

	if (m_bOffsetStartDate)
		dwWhat |= ODD_STARTDATE;

	if (m_bOffsetDueDate)
		dwWhat |= ODD_DUEDATE;

	if (m_bOffsetDoneDate)
		dwWhat |= ODD_DONEDATE;

	return dwWhat;
}

int COffsetDatesDlg::GetOffsetAmount(ODD_UNITS& nUnits) const
{
	nUnits = (ODD_UNITS)m_nOffsetByUnits;

	return (m_bForward ? m_nOffsetBy : -m_nOffsetBy);
}

void COffsetDatesDlg::OnOK()
{
	CDialog::OnOK();

	// save state
	CPreferences prefs;

	prefs.WriteProfileInt("OffsetDates", "StartDate", m_bOffsetStartDate);
	prefs.WriteProfileInt("OffsetDates", "DueDate", m_bOffsetDueDate);
	prefs.WriteProfileInt("OffsetDates", "DoneDate", m_bOffsetDoneDate);
	prefs.WriteProfileInt("OffsetDates", "Forward", m_bForward);
	prefs.WriteProfileInt("OffsetDates", "Amount", m_nOffsetBy);
	prefs.WriteProfileInt("OffsetDates", "AmountUnits", m_nOffsetByUnits);
	prefs.WriteProfileInt("OffsetDates", "Subtasks", m_bOffsetSubtasks);
}
