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

// TDLFilterBar.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLFilterBar.h"
#include "TDCMsg.h"
#include "FilteredToDoCtrl.h"

#include "../../../CodeProject/Source/DeferWndMove.h"
#include "../../../CodeProject/Source/DlgUnits.h"
#include "../../../CodeProject/Source/EnString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

const int FILTERCTRLXSPACING = 6; // dlu
const int FILTERCTRLYSPACING = 2; // dlu

struct FILTERCTRL
{
	UINT nIDLabel;
	UINT nIDCtrl;
	int nLenLabelDLU;
	int nLenCtrlDLU;
	TDC_COLUMN nType;
};

static FILTERCTRL FILTERCTRLS[] =
{
	{ IDC_FILTERLABEL, IDC_FILTERCOMBO, 22, 150, (TDC_COLUMN)-1 },
	{ IDC_TITLEFILTERLABEL, IDC_TITLEFILTERTEXT, 47, 75, (TDC_COLUMN)-1 },
	{ IDC_PRIORITYFILTERLABEL, IDC_PRIORITYFILTERCOMBO, 47, 75, TDCC_PRIORITY },
	{ IDC_RISKFILTERLABEL, IDC_RISKFILTERCOMBO, 47, 75, TDCC_RISK },
	{ IDC_ALLOCTOFILTERLABEL, IDC_ALLOCTOFILTERCOMBO, 47, 75, TDCC_ALLOCTO },
	{ IDC_ALLOCBYFILTERLABEL, IDC_ALLOCBYFILTERCOMBO, 47, 75, TDCC_ALLOCBY },
	{ IDC_STATUSFILTERLABEL, IDC_STATUSFILTERCOMBO, 47, 75, TDCC_STATUS },
	{ IDC_CATEGORYFILTERLABEL, IDC_CATEGORYFILTERCOMBO, 47, 75, TDCC_CATEGORY },
	{ IDC_VERSIONFILTERLABEL, IDC_VERSIONFILTERCOMBO, 47, 75, TDCC_VERSION },
	{ IDC_OPTIONFILTERLABEL, IDC_OPTIONFILTERCOMBO, 47, 75, (TDC_COLUMN)-1 }
};

const int NUMFILTERCTRLS = sizeof(FILTERCTRLS) / sizeof(FILTERCTRL);

#define WM_WANTCOMBOPROMPT (WM_APP+1)

/////////////////////////////////////////////////////////////////////////////
// CFilterBar dialog

CTDLFilterBar::CTDLFilterBar(CWnd* pParent /*=NULL*/):
CDialog(IDD_FILTER_BAR, pParent),
m_cbCategoryFilter(TRUE, IDS_NOCATEGORY, IDS_TDC_ANY),
m_cbAllocToFilter(TRUE, IDS_NOALLOCTO, IDS_TDC_ANYONE),
m_cbAllocByFilter(TRUE, IDS_NOONE, IDS_TDC_ANYONE),
m_cbStatusFilter(TRUE, IDS_NOSTATUS, IDS_TDC_ANY),
m_cbVersionFilter(TRUE, IDS_NOVERSION, IDS_TDC_ANY),
m_nView(FTCV_UNSET),
m_bCustomFilter(FALSE)
{
	//{{AFX_DATA_INIT(CFilterBar)
	//}}AFX_DATA_INIT

	m_aVisibility.SetSize(TDCC_LAST);

	// add update button to title text
	m_eTitleFilter.AddButton(1, 0xC4,
		CEnString(IDS_TDC_UPDATEFILTER_TIP),
		CALC_BTNWIDTH, _T("Wingdings"));
}

void CTDLFilterBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterBar)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_VERSIONFILTERCOMBO, m_cbVersionFilter);
	DDX_Control(pDX, IDC_OPTIONFILTERCOMBO, m_cbOptions);
	DDX_Control(pDX, IDC_FILTERCOMBO, m_cbTaskFilter);
	DDX_Control(pDX, IDC_ALLOCTOFILTERCOMBO, m_cbAllocToFilter);
	DDX_Control(pDX, IDC_ALLOCBYFILTERCOMBO, m_cbAllocByFilter);
	DDX_Control(pDX, IDC_CATEGORYFILTERCOMBO, m_cbCategoryFilter);
	DDX_Control(pDX, IDC_STATUSFILTERCOMBO, m_cbStatusFilter);
	DDX_Control(pDX, IDC_PRIORITYFILTERCOMBO, m_cbPriorityFilter);
	DDX_Control(pDX, IDC_RISKFILTERCOMBO, m_cbRiskFilter);
	DDX_Control(pDX, IDC_TITLEFILTERTEXT, m_eTitleFilter);
	DDX_Text(pDX, IDC_TITLEFILTERTEXT, m_filter.sTitle);

	// special handling
	if (pDX->m_bSaveAndValidate)
	{
		// filter
		m_filter.nFilter = m_cbTaskFilter.GetSelectedFilter();

		// priority
		int nIndex;
		DDX_CBIndex(pDX, IDC_PRIORITYFILTERCOMBO, nIndex);

		if (nIndex == 0) // any
		{
			m_filter.nPriority = FT_ANYPRIORITY;
		}

		else if (nIndex == 1) // none
		{
			m_filter.nPriority = FT_NOPRIORITY;
		}
		else
		{
			m_filter.nPriority = nIndex - 2;
		}

		// risk
		DDX_CBIndex(pDX, IDC_RISKFILTERCOMBO, nIndex);

		if (nIndex == 0) // any
		{
			m_filter.nRisk = FT_ANYRISK;
		}

		else if (nIndex == 1) // none
		{
			m_filter.nRisk = FT_NORISK;
		}
		else
		{
			m_filter.nRisk = nIndex - 2;
		}

		// cats
		m_cbCategoryFilter.GetChecked(m_filter.aCategories);

		// allocto
		m_cbAllocToFilter.GetChecked(m_filter.aAllocTo);

		// status
		m_cbStatusFilter.GetChecked(m_filter.aStatus);

		// allocby
		m_cbAllocByFilter.GetChecked(m_filter.aAllocBy);

		// version
		m_cbVersionFilter.GetChecked(m_filter.aVersions);

		// flags
		m_filter.dwFlags = m_cbOptions.GetOptions();
	}
	else
	{
		// filter
		if (m_bCustomFilter)
		{
			m_cbTaskFilter.SetCustomFilter(m_sCustomFilter);
		}
		else
		{
			m_cbTaskFilter.SetSelectedFilter(m_filter.nFilter);
		}

		// priority
		int nIndex;

		if (m_filter.nPriority == FT_ANYPRIORITY)
		{
			nIndex = 0;
		}

		else if (m_filter.nPriority == FT_NOPRIORITY)
		{
			nIndex = 1;
		}
		else
		{
			nIndex = m_filter.nPriority + 2;
		}

		DDX_CBIndex(pDX, IDC_PRIORITYFILTERCOMBO, nIndex);

		// risk
		if (m_filter.nRisk == FT_ANYRISK)
		{
			nIndex = 0;
		}

		else if (m_filter.nRisk == FT_NORISK)
		{
			nIndex = 1;
		}
		else
		{
			nIndex = m_filter.nRisk + 2;
		}

		DDX_CBIndex(pDX, IDC_RISKFILTERCOMBO, nIndex);

		// cats
		m_cbCategoryFilter.SetChecked(m_filter.aCategories);

		// allocto
		m_cbAllocToFilter.SetChecked(m_filter.aAllocTo);

		// status
		m_cbStatusFilter.SetChecked(m_filter.aStatus);

		// allocby
		m_cbAllocByFilter.SetChecked(m_filter.aAllocBy);

		// version
		m_cbVersionFilter.SetChecked(m_filter.aVersions);

		// options
		m_cbOptions.Initialize(m_filter, m_nView);
	}
}

