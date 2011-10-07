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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

#if !defined(AFX_TODOCTRL_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_)
#define AFX_TODOCTRL_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ToDoCtrl.h : header file
//

#include "TaskListDropTarget.h"
#include "ToDoCtrlData.h"
#include "ToDoCtrlFind.h"
#include "TaskFile.h"
#include "TDCStruct.h"
#include "TDLPriorityComboBox.h"
#include "TDLRiskComboBox.h"
#include "RecurringTaskEdit.h"

#include "../../../CodeProject/Source/RuntimeDlg.h"
#include "../../../CodeProject/Source/DialogHelper.h"
#include "../../../CodeProject/Source/OrderedTreeCtrl.h"
#include "../../Common/FileEdit.h"
#include "../../../CodeProject/Source/SysImageList.h"
#include "../../Common/UrlRichEditCtrl.h"
#include "../../../CodeProject/Source/ColorComboBox.h"
#include "../../../CodeProject/Source/AutoComboBox.h"
#include "../../../CodeProject/Source/MaskEdit.h"
#include "../../Common/TimeEdit.h"
#include "../../../CodeProject/Source/TreeDragDropHelper.h"
#include "../../../CodeProject/Source/WndPrompt.h"
#include "../../Common/ContentMgr.h"
#include "../../../CodeProject/Source/CheckComboBox.h"
#include "../../Common/ContentTypeComboBox.h"
#include "../../../CodeProject/Source/TimeComboBox.h"
#include "../../Common/PopupEditCtrl.h"
#include "../../Common/UITheme.h"

#include "../../../CodeProject/Source/ColourPicker.h"

/////////////////////////////////////////////////////////////////////////////
// CToDoCtrl dialog

// predeclarations
class CXmlItem;
class CTaskFile;
class CDeferWndMove;
class CDlgUnits;
struct CTRLITEM;
struct TDCCOLUMN;
class CSpellCheckDlg;
class CPreferences;

class CToDoCtrl : public CRuntimeDlg
{
// Construction
public:
	CToDoCtrl(CContentMgr& mgr, const CONTENTFORMAT& cfDefault);
	virtual ~CToDoCtrl();

	BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL bVisible = TRUE);

	TDC_FILE Save(LPCTSTR szFilePath = NULL);
	TDC_FILE Save(CTaskFile& tasks/*out*/, LPCTSTR szFilePath = NULL);

	TDC_FILE Load(LPCTSTR szFilePath);
	TDC_FILE Load(LPCTSTR szFilePath, CTaskFile& tasks/*out*/);

	BOOL DelayLoad(LPCTSTR szFilePath, COleDateTime& dtEarliestDue);

	BOOL ArchiveDoneTasks(LPCTSTR szFilePath, TDC_ARCHIVE nFlags, BOOL bRemoveFlagged); // returns true if any tasks were removed
	BOOL ArchiveSelectedTasks(LPCTSTR szFilePath, BOOL bRemove); // returns true if any tasks were removed

	void EnableEncryption(BOOL bEnable = TRUE);
	BOOL IsEncrypted() const
	{
		return (!m_sPassword.IsEmpty());
	}
	static BOOL CanEncrypt(); // returns true only if the required encryption capabilities are present
	CString GetPassword() const
	{
		return m_sPassword;
	}
	BOOL VerifyPassword(LPCTSTR szExplanation = NULL) const;

	void SetMaximizeState(TDC_MAXSTATE nState);

	BOOL CheckIn();
	BOOL CheckOut();
	BOOL CheckOut(CString& sCheckedOutTo);
	BOOL IsCheckedOut() const
	{
		return m_bCheckedOut;
	}
	BOOL IsSourceControlled() const
	{
		return m_bSourceControlled;
	}
	BOOL RemoveFromSourceControl();

	void Flush(BOOL bEndTimeTracking = FALSE); // called to end current editing actions
	BOOL IsModified() const
	{
		return (m_bModified || m_bCommentsChange) && !IsReadOnly();
	}
	void SetModified(BOOL bMod = TRUE);

	CString GetFilePath() const
	{
		return m_sLastSavePath;
	}
	void ClearFilePath()
	{
		m_sLastSavePath.Empty();
	}
	CString GetProjectName() const
	{
		return m_sProjectName;
	}
	void SetProjectName(LPCTSTR szProjectName);
	CString GetFriendlyProjectName(int nUntitledIndex = -1) const;
	void SetFilePath(LPCTSTR szPath)
	{
		m_sLastSavePath = szPath;
	}

	BOOL DeleteAllTasks();
	void NewList();

	int GetTasks(CTaskFile& tasks, const TDCGETTASKS& filter = TDCGETTASKS(TDCGT_ALL)) const;
	int GetSelectedTasks(CTaskFile& tasks, const TDCGETTASKS& filter = TDCGETTASKS(TDCGT_ALL)) const;

	BOOL InsertTasks(const CTaskFile& tasks, TDC_INSERTWHERE nWhere);

	void SetReadonly(BOOL bReadOnly)
	{
		SetStyle(TDCS_READONLY, bReadOnly);
	}
	BOOL IsReadOnly() const
	{
		return HasStyle(TDCS_READONLY);
	}

	BOOL SetStyles(const CTDCStylesMap& styles);
	BOOL SetStyle(TDC_STYLE nStyle, BOOL bOn = TRUE); // one style at a time only
	BOOL HasStyle(TDC_STYLE nStyle) const; // one style at a time only

	void SetVisibleColumns(const CTDCColumnArray& aColumns);
	int GetVisibleColumns(CTDCColumnArray& aColumns) const;
	BOOL IsColumnShowing(TDC_COLUMN nColumn) const;
	void SetMaxColumnWidth(int nWidth);

	BOOL SetPriorityColors(const CDWordArray& aColors); // must have 12 elements
	COLORREF GetPriorityColor(int nPriority) const;
	void SetGridlineColor(COLORREF color)
	{
		m_tree.SetGridlineColor(color);
	}
	void SetCompletedTaskColor(COLORREF color);
	void SetFlaggedTaskColor(COLORREF color);
	void SetCategoryColors(const CTDCColorMap& colors);
	BOOL GetCategoryColor(LPCTSTR szCategory, COLORREF& color) const;
	void SetDueTaskColors(COLORREF crDue, COLORREF crDueToday);
	void GetDueTaskColors(COLORREF& crDue, COLORREF& crDueToday)
	{
		crDue = m_crDue;
		crDueToday = m_crDueToday;
	}
	void SetAlternateLineColor(COLORREF color)
	{
		m_tree.SetAlternateLineColor(color);
	}

	void SetUITheme(const UITHEME& theme);

	// these return the full list of items in each droplist
	int GetAllocToNames(CStringArray& aNames) const
	{
		return m_cbAllocTo.GetItems(aNames);
	}
	int GetAllocByNames(CStringArray& aNames) const
	{
		return m_cbAllocBy.GetItems(aNames);
	}
	int GetCategoryNames(CStringArray& aNames) const
	{
		return m_cbCategory.GetItems(aNames);
	}
	int GetStatusNames(CStringArray& aNames) const
	{
		return m_cbStatus.GetItems(aNames);
	}
	int GetVersionNames(CStringArray& aNames) const
	{
		return m_cbVersion.GetItems(aNames);
	}

	void SetDefaultAllocToNames(const CStringArray& aNames);
	void SetDefaultAllocByNames(const CStringArray& aNames);
	void SetDefaultCategoryNames(const CStringArray& aNames);
	void SetDefaultStatusNames(const CStringArray& aNames);

	HTREEITEM NewTask(LPCTSTR szText, TDC_INSERTWHERE nWhere = TDC_INSERTATTOPOFSELTASKPARENT, BOOL bEditText = TRUE);
	void SetSubtaskDragDropPos(BOOL bTop = TRUE)
	{
		m_bDragDropSubtasksAtTop = bTop;
	}

	virtual BOOL SplitSelectedTask(int nNumSubtasks = 2);
	virtual BOOL CanSplitSelectedTask();

	DWORD GetSelectedTaskID() const
	{
		return m_find.GetTaskID(GetSelectedItem());
	}
	TODOITEM* GetSelectedTask() const
	{
		return m_find.GetTask(GetSelectedItem());
	}
	virtual BOOL SelectTask(DWORD dwTaskID);

	virtual BOOL IsTaskDone(DWORD dwTaskID) const
	{
		return m_data.IsTaskDone(dwTaskID);
	}
	BOOL IsTaskDone(DWORD dwTaskID, DWORD dwExtraCheck) const
	{
		return m_data.IsTaskDone(dwTaskID, dwExtraCheck);
	}
	virtual BOOL IsTaskRecurring(DWORD dwTaskID) const;

	BOOL DeleteSelectedTask();
	BOOL EditSelectedTask(BOOL bTaskIsNew = FALSE);
	void SpellcheckSelectedTask(BOOL bTitle); // else comments
	BOOL CanSpellcheckSelectedTaskComments();

	BOOL EditSelectedTaskIcon();
	BOOL ClearSelectedTaskIcon();

	BOOL SetSelectedTaskDone(BOOL bDone = TRUE);
	BOOL IsSelectedTaskDone() const;
	BOOL IsSelectedTaskDue() const;
	BOOL OffsetSelectedTaskDate(TDC_DATE nDate, int nAmount, int nUnits, BOOL bAndSubtasks);
	COleDateTime GetEarliestDueDate() const
	{
		return m_data.GetEarliestDueDate();   // entire tasklist
	}

	COLORREF GetSelectedTaskColor() const; // -1 on no item selected
	int GetSelectedTaskIconIndex() const; // -1 on no item selected
	CString GetSelectedTaskComments() const;
	CString GetSelectedTaskCustomComments(CString& sCommentsTypeID) const;
	CString GetSelectedTaskTitle() const;
	double GetSelectedTaskTimeEstimate(int& nUnits) const;
	double GetSelectedTaskTimeSpent(int& nUnits) const;
	int GetSelectedTaskAllocTo(CStringArray& aAllocTo) const;
	CString GetSelectedTaskAllocBy() const;
	CString GetSelectedTaskStatus() const;
	int GetSelectedTaskCategories(CStringArray& aCats) const;
	int GetSelectedTaskDependencies(CStringArray& aDepends) const;
	CString GetSelectedTaskFileRef() const;
	CString GetSelectedTaskExtID() const;
	int GetSelectedTaskPercent() const;
	int GetSelectedTaskPriority() const;
	int GetSelectedTaskRisk() const;
	double GetSelectedTaskCost() const;
	BOOL IsSelectedTaskFlagged() const;
	BOOL GetSelectedTaskRecurrence(TDIRECURRENCE& tr) const;
	CString GetSelectedTaskVersion() const;
	BOOL SelectedTaskHasDate(TDC_DATE nDate) const;

	CString GetTaskPath(DWORD dwTaskID, BOOL bIncludeTaskName) const
	{
		return m_data.GetTaskPath(dwTaskID, bIncludeTaskName);
	}
	CString GetTaskTitle(DWORD dwTaskID) const
	{
		return m_data.GetTaskTitle(dwTaskID);
	}
	COleDateTime GetTaskDate(DWORD dwID, TDC_DATE nDate) const;

	double CalcSelectedTaskTimeEstimate(int nUnits = TDITU_HOURS) const;
	double CalcSelectedTaskTimeSpent(int nUnits = TDITU_HOURS) const;
	double CalcSelectedTaskCost() const;

	BOOL SetSelectedTaskColor(COLORREF color);
	BOOL ClearSelectedTaskColor()
	{
		return SetSelectedTaskColor((COLORREF)-1);
	}
	BOOL SetSelectedTaskTitle(LPCTSTR szTitle);
	BOOL SetSelectedTaskPercentDone(int nPercent);
	BOOL SetSelectedTaskTimeEstimate(double dTime, int nUnits = TDITU_HOURS);
	BOOL SetSelectedTaskTimeSpent(double dTime, int nUnits = TDITU_HOURS);
	BOOL SetSelectedTaskTimeEstimateUnits(int nUnits, BOOL bRecalcTime = FALSE);
	BOOL SetSelectedTaskTimeSpentUnits(int nUnits, BOOL bRecalcTime = FALSE);
	BOOL SetSelectedTaskAllocTo(const CStringArray& aAllocTo);
	BOOL SetSelectedTaskAllocBy(LPCTSTR szAllocBy);
	BOOL SetSelectedTaskStatus(LPCTSTR szStatus);
	BOOL SetSelectedTaskCategories(const CStringArray& aCats);
	BOOL SetSelectedTaskDependencies(const CStringArray& aDepends);
	BOOL SetSelectedTaskPriority(int nPriority); // 0-10 (10 is highest)
	BOOL SetSelectedTaskRisk(int nRisk); // 0-10 (10 is highest)
	BOOL SetSelectedTaskFileRef(LPCTSTR szFilePath);
	BOOL SetSelectedTaskExtID(LPCTSTR szID);
	BOOL SetSelectedTaskFlag(BOOL bFlagged);
	BOOL SetSelectedTaskCost(double dCost);
	BOOL SetSelectedTaskRecurrence(const TDIRECURRENCE& tr);
	BOOL SetSelectedTaskVersion(LPCTSTR szVersion);
	BOOL SetSelectedTaskComments(LPCTSTR szComments, const CString& sCustomComments);

	BOOL CanPasteText(); // into focused control
	BOOL PasteText(LPCTSTR szText); // into focused control

	BOOL IncrementSelectedTaskPercentDone(int nAmount); // +ve or -ve
	BOOL IncrementSelectedTaskPriority(int nAmount); // +ve or -ve

	BOOL GotoSelectedTaskFileRef();
	CString GetSelectedTaskPath(BOOL bIncludeTaskName) const
	{
		return GetTaskPath(GetSelectedTaskID(), bIncludeTaskName);
	}

	// time tracking
	void PauseTimeTracking(BOOL bPause = TRUE)
	{
		m_bTimeTrackingPaused = bPause;
	}
	BOOL TimeTrackSelectedTask();
	BOOL CanTimeTrackSelectedTask() const;
	BOOL IsSelectedTaskBeingTimeTracked() const;
	BOOL IsActivelyTimeTracking() const; // this instant
	CString GetSelectedTaskTimeLogPath() const;
	void EndTimeTracking();
	void ResetTimeTracking()
	{
		m_dwTickLast = GetTickCount();
	}
	void AddTimeToTaskLogFile(DWORD dwTaskID, double dHours, const COleDateTime& dtWhen, BOOL bAddToTimeSpent = FALSE);

	static void SetInheritedParentAttributes(const CTDCAttributeArray& aAttribs, BOOL bUpdateAttrib);
	void SetDefaultTaskAttributes(const TODOITEM& tdi);

	// sort functions
	virtual void Sort(TDC_SORTBY nBy, BOOL bAllowToggle = TRUE); // calling twice with the same param will toggle ascending attrib
	virtual void MultiSort(const TDSORTCOLUMNS& sort);
	virtual TDC_SORTBY GetSortBy() const
	{
		return m_sort.nBy1;
	}
	virtual void GetSortBy(TDSORTCOLUMNS& sort) const;
	void Resort(BOOL bAllowToggle = FALSE);
	BOOL IsSortable() const;
	virtual BOOL IsMultiSorting() const;
	static int CALLBACK SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK SortFuncMulti(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int SortTasks(LPARAM lParam1, LPARAM lParam2, TDC_SORTBY nSortBy, BOOL bAscending, BOOL bSortDueTodayHigh,
		DWORD dwTimeTrackedID, const CToDoCtrlData* pData);

	// move functions
	virtual BOOL MoveSelectedTask(TDC_MOVETASK nDirection);
	virtual BOOL CanMoveSelectedTask(TDC_MOVETASK nDirection) const;

	virtual BOOL GotoNextTask(TDC_GOTO nDirection);
	virtual BOOL CanGotoNextTask(TDC_GOTO nDirection) const;
	virtual BOOL GotoNextTopLevelTask(TDC_GOTO nDirection);
	virtual BOOL CanGotoNextTopLevelTask(TDC_GOTO nDirection) const;

	virtual BOOL CanExpandSelectedTask(BOOL bExpand) const;
	virtual void ExpandSelectedTask(BOOL bExpand = TRUE);
	virtual void ExpandAllTasks(BOOL bExpand = TRUE);

	// copy/paste functions
	BOOL CutSelectedTask();
	BOOL CopySelectedTask() const;
	void ClearCopiedItem() const;
	BOOL PasteTasks(TDC_PASTE nWhere);
	BOOL CanPaste() const;
	static void ReleaseClipboard()
	{
		s_clipboard.tasks.Reset();
	}

	void CToDoCtrl::ResetFileVersion(unsigned int nTo = 0)
	{
		m_nFileVersion = max(nTo, 0);
	}
	DWORD CToDoCtrl::GetFileVersion() const
	{
		return m_nFileVersion == 0 ? 1 : m_nFileVersion;
	}
	TDC_FILEFMT CompareFileFormat() const; // older, same, newer

	// tree related
	HTREEITEM GetSelectedItem() const;
	UINT GetTaskCount() const
	{
		return m_data.GetTaskCount();
	}
	BOOL ItemHasChildren(HTREEITEM hti) const
	{
		return m_tree.ItemHasChildren(hti);
	}
	BOOL ItemHasParent(HTREEITEM hti) const
	{
		return (NULL != m_tree.GetParentItem(hti));
	}
	int GetSelectedCount() const
	{
		return Selection().GetCount();
	}
	BOOL IsItemSelected(HTREEITEM hti) const
	{
		return Selection().HasItem(hti);
	}
	BOOL HasSelection() const
	{
		return GetSelectedCount();
	}
	BOOL IsTaskLabelEditing() const;

	virtual BOOL TasksHaveFocus() const
	{
		return (::GetFocus() == m_tree);
	}
	virtual void SetFocusToTasks();
	virtual void SetFocusToComments();
	virtual CString GetControlDescription(const CWnd* pCtrl) const;

	void SelectItem(HTREEITEM hti);
	void SelectAll();
	void DeselectAll(); // call externally only
	BOOL MultiSelectItems(const CDWordArray& aTasks, TSH_SELECT nState = TSHS_SELECT, BOOL bRedraw = TRUE);
	BOOL MultiSelectItem(HTREEITEM hti, TSH_SELECT nState = TSHS_SELECT, BOOL bRedraw = TRUE);
	BOOL MultiSelectItems(HTREEITEM htiFrom, HTREEITEM htiTo, TSH_SELECT nState = TSHS_SELECT, BOOL bRedraw = TRUE);

	BOOL SelectTask(CString sPart, TDC_SELECTTASK nSlect);

	BOOL CanSelectNextTasksInHistory() const
	{
		return Selection().HasNextSelection();
	}
	void SelectNextTasksInHistory();
	BOOL CanSelectPrevTasksInHistory() const
	{
		return Selection().HasPrevSelection();
	}
	void SelectPrevTasksInHistory();

	BOOL SetCheckImageList(HIMAGELIST hImageList); // setter responsible for deleting
	BOOL SetTreeFont(HFONT hFont); // setter responsible for deleting
	BOOL SetCommentsFont(HFONT hFont); // setter responsible for deleting

	int FindTasks(const SEARCHPARAMS& params, CResultArray& aResults) const;
	DWORD FindFirstTask(const SEARCHPARAMS& params, SEARCHRESULT& result) const;
	DWORD FindNextTask(const SEARCHPARAMS& params, SEARCHRESULT& result) const;

	// undo/redo
	BOOL UndoLastAction(BOOL bUndo);
	BOOL CanUndoLastAction(BOOL bUndo) const;

	// misc
	int GetMinWidth();
	void Spellcheck();
	void SetMaxInfotipCommentsLength(int nLength)
	{
		m_nMaxInfotipCommentsLength = max(-1, nLength);   // -1 to switch off
	}
	COleDateTime GetLastTaskModified()
	{
		return m_tLastTaskMod;
	}
	BOOL PtInHeader(CPoint ptScreen) const;
	CString GetPreferencesKey(LPCTSTR szSubKey = NULL, LPCTSTR szFilePath = NULL) const;
	void RedrawReminders() const;

	static void ParseTaskLink(LPCTSTR szLink, DWORD& dwTaskID, CString& sFile);
	static BOOL IsReservedShortcut(DWORD dwShortcut);
	static void EnableExtendedSelection(BOOL bCtrl, BOOL bShift);

protected:
	CDateTimeCtrl m_dateStart, m_dateDue, m_dateDone;
	CTimeComboBox m_timeDue, m_timeStart, m_timeDone;
	CTDLPriorityComboBox m_cbPriority;
	CTDLRiskComboBox m_cbRisk;
	CEnEdit m_eExternalID, m_eDependency;
	CSpinButtonCtrl m_spinPercent;
	CMaskEdit m_ePercentDone, m_eCost;
	CAutoComboBox m_cbAllocBy;
	CAutoComboBox m_cbStatus;
	CCheckComboBox m_cbCategory, m_cbAllocTo;
	CTimeEdit m_eTimeEstimate, m_eTimeSpent;
	CFileEdit m_eFileRef;
	CContentCtrl m_ctrlComments;
	COrderedTreeCtrl m_tree;
	CTDLRecurringTaskEdit m_eRecurrence;
	CAutoComboBox m_cbVersion;
	CContentTypeComboBox m_cbCommentsType;
	CColourPicker m_cpColour;
	CPopupEditCtrl m_eLabel;

	HFONT m_fontTree, m_fontDone, m_fontComments, m_fontBold;
	HIMAGELIST m_hilDone;
	CSysImageList m_ilFileRef;
	CImageList m_ilTaskIcons;
	CBrush m_brDue, m_brDueToday, m_brUIBack;
	COLORREF m_crDue, m_crDueToday, m_crFlagged, m_crDone;
	UITHEME m_theme;

	CToDoCtrlData m_data;
	CToDoCtrlFind m_find;

	TDSORTCOLUMNS m_sort;
	CWordArray m_aStyles;
	CDWordArray m_aPriorityColors;
	CString m_sXmlHeader;
	CTaskListDropTarget m_dtTree, m_dtFileRef;
	CString m_sLastSavePath;
	CString m_sSourceControlID; // for source control
	int m_nCommentsSize;
	CString m_sPassword;
	WORD m_wKeyPress;
	CTreeDragDropHelper m_treeDragDrop;
	CTDCColorMap m_mapCatColors;
	int m_nMaxInfotipCommentsLength;
	TDLSELECTION m_selection;
	CWndPromptManager m_mgrPrompts;
	COleDateTime m_tLastTaskMod;
	CContentMgr& m_mgrContent;
	CTDCColumnArray m_aVisibleColumns;
	CStringArray m_aDefCategory, m_aDefStatus, m_aDefAllocTo, m_aDefAllocBy;
	TDC_MAXSTATE m_nMaxState;

	CString m_sFileRefPath;
	CString m_sTextComments, m_sCustomComments;
	CString m_sAllocBy;
	CString m_sStatus;
	CStringArray m_aCategory, m_aAllocTo;
	CString m_sProjectName;
	CString m_sExternalID, m_sDepends;
	double m_dTimeEstimate, m_dTimeSpent;
	double m_dCost;
	double m_dLogTime; // in hours
	int m_nPriority;
	int m_nRisk;
	int m_nPercentDone;
	int m_nTimeEstUnits, m_nTimeSpentUnits;
	CONTENTFORMAT m_cfComments, m_cfDefault;
	TDIRECURRENCE m_tRecurrence;
	CString m_sOccurrence;
	CString m_sVersion;
	COLORREF m_crColour;
	float m_fAveHeaderCharWidth;
	int m_nMaxColWidth;

	DWORD m_dwNextUniqueID;
	DWORD m_nFileVersion;
	DWORD m_nFileFormat;
	DWORD m_dwTimeTrackTaskID;
	DWORD m_dwTickLast; // time tracking
	DWORD m_dwLastAddedID;
	DWORD m_dwEditingID;

	BOOL m_bModified;
	BOOL m_bArchive;
	BOOL m_bModSinceLastSort;
	BOOL m_bCheckedOut; // intentionally not a style
	BOOL m_bSplitting; // dragging comments splitter
	BOOL m_bTimeTrackingPaused;
	BOOL m_bSourceControlled;
	BOOL m_bDragDropSubtasksAtTop;
	BOOL m_bCommentsChange;
	BOOL m_bDelayLoaded;
	BOOL m_bMultiSort;
	BOOL m_bFirstLoadCommentsPrefs;

	static TDLCLIPBOARD s_clipboard;
	static int s_nCommentsSize; // TDCS_SHAREDCOMMENTSHEIGHT
	static TODOITEM s_tdDefault;
	static short s_nExtendedSelection;
	static CTDCAttributeArray s_aParentAttribs; // inheritable attribs for new tasks
	static BOOL s_bUpdateInheritAttrib; // update as changes are made to parents

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToDoCtrl)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void PreSubclassWindow();
	virtual BOOL OnInitDialog();

