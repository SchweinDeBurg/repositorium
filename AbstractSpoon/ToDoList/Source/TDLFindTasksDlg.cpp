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

// TDLFindTasksDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLFindTasksDlg.h"

#include "../../../CodeProject/Source/DeferWndMove.h"
#include "../../../CodeProject/Source/DlgUnits.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/DialogHelper.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/GraphicsMisc.h"
#include "../../Common/Preferences.h"
#include "../../../CodeProject/Source/Themed.h"
#include "../../../CodeProject/Source/DlgUnits.h"

#include <multimon.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static SEARCHPARAM DEFAULTSEARCH(TDCA_TASKNAMEORCOMMENTS, FO_INCLUDES);

const int TABSTOPS = 20;
const int MATCH_COLWIDTH = 50;

const UINT WM_FTD_SELECTITEM = (WM_APP+1);

enum FIND_INCLUDE
{ 
	FI_COMPLETED,
	FI_PARENT,
};

/////////////////////////////////////////////////////////////////////////////
// CTDLFindTasksDlg dialog


CTDLFindTasksDlg::CTDLFindTasksDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTDLFindTasksDlg::IDD, pParent), m_bDockable(FALSE)
{
	m_sResultsLabel.LoadString(IDS_FTD_RESULTS);
	
	//{{AFX_DATA_INIT(CTDLFindTasksDlg)
	m_bAutoSelectSingles = FALSE;
	//}}AFX_DATA_INIT
}

CTDLFindTasksDlg::~CTDLFindTasksDlg()
{

}

void CTDLFindTasksDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLFindTasksDlg)
	DDX_Control(pDX, IDC_INCLUDE, m_cbInclude);
	DDX_Control(pDX, IDC_SEARCHLIST, m_cbSearches);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_FINDLIST, m_lcFindSetup);
	DDX_Control(pDX, IDC_RESULTS, m_lcResults);
	DDX_CBIndex(pDX, IDC_TASKLISTOPTIONS, m_bAllTasklists);
	DDX_Text(pDX, IDC_RESULTSLABEL, m_sResultsLabel);
	//DDX_Check(pDX, IDC_AUTOSELECTSINGLES, m_bAutoSelectSingles);
}


BEGIN_MESSAGE_MAP(CTDLFindTasksDlg, CDialog)
	//{{AFX_MSG_MAP(CTDLFindTasksDlg)
	ON_COMMAND(ID_FIND_SAVESEARCH, OnSaveSearch)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FINDLIST, OnItemchangedRulelist)
	ON_CBN_SELCHANGE(IDC_SEARCHLIST, OnSelchangeSearchlist)
	ON_COMMAND(ID_FIND_NEWSEARCH, OnNewSearch)
	ON_COMMAND(ID_FIND_DELETESEARCH, OnDeleteSearch)
	ON_CBN_EDITCHANGE(IDC_SEARCHLIST, OnEditchangeSearchlist)
	ON_UPDATE_COMMAND_UI(ID_FIND_DELETESEARCH, OnUpdateDeleteSearch)
	ON_UPDATE_COMMAND_UI(ID_FIND_SAVESEARCH, OnUpdateSaveSearch)
	ON_COMMAND(ID_FIND_MOVERULEUP, OnMoveRuleUp)
	ON_UPDATE_COMMAND_UI(ID_FIND_MOVERULEUP, OnUpdateMoveRuleUp)
	ON_COMMAND(ID_FIND_MOVERULEDOWN, OnMoveRuleDown)
	ON_UPDATE_COMMAND_UI(ID_FIND_MOVERULEDOWN, OnUpdateMoveRuleDown)
	//}}AFX_MSG_MAP
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FIND_ADDRULE, OnAddRule)
	ON_COMMAND(ID_FIND_DELETERULE, OnDeleteRule)
	ON_UPDATE_COMMAND_UI(ID_FIND_DELETERULE, OnUpdateDeleteRule)
	ON_COMMAND(ID_FIND_FIND, OnFind)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_COMMAND(ID_DOCKRIGHT, OnDockright)
	ON_UPDATE_COMMAND_UI(ID_DOCKRIGHT, OnUpdateDockright)
	ON_COMMAND(ID_DOCKLEFT, OnDockleft)
	ON_UPDATE_COMMAND_UI(ID_DOCKLEFT, OnUpdateDockleft)
	ON_COMMAND(ID_UNDOCK, OnUndock)
	ON_UPDATE_COMMAND_UI(ID_UNDOCK, OnUpdateUndock)
	ON_COMMAND(ID_DOCKBELOW, OnDockbelow)
	ON_UPDATE_COMMAND_UI(ID_DOCKBELOW, OnUpdateDockbelow)
	ON_BN_CLICKED(IDC_SEARCHRESULTS, OnSearchresults)
	ON_WM_SETCURSOR()
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_RESULTS, OnItemchangingResults)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_RESULTS, OnItemActivated)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLFindTasksDlg message handlers


