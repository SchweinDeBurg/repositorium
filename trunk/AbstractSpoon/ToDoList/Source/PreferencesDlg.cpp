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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// PreferencesDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "PreferencesDlg.h"

#include "../../../CodeProject/Source/WinClasses.h"
#include "../../../CodeProject/Source/WClassDefines.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/DialogHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg dialog

// default priority colors
const COLORREF PRIORITYLOWCOLOR = RGB(30, 225, 0);
const COLORREF PRIORITYHIGHCOLOR = RGB(255, 0, 0);

const char PATHDELIM = '>';

CPreferencesDlg::CPreferencesDlg(CShortcutManager* pShortcutMgr, UINT nMenuID, const CContentMgr* pContentMgr,
	const CImportExportMgr* pExportMgr, CWnd* pParent /*=NULL*/):
CPreferencesDlgBase(IDD_PREFERENCES, pParent),
m_pageShortcuts(pShortcutMgr, nMenuID, FALSE),
m_pageUI(pContentMgr),
m_pageFile2(pExportMgr)
{
	CPreferencesDlgBase::AddPage(&m_pageGen);
	CPreferencesDlgBase::AddPage(&m_pageMultiUser);
	CPreferencesDlgBase::AddPage(&m_pageFile);
	CPreferencesDlgBase::AddPage(&m_pageFile2);
	CPreferencesDlgBase::AddPage(&m_pageUI);
	CPreferencesDlgBase::AddPage(&m_pageUITasklist);
	CPreferencesDlgBase::AddPage(&m_pageUITasklistColors);
	CPreferencesDlgBase::AddPage(&m_pageTask);
	CPreferencesDlgBase::AddPage(&m_pageTaskCalc);
	CPreferencesDlgBase::AddPage(&m_pageTaskDef);
	CPreferencesDlgBase::AddPage(&m_pageExport);
	CPreferencesDlgBase::AddPage(&m_pageTool);
	CPreferencesDlgBase::AddPage(&m_pageShortcuts);

	ForwardMessage(WM_PTDP_LISTCHANGE);
	ForwardMessage(WM_PUITCP_CATEGORYCHANGE);
	ForwardMessage(WM_PTP_TESTTOOL);
	ForwardMessage(WM_PGP_CLEARMRU);
	ForwardMessage(WM_PPB_CTRLCHANGE);

	LoadPreferences();
}

CPreferencesDlg::~CPreferencesDlg()
{
}

