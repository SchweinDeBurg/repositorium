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
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

// FilteredToDoCtrl.cpp: implementation of the CFilteredToDoCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FilteredToDoCtrl.h"
#include "ToDoItem.h"
#include "Resource.h"
#include "TDCStatic.h"
#include "TDCMsg.h"

#include "../../../CodeProject/Source/HoldRedraw.h"
#include "../../../CodeProject/Source/DateHelper.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../Common/Preferences.h"
#include "../../../CodeProject/Source/DeferWndMove.h"
#include "../../../CodeProject/Source/AutoFlag.h"
#include "../../../CodeProject/Source/HoldRedraw.h"
#include "../../../CodeProject/Source/OSVersion.h"
#include "../../../CodeProject/Source/GraphicsMisc.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#ifndef LVS_EX_DOUBLEBUFFER
#define LVS_EX_DOUBLEBUFFER 0x00010000
#endif

#ifndef LVS_EX_LABELTIP
#define LVS_EX_LABELTIP     0x00004000
#endif

const UINT SORTWIDTH = 10;
#define NOCOLOR ((COLORREF)-1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilteredToDoCtrl::CFilteredToDoCtrl(CContentMgr& mgr, const CONTENTFORMAT& cfDefault) :
CToDoCtrl(mgr, cfDefault),
m_nCurView(FTCV_UNSET),
m_bListNeedRefilter(TRUE),
m_bListNeedResort(FALSE),
m_bTreeNeedResort(FALSE),
m_bListModSinceLastSort(FALSE),
m_tabCtrl(e_tabBottom),
m_bLastFilterWasCustom(FALSE),
m_bCustomFilter(FALSE),
m_bListMultiSort(FALSE)
{
	// add extra controls
	for (int nCtrl = 0; nCtrl < NUM_FTDCCTRLS; nCtrl++)
	{
		const TDCCONTROL& ctrl = FTDCCONTROLS[nCtrl];

		AddRCControl(_T("CONTROL"), ctrl.szClass, CEnString(ctrl.nIDCaption),
			ctrl.dwStyle, ctrl.dwExStyle,
			ctrl.nX, ctrl.nY, ctrl.nCx, ctrl.nCy, ctrl.nID);
	}

	m_sortList.nBy1 = TDC_UNSORTED;
	m_sortList.nBy2 = TDC_UNSORTED;
	m_sortList.nBy3 = TDC_UNSORTED;
	m_sortList.bAscending1 = -1;
	m_sortList.bAscending2 = FALSE;
	m_sortList.bAscending3 = FALSE;
}

CFilteredToDoCtrl::~CFilteredToDoCtrl()
{
}

BEGIN_MESSAGE_MAP(CFilteredToDoCtrl, CToDoCtrl)
	//{{AFX_MSG_MAP(CFilteredToDoCtrl)
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_FTC_TABCTRL, OnSelchangeTabcontrol)
	ON_NOTIFY(NM_CUSTOMDRAW, 0, OnHeaderCustomDraw)
	ON_NOTIFY(NM_RCLICK, 0, OnRClickHeader)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_FTC_TASKLIST, OnClickHeader)
	ON_NOTIFY(NM_CLICK, IDC_FTC_TASKLIST, OnListClick)
	ON_NOTIFY(NM_DBLCLK, IDC_FTC_TASKLIST, OnListDblClick)
	ON_NOTIFY(NM_KEYDOWN, IDC_FTC_TASKLIST, OnListKeyDown)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TASKLIST, OnTreeExpandItem)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FTC_TASKLIST, OnListSelChanged)
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_NOTIFY(LVN_GETINFOTIP, IDC_FTC_TASKLIST, OnListGetInfoTip)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_FTC_TASKLIST, OnListGetDispInfo)
	ON_REGISTERED_MESSAGE(WM_TLDT_DROPFILE, OnDropFileRef)
	ON_REGISTERED_MESSAGE(WM_PCANCELEDIT, OnEditCancel)
	ON_REGISTERED_MESSAGE(WM_NCG_WIDTHCHANGE, OnGutterWidthChange)
	ON_MESSAGE(WM_TDC_REFRESHFILTER, OnRefreshFilter)
	ON_CBN_EDITCHANGE(IDC_DUETIME, OnEditChangeDueTime)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////

void CFilteredToDoCtrl::DoDataExchange(CDataExchange* pDX)
{
	CToDoCtrl::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_FTC_TASKLIST, m_list);
	DDX_Control(pDX, IDC_FTC_TABCTRL, m_tabCtrl);
}

BOOL CFilteredToDoCtrl::OnInitDialog()
{
	CToDoCtrl::OnInitDialog();

	// prepare filtered view
	ListView_SetExtendedListViewStyleEx(m_list,
		LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER,
		LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
	m_dtList.Register(&m_list, this);

	// prevent the list overwriting the label edit
	m_list.ModifyStyle(0, WS_CLIPSIBLINGS);

	// set tree header height to match listview
	CRect rHeader;
	m_list.GetHeaderCtrl()->GetWindowRect(rHeader);
	m_tree.SetHeaderHeight(rHeader.Height());

	// and hook it
	ScHookWindow(m_list);

	// add all columns
	BuildListColumns();

	// prepare tab bar
	if (m_ilTabs.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 1))
	{
		CBitmap bm;
		bm.LoadBitmap(IDB_TREELIST);
		m_ilTabs.Add(&bm, RGB(255, 0, 255));
		m_tabCtrl.SetImageList(&m_ilTabs);
	}

	m_tabCtrl.InsertItem(TCIF_TEXT | TCIF_PARAM | TCIF_IMAGE, 0, CEnString(IDS_TASKTREE), 0, (LPARAM)&m_tree);
	m_tabCtrl.InsertItem(TCIF_TEXT | TCIF_PARAM | TCIF_IMAGE, 1, CEnString(IDS_LISTVIEW), 1, (LPARAM)&m_list);

	Resize();

	return FALSE;
}

BOOL CFilteredToDoCtrl::PreTranslateMessage(MSG* pMsg)
{
	return CToDoCtrl::PreTranslateMessage(pMsg);
}

void CFilteredToDoCtrl::SetUITheme(const UITHEME& theme)
{
	CToDoCtrl::SetUITheme(theme);

	m_tabCtrl.SetBackgroundColor(theme.crAppBackLight);
}

BOOL CFilteredToDoCtrl::LoadTasks(const CTaskFile& file)
{
	CPreferences prefs;

	BOOL bSuccess = CToDoCtrl::LoadTasks(file);

	// reload last view
	if (m_nCurView == FTCV_UNSET)
	{
		RestoreFilter(prefs, m_lastFilter, _T("Last"));
		RestoreFilter(prefs, m_filter);

		if (IsFilterSet(FTCV_TASKTREE))
		{
			RefreshTreeFilter();   // always
		}

		CString sKey = GetPreferencesKey(); // no subkey

		if (!sKey.IsEmpty()) // first time
		{
			m_bListNeedRefilter = TRUE;

			FTC_VIEW nView = (FTC_VIEW)prefs.GetProfileInt(sKey, _T("View"), FTCV_TASKTREE);
			SetView(nView);
		}
	}
	else
	{
		RefreshFilter();
	}

	return bSuccess;
}

void CFilteredToDoCtrl::RestoreFilter(const CPreferences& prefs, FTDCFILTER& filter, const CString& sSubKey)
{
	CString sKey = GetPreferencesKey(_T("Filter"));

	if (!sKey.IsEmpty())
	{
		if (!sSubKey.IsEmpty())
		{
			sKey += _T("\\") + sSubKey;
		}

		filter.nFilter = (FILTER_TYPE)prefs.GetProfileInt(sKey, _T("Filter"), FT_ALL);
		filter.sTitle = prefs.GetProfileString(sKey, _T("Title"));
		filter.nPriority = prefs.GetProfileInt(sKey, _T("Priority"), FT_ANYPRIORITY);
		filter.nRisk = prefs.GetProfileInt(sKey, _T("Risk"), FT_ANYRISK);

		// cats
		CString sCategory = prefs.GetProfileString(sKey, _T("Category"));
		Misc::Split(sCategory, filter.aCategories, TRUE);

		// alloc to
		CString sAllocTo = prefs.GetProfileString(sKey, _T("AllocTo"));
		Misc::Split(sAllocTo, filter.aAllocTo, TRUE);

		// alloc by
		CString sAllocBy = prefs.GetProfileString(sKey, _T("AllocBy"));
		Misc::Split(sAllocBy, filter.aAllocBy, TRUE);

		// status
		CString sStatus = prefs.GetProfileString(sKey, _T("Status"));
		Misc::Split(sStatus, filter.aStatus, TRUE);

		// version
		CString sVersion = prefs.GetProfileString(sKey, _T("Version"));
		Misc::Split(sVersion, filter.aVersions, TRUE);

		// options
		filter.SetFlag(FT_ANYCATEGORY, prefs.GetProfileInt(sKey, _T("AnyCategory"), FALSE));
		filter.SetFlag(FT_ANYALLOCTO, prefs.GetProfileInt(sKey, _T("AnyAllocTo"), FALSE));
		filter.SetFlag(FT_HIDEPARENTS, prefs.GetProfileInt(sKey, _T("HideParents"), FALSE));
		filter.SetFlag(FT_HIDEOVERDUE, prefs.GetProfileInt(sKey, _T("HideOverDue"), FALSE));
		filter.SetFlag(FT_HIDEDONE, prefs.GetProfileInt(sKey, _T("HideDone"), FALSE));
		filter.SetFlag(FT_HIDECOLLAPSED, prefs.GetProfileInt(sKey, _T("HideCollapsed"), FALSE));
	}
}

void CFilteredToDoCtrl::SaveFilter(const FTDCFILTER& filter, CPreferences& prefs, const CString& sSubKey) const
{
	CString sKey = GetPreferencesKey(_T("Filter"));

	if (!sKey.IsEmpty())
	{
		if (!sSubKey.IsEmpty())
		{
			sKey += _T("\\") + sSubKey;
		}

		prefs.WriteProfileInt(sKey, _T("Filter"), filter.nFilter);
		prefs.WriteProfileString(sKey, _T("Title"), filter.sTitle);
		prefs.WriteProfileInt(sKey, _T("Priority"), filter.nPriority);
		prefs.WriteProfileInt(sKey, _T("Risk"), filter.nRisk);
		prefs.WriteProfileString(sKey, _T("AllocBy"), Misc::FormatArray(filter.aAllocBy));
		prefs.WriteProfileString(sKey, _T("Status"), Misc::FormatArray(filter.aStatus));
		prefs.WriteProfileString(sKey, _T("Version"), Misc::FormatArray(filter.aVersions));
		prefs.WriteProfileString(sKey, _T("AllocTo"), Misc::FormatArray(filter.aAllocTo));
		prefs.WriteProfileString(sKey, _T("Category"), Misc::FormatArray(filter.aCategories));

		// options
		prefs.WriteProfileInt(sKey, _T("AnyAllocTo"), filter.HasFlag(FT_ANYALLOCTO));
		prefs.WriteProfileInt(sKey, _T("AnyCategory"), filter.HasFlag(FT_ANYCATEGORY));
		prefs.WriteProfileInt(sKey, _T("HideParents"), filter.HasFlag(FT_HIDEPARENTS));
		prefs.WriteProfileInt(sKey, _T("HideOverDue"), filter.HasFlag(FT_HIDEOVERDUE));
		prefs.WriteProfileInt(sKey, _T("HideDone"), filter.HasFlag(FT_HIDEDONE));
		prefs.WriteProfileInt(sKey, _T("HideCollapsed"), filter.HasFlag(FT_HIDECOLLAPSED));
	}
}

void CFilteredToDoCtrl::OnDestroy()
{
	if (m_nCurView != FTCV_UNSET)
	{
		CPreferences prefs;
		CString sKey = GetPreferencesKey(); // no subkey

		// save view
		if (!sKey.IsEmpty())
		{
			prefs.WriteProfileInt(sKey, _T("View"), m_nCurView);
		}

		SaveFilter(m_filter, prefs);
		SaveFilter(m_lastFilter, prefs, _T("Last"));
	}

	CToDoCtrl::OnDestroy();
}

void CFilteredToDoCtrl::BuildListColumns(BOOL bResizeCols)
{
	while (m_list.DeleteColumn(0));

	int nCol = NUM_COLUMNS - 1; // we handle title column separately

	while (nCol--)
	{
		const TDCCOLUMN& col = COLUMNS[nCol];
		UINT nFmt = col.nAlignment == DT_RIGHT ? LVCFMT_RIGHT : LVCFMT_LEFT;

		m_list.InsertColumn(0, _T(""), nFmt, 10);
	}

	// title column
	if (HasStyle(TDCS_RIGHTSIDECOLUMNS))
	{
		m_list.InsertColumn(0, _T(""), LVCFMT_LEFT, 10);
	}
	else
	{
		m_list.InsertColumn(NUM_COLUMNS - 1, _T(""), LVCFMT_LEFT, 10);
	}

	if (bResizeCols)
	{
		UpdateColumnWidths();
	}
}

void CFilteredToDoCtrl::OnRClickHeader(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	// forward on to parent
	const MSG* pMsg = GetCurrentMessage();
	LPARAM lPos = MAKELPARAM(pMsg->pt.x, pMsg->pt.y);

	GetParent()->SendMessage(WM_CONTEXTMENU, (WPARAM)GetSafeHwnd(), lPos);

	*pResult = 0;
}

void CFilteredToDoCtrl::OnEditChangeDueTime()
{
	// need some special hackery to prevent a re-filter in the middle
	// of the user manually typing into the time field
	BOOL bNeedsRefilter = ModNeedsRefilter(TDCA_DUEDATE, FTCV_TASKTREE);

	if (bNeedsRefilter)
	{
		SetStyle(TDCS_REFILTERONMODIFY, FALSE, FALSE);
	}

	CToDoCtrl::OnSelChangeDueTime();

	if (bNeedsRefilter)
	{
		SetStyle(TDCS_REFILTERONMODIFY, TRUE, FALSE);
	}
}

void CFilteredToDoCtrl::OnClickHeader(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW* pNMLV = (NMLISTVIEW*)pNMHDR;

	int nCol = pNMLV->iSubItem;
	TDCCOLUMN* pCol = GetColumn(nCol);

	if (pCol->nSortBy != TDC_UNSORTED)
	{
		TDC_SORTBY nPrev = m_sortList.nBy1;
		Sort(pCol->nSortBy);

		// notify parent
		if (m_sortList.nBy1 != nPrev)
		{
			GetParent()->SendMessage(WM_TDCN_SORT, GetDlgCtrlID(), MAKELPARAM((WORD)nPrev, (WORD)m_sortList.nBy1));
		}
	}

	*pResult = 0;
}

void CFilteredToDoCtrl::OnTreeExpandItem(NMHDR* /*pNMHDR*/, LRESULT* /*pResult*/)
{
	if (m_filter.HasFlag(FT_HIDECOLLAPSED))
	{
		if (InListView())
		{
			RefreshListFilter();
		}
		else
		{
			m_bListNeedRefilter = TRUE;
		}
	}
}

