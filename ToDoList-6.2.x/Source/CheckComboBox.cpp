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
// - taken out from the original ToDoList package for better sharing
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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// CheckComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "CheckComboBox.h"
#include "Misc.h"
#include "DlgUnits.h"
#include "Themed.h"
#include "AutoFlag.h"
#include "DialogHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckComboBox

CCheckComboBox::CCheckComboBox(DWORD dwFlags) : CAutoComboBox(dwFlags)
{
	m_bItemHeightSet = FALSE;
	m_bDrawing = TRUE;
	m_bTextFits = TRUE;
}

CCheckComboBox::~CCheckComboBox()
{
}

BEGIN_MESSAGE_MAP(CCheckComboBox, CAutoComboBox)
	//{{AFX_MSG_MAP(CCheckComboBox)
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_WM_CHAR()
	ON_CONTROL_REFLECT_EX(CBN_EDITCHANGE, OnEditchange)
	ON_CONTROL_REFLECT_EX(CBN_DROPDOWN, OnDropdown)
	ON_CONTROL(LBN_SELCHANGE, 1000, OnLBSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckComboBox message handlers

void CCheckComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	HDC hdc = lpDrawItemStruct->hDC;

	CRect rcBitmap = lpDrawItemStruct->rcItem;
	CRect rcText   = lpDrawItemStruct->rcItem;
	int nItem = (int)lpDrawItemStruct->itemID;

	CString sText;

	// 0 - No check, 1 - Empty check, 2 - Checked
	int nCheck = 0;

	// Check if we are drawing the static portion of the combobox
	if (nItem < 0)
	{
		// Get the text
		sText = m_sText;

		// Don't draw any boxes on this item
		nCheck = 0;
	}
	else // Otherwise it is one of the items
	{
		GetLBText(lpDrawItemStruct->itemID, sText);
		nCheck = 1 + (GetItemData(lpDrawItemStruct->itemID) != 0);

		TEXTMETRIC metrics;
		GetTextMetrics(hdc, &metrics);

		rcBitmap.left    = 0;
		rcBitmap.right   = rcBitmap.left + metrics.tmHeight + metrics.tmExternalLeading + 6;
		rcBitmap.top    += 1;
		rcBitmap.bottom -= 1;

		rcText.left = rcBitmap.right;
	}

	if (nCheck > 0)
	{
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
		SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));

		UINT nState = DFCS_BUTTONCHECK;

		if (nCheck > 1)
		{
			nState |= DFCS_CHECKED;
		}

		// Draw the checkmark using DrawFrameControl
		CThemed::DrawFrameControl(ScGetCWnd(), CDC::FromHandle(hdc), rcBitmap, DFC_BUTTON, nState);
	}

	if (nCheck > 0 || IsType(CBS_DROPDOWNLIST))
	{
		DrawItemText(hdc, nItem, rcText, sText, lpDrawItemStruct->itemState);
	}
}

void CCheckComboBox::RefreshMaxDropWidth()
{
	int nMaxWidth = CDialogHelper::CalcMaxTextWidth(*this, 0, TRUE) + 18;
	SetDroppedWidth(nMaxWidth);
}

void CCheckComboBox::DrawItemText(HDC hdc, int nItem, CRect rText, const CString& sText, UINT nState)
{
	if (nState & (ODS_GRAYED | ODS_DISABLED))
	{
		SetBkColor(hdc, GetSysColor(COLOR_3DFACE));
		SetTextColor(hdc, GetSysColor(COLOR_3DSHADOW));
	}
	else if (nState & ODS_SELECTED)
	{
		SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
		SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else
	{
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
		SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
	}

	// Erase and draw
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rText, 0, 0, 0);

	rText.DeflateRect(2, 0);

	// update 'text fits' flag
	if (nItem < 0)
	{
		CSize sizeText;
		GetTextExtentPoint(hdc, sText, sText.GetLength(), &sizeText);
		m_bTextFits = sizeText.cx <= rText.Width();
	}

	DrawText(hdc, sText, sText.GetLength(), &rText, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
	rText.InflateRect(2, 0);

	if ((nState & (ODS_FOCUS | ODS_SELECTED)) == (ODS_FOCUS | ODS_SELECTED))
	{
		DrawFocusRect(hdc, &rText);
	}
}

void CCheckComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 0x01 && GetDroppedState())
	{
		CheckAll(IsAnyUnchecked());
	}

	CAutoComboBox::OnChar(nChar, nRepCnt, nFlags);
}

