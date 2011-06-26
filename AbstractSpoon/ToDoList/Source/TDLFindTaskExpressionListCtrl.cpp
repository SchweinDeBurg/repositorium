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

// TDLFindTaskExpressionListCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "Resource.h"
#include "TDLFindTaskExpressionListCtrl.h"
#include "TDCStatic.h"

#include "../../../CodeProject/Source/HoldRedraw.h"
#include "../../../CodeProject/Source/EnString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum
{
	ATTRIB,
	OPERATOR,
	VALUE,
	ANDOR
};
enum
{
	ATTRIB_ID = 5000,
	OPERATOR_ID,
	ANDOR_ID,
	DATE_ID,
	TIME_ID
};

const float COL_PROPORTIONS[] =
{
	12.0f / 47,
	16.0f / 47,
	13.0f / 47,
	6.0f / 47
};

#define ADD_OP_2_COMBO(cb, op) { int i = cb->AddString(GetOpName(op)); cb->SetItemData(i, (DWORD)op); }

/////////////////////////////////////////////////////////////////////////////
// CTDLFindTaskExpressionListCtrl

CTDLFindTaskExpressionListCtrl::CTDLFindTaskExpressionListCtrl()
{
	SetAutoRowPrompt(CEnString(IDS_FP_NEW_RULE));
}

CTDLFindTaskExpressionListCtrl::~CTDLFindTaskExpressionListCtrl()
{
}

BEGIN_MESSAGE_MAP(CTDLFindTaskExpressionListCtrl, CInputListCtrl)
	//{{AFX_MSG_MAP(CTDLFindTaskExpressionListCtrl)
	ON_WM_KILLFOCUS()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_CHAR()
	ON_WM_MEASUREITEM_REFLECT()
	ON_CBN_CLOSEUP(ATTRIB_ID, OnAttribEditCancel)
	ON_CBN_SELENDCANCEL(ATTRIB_ID, OnAttribEditCancel)
	ON_CBN_SELENDOK(ATTRIB_ID, OnAttribEditOK)
	ON_CBN_SELENDCANCEL(OPERATOR_ID, OnOperatorEditCancel)
	ON_CBN_SELENDOK(OPERATOR_ID, OnOperatorEditOK)
	ON_CBN_SELENDCANCEL(ANDOR_ID, OnAndOrEditCancel)
	ON_CBN_SELENDOK(ANDOR_ID, OnAndOrEditOK)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnValueEditOK)
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, OnSelItemChanged)
	ON_NOTIFY(DTN_DATETIMECHANGE, DATE_ID, OnDateChange)
	ON_NOTIFY(DTN_CLOSEUP, DATE_ID, OnDateCloseUp)
	ON_REGISTERED_MESSAGE(WM_TEN_UNITSCHANGE, OnTimeUnitsChange)
	ON_EN_CHANGE(TIME_ID, OnTimeChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLFindTaskExpressionListCtrl message handlers

void CTDLFindTaskExpressionListCtrl::PreSubclassWindow()
{
	// create child controls
	CreateControl(m_cbAttributes, ATTRIB_ID);
	CreateControl(m_cbOperators, OPERATOR_ID, FALSE);
	CreateControl(m_cbAndOr, ANDOR_ID, FALSE);
	CreateControl(m_dtDate, DATE_ID);
	CreateControl(m_eTime, TIME_ID);

	CInputListCtrl::PreSubclassWindow(); // we need combo to be created first

	// build attrib combo because that is static
	for (int nAttrib = 0; nAttrib < ATTRIB_COUNT; nAttrib++)
	{
		const TDCATTRIBUTE& ap = ATTRIBUTES[nAttrib];

		if (ap.nAttribResID)
		{
			int nItem = m_cbAttributes.AddString(CEnString(ap.nAttribResID));
			m_cbAttributes.SetItemData(nItem, (DWORD)ap.attrib);
		}
	}

	// build and/or combo too
	int nItem = m_cbAndOr.AddString(CEnString(IDS_FP_AND));
	m_cbAndOr.SetItemData(nItem, TRUE);

	nItem = m_cbAndOr.AddString(CEnString(IDS_FP_OR));
	m_cbAndOr.SetItemData(nItem, FALSE);

	// post message for our setup
	m_header.EnableTracking(FALSE);
	ShowGrid(TRUE, TRUE);

	InsertColumn(ATTRIB, CEnString(IDS_FT_ATTRIB), LVCFMT_LEFT, 120);
	InsertColumn(OPERATOR, CEnString(IDS_FT_MATCHES), LVCFMT_LEFT, 160);
	InsertColumn(VALUE, CEnString(IDS_FT_VALUE), LVCFMT_LEFT, 130);
	InsertColumn(ANDOR, CEnString(IDS_FT_ANDOR), LVCFMT_LEFT, 60);
	SetView(LVS_REPORT);

	AutoAdd(TRUE, FALSE);

	BuildListCtrl();

	SetColumnType(ATTRIB, DROPLIST);
	SetColumnType(OPERATOR, DROPLIST);
	SetColumnType(ANDOR, DROPLIST);
}

void CTDLFindTaskExpressionListCtrl::SetSearchParams(const SEARCHPARAM& param)
{
	m_aSearchParams.RemoveAll();
	m_aSearchParams.Add(param);

	if (GetSafeHwnd())
	{
		BuildListCtrl();
	}
}

void CTDLFindTaskExpressionListCtrl::ClearSearch()
{
	m_aSearchParams.RemoveAll();

	if (GetSafeHwnd())
	{
		BuildListCtrl();
	}
}

void CTDLFindTaskExpressionListCtrl::SetSearchParams(const CSearchParamArray& params)
{
	m_aSearchParams.Copy(params);

	if (GetSafeHwnd())
	{
		BuildListCtrl();
	}
}

int CTDLFindTaskExpressionListCtrl::GetSearchParams(CSearchParamArray& params) const
{
	params.Copy(m_aSearchParams);

	return params.GetSize();
}

void CTDLFindTaskExpressionListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CRect rCombo;
	m_cbAttributes.GetWindowRect(rCombo);
	m_nItemHeight = rCombo.Height();

	lpMeasureItemStruct->itemHeight = m_nItemHeight - 1;
}