// Implementation
protected:
	void UpdateComments(BOOL bSaveAndValidate);

	// Generated message map functions
	//{{AFX_MSG(CToDoCtrl)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnTreeGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatechange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeDblClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeChangeFocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeGetInfoTip(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnTreeDragDrop(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChangePriority();
	afx_msg LRESULT OnChangeComments(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCommentsKillFocus(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChangePercent();
	afx_msg void OnChangeTimeEstimate();
	afx_msg void OnChangeTimeSpent();
	afx_msg void OnSelChangeDueTime();
	afx_msg void OnSelChangeDoneTime();
	afx_msg void OnSelChangeStartTime();
	afx_msg void OnSelChangeAllocTo();
	afx_msg void OnSelChangeAllocBy();
	afx_msg void OnSelChangeStatus();
	afx_msg void OnSelChangeVersion();
	afx_msg void OnSelChangeCategory();
	afx_msg void OnSelCancelAllocTo();
	afx_msg void OnSelCancelCategory();
	afx_msg void OnChangeRisk();
	afx_msg void OnChangeProjectName();
	afx_msg void OnChangeCost();
	afx_msg void OnChangeDependency();
	afx_msg void OnChangeExternalID();
	afx_msg void OnChangeFileRefPath();
	afx_msg void OnChangeRecurrence();
	afx_msg LRESULT OnGutterWantRedraw(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterDrawItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterPostDrawItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterRecalcColWidth(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterNotifyHeaderClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterNotifyScroll(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterWidthChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterGetCursor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterNotifyItemClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterGetItemColors(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEEBtnClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomUrl(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTimeUnitsChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDropFileRef(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFileEditWantIcon(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFileEditDisplayFile(WPARAM wParam, LPARAM lParam);
	afx_msg void OnGotoFileRef();
	afx_msg LRESULT OnTreeRestoreFocusedItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTDCHasClipboard(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTDCGetClipboard(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTDCDoTaskLink(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAutoComboAddDelete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCommentsWantSpellCheck(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelChangeCommentsType();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnRefreshPercentSpinVisibility(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnChangeColour(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnEditEnd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEditCancel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRecreateRecurringTask(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	TODOITEM* GetTask(DWORD dwUniqueID) const
	{
		return m_data.GetTask(dwUniqueID);
	}
	DWORD GetTaskID(HTREEITEM hti) const
	{
		return m_find.GetTaskID(hti);
	}

	CString FormatInfoTip(const HTREEITEM hti, const TODOITEM* tdi) const;
	void InvalidateSelectedItem();
	virtual void InvalidateItem(HTREEITEM hti);
	void UpdateTask(TDC_ATTRIBUTE nAttrib, DWORD dwFlags = 0);
	void UpdateControls(HTREEITEM hti = NULL);
	virtual void UpdateSelectedTaskPath();
	CRect GetSplitterRect();
	void SetCtrlDate(CDateTimeCtrl& ctrl, const COleDateTime& date);
	BOOL CreateContentControl(BOOL bResendComments = FALSE);
	int GetMaxTaskCategoryWidth(CDC* pDC, BOOL bVisibleOnly = TRUE);
	void RefreshItemStates(HTREEITEM hti = NULL, BOOL bBold = TRUE, BOOL bCheck = TRUE, BOOL bAndChildren = TRUE);
	void IncrementTrackedTime();
	CString FormatDate(const COleDateTime& date, TDC_DATE nDate);
	int CalcMaxDateColWidth(TDC_DATE nDate, CDC* pDC);
	BOOL SetCommentsFont(HFONT hFont, BOOL bResendComments);
	virtual BOOL SetStyle(TDC_STYLE nStyle, BOOL bOn, BOOL bWantUpdate);

	virtual void EndLabelEdit(BOOL bCancel);
	BOOL ShowLabelEdit(const CRect& rPos);
	virtual BOOL GetItemTitleRect(HTREEITEM hti, TDC_TITLERECT nArea, CRect& rect) const;

	// internal version so we can tell how we've been called
	BOOL SetSelectedTaskComments(LPCTSTR szComments, const CString& sCustomComments, BOOL bInternal);

	int SetTaskDone(HTREEITEM hti, const COleDateTime& date, TDC_SETTASKDONE nSetChildren);
	BOOL SetSelectedTaskDone(const COleDateTime& date);
	int CheckWantSubtasksCompleted();
	COleDateTime GetSelectedTaskDate(TDC_DATE nDate) const;
	BOOL SetSelectedTaskDate(TDC_DATE nDate, const COleDateTime& date);
	BOOL SetSelectedTaskIcon(int nIndex);

	virtual void SetModified(BOOL bMod, TDC_ATTRIBUTE nAttrib);
	virtual TODOITEM* NewTask(HTREEITEM htiParent); // overridable
	virtual BOOL DeleteSelectedTask(BOOL bWarnUser, BOOL bResetSel = FALSE);
	virtual BOOL SelectedTasksHaveChildren() const;
	virtual COLORREF GetItemLineColor(HTREEITEM hti)
	{
		return m_tree.GetItemLineColor(hti);
	}

	void SaveGlobals(CTaskFile& file) const;
	void LoadGlobals(const CTaskFile& file);

	DWORD GetParentTaskID(HTREEITEM hti) const;

	virtual BOOL ModNeedsResort(TDC_ATTRIBUTE nModType) const;
	BOOL ModNeedsResort(TDC_ATTRIBUTE nModType, TDC_SORTBY nSortBy) const;
	static BOOL IsSortable(TDC_SORTBY nSortBy);
	virtual void ResortSelectedTaskParents();

	void ProcessItemLButtonDown(HTREEITEM htiHit, int nHitFlags, TDC_COLUMN nColID);
	void ProcessItemLButtonUp(HTREEITEM htiHit, int nHitFlags, TDC_COLUMN nColID);
	UINT MapColumnToCtrlID(TDC_COLUMN nColID);

	BOOL CopyCurrentSelection() const;
	BOOL IsClipboardEmpty() const;
	CTreeSelectionHelper& Selection()
	{
		return m_selection.selection;
	}
	const CTreeSelectionHelper& Selection() const
	{
		return m_selection.selection;
	}

	int GetSelectedTaskIDs(CDWordArray& aTaskIDs, DWORD& dwFocusedTaskID, BOOL bRemoveChildDupes) const;
	int GetSelectedTasks(CTaskFile& tasks, const TDCGETTASKS& filter, BOOL bRemoveChildDupes) const;
	void RestoreTreeSelection(const CDWordArray& aTaskIDs, DWORD dwDefaultID = 0);

	virtual void Resize(int cx = 0, int cy = 0);
	int CalcVisibleCtrlCount();
	BOOL IsCtrlShowing(const CTRLITEM& ctrl);
	void ReposControl(const CTRLITEM& ctrl, CDeferWndMove* pDWM, const CDlgUnits* pDLU, int nCol, int nTop,
		int nClientRight);
	void ReposComments(int cx, int cy, CDeferWndMove* pDWM, CRect& rComments /*out*/);
	int GetMinNonCommentSize();
	virtual void ReposTaskTree(CDeferWndMove* pDWM, const CRect& rPos);
	virtual void UpdateTasklistVisibility();
	void ShowHideControls();
	void EnableDisableControls(HTREEITEM hti);

	void SetLabelAlignment(int nStyle);
	void InitEditPrompts();

	int AddTreeChildrenToTaskFile(HTREEITEM hti, CTaskFile& file, HTASKITEM hTask, const TDCGETTASKS& filter) const;
	BOOL AddTreeItemToTaskFile(HTREEITEM hti, CTaskFile& file, HTASKITEM hParentTask, const TDCGETTASKS& filter,
		int nPos) const;
	BOOL AddSubTasksToTaskFile(const TODOSTRUCTURE* pTDSParent, CTaskFile& tasks, HTASKITEM hParentTask) const;
	BOOL AddTaskToTaskFile(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, int nPos, CTaskFile& tasks,
		HTASKITEM hParentTask) const;
	BOOL SetTaskAttributes(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, CTaskFile& file, HTASKITEM hTask,
		const TDCGETTASKS& filter, int nPos, BOOL bTitleCommentsOnly) const;

	HTREEITEM AddTaskToTreeItem(const CTaskFile& file, HTASKITEM hTask, HTREEITEM htiParent = NULL,
		HTREEITEM htiAfter = TVI_LAST, TDC_RESETIDS nResetID = TDCR_NO);
	BOOL AddTasksToTree(const CTaskFile& tasks, HTREEITEM htiDest, HTREEITEM htiDestAfter, TDC_RESETIDS nResetID);
	HTREEITEM InsertItem(LPCTSTR szText, HTREEITEM htiParent, HTREEITEM htiAfter, BOOL bEdit);
	BOOL GetInsertLocation(TDC_INSERTWHERE nWhere, HTREEITEM& htiDest, HTREEITEM& htiDestAfter);

	virtual BOOL LoadTasks(const CTaskFile& file);
	BOOL CheckRestoreBackupFile(LPCTSTR szFilePath);

	TDCCOLUMN* GetColumn(UINT nColID);
	TDCCOLUMN* GetColumn(TDC_SORTBY nSortBy);

	void SaveExpandedState(CPreferences& prefs); // keyed by last filepath
	HTREEITEM LoadExpandedState(const CPreferences& prefs, BOOL bResetSel = TRUE); // returns the previously selected item if any
	void SaveSplitPos(CPreferences& prefs);
	void LoadSplitPos(const CPreferences& prefs);
	virtual void SaveSortState(CPreferences& prefs); // keyed by last filepath
	virtual void LoadSortState(const CPreferences& prefs, LPCTSTR szFilePath); // keyed by filepath
	int SaveTreeExpandedState(CPreferences& prefs, LPCTSTR szRegKey, HTREEITEM hti = NULL, int nStart = 0);
	void LoadTreeExpandedState(const CPreferences& prefs, LPCTSTR szRegKey);

	void TimeTrackTask(HTREEITEM hti);
	BOOL IsActivelyTimeTracking(BOOL bCheckScreenSaver) const; // this instant

	BOOL SetTextChange(int nChange, CString& sItem, LPCTSTR szNewItem, TDC_ATTRIBUTE nAttrib, UINT nIDC);
	void DrawGutterItemText(CDC* pDC, const CString& sText, const CRect& rect, int nAlign, COLORREF crText = (COLORREF)-1);
	virtual void GetTaskTextColors(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, COLORREF& crText, COLORREF& crBack);

	void RecalcSelectedTimeEstimate();
	BOOL SpellcheckItem(HTREEITEM hti, CSpellCheckDlg* pSpellChecker, BOOL bTitle, BOOL bNotifyNoErrors);
	BOOL SpellcheckItem(HTREEITEM hti, CSpellCheckDlg* pSpellChecker);
	BOOL CanSpellcheckComments();

	BOOL GotoFile(const CString& sUrl, BOOL bShellExecute);
	BOOL ShowTaskLink(DWORD dwTaskID, LPCTSTR szLink);

	BOOL SelectionHasIncompleteDependencies(CString& sIncomplete) const;
	BOOL TaskHasIncompleteDependencies(HTREEITEM hti, CString& sIncomplete) const;
	BOOL SelectionHasIncompleteSubtasks(BOOL bExcludeRecurring) const;
	int SelectionHasCircularDependencies(CDWordArray& aTaskIDs) const;
	BOOL SelectionHasDates(TDC_DATE nDate, BOOL bAll = FALSE) const;
	int GetSelectionRecurringTaskCount() const;

	void HandleUnsavedComments();

	void ShowTaskHasIncompleteDependenciesError(const CString& sIncomplete);
	void ShowTaskHasCircularDependenciesError(const CDWordArray& aTaskIDs) const;

	void FixupParentCompletion(HTREEITEM htiParent);
	void FixupTaskDependencyDates(DWORD dwTaskID, TDC_DATE nDate);
	UINT AdjustTaskDates(DWORD dwTaskID, DWORD dwDependencyID, TDC_DATE nDate);

	BOOL UndoLastActionItems(const CArrayUndoElements& aElms);

	void LoadTaskIcons();

	BOOL CanMoveItem(HTREEITEM hti, TDC_MOVETASK nDirection) const;
	BOOL MoveSelection(TDC_MOVETASK nDirection);
	BOOL MoveSelectionDown();
	BOOL MoveSelectionUp();
	BOOL MoveSelectionLeft(); // up a level
	BOOL MoveSelectionRight(); // down a level
	HTREEITEM MoveItem(HTREEITEM hti, HTREEITEM htiDestParent, HTREEITEM htiDestPrevSibling);

	typedef CMap<DWORD, DWORD, DWORD, DWORD&> CMapID2ID;
	void PrepareTaskIDsForPaste(CTaskFile& tasks, TDC_RESETIDS nResetID) const;
	void BuildTaskIDMapForPaste(CTaskFile& tasks, HTASKITEM hTask, DWORD& dwNextID, CMapID2ID& mapID,
		TDC_RESETIDS nResetID) const;
	void PrepareTaskIDsForPaste(CTaskFile& tasks, HTASKITEM hTask, const CMapID2ID& mapID) const;
	BOOL PrepareTaskIDsForPaste(CString& sLink, const CMapID2ID& mapID) const;

	virtual int GetArchivableTasks(CTaskFile& tasks, BOOL bSelectedOnly = FALSE) const;
	void RemoveArchivedTasks(const CTaskFile& tasks, TDC_ARCHIVE nRemove, BOOL bRemoveFlagged);
	BOOL RemoveArchivedTask(const CTaskFile& tasks, HTASKITEM hTask, TDC_ARCHIVE nRemove, BOOL bRemoveFlagged);
	virtual BOOL RemoveArchivedTask(DWORD dwTaskID);
	BOOL ArchiveTasks(LPCTSTR szArchivePath, const CTaskFile& tasks); // helper to avoud code dupe


	void AppendTaskFileHeader(CTaskFile& tasks) const;

	enum TDI_STATE
	{
		TDIS_NONE,
		TDIS_SELECTED,
		TDIS_SELECTEDNOTFOCUSED,
		TDIS_DROPHILITED,
		TDIS_SELECTEDDROPHILITED,
	};

	TDI_STATE GetItemState(HTREEITEM hti);
	BOOL DrawItem(DWORD dwID, const NCGDRAWITEM* pNCGDI, TDI_STATE nState);
	BOOL DrawItem(const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, const NCGDRAWITEM* pNCGDI, TDI_STATE nState);
	void GetItemColors(DWORD dwID, NCGITEMCOLORS* pColors, TDI_STATE nState);
	void DrawCommentsText(CDC* pDC, const TODOITEM* pTDI, const TODOSTRUCTURE* pTDS, const CRect& rText,
		TDI_STATE nState);
	void DrawItemBackColor(CDC* pDC, const LPRECT rect, COLORREF crBack);

	void MultiSortTree(const TDSORTCOLUMNS& sort, HTREEITEM htiRoot = NULL, BOOL bSortChildren = TRUE);
	void SortTree(TDC_SORTBY nBy, BOOL bAscending, HTREEITEM htiRoot = NULL, BOOL bSortChildren = TRUE);
	void SortTreeItem(HTREEITEM hti, const TDSORTPARAMS& ss, BOOL bMulti = FALSE);
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareFuncMulti(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	static BOOL WantUpdateInheritedAttibute(TDC_ATTRIBUTE nAttrib);
	void ApplyLastInheritedChangeToSubtasks(DWORD dwID, TDC_ATTRIBUTE nAttrib);

	// used for building/creating the tree for saving/loading
	// not for overriding
	int GetAllTasks(CTaskFile& tasks) const;
	HTREEITEM SetAllTasks(const CTaskFile& tasks);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TODOCTRL_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_)
