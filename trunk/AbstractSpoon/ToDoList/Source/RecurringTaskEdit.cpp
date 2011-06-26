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

// RecurringTaskEdit.cpp: implementation of the CRecurringTaskEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RecurringTaskEdit.h"
#include "Resource.h"

#include "../../../CodeProject/Source/DateHelper.h"
#include "../../../CodeProject/Source/DialogHelper.h"
#include "../../../CodeProject/Source/AutoFlag.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTDLRecurringTaskEdit::CTDLRecurringTaskEdit()
{
	AddButton(REBTN_OPTIONS, _T("..."), _T("Options"));
}

CTDLRecurringTaskEdit::~CTDLRecurringTaskEdit()
{
}

BEGIN_MESSAGE_MAP(CTDLRecurringTaskEdit, CEnEdit)
	//{{AFX_MSG_MAP(CRecurringTaskEdit)
	//}}AFX_MSG_MAP
	ON_WM_SETCURSOR()
	ON_WM_CTLCOLOR_REFLECT()
	ON_MESSAGE(EM_SETREADONLY, OnSetReadOnly)
	ON_WM_STYLECHANGING()
	ON_CONTROL_REFLECT_EX(EN_CHANGE, OnReflectChangeDisplayText)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////

void CTDLRecurringTaskEdit::PreSubclassWindow()
{
	CEnEdit::PreSubclassWindow();

	SetWindowText(GetRegularity(m_tr)); // for display purposes
}

BOOL CTDLRecurringTaskEdit::OnReflectChangeDisplayText()
{
	GetWindowText(m_tr.sRegularity);

	if (m_tr.nRegularity == TDIR_ONCE && IsDefaultString(m_tr.sRegularity))
	{
		m_tr.sRegularity.Empty();
	}

	return FALSE; // allow parent to process too
}

void CTDLRecurringTaskEdit::OnBtnClick(UINT nID)
{
	switch (nID)
	{
	case REBTN_OPTIONS:
		DoEdit();
		break;
	}
}

void CTDLRecurringTaskEdit::DoEdit()
{
	if (!m_bReadOnly)
	{
		CTDLRecurringTaskOptionDlg dialog(m_tr, m_dtDefault);

		if (dialog.DoModal() == IDOK)
		{
			TDIRECURRENCE tr;
			dialog.GetRecurrenceOptions(tr);

			if (m_tr != tr)
			{
				// reset display string if the units have changed
				if (m_tr.nRegularity != tr.nRegularity)
				{
					m_tr.sRegularity.Empty();
				}
				else
					// restore current text
				{
					tr.sRegularity = m_tr.sRegularity;
				}

				m_tr = tr;
				SetWindowText(GetRegularity(m_tr)); // for display purposes

				// notify parent
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(EN_CHANGE, GetDlgCtrlID()), (LPARAM)GetSafeHwnd());
			}
		}
	}
}

BOOL CTDLRecurringTaskEdit::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	return TRUE;//CEnEdit::OnSetCursor(pWnd, nHitTest, message);
}

HBRUSH CTDLRecurringTaskEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	HBRUSH hbr = NULL;
	pDC->SetBkMode(TRANSPARENT);

	if (!IsWindowEnabled() || m_bReadOnly)
	{
		hbr = GetSysColorBrush(COLOR_3DFACE);
	}
	else
	{
		hbr = GetSysColorBrush(COLOR_WINDOW);
	}

	return hbr;
}

void CTDLRecurringTaskEdit::GetRecurrenceOptions(TDIRECURRENCE& tr) const
{
	tr = m_tr;

	if (tr.sRegularity.IsEmpty())
	{
		tr.sRegularity = GetRegularity(tr.nRegularity, FALSE);
	}
}

void CTDLRecurringTaskEdit::SetRecurrenceOptions(const TDIRECURRENCE& tr)
{
	m_tr = tr;

	if (GetSafeHwnd())
	{
		SetWindowText(GetRegularity(m_tr));   // for display purposes
	}
}

BOOL CTDLRecurringTaskEdit::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT /*nFlags*/)
{
	if ((dwRemove & ES_READONLY) != (dwAdd & ES_READONLY))
	{
		m_bReadOnly = (dwAdd & ES_READONLY);

		EnableButton(1, !m_bReadOnly);
		Invalidate();
	}

	// make sure we stay readonly
	return CEnEdit::ModifyStyle(dwRemove & ~ES_READONLY, dwAdd | ES_READONLY);
}