BOOL CTDLFindTaskExpressionListCtrl::DeleteSelectedCell()
{
	int nRow, nCol;
	GetCurSel(nRow, nCol);

	if (nRow < GetRuleCount())
	{
		if (nCol == ATTRIB)
		{
			CInputListCtrl::DeleteSelectedCell();
			m_aSearchParams.RemoveAt(nRow);

			ValidateListData();
			SetCurSel(nRow);

			return TRUE;
		}
		else if (nCol == VALUE) // clear text
		{
			SetItemText(nRow, nCol, _T(""));
			m_aSearchParams[nRow].sValue = _T("");
		}
	}

	// else
	return FALSE;
}

CWnd* CTDLFindTaskExpressionListCtrl::GetEditControl(int nItem, int nCol)
{
	if (nItem < 0 || nItem > GetRuleCount() || nCol > ANDOR)
	{
		return NULL;
	}

	const SEARCHPARAM& sp = m_aSearchParams[nItem];

	switch (nCol)
	{
	case ATTRIB:
		return &m_cbAttributes;

	case OPERATOR:
		if (sp.attrib != TDCA_NONE)
		{
			return &m_cbOperators;
		}
		break;

	case VALUE:
		if (sp.op == FO_SET || sp.op == FO_NOT_SET)
		{
			// handled by operator
		}
		else
		{
			switch (sp.GetAttribType())
			{
			case FT_DATE:
				return &m_dtDate;

			case FT_TIME:
				return &m_eTime;

			case FT_BOOL:
				// do nothing: it's handled by the operator
				break;

			case FT_NONE:
				// do nothing.
				break;

			default:
				return CInputListCtrl::GetEditControl();
			}
		}
		break;

	case ANDOR:
		return &m_cbAndOr;
	}

	// all else
	return NULL;
}

