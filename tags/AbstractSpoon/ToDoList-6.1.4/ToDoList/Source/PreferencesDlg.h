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

#if !defined(AFX_PREFERENCESDLG_H__C3FCC72A_6C69_49A6_930D_D5C94EC31298__INCLUDED_)
#define AFX_PREFERENCESDLG_H__C3FCC72A_6C69_49A6_930D_D5C94EC31298__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PreferencesDlg.h : header file
//

#include "../../Common/PreferencesBase.h"

#include "PreferencesGenPage.h"
#include "PreferencesTaskPage.h"
#include "PreferencesTaskDefPage.h"
#include "PreferencesToolPage.h"
#include "PreferencesUIPage.h"
#include "PreferencesUITasklistPage.h"
#include "PreferencesUITasklistColorsPage.h"
#include "PreferencesShortcutsPage.h"
#include "PreferencesFilePage.h"
#include "PreferencesExportPage.h"
#include "PreferencesMultiUserPage.h"
#include "PreferencesFile2Page.h"
#include "PreferencesTaskCalcPage.h"

// matches order of pages
enum
{
	PREFPAGE_GEN,
	PREFPAGE_MULTIUSER,
	PREFPAGE_FILE,
	PREFPAGE_FILE2,
	PREFPAGE_UI,
	PREFPAGE_UITASK,
	PREFPAGE_UIFONTCOLOR,
	PREFPAGE_TASK,
	PREFPAGE_TASKCALC,
	PREFPAGE_TASKDEF,
	PREFPAGE_IMPEXP,
	PREFPAGE_TOOL,
	PREFPAGE_SHORTCUT
};

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg dialog

class CPreferencesDlg : public CPreferencesDlgBase
{
// Construction
public:
	CPreferencesDlg(CShortcutManager* pShortcutMgr = NULL, UINT nMenuID = 0,
		const CContentMgr* pContentMgr = NULL, const CImportExportMgr* pExportMgr = NULL,
		CWnd* pParent = NULL);   // standard constructor
	virtual ~CPreferencesDlg();

	BOOL GetAlwaysOnTop() const
	{
		return m_pageGen.GetAlwaysOnTop();
	}
	BOOL GetUseSysTray() const
	{
		return m_pageGen.GetUseSysTray();
	}
	BOOL GetConfirmDelete() const
	{
		return m_pageGen.GetConfirmDelete();
	}
	BOOL GetConfirmSaveOnExit() const
	{
		return m_pageGen.GetConfirmSaveOnExit();
	}
	BOOL GetShowOnStartup() const
	{
		return m_pageGen.GetShowOnStartup();
	}
	int GetSysTrayOption() const
	{
		return m_pageGen.GetSysTrayOption();
	}
	BOOL GetToggleTrayVisibility() const
	{
		return m_pageGen.GetToggleTrayVisibility();
	}
	BOOL GetMultiInstance() const
	{
		return m_pageGen.GetMultiInstance();
	}
	BOOL GetGlobalHotkey() const
	{
		return m_pageGen.GetGlobalHotkey();
	}
	BOOL GetAddFilesToMRU() const
	{
		return m_pageGen.GetAddFilesToMRU();
	}
	BOOL GetEnableTDLExtension() const
	{
		return m_pageGen.GetEnableTDLExtension();
	}
	BOOL GetEnableTDLProtocol() const
	{
		return m_pageGen.GetEnableTDLProtocol();
	}
	BOOL GetAutoCheckForUpdates() const
	{
		return m_pageGen.GetAutoCheckForUpdates();
	}
	BOOL GetEscapeMinimizes() const
	{
		return m_pageGen.GetEscapeMinimizes();
	}
	BOOL GetEnableDelayedLoading() const
	{
		return m_pageGen.GetEnableDelayedLoading();
	}

