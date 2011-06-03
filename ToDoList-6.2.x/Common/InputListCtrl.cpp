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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// InputListCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "InputListCtrl.h"
#include "../../CodeProject/Source/Themed.h"
#include "../../CodeProject/Source/EnString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputListCtrl

#define MAXCOLWIDTH 600
#define MINCOLWIDTH 80
#define IDC_EDITBOX 101
#define WM_SHOWPOPUPMENU (WM_APP+1001)
#define BTN_WIDTH 17

static DWORD PROMPT = 0xfefefefe;

CInputListCtrl::CInputListCtrl()
{
	m_nItemLastSelected = -1;
	m_nColLastSelected = -1;
	m_nEditItem = -1;
	m_nEditCol = -1;
	m_bAutoAddRows = FALSE;
	m_bAutoAddCols = FALSE;
	m_nAutoColWidth = MINCOLWIDTH;
	m_sAutoRowPrompt = _T("(new row)");
	m_sAutoColPrompt = _T("(new col)");
	m_bAllowDuplication = TRUE;
	m_bNotifyDuplicates = FALSE;
	m_bBaseClassEdit = FALSE;
	m_nCurCol = -1;
	m_nItemHeight = 14;
}

CInputListCtrl::~CInputListCtrl()
{
}


BEGIN_MESSAGE_MAP(CInputListCtrl, CEnListCtrl)
	//{{AFX_MSG_MAP(CInputListCtrl)
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_REGISTERED_MESSAGE(WM_PENDEDIT, OnEditEnd)
	ON_REGISTERED_MESSAGE(WM_PCANCELEDIT, OnEditCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputListCtrl message handlers

const CColumnData2* CInputListCtrl::GetColumnData(int nCol) const
{
	return static_cast<const CColumnData2*>(CEnListCtrl::GetColumnData(nCol));
}

void CInputListCtrl::AllowDuplicates(BOOL bAllow, BOOL bNotify)
{
	m_bAllowDuplication = bAllow;
	m_bNotifyDuplicates = bNotify;
}

void CInputListCtrl::OnLButtonDblClk(UINT /*nFlags*/, CPoint point)
{
	// find out which column the user clicked on
	int nItem = HitTest(point);
	int nCol = GetColumnAtPoint(point);

	// DO NOT DO DEFAULT HANDLING
	// because it causes flicker in combination with our owner draw
	//CEnListCtrl::OnLButtonDblClk(nFlags, point);
	if (GetCurSel() != -1)
	{
		// if we cant edit then pass on to parent but not otherwise since this would
		// cause the focus to be removed from the edit control
		if (!CanEditSelectedCell())
		{
			// dg 4/2/99
			// this produced counter-intuitive results so i'm disabling it for now
			// instead you must right-click and select 'properties'
			//m_nmhdr.hwndFrom = m_hWnd;
			//m_nmhdr.idFrom = GetDlgCtrlID();
			//m_nmhdr.code = LVN_USERSELCHANGEDBLCLK;
			//GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&m_nmhdr);
		}
		else if (!(IsEditing() && nItem == m_nEditItem && nCol == m_nEditCol))
		{
			// the above check deals with the situation than occurs when the user
			// double clicks on an already selected item.
			// ie the WM_LBUTTONDOWN that preceeds WM_LBUTTONDBLCLK will already
			// have initiated an edit
			EditCell(nItem, nCol);
		}
	}
}

void CInputListCtrl::OnLButtonDown(UINT /*nFlags*/, CPoint point)
{
	// DO NOT DO DEFAULT HANDLING
	// because it causes flicker in combination with our owner draw
	//CEnListCtrl::OnLButtonDown(nFlags, point);

	int nSelItem, nSelCol;
	GetCurSel(nSelItem, nSelCol);

	BOOL bHadFocus = (GetFocus() == this);
	BOOL bWasEditing = IsEditing();
	int nItem = HitTest(point);
	int nCol = GetColumnAtPoint(point);

	SetFocus();

	// if we're were editing the same cell we've clicked on then
	// we may be cancelling a listbox edit so don't trigger it again
	if (bWasEditing && nItem == GetCurSel() && nCol == m_nCurCol)
	{
		return;
	}

	// if this is the second click then edit else update clicked pos
	// unless we did not have the focus
	if (nItem != -1)
	{
		if (bHadFocus && nItem == nSelItem && nCol == nSelCol && CanEditSelectedCell())
		{
			EditCell(nItem, nCol);
		}
		else
		{
			m_nItemLastSelected = nItem;
			m_nColLastSelected = nCol;

			SetCurSel(nItem, nCol, TRUE); // notifies parent
			SetItemFocus(nItem, TRUE);

			// scroll cell into view
			ScrollCellIntoView(nItem, nCol);
		}

		m_nCurCol = nCol;
	}
}

void CInputListCtrl::EditCell(int nItem, int nCol)
{
	CString sText;
	CRect rEdit;
	int nLastRow, nLastCol;

	// if this is the right column, scroll item fully into view
	// begin editing and move the
	// edit control to the correct column and fill edit control
	// with the right text
	if (nCol != -1 && nItem != -1 && !IsReadOnly())
	{
		// dont edit if editing is disabled
		if (IsColumnEditingDisabled(nCol))
		{
			return;
		}

		// dont edit if both autorowadding and autocoladding are enabled and we're in
		// pos (0, 0)
		if (m_bAutoAddRows && m_bAutoAddCols && nItem == 0 && nCol == 0)
		{
			return;
		}

		// dont edit if both autorowadding and autocoladding are enabled and we're in
		// other than col0 for row adding or row0 for col adding
		nLastRow = GetItemCount() - 1;
		nLastCol = GetColumnCount() - 1;

		if ((m_bAutoAddRows && nItem == nLastRow && nCol != 0) ||
			(m_bAutoAddCols && nCol == nLastCol && nItem != 0))
		{
			return;
		}

		m_nEditItem = nItem;
		m_nEditCol = nCol;
		GetEditControl()->Reset();

		// position edit box
		ScrollCellIntoView(nItem, nCol);
		GetCellEditRect(nItem, nCol, rEdit);

		// set text and show
		// if its a new row or column then clear the prompt text
		if (IsPrompt(nItem, nCol))
		{
			sText.Empty();
		}
		else
		{
			sText = GetItemText(nItem, nCol);
		}

		GetEditControl()->SetWindowText(sText);

		if (GetEditControl()->GetStyle() & WS_POPUP)
		{
			ClientToScreen(rEdit);
		}

		GetEditControl()->Show(rEdit);

		// this says that we are handling the edit
		// not a derived class.
		m_bBaseClassEdit = TRUE;
	}
}

long CInputListCtrl::OnEditEnd(WPARAM wParam, LPARAM lParam)
{
	int nResult;
	BOOL bReturnSelected = (BOOL)lParam;

	// wParam holds the id of the ctrl which sent the message
	OnEndEdit((UINT)wParam, &nResult);

	// end can occurs either when the user selected return
	// or if the edit box loses the focus so we need to check
	// the lParam and only set the focus if the user chose return
	if (bReturnSelected)
	{
		SetFocus();
	}

	// dg 12/11/01 - user is explicitly responsible for resetting the edit mask
	// if they change it
	//GetEditControl()->SetMask(NULL);

	return 0L;
}

BOOL CInputListCtrl::IsDuplicateRow(CString sRow, int nRowToIgnore)
{
	int nNumRows;

	nNumRows = GetItemCount();

	while (nNumRows--)
	{
		if (sRow.CompareNoCase(GetItemText(nNumRows, 0)) == 0 && nNumRows != nRowToIgnore)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CInputListCtrl::IsDuplicateCol(CString sCol, int nColToIgnore)
{
	int nNumCols;

	nNumCols = GetColumnCount();

	while (nNumCols--)
	{
		if (sCol.CompareNoCase(GetItemText(0, nNumCols)) == 0 && nNumCols != nColToIgnore)
		{
			return TRUE;
		}
	}

	return FALSE;
}

long CInputListCtrl::OnEditCancel(WPARAM /*wParam*/, LPARAM lParam)
{
	BOOL bEscapeSelected = (BOOL)lParam;

	OnCancelEdit();

	// cancel can occurs either when the user selected escape
	// or if the edit box loses the focus so we need to check
	// the lParam and only set the focus if the user chose escape
	if (bEscapeSelected)
	{
		SetFocus();
	}

	// dg 12/11/01 - user is explicitly responsible for resetting the edit mask
	// if they change it
	//GetEditControl()->SetMask(NULL);

	return 0L;
}

void CInputListCtrl::DisableColumnEditing(int nCol, BOOL bDisable)
{
	CColumnData2* pData = (CColumnData2*)CreateColumnData(nCol);

	// add a new item if necessary
	if (pData)
	{
		pData->bEditEnabled = !bDisable;
	}
}

BOOL CInputListCtrl::IsColumnEditingDisabled(int nCol) const
{
	const CColumnData2* pData = GetColumnData(nCol);

	// add a new item if necessary
	if (pData)
	{
		return !pData->bEditEnabled;
	}

	// else
	return FALSE;
}

void CInputListCtrl::AutoAdd(BOOL bRows, BOOL bCols)
{
	ASSERT(m_hWnd);

	if (!m_hWnd)
	{
		return;
	}

	// rows
	if (m_bAutoAddRows && !bRows)
	{
		m_bAutoAddRows = FALSE;

		// delete last row
		DeleteItem(GetItemCount() - 1);
	}
	else if (!m_bAutoAddRows && bRows)
	{
		m_bAutoAddRows = TRUE;

		// ensure there is at least one column
		if (GetColumnCount() == 0)
		{
			InsertColumn(0, _T(""), LVCFMT_LEFT, m_nAutoColWidth);
		}
		else if (bCols)
		{
			SetColumnWidth(0, m_nAutoColWidth);
		}

		int nIndex = InsertItem(GetItemCount(), m_sAutoRowPrompt, -1);
		SetItemData(nIndex, PROMPT);
	}

	// cols
	if (m_bAutoAddCols && !bCols)
	{
		m_bAutoAddCols = FALSE;
		// delete last column
	}
	else if (!m_bAutoAddCols && bCols)
	{
		m_bAutoAddCols = TRUE;

		InsertColumn(GetColumnCount(), _T(""), LVCFMT_LEFT, m_nAutoColWidth);
		InsertItem(0, _T(""));
		SetItemText(0, GetColumnCount() - 1, m_sAutoColPrompt);
	}

	m_nCurCol = 0;
	Invalidate();
}

void CInputListCtrl::SetAutoColumnWidth(int nWidth)
{
	// don't allow outside than min/max sizes
	m_nAutoColWidth = min(nWidth, MAXCOLWIDTH);
	m_nAutoColWidth = max(nWidth, MINCOLWIDTH);
}

void CInputListCtrl::SetAutoRowPrompt(CString sPrompt)
{
	// can't set to empty string
	if (!sPrompt.IsEmpty())
	{
		m_sAutoRowPrompt = sPrompt;
	}
}

void CInputListCtrl::SetAutoColPrompt(CString sPrompt)
{
	// can't set to empty string
	if (!sPrompt.IsEmpty())
	{
		m_sAutoColPrompt = sPrompt;
	}
}

void CInputListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (IsEditing())
	{
		return;
	}

	int nOldCol, nOldItem, nCol = m_nCurCol;
	CRect rItem;
	int nItem;

	// save current selection
	GetCurSel(nOldItem, nOldCol);

	// DO NOT DO DEFAULT HANDLING
	// because it causes flicker in combination with our owner draw
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	nItem = GetFocusedItem();
	m_nItemLastSelected = nItem;

	// if its the right or left cursor keys then update column pos
	if (nChar == VK_LEFT && nCol > 0)
	{
		nCol--;

		// scroll cell into view
		ScrollCellIntoView(nItem, nCol);
	}
	else if (nChar == VK_RIGHT && nCol < GetColumnCount() - 1)
	{
		nCol++;

		// scroll cell into view
		ScrollCellIntoView(nItem, nCol);
	}

	// if the delete key is pressed and we're in col0 or row0
	// then delete correseponding row or column
	// unless its the prompt row or column
	else if (nChar == VK_DELETE && CanDeleteSelectedCell())
	{
		DeleteSelectedCell();
	}

	// if its the space bar then edit the current cell
	else if ((nChar == VK_F2 || nChar == VK_SPACE || nChar == VK_RETURN) && CanEditSelectedCell())
	{
		EditCell(nItem, nCol);
	}

	// update the list selection if its changed
	SetCurSel(nItem, nCol, TRUE); // notifies parent
}

void CInputListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// this function differs from the base class in as much as it
	// highlights only the currently selected cell, not the whole line
	CDC* pDC;
	CRect rText, rItem, rClient, rHeader, rFocus;
	int nItem, nCol;
	BOOL bRes;
	int nWidth, nXPos, nYPos;
	int nFormat;
	CSize sizeText;
	LV_COLUMN lvc;
	CImageList* pImageList;
	CImageList* pStateList;
	int nImage, nImageStyle, nImageWidth = 0, nState;
	BOOL bItemFocused, bListFocused, bSelected, bSelAlways;
	UINT uStyle, uState;
	BOOL bIsPrompt;
	int nColType;
	int nLastRow = GetItemCount() - 1;
	int nLastCol = GetColumnCount() - 1;
	CSize sizeState, sizeImage;

	// get and prepare devide context
	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	pDC->SelectObject(GetFont());
	pDC->SetROP2(R2_COPYPEN);

	// init helper variables
	nItem = lpDrawItemStruct->itemID;
	uStyle = GetStyle();
	uState = GetItemState(nItem, LVIS_FOCUSED | LVIS_SELECTED);
	pImageList = GetImageList(LVSIL_SMALL);
	pStateList = GetImageList(LVSIL_STATE);
	nImage = GetImageIndex(nItem, 0);

	// init helper variables
	rItem = lpDrawItemStruct->rcItem;
	rText = rItem;
	GetClientRect(&rClient);

	// some problems with drophiliting items during drag and drop
	// so we need to make sure drawing is clipped to client area
	// this fixes it admirably!
	if (GetHeader())
	{
		GetHeader()->GetWindowRect(rHeader);
		ScreenToClient(rHeader);
		rClient.top = max(0, rHeader.bottom);
		pDC->IntersectClipRect(rClient);
	}

	bListFocused = (GetFocus() == this);
	bSelected = (nItem == GetCurSel());
	bSelAlways = ((uStyle & LVS_SHOWSELALWAYS) == LVS_SHOWSELALWAYS);
	bSelected = bSelected && (bListFocused || bSelAlways);
	bItemFocused = bListFocused && bSelected;

	// images
	// DO NOT SUPPORT INDENTATION
	pImageList = GetImageList(LVSIL_SMALL);

	if (pImageList)
	{
		nImage = GetImageIndex(nItem, 0);
		ImageList_GetIconSize(pImageList->m_hImageList, (int*)&sizeImage.cx, (int*)&sizeImage.cy);
	}

	// state
	pStateList = GetImageList(LVSIL_STATE);

	if (pStateList)
	{
		ImageList_GetIconSize(pStateList->m_hImageList, (int*)&sizeState.cx, (int*)&sizeState.cy);
	}

	if (lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT))
	{
		lvc.mask = LVCF_WIDTH | LVCF_FMT;
		nCol = 0;

		// draw state image if required
		bRes = GetColumn(nCol, &lvc);

		if (pStateList && bRes)
		{
			nState = (GetItemState(nItem, LVIS_STATEIMAGEMASK) & LVIS_STATEIMAGEMASK);
			nImage = nState >> 12;
			pStateList->Draw(pDC, nImage, CPoint(rText.left + 1, rText.top), ILD_TRANSPARENT);

			if (lvc.cx > sizeState.cx)
			{
				pStateList->Draw(pDC, nState, CPoint(rText.left + 1, rText.top), ILD_TRANSPARENT);
			}

			nImageWidth = sizeState.cx + 2; // 1 pixel border either side
		}

		// draw item image
		if (pImageList && bRes)
		{
			if (bSelected && (nCol == m_nCurCol))
			{
				if (bListFocused && !IsEditing())
				{
					nImageStyle = ILD_BLEND50;
				}
				else
				{
					nImageStyle = ILD_TRANSPARENT;
				}
			}
			else
			{
				nImageStyle = ILD_TRANSPARENT;
			}

			if (lvc.cx > nImageWidth + sizeImage.cx)
			{
				pImageList->Draw(pDC, nImage, CPoint(rText.left + 1 + nImageWidth, rText.top), nImageStyle);
			}

			nImageWidth += sizeImage.cx + 2; // 1 pixel border either side
		}

		// cycle thru the columns drawing each one
		rText.left += nImageWidth;

		while (bRes)
		{
			nColType = GetColumnType(nCol);

			bIsPrompt = IsPrompt(nItem, nCol);
			BOOL bSel = (bSelected && nCol == m_nCurCol);

			// save width and format because GetItem overwrites
			// if first column deduct width of image if exists
			nWidth = (nCol == 0) ? lvc.cx - nImageWidth : lvc.cx;
			nFormat = (lvc.fmt & LVCFMT_JUSTIFYMASK);

			// get next item
			bRes = GetColumn(nCol + 1, &lvc);

			// set pos of rhs of text rect
			GetCellRect(nItem, nCol, rText);

			// draw column type bitmap if necessary
			BOOL bHasButton = FALSE;

			if (nColType != TEXT && (nItem == GetCurSel()) && (nCol == m_nCurCol) &&
				(bListFocused || IsEditing()) && !IsReadOnly() && IsWindowEnabled())
			{
				if (!((m_bAutoAddRows && nItem == nLastRow && nCol != 0) ||
					(m_bAutoAddCols && nCol == nLastCol && nItem != 0)) && CanEditSelectedCell())
				{
					bHasButton = TRUE;

					// draw a button BTN_WIDTH pixels wide by the row height
					CRect rButton(rText);
					rButton.left = rButton.right - BTN_WIDTH;
					rText.right = rButton.left;

					switch (nColType)
					{
					case DROPLIST:
						CThemed::DrawFrameControl(this, pDC, rButton, DFC_SCROLL, DFCS_SCROLLCOMBOBOX);
						break;

					case BROWSE:
						CThemed::DrawFrameControl(this, pDC, rButton, DFC_BUTTON, DFCS_BUTTONPUSH);
						pDC->DrawText(_T("..."), rButton, DT_CENTER | DT_VCENTER);
						break;
					}
				}
			}

			// setup colors
			pDC->SetBkColor(GetItemBackColor(nItem, bSel && !IsEditing(), FALSE, bListFocused));
			pDC->SetTextColor(GetItemTextColor(nItem, nCol, bSel && !IsEditing(), FALSE, bListFocused));

			// get item text and output
			CEnString sText;;

			if (bIsPrompt && IsReadOnly())
			{
				sText.Empty();   // hides the prompt if readonly
			}
			else
			{
				CString sTemp(GetItemText(nItem, nCol));

				sText = sTemp;
				sizeText = sText.FormatDC(pDC, nWidth, GetColumnFormat(nCol));
			}

			// set y pos of first char
			nYPos = rText.top + (rText.Height() - sizeText.cy) / 2;

			// set x pos of first char
			switch (nFormat)
			{
			case LVCFMT_CENTER:
				nXPos = rText.left + (rText.Width() - sizeText.cx) / 2;
				break;

			case LVCFMT_RIGHT:
				nXPos = rText.right - 4 - sizeText.cx;
				break;

			case LVCFMT_LEFT:
			default:
				nXPos = rText.left + 4;
				break;
			}

			pDC->ExtTextOut(nXPos, nYPos, ETO_CLIPPED | ETO_OPAQUE, rText, sText, NULL);

			// setup focus rect
			if (nCol == m_nCurCol && nItem == GetCurSel())
			{
				rFocus = rText;

				if (nCol == 0)
				{
					rFocus.left += 2;
				}
			}

			// adjust for previously removed button width
			if (bHasButton)
			{
				rText.right += BTN_WIDTH;
			}

			// draw vert grid if required
			if (m_bVertGrid)
			{
				if (nCol > 0)
				{
					pDC->FillSolidRect(rText.left, rItem.top, 1, rItem.Height(), GetSysColor(COLOR_3DSHADOW));
				}

				// if its the last column and we're not tight up against the client edge
				// then draw the end line
				if (!bRes && rText.right != rClient.right)
				{
					pDC->FillSolidRect(rText.right, rItem.top, 1, rItem.Height(), GetSysColor(COLOR_3DSHADOW));
				}
			}

			// next column
			nCol++;
		}

		// draw horz grid lines if required
		if (m_bHorzGrid)
		{
			if (!m_bVertGrid)
			{
				pDC->FillSolidRect(rClient.left, rText.bottom - 1, rClient.Width(), 1, GetSysColor(COLOR_3DSHADOW));
			}
			else
			{
				pDC->FillSolidRect(rClient.left, rText.bottom - 1, rText.right, 1, GetSysColor(COLOR_3DSHADOW));
			}
		}

		// then draw focus rect
		if (bItemFocused && bListFocused)
		{
			pDC->DrawFocusRect(rFocus);
		}
	}
}