void CTDLFindTaskExpressionListCtrl::EditCell(int nItem, int nCol)
{
	// handle new rules
	if (nItem == GetRuleCount() && nCol == ATTRIB)
	{
		AddRule();
	}

	CWnd* pEdit = GetEditControl(nItem, nCol);
	ASSERT(pEdit);

	if (!pEdit)
	{
		return;
	}

	const SEARCHPARAM& sp = m_aSearchParams[nItem];

	switch (nCol)
	{
	case ATTRIB:
	case OPERATOR:
	case ANDOR:
		ShowControl(*pEdit, nItem, nCol);
		break;

	case VALUE:
		if (sp.op != FO_SET && sp.op != FO_NOT_SET)
		{
			switch (sp.GetAttribType())
			{
			case FT_DATE:
			case FT_TIME:
				ShowControl(*pEdit, nItem, nCol);
				break;

			default:
				PrepareEdit(nItem, nCol);
				CInputListCtrl::EditCell(nItem, nCol);
			}
		}
		break;
	}
}

BOOL CTDLFindTaskExpressionListCtrl::IsEditing() const
{
	return CInputListCtrl::IsEditing() ||
		m_cbOperators.IsWindowVisible() ||
		m_cbAndOr.IsWindowVisible() ||
		m_cbAttributes.IsWindowVisible() ||
		m_dtDate.IsWindowVisible() ||
		m_eTime.IsWindowVisible();
}

BOOL CTDLFindTaskExpressionListCtrl::CanEditSelectedCell() const
{
	int nRow, nCol;
	GetCurSel(nRow, nCol);

	// Boolean type is a special case
	if (nRow < m_aSearchParams.GetSize())
	{
		const SEARCHPARAM& rule = m_aSearchParams[nRow];

		if (rule.GetAttribType() == FT_BOOL && nCol == VALUE)
		{
			return FALSE;
		}
	}

	// else
	return CInputListCtrl::CanEditSelectedCell();
}

void CTDLFindTaskExpressionListCtrl::PrepareEdit(int nRow, int /*nCol*/)
{
	const SEARCHPARAM& sp = m_aSearchParams[nRow];

	switch (sp.GetAttribType())
	{
	case FT_STRING:
		m_editBox.SetMask(_T(""));
		break;

	case FT_INTEGER:
		m_editBox.SetMask(_T("1234567890"));
		break;

	case FT_DOUBLE:
		m_editBox.SetMask(_T("1234567890."), ME_LOCALIZEDECIMAL);
		break;

	case FT_DATE:
	case FT_BOOL:
	case FT_NONE:
	default:
		ASSERT(0);
	}
}

BOOL CTDLFindTaskExpressionListCtrl::AddRule()
{
	SEARCHPARAM sp(TDCA_TASKNAMEORCOMMENTS, FO_INCLUDES);

	int nRow = InsertRule(GetRuleCount(), sp);

	// make sure the 'and/or' text of the preceding rule is set
	if (nRow > 0)
	{
		SEARCHPARAM& spPrev = m_aSearchParams[nRow - 1];
		CEnString sAndOr(spPrev.bAnd ? IDS_FP_AND : IDS_FP_OR);
		SetItemText(nRow - 1, ANDOR, sAndOr);
	}

	SetCurSel(nRow, ATTRIB);
	EnsureVisible(nRow, FALSE);

	return TRUE;
}

BOOL CTDLFindTaskExpressionListCtrl::DeleteSelectedRule()
{
	int nRow = GetCurSel();

	if (nRow != -1 && CanDeleteSelectedCell())
	{
		DeleteItem(nRow);
		m_aSearchParams.RemoveAt(nRow);

		ValidateListData();

		SetCurSel(nRow);
		EnsureVisible(nRow, FALSE);

		return TRUE;
	}

	return FALSE;
}

void CTDLFindTaskExpressionListCtrl::MoveSelectedRuleUp()
{
	if (CanMoveSelectedRuleUp())
	{
		int nRow, nCol;
		GetCurSel(nRow, nCol);

		// save off rule
		SEARCHPARAM sp = m_aSearchParams[nRow];

		// delete rule
		m_aSearchParams.RemoveAt(nRow);
		DeleteItem(nRow);

		// reinsert rule
		nRow = InsertRule(nRow - 1, sp);

		// sanity check
		ValidateListData();

		// restore selection
		SetCurSel(nRow, nCol);
		EnsureVisible(nRow, FALSE);
	}
}

