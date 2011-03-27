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

// FilteredToDoCtrl.h: interface for the CFilteredToDoCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTEREDTODOCTRL_H__356A6EB9_C7EC_4395_8716_623AFF4A269B__INCLUDED_)
#define AFX_FILTEREDTODOCTRL_H__356A6EB9_C7EC_4395_8716_623AFF4A269B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToDoCtrl.h"

#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/EnHeaderCtrl.h"
#include "../../../CodeProject/Source/Subclass.h"
#include "../../../CodeProject/Source/XPTabCtrl.h"

class CFilteredToDoCtrl : public CToDoCtrl, public CSubclasser
{
public:
	CFilteredToDoCtrl(CContentMgr& mgr, const CONTENTFORMAT& cfDefault);
	virtual ~CFilteredToDoCtrl();

	BOOL ArchiveDoneTasks(LPCTSTR szFilePath, TDC_ARCHIVE nFlags, BOOL bRemoveFlagged); // returns true if any tasks were removed
	BOOL ArchiveSelectedTasks(LPCTSTR szFilePath, BOOL bRemove); // returns true if any tasks were removed

	void SetFilter(const FTDCFILTER& filter);
	FILTER_TYPE GetFilter(FTDCFILTER& filter) const;
	FILTER_TYPE GetFilter() const;
	void RefreshFilter();
	void ClearFilter();
	void ToggleFilter();
	BOOL HasFilter() const
	{
		return m_filter.IsSet();
	}
	BOOL HasLastFilter() const
	{
		return m_lastFilter.IsSet() || m_bLastFilterWasCustom;
	}
	BOOL HasCustomFilter() const;
	BOOL SetCustomFilter(const SEARCHPARAMS& params, LPCTSTR szName);
	BOOL CustomFilterHasRules() const
	{
		return m_customFilter.GetRuleCount();
	}
	BOOL RestoreCustomFilter();
	CString GetCustomFilterName() const
	{
		return m_sCustomFilter;
	}

	UINT GetTaskCount(UINT* pVisible = 0) const;
	BOOL DeleteSelectedTask()
	{
		return CToDoCtrl::DeleteSelectedTask();
	}

	int GetTasks(CTaskFile& tasks, const TDCGETTASKS& filter = TDCGETTASKS(TDCGT_ALL)) const;
	int GetSelectedTasks(CTaskFile& tasks, const TDCGETTASKS& filter = TDCGETTASKS(TDCGT_ALL)) const;
	int GetFilteredTasks(CTaskFile& tasks, const TDCGETTASKS& filter = TDCGETTASKS(TDCGT_ALL)) const;

	HTREEITEM NewTask(LPCTSTR szText, TDC_INSERTWHERE nWhere = TDC_INSERTATTOPOFSELTASKPARENT,
		BOOL bEditText = TRUE);
	BOOL SplitSelectedTask(int nNumSubtasks);

	void SetModified(BOOL bMod = TRUE)
	{
		CToDoCtrl::SetModified(bMod);
	}
	BOOL SelectTask(DWORD dwTaskID);

	int FindTasks(const SEARCHPARAMS& params, CResultArray& aResults) const;
	BOOL SelectTask(CString sPart, TDC_SELECTTASK nSelect);

	BOOL SetStyles(const CTDCStyles& styles);
	BOOL SetStyle(TDC_STYLE nStyle, BOOL bOn = TRUE)
	{
		return CToDoCtrl::SetStyle(nStyle, bOn);
	}

	void SetView(FTC_VIEW nView);
	FTC_VIEW GetView() const
	{
		return m_nCurView == FTCV_UNSET ? FTCV_TASKTREE : m_nCurView;
	}

	BOOL SetTreeFont(HFONT hFont); // setter responsible for deleting

	BOOL PtInHeader(CPoint ptScreen) const;
	void SetVisibleColumns(const CTDCColumnArray& aColumns);

	virtual void Sort(TDC_SORTBY nBy, BOOL bAllowToggle = TRUE);
	virtual void MultiSort(const TDSORTCOLUMNS& sort);
	virtual TDC_SORTBY GetSortBy() const;
	virtual void GetSortBy(TDSORTCOLUMNS& sort) const;
	virtual BOOL IsMultiSorting() const;