BOOL CTDLFindTasksDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDI_FIND);
	SetIcon(hIcon, TRUE);

	m_mgrPrompts.SetComboEditPrompt(m_cbSearches, IDS_FT_SAVESEARCHPROMPT);

	// always add a default search
	m_lcFindSetup.SetSearchParams(DEFAULTSEARCH);
	m_lcFindSetup.SetFocus();

	// toolbar
	VERIFY (InitializeToolbar());

	// gripper
	//GetDlgItem(IDC_GRIPPER)->ModifyStyle(0, SBS_SIZEGRIP | SBS_SIZEBOXTOPLEFTALIGN | WS_CLIPSIBLINGS);
	
	GetDlgItem(IDC_SELECTALL)->EnableWindow(FALSE);
	CenterWindow();

	// hide upper horz divider thwn themed
	if (CThemed::IsThemeActive())
		GetDlgItem(IDC_HDIVIDER2)->ShowWindow(SW_HIDE);
	
	LoadSettings();
	
    m_toolbar.RefreshButtonStates();
	
	ResizeDlg();
	EnableToolTips();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTDLFindTasksDlg::SetUITheme(const UITHEME& theme)
{
	ASSERT (GetSafeHwnd());

	m_theme = theme;
	m_brBkgnd.DeleteObject();
	m_brBkgnd.CreateSolidBrush(theme.crAppBackLight);

	m_toolbar.SetBackgroundColors(theme.crToolbarLight, theme.crToolbarDark, theme.nStyle == UIS_GRADIENT);
	Invalidate();
}

BOOL CTDLFindTasksDlg::InitializeToolbar()
{
	// toolbar
	VERIFY (m_toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP));

	if (m_toolbar.LoadToolBar(IDR_FINDDLG))
		m_toolbar.SetImage(IDB_FIND16, RGB(255, 0, 255));
	
	m_tbHelper.Initialize(&m_toolbar, this);
	
	// very important - turn OFF all the auto positioning and sizing
	// by default have no borders
	UINT nStyle = m_toolbar.GetBarStyle();
	nStyle &= ~(CCS_NORESIZE | CCS_NOPARENTALIGN | CBRS_BORDER_ANY);
	nStyle |= (CBRS_SIZE_FIXED | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_toolbar.SetBarStyle(nStyle);
	
	CRect rToolbar;
	GetDlgItem(IDC_TOOLBAR)->GetWindowRect(rToolbar);
	ScreenToClient(rToolbar);
	m_toolbar.MoveWindow(rToolbar);

	// insert combobox beside the separator to the right of ID_UNDOCK
	int nCombo = m_toolbar.CommandToIndex(ID_UNDOCK) + 2;
	TBBUTTON btnCombo = { 0, ID_SEARCHLIST, 0, TBSTYLE_SEP, 0, NULL };
	m_toolbar.GetToolBarCtrl().InsertButton(nCombo, &btnCombo);

	CRect rCombo;
	m_cbSearches.GetClientRect(rCombo);
	m_cbSearches.SetParent(&m_toolbar); 
	
	TBBUTTONINFO tbi;
	tbi.cbSize = sizeof( TBBUTTONINFO );
	tbi.cx = (WORD)rCombo.Width();
	tbi.dwMask = TBIF_SIZE;  // By index
	m_toolbar.GetToolBarCtrl().SetButtonInfo(ID_SEARCHLIST, &tbi );

	// reposition combo
	m_toolbar.GetToolBarCtrl().GetRect(ID_SEARCHLIST, rCombo);
	rCombo.bottom = rCombo.top + 200;
	m_cbSearches.MoveWindow(rCombo);

	return (m_toolbar.GetSafeHwnd() != NULL);
}

void CTDLFindTasksDlg::OnAddRule() 
{
	m_lcFindSetup.AddRule();
	m_lcFindSetup.SetFocus();
}

void CTDLFindTasksDlg::OnDeleteRule() 
{
	m_lcFindSetup.DeleteSelectedRule();
}

void CTDLFindTasksDlg::OnUpdateDeleteRule(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_lcFindSetup.CanDeleteSelectedRule());
}

/////////////////////////////////////////////////////////////////////////////////////

BOOL CTDLFindTasksDlg::Initialize(CWnd* pParent, BOOL bDockable)
{
	ASSERT (pParent || !bDockable);
	
	if (!pParent && bDockable)
		return FALSE;
	
	if (GetSafeHwnd())
		return TRUE;
	
	m_bDockable = bDockable;

	if (!Create(CTDLFindTasksDlg::IDD, pParent))
		return FALSE;
	
	return TRUE;
}

void CTDLFindTasksDlg::LoadSettings()
{
	CPreferences prefs;

	m_bAllTasklists = prefs.GetProfileInt("FindTasks", "SearchAllTaskLists", FALSE);
	m_bAutoSelectSingles = prefs.GetProfileInt("FindTasks", "AutoSelectSingles", FALSE);

	// include options
	m_cbInclude.SetCheck(FI_COMPLETED, prefs.GetProfileInt("FindTasks", "IncludeDoneTasks", FALSE));
//	m_cbInclude.SetCheck(FI_COLLAPSED, prefs.GetProfileInt("FindTasks", "IncludeCollapsedTasks", TRUE));
	m_cbInclude.SetCheck(FI_PARENT, prefs.GetProfileInt("FindTasks", "IncludeParentTasks", TRUE));

	if (m_bDockable)
	{
		DM_POS nPos = DMP_UNDOCKED, nLastPos = DMP_RIGHT;
		DWORD dwTopLeft = (DWORD)prefs.GetProfileInt("FindTasks", "TopLeft", -1);
		DWORD dwBottomRight = (DWORD)prefs.GetProfileInt("FindTasks", "BottomRight", -1);
		
		if (dwBottomRight != -1 && dwTopLeft != -1)
		{
			nPos = (DM_POS)prefs.GetProfileInt("FindTasks", "DockPos", DMP_UNDOCKED);
			nLastPos = (DM_POS)prefs.GetProfileInt("FindTasks", "LastDockPos", DMP_RIGHT);

			CRect rect(GET_X_LPARAM(dwTopLeft), GET_Y_LPARAM(dwTopLeft), 
					GET_X_LPARAM(dwBottomRight), GET_Y_LPARAM(dwBottomRight));

			// ensure this intersects with the desktop by a decent amount
			int BORDER = 200;
			rect.DeflateRect(BORDER, BORDER);

			if (NULL != MonitorFromRect(rect, MONITOR_DEFAULTTONULL))
			{
				rect.InflateRect(BORDER, BORDER);
				MoveWindow(rect);
			}
		}
		
		// use the default size of the page host to initialize the 
		// docked width/height
		CSize rDef = GetMinDockedSize(nPos);
		
		int nWidthDocked = prefs.GetProfileInt("FindTasks", "DockedWidth", rDef.cx);
		int nWidthDockedMax = prefs.GetProfileInt("FindTasks", "DockedWidthMax", -1);
		
		int nHeightDocked = prefs.GetProfileInt("FindTasks", "DockedHeight", rDef.cy);
		int nHeightDockedMax = prefs.GetProfileInt("FindTasks", "DockedHeightMax", -1);
		
		m_dockMgr.Initialize(GetParent(), this, nPos, nLastPos,
							nWidthDocked, nWidthDockedMax, nHeightDocked, nHeightDockedMax);
	}

	LoadSearches();

	UpdateData(FALSE);
}