int CTDLFindTaskExpressionListCtrl::InsertRule(int nRow, const SEARCHPARAM& sp)
{
	m_aSearchParams.InsertAt(nRow, sp);

	int nNew = InsertItem(nRow, GetAttribName(sp.attrib));
	SetItemText(nNew, OPERATOR, GetOpName(sp.op));

	CString sValue;

	switch (sp.GetAttribType(sp.attrib))
	{
	case FT_STRING:
	case FT_INTEGER:
	case FT_DOUBLE:
		sValue = sp.ValueAsString();
		break;

	case FT_DATE:
		sValue = COleDateTime(sp.dValue).Format(VAR_DATEVALUEONLY);
		break;

	case FT_TIME:
		sValue = CTimeHelper().FormatTime(sp.dValue, sp.dwFlags, 2);
		break;

	case FT_BOOL:
		// handled by operator
		break;
	}
	SetItemText(nNew, VALUE, sValue);

	// omit and/or for last rule
	if (nRow < GetRuleCount() - 1)
	{
		CEnString sAndOr(sp.bAnd ? IDS_FP_AND : IDS_FP_OR);
		SetItemText(nNew, ANDOR, sAndOr);
	}

	return nNew;
}

BOOL CTDLFindTaskExpressionListCtrl::CanMoveSelectedRuleUp() const
{
	int nRow = GetCurSel();

	return (nRow > 0 && nRow < GetRuleCount());
}

void CTDLFindTaskExpressionListCtrl::MoveSelectedRuleDown()
{
	if (CanMoveSelectedRuleDown())
	{
		int nRow, nCol;
		GetCurSel(nRow, nCol);

		// save off rule
		SEARCHPARAM sp = m_aSearchParams[nRow];

		// delete rule
		m_aSearchParams.RemoveAt(nRow);
		DeleteItem(nRow);

		// reinsert rule
		nRow = InsertRule(nRow + 1, sp);

		// sanity check
		ValidateListData();

		// restore selection
		SetCurSel(nRow, nCol);
		EnsureVisible(nRow, FALSE);
	}
}

BOOL CTDLFindTaskExpressionListCtrl::CanMoveSelectedRuleDown() const
{
	return (GetCurSel() < GetRuleCount() - 1);
}

