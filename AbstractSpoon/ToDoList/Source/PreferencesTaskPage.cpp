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
	m_bNoDueDateDueToday = FALSE;
	//}}AFX_DATA_INIT

}

CPreferencesTaskPage::~CPreferencesTaskPage()
{
}

void CPreferencesTaskPage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesTaskPage)
	DDX_Check(pDX, IDC_TREATSUBCOMPLETEDASDONE, m_bTreatSubCompletedAsDone);
	DDX_Check(pDX, IDC_USEHIGHESTPRIORITY, m_bUseHighestPriority);
	DDX_Check(pDX, IDC_AUTOCALCTIMEEST, m_bAutoCalcTimeEst);
	DDX_Check(pDX, IDC_INCLUDEDONEINPRIORITYCALC, m_bIncludeDoneInPriorityCalc);
	DDX_Check(pDX, IDC_WEIGHTPERCENTCALCBYTIMEEST, m_bWeightPercentCompletionByTimeEst);
	DDX_Check(pDX, IDC_WEIGHTPERCENTCALCBYPRIORITY, m_bWeightPercentCompletionByPriority);
	DDX_Check(pDX, IDC_AUTOCALCPERCENTDONE, m_bAutoCalcPercentDone);
	DDX_CBString(pDX, IDC_DAYSINONEWEEK, m_sDaysInWeek);
	DDX_CBString(pDX, IDC_HOURSINONEDAY, m_sHoursInDay);
	DDX_Check(pDX, IDC_LOGTIME, m_bLogTime);
	DDX_Check(pDX, IDC_LOGTASKSEPARATELY, m_bLogTasksSeparately);
	DDX_Check(pDX, IDC_EXCLUSIVETRACKING, m_bExclusiveTimeTracking);
	DDX_Check(pDX, IDC_ALLOWPARENTRACKING, m_bAllowParentTimeTracking);
	DDX_Check(pDX, IDC_AUTOUPDATEDEPENDEES, m_bAutoAdjustDependents);
	DDX_Check(pDX, IDC_WEIGHTPERCENTCALCBYNUMSUB, m_bWeightPercentCompletionByNumSubtasks);
	DDX_Check(pDX, IDC_DUEHAVEHIGHESTPRIORITY, m_bDueTasksHaveHighestPriority);
	DDX_Check(pDX, IDC_DONEHAVELOWESTPRIORITY, m_bDoneTasksHaveLowestPriority);
	DDX_Check(pDX, IDC_TRACKNONSELECTEDTASKS, m_bTrackNonSelectedTasks);
	DDX_Check(pDX, IDC_TRACKSCREENSAVER, m_bTrackOnScreenSaver);
	DDX_Check(pDX, IDC_TRACKNONACTIVETASKLISTS, m_bTrackNonActiveTasklists);
	DDX_Check(pDX, IDC_TRACKHIBERNATED, m_bTrackHibernated);
	DDX_Check(pDX, IDC_NODUEDATEDUETODAY, m_bNoDueDateDueToday);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_USEEARLIESTDUEDATE, m_bUseEarliestDueDate);
	DDX_Check(pDX, IDC_USEPERCENTDONEINTIMEEST, m_bUsePercentDoneInTimeEst);
	DDX_Check(pDX, IDC_AVERAGEPERCENTSUBCOMPLETION, m_bAveragePercentSubCompletion);
	DDX_Check(pDX, IDC_INCLUDEDONEINAVERAGECALC, m_bIncludeDoneInAverageCalc);
}


BEGIN_MESSAGE_MAP(CPreferencesTaskPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesTaskPage)
	ON_BN_CLICKED(IDC_USEHIGHESTPRIORITY, OnUsehighestpriority)
	ON_BN_CLICKED(IDC_LOGTIME, OnLogtime)
	ON_BN_CLICKED(IDC_AUTOCALCPERCENTDONE, OnAutocalcpercentdone)
	ON_BN_CLICKED(IDC_WEIGHTPERCENTCALCBYNUMSUB, OnWeightpercentcalcbynumsub)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_AVERAGEPERCENTSUBCOMPLETION, OnAveragepercentChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskPage message handlers

BOOL CPreferencesTaskPage::OnInitDialog() 
{
	CPreferencesPageBase::OnInitDialog();

	m_mgrGroupLines.AddGroupLine(IDC_TRACKGROUP, *this); 
	m_mgrGroupLines.AddGroupLine(IDC_TIMEGROUP, *this); 

	GetDlgItem(IDC_INCLUDEDONEINAVERAGECALC)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYTIMEEST)->EnableWindow(m_bAveragePercentSubCompletion && m_bWeightPercentCompletionByNumSubtasks);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYPRIORITY)->EnableWindow(m_bAveragePercentSubCompletion && m_bWeightPercentCompletionByNumSubtasks);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYNUMSUB)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_INCLUDEDONEINPRIORITYCALC)->EnableWindow(m_bUseHighestPriority);
	GetDlgItem(IDC_LOGTASKSEPARATELY)->EnableWindow(m_bLogTime);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesTaskPage::OnAveragepercentChange() 
{
	UpdateData();

	GetDlgItem(IDC_INCLUDEDONEINAVERAGECALC)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYTIMEEST)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYPRIORITY)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYNUMSUB)->EnableWindow(m_bAveragePercentSubCompletion);

	// uncheck IDC_AUTOCALCPERCENTDONE if m_bAveragePercentSubCompletion is TRUE
	if (m_bAveragePercentSubCompletion && m_bAutoCalcPercentDone)
	{
		m_bAutoCalcPercentDone = FALSE;
		UpdateData(FALSE);
	}
}


