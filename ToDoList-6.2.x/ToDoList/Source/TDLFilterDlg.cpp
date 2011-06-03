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

// TDLFilterDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLFilterDlg.h"
#include "TDCStatic.h"

#include "../../../CodeProject/Source/EnString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg dialog

CTDLFilterDlg::CTDLFilterDlg(BOOL bMultiSelFilters, CWnd* pParent /*=NULL*/):
CDialog(CTDLFilterDlg::IDD, pParent),
m_cbCategoryFilter(bMultiSelFilters, IDS_NOCATEGORY, IDS_TDC_ANY),
m_cbAllocToFilter(bMultiSelFilters, IDS_NOALLOCTO, IDS_TDC_ANYONE),
m_cbAllocByFilter(bMultiSelFilters, 0, IDS_TDC_ANYONE),
m_cbStatusFilter(bMultiSelFilters, 0, IDS_TDC_ANY),
m_cbVersionFilter(bMultiSelFilters, 0, IDS_TDC_ANY),
m_nView(FTCV_UNSET)
{
	//{{AFX_DATA_INIT(CFilterDlg)
	//}}AFX_DATA_INIT
}

void CTDLFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterDlg)
	DDX_Control(pDX, IDC_FILTEROPTIONS, m_cbOptions);
	DDX_Control(pDX, IDC_CATEGORYFILTERCOMBO, m_cbCategoryFilter);
	DDX_Control(pDX, IDC_STATUSFILTERCOMBO, m_cbStatusFilter);
	DDX_Control(pDX, IDC_PRIORITYFILTERCOMBO, m_cbPriorityFilter);
	DDX_Control(pDX, IDC_RISKFILTERCOMBO, m_cbRiskFilter);
	DDX_Control(pDX, IDC_ALLOCTOFILTERCOMBO, m_cbAllocToFilter);
	DDX_Control(pDX, IDC_ALLOCBYFILTERCOMBO, m_cbAllocByFilter);
	DDX_Control(pDX, IDC_VERSIONFILTERCOMBO, m_cbVersionFilter);
	DDX_Control(pDX, IDC_FILTERCOMBO, m_cbTaskFilter);
	//}}AFX_DATA_MAP
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

		// alloc to
		m_cbAllocToFilter.GetChecked(m_filter.aAllocTo);

		// status
		m_cbStatusFilter.GetChecked(m_filter.aStatus);

		// allocby
		m_cbAllocByFilter.GetChecked(m_filter.aAllocBy);

		// version
		m_cbVersionFilter.GetChecked(m_filter.aVersions);

		// build the filter options from the listbox
		m_filter.dwFlags = m_cbOptions.GetOptions();
	}
	else
	{
		// filter
		m_cbTaskFilter.SetSelectedFilter(m_filter.nFilter);

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

		// alloc to
		m_cbAllocToFilter.SetChecked(m_filter.aAllocTo);

		// status
		m_cbStatusFilter.SetChecked(m_filter.aStatus);

		// allocby
		m_cbAllocByFilter.SetChecked(m_filter.aAllocBy);

		// version
		m_cbVersionFilter.SetChecked(m_filter.aVersions);

		// options states set in OnInitDialog
		m_cbOptions.Initialize(m_filter, m_nView);
	}
}

BEGIN_MESSAGE_MAP(CTDLFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CFilterDlg)
	ON_BN_CLICKED(IDC_CLEARFILTER, OnClearfilter)
	ON_CBN_SELCHANGE(IDC_FILTERCOMBO, OnSelchangeFiltercombo)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg message handlers