void CCheckComboBox::CheckAll(BOOL bCheck)
{
	int nCount = GetCount();

	for (int i = 0; i < nCount; i++)
	{
		SetItemData(i, bCheck);   // prevent multiple refreshes
	}

	// derived classes
	OnCheckChange(-1);

	// update text
	RecalcText();

	// redraw listbox if dropped
	if (GetDroppedState())
	{
		::InvalidateRect(m_hwndListbox, NULL, FALSE);
	}
}

BOOL CCheckComboBox::IsAnyChecked() const
{
	int nCount = GetCount();

	for (int i = 0; i < nCount; i++)
	{
		if (GetCheck(i))
		{
			return true;
		}
	}

	return false;
}

BOOL CCheckComboBox::IsAnyUnchecked() const
{
	int nCount = GetCount();

	for (int i = 0; i < nCount; i++)
	{
		if (!GetCheck(i))
		{
			return true;
		}
	}

	return false;
}


void CCheckComboBox::RecalcText(BOOL bUpdate, BOOL bNotify)
{
	CStringArray aItems;
	GetChecked(aItems);

	CString sText = Misc::FormatArray(aItems);

	// update edit field if necessary
	if (bUpdate)
	{
		if (!IsType(CBS_DROPDOWNLIST))
		{
			GetDlgItem(1001)->SetWindowText(sText);

			// notify parent
			if (bNotify && m_sText != sText)
			{
				NotifyParent(CBN_EDITCHANGE);
			}
		}
		else
		{
			SetWindowText(sText);
		}
	}

	m_sText = sText;
}

int CCheckComboBox::SelectString(int nStartAfter, LPCTSTR lpszString)
{
	CStringArray aItems;

	if (Misc::Split(lpszString, aItems) > 1) // multiple items
	{
		SetChecked(aItems);
		return GetCurSel();
	}

	// else
	return CAutoComboBox::SelectString(nStartAfter, lpszString);
}

int CCheckComboBox::SetCheck(int nIndex, BOOL bFlag)
{
	int nResult = SetItemData(nIndex, bFlag);

	if (nResult < 0)
	{
		return nResult;
	}

	// derived classes
	OnCheckChange(nIndex);

	// update text
	RecalcText();

	// Redraw the window
	CComboBox::Invalidate(FALSE);

	return nResult;
}

BOOL CCheckComboBox::GetCheck(int nIndex) const
{
	return GetItemData(nIndex);
}

void CCheckComboBox::ParseText()
{
	if (IsType(CBS_DROPDOWNLIST))
	{
		return;
	}

	CString sEditText;
	GetDlgItem(1001)->GetWindowText(sEditText);

	// clear existing checks first but don't update window
	int nCount = GetCount();

	for (int i = 0; i < nCount; i++)
	{
		SetItemData(i, 0);
	}

	// now parse the text and set the check states
	CStringArray aText;

	if (Misc::Split(sEditText, aText))
	{
		int nText = aText.GetSize();

		while (nText--)
		{
			int nIndex = FindStringExact(-1, aText[nText]);

			if (nIndex != CB_ERR)
			{
				SetItemData(nIndex, 1);
			}
		}
	}

	if (GetDroppedState())
	{
		ScGetCWnd()->Invalidate();
	}

	// Redraw the window
	CComboBox::Invalidate(FALSE);
}