void CTDLFindTaskExpressionListCtrl::PrepareControl(CWnd& ctrl, int nRow, int nCol)
{
	if (!GetRuleCount())
	{
		return;
	}

	SEARCHPARAM& sp = m_aSearchParams[nRow];

	switch (nCol)
	{
	case ATTRIB:
		{
			ASSERT(ctrl.IsKindOf(RUNTIME_CLASS(CComboBox)));
			CComboBox* pCombo = (CComboBox*)&ctrl;

			if (sp.GetAttribType() != FT_NONE)
			{
				pCombo->SelectString(-1, GetAttribName(sp.attrib));
			}
			else
			{
				pCombo->SetCurSel(-1);
			}
		}
		break;

	case OPERATOR:
		{
			ASSERT(ctrl.IsKindOf(RUNTIME_CLASS(CComboBox)));
			CComboBox* pCombo = (CComboBox*)&ctrl;

			pCombo->ResetContent();

			FIND_ATTRIBTYPE nType = sp.GetAttribType();

			switch (nType)
			{
			case FT_STRING:
				ADD_OP_2_COMBO(pCombo, FO_SET);
				ADD_OP_2_COMBO(pCombo, FO_NOT_SET);
				ADD_OP_2_COMBO(pCombo, FO_EQUALS);
				ADD_OP_2_COMBO(pCombo, FO_NOT_EQUALS);
				ADD_OP_2_COMBO(pCombo, FO_INCLUDES);
				ADD_OP_2_COMBO(pCombo, FO_NOT_INCLUDES);
				break;

			case FT_INTEGER:
			case FT_DOUBLE:
			case FT_TIME:
				ADD_OP_2_COMBO(pCombo, FO_SET);
				ADD_OP_2_COMBO(pCombo, FO_NOT_SET);
				ADD_OP_2_COMBO(pCombo, FO_EQUALS);
				ADD_OP_2_COMBO(pCombo, FO_NOT_EQUALS);
				ADD_OP_2_COMBO(pCombo, FO_GREATER);
				ADD_OP_2_COMBO(pCombo, FO_GREATER_OR_EQUAL);
				ADD_OP_2_COMBO(pCombo, FO_LESS);
				ADD_OP_2_COMBO(pCombo, FO_LESS_OR_EQUAL);
				break;

			case FT_DATE:
				ADD_OP_2_COMBO(pCombo, FO_SET);
				ADD_OP_2_COMBO(pCombo, FO_NOT_SET);
				ADD_OP_2_COMBO(pCombo, FO_EQUALS);
				ADD_OP_2_COMBO(pCombo, FO_NOT_EQUALS);
				ADD_OP_2_COMBO(pCombo, FO_AFTER);
				ADD_OP_2_COMBO(pCombo, FO_ON_OR_AFTER);
				ADD_OP_2_COMBO(pCombo, FO_BEFORE);
				ADD_OP_2_COMBO(pCombo, FO_ON_OR_BEFORE);
				break;

			case FT_BOOL:
				ADD_OP_2_COMBO(pCombo, FO_SET);
				ADD_OP_2_COMBO(pCombo, FO_NOT_SET);
				break;
			}

			if (sp.op != FO_NONE)
			{
				pCombo->SelectString(-1, GetOpName(sp.op));
			}
			else
			{
				pCombo->SetCurSel(-1);
			}
		}
		break;

	case ANDOR:
		{
			ASSERT(ctrl.IsKindOf(RUNTIME_CLASS(CComboBox)));
			CComboBox* pCombo = (CComboBox*)&ctrl;

			if (sp.bAnd)
			{
				pCombo->SelectString(-1, CEnString(IDS_FP_AND));
			}
			else
			{
				pCombo->SelectString(-1, CEnString(IDS_FP_OR));
			}
		}
		break;

	case VALUE:
		switch (sp.GetAttribType())
		{
		case FT_DATE:
			ASSERT(&ctrl == &m_dtDate);

			// if the rule does not yet have a date then set it now to
			// the current date because that's whats the date ctrl will default to
			if (sp.dValue <= 0)
			{
				sp.dValue = COleDateTime::GetCurrentTime();
				SetItemText(nRow, nCol, COleDateTime(sp.dValue).Format(VAR_DATEVALUEONLY));
			}
			else
			{
				m_dtDate.SetTime(sp.dValue);
			}
			break;

		case FT_TIME:
			ASSERT(&ctrl == &m_eTime);
			m_eTime.SetTime(sp.dValue, sp.dwFlags);
			break;
		}
		break;

	}
}

void CTDLFindTaskExpressionListCtrl::ValidateListData() const
{
#ifdef _DEBUG
	for (int nRule = 0; nRule < GetRuleCount(); nRule++)
	{
		const SEARCHPARAM& rule = m_aSearchParams[nRule];

		// check matching attribute text
		CString sRuleAttrib = GetAttribName(rule.attrib);
		CString sListAttrib = GetItemText(nRule, ATTRIB);
		ASSERT(sRuleAttrib == sListAttrib);

		// check matching operator text
		CString sRuleOp = GetOpName(rule.op);
		CString sListOp = GetItemText(nRule, OPERATOR);
		ASSERT(sListOp.IsEmpty() || sRuleOp == sListOp);

		// check valid operator
		ASSERT(rule.HasValidOperator());
	}
#endif
}

void CTDLFindTaskExpressionListCtrl::OnAttribEditCancel()
{
	HideControl(m_cbAttributes);
}

void CTDLFindTaskExpressionListCtrl::OnAttribEditOK()
{
	HideControl(m_cbAttributes);

	// update attribute type
	int nSel = m_cbAttributes.GetCurSel();

	if (nSel != CB_ERR)
	{
		CString sSel;
		m_cbAttributes.GetLBText(nSel, sSel);

		int nRow = GetCurSel();
		SetItemText(nRow, ATTRIB, sSel);

		// keep data store synched
		TDC_ATTRIBUTE attrib = (TDC_ATTRIBUTE)m_cbAttributes.GetItemData(nSel);
		m_aSearchParams[nRow].SetAttribute(attrib);

		// clear the operator cell text if the operator was no longer valid
		if (m_aSearchParams[nRow].op == FO_NONE)
		{
			SetItemText(nRow, OPERATOR, _T(""));
		}

		ValidateListData();
	}
}

BOOL CTDLFindTaskExpressionListCtrl::OnSelItemChanged(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	// always make sure we hide the datetime ctrl and the time edit
	HideControl(m_dtDate);
	HideControl(m_eTime);

	*pResult = 0;

	return FALSE; // continue routing
}