BEGIN_MESSAGE_MAP(CTDLFilterBar, CDialog)
	//{{AFX_MSG_MAP(CFilterBar)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ALLALLOCTO, OnSelchangeFilter)
	ON_BN_CLICKED(IDC_ALLCATEGORIES, OnSelchangeFilter)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_FILTERCOMBO, OnSelchangeFilter)
	ON_CBN_SELCHANGE(IDC_ALLOCTOFILTERCOMBO, OnSelchangeFilter)
	ON_CBN_SELCHANGE(IDC_VERSIONFILTERCOMBO, OnSelchangeFilter)
	ON_CBN_SELCHANGE(IDC_ALLOCBYFILTERCOMBO, OnSelchangeFilter)
	ON_CBN_SELCHANGE(IDC_STATUSFILTERCOMBO, OnSelchangeFilter)
	ON_CBN_SELCHANGE(IDC_PRIORITYFILTERCOMBO, OnSelchangeFilter)
	ON_CBN_SELCHANGE(IDC_CATEGORYFILTERCOMBO, OnSelchangeFilter)
	ON_CBN_SELCHANGE(IDC_RISKFILTERCOMBO, OnSelchangeFilter)
	ON_CBN_CLOSEUP(IDC_OPTIONFILTERCOMBO, OnSelchangeFilter)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipNotify)
	ON_WM_ERASEBKGND()
	ON_REGISTERED_MESSAGE(WM_EE_BTNCLICK, OnEEBtnClick)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterBar message handlers

BOOL CTDLFilterBar::Create(CWnd* pParentWnd, UINT nID, BOOL bVisible)
{
	if (CDialog::Create(IDD_FILTER_BAR, pParentWnd))
	{
		SetDlgCtrlID(nID);
		ShowWindow(bVisible ? SW_SHOW : SW_HIDE);

		GetDlgItem(IDC_TITLEFILTERTEXT)->ModifyStyle(0, ES_WANTRETURN, 0);

		return TRUE;
	}

	return FALSE;
}

void CTDLFilterBar::OnSelchangeFilter()
{
	FTDCFILTER fPrev(m_filter);
	UpdateData(); // updates m_filter

	GetParent()->SendMessage(WM_FBN_FILTERCHNG, 0, (LPARAM)&m_filter);

	// if the main 'Show" filter has changed then we need to re-init the options
	if (m_filter.nFilter != fPrev.nFilter)
	{
		m_cbOptions.Initialize(m_filter, m_nView);
	}
}

LRESULT CTDLFilterBar::OnEEBtnClick(WPARAM /*wp*/, LPARAM /*lp*/)
{
	OnSelchangeFilter();
	return 0L;
}