void CPreferencesDlg::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesDlgBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesDlg)
	DDX_Control(pDX, IDC_PAGES, m_tcPages);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPreferencesDlg, CPreferencesDlgBase)
	//{{AFX_MSG_MAP(CPreferencesDlg)
	ON_BN_CLICKED(IDC_HELP2, OnHelp)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PAGES, OnSelchangedPages)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	//}}AFX_MSG_MAP
	ON_WM_HELPINFO()
	ON_REGISTERED_MESSAGE(WM_PTDP_LISTCHANGE, OnDefPageListChange)
	ON_REGISTERED_MESSAGE(WM_PUITCP_CATEGORYCHANGE, OnColourPageCategoryChange)
	ON_REGISTERED_MESSAGE(WM_PTP_TESTTOOL, OnToolPageTestTool)
	ON_REGISTERED_MESSAGE(WM_PGP_CLEARMRU, OnGenPageClearMRU)
	ON_REGISTERED_MESSAGE(WM_PPB_CTRLCHANGE, OnControlChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg message handlers

BOOL CPreferencesDlg::OnInitDialog()
{
	CPreferencesDlgBase::OnInitDialog();

	m_tcPages.SetIndent(0);

	VERIFY(CreatePPHost(IDC_HOSTFRAME));

	// add pages to tree control
	AddPage(&m_pageGen, IDS_PREF_GEN);
	AddPage(&m_pageMultiUser, IDS_PREF_MULTIUSER);
	AddPage(&m_pageMultiUser, IDS_PREF_MULTIUSERFILE);
	AddPage(&m_pageMultiUser, IDS_PREF_MULTIUSERSS);
	AddPage(&m_pageFile, IDS_PREF_FILE);
	AddPage(&m_pageFile, IDS_PREF_FILELOAD);
	AddPage(&m_pageFile, IDS_PREF_FILEARCHIVE);
	AddPage(&m_pageFile, IDS_PREF_FILENOTIFY);
	AddPage(&m_pageFile2, IDS_PREF_FILEMORE);
	AddPage(&m_pageFile2, IDS_PREF_FILEBACKUP);
	AddPage(&m_pageFile2, IDS_PREF_FILESAVE);
	AddPage(&m_pageUI, IDS_PREF_UI);
	AddPage(&m_pageUI, IDS_PREF_UIFILTERING);
	AddPage(&m_pageUI, IDS_PREF_UITOOLBAR);
	AddPage(&m_pageUI, IDS_PREF_UISORTING);
	AddPage(&m_pageUI, IDS_PREF_UITABBAR);
	AddPage(&m_pageUI, IDS_PREF_UICOMMENTS);
	AddPage(&m_pageUITasklist, IDS_PREF_UITASKCOLUMNS);
	AddPage(&m_pageUITasklist, IDS_PREF_UITASK);
	AddPage(&m_pageUITasklistColors, IDS_PREF_UITASKCOLOR);
	AddPage(&m_pageTask, IDS_PREF_TASK);
	AddPage(&m_pageTask, IDS_PREF_TASKTIME);
	AddPage(&m_pageTaskCalc, IDS_PREF_TASKCALCS);
	AddPage(&m_pageTaskDef, IDS_PREF_TASKDEF);
	AddPage(&m_pageTaskDef, IDS_PREF_TASKDEFINHERIT);
	AddPage(&m_pageExport, IDS_PREF_EXPORT);
	AddPage(&m_pageTool, IDS_PREF_TOOLS);
	AddPage(&m_pageShortcuts, IDS_PREF_SHORTCUT);

	SynchronizeTree();

	GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPreferencesDlg::OnHelpInfo(HELPINFO* /*pHelpInfo*/)
{
	OnHelp();
	return FALSE;
}

void CPreferencesDlg::SynchronizeTree()
{
	HTREEITEM hti = NULL;

	if (m_mapPP2HTI.Lookup(GetActivePage(), (void*&)hti) && hti)
	{
		m_tcPages.SelectItem(hti);
	}
}

BOOL CPreferencesDlg::PreTranslateMessage(MSG* pMsg)
{
	// special handling for hotkeys
	if (CWinClasses::IsClass(pMsg->hwnd, WC_HOTKEY))
	{
		return FALSE;
	}

	return CPreferencesDlgBase::PreTranslateMessage(pMsg);
}

void CPreferencesDlg::OnHelp()
{
	// who is the active page?
	int nSel = GetActiveIndex();
	CString sHelpPage = _T("prefs");

	switch (nSel)
	{
	case PREFPAGE_GEN:
		sHelpPage += _T("#General");
		break;

	case PREFPAGE_MULTIUSER:
		sHelpPage += _T("#MultiUserControl");
		break;

	case PREFPAGE_FILE:
		sHelpPage += _T("#FileActions");
		break;

	case PREFPAGE_FILE2:
		sHelpPage += _T("#FileActionsmore");
		break;

	case PREFPAGE_UI:
		sHelpPage += _T("#UserInterfacegtGeneral");
		break;

	case PREFPAGE_UITASK:
		sHelpPage += _T("#UserInterfacegtTasklistgtAttributes");
		break;

	case PREFPAGE_UIFONTCOLOR:
		sHelpPage += _T("#UserInterfacegtTasklistgtFontsandColours");
		break;

	case PREFPAGE_TASK:
		sHelpPage += _T("#TasksgtAttributeCalculations");
		break;

	case PREFPAGE_TASKDEF:
		sHelpPage += _T("#TasksgtDefaultTaskAttributes");
		break;

	case PREFPAGE_TOOL:
		sHelpPage += _T("#Tools");
		break;

	case PREFPAGE_SHORTCUT:
		sHelpPage += _T("#KeyboardShortcuts");
		break;

	default:
		ASSERT(0);
		break;
	}

	AfxGetApp()->WinHelp((DWORD)(LPCTSTR)sHelpPage);
}

void CPreferencesDlg::AddPage(CPreferencesPageBase* pPage, UINT nIDPath)
{
	CString sPath;
	sPath.LoadString(nIDPath);

	if (FindPage(pPage) != -1)
	{
		HTREEITEM htiParent = TVI_ROOT; // default
		int nFind = sPath.Find(PATHDELIM);

		while (nFind != -1)
		{
			CString sParent = sPath.Left(nFind);
			sPath = sPath.Mid(nFind + 1);

			// see if parent already exists
			HTREEITEM htiParentParent = htiParent;
			htiParent = m_tcPages.GetChildItem(htiParentParent);

			while (htiParent)
			{
				if (sParent.CompareNoCase(m_tcPages.GetItemText(htiParent)) == 0)
				{
					break;
				}

				htiParent = m_tcPages.GetNextItem(htiParent, TVGN_NEXT);
			}

			if (!htiParent)
			{
				htiParent = m_tcPages.InsertItem(sParent, htiParentParent);

				// embolden root items
				if (htiParentParent == TVI_ROOT)
				{
					m_tcPages.SetItemState(htiParent, TVIS_BOLD, TVIS_BOLD);
				}
			}

			nFind = sPath.Find(PATHDELIM);
		}

		HTREEITEM hti = m_tcPages.InsertItem(sPath, htiParent); // whatever's left
		m_tcPages.EnsureVisible(hti);

		// embolden root items
		if (htiParent == TVI_ROOT)
		{
			m_tcPages.SetItemState(hti, TVIS_BOLD, TVIS_BOLD);
		}

		// map both ways
		m_tcPages.SetItemData(hti, (DWORD)pPage);

		// don't remap the page if already done
		HTREEITEM htiMap = NULL;

		if (!m_mapPP2HTI.Lookup(pPage, (void*&)htiMap))
		{
			m_mapPP2HTI[(void*)pPage] = (void*)hti;
		}
	}
}

void CPreferencesDlg::OnSelchangedPages(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	HTREEITEM htiSel = m_tcPages.GetSelectedItem();

	while (m_tcPages.ItemHasChildren(htiSel))
	{
		htiSel = m_tcPages.GetChildItem(htiSel);
	}

	CPreferencesPageBase* pPage = (CPreferencesPageBase*)m_tcPages.GetItemData(htiSel);
	ASSERT(pPage);

	if (pPage && CPreferencesDlgBase::SetActivePage(pPage))
	{
		// special handling:
		// if we're activating the defaults page then update the
		// priority colors
		if (pPage == &m_pageTaskDef)
		{
			CDWordArray aColors;
			m_pageUITasklistColors.GetPriorityColors(aColors);
			m_pageTaskDef.SetPriorityColors(aColors);
		}

		// update caption
		CString sCaption;
		sCaption.Format(IDS_PREF_TITLE, GetItemPath(htiSel));
		SetWindowText(sCaption);
	}

	m_tcPages.SetFocus();

	*pResult = 0;
}

BOOL CPreferencesDlg::SetActivePage(int nPage)
{
	if (CPreferencesDlgBase::SetActivePage(nPage))
	{
		SynchronizeTree();

		return TRUE;
	}

	return FALSE;
}

BOOL CPreferencesDlg::IsPrefTab(HWND hWnd) const
{
	return (hWnd == m_pageGen ||
		hWnd == m_pageMultiUser ||
		hWnd == m_pageFile ||
		hWnd == m_pageFile2 ||
		hWnd == m_pageUI ||
		hWnd == m_pageUITasklist ||
		hWnd == m_pageUITasklistColors ||
		hWnd == m_pageTask ||
		hWnd == m_pageTaskCalc ||
		hWnd == m_pageTaskDef ||
		hWnd == m_pageExport ||
		hWnd == m_pageTool ||
		hWnd == m_pageShortcuts);
}

CString CPreferencesDlg::GetItemPath(HTREEITEM hti) const
{
	CString sPath = m_tcPages.GetItemText(hti);
	hti = m_tcPages.GetParentItem(hti);

	while (hti)
	{
		sPath = m_tcPages.GetItemText(hti) + _T(" > ") + sPath;
		hti = m_tcPages.GetParentItem(hti);
	}

	return sPath;
}

LRESULT CPreferencesDlg::OnDefPageListChange(WPARAM wp, LPARAM lp)
{
	// decode params
	int nList = LOWORD(wp);
	BOOL bAdd = HIWORD(wp);
	LPCTSTR szItem = (LPCTSTR)lp;

	switch (nList)
	{
	case PTDP_ALLOCBY:
		break;

	case PTDP_ALLOCTO:
		break;

	case PTDP_STATUS:
		break;

	case PTDP_CATEGORY:
		// pass on to category color page
		// pass on to task def page
		if (!EnsurePageCreated(&m_pageUITasklistColors))
		{
			return 0L;
		}

		if (bAdd)
		{
			m_pageUITasklistColors.AddCategory(szItem);
		}
		else
		{
			m_pageUITasklistColors.DeleteCategory(szItem);
		}
		break;
	}

	// forward on to main app
	return AfxGetMainWnd()->SendMessage(WM_PTDP_LISTCHANGE, wp, lp);
}

LRESULT CPreferencesDlg::OnColourPageCategoryChange(WPARAM wp, LPARAM lp)
{
	// decode params
	BOOL bAdd = wp;
	LPCTSTR szItem = (LPCTSTR)lp;

	// pass on to task def page
	if (!EnsurePageCreated(&m_pageTaskDef))
	{
		return 0L;
	}

	if (bAdd)
	{
		m_pageTaskDef.AddListItem(PTDP_CATEGORY, szItem);
	}
	else
	{
		m_pageTaskDef.DeleteListItem(PTDP_CATEGORY, szItem);
	}

	// forward on to main app
	// wrap it up as a WM_PTDP_LISTCHANGE to simplify the handling
	return AfxGetMainWnd()->SendMessage(WM_PTDP_LISTCHANGE, MAKEWPARAM(PTDP_CATEGORY, bAdd), lp);
}

LRESULT CPreferencesDlg::OnToolPageTestTool(WPARAM wp, LPARAM lp)
{
	// forward on to main app
	return AfxGetMainWnd()->SendMessage(WM_PTP_TESTTOOL, wp, lp);
}

LRESULT CPreferencesDlg::OnGenPageClearMRU(WPARAM wp, LPARAM lp)
{
	// forward on to main app
	return AfxGetMainWnd()->SendMessage(WM_PGP_CLEARMRU, wp, lp);
}

void CPreferencesDlg::OnApply()
{
	CPreferencesDlgBase::OnApply();

	GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
}

LRESULT CPreferencesDlg::OnControlChange(WPARAM /*wp*/, LPARAM /*lp*/)
{
	GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);

	return 0L;
}