	BOOL MoveSelectedTask(TDC_MOVETASK nDirection);
	BOOL CanMoveSelectedTask(TDC_MOVETASK nDirection) const;

	BOOL GotoNextTask(TDC_GOTO nDirection);
	BOOL CanGotoNextTask(TDC_GOTO nDirection) const;
	BOOL GotoNextTopLevelTask(TDC_GOTO nDirection);
	BOOL CanGotoNextTopLevelTask(TDC_GOTO nDirection) const;

	BOOL CanExpandSelectedTask(BOOL bExpand) const;
	void ExpandSelectedTask(BOOL bExpand = TRUE);
	void ExpandAllTasks(BOOL bExpand = TRUE);

	void SetFocusToTasks();
	BOOL TasksHaveFocus() const;

	void SelectAll();
	void DeselectAll(); // call externally only
	void SelectNextTasksInHistory();
	void SelectPrevTasksInHistory();
	BOOL MultiSelectItems(const CDWordArray& aTasks, TSH_SELECT nState = TSHS_SELECT, BOOL bRedraw = TRUE);

	void SetUITheme(const UITHEME& theme);
	virtual int GetArchivableTasks(CTaskFile& tasks, BOOL bSelectedOnly) const;
	void RedrawReminders() const;

protected:
	CXPTabCtrl m_tabCtrl;
	CListCtrl m_list;
	CEnHeaderCtrl m_header;

	FTDCFILTER m_filter;
	FTDCFILTER m_lastFilter;
	SEARCHPARAMS m_customFilter;
	SEARCHPARAMS m_lastCustomFilter;
	BOOL m_bCustomFilter;
	BOOL m_bLastFilterWasCustom;
	CString m_sCustomFilter;

	FTC_VIEW m_nCurView;
	DWORD m_dw2ndClickItem;
	TDSORTCOLUMNS m_sortList;

	CTaskListDropTarget m_dtList;
	CImageList m_ilTabs;

	mutable BOOL m_bListNeedRefilter;
	mutable BOOL m_bListNeedResort, m_bTreeNeedResort;
	BOOL m_bListModSinceLastSort;
	BOOL m_bListMultiSort;

// Overrides
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilteredToDoCtrl)
	//}}AFX_VIRTUAL
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	// Generated message map functions
	//{{AFX_MSG(CFilteredToDoCtrl)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnSelchangeTabcontrol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRClickHeader(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickHeader(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListDblClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeExpandItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg LRESULT OnDropFileRef(WPARAM wParam, LPARAM lParam);
	afx_msg void OnListGetInfoTip(NMHDR* pNMHDR, LRESULT* pResult);
	//	afx_msg BOOL OnToolTipNotify( UINT id, NMHDR* pNMHDR, LRESULT* pResult );
	//	afx_msg BOOL OnToolTipShow( UINT id, NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg LRESULT OnEditCancel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterWidthChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshFilter(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEditChangeDueTime();

	DECLARE_MESSAGE_MAP()