CSize CTDLFindTasksDlg::GetMinDockedSize(DM_POS nPos)
{
	CRect rMin(0, 0, 0, 0);

	if (nPos == DMP_BELOW)
	{
		// we need enough height to show a few results and enough width
		// to display the results list
		CRect rCtrl;
		GetDlgItem(IDC_TASKLISTOPTIONS)->GetWindowRect(rCtrl);
		ScreenToClient(rCtrl);

		rMin.bottom = rCtrl.bottom + CDlgUnits(this).ToPixelsY(60);

		GetDlgItem(IDC_VDIVIDER)->GetWindowRect(rCtrl);
		ScreenToClient(rCtrl);
		
		rMin.right = rCtrl.right + CDlgUnits(this).ToPixelsX(60);
	}
	else
	{
		// we need enough width to show the toolbar and enough height
		// to display a few results
		CRect rCtrl;
		GetDlgItem(IDC_HDIVIDER)->GetWindowRect(rCtrl);
		ScreenToClient(rCtrl);

		rMin.bottom = rCtrl.bottom + CDlgUnits(this).ToPixelsY(90);

		GetDlgItem(IDC_TOOLBAR)->GetWindowRect(rCtrl);
		ScreenToClient(rCtrl);
		
		rMin.right = rCtrl.right;
	}

	// add border
	int nBorder = CDlgUnits(this).ToPixelsX(7);
	rMin.InflateRect(nBorder, nBorder);
	
	// allow for non-client border
	rMin.left = rMin.top = 0;
	CalcWindowRect(rMin);
	
	return rMin.Size();
}

BOOL CTDLFindTasksDlg::Show(BOOL bShow)
{
	if (!GetSafeHwnd())
		return FALSE;
	
	if (bShow)
	{
		// reset focus if dialog was previously hidden
		if (!IsWindowVisible())
		{
			if (m_lcFindSetup.GetItemText(0, 0).IsEmpty())
				m_lcFindSetup.SetCurSel(0, 0);

			else if (m_lcFindSetup.GetItemText(0, 1).IsEmpty())
				m_lcFindSetup.SetCurSel(0, 1);

			else 
				m_lcFindSetup.SetCurSel(0, 2);

			m_lcFindSetup.SetFocus();
		}
		
	    m_toolbar.RefreshButtonStates();

		ShowWindow(SW_SHOW);
		SetForegroundWindow(); // give it the focus
	}
	else
		ShowWindow(SW_HIDE);
	
	return TRUE;
}

void CTDLFindTasksDlg::AddResult(const SEARCHRESULT& result, LPCTSTR szTask, LPCTSTR szPath, const CFilteredToDoCtrl* pTDC)
{
	if (GetSafeHwnd())
	{
		// turn off stretchability for the duration
		m_lcResults.SetFirstColumnStretchy(FALSE);

		// add result
		BOOL bAdd = (m_cbInclude.GetCheck(FI_COMPLETED) || !result.HasFlag(RF_DONE)) &&
					(m_cbInclude.GetCheck(FI_PARENT) || !result.HasFlag(RF_PARENT));

		if (bAdd)
		{
			int nIndex = m_lcResults.AddResult(result, szTask, szPath, pTDC);

			if (nIndex != -1)
			{
				// update 'found' count
				m_sResultsLabel.Format(IDS_FTD_SOMERESULTS, GetResultCount());
				UpdateData(FALSE);
				
				// focus first item added
				if (!GetDlgItem(IDC_SELECTALL)->IsWindowEnabled())
				{
					SelectItem(nIndex);
					m_lcResults.SetFocus();

					// update 'search results' button' state
					m_toolbar.RefreshButtonStates();
					
					// enable 'select all' button
					GetDlgItem(IDC_SELECTALL)->EnableWindow(TRUE);
				}
			}
		}

		// restore stretchability
		m_lcResults.SetFirstColumnStretchy(TRUE);
	}
}

void CTDLFindTasksDlg::AddHeaderRow(LPCTSTR szText, BOOL bSpaceAbove)
{
	m_lcResults.AddHeaderRow(szText, bSpaceAbove);
}

BOOL CTDLFindTasksDlg::GetSearchAllTasklists()
{
	if (GetSafeHwnd())
		UpdateData();
	
	return m_bAllTasklists;
}