void CFilteredToDoCtrl::OnListGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* lplvdi = (NMLVDISPINFO*)pNMHDR;
	*pResult = 0;

	UINT nMask = lplvdi->item.mask;
	DWORD dwTaskID = (DWORD)lplvdi->item.lParam;

	if ((nMask & LVIF_TEXT) &&  m_dwEditingID != dwTaskID)
	{
		TODOITEM* pTDI = GetTask(dwTaskID);

		// it's possible that the task does not exist if it's just been
		// deleted from the tree view
		if (!pTDI)
		{
			return;
		}

		// all else
		lplvdi->item.pszText = const_cast<LPTSTR>((LPCTSTR)pTDI->sTitle);
	}

	if (nMask & LVIF_IMAGE)
	{
		if (!HasStyle(TDCS_TREETASKICONS))
		{
			lplvdi->item.iImage = -1;
		}
		else
		{
			TODOITEM* pTDI = NULL;
			TODOSTRUCTURE* pTDS = NULL;
			m_data.GetTask(dwTaskID, pTDI, pTDS);

			BOOL bHasChildren = pTDS->HasSubTasks();
			int nImage = -1;

			if (pTDI->nIconIndex >= 0)
			{
				nImage = pTDI->nIconIndex;
			}

			else if (HasStyle(TDCS_SHOWPARENTSASFOLDERS) && bHasChildren)
			{
				nImage = 0;
			}

			lplvdi->item.iImage = nImage;
		}
	}
}

void CFilteredToDoCtrl::SetVisibleColumns(const CTDCColumnArray& aColumns)
{
	CToDoCtrl::SetVisibleColumns(aColumns);

	if (InListView())
	{
		UpdateColumnWidths();
	}
}

void CFilteredToDoCtrl::OnSelchangeTabcontrol(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	SetView((FTC_VIEW)m_tabCtrl.GetCurSel());

	*pResult = 0;
}

BOOL CFilteredToDoCtrl::ArchiveDoneTasks(LPCTSTR szFilePath, TDC_ARCHIVE nFlags, BOOL bRemoveFlagged)
{
	if (CToDoCtrl::ArchiveDoneTasks(szFilePath, nFlags, bRemoveFlagged))
	{
		if (InListView())
		{
			if (IsFilterSet(FTCV_TASKLIST))
			{
				RefreshListFilter();
			}
		}
		else if (IsFilterSet(FTCV_TASKTREE))
		{
			RefreshTreeFilter();
		}

		return TRUE;
	}

	// else
	return FALSE;
}

BOOL CFilteredToDoCtrl::ArchiveSelectedTasks(LPCTSTR szFilePath, BOOL bRemove)
{
	if (CToDoCtrl::ArchiveSelectedTasks(szFilePath, bRemove))
	{
		if (InListView())
		{
			if (IsFilterSet(FTCV_TASKLIST))
			{
				RefreshListFilter();
			}
		}
		else if (IsFilterSet(FTCV_TASKTREE))
		{
			RefreshTreeFilter();
		}

		return TRUE;
	}

	// else
	return FALSE;
}

int CFilteredToDoCtrl::GetArchivableTasks(CTaskFile& tasks, BOOL bSelectedOnly) const
{
	if (bSelectedOnly || !IsFilterSet(FTCV_TASKTREE))
	{
		return CToDoCtrl::GetArchivableTasks(tasks, bSelectedOnly);
	}

	// else
	GetCompletedTasks(m_data.GetStructure(), tasks, NULL, FALSE);

	return tasks.GetTaskCount();
}

BOOL CFilteredToDoCtrl::RemoveArchivedTask(DWORD dwTaskID)
{
	ASSERT(GetTask(dwTaskID));

	// note: if the tasks does not exist in the tree then this is not a bug
	// if a filter is set
	HTREEITEM hti = m_find.GetItem(dwTaskID);
	ASSERT(hti || IsFilterSet(FTCV_TASKTREE));

	if (!hti && !IsFilterSet(FTCV_TASKTREE))
	{
		return FALSE;
	}

	if (hti)
	{
		m_tree.DeleteItem(hti);
	}

	return m_data.DeleteTask(dwTaskID);
}

void CFilteredToDoCtrl::GetCompletedTasks(const TODOSTRUCTURE* pTDS, CTaskFile& tasks, HTASKITEM hTaskParent, BOOL bSelectedOnly) const
{
	const TODOITEM* pTDI = NULL;

	if (!pTDS->IsRoot())
	{
		DWORD dwTaskID = pTDS->GetTaskID();
		pTDI = GetTask(dwTaskID);
		ASSERT(pTDI);

		// we add the task if it is completed (and optionally selected) or it has children
		if (pTDI->IsDone() || pTDS->HasSubTasks())
		{
			HTASKITEM hTask = tasks.NewTask("", hTaskParent, dwTaskID);
			ASSERT(hTask);

			// copy attributes
			TDCGETTASKS allTasks;
			SetTaskAttributes(pTDI, pTDS, tasks, hTask, allTasks, -1, FALSE);

			// this task is now the new parent
			hTaskParent = hTask;
		}
	}

	// children
	if (pTDS->HasSubTasks())
	{
		for (int nSubtask = 0; nSubtask < pTDS->GetSubTaskCount(); nSubtask++)
		{
			const TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubtask);
			GetCompletedTasks(pTDSChild, tasks, hTaskParent, bSelectedOnly);
		}

		// if no subtasks were added and the parent is not completed
		// (and optionally selected) then we remove it
		if (hTaskParent && tasks.GetFirstTask(hTaskParent) == NULL)
		{
			ASSERT(pTDI);

			if (!pTDI->IsDone())
			{
				tasks.DeleteTask(hTaskParent);
			}
		}
	}
}

void CFilteredToDoCtrl::SetView(FTC_VIEW nView)
{
	if (nView == FTCV_UNSET)
	{
		ASSERT(0);
		return;
	}

	if (m_nCurView == nView)
	{
		return;
	}

	m_nCurView = nView;

	// show the incoming selection and hide the outgoing in that order
	BOOL bFiltered = FALSE;

	// take a note of what task is currently singly selected
	// so that we can prevent unnecessary calls to UpdateControls
	DWORD dwSelTaskID = (GetSelectedCount() == 1) ? GetTaskID(GetSelectedItem()) : 0;

	switch (nView)
	{
	case FTCV_TASKTREE:
		// make sure something is selected
		if (GetSelectedCount() == 0)
		{
			HTREEITEM hti = m_tree.GetSelectedItem();

			if (!hti)
			{
				hti = m_tree.GetChildItem(NULL);
			}

			CToDoCtrl::SelectTask(GetTaskID(hti));
		}

		// update sort
		if (m_bTreeNeedResort)
		{
			m_bTreeNeedResort = FALSE;
			Resort();
		}

		m_tree.EnsureVisible(Selection().GetFirstItem());
		m_tree.ShowWindow(SW_SHOW);

		m_list.ShowWindow(SW_HIDE);
		m_tree.SetFocus();
		break;

	case FTCV_TASKLIST:
		// processed any unhandled comments
		HandleUnsavedComments();

		if (!m_header.GetSafeHwnd())
		{
			m_header.SubclassDlgItem(0, &m_list);
			m_header.EnableTracking(FALSE);

			// set the prompt now we know how tall the header is
			CRect rHeader;
			m_header.GetWindowRect(rHeader);
			m_list.ScreenToClient(rHeader);
			m_mgrPrompts.SetPrompt(m_list, IDS_TDC_FILTEREDTASKLISTPROMPT, LVM_GETITEMCOUNT, 0, rHeader.bottom);
		}

		// make sure row height is correct by forcing a WM_MEASUREITEM
		RemeasureList();

		// update column widths
		UpdateColumnWidths();

		// update filter
		bFiltered = m_bListNeedRefilter;

		if (m_bListNeedRefilter)
		{
			RefreshListFilter();
		}

		// update sort
		if (m_bListNeedResort)
		{
			m_bListNeedResort = FALSE;
			Resort();
		}

		// restore selection
		ResyncListSelection();

		m_list.EnsureVisible(GetFirstSelectedItem(), FALSE);

		m_list.ShowWindow(SW_SHOW);
		m_list.SetFocus();
		m_tree.ShowWindow(SW_HIDE);

		break;
	}

	// update controls only if the selection has changed and
	// we didn't refilter (RefreshFilter will already have called UpdateControls)
	BOOL bSelChange = !(GetSelectedCount() == 1 && GetTaskID(GetSelectedItem()) == dwSelTaskID);

	if (bSelChange && !bFiltered)
	{
		UpdateControls();
	}

	m_tabCtrl.SetCurSel((int)nView);

	// notify parent
	GetParent()->PostMessage(WM_TDCN_VIEWCHANGE);
}

void CFilteredToDoCtrl::RemeasureList()
{
	CRect rList;
	m_list.GetWindowRect(rList);
	ScreenToClient(rList);

	WINDOWPOS wpos = { m_list, NULL, rList.left, rList.top, rList.Width(), rList.Height(), SWP_NOZORDER };
	m_list.SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wpos);
}

void CFilteredToDoCtrl::UpdateColumnWidths()
{
	if (!InListView())
	{
		return;
	}

	int nCol = NUM_COLUMNS;
	int nTotalWidth = 0;
	BOOL bFirstWidth = TRUE;

	for (nCol = 0; nCol < NUM_COLUMNS; nCol++)
	{
		const TDCCOLUMN* pCol = GetColumn(nCol);
		ASSERT(pCol);

		if (pCol->nColID == TDCC_CLIENT)
		{
			continue;   // we'll deal with this at the end
		}

		int nWidth = m_tree.GetColumnWidth(pCol->nColID);

		if (nWidth && bFirstWidth)
		{
			bFirstWidth = FALSE;
			nWidth += 1;
		}

		// sort indicator
		if (IsColumnShowing(pCol->nColID))
		{
			BOOL bTreeColWidened = !(m_bMultiSort || m_sort.nBy1 == TDC_UNSORTED) && m_sort.nBy1 == pCol->nSortBy;
			BOOL bListColWidened = !(m_bListMultiSort || m_sortList.nBy1 == TDC_UNSORTED) && m_sortList.nBy1 == pCol->nSortBy;

			if (!bTreeColWidened && bListColWidened)
			{
				nWidth += SORTWIDTH;
			}

			else if (bTreeColWidened && !bListColWidened)
			{
				nWidth -= SORTWIDTH;
			}
		}

		m_list.SetColumnWidth(nCol, nWidth);

		nTotalWidth += nWidth;
	}

	// client column is what's left
	CRect rList;
	m_list.GetClientRect(rList);

	int nColWidth = max(300, rList.Width() - nTotalWidth);
	m_list.SetColumnWidth(GetColumnIndex(TDCC_CLIENT), nColWidth);

	m_list.Invalidate(FALSE);
	m_list.UpdateWindow();
}

void CFilteredToDoCtrl::ReposTaskTree(CDeferWndMove* pDWM, const CRect& rPos)
{
	// position tab ctrl below task tree
	CRect rList(rPos);

	if (HasStyle(TDCS_SHOWTREELISTBAR))
	{
		CRect rTabs(0, 0, rPos.Width(), 0);
		m_tabCtrl.AdjustRect(TRUE, rTabs);

		int nTabHeight = rTabs.Height();
		rTabs = rPos;
		rTabs.right += 2;
		rTabs.top = rTabs.bottom - nTabHeight + 5;

		pDWM->MoveWindow(&m_tabCtrl, rTabs);

		// adjust rect for tab ctrl
		rList.bottom = rTabs.top + 1;
	}

	CToDoCtrl::ReposTaskTree(pDWM, rList);

	pDWM->MoveWindow(&m_list, rList);
}

void CFilteredToDoCtrl::UpdateTasklistVisibility()
{
	BOOL bTasksVis = (m_nMaxState != TDCMS_MAXCOMMENTS);

	if (InListView())
	{
		m_list.ShowWindow(bTasksVis ? SW_SHOW : SW_HIDE);
	}
	else
	{
		CToDoCtrl::UpdateTasklistVisibility();
	}

	// handle tab control
	m_tabCtrl.ShowWindow(bTasksVis && HasStyle(TDCS_SHOWTREELISTBAR) ? SW_SHOW : SW_HIDE);
}

BOOL CFilteredToDoCtrl::OnEraseBkgnd(CDC* pDC)
{
	// clip out tab ctrl
	if (m_tabCtrl.GetSafeHwnd())
	{
		ExcludeChild(this, pDC, &m_tabCtrl);
	}

	return CToDoCtrl::OnEraseBkgnd(pDC);
}


void CFilteredToDoCtrl::Resize(int cx, int cy)
{
	CToDoCtrl::Resize(cx, cy);

	UpdateColumnWidths();
}

void CFilteredToDoCtrl::SelectAll()
{
	if (InListView())
	{
		int nNumItems = m_list.GetItemCount();
		BOOL bAllTasks = (CToDoCtrl::GetTaskCount() == (UINT)nNumItems);
		CDWordArray aTaskIDs;

		for (int nItem = 0; nItem < nNumItems; nItem++)
		{
			// select item
			m_list.SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

			// save ID only not showing all tasks
			if (!bAllTasks)
			{
				aTaskIDs.Add(m_list.GetItemData(nItem));
			}
		}

		// select items in tree
		if (bAllTasks)
		{
			CToDoCtrl::SelectAll();
		}
		else
		{
			MultiSelectItems(aTaskIDs, TSHS_SELECT, FALSE);
		}
	}
	else
	{
		CToDoCtrl::SelectAll();
	}
}

void CFilteredToDoCtrl::DeselectAll()
{
	CToDoCtrl::DeselectAll();
	ClearListSelection();
}

int CFilteredToDoCtrl::GetTasks(CTaskFile& tasks, const TDCGETTASKS& filter) const
{
	if (InListView())
	{
		// we return exactly what's selected in the list and in the same order
		// so we make sure the filter includes TDCGT_NOTSUBTASKS
		TDCGETTASKS tdcf(filter);
		tdcf.dwFlags |= TDCGTF_NOTSUBTASKS;

		for (int nItem = 0; nItem < m_list.GetItemCount(); nItem++)
		{
			HTREEITEM hti = GetTreeItem(nItem);
			AddTreeItemToTaskFile(hti, tasks, NULL, tdcf, 0);
		}

		if (filter.dwFlags & TDCGTF_FILENAME)
		{
			tasks.SetFileName(m_sLastSavePath);
		}

		return tasks.GetTaskCount();
	}
	else
	{
		return CToDoCtrl::GetTasks(tasks, filter);
	}
}

int CFilteredToDoCtrl::GetSelectedTasks(CTaskFile& tasks, const TDCGETTASKS& filter) const
{
	if (InListView())
	{
		// we return exactly what's selected in the list and in the same order
		// so we make sure the filter includes TDCGT_NOTSUBTASKS
		TDCGETTASKS tdcf(filter);
		tdcf.dwFlags |= TDCGTF_NOTSUBTASKS;

		POSITION pos = m_list.GetFirstSelectedItemPosition();

		while (pos)
		{
			int nItem = m_list.GetNextSelectedItem(pos);
			HTREEITEM hti = GetTreeItem(nItem);

			AddTreeItemToTaskFile(hti, tasks, NULL, tdcf, 0);
		}

		return tasks.GetTaskCount();
	}
	else
	{
		return CToDoCtrl::GetSelectedTasks(tasks, filter);
	}
}