protected:
	void InitFilterDueDate(COleDateTime& dateDue) const;
	BOOL ModNeedsRefilter(TDC_ATTRIBUTE nModType, FTC_VIEW nView) const;
	BOOL ModNeedsResort(TDC_ATTRIBUTE nModType) const;
	void ResortSelectedTaskParents();

	virtual BOOL DeleteSelectedTask(BOOL bWarnUser, BOOL bResetSel = FALSE);
	virtual BOOL SelectedTasksHaveChildren() const;
	virtual void SetModified(BOOL bMod, TDC_ATTRIBUTE nAttrib);
	virtual TODOITEM* NewTask(HTREEITEM htiParent);
	virtual void ReposTaskTree(CDeferWndMove* pDWM, const CRect& rPos);
	virtual COLORREF GetItemLineColor(HTREEITEM hti);
	CRect GetSelectedItemsRect() const;
	virtual BOOL SetStyle(TDC_STYLE nStyle, BOOL bOn, BOOL bWantUpdate); // one style at a time only

	virtual BOOL LoadTasks(const CTaskFile& file);
	virtual void SaveSortState(CPreferences& prefs);
	virtual void LoadSortState(const CPreferences& prefs, LPCTSTR szFilePath);

	// we hook the list view for mouse clicks
	virtual LRESULT ScWindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);

	void DrawColumnHeaderText(CDC* pDC, int nCol, const CRect& rCol, UINT nState);
	void UpdateColumnWidths();
	TDCCOLUMN* GetColumn(int nCol) const;
	TDC_COLUMN GetColumnID(int nCol) const;
	int GetColumnIndex(TDC_COLUMN nColID) const;
	int GetSelectedListTaskIDs(CDWordArray& aTaskIDs, DWORD& dwFocusedTaskID) const;
	void SetSelectedListTasks(const CDWordArray& aTaskIDs, DWORD dwFocusedTaskID);
	void RemoveDeletedListItems();
	DWORD GetFocusedListTaskID() const;
	void RemeasureList();
	void BuildListColumns(BOOL bResizeCols = TRUE);
	void GetItemColors(int nItem, NCGITEMCOLORS& colors);
	void UpdateTreeSelection();
	void UpdateSelectedTaskPath();
	void InvalidateItem(HTREEITEM hti);
	virtual void Resize(int cx = 0, int cy = 0);
	int FindTask(LPCTSTR szPart, int nStart = 0, BOOL bNext = TRUE);
	void RestoreTreeSelection(const CDWordArray& aTaskIDs, DWORD dwDefaultID = 0);
	virtual void UpdateTasklistVisibility();

	int GetFirstSelectedItem() const;
	void ClearListSelection();
	void ResyncListSelection();
	BOOL IsItemSelected(int nItem) const;

	void GetCompletedTasks(const TODOSTRUCTURE* pTDS, CTaskFile& tasks, HTASKITEM hTaskParent, BOOL bSelectedOnly = FALSE) const;
	virtual BOOL RemoveArchivedTask(DWORD dwTaskID);

	int FindTask(DWORD dwTaskID) const;
	void BuildFilterQuery(SEARCHPARAMS& params);
	void AddNonDateFilterQueryRules(SEARCHPARAMS& params);

	void RestoreFilter(const CPreferences& prefs, FTDCFILTER& filter, const CString& sSubKey = _T(""));
	void SaveFilter(const FTDCFILTER& filter, CPreferences& prefs, const CString& sSubKey = _T("")) const;
	void RefreshListFilter();
	void RefreshTreeFilter();

	void RebuildTree(const SEARCHPARAMS& filter);
	BOOL BuildTreeItem(HTREEITEM hti, const TODOSTRUCTURE* pTDS, const SEARCHPARAMS& filter);
	void RebuildList(const SEARCHPARAMS& filter);
	void AddTreeItemToList(HTREEITEM hti, const SEARCHPARAMS& filter);

	virtual BOOL GetItemTitleRect(HTREEITEM hti, TDC_TITLERECT nArea, CRect& rect) const;
	BOOL GetItemTitleRect(int nItem, TDC_TITLERECT nArea, CRect& rect, CDC* pDC = NULL, LPCTSTR szTitle = NULL) const;
	TDI_STATE GetItemState(int nItem);

	HTREEITEM GetTreeItem(int nItem) const;
	int GetListItem(HTREEITEM hti) const;
	DWORD GetTaskID(int nItem) const
	{
		return m_list.GetItemData(nItem);
	}
	DWORD GetTaskID(HTREEITEM hti) const
	{
		return CToDoCtrl::GetTaskID(hti);
	}

	BOOL InListView() const
	{
		return (GetView() == FTCV_TASKLIST);
	}
	BOOL IsFilterSet(FTC_VIEW nView) const;

	static int CALLBACK FTCCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	BOOL IsFilterSet(const FTDCFILTER& filter, FTC_VIEW nView) const;
	BOOL FiltersMatch(const FTDCFILTER& filter1, const FTDCFILTER& filter2, FTC_VIEW nView) const;

};

#endif // !defined(AFX_FILTEREDTODOCTRL_H__356A6EB9_C7EC_4395_8716_623AFF4A269B__INCLUDED_)
