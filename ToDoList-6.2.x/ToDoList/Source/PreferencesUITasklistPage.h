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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

#if !defined(AFX_PREFERENCESUITASKLISTPAGE_H__9612D6FB_2A00_46DA_99A4_1AC6270F060D__INCLUDED_)
#define AFX_PREFERENCESUITASKLISTPAGE_H__9612D6FB_2A00_46DA_99A4_1AC6270F060D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PreferencesUITasklistPage.h : header file
//

#include "TDLColumnListBox.h"

#include "../../../CodeProject/Source/DialogHelper.h"
#include "../../Common/GroupLine.h"
#include "../../Common/PreferencesBase.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUITasklistPage dialog

class CPreferencesUITasklistPage : public CPreferencesPageBase, protected CDialogHelper
{
// Construction
public:
	CPreferencesUITasklistPage();
	~CPreferencesUITasklistPage();

	BOOL GetShowInfoTips() const
	{
		return m_bShowInfoTips;
	}
	BOOL GetShowComments() const
	{
		return m_bShowComments;
	}
	int GetVisibleColumns(CTDCColumnArray& aColumns) const;
	void SetVisibleColumns(const CTDCColumnArray& aColumns);
	BOOL GetShowPathInHeader() const
	{
		return m_bShowPathInHeader;
	}
	BOOL GetStrikethroughDone() const
	{
		return m_bStrikethroughDone;
	}
	BOOL GetFullRowSelection() const
	{
		return m_bFullRowSelection;
	}
	BOOL GetTreeCheckboxes() const
	{
		return m_bTreeCheckboxes;
	}
	BOOL GetTreeTaskIcons() const
	{
		return m_bTreeTaskIcons;
	}
	BOOL GetDisplayDatesInISO() const
	{
		return m_bUseISOForDates;
	}
	BOOL GetShowWeekdayInDates() const
	{
		return m_bShowWeekdayInDates;
	}
	BOOL GetShowParentsAsFolders() const
	{
		return m_bShowParentsAsFolders;
	}
	BOOL GetDisplayFirstCommentLine() const
	{
		return m_bShowComments && m_bDisplayFirstCommentLine;
	}
	int GetMaxInfoTipCommentsLength() const;
	BOOL GetHidePercentForDoneTasks() const
	{
		return m_bHidePercentForDoneTasks;
	}
	BOOL GetHideZeroTimeCost() const
	{
		return m_bHideZeroTimeCost;
	}
	BOOL GetHideStartDueForDoneTasks() const
	{
		return m_bHideStartDueForDoneTasks;
	}
	BOOL GetShowPercentAsProgressbar() const
	{
		return m_bShowPercentAsProgressbar;
	}
	BOOL GetRoundTimeFractions() const
	{
		return m_bRoundTimeFractions;
	}
	BOOL GetShowNonFilesAsText() const
	{
		return m_bShowNonFilesAsText;
	}
	BOOL GetUseHMSTimeFormat() const
	{
		return m_bUseHMSTimeFormat;
	}
	BOOL GetAutoFocusTasklist() const
	{
		return m_bAutoFocusTasklist;
	}
	BOOL GetShowSubtaskCompletion() const
	{
		return m_bShowSubtaskCompletion;
	}
	BOOL GetShowColumnsOnRight() const
	{
		return m_bShowColumnsOnRight;
	}
	BOOL GetHideDueTimeField() const
	{
		return m_bHideDueTimeField;
	}
	BOOL GetHideStartTimeField() const
	{
		return m_bHideStartTimeField;
	}
	BOOL GetHideDoneTimeField() const
	{
		return m_bHideDoneTimeField;
	}
	int GetMaxColumnWidth() const;

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesUITasklistPage)
	enum { IDD = IDD_PREFUITASKLIST_PAGE };
	BOOL    m_bUseISOForDates;
	BOOL    m_bShowWeekdayInDates;
	BOOL    m_bShowParentsAsFolders;
	BOOL    m_bDisplayFirstCommentLine;
	int     m_nMaxInfoTipCommentsLength;
	BOOL    m_bLimitInfoTipCommentsLength;
	BOOL    m_bAutoFocusTasklist;
	BOOL    m_bShowSubtaskCompletion;
	BOOL    m_bShowColumnsOnRight;
	BOOL    m_bHideDueTimeField;
	BOOL    m_bHideStartTimeField;
	BOOL    m_bLimitColumnWidths;
	int     m_nMaxColumnWidth;
	BOOL    m_bHideDoneTimeField;
	//}}AFX_DATA
	BOOL    m_bShowPathInHeader;
	BOOL    m_bStrikethroughDone;
	BOOL    m_bFullRowSelection;
	BOOL    m_bTreeCheckboxes;
	BOOL    m_bTreeTaskIcons;
	CTDLColumnListBox m_lbColumnVisibility;
	BOOL    m_bShowInfoTips;
	BOOL    m_bShowComments;
	BOOL    m_bShowPercentColumn;
	BOOL    m_bShowPriorityColumn;
	int     m_nSelColumnVisibility;
	BOOL    m_bHideZeroTimeCost;
	BOOL    m_bHideStartDueForDoneTasks;
	BOOL    m_bRoundTimeFractions;
	BOOL    m_bShowNonFilesAsText;
	BOOL    m_bUseHMSTimeFormat;
	BOOL    m_bShowPercentAsProgressbar;
	BOOL    m_bHidePercentForDoneTasks;
	CGroupLineManager m_mgrGroupLines;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesUITasklistPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesUITasklistPage)
	afx_msg void OnShowcomments();
	afx_msg void OnShowinfotips();
	afx_msg void OnLimitinfotipcomments();
	afx_msg void OnLimitcolwidths();
	afx_msg void OnTreecheckboxes();
	afx_msg void OnShowparentsasfolders();
	afx_msg void OnTreetaskicons();
	//}}AFX_MSG
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	void SaveColumns() const;
	virtual void LoadPreferences(const CPreferences& prefs);
	virtual void SavePreferences(CPreferences& prefs);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESUITASKLISTPAGE_H__9612D6FB_2A00_46DA_99A4_1AC6270F060D__INCLUDED_)