	int GetReadonlyReloadOption() const
	{
		return m_pageMultiUser.GetReadonlyReloadOption();
	}
	int GetTimestampReloadOption() const
	{
		return m_pageMultiUser.GetTimestampReloadOption();
	}
	BOOL GetEnableSourceControl() const
	{
		return m_pageMultiUser.GetEnableSourceControl();
	}
	BOOL GetSourceControlLanOnly() const
	{
		return m_pageMultiUser.GetSourceControlLanOnly();
	}
	BOOL GetAutoCheckOut() const
	{
		return m_pageMultiUser.GetAutoCheckOut();
	}
	BOOL GetCheckoutOnCheckin() const
	{
		return m_pageMultiUser.GetCheckoutOnCheckin();
	}
	BOOL GetCheckinOnClose() const
	{
		return m_pageMultiUser.GetCheckinOnClose();
	}
	UINT GetRemoteFileCheckFrequency() const
	{
		return m_pageMultiUser.GetRemoteFileCheckFrequency();
	}
	UINT GetCheckinOnNoEditTime() const
	{
		return m_pageMultiUser.GetCheckinOnNoEditTime();
	}
	BOOL GetUsing3rdPartySourceControl() const
	{
		return m_pageMultiUser.GetUsing3rdPartySourceControl();
	}

	int GetNotifyDueByOnLoad() const
	{
		return m_pageFile.GetNotifyDueByOnLoad();
	}
	int GetNotifyDueByOnSwitch() const
	{
		return m_pageFile.GetNotifyDueByOnSwitch();
	}
	BOOL GetDisplayDueTasksInHtml() const
	{
		return m_pageFile.GetDisplayDueTasksInHtml();
	}
	BOOL GetAutoArchive() const
	{
		return m_pageFile.GetAutoArchive();
	}
	BOOL GetNotifyReadOnly() const
	{
		return m_pageFile.GetNotifyReadOnly();
	}
	BOOL GetRemoveArchivedTasks() const
	{
		return m_pageFile.GetRemoveArchivedTasks();
	}
	BOOL GetRemoveOnlyOnAbsoluteCompletion() const
	{
		return m_pageFile.GetRemoveOnlyOnAbsoluteCompletion();
	}
	BOOL GetRefreshFindOnLoad() const
	{
		return m_pageFile.GetRefreshFindOnLoad();
	}
	BOOL GetDueTaskTitlesOnly() const
	{
		return m_pageFile.GetDueTaskTitlesOnly();
	}
	CString GetDueTaskStylesheet() const
	{
		return m_pageFile.GetDueTaskStylesheet();
	}
	CString GetDueTaskPerson() const
	{
		return m_pageFile.GetDueTaskPerson();
	}
	BOOL GetWarnAddDeleteArchive() const
	{
		return m_pageFile.GetWarnAddDeleteArchive();
	}
	BOOL GetDontRemoveFlagged() const
	{
		return m_pageFile.GetDontRemoveFlagged();
	}
	BOOL GetExpandTasksOnLoad() const
	{
		return m_pageFile.GetExpandTasksOnLoad();
	}

	BOOL GetBackupOnSave() const
	{
		return m_pageFile2.GetBackupOnSave();
	}
	CString GetBackupLocation() const
	{
		return m_pageFile2.GetBackupLocation();
	}
	int GetKeepBackupCount() const
	{
		return m_pageFile2.GetKeepBackupCount();
	}
	BOOL GetAutoExport() const
	{
		return m_pageFile2.GetAutoExport();
	}
	BOOL GetExportToHTML() const
	{
		return m_pageFile2.GetExportToHTML();
	}
	int GetOtherExporter() const
	{
		return m_pageFile2.GetOtherExporter();
	}
	int GetAutoSaveFrequency() const
	{
		return m_pageFile2.GetAutoSaveFrequency();
	}
	CString GetAutoExportFolderPath() const
	{
		return m_pageFile2.GetAutoExportFolderPath();
	}
	CString GetSaveExportStylesheet() const
	{
		return m_pageFile2.GetSaveExportStylesheet();
	}
	BOOL GetAutoSaveOnSwitchTasklist() const
	{
		return m_pageFile2.GetAutoSaveOnSwitchTasklist();
	}
	BOOL GetAutoSaveOnSwitchApp() const
	{
		return m_pageFile2.GetAutoSaveOnSwitchApp();
	}
	BOOL GetExportFilteredOnly() const
	{
		return m_pageFile2.GetExportFilteredOnly();
	}