BOOL CInputListCtrl::CanDeleteSelectedCell() const
{
	// if readonly or disabled then no
	if (IsReadOnly() || !IsWindowEnabled())
	{
		return FALSE;
	}

	if (GetCurSel() != -1)
	{
		// don't delete it if its the topleft item
		if (m_bAutoAddCols && GetCurSel() == 0 && m_bAutoAddRows && m_nCurCol == 0)
		{
			return FALSE;
		}

		// else can delete it if its not the row prompt
		else if (m_bAutoAddRows && m_nCurCol == 0)
		{
			if (GetCurSel() < GetItemCount() - 1)
			{
				return TRUE;
			}
		}
		// else can delete it if its not the col prompt
		else if (m_bAutoAddCols && GetCurSel() == 0)
		{
			if (m_nCurCol < GetColumnCount() - 1)
			{
				return TRUE;
			}
		}
	}

	// else can't delete
	return FALSE;
}

BOOL CInputListCtrl::DeleteSelectedCell()
{
	if (GetCurSel() != - 1)
	{
		// don't delete it if its the topleft item
		if (m_bAutoAddCols && GetCurSel() == 0 && m_bAutoAddRows && m_nCurCol == 0)
		{
			return FALSE;
		}

		// else delete the cell if its not the 'add row' prompt
		else if (m_bAutoAddRows && m_nCurCol == 0)
		{
			if (GetCurSel() < GetItemCount() - 1)
			{
				DeleteItem(GetCurSel());
			}
			else
			{
				return FALSE;
			}
		}
		// else delete the cell if its not the 'add col' prompt
		else if (m_bAutoAddCols && GetCurSel() == 0)
		{
			if (m_nCurCol < GetColumnCount() - 1)
			{
				DeleteColumn(m_nCurCol);
				m_nCurCol = -1;
			}
			else
			{
				return FALSE;
			}
		}
		else // clear the field
		{
			SetItemText(GetCurSel(), m_nCurCol, _T(""));
		}

		return TRUE;
	}
	// else do nothing
	return FALSE;
}