BOOL CTDLFilterBar::PreTranslateMessage(MSG* pMsg)
{
	// handle return key in title field
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == m_eTitleFilter &&
			pMsg->wParam == VK_RETURN)
	{
		OnSelchangeFilter();
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CTDLFilterBar::SetCustomFilter(BOOL bCustom, LPCTSTR szFilter)
{
	if (bCustom)
	{
		m_cbTaskFilter.SetCustomFilter(szFilter);
	}

	m_bCustomFilter = bCustom;
	m_sCustomFilter = szFilter;

	// disable controls if a custom filter.
	// just do a repos because this also handles enabled state
	ReposControls();
}

void CTDLFilterBar::RemoveCustomFilter()
{
	m_cbTaskFilter.RemoveCustomFilter();
}

void CTDLFilterBar::RefreshFilterControls(const CFilteredToDoCtrl& tdc)
{
	// column visibility
	CTDCColumnArray aColumns;
	tdc.GetVisibleColumns(aColumns);

	SetVisibleFilters(aColumns);

	// get filter
	tdc.GetFilter(m_filter);
	m_nView = tdc.GetView();

	// get custom filter
	m_bCustomFilter = tdc.HasCustomFilter();
	m_sCustomFilter = tdc.GetCustomFilterName();

	// if this todoctrl does not have a custom filter
	// then remove the option from the combo
	if (!tdc.CustomFilterHasRules())
	{
		m_cbTaskFilter.RemoveCustomFilter();
	}
	else
	{
		m_cbTaskFilter.SetCustomFilter(tdc.GetCustomFilterName());
	}

	// alloc to filter
	CStringArray aItems;

	tdc.GetAllocToNames(aItems);
	aItems.Add(_T("")); // add blank item to top
	SetComboBoxItems(m_cbAllocToFilter, aItems);

	// alloc by filter
	tdc.GetAllocByNames(aItems);
	aItems.Add(_T("")); // add blank item to top
	SetComboBoxItems(m_cbAllocByFilter, aItems);

	// category filter
	tdc.GetCategoryNames(aItems);
	aItems.Add(_T("")); // add blank item to top
	SetComboBoxItems(m_cbCategoryFilter, aItems);

	// status filter
	tdc.GetStatusNames(aItems);
	aItems.Add(_T("")); // add blank item to top
	SetComboBoxItems(m_cbStatusFilter, aItems);

	// version filter
	tdc.GetVersionNames(aItems);
	aItems.Add(_T("")); // add blank item to top
	SetComboBoxItems(m_cbVersionFilter, aItems);

	// priority
	m_cbPriorityFilter.SetColors(m_aPriorityColors);
	m_cbPriorityFilter.InsertColor(0, (COLORREF) - 1, CEnString(IDS_TDC_ANY)); // add a blank item

	// risk never needs changing

	// update UI
	UpdateData(FALSE);
	Invalidate();

	// disable controls if a custom filter.
	// just do a repos because this also handles enabled state
	ReposControls();
}

void CTDLFilterBar::SetFilterLabelAlignment(BOOL bLeft)
{
	UINT nAlign = bLeft ? SS_LEFT : SS_RIGHT;

	int nLabel = NUMFILTERCTRLS;

	while (nLabel--)
	{
		UINT nLabelID = FILTERCTRLS[nLabel].nIDLabel;

		if (nLabelID)
		{
			CWnd* pLabel = GetDlgItem(nLabelID);
			pLabel->ModifyStyle(SS_TYPEMASK, nAlign);
			pLabel->Invalidate();
		}
	}
}

void CTDLFilterBar::SetPriorityColors(const CDWordArray& aColors)
{
	m_aPriorityColors.Copy(aColors);

	if (m_cbPriorityFilter.GetSafeHwnd())
	{
		// save and restore current selection
		int nSel = m_cbPriorityFilter.GetCurSel();

		m_cbPriorityFilter.SetColors(aColors);
		m_cbPriorityFilter.InsertColor(0, (COLORREF) - 1, CEnString(IDS_TDC_ANY)); // add a blank item

		m_cbPriorityFilter.SetCurSel(nSel);
	}
}

void CTDLFilterBar::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// check we're not in the middle of creation
	if (m_cbStatusFilter.GetSafeHwnd())
	{
		ReposControls(cx, FALSE);
	}
}

int CTDLFilterBar::CalcHeight(int nWidth)
{
	return ReposControls(nWidth, TRUE);
}

void CTDLFilterBar::SetVisibleFilters(const CTDCColumnArray& aFilters)
{
	// clear first
	int nFilter = m_aVisibility.GetSize();

	while (nFilter--)
	{
		m_aVisibility[nFilter] = FALSE;
	}

	// then set the visible ones
	int nItem = aFilters.GetSize();

	while (nItem--)
	{
		nFilter = aFilters[nItem];
		m_aVisibility[nFilter] = TRUE;
	}

	ReposControls();
}

BOOL CTDLFilterBar::WantShowFilter(TDC_COLUMN nType)
{
	if ((int)nType < 0) // first filter is -1 and is therefore always visible
	{
		return TRUE;
	}

	return (BOOL)m_aVisibility[(DWORD)nType];
}

void CTDLFilterBar::EnableMultiSelection(BOOL bEnable)
{
	m_cbCategoryFilter.EnableMultiSelection(bEnable);
	m_cbAllocToFilter.EnableMultiSelection(bEnable);
	m_cbStatusFilter.EnableMultiSelection(bEnable);
	m_cbAllocByFilter.EnableMultiSelection(bEnable);
	m_cbVersionFilter.EnableMultiSelection(bEnable);
}

