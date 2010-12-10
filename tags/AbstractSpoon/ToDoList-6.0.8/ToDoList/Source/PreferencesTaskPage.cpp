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
	m_bTreatSubCompletedAsDone = prefs.GetProfileInt(_T("Preferences"), _T("TreatSubCompletedAsDone"), TRUE);
	m_bAveragePercentSubCompletion = prefs.GetProfileInt(_T("Preferences"), _T("AveragePercentSubCompletion"), TRUE);
	m_bIncludeDoneInAverageCalc = prefs.GetProfileInt(_T("Preferences"), _T("IncludeDoneInAverageCalc"), TRUE);
	m_bUseEarliestDueDate = prefs.GetProfileInt(_T("Preferences"), _T("UseEarliestDueDate"), FALSE);
	m_bUsePercentDoneInTimeEst = prefs.GetProfileInt(_T("Preferences"), _T("UsePercentDoneInTimeEst"), TRUE);
	m_bUseHighestPriority = prefs.GetProfileInt(_T("Preferences"), _T("UseHighestPriority"), FALSE);
	m_bAutoCalcTimeEst = prefs.GetProfileInt(_T("Preferences"), _T("AutoCalcTimeEst"), FALSE);
	m_bIncludeDoneInPriorityCalc = prefs.GetProfileInt(_T("Preferences"), _T("IncludeDoneInPriorityCalc"), FALSE);
	m_bAutoCalcPercentDone = prefs.GetProfileInt(_T("Preferences"), _T("AutoCalcPercentDone"), FALSE);
	m_sDaysInWeek = prefs.GetProfileString(_T("Preferences"), _T("DaysInWeek"), _T("5.00"));
	m_sHoursInDay = prefs.GetProfileString(_T("Preferences"), _T("HoursInDay"), _T("8.00"));
	m_bLogTime = prefs.GetProfileInt(_T("Preferences"), _T("LogTime"), TRUE);
	m_bLogTasksSeparately = prefs.GetProfileInt(_T("Preferences"), _T("LogTasksSeparately"), FALSE);
	m_bExclusiveTimeTracking = prefs.GetProfileInt(_T("Preferences"), _T("ExclusiveTimeTracking"), FALSE);
	m_bAllowParentTimeTracking = prefs.GetProfileInt(_T("Preferences"), _T("AllowParentTimeTracking"), TRUE);
	m_bAutoAdjustDependents = prefs.GetProfileInt(_T("Preferences"), _T("AutoAdjustDependents"), FALSE);
	m_bDueTasksHaveHighestPriority = prefs.GetProfileInt(_T("Preferences"), _T("DueTasksHaveHighestPriority"), FALSE);
	m_bDoneTasksHaveLowestPriority = prefs.GetProfileInt(_T("Preferences"), _T("DoneTasksHaveLowestPriority"), TRUE);
	m_bNoDueDateDueToday = prefs.GetProfileInt(_T("Preferences"), _T("NoDueDateIsDueToday"), FALSE);

	m_bWeightPercentCompletionByTimeEst = prefs.GetProfileInt(_T("Preferences"), _T("WeightPercentCompletionByTimeEst"), FALSE);
	m_bWeightPercentCompletionByPriority = prefs.GetProfileInt(_T("Preferences"), _T("WeightPercentCompletionByPriority"), FALSE);
	m_bWeightPercentCompletionByNumSubtasks = prefs.GetProfileInt(_T("Preferences"), _T("WeightPercentCompletionByNumSubtasks"), TRUE);

	// backwards compatibility: Weight by Num Tasks must be turned on for 
	// Weight by Priority and Weight by Time Estimate
	m_bWeightPercentCompletionByNumSubtasks |= m_bWeightPercentCompletionByPriority;
	m_bWeightPercentCompletionByNumSubtasks |= m_bWeightPercentCompletionByTimeEst;

	m_bTrackNonActiveTasklists = prefs.GetProfileInt(_T("Preferences"), _T("TrackNonActiveTasklists"), TRUE);
	m_bTrackNonSelectedTasks = prefs.GetProfileInt(_T("Preferences"), _T("TrackNonSelectedTasks"), TRUE);
	m_bTrackOnScreenSaver = prefs.GetProfileInt(_T("Preferences"), _T("TrackOnScreenSaver"), TRUE);
	m_bTrackHibernated = prefs.GetProfileInt(_T("Preferences"), _T("AllowTrackingWhenHibernated"), FALSE);

	// fix up m_bAveragePercentSubCompletion because it's overridden by m_bAutoCalcPercentDone
	if (m_bAutoCalcPercentDone)
		m_bAveragePercentSubCompletion = FALSE;
}