BOOL CInputListCtrl::CanEditSelectedCell() const
{
	int nNumCols, nNumRows;

	// if readonly or disabled then no
	if (IsReadOnly() || !IsWindowEnabled())
	{
		return FALSE;
	}

	if (GetCurSel() != -1 && m_nCurCol != -1)
	{
		nNumRows = GetItemCount();
		nNumCols = GetColumnCount();

		// don't edit it:
		//
		// if editing is disabled in the current column
		if (IsColumnEditingDisabled(m_nCurCol))
		{
			return FALSE;
		}

		// or if its the last row and not the first column when autoadding rows
		if (m_bAutoAddRows && GetCurSel() == nNumRows - 1 && m_nCurCol != 0)
		{
			return FALSE;
		}

		// or if its the last col and not the first row when autoadding cols
		if (m_bAutoAddCols && m_nCurCol == nNumCols - 1 && GetCurSel() != 0)
		{
			return FALSE;
		}

		// or if its the topleft item when auto adding rows AND cols
		if (m_bAutoAddCols && m_bAutoAddRows && GetCurSel() == 0 && m_nCurCol == 0)
		{
			return FALSE;
		}

		// of if its the bottom right item when auto adding rows AND cols
		if (m_bAutoAddCols && m_bAutoAddRows &&
			GetCurSel() == nNumRows - 1 && m_nCurCol == nNumCols - 1)
		{
			return FALSE;
		}

		// go ahead
		return TRUE;
	}

	// can't edit
	return FALSE;
}