LRESULT CTDLRecurringTaskEdit::OnSetReadOnly(WPARAM wp, LPARAM /*lp*/)
{
	m_bReadOnly = wp;
	EnableButton(REBTN_OPTIONS, !m_bReadOnly);

	// always set to readonly
	// we set this flag so that OnStyleChanging doesn't respond to this trickery
	CAutoFlag af(m_bInOnSetReadOnly, TRUE);

	return DefWindowProc(EM_SETREADONLY, TRUE, 0);
}

void CTDLRecurringTaskEdit::OnStyleChanging(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	CEnEdit::OnStyleChanging(nStyleType, lpStyleStruct);

	if (nStyleType == GWL_STYLE && !m_bInOnSetReadOnly)
	{
		// check for change in readonly style
		if ((lpStyleStruct->styleOld & ES_READONLY) != (lpStyleStruct->styleNew & ES_READONLY))
		{
			m_bReadOnly = (lpStyleStruct->styleNew & ES_READONLY);
			lpStyleStruct->styleNew |= ES_READONLY; // make sure we stay readonly

			EnableButton(REBTN_OPTIONS, !m_bReadOnly);
			Invalidate();
		}
	}
}

CString CTDLRecurringTaskEdit::GetRegularity(const TDIRECURRENCE& tr, BOOL bIncOnce)
{
	if (tr.sRegularity.IsEmpty())
	{
		return GetRegularity(tr.nRegularity, bIncOnce);
	}

	// else
	return tr.sRegularity;
}

CString CTDLRecurringTaskEdit::GetRegularity(TDI_REGULARITY nRegularity, BOOL bIncOnce)
{
	CString sRegularity;

	switch (nRegularity)
	{
	case TDIR_ONCE:
		if (bIncOnce)
		{
			sRegularity.LoadString(IDS_ONCEONLY);
		}
		break;

	case TDIR_DAILY:
		sRegularity.LoadString(IDS_DAILY);
		break;
	case TDIR_WEEKLY:
		sRegularity.LoadString(IDS_WEEKLY);
		break;
	case TDIR_MONTHLY:
		sRegularity.LoadString(IDS_MONTHLY);
		break;
	case TDIR_YEARLY:
		sRegularity.LoadString(IDS_YEARLY);
		break;
	}

	return sRegularity;
}

BOOL CTDLRecurringTaskEdit::IsDefaultString(const CString& sRegularity)
{
	int nReg = (int)TDIR_YEARLY + 1;

	while (nReg--)
	{
		if (sRegularity.CompareNoCase(GetRegularity((TDI_REGULARITY)nReg, TRUE)) == 0)
		{
			return TRUE;
		}
	}

	// else
	return FALSE;
}

int CTDLRecurringTaskEdit::CalcMaxRegularityWidth(CDC* pDC, BOOL bIncOnce)
{
	int nReg = (int)TDIR_YEARLY + 1;
	int nMax = 0;

	while (nReg--)
	{
		CString sRegularity = GetRegularity((TDI_REGULARITY)nReg, bIncOnce);
		int nItem = pDC->GetTextExtent(sRegularity).cx;
		nMax = max(nItem, nMax);
	}

	return nMax;
}

/////////////////////////////////////////////////////////////////////////////
// CRecurringTaskOptionDlg dialog

#define WM_VALUECHANGE (WM_APP+1)

CTDLRecurringTaskOptionDlg::CTDLRecurringTaskOptionDlg(const TDIRECURRENCE& tr, const COleDateTime& dtDefault,
	CWnd* pParent /*=NULL*/):
CDialog(IDD_RECURRING_TASK_DIALOG, pParent)
{
	//{{AFX_DATA_INIT(CRecurringTaskOptionDlg)
	m_nRegularity = tr.nRegularity;
	m_bRecalcFromDue = tr.bRecalcFromDue;
	m_nReuse = tr.nReuse;
	//}}AFX_DATA_INIT
	m_nNumDays = 1;
	m_nNumWeeks = 1;
	m_dwWeekdays = 0;
	m_nNumMonths = 1;
	m_nMonthDay = 1;
	m_nMonth = 0;
	m_nYearMonthDay = 1;

	// first set to default values
	if (dtDefault.m_dt > 0.0)
	{
		SYSTEMTIME stDefault;
		dtDefault.GetAsSystemTime(stDefault);

		m_dwWeekdays = 2 << (stDefault.wDayOfWeek - 1);
		m_nMonthDay = stDefault.wDay;
		m_nMonth = stDefault.wMonth - 1;
		m_nYearMonthDay = stDefault.wDay;
	}

	// then overwrite specific values
	switch (tr.nRegularity)
	{
	case TDIR_DAILY:
		m_nNumDays = tr.dwSpecific1;
		break;

	case TDIR_WEEKLY:
		m_nNumWeeks = tr.dwSpecific1;
		m_dwWeekdays = tr.dwSpecific2;
		break;

	case TDIR_MONTHLY:
		m_nNumMonths = tr.dwSpecific1;
		m_nMonthDay = tr.dwSpecific2;
		break;

	case TDIR_YEARLY:
		m_nMonth = tr.dwSpecific1 - 1;
		m_nYearMonthDay = tr.dwSpecific2;
		break;
	}
}


void CTDLRecurringTaskOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecurringTaskOptionDlg)
	DDX_Radio(pDX, IDC_ONCE, m_nRegularity);
	DDX_CBIndex(pDX, IDC_RECALCFROM, m_bRecalcFromDue);
	DDX_CBIndex(pDX, IDC_REUSEOPTIONS, m_nReuse);
	//}}AFX_DATA_MAP
	CDialogHelper::DDX_Text(pDX, IDC_DAYS, m_nNumDays);
	DDX_Control(pDX, IDC_WEEKDAYS, m_lbWeekdays);

	CDialogHelper::DDX_Text(pDX, IDC_WEEKS, m_nNumWeeks);

	if (pDX->m_bSaveAndValidate)
	{
		m_dwWeekdays = 0;

		if (m_lbWeekdays.GetCheck(0))
		{
			m_dwWeekdays |= TDIW_SUNDAY;
		}

		if (m_lbWeekdays.GetCheck(1))
		{
			m_dwWeekdays |= TDIW_MONDAY;
		}

		if (m_lbWeekdays.GetCheck(2))
		{
			m_dwWeekdays |= TDIW_TUESDAY;
		}

		if (m_lbWeekdays.GetCheck(3))
		{
			m_dwWeekdays |= TDIW_WEDNESDAY;
		}

		if (m_lbWeekdays.GetCheck(4))
		{
			m_dwWeekdays |= TDIW_THURSDAY;
		}

		if (m_lbWeekdays.GetCheck(5))
		{
			m_dwWeekdays |= TDIW_FRIDAY;
		}

		if (m_lbWeekdays.GetCheck(6))
		{
			m_dwWeekdays |= TDIW_SATURDAY;
		}
	}
	else
	{
		m_lbWeekdays.SetCheck(0, (m_dwWeekdays & TDIW_SUNDAY) ? 1 : 0);
		m_lbWeekdays.SetCheck(1, (m_dwWeekdays & TDIW_MONDAY) ? 1 : 0);
		m_lbWeekdays.SetCheck(2, (m_dwWeekdays & TDIW_TUESDAY) ? 1 : 0);
		m_lbWeekdays.SetCheck(3, (m_dwWeekdays & TDIW_WEDNESDAY) ? 1 : 0);
		m_lbWeekdays.SetCheck(4, (m_dwWeekdays & TDIW_THURSDAY) ? 1 : 0);
		m_lbWeekdays.SetCheck(5, (m_dwWeekdays & TDIW_FRIDAY) ? 1 : 0);
		m_lbWeekdays.SetCheck(6, (m_dwWeekdays & TDIW_SATURDAY) ? 1 : 0);
	}
	CDialogHelper::DDX_Text(pDX, IDC_MONTHS, m_nNumMonths);
	CDialogHelper::DDX_Text(pDX, IDC_MONTHDAY, m_nMonthDay);

	DDX_Control(pDX, IDC_MONTHLIST, m_cbMonths);
	DDX_CBIndex(pDX, IDC_MONTHLIST, m_nMonth);
	DDX_Text(pDX, IDC_YEARMONTHDAY, m_nYearMonthDay);
}