int CTDLFilterBar::ReposControls(int nWidth, BOOL bCalcOnly)
{
	CDeferWndMove dwm(bCalcOnly ? 0 : NUMFILTERCTRLS + 1);

	if (nWidth <= 0)
	{
		CRect rClient;
		GetClientRect(rClient);

		nWidth = rClient.Width();
	}

	// Note: All calculations are performed in DLU until just before the move
	// is performed. This ensures that we minimize the risk of rounding errors.
	CDlgUnits dlu(*this);

	int nCtrlHeight = dwm.OffsetCtrl(this, IDC_FILTERCOMBO).Height();
	int nXPosDLU = 0, nYPosDLU = 2, nVisible = 0;

	int nWidthDLU = nWidth, nCtrlHeightDLU = nCtrlHeight;
	dlu.FromPixels(nWidthDLU, nCtrlHeightDLU);

	// indent controls after the first line so that they
	// line up neatly with the first line. Unless we're only
	// at the second control in which case there is nothing
	// to line up with
	const FILTERCTRL& fc0 = FILTERCTRLS[0];
	const FILTERCTRL& fc1 = FILTERCTRLS[1];
	int nLineIndentDLU = fc0.nLenLabelDLU + fc0.nLenCtrlDLU -
		fc1.nLenLabelDLU - fc1.nLenCtrlDLU;

	for (int nCtrl = 0; nCtrl < NUMFILTERCTRLS; nCtrl++)
	{
		CRect rCtrl, rCtrlDLU;
		const FILTERCTRL& fc = FILTERCTRLS[nCtrl];

		// display this control only if the corresponding column
		// is also showing
		BOOL bWantCtrl = WantShowFilter(fc.nType);

		if (bWantCtrl)
		{
			// if we're at the start of the line then just move ctrls
			// else we must check whether there's enough space to fit
			if (nXPosDLU > 0 || (nYPosDLU > 0 && nXPosDLU > nLineIndentDLU)) // we're not the first
			{
				// work out the total length of label + ctrl
				int nLenDLU = fc.nLenLabelDLU + FILTERCTRLXSPACING + fc.nLenCtrlDLU;

				// do we fit?
				if ((nLenDLU + nXPosDLU) > nWidthDLU) // no
				{
					// move to next line
					nXPosDLU = nLineIndentDLU;
					nYPosDLU += FILTERCTRLYSPACING + nCtrlHeightDLU + 2;
				}
			}

			// move label
			rCtrlDLU.left = nXPosDLU;
			rCtrlDLU.right = nXPosDLU + fc.nLenLabelDLU;
			rCtrlDLU.top = nYPosDLU;
			rCtrlDLU.bottom = nYPosDLU + nCtrlHeightDLU;

			rCtrl = rCtrlDLU;
			dlu.ToPixels(rCtrl);

			if (fc.nIDLabel && !bCalcOnly)
			{
				dwm.MoveWindow(GetDlgItem(fc.nIDLabel), rCtrl);
			}

			// update XPos for the label
			nXPosDLU = rCtrlDLU.right;

			// add spacing
			nXPosDLU += FILTERCTRLXSPACING;

			// move ctrl
			rCtrlDLU.left = nXPosDLU;
			rCtrlDLU.right = nXPosDLU + fc.nLenCtrlDLU;
			rCtrlDLU.top = nYPosDLU;
			rCtrlDLU.bottom = nYPosDLU + nCtrlHeightDLU;

			rCtrl = rCtrlDLU;
			dlu.ToPixels(rCtrl);

			if (!bCalcOnly)
			{
				// make sure we use the original height of the control
				// otherwise it makes a mess of combo boxes
				rCtrl.bottom = rCtrl.top + dwm.OffsetCtrl(this, fc.nIDCtrl).Height();

				dwm.MoveWindow(GetDlgItem(fc.nIDCtrl), rCtrl);
			}

			// update XPos for the control
			nXPosDLU = rCtrlDLU.right + FILTERCTRLXSPACING;

			nVisible++;
		}

		//show/hide and enable as appropriate
		if (!bCalcOnly)
		{
			if (fc.nIDLabel)
			{
				GetDlgItem(fc.nIDLabel)->ShowWindow(bWantCtrl ? SW_SHOW : SW_HIDE);

				// note the first ctrl is always enabled even for custom filter
				GetDlgItem(fc.nIDLabel)->EnableWindow(bWantCtrl && (nCtrl == 0 || !m_bCustomFilter));
			}

			GetDlgItem(fc.nIDCtrl)->ShowWindow(bWantCtrl ? SW_SHOW : SW_HIDE);
			GetDlgItem(fc.nIDCtrl)->EnableWindow(bWantCtrl && (nCtrl == 0 || !m_bCustomFilter));
		}
	}

	// update bottom of filter bar
	nYPosDLU += nCtrlHeightDLU;

	return dlu.ToPixelsY(nYPosDLU) + 2;
}