void CPreferencesTaskPage::SavePreferences(CPreferences& prefs)
{
	// save settings
	prefs.WriteProfileInt(_T("Preferences"), _T("TreatSubCompletedAsDone"), m_bTreatSubCompletedAsDone);
	prefs.WriteProfileInt(_T("Preferences"), _T("AveragePercentSubCompletion"), m_bAveragePercentSubCompletion);
	prefs.WriteProfileInt(_T("Preferences"), _T("IncludeDoneInAverageCalc"), m_bIncludeDoneInAverageCalc);
	prefs.WriteProfileInt(_T("Preferences"), _T("UseEarliestDueDate"), m_bUseEarliestDueDate);
	prefs.WriteProfileInt(_T("Preferences"), _T("UsePercentDoneInTimeEst"), m_bUsePercentDoneInTimeEst);
	prefs.WriteProfileInt(_T("Preferences"), _T("UseHighestPriority"), m_bUseHighestPriority);
	prefs.WriteProfileInt(_T("Preferences"), _T("AutoCalcTimeEst"), m_bAutoCalcTimeEst);
	prefs.WriteProfileInt(_T("Preferences"), _T("IncludeDoneInPriorityCalc"), m_bIncludeDoneInPriorityCalc);
	prefs.WriteProfileInt(_T("Preferences"), _T("WeightPercentCompletionByTimeEst"), m_bWeightPercentCompletionByTimeEst);
	prefs.WriteProfileInt(_T("Preferences"), _T("WeightPercentCompletionByPriority"), m_bWeightPercentCompletionByPriority);
	prefs.WriteProfileInt(_T("Preferences"), _T("WeightPercentCompletionByNumSubtasks"), m_bWeightPercentCompletionByNumSubtasks);
	prefs.WriteProfileInt(_T("Preferences"), _T("AutoCalcPercentDone"), m_bAutoCalcPercentDone);
	prefs.WriteProfileInt(_T("Preferences"), _T("TrackNonSelectedTasks"), m_bTrackNonSelectedTasks);
	prefs.WriteProfileInt(_T("Preferences"), _T("TrackNonActiveTasklists"), m_bTrackNonActiveTasklists);
	prefs.WriteProfileInt(_T("Preferences"), _T("TrackOnScreenSaver"), m_bTrackOnScreenSaver);
	prefs.WriteProfileInt(_T("Preferences"), _T("AllowTrackingWhenHibernated"), m_bTrackHibernated);
	prefs.WriteProfileInt(_T("Preferences"), _T("LogTime"), m_bLogTime);
	prefs.WriteProfileInt(_T("Preferences"), _T("LogTasksSeparately"), m_bLogTasksSeparately);
	prefs.WriteProfileInt(_T("Preferences"), _T("ExclusiveTimeTracking"), m_bExclusiveTimeTracking);
	prefs.WriteProfileInt(_T("Preferences"), _T("AllowParentTimeTracking"), m_bAllowParentTimeTracking);
	prefs.WriteProfileInt(_T("Preferences"), _T("AutoAdjustDependents"), m_bAutoAdjustDependents);
	prefs.WriteProfileInt(_T("Preferences"), _T("DueTasksHaveHighestPriority"), m_bDueTasksHaveHighestPriority);
	prefs.WriteProfileInt(_T("Preferences"), _T("DoneTasksHaveLowestPriority"), m_bDoneTasksHaveLowestPriority);
	prefs.WriteProfileInt(_T("Preferences"), _T("NoDueDateIsDueToday"), m_bNoDueDateDueToday);

	// validate time periods before writing
	m_sHoursInDay.Format(_T("%.2f"), GetHoursInOneDay());
	m_sDaysInWeek.Format(_T("%.2f"), GetDaysInOneWeek());
	prefs.WriteProfileString(_T("Preferences"), _T("DaysInWeek"), m_sDaysInWeek);
	prefs.WriteProfileString(_T("Preferences"), _T("HoursInDay"), m_sHoursInDay);
}


void CPreferencesTaskPage::OnWeightpercentcalcbynumsub() 
{
	UpdateData();

	GetDlgItem(IDC_WEIGHTPERCENTCALCBYTIMEEST)->EnableWindow(m_bAveragePercentSubCompletion && m_bWeightPercentCompletionByNumSubtasks);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYPRIORITY)->EnableWindow(m_bAveragePercentSubCompletion && m_bWeightPercentCompletionByNumSubtasks);
}