void CInputListCtrl::EditSelectedCell()
{
	EditCell(GetCurSel(), m_nCurCol);
}

void CInputListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	// ignore if WE are pNewWnd
	if (pNewWnd == this)
	{
		return;
	}

	// if we're not editing then clear the current selection
	if (!IsEditing())
	{
		m_nItemLastSelected = -1;
		m_nColLastSelected = -1;
	}

	CRect rItem;
	GetItemRect(GetCurSel(), rItem, LVIR_BOUNDS);
	InvalidateRect(rItem, FALSE);

	CEnListCtrl::OnKillFocus(pNewWnd);
}

BOOL CInputListCtrl::SetCellText(int nRow, int nCol, CString sText)
{
	ASSERT(m_hWnd);
	ASSERT((m_bAutoAddRows && nRow >= 0 && nRow < GetItemCount() - 1) ||
		(!m_bAutoAddRows && nRow >= 0 && nRow < GetItemCount()));
	ASSERT((m_bAutoAddCols && nCol >= 0 && nCol < GetColumnCount() - 1) ||
		(!m_bAutoAddCols && nCol >= 0 && nCol < GetColumnCount()));

	// only allow text setting if within valid range and if user is
	// not trying to change prompt string if auto adding
	if (((m_bAutoAddRows && nRow >= 0 && nRow < GetItemCount() - 1) ||
		(!m_bAutoAddRows && nRow >= 0 && nRow < GetItemCount())) &&
		((m_bAutoAddCols && nCol >= 0 && nCol < GetColumnCount() - 1) ||
		(!m_bAutoAddCols && nCol >= 0 && nCol < GetColumnCount())))
	{
		SetItemText(nRow, nCol, sText);
		return TRUE;
	}

	return FALSE;
}

