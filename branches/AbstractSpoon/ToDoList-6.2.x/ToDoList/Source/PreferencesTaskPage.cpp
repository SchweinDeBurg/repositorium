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

// PreferencesTaskPage.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "PreferencesTaskPage.h"

#include "../../../CodeProject/Source/Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskPage property page

IMPLEMENT_DYNCREATE(CPreferencesTaskPage, CPreferencesPageBase)

CPreferencesTaskPage::CPreferencesTaskPage() :
CPreferencesPageBase(CPreferencesTaskPage::IDD)
{
	//{{AFX_DATA_INIT(CPreferencesTaskPage)
	//}}AFX_DATA_INIT
}

CPreferencesTaskPage::~CPreferencesTaskPage()
{
}

void CPreferencesTaskPage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesTaskPage)
	DDX_CBString(pDX, IDC_DAYSINONEWEEK, m_sDaysInWeek);
	DDX_CBString(pDX, IDC_HOURSINONEDAY, m_sHoursInDay);
	DDX_Check(pDX, IDC_LOGTIME, m_bLogTime);
	DDX_Check(pDX, IDC_LOGTASKSEPARATELY, m_bLogTasksSeparately);
	DDX_Check(pDX, IDC_EXCLUSIVETRACKING, m_bExclusiveTimeTracking);
	DDX_Check(pDX, IDC_ALLOWPARENTRACKING, m_bAllowParentTimeTracking);
	DDX_Check(pDX, IDC_TRACKNONSELECTEDTASKS, m_bTrackNonSelectedTasks);
	DDX_Check(pDX, IDC_TRACKSCREENSAVER, m_bTrackOnScreenSaver);
	DDX_Check(pDX, IDC_TRACKNONACTIVETASKLISTS, m_bTrackNonActiveTasklists);
	DDX_Check(pDX, IDC_TRACKHIBERNATED, m_bTrackHibernated);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPreferencesTaskPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesTaskPage)
	ON_BN_CLICKED(IDC_LOGTIME, OnLogtime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskPage message handlers

BOOL CPreferencesTaskPage::OnInitDialog()
{
	CPreferencesPageBase::OnInitDialog();

	m_mgrGroupLines.AddGroupLine(IDC_TRACKGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_TIMEGROUP, *this);

	GetDlgItem(IDC_LOGTASKSEPARATELY)->EnableWindow(m_bLogTime);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

double CPreferencesTaskPage::GetHoursInOneDay() const
{
	double dHours = Misc::Atof(m_sHoursInDay);

	if (dHours <= 0 || dHours > 24)
	{
		dHours = 8;
	}

	return dHours;
}

double CPreferencesTaskPage::GetDaysInOneWeek() const
{
	double dDays = Misc::Atof(m_sDaysInWeek);

	if (dDays <= 0 || dDays > 7)
	{
		dDays = 5;
	}

	return dDays;
}

void CPreferencesTaskPage::OnLogtime()
{
	UpdateData();

	GetDlgItem(IDC_LOGTASKSEPARATELY)->EnableWindow(m_bLogTime);

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesTaskPage::LoadPreferences(const CPreferences& prefs)
{
	// load settings
	m_sDaysInWeek = prefs.GetProfileString(_T("Preferences"), _T("DaysInWeek"), _T("5.00"));
	m_sHoursInDay = prefs.GetProfileString(_T("Preferences"), _T("HoursInDay"), _T("8.00"));
	m_bLogTime = prefs.GetProfileInt(_T("Preferences"), _T("LogTime"), TRUE);
	m_bLogTasksSeparately = prefs.GetProfileInt(_T("Preferences"), _T("LogTasksSeparately"), FALSE);
	m_bExclusiveTimeTracking = prefs.GetProfileInt(_T("Preferences"), _T("ExclusiveTimeTracking"), FALSE);
	m_bAllowParentTimeTracking = prefs.GetProfileInt(_T("Preferences"), _T("AllowParentTimeTracking"), TRUE);
	m_bTrackNonActiveTasklists = prefs.GetProfileInt(_T("Preferences"), _T("TrackNonActiveTasklists"), TRUE);
	m_bTrackNonSelectedTasks = prefs.GetProfileInt(_T("Preferences"), _T("TrackNonSelectedTasks"), TRUE);
	m_bTrackOnScreenSaver = prefs.GetProfileInt(_T("Preferences"), _T("TrackOnScreenSaver"), TRUE);
	m_bTrackHibernated = prefs.GetProfileInt(_T("Preferences"), _T("AllowTrackingWhenHibernated"), FALSE);
}

void CPreferencesTaskPage::SavePreferences(CPreferences& prefs)
{
	// save settings
	prefs.WriteProfileInt(_T("Preferences"), _T("TrackNonSelectedTasks"), m_bTrackNonSelectedTasks);
	prefs.WriteProfileInt(_T("Preferences"), _T("TrackNonActiveTasklists"), m_bTrackNonActiveTasklists);
	prefs.WriteProfileInt(_T("Preferences"), _T("TrackOnScreenSaver"), m_bTrackOnScreenSaver);
	prefs.WriteProfileInt(_T("Preferences"), _T("AllowTrackingWhenHibernated"), m_bTrackHibernated);
	prefs.WriteProfileInt(_T("Preferences"), _T("LogTime"), m_bLogTime);
	prefs.WriteProfileInt(_T("Preferences"), _T("LogTasksSeparately"), m_bLogTasksSeparately);
	prefs.WriteProfileInt(_T("Preferences"), _T("ExclusiveTimeTracking"), m_bExclusiveTimeTracking);
	prefs.WriteProfileInt(_T("Preferences"), _T("AllowParentTimeTracking"), m_bAllowParentTimeTracking);

	// validate time periods before writing
	m_sHoursInDay.Format(_T("%.2f"), GetHoursInOneDay());
	m_sDaysInWeek.Format(_T("%.2f"), GetDaysInOneWeek());
	prefs.WriteProfileString(_T("Preferences"), _T("DaysInWeek"), m_sDaysInWeek);
	prefs.WriteProfileString(_T("Preferences"), _T("HoursInDay"), m_sHoursInDay);
}