void CTDLFilterBar::SetFilter(const FTDCFILTER& filter, FTC_VIEW nView)
{
	m_filter = filter;
	m_nView = nView;
	m_bCustomFilter = FALSE;

	SetCustomFilter(FALSE);

	UpdateData(FALSE);
}

BOOL CTDLFilterBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	// one-time init for risk filter combo
	m_cbRiskFilter.InsertString(0, CEnString(IDS_TDC_ANY)); // add a blank item

	EnableToolTips();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTDLFilterBar::OnToolTipNotify(UINT /*id*/, NMHDR* pNMHDR, LRESULT* /*pResult*/)
{
	// Get the tooltip structure.
	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;

	// Actually the idFrom holds Control's handle.
	UINT CtrlHandle = pNMHDR->idFrom;

	// Check once again that the idFrom holds handle itself.
	if (pTTT->uFlags & TTF_IDISHWND)
	{
		static CString sTooltip;
		sTooltip.Empty();

		// Get the control's ID.
		UINT nID = ::GetDlgCtrlID(HWND(CtrlHandle));

		switch (nID)
		{
		case IDC_CATEGORYFILTERCOMBO:
			sTooltip = m_cbCategoryFilter.GetTooltip();
			break;

		case IDC_ALLOCTOFILTERCOMBO:
			sTooltip = m_cbAllocToFilter.GetTooltip();
			break;

		case IDC_STATUSFILTERCOMBO:
			sTooltip = m_cbStatusFilter.GetTooltip();
			break;

		case IDC_ALLOCBYFILTERCOMBO:
			sTooltip = m_cbAllocByFilter.GetTooltip();
			break;

		case IDC_VERSIONFILTERCOMBO:
			sTooltip = m_cbVersionFilter.GetTooltip();
			break;

		case IDC_OPTIONFILTERCOMBO:
			sTooltip = m_cbOptions.GetTooltip();
			break;
		}

		if (!sTooltip.IsEmpty())
		{
			// Set the tooltip text.
			::SendMessage(pNMHDR->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 300);
			pTTT->lpszText = const_cast<LPTSTR>((LPCTSTR)sTooltip);
			return TRUE;
		}
	}

	// Not handled.
	return FALSE;
}

void CTDLFilterBar::SetUIColors(COLORREF crBack, COLORREF crText)
{
	m_crUIText = crText;

	if (crBack != m_crUIBack)
	{
		m_crUIBack = crBack;
		m_brUIBack.DeleteObject();

		if (crBack != (COLORREF) - 1)
		{
			m_brUIBack.CreateSolidBrush(m_crUIBack);
		}
	}

	Invalidate();
}

BOOL CTDLFilterBar::OnEraseBkgnd(CDC* pDC)
{
	int nDC = pDC->SaveDC();

	// clip out all the child controls to reduce flicker
	if (!(GetStyle() & WS_CLIPCHILDREN))
	{
		ExcludeControls(this, pDC, IDC_FILTERLABEL, IDC_OPTIONFILTERCOMBO);
	}

	if (m_brUIBack.GetSafeHandle())
	{
		CRect rect;
		GetClientRect(rect);
		pDC->FillSolidRect(rect, m_crUIBack);
	}
	else
	{
		CDialog::OnEraseBkgnd(pDC);
	}

	pDC->RestoreDC(nDC);

	return TRUE;
}

HBRUSH CTDLFilterBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC && m_brUIBack.GetSafeHandle())
	{
		pDC->SetTextColor(m_crUIText);
		pDC->SetBkMode(TRANSPARENT);
		hbr = m_brUIBack;
	}

	return hbr;
}
