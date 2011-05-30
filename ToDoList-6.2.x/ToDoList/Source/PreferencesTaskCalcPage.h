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
//*****************************************************************************

#if !defined(AFX_PREFERENCESTASKCALCPAGE_H__84CBF881_D8CA_4D00_ADD6_1DCB7DE71C5B__INCLUDED_)
#define AFX_PREFERENCESTASKCALCPAGE_H__84CBF881_D8CA_4D00_ADD6_1DCB7DE71C5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreferencesTaskCalcPage.h : header file
//

#include "../../Common/GroupLine.h"
#include "../../../CodeProject/Source/DialogHelper.h"
#include "../../Common/PreferencesBase.h"

enum
{
	PTCP_REMAININGTTIMEISDUEDATE,
	PTCP_REMAININGTTIMEISSPENT,
	PTCP_REMAININGTTIMEISPERCENTAGE,
};

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskCalcPage dialog

class CPreferencesTaskCalcPage : public CPreferencesPageBase, public CDialogHelper
{
	DECLARE_DYNCREATE(CPreferencesTaskCalcPage)

// Construction
public:
	CPreferencesTaskCalcPage();
	~CPreferencesTaskCalcPage();

	BOOL GetAveragePercentSubCompletion() const
	{
		return m_bAveragePercentSubCompletion;
	}
	BOOL GetIncludeDoneInAverageCalc() const
	{
		return m_bIncludeDoneInAverageCalc;
	}
	BOOL GetUseEarliestDueDate() const
	{
		return m_bUseEarliestDueDate;
	}
	BOOL GetUsePercentDoneInTimeEst() const
	{
		return m_bUsePercentDoneInTimeEst;
	}
	BOOL GetTreatSubCompletedAsDone() const
	{
		return m_bTreatSubCompletedAsDone;
	}
	BOOL GetUseHighestPriority() const
	{
		return m_bUseHighestPriority;
	}
	BOOL GetUseHighestRisk() const
	{
		return m_bUseHighestPriority;   // Note: this uses same flag as priority
	}
	BOOL GetAutoCalcTimeEstimates() const
	{
		return m_bAutoCalcTimeEst;
	}
	BOOL GetIncludeDoneInPriorityRiskCalc() const
	{
		return m_bIncludeDoneInPriorityRiskCalc;
	}
	BOOL GetWeightPercentCompletionByNumSubtasks() const
	{
		return m_bWeightPercentCompletionByNumSubtasks;
	}
	BOOL GetAutoCalcPercentDone() const
	{
		return m_bAutoCalcPercentDone;
	}
	BOOL GetAutoAdjustDependents() const
	{
		return m_bAutoAdjustDependents;
	}
	BOOL GetDueTasksHaveHighestPriority() const
	{
		return m_bDueTasksHaveHighestPriority;
	}
	BOOL GetDoneTasksHaveLowestPriority() const
	{
		return m_bDoneTasksHaveLowestPriority;
	}
	BOOL GetDoneTasksHaveLowestRisk() const
	{
		return m_bDoneTasksHaveLowestPriority;   // Note: this uses same flag as priority
	}
	BOOL GetNoDueDateIsDueToday() const
	{
		return m_bNoDueDateDueToday;
	}
	int GetTimeRemainingCalculation() const
	{
		return m_nCalcRemainingTime;
	}

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesTaskCalcPage)
	enum { IDD = IDD_PREFTASKCALC_PAGE };
	BOOL    m_bTreatSubCompletedAsDone;
	BOOL    m_bUseHighestPriority;
	BOOL    m_bAutoCalcTimeEst;
	BOOL    m_bIncludeDoneInPriorityRiskCalc;
	BOOL    m_bAutoCalcPercentDone;
	BOOL    m_bAutoAdjustDependents;
	BOOL    m_bWeightPercentCompletionByNumSubtasks;
	BOOL    m_bDueTasksHaveHighestPriority;
	BOOL    m_bDoneTasksHaveLowestPriority;
	BOOL    m_bNoDueDateDueToday;
	int     m_nCalcRemainingTime;
	//}}AFX_DATA
	BOOL    m_bUseEarliestDueDate;
	BOOL    m_bUsePercentDoneInTimeEst;
	BOOL    m_bAveragePercentSubCompletion;
	BOOL    m_bIncludeDoneInAverageCalc;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesTaskCalcPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesTaskCalcPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnUsehighestpriority();
	afx_msg void OnAutocalcpercentdone();
	//}}AFX_MSG
	afx_msg void OnAveragepercentChange();
	DECLARE_MESSAGE_MAP()

	virtual void LoadPreferences(const CPreferences& prefs);
	virtual void SavePreferences(CPreferences& prefs);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESTASKCALCPAGE_H__84CBF881_D8CA_4D00_ADD6_1DCB7DE71C5B__INCLUDED_)