	CString GetHtmlFont() const
	{
		return m_pageExport.GetHtmlFont();
	}
	int GetHtmlFontSize() const
	{
		return m_pageExport.GetHtmlFontSize();
	}
	BOOL GetPreviewExport() const
	{
		return m_pageExport.GetPreviewExport();
	}
	int GetTextIndent() const
	{
		return m_pageExport.GetTextIndent();
	}
	BOOL GetExportParentTitleCommentsOnly() const
	{
		return m_pageExport.GetExportParentTitleCommentsOnly();
	}
	BOOL GetExportSpaceForNotes() const
	{
		return m_pageExport.GetExportSpaceForNotes();
	}
	BOOL GetExportVisibleColsOnly() const
	{
		return m_pageExport.GetExportVisibleColsOnly();
	}
	BOOL GetExportAllAttributes() const
	{
		return !GetExportVisibleColsOnly();
	}
	CString GetHtmlCharSet() const
	{
		return m_pageExport.GetHtmlCharSet();
	}

	int GetDefaultPriority() const
	{
		return m_pageTaskDef.GetDefaultPriority();
	}
	int GetDefaultRisk() const
	{
		return m_pageTaskDef.GetDefaultRisk();
	}
	int GetDefaultAllocTo(CStringArray& aAllocTo) const
	{
		return m_pageTaskDef.GetDefaultAllocTo(aAllocTo);
	}
	CString GetDefaultAllocBy() const
	{
		return m_pageTaskDef.GetDefaultAllocBy();
	}
	CString GetDefaultStatus() const
	{
		return m_pageTaskDef.GetDefaultStatus();
	}
	int GetDefaultCategories(CStringArray& aCats) const
	{
		return m_pageTaskDef.GetDefaultCategories(aCats);
	}
	CString GetDefaultCreatedBy() const
	{
		return m_pageTaskDef.GetDefaultCreatedBy();
	}
	double GetDefaultTimeEst(int& nUnits) const
	{
		return m_pageTaskDef.GetDefaultTimeEst(nUnits);
	}
	double GetDefaultTimeSpent(int& nUnits) const
	{
		return m_pageTaskDef.GetDefaultTimeSpent(nUnits);
	}
	COLORREF GetDefaultColor() const
	{
		return m_pageTaskDef.GetDefaultColor();
	}
	BOOL GetAutoDefaultStartDate() const
	{
		return m_pageTaskDef.GetAutoDefaultStartDate();
	}
	int GetParentAttribsUsed(CTDCAttributeArray& aAttribs, BOOL& bUpdateAttrib) const
	{
		return m_pageTaskDef.GetParentAttribsUsed(aAttribs, bUpdateAttrib);
	}
	int GetDefaultListItems(PTDP_LIST nList, CStringArray& aItems) const
	{
		return m_pageTaskDef.GetListItems(nList, aItems);
	}
	double GetDefaultCost() const
	{
		return m_pageTaskDef.GetDefaultCost();
	}

	BOOL GetTrackNonActiveTasklists() const
	{
		return m_pageTask.GetTrackNonActiveTasklists();
	}
	BOOL GetTrackNonSelectedTasks() const
	{
		return m_pageTask.GetTrackNonSelectedTasks();
	}
	BOOL GetTrackOnScreenSaver() const
	{
		return m_pageTask.GetTrackOnScreenSaver();
	}
	BOOL GetTrackHibernated() const
	{
		return m_pageTask.GetTrackHibernated();
	}
	double GetHoursInOneDay() const
	{
		return m_pageTask.GetHoursInOneDay();
	}
	double GetDaysInOneWeek() const
	{
		return m_pageTask.GetDaysInOneWeek();
	}
	BOOL GetLogTimeTracking() const
	{
		return m_pageTask.GetLogTimeTracking();
	}
	BOOL GetLogTaskTimeSeparately() const
	{
		return m_pageTask.GetLogTaskTimeSeparately();
	}
	BOOL GetExclusiveTimeTracking() const
	{
		return m_pageTask.GetExclusiveTimeTracking();
	}
	BOOL GetAllowParentTimeTracking() const
	{
		return m_pageTask.GetAllowParentTimeTracking();
	}