int CTDLFindTasksDlg::GetSearchParams(SEARCHPARAMS& params) 
{ 
	if (GetSafeHwnd())
		UpdateData();
	
	int nNumParam = m_lcFindSetup.GetSearchParams(params.rules); 

	// if the the search params include TDCA_DONE date then 
	// forcibly remove FT_HIDEDONE flag
	BOOL bIgnoreDone = !m_cbInclude.GetCheck(FI_COMPLETED);
	params.bIgnoreDone = bIgnoreDone;

	if (bIgnoreDone)
	{
		for (int nParam = 0; nParam < nNumParam; nParam++)
		{
			if (params.rules[nParam].attrib == TDCA_DONEDATE)
			{
				params.bIgnoreDone = FALSE;
				break;
			}
		}
	}

	return params.rules.GetSize();
}

BOOL CTDLFindTasksDlg::GetAutoSelectSingles()
{
	if (GetSafeHwnd())
		UpdateData();
	
	return m_bAutoSelectSingles;
}

int CTDLFindTasksDlg::GetResultCount() const
{
	return m_lcResults.GetResultCount();
}

int CTDLFindTasksDlg::GetResultCount(const CFilteredToDoCtrl* pTDC) const
{
	return m_lcResults.GetResultCount(pTDC);
}

int CTDLFindTasksDlg::GetAllResults(CFTDResultsArray& aResults) const
{
	return m_lcResults.GetAllResults(aResults);
}

int CTDLFindTasksDlg::GetResults(const CFilteredToDoCtrl* pTDC, CFTDResultsArray& aResults) const
{
	return m_lcResults.GetResults(pTDC, aResults);
}

int CTDLFindTasksDlg::GetResultIDs(const CFilteredToDoCtrl* pTDC, CDWordArray& aTaskIDs) const
{
	return m_lcResults.GetResultIDs(pTDC, aTaskIDs);
}

void CTDLFindTasksDlg::RefreshSearch()
{
	ASSERT (GetSafeHwnd());
	
	OnFind();
}

void CTDLFindTasksDlg::OnFind() 
{
	m_lcFindSetup.EndEdit();
	UpdateData();

	// notify parent
	GetParent()->PostMessage(WM_FTD_FIND);
	
	// clear results
	DeleteAllResults();
	
	// disable 'select all' button
	GetDlgItem(IDC_SELECTALL)->EnableWindow(FALSE);
}

void CTDLFindTasksDlg::OnClose() 
{
	CDialog::OnClose();

	// end any current edit
	m_lcFindSetup.EndEdit();
	
	// hide
	ShowWindow(SW_HIDE);
	
	// notify parent
	GetParent()->SendMessage(WM_FTD_CLOSE);
}

void CTDLFindTasksDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	// end any current edit
	if (m_lcFindSetup.GetSafeHwnd())
		m_lcFindSetup.EndEdit();
	
	ResizeDlg(cx, cy);

	if (m_toolbar.GetSafeHwnd())
		m_toolbar.RefreshButtonStates();
}

void CTDLFindTasksDlg::ResizeDlg(int cx, int cy)
{
	if (m_lcResults.GetSafeHwnd())
	{
		if (!cx && !cy)
		{
			CRect rClient;
			GetClientRect(rClient);
			
			cx = rClient.right;
			cy = rClient.bottom;
			
			// check again 
			if (!cx && !cy)
				return;
		}
		
		// we compare the new size with the lower right hand corner
		// of the results list
		CRect rect;
		m_lcResults.GetWindowRect(rect);
		ScreenToClient(rect);
		
		int nXOffset = cx - rect.right;
		int nYOffset = cy - rect.bottom;
		
		BOOL bDockedBelow = (m_dockMgr.GetDockPos() == DMP_BELOW);
		
		{
			CDeferWndMove dwm(10);
			
			// dividers
			CRect rHDiv, rVDiv, rTBDiv;
			
			rHDiv = dwm.ResizeCtrl(this, IDC_HDIVIDER);
			dwm.ResizeCtrl(this, IDC_HDIVIDER, cx - rHDiv.right - 8);

			rVDiv = dwm.ResizeCtrl(this, IDC_VDIVIDER);
			rVDiv = dwm.ResizeCtrl(this, IDC_VDIVIDER, 0, cy - rVDiv.bottom - 8);

			rTBDiv = dwm.ResizeCtrl(this, IDC_HDIVIDER2);
			rTBDiv.right = (bDockedBelow ? rVDiv.left : cx) - 8;
			dwm.MoveWindow(GetDlgItem(IDC_HDIVIDER2), rTBDiv);
			
			// gripper
			//dwm.OffsetCtrl(this, IDC_GRIPPER, nXOffset, nYOffset);

			// resize rules
			CRect rRules = dwm.ResizeCtrl(this, IDC_FINDLIST);

			if (bDockedBelow)
			{
				rRules.right = rVDiv.left - 8;
				rRules.bottom = cy - 8;
			}
			else
			{
				rRules.right = cx - 8;
				rRules.bottom = rHDiv.top - 8;
			}

			dwm.MoveWindow(&m_lcFindSetup, rRules);

			// resize the toolbar to be the same width as the rules list
			CRect rToolbar;
			m_toolbar.GetWindowRect(rToolbar);
			ScreenToClient(rToolbar);
			rToolbar.right = rRules.right;

			dwm.MoveWindow(&m_toolbar, rToolbar);
			
			// move 'select all', results and other assorted items
			// depending on where we are docked
			CRect rSelAll = dwm.ResizeCtrl(this, IDC_SELECTALL);
			
			if (bDockedBelow)
			{
				nXOffset = rVDiv.right - rSelAll.left + 8;
				nYOffset = rVDiv.top - rSelAll.top;
			}
			else
			{
				nXOffset = rHDiv.left - rSelAll.left;
				nYOffset = rHDiv.top - rSelAll.top + 8;
			}
			
			dwm.OffsetCtrl(this, IDC_SELECTALL, nXOffset, nYOffset);
			dwm.OffsetCtrl(this, IDC_RESULTSLABEL, nXOffset, nYOffset);
			
			//CRect rAuto = dwm.OffsetCtrl(this, IDC_AUTOSELECTSINGLES);
			//rAuto = dwm.OffsetCtrl(this, IDC_AUTOSELECTSINGLES, nXOffset, cy - rAuto.bottom - 10);
			
			CRect rResults = dwm.OffsetCtrl(this, IDC_RESULTS, nXOffset, nYOffset);
			
			rResults.right = cx - 8;
			rResults.bottom = cy - 8;
			
			dwm.MoveWindow(&m_lcResults, rResults);

		}
		
		// hide the gripper if we are maximized
		//GetDlgItem(IDC_GRIPPER)->ShowWindow(m_dockMgr.Maximized() ? SW_HIDE : SW_SHOW);
		
		// hide appropriate divider
		GetDlgItem(IDC_HDIVIDER)->ShowWindow(bDockedBelow ? SW_HIDE : SW_SHOW);
		GetDlgItem(IDC_VDIVIDER)->ShowWindow(bDockedBelow ? SW_SHOW : SW_HIDE);
	}	
}