BOOL CInputListCtrl::DeleteAllItems(BOOL bIncludeCols)
{
	CString sText;
	BOOL bRes;
	int nNumCols;

	// delete all
	bRes = CEnListCtrl::DeleteAllItems();

	nNumCols = GetColumnCount();

	// delete columns if necessary
	if (bIncludeCols)
	{
		while (DeleteColumn(0));
	}

	if (!bRes || (!m_bAutoAddRows && !m_bAutoAddCols))
	{
		return bRes;
	}

	// if auto adding re-add prompts as appropriate
	nNumCols = GetColumnCount();

	// rows
	if (m_bAutoAddRows)
	{
		// ensure there is at least one column
		if (nNumCols == 0)
		{
			InsertColumn(0, _T(""), LVCFMT_LEFT, m_nAutoColWidth);
		}

		int nIndex = InsertItem(0, m_sAutoRowPrompt, -1);
		SetItemData(nIndex, PROMPT);
	}

	// cols
	nNumCols = GetColumnCount();

	if (m_bAutoAddCols)
	{
		// however, ensure there is at least one column
		if (nNumCols == 0 || m_bAutoAddRows && nNumCols == 1)
		{
			InsertColumn(nNumCols, _T(""), LVCFMT_LEFT, m_nAutoColWidth);
			nNumCols++;
		}

		InsertItem(0, _T(""));
		SetItemText(0, nNumCols - 1, m_sAutoColPrompt);
	}

	m_nCurCol = 0;
	Invalidate();

	// cols
	nNumCols = GetColumnCount();

	return TRUE;
}

void CInputListCtrl::SetCurSel(int nRow, int nCol, BOOL bNotifyParent)
{
	ASSERT(m_hWnd);
	ASSERT(nRow >= -1 && nRow < GetItemCount() && nCol >= -1 && nCol < GetColumnCount());

	// only allow selection setting if within valid range
	if (nRow < 0 || nRow >= GetItemCount() || nCol < 0 || nCol >= GetColumnCount())
	{
		return;
	}

	// don't update if nothing's changed
	if (nCol == m_nCurCol && nRow == CEnListCtrl::GetCurSel())
	{
		return;
	}

	CRect rItem;
	int nSel = GetCurSel();
	RedrawItems(nSel, nSel);

	m_nCurCol = nCol;
	CEnListCtrl::SetCurSel(nRow, bNotifyParent == TRUE);

	m_nColLastSelected = nCol;
	m_nItemLastSelected = nRow;
	RedrawItems(nRow, nRow);
}

int CInputListCtrl::GetLastEdit(int* pRow, int* pCol)
{
	if (pRow)
	{
		*pRow = m_nLastEditRow;
	}

	if (pCol)
	{
		*pCol = m_nLastEditCol;
	}

	return m_nLastEdit;
}

void CInputListCtrl::GetCurSel(int& nRow, int& nCol) const
{
	nRow = GetCurSel();
	nCol = m_nCurCol;
}