	BOOL GetAveragePercentSubCompletion() const
	{
		return m_pageTaskCalc.GetAveragePercentSubCompletion();
	}
	BOOL GetIncludeDoneInAverageCalc() const
	{
		return m_pageTaskCalc.GetIncludeDoneInAverageCalc();
	}
	BOOL GetUseEarliestDueDate() const
	{
		return m_pageTaskCalc.GetUseEarliestDueDate();
	}
	BOOL GetUsePercentDoneInTimeEst() const
	{
		return m_pageTaskCalc.GetUsePercentDoneInTimeEst();
	}
	BOOL GetTreatSubCompletedAsDone() const
	{
		return m_pageTaskCalc.GetTreatSubCompletedAsDone();
	}
	BOOL GetUseHighestPriority() const
	{
		return m_pageTaskCalc.GetUseHighestPriority();
	}
	BOOL GetUseHighestRisk() const
	{
		return m_pageTaskCalc.GetUseHighestRisk();
	}
	BOOL GetDueTasksHaveHighestPriority() const
	{
		return m_pageTaskCalc.GetDueTasksHaveHighestPriority();
	}
	BOOL GetDoneTasksHaveLowestPriority() const
	{
		return m_pageTaskCalc.GetDoneTasksHaveLowestPriority();
	}
	BOOL GetDoneTasksHaveLowestRisk() const
	{
		return m_pageTaskCalc.GetDoneTasksHaveLowestRisk();
	}
	BOOL GetAutoCalcTimeEstimates() const
	{
		return m_pageTaskCalc.GetAutoCalcTimeEstimates();
	}
	BOOL GetIncludeDoneInPriorityRiskCalc() const
	{
		return m_pageTaskCalc.GetIncludeDoneInPriorityRiskCalc();
	}
	BOOL GetWeightPercentCompletionByNumSubtasks() const
	{
		return m_pageTaskCalc.GetWeightPercentCompletionByNumSubtasks();
	}
	BOOL GetAutoCalcPercentDone() const
	{
		return m_pageTaskCalc.GetAutoCalcPercentDone();
	}
	BOOL GetAutoAdjustDependents() const
	{
		return m_pageTaskCalc.GetAutoAdjustDependents();
	}
	BOOL GetNoDueDateIsDueToday() const
	{
		return m_pageTaskCalc.GetNoDueDateIsDueToday();
	}
	int GetTimeRemainingCalculation() const
	{
		return m_pageTaskCalc.GetTimeRemainingCalculation();
	}

	BOOL GetShowCtrlsAsColumns() const
	{
		return m_pageUI.GetShowCtrlsAsColumns();
	}
	BOOL GetShowEditMenuAsColumns() const
	{
		return m_pageUI.GetShowEditMenuAsColumns();
	}
	BOOL GetShowCommentsAlways() const
	{
		return m_pageUI.GetShowCommentsAlways();
	}
	BOOL GetAutoReposCtrls() const
	{
		return m_pageUI.GetAutoReposCtrls();
	}
	BOOL GetSharedCommentsHeight() const
	{
		return m_pageUI.GetSharedCommentsHeight();
	}
	BOOL GetAutoHideTabbar() const
	{
		return m_pageUI.GetAutoHideTabbar();
	}
	BOOL GetStackTabbarItems() const
	{
		return m_pageUI.GetStackTabbarItems();
	}
	BOOL GetRightAlignLabels() const
	{
		return m_pageUI.GetRightAlignLabels();
	}
	BOOL GetFocusTreeOnEnter() const
	{
		return m_pageUI.GetFocusTreeOnEnter();
	}
	int GetNewTaskPos() const
	{
		return m_pageUI.GetNewTaskPos();
	}
	int GetNewSubtaskPos() const
	{
		return m_pageUI.GetNewSubtaskPos();
	}
	BOOL GetKeepTabsOrdered() const
	{
		return m_pageUI.GetKeepTabsOrdered();
	}
	BOOL GetShowTasklistCloseButton() const
	{
		return m_pageUI.GetShowTasklistCloseButton();
	}
	BOOL GetSortDoneTasksAtBottom() const
	{
		return m_pageUI.GetSortDoneTasksAtBottom();
	}
	BOOL GetRTLComments() const
	{
		return m_pageUI.GetRTLComments();
	}
	int GetCommentsPos() const
	{
		return m_pageUI.GetCommentsPos();
	}
	CONTENTFORMAT GetDefaultCommentsFormat() const
	{
		return m_pageUI.GetDefaultCommentsFormat();
	}
	BOOL GetMultiSelFilters() const
	{
		return m_pageUI.GetMultiSelFilters();
	}
	BOOL GetRestoreTasklistFilters() const
	{
		return m_pageUI.GetRestoreTasklistFilters();
	}
	BOOL GetReFilterOnModify() const
	{
		return m_pageUI.GetReFilterOnModify();
	}
	CString GetUITheme() const
	{
		return m_pageUI.GetUITheme();
	}
	BOOL GetEnableLightboxMgr() const
	{
		return m_pageUI.GetEnableLightboxMgr();
	}