int CFilteredToDoCtrl::GetFilteredTasks(CTaskFile& tasks, const TDCGETTASKS& filter) const
{
	// synonym for GetTasks which always returns the filtered tasks
	return GetTasks(tasks, filter);
}

FILTER_TYPE CFilteredToDoCtrl::GetFilter(FTDCFILTER& filter) const
{
	filter = m_filter;
	return m_filter.nFilter;
}

void CFilteredToDoCtrl::SetFilter(const FTDCFILTER& filter)
{
	FTDCFILTER fPrev = m_filter;
	m_filter = filter;

	if (m_bDelayLoaded)
	{
		m_bListNeedRefilter = TRUE;
	}
	else
	{
		BOOL bTreeNeedsFilter = m_bCustomFilter || !FiltersMatch(fPrev, filter, FTCV_TASKTREE);
		BOOL bListNeedRefilter = m_bCustomFilter || !FiltersMatch(fPrev, filter, FTCV_TASKLIST);

		m_bCustomFilter = FALSE;

		if (bTreeNeedsFilter)
		{
			RefreshFilter();
		}

		else if (bListNeedRefilter)
		{
			if (InListView())
			{
				RefreshListFilter();
			}
			else
			{
				m_bListNeedRefilter = TRUE;
			}
		}
	}

	// modify the tree prompt depending on whether there is a filter set
	if (IsFilterSet(FTCV_TASKTREE))
	{
		m_mgrPrompts.SetPrompt(m_tree, IDS_TDC_FILTEREDTASKLISTPROMPT, TVM_GETCOUNT);
	}
	else
	{
		m_mgrPrompts.SetPrompt(m_tree, IDS_TDC_TASKLISTPROMPT, TVM_GETCOUNT);
	}
}

void CFilteredToDoCtrl::ClearFilter()
{
	if (HasFilter() || HasCustomFilter())
	{
		FTDCFILTER filter; // empty filter
		SetFilter(filter);
		m_lastFilter = filter; // clear

		m_customFilter = SEARCHPARAMS(); // empty ruleset
		m_bCustomFilter = FALSE;
		m_bLastFilterWasCustom = FALSE;
	}
}

void CFilteredToDoCtrl::ToggleFilter()
{
	if (HasFilter() || HasCustomFilter()) // turn off
	{
		// save last filter
		if (m_bCustomFilter)
		{
			m_bLastFilterWasCustom = TRUE;
			m_lastCustomFilter = m_customFilter;
			SEARCHPARAMS filter; // empty filter
			SetCustomFilter(filter, m_sCustomFilter);
			m_bCustomFilter = FALSE;
		}
		else
		{
			m_bLastFilterWasCustom = FALSE;
			m_lastFilter = m_filter;
			FTDCFILTER filter; // empty filter
			SetFilter(filter);
			m_bCustomFilter = FALSE;
		}
	}
	else // turn on
	{
		if (m_bLastFilterWasCustom)
		{
			SetCustomFilter(m_lastCustomFilter, m_sCustomFilter);
		}
		else
		{
			SetFilter(m_lastFilter);
		}
	}
}

BOOL CFilteredToDoCtrl::FiltersMatch(const FTDCFILTER& filter1, const FTDCFILTER& filter2, FTC_VIEW nView) const
{
	if (nView == FTCV_UNSET)
	{
		return FALSE;
	}

	DWORD dwIgnore = 0;

	if (nView == FTCV_TASKTREE)
	{
		dwIgnore = (FT_HIDECOLLAPSED | FT_HIDEPARENTS);
	}

	return FTDCFILTER::FiltersMatch(filter1, filter2, dwIgnore);
}

BOOL CFilteredToDoCtrl::IsFilterSet(FTC_VIEW nView) const
{
	return IsFilterSet(m_filter, nView);
}

BOOL CFilteredToDoCtrl::IsFilterSet(const FTDCFILTER& filter, FTC_VIEW nView) const
{
	if (nView == FTCV_UNSET)
	{
		return FALSE;
	}

	DWORD dwIgnore = 0;

	if (nView == FTCV_TASKTREE)
	{
		dwIgnore = (FT_HIDECOLLAPSED | FT_HIDEPARENTS);
	}

	return filter.IsSet(dwIgnore);
}

UINT CFilteredToDoCtrl::GetTaskCount(UINT* pVisible) const
{
	if (pVisible)
	{
		if (InListView())
		{
			*pVisible = m_list.GetItemCount();
		}
		else
		{
			*pVisible = m_tree.GetCount();
		}
	}

	return CToDoCtrl::GetTaskCount();
}

BOOL CFilteredToDoCtrl::HasCustomFilter() const
{
	return m_bCustomFilter;
}

BOOL CFilteredToDoCtrl::SetCustomFilter(const SEARCHPARAMS& params, LPCTSTR szName)
{
	m_customFilter = params;
	m_sCustomFilter = szName;

	RestoreCustomFilter();

	return TRUE;
}

BOOL CFilteredToDoCtrl::RestoreCustomFilter()
{
	m_bCustomFilter = TRUE;

	RefreshTreeFilter();

	if (InListView())
	{
		RefreshListFilter();
	}
	else
	{
		m_bListNeedRefilter = TRUE;
	}

	return TRUE;
}

void CFilteredToDoCtrl::BuildFilterQuery(SEARCHPARAMS& params)
{
	// reset the search
	params.rules.RemoveAll();
	params.bIgnoreDone = m_filter.HasFlag(FT_HIDEDONE);

	// handle principle filter
	switch (m_filter.nFilter)
	{
	case FT_ALL:
		break;

	case FT_DONE:
		params.rules.Add(SEARCHPARAM(TDCA_DONEDATE, FO_SET));
		params.bIgnoreDone = FALSE;
		break;

	case FT_NOTDONE:
		params.rules.Add(SEARCHPARAM(TDCA_DONEDATE, FO_NOT_SET));
		params.bIgnoreDone = TRUE;
		break;

	case FT_STARTTODAY:
		{
			COleDateTime date = CDateHelper::GetDate(DHD_TODAY);
			params.rules.Add(SEARCHPARAM(TDCA_STARTDATE, FO_ON_OR_BEFORE, date.m_dt));
			params.rules.Add(SEARCHPARAM(TDCA_DONEDATE, FO_NOT_SET));
		}
		break;

	case FT_STARTENDTHISWEEK:
		{
			COleDateTime date = CDateHelper::GetDate(DHD_ENDTHISWEEK);
			params.rules.Add(SEARCHPARAM(TDCA_STARTDATE, FO_ON_OR_BEFORE, date.m_dt));
			params.rules.Add(SEARCHPARAM(TDCA_DONEDATE, FO_NOT_SET));
		}
		break;

	case FT_STARTNEXTSEVENDAYS:
		{
			COleDateTime date = CDateHelper::GetDate(DHD_TODAY);
			params.rules.Add(SEARCHPARAM(TDCA_STARTDATE, FO_ON_OR_AFTER, date.m_dt));
			params.rules.Add(SEARCHPARAM(TDCA_STARTDATE, FO_BEFORE, date.m_dt + 7));
			params.rules.Add(SEARCHPARAM(TDCA_DONEDATE, FO_NOT_SET));
		}
		break;

	case FT_FLAGGED:
		params.rules.Add(SEARCHPARAM(TDCA_FLAG, FO_SET));
		break;

	case FT_DUENEXTSEVENDAYS: // special case
		{
			COleDateTime date = CDateHelper::GetDate(DHD_TODAY);
			params.rules.Add(SEARCHPARAM(TDCA_DUEDATE, FO_ON_OR_AFTER, date.m_dt));
		}
		// then fall through

	case FT_DUETODAY:
	case FT_DUETOMORROW:
	case FT_DUEENDTHISWEEK:
	case FT_DUEENDNEXTWEEK:
	case FT_DUEENDTHISMONTH:
	case FT_DUEENDNEXTMONTH:
	case FT_DUEENDTHISYEAR:
	case FT_DUEENDNEXTYEAR:
		{
			COleDateTime dateDue;
			InitFilterDueDate(dateDue);

			if (dateDue.m_dt > 0.0)
			{
				//params.rules.Add(SEARCHPARAM(TDCA_DONEDATE, FO_NOT_SET));
				params.rules.Add(SEARCHPARAM(TDCA_DUEDATE, FO_ON_OR_BEFORE, dateDue.m_dt));
			}

			// this flag only applies to due filters
			params.bIgnoreOverDue = m_filter.HasFlag(FT_HIDEOVERDUE);
		}
		break;

	default:
		ASSERT(0); // to catch unimplemented filters
		break;
	}

	// handle other attributes
	AddNonDateFilterQueryRules(params);
}

void CFilteredToDoCtrl::AddNonDateFilterQueryRules(SEARCHPARAMS& params)
{
	// title text
	if (!m_filter.sTitle.IsEmpty())
	{
		params.rules.Add(SEARCHPARAM(TDCA_TASKNAME, FO_INCLUDES, m_filter.sTitle));
	}

	// note: these are all 'AND'
	// category
	if (m_filter.aCategories.GetSize())
	{
		CString sMatchBy = Misc::FormatArray(m_filter.aCategories);

		if (m_filter.aCategories.GetSize() == 1 && sMatchBy.IsEmpty())
		{
			params.rules.Add(SEARCHPARAM(TDCA_CATEGORY, FO_NOT_SET));
		}

		else if (m_filter.dwFlags & FT_ANYCATEGORY)
		{
			params.rules.Add(SEARCHPARAM(TDCA_CATEGORY, FO_INCLUDES, sMatchBy));
		}
		else
		{
			params.rules.Add(SEARCHPARAM(TDCA_CATEGORY, FO_EQUALS, sMatchBy));
		}
	}

	// allocated to
	if (m_filter.aAllocTo.GetSize())
	{
		CString sMatchBy = Misc::FormatArray(m_filter.aAllocTo);

		if (m_filter.aAllocTo.GetSize() == 1 && sMatchBy.IsEmpty())
		{
			params.rules.Add(SEARCHPARAM(TDCA_ALLOCTO, FO_NOT_SET));
		}

		else if (m_filter.dwFlags & FT_ANYALLOCTO)
		{
			params.rules.Add(SEARCHPARAM(TDCA_ALLOCTO, FO_INCLUDES, sMatchBy));
		}
		else
		{
			params.rules.Add(SEARCHPARAM(TDCA_ALLOCTO, FO_EQUALS, sMatchBy));
		}
	}

	// allocated by
	if (m_filter.aAllocBy.GetSize())
	{
		CString sMatchBy = Misc::FormatArray(m_filter.aAllocBy);

		if (m_filter.aAllocBy.GetSize() == 1 && sMatchBy.IsEmpty())
		{
			params.rules.Add(SEARCHPARAM(TDCA_ALLOCBY, FO_NOT_SET));
		}
		else
		{
			params.rules.Add(SEARCHPARAM(TDCA_ALLOCBY, FO_INCLUDES, sMatchBy));
		}
	}

	// status
	if (m_filter.aStatus.GetSize())
	{
		CString sMatchBy = Misc::FormatArray(m_filter.aStatus);

		if (m_filter.aStatus.GetSize() == 1 && sMatchBy.IsEmpty())
		{
			params.rules.Add(SEARCHPARAM(TDCA_STATUS, FO_NOT_SET));
		}
		else
		{
			params.rules.Add(SEARCHPARAM(TDCA_STATUS, FO_INCLUDES, sMatchBy));
		}
	}

	// version
	if (m_filter.aVersions.GetSize())
	{
		CString sMatchBy = Misc::FormatArray(m_filter.aVersions);

		if (m_filter.aVersions.GetSize() == 1 && sMatchBy.IsEmpty())
		{
			params.rules.Add(SEARCHPARAM(TDCA_VERSION, FO_NOT_SET));
		}
		else
		{
			params.rules.Add(SEARCHPARAM(TDCA_VERSION, FO_INCLUDES, sMatchBy));
		}
	}

	// priority
	if (m_filter.nPriority != FT_ANYPRIORITY)
	{
		if (m_filter.nPriority == FT_NOPRIORITY)
		{
			params.rules.Add(SEARCHPARAM(TDCA_PRIORITY, FO_NOT_SET));
		}

		else if (m_filter.nPriority != FT_ANYPRIORITY)
		{
			params.rules.Add(SEARCHPARAM(TDCA_PRIORITY, FO_GREATER_OR_EQUAL, m_filter.nPriority));
		}
	}

	// risk
	if (m_filter.nRisk != FT_ANYRISK)
	{
		if (m_filter.nRisk == FT_NORISK)
		{
			params.rules.Add(SEARCHPARAM(TDCA_RISK, FO_NOT_SET));
		}

		else if (m_filter.nRisk != FT_ANYRISK)
		{
			params.rules.Add(SEARCHPARAM(TDCA_RISK, FO_GREATER_OR_EQUAL, m_filter.nRisk));
		}
	}

	// special case: no rules + ignore completed
	if ((params.bIgnoreDone) && params.rules.GetSize() == 0)
	{
		params.rules.Add(SEARCHPARAM(TDCA_DONEDATE, FO_NOT_SET));
	}
}

LRESULT CFilteredToDoCtrl::OnRefreshFilter(WPARAM wParam, LPARAM lParam)
{
	BOOL bUndo = lParam;
	FTC_VIEW nView = (FTC_VIEW)wParam;

	if (nView == FTCV_TASKTREE)
	{
		RefreshFilter();
	}

	// if undoing then we must also refresh the list filter because
	// otherwise ResyncListSelection will fail in the case where
	// we are undoing a delete because the undone item will not yet be in the list.
	if (nView == FTCV_TASKLIST || bUndo)
	{
		RefreshListFilter();
	}

	// resync selection?
	if (bUndo)
	{
		ResyncListSelection();
	}

	return 0L;
}

void CFilteredToDoCtrl::RefreshFilter()
{
	RefreshTreeFilter(); // always

	if (InListView())
	{
		RefreshListFilter();
	}
	else // tree view
	{
		m_bListNeedRefilter = TRUE;
	}
}

void CFilteredToDoCtrl::RefreshTreeFilter()
{
	if (!m_data.GetTaskCount())
	{
		return;
	}

	BOOL bTreeVis = !InListView();

	// save and reset current focus to work around a bug
	CWnd* pFocus = CWnd::GetFocus();
	SetFocusToTasks();

	CHoldRedraw hr(GetSafeHwnd());
	CHoldRedraw hr2(m_tree);
	CWaitCursor cursor;

	// cache current selection
	CDWordArray aTaskIDs;
	DWORD dwFocusedTaskID, dwPrevID = 0;

	GetSelectedTaskIDs(aTaskIDs, dwFocusedTaskID, FALSE);

	// cache previous task to selected item
	if (aTaskIDs.GetSize() == 1)
	{
		HTREEITEM htiPrev = m_tree.TCH().GetPrevVisibleItem(GetSelectedItem());
		dwPrevID = GetTaskID(htiPrev);
	}

	Selection().RemoveAll();

	// and scrolled pos
	DWORD dwFirstVis = bTreeVis ? GetTaskID(m_tree.GetFirstVisibleItem()) : 0;

	// and expanded selection
	CPreferences prefs;
	SaveExpandedState(prefs);

	// build a find query that matches the filter
	SEARCHPARAMS filter;

	if (m_bCustomFilter)
	{
		filter = m_customFilter;
	}
	else
	{
		BuildFilterQuery(filter);
	}

	// rebuild the tree
	RebuildTree(filter);

	// redo last sort
	if (bTreeVis && IsSortable())
	{
		Resort();
		m_bTreeNeedResort = FALSE;
	}
	else
	{
		m_bTreeNeedResort = TRUE;
	}

	// restore expanded state
	LoadExpandedState(prefs);

	// restore scrolled pos
	if (dwFirstVis)
	{
		HTREEITEM hti = m_find.GetItem(dwFirstVis);
		m_tree.SelectSetFirstVisible(hti);
		m_tree.SelectItem(NULL);
	}

	// restore selection
	RestoreTreeSelection(aTaskIDs, dwPrevID);

	// restore focused window
	if (pFocus)
	{
		pFocus->SetFocus();
	}
}