int CInputListCtrl::InsertRow(CString sRowText, int nRow, int nImage)
{
	if (m_bAutoAddRows && nRow == GetItemCount())
	{
		nRow--;   // add before prompt
	}

	if (nImage != -1)
	{
		return InsertItem(nRow, sRowText, nImage);
	}

	// else
	return InsertItem(nRow, sRowText);
}

int CInputListCtrl::AddRow(CString sRowText, int nImage)
{
	return InsertRow(sRowText, GetItemCount(), nImage);
}

int CInputListCtrl::AddCol(CString sColText, int nWidth)
{
	int nCol;

	nCol = GetColumnCount();

	if (m_bAutoAddCols)
	{
		nCol--;   // add before prompt
	}

	nCol = InsertColumn(nCol, sColText, LVCFMT_LEFT, nWidth == -1 ? m_nAutoColWidth : nWidth);
	SetItemText(0, nCol, sColText);

	return nCol;
}


void CInputListCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CEnListCtrl::OnSetFocus(pOldWnd);

	if (GetCurSel() == -1)
	{
		if (GetItemCount())
		{
			SetCurSel(0, 0);
			SetItemFocus(0, TRUE); // set focus
		}
	}
	else
	{
		SetItemFocus(GetCurSel(), TRUE);   // reset focus
	}
}

void CInputListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// if we're editing then quit editing
	if (m_nEditItem != -1 || m_nEditCol != -1)
	{
		GetEditControl()->Hide();
		OnCancelEdit();
	}

	CEnListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CInputListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// if we're editing then quit editing
	if (m_nEditItem != -1 && m_nEditCol != -1)
	{
		GetEditControl()->Hide();
		OnCancelEdit();
	}

	CEnListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

CRect CInputListCtrl::ScrollCellIntoView(int nRow, int nCol)
{
	CRect rClient, rIntersect, rCell;
	CPoint ptItem;
	CSize sizeScroll;

	// do nothing if invalid coords
	if (nRow == -1 || nCol == -1)
	{
		return CRect(0, 0, 0, 0);
	}

	// note: amount to scroll is relative to current position
	// so we need to determine whether the entire cell is visible and
	// if not, how much to scroll to bring it into view.
	GetClientRect(rClient);
	GetCellRect(nRow, nCol, rCell);

	// if the cell is completely contained within the client area
	// we don't need to scroll
	rIntersect.IntersectRect(rClient, rCell);

	if (rIntersect != rCell)
	{
		// calc how much to scroll
		if (rCell.left <= 0)
		{
			sizeScroll.cx = rCell.left;
		}
		else if (rCell.right >= rClient.right)
		{
			sizeScroll.cx = min(rCell.left, rCell.right - rClient.right);
		}
		else
		{
			sizeScroll.cx = 0;
		}

		if (rCell.top < 0)
		{
			sizeScroll.cy = rCell.top;
		}
		else if (rCell.bottom > rClient.bottom)
		{
			sizeScroll.cy = rCell.bottom - rClient.bottom;
		}
		else
		{
			sizeScroll.cy = 0;
		}

		// now, listctrls only scroll by whole lines so..
		if (sizeScroll.cy > 0 && sizeScroll.cy % rCell.Height() != 0)
		{
			sizeScroll.cy += rCell.Height();
		}

		// do the scroll
		if (sizeScroll != CSize(0, 0))
		{
			Scroll(sizeScroll);
		}

		// calc final cell position
		GetCellRect(nRow, nCol, rCell);
	}

	return rCell;
}

void CInputListCtrl::GetCellEditRect(int nRow, int nCol, CRect& rCell)
{
	switch (GetColumnType(nCol))
	{
	case TEXT:
	case BROWSE:
		CEnListCtrl::GetCellRect(nRow, nCol, rCell);

		rCell.right++;

		// move top edge up one pixel so that it looks right
		// but not of the first row else it gets clipped
		// by the window border
		if (nRow > 0)
		{
			rCell.top--;
		}
		break;

	case DROPLIST:
		CEnListCtrl::GetCellRect(nRow, nCol, rCell);

		rCell.OffsetRect(1, -1);
		rCell.left--;
		break;
	}
}

void CInputListCtrl::SetView(int nView)
{
	// this control only supports LVS_REPORT view
	ASSERT(nView == LVS_REPORT);
	UNREFERENCED_PARAMETER(nView);

	CEnListCtrl::SetView(LVS_REPORT);
}

void CInputListCtrl::PreSubclassWindow()
{
	CEnListCtrl::PreSubclassWindow();

	// make sure we clip out the edit field
	ModifyStyle(0, WS_CLIPCHILDREN);
}

int CInputListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEnListCtrl::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	return 0;
}

void CInputListCtrl::HideControl(CWnd& ctrl)
{
	if (ctrl.IsWindowVisible())
	{
		ctrl.ShowWindow(SW_HIDE);
		ctrl.EnableWindow(FALSE);
	}
}

void CInputListCtrl::ShowControl(CWnd& ctrl, int nRow, int nCol)
{
	PrepareControl(ctrl, nRow, nCol);

	CRect rCell;
	GetCellEditRect(nRow, nCol, rCell);

	if (ctrl.IsKindOf(RUNTIME_CLASS(CComboBox)))
	{
		rCell.bottom += 200;
	}

	ctrl.MoveWindow(rCell);
	ctrl.EnableWindow(TRUE);
	ctrl.ShowWindow(SW_SHOW);
	ctrl.SetFocus();

	if (ctrl.IsKindOf(RUNTIME_CLASS(CComboBox)))
	{
		CComboBox* pCombo = (CComboBox*)&ctrl;
		pCombo->ShowDropDown(TRUE);
	}
	else //if (ctrl.IsKindOf(RUNTIME_CLASS(CDateTimeCtrl)))
	{
		//CDateTimeCtrl* pDateTime = (CDateTimeCtrl*)&ctrl;
	}
}

void CInputListCtrl::EndEdit()
{
	int nRow, nCol;
	GetCurSel(nRow, nCol);

	if (nRow == -1 || nCol == -1)
	{
		return;
	}

	// if any editing control is visible, just shift the
	// focus back to the list to end the edit
	if (IsEditing())
	{
		SetFocus();
	}
}