// this handles messages destined for the dropped listbox
LRESULT CCheckComboBox::ScWindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		// Make the combobox always return -1 as the current selection for
		// combos without an edit potion when drawing. This
		// causes the lpDrawItemStruct->itemID in DrawItem() to be -1
		// when the always-visible-portion of the combo is drawn
	case LB_GETCURSEL:
		if (IsType(CBS_DROPDOWNLIST) && m_bDrawing)
		{
			return -1;
		}
		break;

	case WM_CHAR: // sent by the edit control (if present) or the list box
		if (wp == VK_SPACE && GetDroppedState())
		{
			if ((hRealWnd == GetHwnd() && Misc::KeyIsPressed(VK_CONTROL)) /*Edit*/ ||
					(hRealWnd == ScGetHwnd())) /*Listbox*/
			{
				// Get the current selection
				int nIndex = GetCurSel();

				CRect rcItem;
				::SendMessage(hRealWnd, LB_GETITEMRECT, nIndex, (LONG)(VOID*)&rcItem);
				::InvalidateRect(hRealWnd, rcItem, FALSE);

				// Invert the check mark
				SetCheck(nIndex, !GetCheck(nIndex));

				m_bEditChange = TRUE;

				// Notify that selection has changed
				if (IsType(CBS_DROPDOWNLIST))
				{
					NotifyParent(CBN_SELCHANGE);
				}

				return 0;
			}
		}
		break;

	case WM_LBUTTONDOWN:
		{
			CPoint pt(lp);
			int nItem = GetCount();

			while (nItem--)
			{
				CRect rItem;
				::SendMessage(hRealWnd, LB_GETITEMRECT, nItem, (LPARAM)(LPRECT)rItem);

				if (rItem.PtInRect(pt))
				{
					// toggle check state
					::InvalidateRect(hRealWnd, rItem, FALSE);
					SetCheck(nItem, !GetCheck(nItem));
					m_bEditChange = TRUE;

					// Notify that selection has changed
					if (IsType(CBS_DROPDOWNLIST))
					{
						NotifyParent(CBN_SELCHANGE);
					}

					return 0;
				}
			}
		}
		// Do the default handling now (such as close the popup
		// window when clicked outside)
		break;

	case WM_LBUTTONUP:
		{
			// Don't do anything here. This causes the combobox popup
			// windows to remain open after a selection has been made
			if (IsType(CBS_SIMPLE))
			{
				return 0;
			}
			else
			{
				LRESULT lr = CSubclasser::ScWindowProc(hRealWnd, msg, wp, lp);
				SetCheck(0, GetCheck(0));
				return lr;
			}
		}
		break;
	}

	return CSubclasser::ScWindowProc(hRealWnd, msg, wp, lp);
}

int CCheckComboBox::GetCurSel() const
{
	CAutoFlag af(m_bDrawing, FALSE);

	return CComboBox::GetCurSel();
}

BOOL CCheckComboBox::IsType(UINT nComboType)
{
	return ((CWnd::GetStyle() & 0xf) == nComboType);
}

int CCheckComboBox::AddUniqueItem(const CString& sItem)
{
	if (sItem.Find(Misc::GetListSeparator()) != -1)
	{
		CStringArray aText;

		Misc::Split(sItem, aText);
		return CAutoComboBox::AddUniqueItems(aText);
	}

	// else add single item and mark as selected
	int nIndex = CAutoComboBox::AddUniqueItem(sItem);

	if (nIndex != CB_ERR)
	{
		SetItemData(nIndex, 1);
	}

	return nIndex;
}

HBRUSH CCheckComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CAutoComboBox::OnCtlColor(pDC, pWnd, nCtlColor);

	// hook list box
	if (nCtlColor == CTLCOLOR_LISTBOX && !ScIsHooked())
	{
		ScHookWindow(pWnd->GetSafeHwnd());
	}

	return hbr;
}

void CCheckComboBox::PreSubclassWindow()
{
	if (!m_bItemHeightSet)
	{
		m_bItemHeightSet = TRUE;

		CDlgUnits dlu(CComboBox::GetParent());
		SetItemHeight(-1, dlu.ToPixelsY(9));
	}

	CAutoComboBox::PreSubclassWindow();
}

int CCheckComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CAutoComboBox::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	if (!m_bItemHeightSet)
	{
		m_bItemHeightSet = TRUE;

		CDlgUnits dlu(CComboBox::GetParent());
		SetItemHeight(-1, dlu.ToPixelsY(9));
	}

	return 0;
}

BOOL CCheckComboBox::OnEditchange()
{
	// update check states
	if (GetDroppedState())
	{
		ParseText();
	}

	m_bEditChange = TRUE;

	return FALSE; // pass to parent
}

BOOL CCheckComboBox::OnDropdown()
{
	ParseText();
	RecalcText(FALSE); // updates m_sText

	return FALSE; // pass to parent
}