void CFilteredToDoCtrl::RebuildTree(const SEARCHPARAMS& filter)
{
	m_tree.TCH().SelectItem(NULL);
	m_tree.DeleteAllItems();

	BuildTreeItem(NULL, m_data.GetStructure(), filter);
}

BOOL CFilteredToDoCtrl::BuildTreeItem(HTREEITEM hti, const TODOSTRUCTURE* pTDS, const SEARCHPARAMS& filter)
{
	// sanity checks
	if (pTDS)
	{
		if (hti == NULL || hti == TVI_ROOT)
		{
			ASSERT(pTDS->GetTaskID() == 0);

			if (pTDS->GetTaskID() != 0)
			{
				return FALSE;
			}
		}
		else
		{
			ASSERT(m_find.GetTaskID(hti) == pTDS->GetTaskID());

			if (m_find.GetTaskID(hti) != pTDS->GetTaskID())
			{
				return FALSE;
			}
		}
	}
	else
	{
		ASSERT(hti);
		ASSERT(!m_tree.ItemHasChildren(hti));

		DWORD dwID = m_find.GetTaskID(hti);
		pTDS = m_data.LocateTask(dwID);
	}

	// rebuild
	HTREEITEM htiAfter = TVI_FIRST;

	for (int nSubtask = 0; nSubtask < pTDS->GetSubTaskCount(); nSubtask++)
	{
		const TODOSTRUCTURE* pTDSChild = pTDS->GetSubTask(nSubtask);
		DWORD dwID = pTDSChild->GetTaskID();

		const TODOITEM* pTDIChild = GetTask(dwID);
		ASSERT(pTDIChild);

		// does this task match the filter
		BOOL bMatch = TRUE;
		BOOL bHasChildren = pTDSChild->HasSubTasks();

		if (!bHasChildren)
		{
			SEARCHRESULT result;
			bMatch = m_data.TaskMatches(pTDIChild, pTDSChild, filter, result);
		}

		if (bMatch)
		{
			// add this item to tree
			BOOL bBold = (pTDSChild->ParentIsRoot());
			HTREEITEM htiChild = m_tree.InsertItem(TVIF_TEXT | TVIF_PARAM | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE,
				LPSTR_TEXTCALLBACK,
				I_IMAGECALLBACK,
				I_IMAGECALLBACK,
				(bBold ? TVIS_BOLD : 0),
				TVIS_BOLD, // state mask
				dwID, // lParam
				hti,
				htiAfter);

			ASSERT(htiChild);

			// check state
			m_tree.TCH().SetItemChecked(htiChild, pTDIChild->IsDone());

			// and its children
			if (bHasChildren)
			{
				BuildTreeItem(htiChild, pTDSChild, filter);

				// if no subtasks got added to the tree then we may need to remove the parent too
				if (!m_tree.ItemHasChildren(htiChild))
				{
					SEARCHRESULT result;
					bMatch = m_data.TaskMatches(pTDIChild, pTDSChild, filter, result);

					if (!bMatch)
					{
						m_tree.DeleteItem(htiChild);
						htiChild = NULL;
					}
				}
			}

			if (htiChild)
			{
				htiAfter = htiChild;
			}
		}
	}

	return TRUE;
}

void CFilteredToDoCtrl::RefreshListFilter()
{
	m_bListNeedRefilter = FALSE;

	if (!m_data.GetTaskCount())
	{
		m_list.DeleteAllItems();
		return;
	}

	// note: the call to CListCtrl::Scroll at the bottom fails if the
	// list has redraw disabled so it must happen outside the scope of hr2
	CSize sizePos(0, 0);
	{
		CHoldRedraw hr(GetSafeHwnd());
		CHoldRedraw hr2(m_list);
		CWaitCursor cursor;

		// cache current selection
		CDWordArray aTaskIDs;
		DWORD dwFocusedTaskID;
		GetSelectedTaskIDs(aTaskIDs, dwFocusedTaskID, FALSE);

		// and scrolled pos
		if (m_list.GetItemCount())
		{
			CRect rItem;
			m_list.GetItemRect(0, rItem, LVIR_BOUNDS);

			sizePos.cy = -rItem.top + rItem.Height();
		}

		// build a find query that matches the filter
		SEARCHPARAMS filter;

		if (m_bCustomFilter)
		{
			filter = m_customFilter;
		}
		else
		{
			BuildFilterQuery(filter);
		}

		// rebuild the list
		RebuildList(filter);

		// redo last sort
		if (IsSortable())
		{
			Resort();
			m_bListNeedResort = FALSE;
		}

		// restore selection
		SetSelectedListTasks(aTaskIDs, dwFocusedTaskID);

		// don't update controls is only one item is selected and it did not
		// change as a result of the filter
		BOOL bSelChange = !(GetSelectedCount() == 1 && aTaskIDs.GetSize() == 1 &&
			GetTaskID(GetSelectedItem()) == aTaskIDs[0]);

		if (bSelChange)
		{
			UpdateControls();
		}
	}

	// restore pos
	if (sizePos.cy != 0)
	{
		m_list.Scroll(sizePos);
	}

	UpdateColumnWidths();
}

void CFilteredToDoCtrl::RebuildList(const SEARCHPARAMS& filter)
{
	// remove all existing items
	m_list.DeleteAllItems();

	// since the tree will have already go the items we want
	// we can optimize the rebuild under certain circumstances
	// which are:
	// 1. the list is sorted
	// 2. or the tree is unsorted
	// otherwise we need the data in it's unsorted state and the
	// tree doesn't have it
	if (IsSortable(m_sortList.nBy1) || !IsSortable(m_sort.nBy1))
	{
		// rebuild the list from the tree
		AddTreeItemToList(NULL, filter);
	}
	else // rebuild from scratch
	{
		// do the find
		CResultArray aResults;
		m_data.FindTasks(filter, aResults);

		// add tasks to list
		for (int nRes = 0; nRes < aResults.GetSize(); nRes++)
		{
			const SEARCHRESULT& res = aResults[nRes];

			// some more filtering required
			if (m_filter.HasFlag(FT_HIDEPARENTS))
			{
				TODOSTRUCTURE* pTDS = m_data.LocateTask(res.dwID);
				ASSERT(pTDS);

				if (pTDS->HasSubTasks())
				{
					continue;
				}
			}
			else if (m_filter.HasFlag(FT_HIDECOLLAPSED))
			{
				HTREEITEM hti = m_find.GetItem(res.dwID);
				ASSERT(hti);

				if (m_tree.ItemHasChildren(hti) && !m_tree.TCH().IsItemExpanded(hti))
				{
					continue;
				}
			}

			int nItem = m_list.InsertItem(nRes, LPSTR_TEXTCALLBACK, I_IMAGECALLBACK);
			m_list.SetItemData(nItem, res.dwID);
		}
	}
}

void CFilteredToDoCtrl::AddTreeItemToList(HTREEITEM hti, const SEARCHPARAMS& filter)
{
	if (hti)
	{
		BOOL bAdd = TRUE;
		DWORD dwTaskID = GetTaskID(hti);

		// check if parent items are to be ignored
		if (m_filter.HasFlag(FT_HIDEPARENTS))
		{
			// quick test first
			if (m_tree.ItemHasChildren(hti))
			{
				bAdd = FALSE;
			}
			else
			{
				const TODOSTRUCTURE* pTDS = m_data.LocateTask(dwTaskID);
				ASSERT(pTDS);

				bAdd = !pTDS->HasSubTasks();
			}
		}
		// else check if it's a parent item that's only present because
		// it has matching subtasks
		else if (m_tree.ItemHasChildren(hti))
		{
			const TODOSTRUCTURE* pTDS = m_data.LocateTask(dwTaskID);
			const TODOITEM* pTDI = GetTask(dwTaskID);
			SEARCHRESULT result;

			// ie. check that parent actually matches
			bAdd = m_data.TaskMatches(pTDI, pTDS, filter, result);
		}

		if (bAdd)
		{
			int nItem = m_list.InsertItem(m_list.GetItemCount(), LPSTR_TEXTCALLBACK, I_IMAGECALLBACK);
			m_list.SetItemData(nItem, GetTaskID(hti));
		}
	}

	// always check the children unless collapsed tasks ignored
	if (!m_filter.HasFlag(FT_HIDECOLLAPSED) || !hti || m_tree.TCH().IsItemExpanded(hti))
	{
		HTREEITEM htiChild = m_tree.GetChildItem(hti);

		while (htiChild)
		{
			// check
			AddTreeItemToList(htiChild, filter);
			htiChild = m_tree.GetNextItem(htiChild, TVGN_NEXT);
		}
	}
}

void CFilteredToDoCtrl::OnHeaderCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMCUSTOMDRAW* pNMCD = (NMCUSTOMDRAW*)pNMHDR;

	if (pNMCD->dwDrawStage == CDDS_PREPAINT)
	{
		*pResult |= CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
	}

	else if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT)
	{
		*pResult |= CDRF_NOTIFYPOSTPAINT;
	}

	else if (pNMCD->dwDrawStage == CDDS_ITEMPOSTPAINT)
	{
		CDC* pDC = CDC::FromHandle(pNMCD->hdc);
		CFont* pOldFont = (CFont*)pDC->SelectObject(CWnd::GetFont());

		// clip column to client rect
		CRect rHeader;
		m_header.GetClientRect(rHeader);
		pNMCD->rc.right = min(pNMCD->rc.right, rHeader.right);

		DrawColumnHeaderText(pDC, pNMCD->dwItemSpec, pNMCD->rc, pNMCD->uItemState);

		pDC->SelectObject(pOldFont);
	}
}

void CFilteredToDoCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (nIDCtl == IDC_FTC_TASKLIST)
	{
		lpMeasureItemStruct->itemHeight = m_tree.TCH().GetItemHeight();
	}
	else
	{
		CToDoCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
	}
}

void CFilteredToDoCtrl::RemoveDeletedListItems()
{
	int nItem = m_list.GetItemCount();

	while (nItem--)
	{
		DWORD dwTaskID = m_list.GetItemData(nItem);

		if (!GetTask(dwTaskID))
		{
			m_list.DeleteItem(nItem);
		}
	}
}

CFilteredToDoCtrl::TDI_STATE CFilteredToDoCtrl::GetItemState(int nItem)
{
	if (m_list.GetItemState(nItem, LVIS_DROPHILITED) & LVIS_DROPHILITED)
	{
		return TDIS_DROPHILITED;
	}
	else if (m_list.GetItemState(nItem, LVIS_SELECTED) & LVIS_SELECTED)
	{
		return (TasksHaveFocus() ? TDIS_SELECTED : TDIS_SELECTEDNOTFOCUSED);
	}

	return TDIS_NONE;
}

void CFilteredToDoCtrl::GetItemColors(int nItem, NCGITEMCOLORS& colors)
{
	TDI_STATE nState = GetItemState(nItem);
	DWORD dwID = GetTaskID(nItem);

	colors.crText = GetSysColor(COLOR_WINDOWTEXT);
	colors.crBack = GetSysColor(COLOR_WINDOW);

	if (nItem % 2)
	{
		COLORREF crAlt = m_tree.GetAlternateLineColor();

		if (crAlt != NOCOLOR)
		{
			colors.crBack = crAlt;
		}
	}

	CToDoCtrl::GetItemColors(dwID, &colors, nState);
}

void CFilteredToDoCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == IDC_FTC_TASKLIST)
	{
		// don't bother drawing if we're just switching to the tree view
		if (!InListView())
		{
			return;
		}

		int nItem = lpDrawItemStruct->itemID;
		DWORD dwTaskID = lpDrawItemStruct->itemData;

		TODOITEM* pTDI = NULL;
		TODOSTRUCTURE* pTDS = NULL;

		if (!m_data.GetTask(dwTaskID, pTDI, pTDS))
		{
			return;
		}

		CRect rItem, rTitleBounds;

		m_list.GetSubItemRect(nItem, 0, LVIR_BOUNDS, rItem);
		GetItemTitleRect(nItem, TDCTR_BOUNDS, rTitleBounds);

		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		int nSaveDC = pDC->SaveDC(); // so that DrawFocusRect works

		pDC->SetBkMode(TRANSPARENT);

		COLORREF crGrid = m_tree.GetGridlineColor();
		NCGITEMCOLORS colors = { 0, 0, 0, FALSE, FALSE };

		GetItemColors(nItem, colors);
		TDI_STATE nState = GetItemState(nItem);

		// fill back color
		if (HasStyle(TDCS_FULLROWSELECTION) || !colors.bBackSet)
		{
			DrawItemBackColor(pDC, rItem, colors.crBack);
		}
		else
		{
			// calculate the rect containing the rest of the columns
			CRect rCols(rItem);

			if (HasStyle(TDCS_RIGHTSIDECOLUMNS))
			{
				rCols.left = rTitleBounds.right;
			}
			else
			{
				rCols.right = rTitleBounds.left;
			}

			// fill the background colour of the rest of the columns
			rCols.right--;

			DrawItemBackColor(pDC, rCols, colors.crBack);

			// check to see whether we should fill the label cell background
			// with the alternate line colour
			if (nItem % 2) // odd row
			{
				COLORREF crAltLine = m_tree.GetAlternateLineColor();

				if (crAltLine != NOCOLOR)
				{
					rTitleBounds.OffsetRect(-1, 0);
					pDC->FillSolidRect(rTitleBounds, crAltLine);
					rTitleBounds.OffsetRect(1, 0);
				}
			}
		}

		// and set the text color
		if (colors.bTextSet)
		{
			pDC->SetTextColor(colors.crText);
		}
		else
		{
			pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		}

		// render column text
		// use CToDoCtrl::OnGutterDrawItem to do the hard work
		NCGDRAWITEM ncgDI;
		CRect rFocus; // will be set up during TDCC_CLIENT handling

		for (int nCol = 0; nCol < NUM_COLUMNS; nCol++)
		{
			// client column/task title is special case
			TDCCOLUMN* pCol = GetColumn(nCol);

			if (pCol->nColID == TDCC_CLIENT)
			{
				BOOL bTopLevel = pTDS->ParentIsRoot();
				BOOL bFolder = pTDS->HasSubTasks() && HasStyle(TDCS_SHOWPARENTSASFOLDERS);
				BOOL bDoneAndStrikeThru = pTDI->IsDone() && HasStyle(TDCS_STRIKETHOUGHDONETASKS);

				// icon
				CRect rSubItem;

				if (GetItemTitleRect(nItem, TDCTR_ICON, rSubItem))
				{
					if (pTDI->nIconIndex >= 0)
					{
						m_ilTaskIcons.Draw(pDC, pTDI->nIconIndex, rSubItem.TopLeft(), ILD_TRANSPARENT);
					}

					else if (bFolder)
					{
						m_ilTaskIcons.Draw(pDC, 0, rSubItem.TopLeft(), ILD_TRANSPARENT);
					}
				}

				// checkbox
				if (GetItemTitleRect(nItem, TDCTR_CHECKBOX, rSubItem))
				{
					int nImage = pTDI->IsDone() ? 2 : 1;
					CImageList::FromHandle(m_hilDone)->Draw(pDC, nImage, rSubItem.TopLeft(), ILD_TRANSPARENT);
				}

				// set fonts before calculating title rect
				HFONT hFontOld = NULL;

				if (bDoneAndStrikeThru)
				{
					hFontOld = (HFONT)::SelectObject(lpDrawItemStruct->hDC, m_fontDone);
				}

				else if (bTopLevel)
				{
					hFontOld = (HFONT)::SelectObject(lpDrawItemStruct->hDC, m_fontBold);
				}

				// Task title
				GetItemTitleRect(nItem, TDCTR_LABEL, rSubItem, pDC, pTDI->sTitle);
				rSubItem.OffsetRect(-1, 0);

				// back colour
				if (!HasStyle(TDCS_FULLROWSELECTION) && colors.bBackSet)
				{
					DrawItemBackColor(pDC, rSubItem, colors.crBack);
				}

				// text
				DrawGutterItemText(pDC, pTDI->sTitle, rSubItem, LVCFMT_LEFT);
				rFocus = rSubItem; // save for focus rect drawing

				// vertical divider
				if (crGrid != NOCOLOR)
				{
					pDC->FillSolidRect(rTitleBounds.right - 1, rTitleBounds.top, 1, rTitleBounds.Height(), crGrid);
				}

				// render comment text if not editing this task label
				if (m_dwEditingID != dwTaskID)
				{
					// deselect bold font if set
					if (bTopLevel && !bDoneAndStrikeThru)
					{
						::SelectObject(lpDrawItemStruct->hDC, hFontOld);
						hFontOld = NULL;
					}

					rTitleBounds.top++;
					rTitleBounds.left = rSubItem.right + 4;
					DrawCommentsText(pDC, pTDI, pTDS, rTitleBounds, nState);
				}

				if (hFontOld)
				{
					::SelectObject(lpDrawItemStruct->hDC, hFontOld);
				}
			}
			else
			{
				// get col rect
				CRect rSubItem;
				m_list.GetSubItemRect(nItem, nCol, LVIR_LABEL, rSubItem);
				rSubItem.OffsetRect(-1, 0);

				ncgDI.pDC = pDC;
				ncgDI.dwItem = NULL;
				ncgDI.dwParentItem = NULL;
				ncgDI.nLevel = 0;
				ncgDI.nItemPos = 0;
				ncgDI.rWindow = &rSubItem;
				ncgDI.rItem = &rSubItem;
				ncgDI.nColID = pCol->nColID;
				ncgDI.nTextAlign = pCol->nAlignment;

				DrawItem(pTDI, pTDS, &ncgDI, nState);
			}
		}

		// base gridline
		if (crGrid != NOCOLOR)
		{
			pDC->FillSolidRect(rItem.left, rItem.bottom - 1, rItem.Width() - 1, 1, crGrid);
		}

		pDC->RestoreDC(nSaveDC); // so that DrawFocusRect works

		// focus rect
		if ((lpDrawItemStruct->itemState & ODS_FOCUS) && (nState == TDIS_SELECTED))
		{
			pDC->DrawFocusRect(rFocus);
		}

	}
	else
	{
		CToDoCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}

COLORREF CFilteredToDoCtrl::GetItemLineColor(HTREEITEM hti)
{
	if (InListView())
	{
		COLORREF crAltLines = m_tree.GetAlternateLineColor();

		if (crAltLines != NOCOLOR)
		{
			int nItem = FindTask(GetTaskID(hti));

			if (nItem != -1 && (nItem % 2))
			{
				return crAltLines;
			}
		}

		// else
		return GetSysColor(COLOR_WINDOW);
	}

	// else
	return CToDoCtrl::GetItemLineColor(hti);
}

void CFilteredToDoCtrl::DrawColumnHeaderText(CDC* pDC, int nCol, const CRect& rCol, UINT nState)
{
	if (nCol >= NUM_COLUMNS)
	{
		return;
	}

	// filter out zero width columns
	if (rCol.Width() == 0)
	{
		return;
	}

	const TDCCOLUMN* pCol = GetColumn(nCol);
	ASSERT(pCol);

	CString sColumn;
	sColumn.LoadString(pCol->nIDName);

	// special cases
	if (sColumn.IsEmpty())
	{
		sColumn = CString(static_cast<TCHAR>(pCol->nIDName));
	}

	else if (pCol->nColID == TDCC_CLIENT && HasStyle(TDCS_SHOWPATHINHEADER) && m_list.GetSelectedCount() == 1)
	{
		int nColWidthInChars = (int)(rCol.Width() / m_fAveHeaderCharWidth);
		CString sPath = m_data.GetTaskPath(GetSelectedTaskID(), FALSE, nColWidthInChars);

		if (!sPath.IsEmpty())
		{
			sColumn.Format(_T("%s [%s]"), CEnString(IDS_TDC_COLUMN_TASK), sPath);
		}
	}
	else if (sColumn == _T("%%"))
	{
		sColumn = _T("%");
	}

	const UINT DEFFLAGS = DT_END_ELLIPSIS | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
	pDC->SetBkMode(TRANSPARENT);

	CRect rText(rCol);
	rText.bottom -= 2;

	switch (pCol->nAlignment)
	{
	case DT_LEFT:
		rText.left += NCG_COLPADDING - 1;
		break;

	case DT_RIGHT:
		rText.right -= NCG_COLPADDING;
		break;

	case DT_CENTER:
		rText.right--;
	}

	BOOL bDown = (nState & CDIS_SELECTED);
	BOOL bSorted = (IsSortable() && m_sortList.nBy1 == pCol->nSortBy && !m_bListMultiSort);

	if (bDown)
	{
		rText.OffsetRect(1, 1);
	}

	if (bSorted)
	{
		rText.right -= (SORTWIDTH + 2);
	}

	if (pCol->szFont && *pCol->szFont)
	{
		CFont* pOldFont = NULL;

		if (CString(pCol->szFont).CompareNoCase(_T("WingDings")) == 0)
		{
			pOldFont = pDC->SelectObject(&GraphicsMisc::WingDings());
		}

		else if (CString(pCol->szFont).CompareNoCase(_T("Marlett")) == 0)
		{
			pOldFont = pDC->SelectObject(&GraphicsMisc::Marlett());
		}

		pDC->DrawText(sColumn, rText, DEFFLAGS | pCol->nAlignment);

		if (pOldFont)
		{
			pDC->SelectObject(pOldFont);
		}
	}
	else
	{
		pDC->DrawText(sColumn, rText, DEFFLAGS | pCol->nAlignment);
	}

	// draw sort direction if required
	if (bSorted)
	{
		// adjust for sort arrow
		if (pCol->nAlignment & DT_CENTER)
		{
			rText.left = ((rText.left + rText.right + pDC->GetTextExtent(sColumn).cx) / 2) + 2;
		}

		else if (pCol->nAlignment & DT_RIGHT)
		{
			rText.left = rText.right + 2;
		}
		else
		{
			rText.left = rText.left + pDC->GetTextExtent(sColumn).cx + 2;
		}

		BOOL bAscending = m_sortList.bAscending1 ? 1 : -1;
		int nMidY = (bDown ? 1 : 0) + (rText.top + rText.bottom) / 2;
		POINT ptArrow[3] =
		{
			{ 0, 0 },
			{ 3, -bAscending * 3 },
			{ 7, bAscending }
		};

		// translate the arrow to the appropriate location
		int nPoint = 3;

		while (nPoint--)
		{
			ptArrow[nPoint].x += rText.left + 3 + (bDown ? 1 : 0);
			ptArrow[nPoint].y += nMidY;
		}
		pDC->Polyline(ptArrow, 3);
	}
}

TDC_COLUMN CFilteredToDoCtrl::GetColumnID(int nCol) const
{
	TDCCOLUMN* pCol = GetColumn(nCol);
	return pCol ? pCol->nColID : (TDC_COLUMN)-1;
}

TDCCOLUMN* CFilteredToDoCtrl::GetColumn(int nCol) const
{
	if (nCol < 0 || nCol >= NUM_COLUMNS)
	{
		return NULL;
	}

	// else
	if (HasStyle(TDCS_RIGHTSIDECOLUMNS))
	{
		if (nCol == 0)
		{
			return &COLUMNS[NUM_COLUMNS - 1];
		}
		else
		{
			return &COLUMNS[nCol - 1];
		}
	}

	// else
	return &COLUMNS[nCol];
}

int CFilteredToDoCtrl::GetColumnIndex(TDC_COLUMN nColID) const
{
	if (HasStyle(TDCS_RIGHTSIDECOLUMNS))
	{
		if (nColID == TDCC_CLIENT)
		{
			return 0;
		}

		// else it's 1 more than the COLUMNS index
		int nCol = NUM_COLUMNS - 1;

		while (nCol--)
		{
			if (COLUMNS[nCol].nColID == nColID)
			{
				return nCol + 1;
			}
		}

		ASSERT(0);
		return 0;
	}

	// else as COLUMNS
	int nCol = NUM_COLUMNS;

	while (nCol--)
	{
		if (COLUMNS[nCol].nColID == nColID)
		{
			return nCol;
		}
	}

	ASSERT(0);
	return 0;
}