void CInputListCtrl::CreateControl(CWnd& ctrl, UINT nID, BOOL bSort)
{
	DWORD dwStyle = WS_CHILD;

	if (ctrl.IsKindOf(RUNTIME_CLASS(CComboBox)))
	{
		dwStyle |= CBS_DROPDOWNLIST | WS_VSCROLL | CBS_AUTOHSCROLL;

		if (bSort)
		{
			dwStyle |= CBS_SORT;
		}

		CComboBox* pCombo = (CComboBox*)&ctrl;
		VERIFY(pCombo->Create(dwStyle, CRect(0, 0, 0, 0), this, nID));
	}
	else if (ctrl.IsKindOf(RUNTIME_CLASS(CEdit)))
	{
		CEdit* pEdit = (CEdit*)&ctrl;
		VERIFY(pEdit->Create(dwStyle, CRect(0, 0, 0, 0), this, nID));

		pEdit->ModifyStyleEx(0, WS_EX_CLIENTEDGE, 0);
	}
	else //if (ctrl.IsKindOf(RUNTIME_CLASS(CDateTimeCtrl)))
	{
		CDateTimeCtrl* pDateTime = (CDateTimeCtrl*)&ctrl;
		VERIFY(pDateTime->Create(dwStyle, CRect(0, 0, 0, 0), this, nID));
	}

	ctrl.SetFont(GetFont()); // set font to parents
}

CPopupEditCtrl* CInputListCtrl::GetEditControl()
{
	ASSERT(m_hWnd);

	if (!m_hWnd)
	{
		return NULL;
	}

	if (!m_editBox.m_hWnd)
	{
		m_editBox.Create(this, IDC_EDITBOX, WS_CHILD | WS_BORDER);
	}

	return &m_editBox;
}

void CInputListCtrl::OnEndEdit(UINT /*uIDCtrl*/, int* pResult)
{
	CString sText, sMessage;
	int nLastRow, nLastCol, nIndex, nNumCols, nNumRows;
	BOOL bNotifyParent = FALSE;
	BOOL bItemDeleted = FALSE;

	nLastRow = GetItemCount() - 1;
	nLastCol = GetColumnCount() - 1;
	*pResult = NOTVALID;

	// update item text that was edited
	// if we're auto adding and it was the last row don't allow empty strings
	// if autoaddrows is enabled and we've just edited the last row
	// re-add the prompt item
	GetEditControl()->GetWindowText(sText);

	if (m_bAutoAddRows && m_nEditItem == nLastRow && m_nEditCol == 0)
	{
		// if its a duplicate and notifying, so notify user
		if (!sText.IsEmpty())
		{
			if (IsDuplicateRow(sText, nLastRow) && !m_bAllowDuplication)
			{
				if (m_bNotifyDuplicates)
				{
					sMessage.Format(_T("The item '%s' already exists in the list"), sText);
					AfxMessageBox(sMessage, MB_OK | MB_ICONEXCLAMATION);
					SetFocus();
				}

				EditSelectedCell();
				return;
			}
			else
			{
				SetItemText(m_nEditItem, m_nEditCol, sText);
				SetItemData(m_nEditItem, 0);

				// reinsert prompt
				nIndex = InsertItem(nLastRow + 1, m_sAutoRowPrompt, -1);
				SetItemData(nIndex, PROMPT); // for sorting

				// reset text in other cols
				nNumCols = nLastCol;
				while (nNumCols > 0)
				{
					SetItemText(nIndex, nNumCols, _T(""));
					nNumCols--;
				}

				bNotifyParent = TRUE;
				*pResult = ADDITEM;
			}
		}
	}
	else if (m_bAutoAddCols && m_nEditItem == 0 && m_nEditCol == nLastCol)
	{
		if (!sText.IsEmpty())
		{
			if (IsDuplicateCol(sText, nLastCol) && !m_bAllowDuplication)
			{
				if (m_bNotifyDuplicates)
				{
					sMessage.Format(_T("The item '%s' already exists in the list"), sText);
					AfxMessageBox(sMessage, MB_OK | MB_ICONEXCLAMATION);
					SetFocus();
				}

				EditSelectedCell();
				return;
			}
			else
			{
				nIndex = InsertColumn(GetColumnCount(), _T(""), LVCFMT_LEFT, m_nAutoColWidth);
				SetItemText(m_nEditItem, m_nEditCol + 1, m_sAutoColPrompt);
				SetItemText(m_nEditItem, m_nEditCol, sText);

				// reset text in other rows
				nNumRows = nLastRow;
				while (nNumRows > 0)
				{
					SetItemText(nNumRows, nIndex, _T(""));
					nNumRows--;
				}

				bNotifyParent = TRUE;
				*pResult = ADDITEM;
			}
		}
	}
	else // just a normal column and row but we still need to check for duplication
	{
		if (!sText.IsEmpty())
		{
			if ((m_bAutoAddRows && m_nEditCol == 0 && IsDuplicateRow(sText, m_nEditItem) && !m_bAllowDuplication) ||
				(m_bAutoAddCols && m_nEditItem == 0 && IsDuplicateCol(sText, m_nEditCol) && !m_bAllowDuplication))
			{
				if (m_bNotifyDuplicates)
				{
					sMessage.Format(_T("The item '%s' already exists in the list"), sText);
					AfxMessageBox(sMessage, MB_OK | MB_ICONEXCLAMATION);
					SetFocus();
				}

				EditSelectedCell();
				return;
			}
			else
			{
				SetItemText(m_nEditItem, m_nEditCol, sText);
				bNotifyParent = TRUE;
				*pResult = EDITITEM;
			}
		}
		else // remove item
		{
			SetItemText(m_nEditItem, m_nEditCol, _T(""));
			bItemDeleted = TRUE;
			bNotifyParent = FALSE;
			DeleteSelectedCell();
		}
	}

	if (!bItemDeleted)
	{
		int nRow = (*pResult == ADDITEM) ? m_nEditItem + 1 : m_nEditItem;
		m_nCurCol = m_nEditCol;
		SetCurSel(nRow, m_nCurCol);

		m_nLastEditRow = m_nEditItem;
		m_nLastEditCol = m_nEditCol;
		m_nLastEdit = *pResult;
	}
	else
	{
		m_nLastEditRow = -1;
		m_nLastEditCol = -1;
		m_nLastEdit = NOTVALID;
	}

	m_nEditCol = m_nEditItem = -1;

	// reset redrawing
	SetRedraw(TRUE);
	Invalidate(FALSE);
	UpdateWindow();

	// notify parent
	if (bNotifyParent)
	{
		LV_DISPINFO lvd;

		lvd.item.iItem = m_nLastEditRow;
		lvd.item.iSubItem = m_nLastEditCol;
		lvd.item.mask = LVIF_TEXT;
		lvd.item.pszText = (LPTSTR)(LPCTSTR)sText;
		lvd.item.cchTextMax = sText.GetLength();

		lvd.hdr.hwndFrom = m_hWnd;
		lvd.hdr.idFrom = GetDlgCtrlID();
		lvd.hdr.code = LVN_ENDLABELEDIT;

		GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&lvd);
	}
}