	BOOL GetShowInfoTips() const
	{
		return m_pageUITasklist.GetShowInfoTips();
	}
	BOOL GetShowComments() const
	{
		return m_pageUITasklist.GetShowComments();
	}
	int GetVisibleColumns(CTDCColumnArray& aColumns) const
	{
		return m_pageUITasklist.GetVisibleColumns(aColumns);
	}
	void SetVisibleColumns(const CTDCColumnArray& aColumns)
	{
		m_pageUITasklist.SetVisibleColumns(aColumns);
	}
	BOOL GetShowPathInHeader() const
	{
		return m_pageUITasklist.GetShowPathInHeader();
	}
	BOOL GetStrikethroughDone() const
	{
		return m_pageUITasklist.GetStrikethroughDone();
	}
	BOOL GetFullRowSelection() const
	{
		return m_pageUITasklist.GetFullRowSelection();
	}
	BOOL GetTreeCheckboxes() const
	{
		return m_pageUITasklist.GetTreeCheckboxes();
	}
	BOOL GetTreeTaskIcons() const
	{
		return m_pageUITasklist.GetTreeTaskIcons();
	}
	BOOL GetDisplayDatesInISO() const
	{
		return m_pageUITasklist.GetDisplayDatesInISO();
	}
	BOOL GetShowWeekdayInDates() const
	{
		return m_pageUITasklist.GetShowWeekdayInDates();
	}
	BOOL GetShowParentsAsFolders() const
	{
		return m_pageUITasklist.GetShowParentsAsFolders();
	}
	BOOL GetDisplayFirstCommentLine() const
	{
		return m_pageUITasklist.GetDisplayFirstCommentLine();
	}
	int GetMaxInfoTipCommentsLength() const
	{
		return m_pageUITasklist.GetMaxInfoTipCommentsLength();
	}
	int GetMaxColumnWidth() const
	{
		return m_pageUITasklist.GetMaxColumnWidth();
	}
	BOOL GetHidePercentForDoneTasks() const
	{
		return m_pageUITasklist.GetHidePercentForDoneTasks();
	}
	BOOL GetHideZeroTimeCost() const
	{
		return m_pageUITasklist.GetHideZeroTimeCost();
	}
	BOOL GetHideStartDueForDoneTasks() const
	{
		return m_pageUITasklist.GetHideStartDueForDoneTasks();
	}
	BOOL GetShowPercentAsProgressbar() const
	{
		return m_pageUITasklist.GetShowPercentAsProgressbar();
	}
	BOOL GetRoundTimeFractions() const
	{
		return m_pageUITasklist.GetRoundTimeFractions();
	}
	BOOL GetShowNonFilesAsText() const
	{
		return m_pageUITasklist.GetShowNonFilesAsText();
	}
	BOOL GetUseHMSTimeFormat() const
	{
		return m_pageUITasklist.GetUseHMSTimeFormat();
	}
	BOOL GetAutoFocusTasklist() const
	{
		return m_pageUITasklist.GetAutoFocusTasklist();
	}
	BOOL GetShowSubtaskCompletion() const
	{
		return m_pageUITasklist.GetShowSubtaskCompletion();
	}
	BOOL GetShowColumnsOnRight() const
	{
		return m_pageUITasklist.GetShowColumnsOnRight();
	}
	BOOL GetHideDueTimeField() const
	{
		return m_pageUITasklist.GetHideDueTimeField();
	}
	BOOL GetHideStartTimeField() const
	{
		return m_pageUITasklist.GetHideStartTimeField();
	}
	BOOL GetHideDoneTimeField() const
	{
		return m_pageUITasklist.GetHideDoneTimeField();
	}