BOOL CFilteredToDoCtrl::SetStyles(const CTDCStylesMap& styles)
{
	if (CToDoCtrl::SetStyles(styles))
	{
		// do we need to refilter?
		if (m_bListNeedRefilter)
		{
			RefreshTreeFilter(); // always

			if (InListView())
			{
				RefreshListFilter();
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CFilteredToDoCtrl::SetStyle(TDC_STYLE nStyle, BOOL bOn, BOOL bWantUpdate)
{
	// base class processing
	if (CToDoCtrl::SetStyle(nStyle, bOn, bWantUpdate))
	{
		// post-precessing
		switch (nStyle)
		{
		case TDCS_DUEHAVEHIGHESTPRIORITY:
		case TDCS_DONEHAVELOWESTPRIORITY:
			m_bListNeedRefilter = TRUE;
			break;

		case TDCS_TREATSUBCOMPLETEDASDONE:
			if (m_filter.nFilter != FT_ALL)
			{
				m_bListNeedRefilter = TRUE;
			}
			break;

		case TDCS_RIGHTSIDECOLUMNS:
			BuildListColumns();
			break;

		case TDCS_SHOWINFOTIPS:
			ListView_SetExtendedListViewStyleEx(m_list, LVS_EX_INFOTIP, bOn ? LVS_EX_INFOTIP : 0);
			break;

		case TDCS_TREETASKICONS:
			if (bOn && !IsColumnShowing(TDCC_ICON)) // this overrides
			{
				ListView_SetImageList(m_list, m_ilTaskIcons, LVSIL_NORMAL);
			}
			else
			{
				ListView_SetImageList(m_list, NULL, LVSIL_NORMAL);
			}
			break;

		case TDCS_SHOWTREELISTBAR:
			m_tabCtrl.ShowWindow(bOn ? SW_SHOW : SW_HIDE);
			m_tabCtrl.EnableWindow(bOn);

			if (bWantUpdate)
			{
				Resize();
			}
			break;
		}

		return TRUE;
	}

	return FALSE;
}

CString CFilteredToDoCtrl::GetControlDescription(CWnd* pCtrl) const
{
	if (pCtrl && *pCtrl == m_list)
	{
		return CEnString(IDS_LISTVIEW);
	}

	// else
	return CToDoCtrl::GetControlDescription(pCtrl);
}

void CFilteredToDoCtrl::InitFilterDueDate(COleDateTime& dateDue) const
{
	switch (m_filter.nFilter)
	{
	case FT_ALL:
	case FT_DONE:
	case FT_NOTDONE:
	case FT_FLAGGED:
	case FT_STARTTODAY:
	case FT_STARTENDTHISWEEK:
		dateDue.m_dt = 0;
		break;

	case FT_DUETODAY:
		dateDue = CDateHelper::GetDate(DHD_TODAY);
		break;

	case FT_DUETOMORROW:
		dateDue = CDateHelper::GetDate(DHD_TOMORROW);
		break;

	case FT_DUEENDTHISWEEK:
		dateDue = CDateHelper::GetDate(DHD_ENDTHISWEEK);
		break;

	case FT_DUEENDNEXTWEEK:
		dateDue = CDateHelper::GetDate(DHD_ENDNEXTWEEK);
		break;

	case FT_DUEENDTHISMONTH:
		dateDue = CDateHelper::GetDate(DHD_ENDTHISMONTH);
		break;

	case FT_DUEENDNEXTMONTH:
		dateDue = CDateHelper::GetDate(DHD_ENDNEXTMONTH);
		break;

	case FT_DUEENDTHISYEAR:
		dateDue = CDateHelper::GetDate(DHD_ENDTHISYEAR);
		break;

	case FT_DUEENDNEXTYEAR:
		dateDue = CDateHelper::GetDate(DHD_ENDNEXTYEAR);
		break;

	case FT_DUENEXTSEVENDAYS:
		dateDue = CDateHelper::GetDate(DHD_TODAY) + 6; // +6 because filter is FO_ON_OR_BEFORE
		break;

	default:
		ASSERT(0); // to help highlight missing filters
		break;
	}
}

BOOL CFilteredToDoCtrl::DeleteSelectedTask(BOOL bWarnUser, BOOL bResetSel)
{
	BOOL bDel = CToDoCtrl::DeleteSelectedTask(bWarnUser, bResetSel);

	if (bDel && InListView())
	{
		// work out what to select
		DWORD dwSelID = GetSelectedTaskID();
		int nSel = FindTask(dwSelID);

		if (nSel == -1 && m_list.GetItemCount())
		{
			nSel = 0;
		}

		if (nSel != -1)
		{
			SelectTask(m_list.GetItemData(nSel));
		}
	}

	return bDel;
}

BOOL CFilteredToDoCtrl::SelectedTasksHaveChildren() const
{
	return CToDoCtrl::SelectedTasksHaveChildren();
}

HTREEITEM CFilteredToDoCtrl::NewTask(LPCTSTR szText, TDC_INSERTWHERE nWhere, BOOL bEditText)
{
	BOOL bWantEditText = bEditText;

	if (InListView())
	{
		bEditText = FALSE;
	}

	HTREEITEM htiNew = CToDoCtrl::NewTask(szText, nWhere, bEditText);

	if (htiNew)
	{
		if (InListView())
		{
			DWORD dwTaskID = GetTaskID(htiNew);
			ASSERT(dwTaskID == m_dwNextUniqueID - 1);

			// make the new task appear
			RefreshListFilter();

			// select that task
			SelectTask(dwTaskID);

			if (bWantEditText)
			{
				m_dwLastAddedID = dwTaskID;
				EditSelectedTask(TRUE);
			}
		}
		else
		{
			m_bListNeedRefilter = TRUE;
		}
	}

	return htiNew;
}

BOOL CFilteredToDoCtrl::SplitSelectedTask(int nNumSubtasks)
{
	if (CToDoCtrl::SplitSelectedTask(nNumSubtasks))
	{
		if (InListView())
		{
			RefreshListFilter();
		}

		return TRUE;
	}

	return FALSE;
}

void CFilteredToDoCtrl::SetModified(BOOL bMod, TDC_ATTRIBUTE nAttrib)
{
	if (bMod)
	{
		m_data.ResetCachedCalculations();

		if (ModNeedsRefilter(nAttrib, FTCV_TASKTREE))
		{
			PostMessage(WM_TDC_REFRESHFILTER, FTCV_TASKTREE, (nAttrib == TDCA_UNDO));
		}

		else if (ModNeedsRefilter(nAttrib, FTCV_TASKLIST))
		{
			if (InListView())
			{
				PostMessage(WM_TDC_REFRESHFILTER, FTCV_TASKLIST, (nAttrib == TDCA_UNDO));
			}
			else
			{
				m_bListNeedRefilter = TRUE;
			}
		}

		if (nAttrib == TDCA_DELETE || nAttrib == TDCA_ARCHIVE)
		{
			RemoveDeletedListItems();
		}

		else if (InListView())
		{
			m_list.Invalidate(FALSE);
		}
	}

	CToDoCtrl::SetModified(bMod, nAttrib);
}

BOOL CFilteredToDoCtrl::ModNeedsResort(TDC_ATTRIBUTE nModType) const
{
	// need to check all three sort attributes if multisorting
	BOOL bListNeedsResort = CToDoCtrl::ModNeedsResort(nModType, m_sortList.nBy1);

	if (!bListNeedsResort && m_bListMultiSort)
	{
		bListNeedsResort |= CToDoCtrl::ModNeedsResort(nModType, m_sortList.nBy2);

		if (!bListNeedsResort)
		{
			bListNeedsResort |= CToDoCtrl::ModNeedsResort(nModType, m_sortList.nBy3);
		}
	}

	BOOL bTreeNeedsResort = CToDoCtrl::ModNeedsResort(nModType);

	if (InListView())
	{
		m_bTreeNeedResort |= bTreeNeedsResort;
		return bListNeedsResort;
	}
	else // tree view
	{
		m_bListNeedResort |= bListNeedsResort;
		return bTreeNeedsResort;
	}
}

void CFilteredToDoCtrl::ResortSelectedTaskParents()
{
	// do a full sort
	Resort();
}

TDC_SORTBY CFilteredToDoCtrl::GetSortBy() const
{
	return InListView() ? m_sortList.nBy1 : m_sort.nBy1;
}

void CFilteredToDoCtrl::GetSortBy(TDSORTCOLUMNS& sort) const
{
	if (InListView())
	{
		sort = m_sortList;
	}
	else
	{
		CToDoCtrl::GetSortBy(sort);
	}
}

BOOL CFilteredToDoCtrl::ModNeedsRefilter(TDC_ATTRIBUTE nModType, FTC_VIEW nView) const
{
	if (!HasStyle(TDCS_REFILTERONMODIFY))
	{
		return FALSE;
	}

	BOOL bNeedRefilter = FALSE;

	if (m_bCustomFilter)
	{
		bNeedRefilter = m_customFilter.HasAttribute(nModType);
	}
	else if (HasFilter())
	{
		switch (nModType)
		{
		case TDCA_TASKNAME:
			bNeedRefilter = !m_filter.sTitle.IsEmpty();
			break;

		case TDCA_PRIORITY:
			bNeedRefilter = (m_filter.nPriority != -1);
			break;

		case TDCA_FLAG:
			bNeedRefilter = (m_filter.nFilter == FT_FLAGGED);
			break;

		case TDCA_RISK:
			bNeedRefilter = (m_filter.nRisk != -1);
			break;

		case TDCA_ALLOCBY:
			bNeedRefilter = (m_filter.aAllocBy.GetSize() != 0);
			break;

		case TDCA_STATUS:
			bNeedRefilter = (m_filter.aStatus.GetSize() != 0);
			break;

		case TDCA_VERSION:
			bNeedRefilter = (m_filter.aVersions.GetSize() != 0);
			break;

		case TDCA_CATEGORY:
			bNeedRefilter = (m_filter.aCategories.GetSize() != 0);
			break;

		case TDCA_ALLOCTO:
			bNeedRefilter = (m_filter.aAllocTo.GetSize() != 0);
			break;

		case TDCA_PERCENT:
			bNeedRefilter = (m_filter.nFilter == FT_DONE ||
				m_filter.nFilter == FT_NOTDONE);
			break;

		case TDCA_DONEDATE:
			bNeedRefilter = (m_filter.nFilter == FT_DONE ||
				m_filter.nFilter == FT_NOTDONE ||
				m_filter.nFilter == FT_STARTTODAY ||
				m_filter.nFilter == FT_STARTENDTHISWEEK ||
				m_filter.HasFlag(FT_HIDEDONE) ||
				m_filter.nPriority > 0);
			break;

		case TDCA_DUEDATE:
			bNeedRefilter = m_filter.HasFlag(FT_HIDEOVERDUE) ||
				(m_filter.nPriority > 0) ||
				(m_filter.nFilter != FT_DONE &&
				m_filter.nFilter != FT_NOTDONE &&
				m_filter.nFilter != FT_ALL &&
				m_filter.nFilter != FT_STARTTODAY &&
				m_filter.nFilter != FT_STARTENDTHISWEEK);
			break;

		case TDCA_STARTDATE:
			bNeedRefilter = (m_filter.nFilter == FT_STARTTODAY ||
				m_filter.nFilter == FT_STARTENDTHISWEEK);
			break;
		}
	}

	// handle attributes common to both filter types
	if (!bNeedRefilter)
	{
		switch (nModType)
		{
		case TDCA_NEWTASK:
			// if we refilter in the middle of adding a task it messes
			// up the tree items so we handle it in NewTask
			break;

		case TDCA_DELETE:
			// this is handled in SetModified
			break;

		case TDCA_COPY:
		case TDCA_UNDO:
			bNeedRefilter = TRUE;
			break;

		case TDCA_MOVE:
			bNeedRefilter = (nView == FTCV_TASKLIST && !IsSortable());
			break;
		}
	}

	return bNeedRefilter;
}

BOOL CFilteredToDoCtrl::SelectTask(DWORD dwTaskID)
{
	BOOL bRes = CToDoCtrl::SelectTask(dwTaskID);

	// check task has not been filtered out
	if (InListView())
	{
		int nItem = FindTask(dwTaskID);

		if (nItem == -1)
		{
			ASSERT(0);
			return FALSE;
		}

		// remove focused state from existing task
		int nFocus = m_list.GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);

		if (nFocus != -1)
		{
			m_list.SetItemState(nFocus, 0, LVIS_SELECTED | LVIS_FOCUSED);
		}

		m_list.SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_list.EnsureVisible(nItem, FALSE);
	}

	return bRes;
}

LRESULT CFilteredToDoCtrl::OnEditCancel(WPARAM wParam, LPARAM lParam)
{
	// check if we need to delete the just added item
	if (InListView() && GetSelectedTaskID() == m_dwLastAddedID)
	{
		int nDelItem = GetFirstSelectedItem();
		m_list.DeleteItem(nDelItem);
	}

	LRESULT lr = CToDoCtrl::OnEditCancel(wParam, lParam);

	return lr;
}

int CFilteredToDoCtrl::FindTask(DWORD dwTaskID) const
{
	if (!dwTaskID)
	{
		return -1;
	}

	LVFINDINFO lvfi;
	ZeroMemory(&lvfi, sizeof(lvfi));

	lvfi.flags = LVFI_PARAM;
	lvfi.lParam = dwTaskID;
	lvfi.vkDirection = VK_DOWN;

	return m_list.FindItem(&lvfi);
}

DWORD CFilteredToDoCtrl::GetFocusedListTaskID() const
{
	int nItem = m_list.GetNextItem(-1, LVNI_FOCUSED | LVNI_SELECTED);

	if (nItem != -1)
	{
		return m_list.GetItemData(nItem);
	}

	// else
	return 0;
}

void CFilteredToDoCtrl::SetSelectedListTasks(const CDWordArray& aTaskIDs, DWORD dwFocusedTaskID)
{
	int nSel = -1;

	for (int nTask = 0; nTask < aTaskIDs.GetSize(); nTask++)
	{
		DWORD dwTaskID = aTaskIDs[nTask];
		int nItem = FindTask(dwTaskID);

		if (nItem != -1)
		{
			if (dwTaskID == dwFocusedTaskID)
			{
				m_list.SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			}
			else
			{
				m_list.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
			}

			if (nSel == -1)
			{
				nSel = nItem;
			}
		}
		else // deselect in tree
		{
			HTREEITEM hti = m_find.GetItem(dwTaskID);
			Selection().SetItem(hti, TSHS_DESELECT, FALSE);
		}
	}

	// focused item
	if (dwFocusedTaskID)
	{
		int nItem = FindTask(dwFocusedTaskID);

		if (nItem != -1)
		{
			m_list.SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			nSel = nItem;
		}
	}

	// make sure selection is visible
	if (nSel != -1)
	{
		m_list.EnsureVisible(nSel, FALSE);
	}
}

LRESULT CFilteredToDoCtrl::OnGutterWidthChange(WPARAM wParam, LPARAM lParam)
{
	CToDoCtrl::OnGutterWidthChange(wParam, lParam);

	// update column widths if in list view
	if (InListView())
	{
		UpdateColumnWidths();
	}

	return 0;
}

void CFilteredToDoCtrl::OnListSelChanged(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	*pResult = 0;
}

void CFilteredToDoCtrl::ClearListSelection()
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();

	while (pos)
	{
		int nItem = m_list.GetNextSelectedItem(pos);
		m_list.SetItemState(nItem, 0, LVIS_SELECTED);
	}
}

int CFilteredToDoCtrl::GetFirstSelectedItem() const
{
	return m_list.GetNextItem(-1, LVNI_SELECTED);
}

int CFilteredToDoCtrl::GetSelectedListTaskIDs(CDWordArray& aTaskIDs, DWORD& dwFocusedTaskID) const
{
	aTaskIDs.RemoveAll();
	aTaskIDs.SetSize(m_list.GetSelectedCount());

	int nCount = 0;
	POSITION pos = m_list.GetFirstSelectedItemPosition();

	while (pos)
	{
		int nItem = m_list.GetNextSelectedItem(pos);

		aTaskIDs[nCount] = m_list.GetItemData(nItem);
		nCount++;
	}

	dwFocusedTaskID = GetFocusedListTaskID();

	return aTaskIDs.GetSize();
}

BOOL CFilteredToDoCtrl::SetTreeFont(HFONT hFont)
{
	CToDoCtrl::SetTreeFont(hFont);

	if (m_list.GetSafeHwnd())
	{
		if (!hFont) // set to our font
		{
			// for some reason i can not yet explain, our font
			// is not correctly set so we use our parent's font instead
			// hFont = (HFONT)SendMessage(WM_GETFONT);
			hFont = (HFONT)GetParent()->SendMessage(WM_GETFONT);
		}

		HFONT hListFont = (HFONT)m_list.SendMessage(WM_GETFONT);
		BOOL bChange = (hFont != hListFont || !GraphicsMisc::SameFontNameSize(hFont, hListFont));

		if (bChange)
		{
			m_list.SendMessage(WM_SETFONT, (WPARAM)hFont, TRUE);
			RemeasureList();
		}
	}

	return TRUE;
}

void CFilteredToDoCtrl::OnListClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMIA = (LPNMITEMACTIVATE)pNMHDR;

	UpdateTreeSelection();

	if (pNMIA->iItem != -1) // validate item
	{
		int nHit = pNMIA->iItem;
		TDC_COLUMN nCol = GetColumnID(pNMIA->iSubItem);
		DWORD dwClickID = m_list.GetItemData(nHit);
		HTREEITEM htiHit = m_find.GetItem(dwClickID);

		BOOL bAlt = Misc::KeyIsPressed(VK_MENU);

		if (bAlt)
		{
			return;
		}

		// column specific handling
		BOOL bCtrl = Misc::KeyIsPressed(VK_CONTROL);

		switch (nCol)
		{
		case TDCC_CLIENT:
			if (dwClickID == m_dw2ndClickItem)
			{
				m_list.PostMessage(LVM_EDITLABEL);
			}
			else
			{
				CRect rCheck;

				if (GetItemTitleRect(nHit, TDCTR_CHECKBOX, rCheck))
				{
					CPoint ptHit(::GetMessagePos());
					m_list.ScreenToClient(&ptHit);

					if (rCheck.PtInRect(ptHit))
					{
						BOOL bDone = m_data.IsTaskDone(dwClickID);
						SetSelectedTaskDone(!bDone);
					}
				}
			}
			break;

		case TDCC_FILEREF:
			if (bCtrl)
			{
				CString sFile = m_data.GetTaskFileRef(dwClickID);

				if (!sFile.IsEmpty())
				{
					GotoFile(sFile, TRUE);
				}
			}
			break;

		case TDCC_DEPENDENCY:
			if (bCtrl)
			{
				CStringArray aDepends;
				m_data.GetTaskDependencies(dwClickID, aDepends);

				if (aDepends.GetSize())
				{
					ShowTaskLink(0, aDepends[0]);
				}
			}
			break;

		case TDCC_TRACKTIME:
			if (!IsReadOnly())
			{
				if (GetSelectedCount() == 1 && IsItemSelected(nHit) && m_data.IsTaskTimeTrackable(dwClickID))
				{
					int nPrev = FindTask(m_dwTimeTrackTaskID);

					if (nPrev == -1)
					{
						m_list.RedrawItems(nPrev, nPrev);
						m_list.UpdateWindow();
					}

					TimeTrackTask(htiHit);
					m_list.RedrawItems(nHit, nHit);
					m_list.UpdateWindow();

					// resort if required
					if (!m_bListMultiSort && m_sortList.nBy1 == TDC_SORTBYTIMETRACKING)
					{
						Sort(TDC_SORTBYTIMETRACKING, FALSE);
					}
				}
			}
			break;

		case TDCC_DONE:
			if (!IsReadOnly())
			{
				SetSelectedTaskDone(!m_data.IsTaskDone(dwClickID));
			}
			break;

		case TDCC_FLAG:
			if (!IsReadOnly())
			{
				BOOL bFlagged = m_data.IsTaskFlagged(dwClickID);
				SetSelectedTaskFlag(!bFlagged);
			}
			break;
		}
	}

	m_dw2ndClickItem = 0;

	*pResult = 0;
}

void CFilteredToDoCtrl::OnListDblClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMIA = (LPNMITEMACTIVATE)pNMHDR;

	if (pNMIA->iItem != -1) // validate item
	{
		DWORD dwTaskID = GetTaskID(pNMIA->iItem);
		TDC_COLUMN nCol = GetColumnID(pNMIA->iSubItem);

		switch (nCol)
		{
		case TDCC_CLIENT:
			{
				// did they double click the label?
				CPoint ptHit(::GetMessagePos());
				m_list.ScreenToClient(&ptHit);

				CRect rLabel;
				CClientDC dc(&m_list);
				GetItemTitleRect(pNMIA->iItem, TDCTR_LABEL, rLabel, &dc, GetSelectedTaskTitle());

				if (rLabel.PtInRect(ptHit))
				{
					m_list.PostMessage(LVM_EDITLABEL);
				}
			}

		case TDCC_FILEREF:
			{
				CString sFile = m_data.GetTaskFileRef(dwTaskID);

				if (!sFile.IsEmpty())
				{
					GotoFile(sFile, TRUE);
				}
			}
			break;

		case TDCC_DEPENDENCY:
			{
				CStringArray aDepends;
				m_data.GetTaskDependencies(dwTaskID, aDepends);

				if (aDepends.GetSize())
				{
					ShowTaskLink(0, aDepends[0]);
				}
			}
			break;

		case TDCC_RECURRENCE:
			m_eRecurrence.DoEdit();
			break;

		case TDCC_ICON:
			EditSelectedTaskIcon();
			break;

		case TDCC_REMINDER:
			AfxGetMainWnd()->SendMessage(WM_TDCN_DOUBLECLKREMINDERCOL);
			break;
		}
	}

	*pResult = 0;
}