void CTDLFindTasksDlg::OnItemActivated(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	ASSERT (pNMListView->iItem >= 0);
	
	FTDRESULT* pRes = m_lcResults.GetResult(pNMListView->iItem);
	
	if (pRes)
		GetParent()->SendMessage(WM_FTD_SELECTRESULT, 0, (LPARAM)pRes);
	
	*pResult = 0;
}

void CTDLFindTasksDlg::OnDestroy() 
{
	SaveSettings();
	DeleteAllResults();
	
	CDialog::OnDestroy();
}

void CTDLFindTasksDlg::SaveSettings()
{
	UpdateData();

	CPreferences prefs;
	
	prefs.WriteProfileInt("FindTasks", "SearchAllTaskLists", m_bAllTasklists);
	prefs.WriteProfileInt("FindTasks", "AutoSelectSingles", m_bAutoSelectSingles);

	// include settings
	prefs.WriteProfileInt("FindTasks", "IncludeDoneTasks", m_cbInclude.GetCheck(FI_COMPLETED));
//	prefs.WriteProfileInt("FindTasks", "IncludeCollapsedTasks", m_cbInclude.GetCheck(FI_COLLAPSED));
	prefs.WriteProfileInt("FindTasks", "IncludeParentTasks", m_cbInclude.GetCheck(FI_PARENT));

	// pos
	CRect rDialog = m_dockMgr.GetUnDockedRect();
	
	prefs.WriteProfileInt("FindTasks", "TopLeft", MAKELPARAM(rDialog.left, rDialog.top));
	prefs.WriteProfileInt("FindTasks", "BottomRight", MAKELPARAM(rDialog.right, rDialog.bottom));
	prefs.WriteProfileInt("FindTasks", "DockPos", m_dockMgr.GetDockPos());
	prefs.WriteProfileInt("FindTasks", "LastDockPos", m_dockMgr.GetLastDockPos());
	prefs.WriteProfileInt("FindTasks", "DockedWidth", m_dockMgr.GetDockedWidth(FALSE));
	prefs.WriteProfileInt("FindTasks", "DockedWidthMax", m_dockMgr.GetDockedWidth(TRUE));
	prefs.WriteProfileInt("FindTasks", "DockedHeight", m_dockMgr.GetDockedHeight(FALSE));
	prefs.WriteProfileInt("FindTasks", "DockedHeightMax", m_dockMgr.GetDockedHeight(TRUE));

	// searches
	SaveSearches();
}

void CTDLFindTasksDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CDialog::OnGetMinMaxInfo(lpMMI);
	
	if (m_dockMgr.Initialized())
	{
		CSize rDef = GetMinDockedSize(m_dockMgr.GetDockPos());

		lpMMI->ptMinTrackSize.y = rDef.cy;
		lpMMI->ptMinTrackSize.x = rDef.cx;
	}
}

BOOL CTDLFindTasksDlg::PreTranslateMessage(MSG* pMsg) 
{
	// handle enter key if results list has the focus
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == m_lcResults)
	{
		int nSel = m_lcResults.GetCurSel();
		
		if (nSel != -1)
		{
			FTDRESULT* pRes = m_lcResults.GetResult(nSel);
			
			if (pRes)
				GetParent()->SendMessage(WM_FTD_SELECTRESULT, 0, (LPARAM)pRes);
			
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CTDLFindTasksDlg::OnSelectall() 
{
	int nRes = GetResultCount();

	// if there is only one result then act as if the user 
	// clicked on it directly
	if (nRes == 1)
	{
		FTDRESULT* pRes = m_lcResults.GetResult(nRes);

		if (pRes && m_lcResults.GetItemCount() == 2)
			GetParent()->SendMessage(WM_FTD_SELECTRESULT, 0, (LPARAM)pRes);
	}
	else if (nRes) // > 1
		GetParent()->SendMessage(WM_FTD_SELECTALL);
}

void CTDLFindTasksDlg::OnDockright() 
{
	// toggle docked state
	if (m_dockMgr.GetDockPos() == DMP_RIGHT)
		m_dockMgr.UnDock();
	else
		m_dockMgr.Dock(DMP_RIGHT);

	ResizeDlg();
    m_toolbar.RefreshButtonStates();
}

void CTDLFindTasksDlg::OnUpdateDockright(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_dockMgr.GetDockPos() == DMP_RIGHT);
}

void CTDLFindTasksDlg::OnDockleft() 
{
	// toggle docked state
	if (m_dockMgr.GetDockPos() == DMP_LEFT)
		m_dockMgr.UnDock();
	else
		m_dockMgr.Dock(DMP_LEFT);

	ResizeDlg();
	
    m_toolbar.RefreshButtonStates();
}

void CTDLFindTasksDlg::OnUpdateDockleft(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_dockMgr.GetDockPos() == DMP_LEFT);
}

