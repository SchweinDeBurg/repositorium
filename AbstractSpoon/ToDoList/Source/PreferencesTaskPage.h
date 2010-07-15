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

#if !defined(AFX_PREFERENCESTASKPAGE_H__84CBF881_D8CA_4D00_ADD6_1DCB7DE71C5B__INCLUDED_)
#define AFX_PREFERENCESTASKPAGE_H__84CBF881_D8CA_4D00_ADD6_1DCB7DE71C5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PreferencesTaskPage.h : header file
//

#include "../../Common/GroupLine.h"
#include "../../../CodeProject/Source/DialogHelper.h"
#include "../../Common/PreferencesBase.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskPage dialog

class CPreferencesTaskPage : public CPreferencesPageBase, public CDialogHelper
{
	DECLARE_DYNCREATE(CPreferencesTaskPage)

// Construction
public:
	CPreferencesTaskPage();
	~CPreferencesTaskPage();

	BOOL GetAveragePercentSubCompletion() const { return m_bAveragePercentSubCompletion; }
	BOOL GetIncludeDoneInAverageCalc() const { return m_bIncludeDoneInAverageCalc; }
	BOOL GetUseEarliestDueDate() const { return m_bUseEarliestDueDate; }
	BOOL GetUsePercentDoneInTimeEst() const { return m_bUsePercentDoneInTimeEst; }
	BOOL GetTreatSubCompletedAsDone() const { return m_bTreatSubCompletedAsDone; }
	BOOL GetUseHighestPriority() const { return m_bUseHighestPriority; }
	BOOL GetUseHighestRisk() const { return m_bUseHighestPriority; } // Note: this uses same flag as priority
	BOOL GetAutoCalcTimeEstimates() const { return m_bAutoCalcTimeEst; }
	BOOL GetIncludeDoneInPriorityCalc() const { return m_bIncludeDoneInPriorityCalc; }
	BOOL GetWeightPercentCompletionByTimeEst() const { return m_bWeightPercentCompletionByTimeEst && m_bWeightPercentCompletionByNumSubtasks; }
	BOOL GetWeightPercentCompletionByPriority() const { return m_bWeightPercentCompletionByPriority && m_bWeightPercentCompletionByNumSubtasks; }
	BOOL GetWeightPercentCompletionByNumSubtasks() const { return m_bWeightPercentCompletionByNumSubtasks; }
	BOOL GetAutoCalcPercentDone() const { return m_bAutoCalcPercentDone; }
	BOOL GetTrackNonActiveTasklists() const { return m_bTrackNonActiveTasklists; }
	BOOL GetTrackOnScreenSaver() const { return m_bTrackOnScreenSaver; }
	BOOL GetTrackNonSelectedTasks() const { return m_bTrackNonSelectedTasks; }
	BOOL GetTrackHibernated() const { return m_bTrackHibernated; }
	double GetHoursInOneDay() const;
	double GetDaysInOneWeek() const;
	BOOL GetLogTimeTracking() const { return m_bLogTime; }
	BOOL GetLogTaskTimeSeparately() const { return m_bLogTime && m_bLogTasksSeparately; }
	BOOL GetExclusiveTimeTracking() const { return m_bExclusiveTimeTracking; }
	BOOL GetAllowParentTimeTracking() const { return m_bAllowParentTimeTracking; }
	BOOL GetAutoAdjustDependents() const { return m_bAutoAdjustDependents; }
	BOOL GetDueTasksHaveHighestPriority() const { return m_bDueTasksHaveHighestPriority; }
	BOOL GetDoneTasksHaveLowestPriority() const { return m_bDoneTasksHaveLowestPriority; }
	BOOL GetDoneTasksHaveLowestRisk() const { return m_bDoneTasksHaveLowestPriority; } // Note: this uses same flag as priority
	BOOL GetNoDueDateIsDueToday() const { return m_bNoDueDateDueToday; }

//	BOOL Get() const { return m_b; }

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesTaskPage)
	enum { IDD = IDD_PREFTASK_PAGE };
	BOOL	m_bTreatSubCompletedAsDone;
	BOOL	m_bUseHighestPriority;
	BOOL	m_bAutoCalcTimeEst;
	BOOL	m_bIncludeDoneInPriorityCalc;
	BOOL	m_bWeightPercentCompletionByTimeEst;
	BOOL	m_bWeightPercentCompletionByPriority;
	BOOL	m_bAutoCalcPercentDone;
	CString	m_sDaysInWeek;
	CString	m_sHoursInDay;
	BOOL	m_bLogTime;
	BOOL	m_bLogTasksSeparately;
	BOOL	m_bExclusiveTimeTracking;
	BOOL	m_bAllowParentTimeTracking;
	BOOL	m_bAutoAdjustDependents;
	BOOL	m_bWeightPercentCompletionByNumSubtasks;
	BOOL	m_bDueTasksHaveHighestPriority;
	BOOL	m_bDoneTasksHaveLowestPriority;
	BOOL	m_bNoDueDateDueToday;
	//}}AFX_DATA
	BOOL	m_bUseEarliestDueDate;
	BOOL	m_bUsePercentDoneInTimeEst;
	BOOL	m_bAveragePercentSubCompletion;
	BOOL	m_bIncludeDoneInAverageCalc;
	BOOL	m_bTrackNonSelectedTasks;
	BOOL	m_bTrackOnScreenSaver;
	BOOL	m_bTrackNonActiveTasklists;
	BOOL	m_bTrackHibernated;
	CGroupLineManager m_mgrGroupLines;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesTaskPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesTaskPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnUsehighestpriority();
	afx_msg void OnLogtime();
	afx_msg void OnAutocalcpercentdone();
	afx_msg void OnWeightpercentcalcbynumsub();
	//}}AFX_MSG
	afx_msg void OnAveragepercentChange();
	DECLARE_MESSAGE_MAP()

   virtual void LoadPreferences(const CPreferences& prefs);
   virtual void SavePreferences(CPreferences& prefs);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESTASKPAGE_H__84CBF881_D8CA_4D00_ADD6_1DCB7DE71C5B__INCLUDED_)