void CFilteredToDoCtrl::OnListKeyDown(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	// for reasons I have not yet divined, we are not receiving this message
	// as expected. So I've added an ASSERT(0) should it ever come to life
	// and have handled the key down message in ScWindowProc
	ASSERT(0);

	*pResult = 0;
}

BOOL CFilteredToDoCtrl::PtInHeader(CPoint ptScreen) const
{
	if (InListView())
	{
		CRect rHeader;
		m_header.GetWindowRect(rHeader);

		return rHeader.PtInRect(ptScreen);
	}

	// else
	return CToDoCtrl::PtInHeader(ptScreen);
}

BOOL CFilteredToDoCtrl::IsMultiSorting() const
{
	if (InListView())
	{
		return m_bListMultiSort;
	}

	// else
	return CToDoCtrl::IsMultiSorting();
}

void CFilteredToDoCtrl::MultiSort(const TDSORTCOLUMNS& sort)
{
	if (!InListView())
	{
		CToDoCtrl::MultiSort(sort);
		return;
	}

	ASSERT(sort.nBy1 != TDC_UNSORTED);

	if (sort.nBy1 == TDC_UNSORTED)
	{
		return;
	}

	// do the sort using whatever we can out of CToDoCtrlData
	TDSORTPARAMS ss;
	ss.sort = sort;
	ss.pData = &m_data;
	ss.bSortChildren = FALSE;
	ss.dwTimeTrackID = 0;

	m_list.SortItems(CToDoCtrl::SortFuncMulti, (DWORD)&ss);

	m_bListModSinceLastSort = FALSE;
	m_bListMultiSort = TRUE;
	m_sortList = sort;

	// update registry
	SaveSortState(CPreferences());

	UpdateColumnWidths();
}

void CFilteredToDoCtrl::Sort(TDC_SORTBY nBy, BOOL bAllowToggle)
{
	if (!InListView())
	{
		CToDoCtrl::Sort(nBy, bAllowToggle);
		return;
	}

	// we rebuild the entire listview if 'unsorting'
	if (nBy == TDC_UNSORTED)
	{
		TDC_SORTBY nPrevBy = m_sortList.nBy1;
		m_sortList.nBy1 = nBy;
		m_bListMultiSort = FALSE;

		if (nPrevBy != TDC_UNSORTED)
		{
			RefreshListFilter();
			UpdateColumnWidths();
		}

		return;
	}

	// else all the rest
	TDCCOLUMN* pTDCC = CToDoCtrl::GetColumn(nBy);
	ASSERT(pTDCC);

	if (!pTDCC)
	{
		return;
	}

	if (m_sortList.bAscending1 == -1 || nBy != m_sortList.nBy1)
	{
		m_sortList.bAscending1 = pTDCC->bSortAscending;
	}

	// if there's been a mod since last sorting then its reasonable to assume
	// that the user is not toggling direction but wants to simply resort
	// in the same direction.
	else if (bAllowToggle && !m_bListModSinceLastSort)
	{
		m_sortList.bAscending1 = !m_sortList.bAscending1;
	}

	// do the sort using whatever we can out of CToDoCtrlData
	TDSORTPARAMS ss;
	ss.sort.nBy1 = nBy;
	ss.sort.nBy2 = TDC_UNSORTED;
	ss.sort.nBy3 = TDC_UNSORTED;
	ss.sort.bAscending1 = m_sortList.bAscending1;
	ss.sort.bAscending2 = TRUE;
	ss.sort.bAscending3 = TRUE;
	ss.pData = &m_data;
	ss.bSortChildren = FALSE;
	ss.dwTimeTrackID = m_dwTimeTrackTaskID;

	m_list.SortItems(CToDoCtrl::SortFunc, (DWORD)&ss);

	m_sortList.nBy1 = nBy;
	m_sortList.nBy2 = TDC_UNSORTED;
	m_sortList.nBy3 = TDC_UNSORTED;
	m_bListModSinceLastSort = FALSE;
	m_bListMultiSort = FALSE;

	// update registry
	SaveSortState(CPreferences());

	UpdateColumnWidths();
}

BOOL CFilteredToDoCtrl::MoveSelectedTask(TDC_MOVETASK nDirection)
{
	return InListView() ? FALSE : CToDoCtrl::MoveSelectedTask(nDirection);
}

BOOL CFilteredToDoCtrl::CanMoveSelectedTask(TDC_MOVETASK nDirection) const
{
	return InListView() ? FALSE : CToDoCtrl::CanMoveSelectedTask(nDirection);
}

BOOL CFilteredToDoCtrl::GotoNextTask(TDC_GOTO nDirection)
{
	if (InListView() && CanGotoNextTask(nDirection))
	{
		int nSel = GetFirstSelectedItem();

		if (nDirection == TDCG_NEXT)
		{
			nSel++;
		}
		else
		{
			nSel--;
		}

		return SelectTask(m_list.GetItemData(nSel));
	}

	// else
	return CToDoCtrl::GotoNextTask(nDirection);
}

CRect CFilteredToDoCtrl::GetSelectedItemsRect() const
{
	CRect rInvalid(0, 0, 0, 0), rItem;
	POSITION pos = m_list.GetFirstSelectedItemPosition();

	while (pos)
	{
		int nItem = m_list.GetNextSelectedItem(pos);

		if (m_list.GetItemRect(nItem, rItem, LVIR_BOUNDS))
		{
			rInvalid |= rItem;
		}
	}

	return rInvalid;
}

BOOL CFilteredToDoCtrl::CanGotoNextTask(TDC_GOTO nDirection) const
{
	if (InListView())
	{
		int nSel = GetFirstSelectedItem();

		if (nDirection == TDCG_NEXT)
		{
			return (nSel >= 0 && nSel < m_list.GetItemCount() - 1);
		}

		// else prev
		return (nSel > 0 && nSel <= m_list.GetItemCount() - 1);
	}
	else
	{
		return CToDoCtrl::CanGotoNextTask(nDirection);
	}
}

BOOL CFilteredToDoCtrl::GotoNextTopLevelTask(TDC_GOTO nDirection)
{
	if (InListView())
	{
		return FALSE;   // not supported
	}

	// else
	return CToDoCtrl::GotoNextTopLevelTask(nDirection);
}

BOOL CFilteredToDoCtrl::CanGotoNextTopLevelTask(TDC_GOTO nDirection) const
{
	return InListView() ? FALSE : CToDoCtrl::CanGotoNextTopLevelTask(nDirection);
}

BOOL CFilteredToDoCtrl::CanExpandSelectedTask(BOOL bExpand) const
{
	return InListView() ? FALSE : CToDoCtrl::CanExpandSelectedTask(bExpand);
}

void CFilteredToDoCtrl::ExpandSelectedTask(BOOL bExpand)
{
	if (InListView())
	{
		return;   // not supported
	}

	CToDoCtrl::ExpandSelectedTask(bExpand);
}

void CFilteredToDoCtrl::ExpandAllTasks(BOOL bExpand)
{
	if (InListView())
	{
		return;   // not supported
	}

	CToDoCtrl::ExpandAllTasks(bExpand);
}

void CFilteredToDoCtrl::SetFocusToTasks()
{
	if (InListView())
	{
		m_list.SetFocus();
	}
	else
	{
		CToDoCtrl::SetFocusToTasks();
	}
}

BOOL CFilteredToDoCtrl::TasksHaveFocus() const
{
	if (InListView())
	{
		return (::GetFocus() == m_list);
	}

	return CToDoCtrl::TasksHaveFocus();
}

int CFilteredToDoCtrl::FindTasks(const SEARCHPARAMS& params, CResultArray& aResults) const
{
	if (!InListView())
	{
		return CToDoCtrl::FindTasks(params, aResults);
	}

	// iterate the tasks
	for (int nItem = 0; nItem < m_list.GetItemCount(); nItem++)
	{
		DWORD dwTaskID = GetTaskID(nItem);
		SEARCHRESULT result;

		if (m_data.TaskMatches(dwTaskID, params, result))
		{
			aResults.Add(result);
		}
	}

	return aResults.GetSize();
}

BOOL CFilteredToDoCtrl::SelectTask(CString sPart, TDC_SELECTTASK nSelect)
{
	if (!InListView())
	{
		return CToDoCtrl::SelectTask(sPart, nSelect);
	}

	// else
	int nFind = -1;

	switch (nSelect)
	{
	case TDC_SELECTFIRST:
		nFind = FindTask(sPart);
		break;

	case TDC_SELECTNEXT:
		nFind = FindTask(sPart, GetFirstSelectedItem() + 1);
		break;

	case TDC_SELECTNEXTINCLCURRENT:
		nFind = FindTask(sPart, GetFirstSelectedItem());
		break;

	case TDC_SELECTPREV:
		nFind = FindTask(sPart, GetFirstSelectedItem() - 1, FALSE);
		break;

	case TDC_SELECTLAST:
		nFind = FindTask(sPart, m_list.GetItemCount() - 1, FALSE);
		break;
	}

	if (nFind != -1)
	{
		SelectTask(GetTaskID(nFind));
	}

	return (nFind != -1);
}

int CFilteredToDoCtrl::FindTask(LPCTSTR szPart, int nStart, BOOL bNext)
{
	// build a search query
	SEARCHPARAMS params;
	params.rules.Add(SEARCHPARAM(TDCA_TASKNAMEORCOMMENTS, FO_INCLUDES, szPart));

	// we need to do this manually because CListCtrl::FindItem
	// only looks at the start of the string
	SEARCHRESULT result;

	int nFrom = nStart;
	int nTo = bNext ? m_list.GetItemCount() : -1;
	int nInc = bNext ? 1 : -1;

	for (int nItem = nFrom; nItem != nTo; nItem += nInc)
	{
		DWORD dwTaskID = GetTaskID(nItem);

		if (m_data.TaskMatches(dwTaskID, params, result))
		{
			return nItem;
		}
	}

	return -1; // no match
}

void CFilteredToDoCtrl::SelectNextTasksInHistory()
{
	if (InListView() && CanSelectNextTasksInHistory())
	{
		// let CToDoCtrl do it's thing
		CToDoCtrl::SelectNextTasksInHistory();

		// then update our own selection
		ResyncListSelection();
	}
	else
	{
		CToDoCtrl::SelectNextTasksInHistory();
	}
}

BOOL CFilteredToDoCtrl::MultiSelectItems(const CDWordArray& aTasks, TSH_SELECT nState, BOOL bRedraw)
{
	BOOL bRes = CToDoCtrl::MultiSelectItems(aTasks, nState, bRedraw);

	if (InListView())
	{
		ResyncListSelection();
	}

	return bRes;
}

void CFilteredToDoCtrl::ResyncListSelection()
{
	ClearListSelection();

	// then update our own selection
	CDWordArray aTreeTaskIDs;
	DWORD dwFocusedID;

	GetSelectedTaskIDs(aTreeTaskIDs, dwFocusedID, FALSE);
	SetSelectedListTasks(aTreeTaskIDs, dwFocusedID);

	// now check that the tree is synced with us!
	CDWordArray aListTaskIDs;
	GetSelectedListTaskIDs(aListTaskIDs, dwFocusedID);

	if (!Misc::ArraysMatch(aListTaskIDs, aTreeTaskIDs))
	{
		CToDoCtrl::MultiSelectItems(aListTaskIDs, TSHS_SELECT, FALSE);
	}
}

void CFilteredToDoCtrl::SelectPrevTasksInHistory()
{
	if (InListView() && CanSelectPrevTasksInHistory())
	{
		// let CToDoCtrl do it's thing
		CToDoCtrl::SelectPrevTasksInHistory();

		// then update our own selection
		ResyncListSelection();
	}
	else
	{
		CToDoCtrl::SelectPrevTasksInHistory();
	}
}

void CFilteredToDoCtrl::InvalidateItem(HTREEITEM hti)
{
	if (InListView())
	{
		int nItem = GetListItem(hti);
		CRect rItem;

		if (GetItemTitleRect(nItem, TDCTR_BOUNDS, rItem))
		{
			m_list.InvalidateRect(rItem, FALSE);
		}
	}
	else
	{
		CToDoCtrl::InvalidateItem(hti);
	}
}

