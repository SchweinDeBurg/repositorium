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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// PreferencesTaskCalcPage.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "PreferencesTaskCalcPage.h"

#include "../../../CodeProject/Source/Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskCalcPage property page

IMPLEMENT_DYNCREATE(CPreferencesTaskCalcPage, CPreferencesPageBase)

CPreferencesTaskCalcPage::CPreferencesTaskCalcPage() :
CPreferencesPageBase(CPreferencesTaskCalcPage::IDD)
{
	//{{AFX_DATA_INIT(CPreferencesTaskCalcPage)
	m_bNoDueDateDueToday = FALSE;
	m_nCalcRemainingTime = 0;
	//}}AFX_DATA_INIT
}

CPreferencesTaskCalcPage::~CPreferencesTaskCalcPage()
{
}

void CPreferencesTaskCalcPage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesTaskCalcPage)
	DDX_Check(pDX, IDC_TREATSUBCOMPLETEDASDONE, m_bTreatSubCompletedAsDone);
	DDX_Check(pDX, IDC_USEHIGHESTPRIORITY, m_bUseHighestPriority);
	DDX_Check(pDX, IDC_AUTOCALCTIMEEST, m_bAutoCalcTimeEst);
	DDX_Check(pDX, IDC_INCLUDEDONEINPRIORITYCALC, m_bIncludeDoneInPriorityRiskCalc);
	DDX_Check(pDX, IDC_AUTOCALCPERCENTDONE, m_bAutoCalcPercentDone);
	DDX_Check(pDX, IDC_AUTOUPDATEDEPENDEES, m_bAutoAdjustDependents);
	DDX_Check(pDX, IDC_WEIGHTPERCENTCALCBYNUMSUB, m_bWeightPercentCompletionByNumSubtasks);
	DDX_Check(pDX, IDC_DUEHAVEHIGHESTPRIORITY, m_bDueTasksHaveHighestPriority);
	DDX_Check(pDX, IDC_DONEHAVELOWESTPRIORITY, m_bDoneTasksHaveLowestPriority);
	DDX_Check(pDX, IDC_NODUEDATEDUETODAY, m_bNoDueDateDueToday);
	DDX_Radio(pDX, IDC_REMAININGTIMEISDUEDATE, m_nCalcRemainingTime);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_USEEARLIESTDUEDATE, m_bUseEarliestDueDate);
	DDX_Check(pDX, IDC_USEPERCENTDONEINTIMEEST, m_bUsePercentDoneInTimeEst);
	DDX_Check(pDX, IDC_AVERAGEPERCENTSUBCOMPLETION, m_bAveragePercentSubCompletion);
	DDX_Check(pDX, IDC_INCLUDEDONEINAVERAGECALC, m_bIncludeDoneInAverageCalc);
}

BEGIN_MESSAGE_MAP(CPreferencesTaskCalcPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesTaskCalcPage)
	ON_BN_CLICKED(IDC_USEHIGHESTPRIORITY, OnUsehighestpriority)
	ON_BN_CLICKED(IDC_AUTOCALCPERCENTDONE, OnAutocalcpercentdone)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_AVERAGEPERCENTSUBCOMPLETION, OnAveragepercentChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskCalcPage message handlers