void CTDLFindTaskExpressionListCtrl::OnValueEditOK(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	int nRow = pDispInfo->item.iItem;
	int nCol = pDispInfo->item.iSubItem;

	ASSERT(nCol == VALUE);

	SEARCHPARAM& sp = m_aSearchParams[nRow];

	switch (sp.GetAttribType())
	{
	case FT_STRING:
		sp.sValue = pDispInfo->item.pszText;
		break;

	case FT_INTEGER:
		sp.nValue = _ttoi(pDispInfo->item.pszText);
		break;

	case FT_DOUBLE:
		sp.dValue = _tstof(pDispInfo->item.pszText);
		break;

	case FT_DATE:
	case FT_BOOL:
		// not handled here
		break;
	}

	*pResult = 0;
}

void CTDLFindTaskExpressionListCtrl::OnOperatorEditCancel()
{
	HideControl(m_cbOperators);
}

void CTDLFindTaskExpressionListCtrl::OnOperatorEditOK()
{
	HideControl(m_cbOperators);

	// update operator type
	int nSel = m_cbOperators.GetCurSel();

	if (nSel != CB_ERR)
	{
		CString sSel;
		m_cbOperators.GetLBText(nSel, sSel);

		int nRow = GetCurSel();
		SetItemText(nRow, OPERATOR, sSel);

		// keep data store synched
		FIND_OPERATOR nOp = (FIND_OPERATOR)m_cbOperators.GetItemData(nSel);
		SEARCHPARAM& rule = m_aSearchParams[nRow];

		rule.SetOperator(nOp);

		// if the op is set/notset then clear the field
		if (nOp == FO_SET || nOp == FO_NOT_SET)
		{
			rule.dValue = 0.0;
			rule.nValue = 0;
			rule.sValue.Empty();

			SetItemText(nRow, VALUE, _T(""));
		}

		ValidateListData();
	}
}

void CTDLFindTaskExpressionListCtrl::OnAndOrEditCancel()
{
	HideControl(m_cbAndOr);
}

void CTDLFindTaskExpressionListCtrl::OnAndOrEditOK()
{
	HideControl(m_cbAndOr);

	// update operator type
	int nSel = m_cbAndOr.GetCurSel();

	if (nSel != CB_ERR)
	{
		CString sSel;
		m_cbAndOr.GetLBText(nSel, sSel);

		int nRow = GetCurSel();
		SetItemText(nRow, ANDOR, sSel);

		// keep data store synched
		m_aSearchParams[nRow].bAnd = m_cbAndOr.GetItemData(nSel);

		ValidateListData();
	}
}

void CTDLFindTaskExpressionListCtrl::BuildListCtrl()
{
	DeleteAllItems();

	for (int nParam = 0; nParam < GetRuleCount(); nParam++)
	{
		SEARCHPARAM& sp = m_aSearchParams[nParam];

		// attrib
		CString sAttrib = GetAttribName(sp.attrib);
		int nItem = InsertItem(nParam, sAttrib);

		// operator
		CString sOp = GetOpName(sp.op);
		SetItemText(nItem, OPERATOR, sOp);

		// and/or (but not for last row)
		if (nParam < GetRuleCount() - 1)
		{
			CEnString sAndOr(sp.bAnd ? IDS_FP_AND : IDS_FP_OR);
			SetItemText(nItem, ANDOR, sAndOr);
		}

		// value
		CString sValue;

		try
		{
			switch (sp.GetAttribType())
			{
			case FT_STRING:
			case FT_INTEGER:
			case FT_DOUBLE:
				sValue = sp.ValueAsString();
				break;

			case FT_DATE:
				if (sp.op == FO_SET || sp.op == FO_NOT_SET)
				{
					// handled by operator
				}
				else
				{
					sValue = COleDateTime(sp.dValue).Format(VAR_DATEVALUEONLY);
				}
				break;

			case FT_TIME:
				sValue = CTimeHelper().FormatTime(sp.dValue, sp.dwFlags, 2);
				break;

			case FT_BOOL:
				// handled by operator
				break;
			}

			SetItemText(nItem, VALUE, sValue);
		}
		catch (...)
		{
			// bad value but we continue
		}
	}

	ValidateListData();
	SetCurSel(0);
}