LRESULT CFilteredToDoCtrl::ScWindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_RBUTTONDOWN:
		{
			// work out what got hit and make sure it's selected
			CPoint ptHit(lp);
			LVHITTESTINFO lvhti = { { ptHit.x, ptHit.y }, 0, 0, 0 };

			int nHit = m_list.HitTest(&lvhti);

			if (!IsItemSelected(nHit))
			{
				SelectTask(GetTaskID(nHit));
			}
		}
		break;

	case WM_LBUTTONDOWN:
		{
			// work out what got hit
			CPoint ptHit(lp);
			LVHITTESTINFO lvhti = { { ptHit.x, ptHit.y }, 0, 0, 0 };

			m_list.SubItemHitTest(&lvhti);
			int nHit = lvhti.iItem;

			if (nHit != -1 && !IsReadOnly())
			{
				TDC_COLUMN nCol = GetColumnID(lvhti.iSubItem);
				DWORD dwTaskID = GetTaskID(nHit);

				// if the user is clicking on an already multi-selected
				// item since we may need to carry out an operation on multiple items
				int nSelCount = GetSelectedCount();

				if (nSelCount > 1 && IsItemSelected(nHit))
				{
					switch (nCol)
					{
					case TDCC_DONE:
						{
							BOOL bDone = m_data.IsTaskDone(dwTaskID);
							SetSelectedTaskDone(!bDone);
							return 0; // eat it
						}
						break;

					case TDCC_CLIENT:
						{
							CRect rCheck;

							if (GetItemTitleRect(nHit, TDCTR_CHECKBOX, rCheck) && rCheck.PtInRect(ptHit))
							{
								BOOL bDone = m_data.IsTaskDone(dwTaskID);
								SetSelectedTaskDone(!bDone);
								return 0; // eat it
							}
						}
						break;

					case TDCC_FLAG:
						{
							BOOL bFlagged = m_data.IsTaskFlagged(dwTaskID);
							SetSelectedTaskFlag(!bFlagged);
							return 0; // eat it
						}
						break;
					}
				}
				else if (nSelCount == 1)
				{
					// if the click was on the task title of an already singly selected item
					// we record the task ID unless the control key is down in which case
					// it really means that the user has deselected the item
					if (!Misc::ModKeysArePressed(MKS_CTRL))
					{
						m_dw2ndClickItem = 0;

						int nSel = GetFirstSelectedItem();
						if (nHit != -1 && nHit == nSel && nCol == TDCC_CLIENT)
						{
							// unfortunately we cannot rely on the flags attribute of LVHITTESTINFO
							// to see if the user clicked on the text because LVIR_LABEL == LVIR_BOUNDS
							CRect rLabel;
							CClientDC dc(&m_list);
							CFont* pOld = NULL;

							if (m_tree.GetParentItem(GetTreeItem(nHit)) == NULL) // top level items
							{
								pOld = (CFont*)dc.SelectObject(CFont::FromHandle(m_fontBold));
							}
							else
							{
								pOld = (CFont*)dc.SelectObject(m_list.GetFont());
							}

							GetItemTitleRect(nHit, TDCTR_LABEL, rLabel, &dc, GetSelectedTaskTitle());

							if (rLabel.PtInRect(ptHit))
							{
								m_dw2ndClickItem = m_list.GetItemData(nHit);
							}

							// cleanup
							dc.SelectObject(pOld);
						}

						// note: we handle WM_LBUTTONUP in OnListClick() to
						// decide whether to do a label edit
					}
				}
			}

			// because the visual state of the list selection is actually handled by
			// whether the tree selection is up to date we need to update the tree
			// selection here, because the list ctrl does it this way natively.
			LRESULT	lr = CSubclasser::ScWindowProc(hRealWnd, msg, wp, lp);
			UpdateTreeSelection();

			return lr;
		}
		break;

	case LVM_EDITLABEL:
		if (!IsReadOnly())
		{
			EditSelectedTask(FALSE);
		}
		return 0; // eat it

	case WM_KEYDOWN:
		{
			// if any of the cursor keys are used and nothing is currently selected
			// then we select the top/bottom item and ignore the default list ctrl processing
			LRESULT lr = 0;

			lr = CSubclasser::ScWindowProc(hRealWnd, msg, wp, lp);

			m_wKeyPress = (WORD)wp;

			if (Misc::IsCursorKey(wp))
			{
				UpdateTreeSelection();
			}

			return lr;
		}
		break;

	case WM_KEYUP:
		if (Misc::IsCursorKey(wp))
		{
			UpdateControls();
		}
		break;

	case WM_MOUSEWHEEL:
	case WM_VSCROLL:
	case WM_HSCROLL:
		if (InListView() && IsTaskLabelEditing())
		{
			EndLabelEdit(FALSE);
		}

		// extra processing for WM_HSCROLL
		if (msg == WM_HSCROLL && InListView())
		{
			m_header.Invalidate(FALSE);
		}

		break;

	case WM_NOTIFY:
		{
			LPNMHDR pNMHDR = (LPNMHDR)lp;

			switch (pNMHDR->code)
			{
			case NM_CUSTOMDRAW:
				// We must forward on
				if (pNMHDR->hwndFrom == m_header)
				{
					LRESULT lr = 0;
					OnHeaderCustomDraw(pNMHDR, &lr);
					return lr;
				}
				break;
			}
		}
		break;
	}

	return CSubclasser::ScWindowProc(hRealWnd, msg, wp, lp);
}

void CFilteredToDoCtrl::UpdateTreeSelection()
{
	// update the tree selection if it needs to be
	CDWordArray aListTaskIDs, aTreeTaskIDs;
	DWORD dwFocusedID;

	GetSelectedTaskIDs(aTreeTaskIDs, dwFocusedID, FALSE);
	GetSelectedListTaskIDs(aListTaskIDs, dwFocusedID);

	if (!Misc::ArraysMatch(aTreeTaskIDs, aListTaskIDs))
	{
		MultiSelectItems(aListTaskIDs, TSHS_SELECT, FALSE);

		// also set the focused item
		m_list.SetItemState(FindTask(dwFocusedID), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_list.Invalidate(FALSE);
	}
}

BOOL CFilteredToDoCtrl::IsItemSelected(int nItem) const
{
	HTREEITEM hti = GetTreeItem(nItem);
	return hti ? Selection().HasItem(hti) : FALSE;
}

HTREEITEM CFilteredToDoCtrl::GetTreeItem(int nItem) const
{
	if (nItem < 0 || nItem >= m_list.GetItemCount())
	{
		return NULL;
	}

	DWORD dwID = m_list.GetItemData(nItem);
	return m_find.GetItem(dwID);
}

int CFilteredToDoCtrl::GetListItem(HTREEITEM hti) const
{
	DWORD dwID = GetTaskID(hti);
	return (dwID ? FindTask(dwID) : -1);
}

BOOL CFilteredToDoCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (&m_list == pWnd)
	{
		CPoint pt(::GetMessagePos());
		m_list.ScreenToClient(&pt);

		LVHITTESTINFO lvhti = { { pt.x, pt.y }, 0, 0, 0 };
		m_list.SubItemHitTest(&lvhti);

		int nHit = lvhti.iItem;

		if (nHit >= 0)
		{
			TDC_COLUMN nCol	= GetColumnID(lvhti.iSubItem);
			DWORD dwID = m_list.GetItemData(nHit);

			BOOL bCtrl = Misc::ModKeysArePressed(MKS_CTRL);
			BOOL bShowHand = FALSE;

			switch (nCol)
			{
			case TDCC_FILEREF:
				if (bCtrl)
				{
					CString sFile = m_data.GetTaskFileRef(dwID);
					bShowHand = (!sFile.IsEmpty());
				}
				break;

			case TDCC_DEPENDENCY:
				if (bCtrl)
				{
					CStringArray aDepends;
					m_data.GetTaskDependencies(dwID, aDepends);
					bShowHand = aDepends.GetSize();
				}
				break;

			case TDCC_TRACKTIME:
				if (!IsReadOnly())
				{
					bShowHand = ((!IsItemSelected(nHit) || GetSelectedCount() == 1) &&
						m_data.IsTaskTimeTrackable(dwID));
				}
				break;

			case TDCC_FLAG:
				if (!IsReadOnly())
				{
					bShowHand = TRUE;
				}
			}

			if (bShowHand)
			{
				::SetCursor(GraphicsMisc::HandCursor());
				return TRUE;
			}
		}
	}

	return CToDoCtrl::OnSetCursor(pWnd, nHitTest, message);
}

LRESULT CFilteredToDoCtrl::OnDropFileRef(WPARAM wParam, LPARAM lParam)
{
	int nItem = wParam;

	if (!InListView() || nItem == -1)
	{
		return CToDoCtrl::OnDropFileRef(wParam, lParam);
	}

	// else
	if (!IsReadOnly())
	{
		SelectTask(m_list.GetItemData(nItem));
		SetSelectedTaskFileRef((LPCTSTR)lParam);
	}

	return 0;
}

BOOL CFilteredToDoCtrl::GetItemTitleRect(HTREEITEM hti, TDC_TITLERECT nArea, CRect& rect) const
{
	if (InListView())
	{
		int nItem = GetListItem(hti);
		return GetItemTitleRect(nItem, nArea, rect);
	}

	// else
	return CToDoCtrl::GetItemTitleRect(hti, nArea, rect);
}

BOOL CFilteredToDoCtrl::GetItemTitleRect(int nItem, TDC_TITLERECT nArea, CRect& rect, CDC* pDC, LPCTSTR szTitle) const
{
	ASSERT(InListView());

	if (nItem == -1)
	{
		return FALSE;
	}

	// basic title rect
	int nColIndex = GetColumnIndex(TDCC_CLIENT);
	const_cast<CListCtrl*>(&m_list)->GetSubItemRect(nItem, nColIndex, LVIR_LABEL, rect);

	if (nColIndex == 0 && COSVersion() >= OSV_XP) // right side columns in XP
	{
		rect.left -= 4;
	}

	BOOL bIcon = HasStyle(TDCS_TREETASKICONS) && !IsColumnShowing(TDCC_ICON);
	BOOL bCheckbox = HasStyle(TDCS_TREECHECKBOXES) && !IsColumnShowing(TDCC_DONE);

	switch (nArea)
	{
	case TDCTR_CHECKBOX:
		if (bCheckbox)
		{
			rect.right = rect.left + 16;
			return TRUE;
		}
		break;

	case TDCTR_ICON:
		if (bIcon)
		{
			if (bCheckbox)
			{
				rect.left += 18;
			}

			rect.right = rect.left + 16;
			return TRUE;
		}
		break;

	case TDCTR_LABEL:
		{
			if (bIcon)
			{
				rect.left += 18;
			}

			if (bCheckbox)
			{
				rect.left += 18;
			}

			if (pDC && szTitle)
			{
				int nTextExt = pDC->GetTextExtent(szTitle).cx;
				rect.right = rect.left + min(rect.Width(), nTextExt + 6 + 1);
			}

			return TRUE;
		}
		break;

	case TDCTR_BOUNDS:
		return TRUE; // nothing more to do

	case TDCTR_EDIT:
		if (GetItemTitleRect(nItem, TDCTR_LABEL, rect)) // recursive call
		{
			CRect rClient, rInter;
			m_list.GetClientRect(rClient);

			if (rInter.IntersectRect(rect, rClient))
			{
				rect = rInter;
			}

			rect.top--;

			m_list.ClientToScreen(rect);
			ScreenToClient(rect);

			return TRUE;
		}
		break;
	}

	return FALSE;
}

void CFilteredToDoCtrl::OnListGetInfoTip(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVGETINFOTIP* pLVGIT = (NMLVGETINFOTIP*)pNMHDR;
	*pResult = 0;

	int nHit = pLVGIT->iItem;

	if (nHit >= 0)
	{
		HTREEITEM hti = GetTreeItem(nHit);
		TODOITEM* pTDI = GetTask(m_list.GetItemData(nHit));

		ASSERT(pTDI);

		if (!pTDI)
		{
			return;
		}

		// we only display info tips when over the task title
		CRect rTitle;
		CClientDC dc(&m_list);
		CFont* pOld = NULL;

		if (m_tree.GetParentItem(hti) == NULL) // top level item
		{
			pOld = (CFont*)dc.SelectObject(CFont::FromHandle(m_fontBold));
		}
		else
		{
			pOld = (CFont*)dc.SelectObject(m_list.GetFont());
		}

		GetItemTitleRect(nHit, TDCTR_LABEL, rTitle, &dc, pTDI->sTitle);

		// cleanup
		dc.SelectObject(pOld);

		CPoint pt(::GetMessagePos());
		m_list.ScreenToClient(&pt);

		if (rTitle.PtInRect(pt))
		{
			//fabio_2005
#if _MSC_VER >= 1400
			_tcsncpy_s(pLVGIT->pszText, pLVGIT->cchTextMax, FormatInfoTip(hti, pTDI), pLVGIT->cchTextMax);
#else
			_tcsncpy(pLVGIT->pszText, FormatInfoTip(hti, pTDI), pLVGIT->cchTextMax);
#endif
		}
	}
}

void CFilteredToDoCtrl::UpdateSelectedTaskPath()
{
	CToDoCtrl::UpdateSelectedTaskPath();

	// redraw the client column header
	if (m_header.GetSafeHwnd())
	{
		CRect rClient;

		if (m_header.GetItemRect(GetColumnIndex(TDCC_CLIENT), rClient))
		{
			m_header.InvalidateRect(rClient, FALSE);
		}
	}
}

void CFilteredToDoCtrl::SaveSortState(CPreferences& prefs)
{
	// ignore this if we have no tasks
	if (GetTaskCount() == 0)
	{
		return;
	}

	// create a new key using the filepath
	ASSERT(GetSafeHwnd());

	CString sKey = GetPreferencesKey(_T("SortState"));

	if (!sKey.IsEmpty())
	{
		prefs.WriteProfileInt(sKey, _T("ListMulti"), m_bListMultiSort);
		prefs.WriteProfileInt(sKey, _T("ListColumn"), m_sortList.nBy1);
		prefs.WriteProfileInt(sKey, _T("ListColumn2"), m_sortList.nBy2);
		prefs.WriteProfileInt(sKey, _T("ListColumn3"), m_sortList.nBy3);
		prefs.WriteProfileInt(sKey, _T("ListAscending"), m_sortList.bAscending1);
		prefs.WriteProfileInt(sKey, _T("ListAscending2"), m_sortList.bAscending2);
		prefs.WriteProfileInt(sKey, _T("ListAscending3"), m_sortList.bAscending3);
	}

	// base class
	CToDoCtrl::SaveSortState(prefs);
}

void CFilteredToDoCtrl::LoadSortState(const CPreferences& prefs, LPCTSTR szFilePath)
{
	CString sKey = GetPreferencesKey(_T("SortState"), szFilePath);

	if (!sKey.IsEmpty())
	{
		// is this the first time since we restored tree sorting (disabled in 5.6)
		int nListSortBy = prefs.GetProfileInt(sKey, _T("ListColumn"), -99);

		if (nListSortBy == -99) // yes
		{
			// so we use whatever the tree has set
			m_sortList.nBy1 = (TDC_SORTBY)prefs.GetProfileInt(sKey, _T("Column"), TDC_UNSORTED);
			m_sortList.bAscending1 = prefs.GetProfileInt(sKey, _T("Ascending"), TRUE);

			// and clear the tree's state
			m_sort.nBy1 = TDC_UNSORTED;
			m_sort.bAscending1 = -1;
		}
		else // each has separate settings
		{
			m_bListMultiSort = prefs.GetProfileInt(sKey, _T("ListMulti"), FALSE);
			m_sortList.nBy1 = (TDC_SORTBY)prefs.GetProfileInt(sKey, _T("ListColumn"), TDC_UNSORTED);
			m_sortList.nBy2 = (TDC_SORTBY)prefs.GetProfileInt(sKey, _T("ListColumn2"), TDC_UNSORTED);
			m_sortList.nBy3 = (TDC_SORTBY)prefs.GetProfileInt(sKey, _T("ListColumn3"), TDC_UNSORTED);
			m_sortList.bAscending1 = prefs.GetProfileInt(sKey, _T("ListAscending"), TRUE);
			m_sortList.bAscending2 = prefs.GetProfileInt(sKey, _T("ListAscending2"), TRUE);
			m_sortList.bAscending3 = prefs.GetProfileInt(sKey, _T("ListAscending3"), TRUE);

			CToDoCtrl::LoadSortState(prefs, szFilePath);
		}

		m_sortList.nBy1 = max(m_sortList.nBy1, TDC_UNSORTED); // backwards compatibility
	}
}

void CFilteredToDoCtrl::RedrawReminders() const
{
	if (!InListView())
	{
		CToDoCtrl::RedrawReminders();
	}

	else if (IsColumnShowing(TDCC_REMINDER))
	{
		CListCtrl* pList = const_cast<CListCtrl*>(&m_list);
		pList->Invalidate(FALSE);
	}
}