BOOL CPreferencesTaskCalcPage::OnInitDialog()
{
	CPreferencesPageBase::OnInitDialog();

	GetDlgItem(IDC_INCLUDEDONEINAVERAGECALC)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYNUMSUB)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_INCLUDEDONEINPRIORITYCALC)->EnableWindow(m_bUseHighestPriority);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesTaskCalcPage::OnAveragepercentChange()
{
	UpdateData();

	GetDlgItem(IDC_INCLUDEDONEINAVERAGECALC)->EnableWindow(m_bAveragePercentSubCompletion);
	GetDlgItem(IDC_WEIGHTPERCENTCALCBYNUMSUB)->EnableWindow(m_bAveragePercentSubCompletion);

	// uncheck IDC_AUTOCALCPERCENTDONE if m_bAveragePercentSubCompletion is TRUE
	if (m_bAveragePercentSubCompletion && m_bAutoCalcPercentDone)
	{
		m_bAutoCalcPercentDone = FALSE;
		UpdateData(FALSE);
	}

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesTaskCalcPage::OnUsehighestpriority()
{
	UpdateData();

	GetDlgItem(IDC_INCLUDEDONEINPRIORITYCALC)->EnableWindow(m_bUseHighestPriority);

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesTaskCalcPage::OnAutocalcpercentdone()
{
	UpdateData();

	// uncheck IDC_AUTOCALCPERCENTDONE if m_bAveragePercentSubCompletion is TRUE
	if (m_bAutoCalcPercentDone && m_bAveragePercentSubCompletion)
	{
		m_bAveragePercentSubCompletion = FALSE;
		UpdateData(FALSE);

		OnAveragepercentChange();
	}

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesTaskCalcPage::LoadPreferences(const CPreferences& prefs)
{
	// load settings
	m_bTreatSubCompletedAsDone = prefs.GetProfileInt(_T("Preferences"), _T("TreatSubCompletedAsDone"), TRUE);
	m_bAveragePercentSubCompletion = prefs.GetProfileInt(_T("Preferences"), _T("AveragePercentSubCompletion"), TRUE);
	m_bIncludeDoneInAverageCalc = prefs.GetProfileInt(_T("Preferences"), _T("IncludeDoneInAverageCalc"), TRUE);
	m_bUseEarliestDueDate = prefs.GetProfileInt(_T("Preferences"), _T("UseEarliestDueDate"), FALSE);
	m_bUsePercentDoneInTimeEst = prefs.GetProfileInt(_T("Preferences"), _T("UsePercentDoneInTimeEst"), TRUE);
	m_bUseHighestPriority = prefs.GetProfileInt(_T("Preferences"), _T("UseHighestPriority"), FALSE);
	m_bAutoCalcTimeEst = prefs.GetProfileInt(_T("Preferences"), _T("AutoCalcTimeEst"), FALSE);
	m_bIncludeDoneInPriorityRiskCalc = prefs.GetProfileInt(_T("Preferences"), _T("IncludeDoneInPriorityCalc"), FALSE);
	m_bAutoCalcPercentDone = prefs.GetProfileInt(_T("Preferences"), _T("AutoCalcPercentDone"), FALSE);
	m_bAutoAdjustDependents = prefs.GetProfileInt(_T("Preferences"), _T("AutoAdjustDependents"), TRUE);
	m_bDueTasksHaveHighestPriority = prefs.GetProfileInt(_T("Preferences"), _T("DueTasksHaveHighestPriority"), FALSE);
	m_bDoneTasksHaveLowestPriority = prefs.GetProfileInt(_T("Preferences"), _T("DoneTasksHaveLowestPriority"), TRUE);
	m_bNoDueDateDueToday = prefs.GetProfileInt(_T("Preferences"), _T("NoDueDateIsDueToday"), FALSE);
	m_bWeightPercentCompletionByNumSubtasks = prefs.GetProfileInt(_T("Preferences"), _T("WeightPercentCompletionByNumSubtasks"), TRUE);
	m_nCalcRemainingTime = prefs.GetProfileInt(_T("Preferences"), _T("CalcRemainingTime"), 0);

	// fix up m_bAveragePercentSubCompletion because it's overridden by m_bAutoCalcPercentDone
	if (m_bAutoCalcPercentDone)
	{
		m_bAveragePercentSubCompletion = FALSE;
	}
}

void CPreferencesTaskCalcPage::SavePreferences(CPreferences& prefs)
{
	// save settings
	prefs.WriteProfileInt(_T("Preferences"), _T("TreatSubCompletedAsDone"), m_bTreatSubCompletedAsDone);
	prefs.WriteProfileInt(_T("Preferences"), _T("AveragePercentSubCompletion"), m_bAveragePercentSubCompletion);
	prefs.WriteProfileInt(_T("Preferences"), _T("IncludeDoneInAverageCalc"), m_bIncludeDoneInAverageCalc);
	prefs.WriteProfileInt(_T("Preferences"), _T("UseEarliestDueDate"), m_bUseEarliestDueDate);
	prefs.WriteProfileInt(_T("Preferences"), _T("UsePercentDoneInTimeEst"), m_bUsePercentDoneInTimeEst);
	prefs.WriteProfileInt(_T("Preferences"), _T("UseHighestPriority"), m_bUseHighestPriority);
	prefs.WriteProfileInt(_T("Preferences"), _T("AutoCalcTimeEst"), m_bAutoCalcTimeEst);
	prefs.WriteProfileInt(_T("Preferences"), _T("IncludeDoneInPriorityCalc"), m_bIncludeDoneInPriorityRiskCalc);
	prefs.WriteProfileInt(_T("Preferences"), _T("WeightPercentCompletionByNumSubtasks"), m_bWeightPercentCompletionByNumSubtasks);
	prefs.WriteProfileInt(_T("Preferences"), _T("AutoCalcPercentDone"), m_bAutoCalcPercentDone);
	prefs.WriteProfileInt(_T("Preferences"), _T("AutoAdjustDependents"), m_bAutoAdjustDependents);
	prefs.WriteProfileInt(_T("Preferences"), _T("DueTasksHaveHighestPriority"), m_bDueTasksHaveHighestPriority);
	prefs.WriteProfileInt(_T("Preferences"), _T("DoneTasksHaveLowestPriority"), m_bDoneTasksHaveLowestPriority);
	prefs.WriteProfileInt(_T("Preferences"), _T("NoDueDateIsDueToday"), m_bNoDueDateDueToday);
	prefs.WriteProfileInt(_T("Preferences"), _T("CalcRemainingTime"), m_nCalcRemainingTime);
}