void CInputListCtrl::OnCancelEdit()
{
	CString sText;

	// note: edit ctrl auto hides itself
	//GetEditControl()->ShowWindow(SW_HIDE);

	m_nCurCol = m_nEditCol;
	SetCurSel(m_nEditItem, m_nEditCol);
	m_nEditItem = m_nEditCol = -1;

	// reset redrawing
	SetRedraw(TRUE);
	Invalidate();
}

void CInputListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd* pFocus;

	// note: if we are currently editing then don't pass on
	// else the edit box loses the focus
	pFocus = GetFocus();

	if (m_editBox.m_hWnd && (pFocus == &m_editBox || m_editBox.IsWindowVisible()))
	{
		return;
	}

	CEnListCtrl::OnLButtonUp(nFlags, point);
}

BOOL CInputListCtrl::IsPrompt(int nItem, int nCol)
{
	return ((m_bAutoAddRows && nItem == GetItemCount() - 1 && nCol == 0) ||
		(m_bAutoAddCols && nItem == 0 && nCol == GetColumnCount() - 1));
}

COLORREF CInputListCtrl::GetItemTextColor(int nItem, int nCol, BOOL bSelected, BOOL /*bDropHighlighted*/, BOOL bWndFocus)
{
	BOOL bIsPrompt = IsPrompt(nItem, nCol);

	// setup colors
	if (bSelected && nCol == m_nCurCol && nItem == GetCurSel())
	{
		// if focused then draw item in focused colors
		if (bWndFocus)
		{
			return ::GetSysColor(COLOR_HIGHLIGHTTEXT);
		}

		// else if not focused then draw selection else
		// draw in column colors as below unless we're the prompt and
		// readonly - then draw in back color (ie hide it)
		else if (bSelected)
		{
			return ::GetSysColor(COLOR_WINDOWTEXT);
		}

		else if (bIsPrompt)
		{
			return ::GetSysColor(COLOR_3DSHADOW);
		}

		else
		{
			return GetColumnColor(nCol);
		}
	}
	else // not selected so draw in specified column colors unless its
		// the prompt in which case draw in COLOR_3DSHADOW unless we're
		// readonly else draw in back color (ie hide it)
	{
		if (bIsPrompt)
		{
			return ::GetSysColor(COLOR_3DSHADOW);
		}
		else
		{
			return GetColumnColor(nCol);
		}
	}
}

COLORREF CInputListCtrl::GetItemBackColor(int nItem, BOOL bSelected, BOOL bDropHighlighted, BOOL bWndFocus)
{
	BOOL bIsPrompt = IsPrompt(nItem, 0);

	if (bIsPrompt)
	{
		return CEnListCtrl::GetItemBackColor(0, bSelected, bDropHighlighted, bWndFocus);
	}
	else
	{
		return CEnListCtrl::GetItemBackColor(nItem, bSelected, bDropHighlighted, bWndFocus);
	}
}

void CInputListCtrl::SetColumnType(int nCol, int nType)
{
	CColumnData2* pData = (CColumnData2*)CreateColumnData(nCol);

	if (pData)
	{
		pData->nType = nType;
	}
}

int CInputListCtrl::GetColumnType(int nCol) const
{
	const CColumnData2* pData = GetColumnData(nCol);

	if (pData)
	{
		switch (pData->nType)
		{
		case DROPLIST:
		case BROWSE:
		case TEXT:
			break;

		default:
			break;
		}

		return pData->nType;
	}

	// else
	return TEXT;
}

int CInputListCtrl::CompareItems(DWORD dwItemData1, DWORD dwItemData2, int nSortColumn)
{
	// if one of the items is the prompt then this is always last
	if (dwItemData1 == PROMPT || dwItemData2 == PROMPT)
	{
		return 0;   // don't move item
	}

	// else
	return CEnListCtrl::CompareItems(dwItemData1, dwItemData2, nSortColumn);
}

// base class overrides
int CInputListCtrl::SetCurSel(int nIndex, bool bNotifyParent)
{
	SetCurSel(nIndex, m_nCurCol, (BOOL)bNotifyParent);

	return GetCurSel();
}

int CInputListCtrl::GetCurSel() const
{
	return CEnListCtrl::GetCurSel();
}

void CInputListCtrl::SetEditMask(LPCTSTR szMask, DWORD dwFlags)
{
	m_editBox.SetMask(szMask, dwFlags);
}

void CInputListCtrl::SetReadOnly(BOOL bReadOnly)
{
	if (m_bReadOnly != bReadOnly && (m_bAutoAddRows || m_bAutoAddCols))
	{
		Invalidate(FALSE);   // to redraw the prompt string
	}

	CEnListCtrl::SetReadOnly(bReadOnly);
}