void CPreferencesTaskPage::OnUsehighestpriority() 
{
	UpdateData();

	GetDlgItem(IDC_INCLUDEDONEINPRIORITYCALC)->EnableWindow(m_bUseHighestPriority);
}

double CPreferencesTaskPage::GetHoursInOneDay() const
{
	double dHours = Misc::Atof(m_sHoursInDay);

	if (dHours <= 0 || dHours > 24)
		dHours = 8;

	return dHours;
}

double CPreferencesTaskPage::GetDaysInOneWeek() const
{
	double dDays = Misc::Atof(m_sDaysInWeek);

	if (dDays <= 0 || dDays > 7)
		dDays = 5;

	return dDays;
}

void CPreferencesTaskPage::OnLogtime() 
{
	UpdateData();
	
	GetDlgItem(IDC_LOGTASKSEPARATELY)->EnableWindow(m_bLogTime);
}

void CPreferencesTaskPage::OnAutocalcpercentdone() 
{
	UpdateData();

	// uncheck IDC_AUTOCALCPERCENTDONE if m_bAveragePercentSubCompletion is TRUE
	if (m_bAutoCalcPercentDone && m_bAveragePercentSubCompletion)
	{
		m_bAveragePercentSubCompletion = FALSE;
		UpdateData(FALSE);

		OnAveragepercentChange();
	}
}

void CPreferencesTaskPage::LoadPreferences(const CPreferences& prefs)
{
	// load settings
	m_bTreatSubCompletedAsDone = prefs.GetProfileInt("Preferences", "TreatSubCompletedAsDone", TRUE);
	m_bAveragePercentSubCompletion = prefs.GetProfileInt("Preferences", "AveragePercentSubCompletion", TRUE);
	m_bIncludeDoneInAverageCalc = prefs.GetProfileInt("Preferences", "IncludeDoneInAverageCalc", TRUE);
	m_bUseEarliestDueDate = prefs.GetProfileInt("Preferences", "UseEarliestDueDate", FALSE);
	m_bUsePercentDoneInTimeEst = prefs.GetProfileInt("Preferences", "UsePercentDoneInTimeEst", TRUE);
	m_bUseHighestPriority = prefs.GetProfileInt("Preferences", "UseHighestPriority", FALSE);
	m_bAutoCalcTimeEst = prefs.GetProfileInt("Preferences", "AutoCalcTimeEst", FALSE);
	m_bIncludeDoneInPriorityCalc = prefs.GetProfileInt("Preferences", "IncludeDoneInPriorityCalc", FALSE);
	m_bAutoCalcPercentDone = prefs.GetProfileInt("Preferences", "AutoCalcPercentDone", FALSE);
	m_sDaysInWeek = prefs.GetProfileString("Preferences", "DaysInWeek", "5.00");
	m_sHoursInDay = prefs.GetProfileString("Preferences", "HoursInDay", "8.00");
	m_bLogTime = prefs.GetProfileInt("Preferences", "LogTime", TRUE);
	m_bLogTasksSeparately = prefs.GetProfileInt("Preferences", "LogTasksSeparately", FALSE);
	m_bExclusiveTimeTracking = prefs.GetProfileInt("Preferences", "ExclusiveTimeTracking", FALSE);
	m_bAllowParentTimeTracking = prefs.GetProfileInt("Preferences", "AllowParentTimeTracking", TRUE);
	m_bAutoAdjustDependents = prefs.GetProfileInt("Preferences", "AutoAdjustDependents", FALSE);
	m_bDueTasksHaveHighestPriority = prefs.GetProfileInt("Preferences", "DueTasksHaveHighestPriority", FALSE);
	m_bDoneTasksHaveLowestPriority = prefs.GetProfileInt("Preferences", "DoneTasksHaveLowestPriority", TRUE);
	m_bNoDueDateDueToday = prefs.GetProfileInt("Preferences", "NoDueDateIsDueToday", FALSE);

	m_bWeightPercentCompletionByTimeEst = prefs.GetProfileInt("Preferences", "WeightPercentCompletionByTimeEst", FALSE);
	m_bWeightPercentCompletionByPriority = prefs.GetProfileInt("Preferences", "WeightPercentCompletionByPriority", FALSE);
	m_bWeightPercentCompletionByNumSubtasks = prefs.GetProfileInt("Preferences", "WeightPercentCompletionByNumSubtasks", TRUE);

	// backwards compatibility: Weight by Num Tasks must be turned on for 
	// Weight by Priority and Weight by Time Estimate
	m_bWeightPercentCompletionByNumSubtasks |= m_bWeightPercentCompletionByPriority;
	m_bWeightPercentCompletionByNumSubtasks |= m_bWeightPercentCompletionByTimeEst;

	m_bTrackNonActiveTasklists = prefs.GetProfileInt("Preferences", "TrackNonActiveTasklists", TRUE);
	m_bTrackNonSelectedTasks = prefs.GetProfileInt("Preferences", "TrackNonSelectedTasks", TRUE);
	m_bTrackOnScreenSaver = prefs.GetProfileInt("Preferences", "TrackOnScreenSaver", TRUE);
	m_bTrackHibernated = prefs.GetProfileInt("Preferences", "AllowTrackingWhenHibernated", FALSE);

	// fix up m_bAveragePercentSubCompletion because it's overridden by m_bAutoCalcPercentDone
	if (m_bAutoCalcPercentDone)
		m_bAveragePercentSubCompletion = FALSE;

//	m_b = prefs.GetProfileInt("Preferences", "", FALSE);
}