void CTDLFindTasksDlg::OnUndock() 
{
	m_dockMgr.Dock(DMP_UNDOCKED);
	ResizeDlg();
	
    m_toolbar.RefreshButtonStates();
}

void CTDLFindTasksDlg::OnUpdateUndock(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_dockMgr.GetDockPos() == DMP_UNDOCKED);
}

void CTDLFindTasksDlg::OnDockbelow() 
{
	// toggle docked state
	if (m_dockMgr.GetDockPos() == DMP_BELOW)
		m_dockMgr.UnDock();
	else
		m_dockMgr.Dock(DMP_BELOW);
	ResizeDlg();
	
    m_toolbar.RefreshButtonStates();
}

void CTDLFindTasksDlg::OnUpdateDockbelow(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_dockMgr.GetDockPos() == DMP_BELOW);
}

void CTDLFindTasksDlg::DeleteResults(const CFilteredToDoCtrl* pTDC)
{
	m_lcResults.DeleteResults(pTDC);

	// update 'found' count
	m_sResultsLabel.Format(IDS_FTD_SOMERESULTS, GetResultCount());
	UpdateData(FALSE);
}

void CTDLFindTasksDlg::DeleteAllResults()
{
	m_lcResults.DeleteAllResults();

	// update 'found' count
	m_sResultsLabel.Format(IDS_FTD_SOMERESULTS, m_lcResults.GetItemCount());
	UpdateData(FALSE);
}

void CTDLFindTasksDlg::OnSearchresults() 
{
	UpdateData();
}

BOOL CTDLFindTasksDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// if the cursor is over a non-header row then show the 'hand' pointer
	// can't use LVS_EX_ONECLICKACTIVATE for this because this *cannot* be
	// overridden for header items
	if (pWnd == &m_lcResults && nHitTest == HTCLIENT)
	{
		CPoint ptCursor(::GetMessagePos());
		pWnd->ScreenToClient(&ptCursor);

		int nItem = m_lcResults.HitTest(ptCursor);

		if (nItem != -1 && m_lcResults.GetResult(nItem)) // normal result
		{
			SetCursor(GraphicsMisc::HandCursor());
			return TRUE;
		}
	}
	
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CTDLFindTasksDlg::OnItemchangingResults(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// we're interested in keyboard selection state changes only
	if (pNMListView->uChanged & LVIF_STATE)
	{
		BOOL bWasSel = (pNMListView->uOldState & LVIS_SELECTED);
		BOOL bIsSel = (pNMListView->uNewState & LVIS_SELECTED);
		
		// has a header row just become selected?
		if (bIsSel && !bWasSel && pNMListView->lParam == 0)
		{
			if (Misc::KeyIsPressed(VK_LBUTTON))
			{
				// restore previous selection
				SelectItem(m_nCurSel);
			}
			else // keyboard
			{
				// we need to get the next valid item in the direction
				// the user is moving
				BOOL bDown = (GetSelectedItem() < pNMListView->iItem);
				int nNextItem = GetNextResult(pNMListView->iItem, bDown);
				
				if (nNextItem != -1)
					SelectItem(nNextItem);
				else
				{
					SelectItem(m_nCurSel);

					// make sure the non-selectable item is visible
					m_lcResults.EnsureVisible(pNMListView->iItem, FALSE);
				}
			}
			
			*pResult = TRUE;
			return;
		}
	}

	m_nCurSel = pNMListView->iItem;
	
	*pResult = 0;
}

int CTDLFindTasksDlg::GetNextResult(int nItem, BOOL bDown)
{
	int nNext = nItem + (bDown ? 1 : -1); // next item

	while (nNext >= 0 && nNext < m_lcResults.GetItemCount())
	{
		if (m_lcResults.GetResult(nNext))
			return nNext;

		nNext += bDown ? 1 : -1; // next item
	}

	// no next item
	return -1;
}

void CTDLFindTasksDlg::SelectItem(int nItem)
{
	m_lcResults.SetItemState(nItem, LVIS_FOCUSED | LVIS_SELECTED, 
							 		 LVIS_FOCUSED | LVIS_SELECTED);

	m_nCurSel = nItem;
}

int CTDLFindTasksDlg::GetSelectedItem()
{
	return m_nCurSel;
}

void CTDLFindTasksDlg::OnItemchangedRulelist(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_toolbar.GetSafeHwnd())
		m_toolbar.RefreshButtonStates();

	*pResult = 0;
}

void CTDLFindTasksDlg::OnSaveSearch() 
{
	m_lcFindSetup.EndEdit();
	
	CString sSearch;
	m_cbSearches.GetWindowText(sSearch);

	CSearchParamArray params;
	m_lcFindSetup.GetSearchParams(params);

	SaveSearch(sSearch, params);

	// is it a new search?
	if (m_cbSearches.FindStringExact(-1, sSearch) == CB_ERR)
		m_cbSearches.AddString(sSearch); // yes

	m_sActiveSearch = sSearch;
}

