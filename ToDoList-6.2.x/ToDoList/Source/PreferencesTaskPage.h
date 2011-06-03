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

	BOOL GetTrackNonActiveTasklists() const
	{
		return m_bTrackNonActiveTasklists;
	}
	BOOL GetTrackOnScreenSaver() const
	{
		return m_bTrackOnScreenSaver;
	}
	BOOL GetTrackNonSelectedTasks() const
	{
		return m_bTrackNonSelectedTasks;
	}
	BOOL GetTrackHibernated() const
	{
		return m_bTrackHibernated;
	}
	double GetHoursInOneDay() const;
	double GetDaysInOneWeek() const;
	BOOL GetLogTimeTracking() const
	{
		return m_bLogTime;
	}
	BOOL GetLogTaskTimeSeparately() const
	{
		return m_bLogTime && m_bLogTasksSeparately;
	}
	BOOL GetExclusiveTimeTracking() const
	{
		return m_bExclusiveTimeTracking;
	}
	BOOL GetAllowParentTimeTracking() const
	{
		return m_bAllowParentTimeTracking;
	}

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesTaskPage)
	enum { IDD = IDD_PREFTASK_PAGE };
	CString m_sDaysInWeek;
	CString m_sHoursInDay;
	BOOL    m_bLogTime;
	BOOL    m_bLogTasksSeparately;
	BOOL    m_bExclusiveTimeTracking;
	BOOL    m_bAllowParentTimeTracking;
	//}}AFX_DATA
	BOOL    m_bTrackNonSelectedTasks;
	BOOL    m_bTrackOnScreenSaver;
	BOOL    m_bTrackNonActiveTasklists;
	BOOL    m_bTrackHibernated;
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
	afx_msg void OnLogtime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void LoadPreferences(const CPreferences& prefs);
	virtual void SavePreferences(CPreferences& prefs);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESTASKPAGE_H__84CBF881_D8CA_4D00_ADD6_1DCB7DE71C5B__INCLUDED_)