BEGIN_MESSAGE_MAP(CTDLRecurringTaskOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CRecurringTaskOptionDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ONCE, OnSelchangeRegularity)
	ON_BN_CLICKED(IDC_DAILY, OnSelchangeRegularity)
	ON_BN_CLICKED(IDC_WEEKLY, OnSelchangeRegularity)
	ON_BN_CLICKED(IDC_MONTHLY, OnSelchangeRegularity)
	ON_BN_CLICKED(IDC_YEARLY, OnSelchangeRegularity)
	ON_EN_CHANGE(IDC_DAYS, OnChangeDailyValues)
	ON_EN_CHANGE(IDC_WEEKS, OnChangeWeeklyValues)
	ON_CLBN_CHKCHANGE(IDC_WEEKDAYS, OnChangeWeeklyValues)
	ON_EN_CHANGE(IDC_MONTHDAY, OnChangeMonthlyValues)
	ON_EN_CHANGE(IDC_MONTHS, OnChangeMonthlyValues)
	ON_CBN_SELCHANGE(IDC_MONTHLIST, OnChangeYearlyValues)
	ON_EN_CHANGE(IDC_YEARMONTHDAY, OnChangeYearlyValues)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecurringTaskOptionDlg message handlers
/////////////////////////////////////////////////////////////////////////////

void CTDLRecurringTaskOptionDlg::GetRecurrenceOptions(TDIRECURRENCE& tr) const
{
	tr.nRegularity = (TDI_REGULARITY)m_nRegularity;
	tr.bRecalcFromDue = m_bRecalcFromDue;
	tr.nReuse = (TDI_RECURREUSEOPTION)m_nReuse;

	switch (tr.nRegularity)
	{
	case TDIR_DAILY:
		tr.dwSpecific1 = m_nNumDays;
		break;

	case TDIR_WEEKLY:
		tr.dwSpecific1 = m_nNumWeeks;
		tr.dwSpecific2 = m_dwWeekdays;
		break;

	case TDIR_MONTHLY:
		tr.dwSpecific1 = m_nNumMonths;
		tr.dwSpecific2 = m_nMonthDay;
		break;

	case TDIR_YEARLY:
		tr.dwSpecific1 = m_nMonth + 1;
		tr.dwSpecific2 = m_nYearMonthDay;
		break;
	}
}

void CTDLRecurringTaskOptionDlg::OnSelchangeRegularity()
{
	UpdateData();

	GetDlgItem(IDC_REUSEOPTIONS)->EnableWindow(m_nRegularity != TDIR_ONCE);
	GetDlgItem(IDC_RECALCFROM)->EnableWindow(m_nRegularity != TDIR_ONCE);
	GetDlgItem(IDC_DAYS)->EnableWindow(m_nRegularity == TDIR_DAILY);
	GetDlgItem(IDC_WEEKDAYS)->EnableWindow(m_nRegularity == TDIR_WEEKLY);
	GetDlgItem(IDC_WEEKS)->EnableWindow(m_nRegularity == TDIR_WEEKLY);
	GetDlgItem(IDC_MONTHDAY)->EnableWindow(m_nRegularity == TDIR_MONTHLY);
	GetDlgItem(IDC_MONTHS)->EnableWindow(m_nRegularity == TDIR_MONTHLY);
	GetDlgItem(IDC_MONTHLIST)->EnableWindow(m_nRegularity == TDIR_YEARLY);
	GetDlgItem(IDC_YEARMONTHDAY)->EnableWindow(m_nRegularity == TDIR_YEARLY);
}

BOOL CTDLRecurringTaskOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// init weekdays
	for (int nDay = 1; nDay <= 7; nDay++)
	{
		m_lbWeekdays.AddString(CDateHelper::GetWeekday(nDay, FALSE));
	}

	// restore check states
	UpdateData(FALSE);

	// set the listbox column width to be half the width
	CRect rListbox;
	m_lbWeekdays.GetClientRect(rListbox);
	m_lbWeekdays.SetColumnWidth(rListbox.Width() / 2);

	OnSelchangeRegularity();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTDLRecurringTaskOptionDlg::OnChangeDailyValues()
{
	UpdateData();

	GetDlgItem(IDOK)->EnableWindow(m_nNumDays > 0);
}

void CTDLRecurringTaskOptionDlg::OnChangeWeeklyValues()
{
	UpdateData();

	GetDlgItem(IDOK)->EnableWindow(m_nNumWeeks > 0);
}

void CTDLRecurringTaskOptionDlg::OnChangeMonthlyValues()
{
	UpdateData();

	GetDlgItem(IDOK)->EnableWindow(m_nNumMonths && (m_nMonthDay > 0 && m_nMonthDay <= 31));
}

void CTDLRecurringTaskOptionDlg::OnChangeYearlyValues()
{
	UpdateData();

	GetDlgItem(IDOK)->EnableWindow(m_nMonth >= 0 && m_nMonth < 12) && (m_nYearMonthDay > 0 && m_nYearMonthDay <= 31);
}

/////////////////////////////////////////////////////////////////////////////