BOOL CTDLFindTasksDlg::LoadSearch(LPCTSTR szName, CSearchParamArray& params) const
{
	CPreferences prefs;
	CEnString sKey("FindTasks\\Searches\\%s", szName);

	int nNumRules = prefs.GetProfileInt(sKey, "NumRules", 0);

	for (int nRule = 0; nRule < nNumRules; nRule++)
	{
		CEnString sRule("%s\\Rule%d", sKey, nRule);
		SEARCHPARAM rule;

		rule.attrib = (TDC_ATTRIBUTE)prefs.GetProfileInt(sRule, "Attribute", TDCA_NONE);
		rule.op = (FIND_OPERATOR)prefs.GetProfileInt(sRule, "Operator", FO_EQUALS);
		rule.bAnd = prefs.GetProfileInt(sRule, "And", TRUE);
		rule.dwFlags = prefs.GetProfileInt(sRule, "Flags", 0);

		// read value
		CString sValue = prefs.GetProfileString(sRule, "Value");

		switch (rule.GetAttribType())
		{
		case FT_STRING:
			rule.sValue = sValue;
			break;

		case FT_DATE:
		case FT_DOUBLE:
		case FT_TIME:
			rule.dValue = atof(sValue);
			break;

		case FT_INTEGER:
		case FT_BOOL:
			rule.nValue = atoi(sValue);
			break;
		}

		params.Add(rule);
	}

	return (nNumRules >= 1);
}

BOOL CTDLFindTasksDlg::SaveSearch(LPCTSTR szName, const CSearchParamArray& params)
{
	CPreferences prefs;
	CEnString sKey("FindTasks\\Searches\\%s", szName);

	prefs.WriteProfileInt(sKey, "NumRules", params.GetSize());

	for (int nRule = 0; nRule < params.GetSize(); nRule++)
	{
		CEnString sRule("%s\\Rule%d", sKey, nRule);
		const SEARCHPARAM& rule = params[nRule];

		prefs.WriteProfileInt(sRule, "Attribute", rule.attrib);
		prefs.WriteProfileInt(sRule, "Operator", rule.op);
		prefs.WriteProfileInt(sRule, "And", rule.bAnd);
		prefs.WriteProfileString(sRule, "Value", rule.ValueAsString());
		prefs.WriteProfileInt(sRule, "Flags", rule.dwFlags);
	}

	return TRUE;
}

int CTDLFindTasksDlg::LoadSearches()
{
	CPreferences prefs;

	m_cbSearches.ResetContent();

	int nNumItems = prefs.GetProfileInt("FindTasks\\Searches", "NumSearches", 0);

	for (int nItem = 0; nItem < nNumItems; nItem++)
	{
		CEnString sKey("Search%d", nItem);
		CString sSearch = prefs.GetProfileString("FindTasks\\Searches", sKey);

		// only add to the list if it can be loaded
		CSearchParamArray dummy;

		if (LoadSearch(sSearch, dummy))
			m_cbSearches.AddString(sSearch);
	}

	// restore last search
	CString sSearch = prefs.GetProfileString("FindTasks\\Searches", "Current");

	// backup
	if (m_cbSearches.FindStringExact(-1, sSearch) == CB_ERR)
	{
		if (m_cbSearches.GetCount())
			m_cbSearches.GetLBText(0, sSearch);
	}
	
	if (m_cbSearches.SelectString(-1, sSearch) != CB_ERR)
	{
		CSearchParamArray params;
		VERIFY (LoadSearch(sSearch, params));

		m_lcFindSetup.SetSearchParams(params);
		m_sActiveSearch = sSearch;
	}

	return m_cbSearches.GetCount();
}

int CTDLFindTasksDlg::SaveSearches()
{
	CPreferences prefs;

	int nNumSearches = m_cbSearches.GetCount();
	prefs.WriteProfileInt("FindTasks\\Searches", "NumSearches", nNumSearches);

	for (int nItem = 0; nItem < nNumSearches; nItem++)
	{
		CEnString sKey("Search%d", nItem);
		CString sSearch;

		m_cbSearches.GetLBText(nItem, sSearch);
		prefs.WriteProfileString("FindTasks\\Searches", sKey, sSearch);
	}

	// save active search
/*
	if (m_sActiveSearch == GetCurrentSearch(FALSE))
	{
		CSearchParamArray params;

		m_lcFindSetup.GetSearchParams(params);
		SaveSearch(m_sActiveSearch, params);
	}
*/

	prefs.WriteProfileString("FindTasks\\Searches", "Current", m_sActiveSearch);

	return m_cbSearches.GetCount();
}

void CTDLFindTasksDlg::OnSelchangeSearchlist() 
{
	// do we want to save the current search?
	// we do if the combo text is still the same as the active search
/*
	if (m_sActiveSearch == GetCurrentSearch(FALSE))
	{
		CSearchParamArray params;

		m_lcFindSetup.GetSearchParams(params);
		SaveSearch(m_sActiveSearch, params);
	}
*/

	int nSel = m_cbSearches.GetCurSel();	

	if (nSel != CB_ERR)
	{
		CString sName;
		m_cbSearches.GetLBText(nSel, sName);

		CSearchParamArray params;

		if (LoadSearch(sName, params))
		{
			m_lcFindSetup.SetSearchParams(params);
			m_sActiveSearch = sName;
		}
	}

	m_toolbar.RefreshButtonStates(FALSE);
}

CString CTDLFindTasksDlg::GetCurrentSearch(BOOL bSelected)
{
	CString sSearch;

	if (bSelected)
	{
		int nSel = m_cbSearches.GetCurSel();	

		if (nSel != CB_ERR)
			m_cbSearches.GetLBText(nSel, sSearch);
	}
	else
		m_cbSearches.GetWindowText(sSearch);

	return sSearch;
}

void CTDLFindTasksDlg::OnNewSearch() 
{
	m_cbSearches.SetCurSel(-1); // clears the combo edit
	m_lcFindSetup.ClearSearch();
	m_sActiveSearch.Empty();

	m_toolbar.RefreshButtonStates();
}