	int GetTextColorOption() const
	{
		return m_pageUITasklistColors.GetTextColorOption();
	}
	BOOL GetColorTaskBackground() const
	{
		return m_pageUITasklistColors.GetColorTaskBackground();
	}
	BOOL GetCommentsUseTreeFont() const
	{
		return m_pageUITasklistColors.GetCommentsUseTreeFont();
	}
	BOOL GetColorPriority() const
	{
		return m_pageUITasklistColors.GetColorPriority();
	}
	int GetPriorityColors(CDWordArray& aColors) const
	{
		return m_pageUITasklistColors.GetPriorityColors(aColors);
	}
	void GetDueTaskColors(COLORREF& crDue, COLORREF& crDueToday) const
	{
		m_pageUITasklistColors.GetDueTaskColors(crDue, crDueToday);
	}
	BOOL GetTreeFont(CString& sFaceName, int& nPointSize) const
	{
		return m_pageUITasklistColors.GetTreeFont(sFaceName, nPointSize);
	}
	BOOL GetCommentsFont(CString& sFaceName, int& nPointSize) const
	{
		return m_pageUITasklistColors.GetCommentsFont(sFaceName, nPointSize);
	}
	COLORREF GetGridlineColor() const
	{
		return m_pageUITasklistColors.GetGridlineColor();
	}
	COLORREF GetDoneTaskColor() const
	{
		return m_pageUITasklistColors.GetDoneTaskColor();
	}
	COLORREF GetFlaggedTaskColor() const
	{
		return m_pageUITasklistColors.GetFlaggedTaskColor();
	}
	COLORREF GetHidePriorityNumber() const
	{
		return m_pageUITasklistColors.GetHidePriorityNumber();
	}
	COLORREF GetAlternateLineColor() const
	{
		return m_pageUITasklistColors.GetAlternateLineColor();
	}
	int GetCategoryColors(CCatColorArray& aColors) const
	{
		return m_pageUITasklistColors.GetCategoryColors(aColors);
	}

	int GetUserTools(CUserToolArray& aTools) const
	{
		return m_pageTool.GetUserTools(aTools);
	}
	BOOL GetUserTool(int nTool, USERTOOL& tool) const
	{
		return m_pageTool.GetUserTool(nTool, tool);
	}

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesDlg)
	enum { IDD = IDD_PREFERENCES };
	CTreeCtrl m_tcPages;
	//}}AFX_DATA
	CPreferencesGenPage m_pageGen;
	CPreferencesFilePage m_pageFile;
	CPreferencesFile2Page m_pageFile2;
	CPreferencesExportPage m_pageExport;
	CPreferencesUIPage m_pageUI;
	CPreferencesUITasklistPage m_pageUITasklist;
	CPreferencesUITasklistColorsPage m_pageUITasklistColors;
	CPreferencesTaskPage m_pageTask;
	CPreferencesTaskCalcPage m_pageTaskCalc;
	CPreferencesTaskDefPage m_pageTaskDef;
	CPreferencesToolPage m_pageTool;
	CPreferencesShortcutsPage m_pageShortcuts;
	CPreferencesMultiUserPage m_pageMultiUser;
	CMapPtrToPtr m_mapPP2HTI;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesDlg)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelp();
	afx_msg void OnSelchangedPages(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnApply();
	//}}AFX_MSG
	afx_msg LRESULT OnDefPageListChange(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnColourPageCategoryChange(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToolPageTestTool(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnGenPageClearMRU(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnControlChange(WPARAM wp, LPARAM lp);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	DECLARE_MESSAGE_MAP()

protected:
	void AddPage(CPreferencesPageBase* pPage, UINT nIDPath);
	BOOL SetActivePage(int nPage); // override
	CString GetItemPath(HTREEITEM hti) const;
	void SynchronizeTree();
	BOOL IsPrefTab(HWND hWnd) const;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESDLG_H__C3FCC72A_6C69_49A6_930D_D5C94EC31298__INCLUDED_)