int CTDLFilterDlg::DoModal(const CFilteredToDoCtrl& tdc, const CDWordArray& aPriorityColors)
{
	// main filter
	tdc.GetFilter(m_filter);

	// alloc filters
	tdc.GetAllocToNames(m_aAllocTo);
	tdc.GetAllocByNames(m_aAllocBy);
	m_aAllocTo.Add(_T("")); // add blank item to top
	m_aAllocBy.Add(_T("")); // add blank item to top

	// category filter
	tdc.GetCategoryNames(m_aCategory);
	m_aCategory.Add(_T("")); // add blank item to top

	// status filter
	tdc.GetStatusNames(m_aStatus);
	m_aStatus.Add(_T("")); // add blank item to top

	// version filter
	tdc.GetVersionNames(m_aVersion);
	m_aVersion.Add(_T("")); // add blank item to top

	m_aPriorityColors.Copy(aPriorityColors);
	m_nView = tdc.GetView();

	if (tdc.HasCustomFilter())
	{
		m_sCustomFilter = tdc.GetCustomFilterName();
	}

	return CDialog::DoModal();
}

void CTDLFilterDlg::GetFilter(FTDCFILTER& filter)
{
	filter = m_filter;
}

BOOL CTDLFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// alloc filters
	SetComboBoxItems(m_cbAllocToFilter, m_aAllocTo);
	SetComboBoxItems(m_cbAllocByFilter, m_aAllocBy);
	SetComboBoxItems(m_cbCategoryFilter, m_aCategory);
	SetComboBoxItems(m_cbStatusFilter, m_aStatus);
	SetComboBoxItems(m_cbVersionFilter, m_aVersion);

	// priority
	m_cbPriorityFilter.SetColors(m_aPriorityColors);
	m_cbPriorityFilter.InsertColor(0, (COLORREF) - 1, CEnString(IDS_TDC_ANY)); // add a blank item

	// risk
	m_cbRiskFilter.InsertString(0, CEnString(IDS_TDC_ANY)); // add a blank item

	// update UI
	UpdateData(FALSE);

	// custom filter
	if (!m_sCustomFilter.IsEmpty())
	{
		m_cbTaskFilter.SetCustomFilter(m_sCustomFilter);

		// disable all control except main filter
		GetDlgItem(IDC_TITLEFILTERTEXT)->EnableWindow(FALSE);
		m_cbOptions.EnableWindow(FALSE);
		m_cbCategoryFilter.EnableWindow(FALSE);
		m_cbStatusFilter.EnableWindow(FALSE);
		m_cbPriorityFilter.EnableWindow(FALSE);
		m_cbRiskFilter.EnableWindow(FALSE);
		m_cbAllocToFilter.EnableWindow(FALSE);
		m_cbAllocByFilter.EnableWindow(FALSE);
		m_cbVersionFilter.EnableWindow(FALSE);
	}

	EnableToolTips();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTDLFilterDlg::OnClearfilter()
{
	m_filter.Reset();

	UpdateData(FALSE);

	// the combos don't get set properly if they
	// are empty but were not previously so we do it manually
	m_cbAllocToFilter.SetCurSel(0);
	m_cbAllocByFilter.SetCurSel(0);
	m_cbCategoryFilter.SetCurSel(0);
	m_cbVersionFilter.SetCurSel(0);
	m_cbStatusFilter.SetCurSel(0);
}

void CTDLFilterDlg::OnSelchangeFiltercombo()
{
	UpdateData();

	// reenable control if not a custom filter
	int nSel = m_cbTaskFilter.GetCurSel();
	ASSERT(nSel != -1);

	BOOL bCustom = (m_cbTaskFilter.GetItemData(nSel) == FT_CUSTOM);

	GetDlgItem(IDC_TITLEFILTERTEXT)->EnableWindow(!bCustom);
	m_cbOptions.EnableWindow(!bCustom);
	m_cbCategoryFilter.EnableWindow(!bCustom);
	m_cbStatusFilter.EnableWindow(!bCustom);
	m_cbPriorityFilter.EnableWindow(!bCustom);
	m_cbRiskFilter.EnableWindow(!bCustom);
	m_cbAllocToFilter.EnableWindow(!bCustom);
	m_cbAllocByFilter.EnableWindow(!bCustom);
	m_cbVersionFilter.EnableWindow(!bCustom);

	m_cbOptions.Initialize(m_filter, m_nView);
}

BOOL CTDLFilterDlg::OnToolTipNotify(UINT /*id*/, NMHDR* pNMHDR, LRESULT* /*pResult*/)
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