void CPreferencesTaskPage::SavePreferences(CPreferences& prefs)
{
	// save settings
	prefs.WriteProfileInt("Preferences", "TreatSubCompletedAsDone", m_bTreatSubCompletedAsDone);
	prefs.WriteProfileInt("Preferences", "AveragePercentSubCompletion", m_bAveragePercentSubCompletion);
	prefs.WriteProfileInt("Preferences", "IncludeDoneInAverageCalc", m_bIncludeDoneInAverageCalc);
	prefs.WriteProfileInt("Preferences", "UseEarliestDueDate", m_bUseEarliestDueDate);
	prefs.WriteProfileInt("Preferences", "UsePercentDoneInTimeEst", m_bUsePercentDoneInTimeEst);
	prefs.WriteProfileInt("Preferences", "UseHighestPriority", m_bUseHighestPriority);
	prefs.WriteProfileInt("Preferences", "AutoCalcTimeEst", m_bAutoCalcTimeEst);
	prefs.WriteProfileInt("Preferences", "IncludeDoneInPriorityCalc", m_bIncludeDoneInPriorityCalc);
	prefs.WriteProfileInt("Preferences", "WeightPercentCompletionByTimeEst", m_bWeightPercentCompletionByTimeEst);
	prefs.WriteProfileInt("Preferences", "WeightPercentCompletionByPriority", m_bWeightPercentCompletionByPriority);
	prefs.WriteProfileInt("Preferences", "WeightPercentCompletionByNumSubtasks", m_bWeightPercentCompletionByNumSubtasks);
	prefs.WriteProfileInt("Preferences", "AutoCalcPercentDone", m_bAutoCalcPercentDone);
	prefs.WriteProfileInt("Preferences", "TrackNonSelectedTasks", m_bTrackNonSelectedTasks);
	prefs.WriteProfileInt("Preferences", "TrackNonActiveTasklists", m_bTrackNonActiveTasklists);
	prefs.WriteProfileInt("Preferences", "TrackOnScreenSaver", m_bTrackOnScreenSaver);
	prefs.WriteProfileInt("Preferences", "AllowTrackingWhenHibernated", m_bTrackHibernated);
	prefs.WriteProfileInt("Preferences", "LogTime", m_bLogTime);
	prefs.WriteProfileInt("Preferences", "LogTasksSeparately", m_bLogTasksSeparately);
	prefs.WriteProfileInt("Preferences", "ExclusiveTimeTracking", m_bExclusiveTimeTracking);
	prefs.WriteProfileInt("Preferences", "AllowParentTimeTracking", m_bAllowParentTimeTracking);
	prefs.WriteProfileInt("Preferences", "AutoAdjustDependents", m_bAutoAdjustDependents);
	prefs.WriteProfileInt("Preferences", "DueTasksHaveHighestPriority", m_bDueTasksHaveHighestPriority);
	prefs.WriteProfileInt("Preferences", "DoneTasksHaveLowestPriority", m_bDoneTasksHaveLowestPriority);
	prefs.WriteProfileInt("Preferences", "NoDueDateIsDueToday", m_bNoDueDateDueToday);

	// validate time periods before writing
	m_sHoursInDay.Format("%.2f", GetHoursInOneDay());
	m_sDaysInWeek.Format("%.2f", GetDaysInOneWeek());
	prefs.WriteProfileString("Preferences", "DaysInWeek", m_sDaysInWeek);
	prefs.WriteProfileString("Preferences", "HoursInDay", m_sHoursInDay);

//	prefs.WriteProfileInt("Preferences", "", m_b);
}


void CPreferencesTaskPage::OnWeightpercentcalcbynumsub() 
{
	UpdateData();

	GetDlgItem(IDC_WEIGHTPERCENTCALCBYTIMEEST)->EnableWindow(m_bAveragePercentSubCompletion && m_bWeightPercentCompletionByNumSubtasks);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYPRIORITY)->EnableWindow(m_bAveragePercentSubCompletion && m_bWeightPercentCompletionByNumSubtasks);
}