CString CTDLFindTaskExpressionListCtrl::GetAttribName(TDC_ATTRIBUTE attrib)
{
	int nAttrib = ATTRIB_COUNT;

	while (nAttrib--)
	{
		if (ATTRIBUTES[nAttrib].attrib == attrib)
		{
			if (ATTRIBUTES[nAttrib].nAttribResID)
			{
				return CEnString(ATTRIBUTES[nAttrib].nAttribResID);
			}
			else
			{
				return _T("");
			}
		}
	}

	ASSERT(0);  // not found
	return _T("");
}

CString CTDLFindTaskExpressionListCtrl::GetOpName(FIND_OPERATOR op)
{
	int nOp = OP_COUNT;

	while (nOp--)
	{
		if (OPERATORS[nOp].op == op)
		{
			if (OPERATORS[nOp].nOpResID)
			{
				return CEnString(OPERATORS[nOp].nOpResID);
			}
			else
			{
				return _T("");
			}
		}
	}

	ASSERT(0);  // not found
	return _T("");
}

void CTDLFindTaskExpressionListCtrl::OnDateChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMDATETIMECHANGE pDTHDR = (LPNMDATETIMECHANGE)pNMHDR;
	COleDateTime dt(pDTHDR->st);

	// update the rule
	int nRow = GetCurSel();

	m_aSearchParams[nRow].dValue = dt.m_dt;
	SetItemText(nRow, VALUE, dt.Format(VAR_DATEVALUEONLY));

	*pResult = 0;
}

void CTDLFindTaskExpressionListCtrl::OnDateCloseUp(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	HideControl(m_dtDate);

	*pResult = 0;
}


void CTDLFindTaskExpressionListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CInputListCtrl::OnKillFocus(pNewWnd);

	if (pNewWnd != &m_dtDate)
	{
		HideControl(m_dtDate);
	}

	if (pNewWnd != &m_eTime)
	{
		HideControl(m_eTime);
	}
}

void CTDLFindTaskExpressionListCtrl::OnTimeChange()
{
	// update the rule
	int nRow = GetCurSel();
	SEARCHPARAM& rule = m_aSearchParams[nRow];

	rule.dValue = m_eTime.GetTime();
	rule.dwFlags = m_eTime.GetUnits();

	SetItemText(nRow, VALUE, CTimeHelper().FormatTime(rule.dValue, rule.dwFlags, 3));
}

LRESULT CTDLFindTaskExpressionListCtrl::OnTimeUnitsChange(WPARAM /*wp*/, LPARAM /*lp*/)
{
	// update the rule
	int nRow = GetCurSel();
	SEARCHPARAM& rule = m_aSearchParams[nRow];

	rule.dwFlags = m_eTime.GetUnits();
	SetItemText(nRow, VALUE, CTimeHelper().FormatTime(rule.dValue, rule.dwFlags, 3));

	return 0L;
}

void CTDLFindTaskExpressionListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CInputListCtrl::OnSize(nType, cx, cy);

	// resize columns by proportion
	SetColumnWidth(ATTRIB, (int)(cx * COL_PROPORTIONS[ATTRIB]));
	SetColumnWidth(OPERATOR, (int)(cx * COL_PROPORTIONS[OPERATOR]));
	SetColumnWidth(VALUE, (int)(cx * COL_PROPORTIONS[VALUE]));
	SetColumnWidth(ANDOR, (int)(cx * COL_PROPORTIONS[ANDOR]));

}

void CTDLFindTaskExpressionListCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int nItem, nCol;
	GetCurSel(nItem, nCol);

	if (GetRuleCount() > 0 || nCol == ATTRIB)
	{
		// if the user typed an alphanumeric char then begin editing automatically
		// numeric keys only work for value column
		if (isalpha(nChar) || (nCol == VALUE && isdigit(nChar)))
		{
			EditCell(nItem, nCol);

			// forward key down on to edit control
			CWnd* pEdit = GetEditControl(nItem, nCol);

			if (pEdit)
			{
				pEdit->PostMessage(WM_CHAR, nChar, MAKELPARAM(nRepCnt, nFlags));
				return; // eat it
			}
		}
	}

	CInputListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