void CTDLFindTasksDlg::OnDeleteSearch() 
{
	CString sSearch = GetCurrentSearch(FALSE);

	if (!sSearch.IsEmpty())
	{
		int nSearch = m_cbSearches.FindStringExact(-1, sSearch);
		
		if (nSearch != CB_ERR)
			m_cbSearches.DeleteString(nSearch);
		
		m_lcFindSetup.ClearSearch();
		
		// set the selection to the next search
		if (nSearch >= m_cbSearches.GetCount())
			nSearch = m_cbSearches.GetCount() - 1;
		
		if (nSearch != CB_ERR)
		{
			m_cbSearches.GetLBText(nSearch, sSearch);
			
			CSearchParamArray params;
			
			if (LoadSearch(sSearch, params))
			{
				m_lcFindSetup.SetSearchParams(params);
				m_sActiveSearch = sSearch;
			}
			else
			{
				nSearch = -1;
				m_sActiveSearch.Empty();
			}
		}
		
		m_cbSearches.SetCurSel(nSearch);
	}
	
	m_toolbar.RefreshButtonStates();
}

void CTDLFindTasksDlg::OnEditchangeSearchlist() 
{
	m_toolbar.RefreshButtonStates();
}

void CTDLFindTasksDlg::OnUpdateDeleteSearch(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!GetCurrentSearch(FALSE).IsEmpty());
}

void CTDLFindTasksDlg::OnUpdateSaveSearch(CCmdUI* pCmdUI) 
{
	// must have a name
	BOOL bEnable = !GetCurrentSearch(FALSE).IsEmpty();

	// and be different from currently saved search
	// TODO

	pCmdUI->Enable(bEnable);
}

void CTDLFindTasksDlg::OnOK() 
{ 
	OnFind();
}

void CTDLFindTasksDlg::OnMoveRuleUp() 
{
	m_lcFindSetup.MoveSelectedRuleUp();
}

void CTDLFindTasksDlg::OnUpdateMoveRuleUp(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_lcFindSetup.CanMoveSelectedRuleUp());
}

void CTDLFindTasksDlg::OnMoveRuleDown() 
{
	m_lcFindSetup.MoveSelectedRuleDown();
}

void CTDLFindTasksDlg::OnUpdateMoveRuleDown(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_lcFindSetup.CanMoveSelectedRuleDown());
}

BOOL CTDLFindTasksDlg::OnToolTipNotify(UINT /*id*/, NMHDR* pNMHDR, LRESULT* /*pResult*/)
{
    // Get the tooltip structure.
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;

    // Actually the idFrom holds Control's handle.
    UINT CtrlHandle = pNMHDR->idFrom;

    // Check once again that the idFrom holds handle itself.
    if (pTTT->uFlags & TTF_IDISHWND)
    {
		static CString sTooltip;
		sTooltip.Empty();

        // Get the control's ID.
        UINT nID = ::GetDlgCtrlID( HWND( CtrlHandle ));

        switch( nID )
        {
        case IDC_INCLUDE:
			sTooltip = m_cbInclude.GetTooltip();
            break;
		}
		
		if (!sTooltip.IsEmpty())
		{
			// Set the tooltip text.
			::SendMessage(pNMHDR->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 300);
			pTTT->lpszText = (LPSTR)(LPCTSTR)sTooltip;
	        return TRUE;
		}
    }

    // Not handled.
    return FALSE;
}

HBRUSH CTDLFindTasksDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (CThemed::IsThemeActive())
	{
		ASSERT (m_brBkgnd.GetSafeHandle());

		if (nCtlColor == CTLCOLOR_STATIC)
		{
			//pDC->SetTextColor(m_theme.crAppText);
			pDC->SetBkMode(TRANSPARENT);
			hbr = m_brBkgnd;
		}
	}
	
	return hbr;
}

BOOL CTDLFindTasksDlg::OnEraseBkgnd(CDC* pDC) 
{
	if (CThemed::IsThemeActive())
	{
		//CDialogHelper::ExcludeChildren(this, pDC);

		CRect rClient;
		GetClientRect(rClient);

		// draw gradient behind the toolbar up to the window edge
		CRect rToolbar;
		m_toolbar.GetWindowRect(rToolbar);
		ScreenToClient(rToolbar);

		CDlgUnits dlu(this);
		rToolbar.InflateRect(dlu.ToPixelsX(6), dlu.ToPixelsY(2), dlu.ToPixelsX(6), dlu.ToPixelsX(1));

		if (m_theme.nStyle == UIS_GRADIENT)
			GraphicsMisc::DrawGradient(*pDC, rToolbar, m_theme.crToolbarLight, m_theme.crToolbarDark, FALSE);
		else
		{
			CRect rElement(rToolbar);
			rElement.bottom = rElement.top + (rToolbar.Height() * 2) / 5;
			pDC->FillSolidRect(rElement, m_theme.crToolbarLight);

			rElement.top = rElement.bottom;
			rElement.bottom = rToolbar.bottom;
			pDC->FillSolidRect(rElement, m_theme.crToolbarDark);
		}

		// draw a bevel below the toolbar
		pDC->FillSolidRect(0, rToolbar.bottom - 2, rClient.Width(), 1, GetSysColor(COLOR_3DSHADOW));
		pDC->FillSolidRect(0, rToolbar.bottom - 1, rClient.Width(), 1, GetSysColor(COLOR_3DHIGHLIGHT));

		pDC->ExcludeClipRect(rToolbar);

		// then app light for the rest
		pDC->FillSolidRect(rClient, m_theme.crAppBackLight);
		return TRUE;
	}
	
	// else
	return CDialog::OnEraseBkgnd(pDC);
}