// this handles messages destined for the embedded edit field
LRESULT CCheckComboBox::WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static UINT nLastMsg = 0;

	switch (msg)
	{
	case WM_KEYDOWN:
		if (wp == VK_DOWN && !GetDroppedState())
		{
			ShowDropDown();
			return 0L; // eat
		}
		else if (wp == VK_RETURN)
		{
			if (GetDroppedState())
			{
				ShowDropDown(FALSE);
			}

			HandleReturnKey();
			return 0L; // eat
		}
		break;

	case WM_CHAR: // if CTRL+Space then forward to listbox
		if (wp == VK_SPACE && Misc::KeyIsPressed(VK_CONTROL))
		{
			if ((GetDroppedState() && IsType(CBS_DROPDOWN)) || IsType(CBS_SIMPLE))
			{
				ScWindowProc(hRealWnd, msg, wp, lp);
				return 0;
			}
		}
		else if (wp == VK_ESCAPE)
		{
			m_bEditChange = FALSE;
			ShowDropDown(FALSE);
		}
		break;

	case EM_SETSEL:
		if (GetDroppedState() && (wp == 0) && (lp == (LPARAM) - 1))
		{
			// avoid duplicate messages
			static LONG lLastParams = 0;

			// if this is a 'select all' and we're dropped then
			// assume it was in response to CTRL+A in the edit field
			if (lLastParams == MAKELONG(wp, lp))
			{
				lLastParams = 0; // duplicate
				break;
			}
			else
			{
				lLastParams = MAKELONG(wp, lp); // cache new value

				CheckAll(IsAnyUnchecked());

				// Notify that selection has changed
				if (IsType(CBS_DROPDOWNLIST))
				{
					NotifyParent(CBN_SELCHANGE);
				}

				m_bEditChange = TRUE;

				return 0; // eat it
			}
		}
		break;
	}

	// default handling
	return CAutoComboBox::WindowProc(hRealWnd, msg, wp, lp);
}

void CCheckComboBox::HandleReturnKey()
{
	m_bEditChange = FALSE;

	if (!GetDroppedState())
	{
		ParseText();
		RecalcText(FALSE, FALSE); // update m_sText

		CAutoComboBox::HandleReturnKey();
	}
	else
	{
		ShowDropDown(FALSE);

		// notify parent of (possible) selection change
		NotifyParent(CBN_SELCHANGE);
	}
}

CString CCheckComboBox::GetSelectedItem() const
{
	return m_sText;
}

void CCheckComboBox::OnLBSelChange()
{
	// do nothing
}

BOOL CCheckComboBox::DeleteSelectedLBItem()
{
	if (CAutoComboBox::DeleteSelectedLBItem())
	{
		RecalcText();
		return TRUE;
	}

	// else
	return FALSE;
}

int CCheckComboBox::GetChecked(CStringArray& aItems) const
{
	aItems.RemoveAll();

	int nCount = GetCount();

	for (int i = 0; i < nCount; i++)
	{
		if (GetItemData(i))
		{
			CString sItem;
			GetLBText(i, sItem);

			aItems.Add(sItem);
		}
	}

	return aItems.GetSize();
}

CString CCheckComboBox::FormatCheckedItems(LPCTSTR szSep) const
{
	CStringArray aChecked;
	GetChecked(aChecked);

	return Misc::FormatArray(aChecked, szSep);
}

void CCheckComboBox::SetChecked(const CStringArray& aItems)
{
	// clear existing checks first but don't update window
	int nCount = GetCount();

	for (int i = 0; i < nCount; i++)
	{
		SetItemData(i, 0);
	}

	// now set the check states
	int nItem = aItems.GetSize(), nChecked = 0;

	while (nItem--)
	{
		int nIndex = FindStringExact(-1, aItems[nItem]);

		if (nIndex != CB_ERR)
		{
			SetItemData(nIndex, 1);
			nChecked++;
		}
	}
	RecalcText(TRUE, FALSE);

	if (GetDroppedState())
	{
		ScGetCWnd()->Invalidate();
	}

	// Redraw the window
	CComboBox::Invalidate(FALSE);
}

void CCheckComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_DOWN && !GetDroppedState())
	{
		ShowDropDown();
		return;
	}

	CAutoComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

int CCheckComboBox::GetCheckedCount() const
{
	int nCount = 0;

	for (int nItem = 0; nItem < GetCount(); nItem++)
	{
		if (GetCheck(nItem))
		{
			nCount++;
		}
	}

	return nCount;
}

CString CCheckComboBox::GetTooltip() const
{
	if (!m_bTextFits)
	{
		return FormatCheckedItems(_T("\n"));
	}

	// else
	return _T("");
}
