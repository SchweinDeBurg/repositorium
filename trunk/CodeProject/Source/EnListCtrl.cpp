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
// - taken out from the original ToDoList package for better sharing
//*****************************************************************************

// EnListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "EnListCtrl.h" 
#include "EnHeaderCtrl.h"
#include "DlgUnits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnListCtrl

IMPLEMENT_DYNAMIC(CEnListCtrl, CListCtrl)

#define IDC_HEADERCTRL 10001
#define WM_SHOWPOPUPMENU (WM_APP+1001)
#define ID_TIMER_HEADERPOS 1
const int MAX_HEADING_SIZE = 100;

CEnListCtrl::CEnListCtrl()
{
	m_bVertGrid = m_bHorzGrid = FALSE;
	m_mapColumnData.RemoveAll();
	m_nCurView = 0;
	m_bLastColStretchy = FALSE;
	m_bFirstColStretchy = FALSE;
	m_nItemHeight = 20;//16;
	m_bReadOnly = FALSE;
	m_nItemDropHilite = -1;
	m_bDropHiliteItemSelected = FALSE;
	m_bContextPopupEnabled = FALSE;
	m_bUserSelChange = FALSE;
	m_bSortingEnabled = TRUE;
	m_nSortColumn = 0;
	m_bSortAscending = TRUE;
	m_bInitColumns = FALSE;
	m_bAlternateRowColoring = FALSE;
}

CEnListCtrl::~CEnListCtrl()
{
	DeleteAllColumnData();
}


BEGIN_MESSAGE_MAP(CEnListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CEnListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYUP()
	ON_WM_KILLFOCUS()
	ON_WM_SIZE()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_WM_GETDLGCODE()
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnListCtrl message handlers


void CEnListCtrl::OnPaint() 
{
	// if no items or in report mode draw to back buffer then blt
	if (GetItemCount() && GetView() != LVS_REPORT)
		Default();

	else
	{
		CPaintDC cleanup(this);

		CHeaderCtrl* pHeader = GetHeader();
		CDC& paintdc = *GetDC();

		CDC dc;
		dc.CreateCompatibleDC(&paintdc);
		
		CRect rClient;
		GetClientRect( &rClient );
		
		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(&paintdc, rClient.right, rClient.bottom);
		CBitmap *pOldBitmap = dc.SelectObject(& bitmap);
			
		if (pHeader && m_nCurView == LVS_REPORT && !(GetStyle() & LVS_NOCOLUMNHEADER))
		{
			CRect rHeader;
			pHeader->GetWindowRect(&rHeader);
			ScreenToClient(rHeader);
			dc.ExcludeClipRect(rHeader);
			rClient.top = rHeader.bottom;
		}

		// fill with back color
		COLORREF crBack = GetItemBackColor(0, FALSE, FALSE, FALSE);

		dc.FillSolidRect(rClient, crBack);

		// default drawing
		CListCtrl::DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, 0);
		
		// do empty text if nec
		if (GetItemCount() == 0)
		{
			CString sText = GetNoItemsText(); // virtual call

			if (!sText.IsEmpty())
			{
				sText.TrimRight();
				sText.TrimLeft();

				if (sText[0] != _T('('))
					sText = _T("(") + sText;

				if (sText[sText.GetLength() - 1] != _T(')'))
					sText += _T(")");
			
				dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
				dc.SetBkColor(crBack);
				dc.SelectStockObject( ANSI_VAR_FONT );

				rClient.top += 10;
				dc.DrawText( sText, rClient, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX/* | DT_NOCLIP */);
				rClient.top -= 10; // reset
			}
		}

		paintdc.BitBlt(0, rClient.top, rClient.right, rClient.bottom, 
			&dc, 0, rClient.top, SRCCOPY);

		dc.SelectObject(pOldBitmap);
		ReleaseDC(&paintdc);
	}

	// Do not call CListView::OnPaint() for painting messages
}

/*
BOOL CEnListCtrl::ESetRegistrySection(CString sSection)
{
	return ERegistry::ESetRegistrySection(sSection);
}

BOOL CEnListCtrl::ERegStoreState(CRegKey& regKey)
{
	int nNumCols, nCol;
	CString sColKey;
	CRect rItem;
	DWORD dwTopLeft;

	if (!ERegistry::ERegStoreState(regKey))
		return FALSE;

	// save current column widths
	nNumCols = GetColumnCount();

	for (nCol = 0; nCol < nNumCols; nCol++)
	{
		sColKey.Format("ColumnWidth%d", nCol);
		EGetRegKey().Write(sColKey, (DWORD)GetColumnWidth(nCol));
	}

	// save view
	EGetRegKey().Write("View", (DWORD)GetView());

	// save scrolled pos
	if (GetItemRect(0, rItem, LVIR_BOUNDS))
	{
		if ((rItem.top % rItem.Height()) != 0)
			rItem.top -= rItem.Height();
	}
	
	dwTopLeft = MAKELONG(rItem.left, rItem.top - 10);
	EGetRegKey().Write("TopLeft", dwTopLeft);

	// sort state
	EGetRegKey().Write("SortColumn", (DWORD)GetSortColumn());
	EGetRegKey().Write("SortAscending", (DWORD)GetSortAscending());

	return TRUE;
}

BOOL CEnListCtrl::ERegRestoreState(CRegKey& regKey)
{
	int nNumCols, nCol, nWidth, nView = LVS_REPORT, nSortColumn = 0;
	BOOL bSortAscending = TRUE;
	CString sColKey;
	CPoint ptTopLeft;
	DWORD dwTopLeft;

	if (!ERegistry::ERegRestoreState(regKey))
		return FALSE;

	SetRedraw(FALSE);

	// restore column widths
	nNumCols = GetColumnCount();

	for (nCol = 0; nCol < nNumCols; nCol++)
	{
		nWidth = 0;
		sColKey.Format("ColumnWidth%d", nCol);
		EGetRegKey().Read(sColKey, (DWORD&)nWidth);

		if (nWidth > 0)
			SetColumnWidth(nCol, nWidth);
	}

	// restore view
	EGetRegKey().Read("View", (DWORD&)nView);
	SetView(nView);

	// sort state
	EGetRegKey().Read("SortColumn", (DWORD&)nSortColumn);
	EGetRegKey().Read("SortAscending", (DWORD&)bSortAscending);
	SetSortColumn(nSortColumn, FALSE);
	SetSortAscending(bSortAscending);
	Sort();

	// restore scrolled pos
	EGetRegKey().Read("TopLeft", dwTopLeft);
	ptTopLeft = CPoint(dwTopLeft);
	Scroll(CSize(0, max(0, -ptTopLeft.y)));

	SetRedraw(TRUE);
	Invalidate();

	return TRUE;
}

*/
void CEnListCtrl::DeleteAllColumnData()
{
	int nCol;
	CColumnData* pData;
	POSITION pos = m_mapColumnData.GetStartPosition();

	while (pos)
	{
		m_mapColumnData.GetNextAssoc(pos, nCol, pData);
		delete pData;
	}
}

CColumnData* CEnListCtrl::CreateColumnData(int nCol)
{
	CColumnData* pData = NULL;

	if (!m_mapColumnData.Lookup(nCol, pData)) // see if its already added
	{
		pData = GetNewColumnData();
		
		if (pData)
			m_mapColumnData.SetAt(nCol, pData);

		// set the column format to the most appropriate
		LV_COLUMN lvc;
		lvc.mask = LVCF_FMT;
		
		if (GetColumn(nCol, &lvc))
		{
			if ((lvc.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
				pData->nFormat = ES_START;
		}
	}

	return pData;
}

const CColumnData* CEnListCtrl::GetColumnData(int nCol) const
{
	CColumnData* pData = NULL;
	m_mapColumnData.Lookup(nCol, pData);

	return pData;
}

CEnHeaderCtrl* CEnListCtrl::GetHeader()
{
	BOOL bRes = TRUE;
	ASSERT (m_hWnd);

	// subclass header as CEnHeaderCtrl
	if (!m_header.GetSafeHwnd())
		bRes = m_header.SubclassDlgItem(0, this);

	if (!bRes)
		AfxMessageBox(_T(""));

	return bRes ? &m_header : NULL;
}

const CEnHeaderCtrl* CEnListCtrl::GetHeader() const
{
	if (!m_header.GetSafeHwnd())
		AfxMessageBox(_T(""));

	return m_header.GetSafeHwnd() ? &m_header : NULL;
}

int CEnListCtrl::GetFirstSel() const
{
	return GetNextItem(-1, LVNI_SELECTED);
}

int CEnListCtrl::GetNextSel(int nPrevSel) const
{
	ASSERT (nPrevSel >= 0 && nPrevSel < GetItemCount());

	return GetNextItem(nPrevSel, LVNI_SELECTED);
}

int CEnListCtrl::GetFocusedItem() const
{
	return GetNextItem(-1, LVNI_FOCUSED);
}

int CEnListCtrl::GetLastSel() const
{
	int nIndexSel, nLastSel;
	
	nIndexSel = GetFirstSel();

	if (nIndexSel == -1)
		return -1;

	do
	{
		nLastSel = nIndexSel;
	}
	while((nIndexSel = GetNextSel(nIndexSel)) != -1);
	
	return nLastSel;
}

int CEnListCtrl::GetPrevSel(int nNextSel) const
{
	ASSERT (nNextSel >= 0 && nNextSel <= GetItemCount());

	int nIndexSel, nPrevSel;
	
	nIndexSel = GetFirstSel();

	if (nIndexSel == -1)
		return -1;

	do
	{
		nPrevSel = nIndexSel;
	}
	while((nIndexSel = GetNextSel(nIndexSel)) != -1 &&
			nIndexSel < nNextSel);
	
	return nPrevSel;
}

int CEnListCtrl::GetCurSel() const
{
	return GetNextItem(-1, LVNI_SELECTED);
}

void CEnListCtrl::SetMulSel(int nIndexStart, int nIndexEnd, BOOL bSelect, BOOL bNotifyParent)
{
	ASSERT (nIndexStart >= 0 && nIndexStart < GetItemCount() &&
			nIndexEnd >= 0 && nIndexEnd < GetItemCount());
	
	int nIndex;

	for (nIndex = nIndexStart; nIndex <= nIndexEnd; nIndex++)
		SetItemState(nIndex, bSelect ? LVIS_SELECTED : 0, LVIS_SELECTED);

	// if we have the focus then update parent 
	if (GetFocus() == this && bNotifyParent)
		NotifySelChange();
}

void CEnListCtrl::SetItemFocus(int nIndex, BOOL bFocused)
{
//	ASSERT (nIndex >= 0 && nIndex < GetItemCount());
	
	if (!(nIndex >= 0 && nIndex < GetItemCount()))
		return;
	
	// set focus
	SetItemState(nIndex, bFocused ? LVIS_FOCUSED : 0, LVIS_FOCUSED);
}

void CEnListCtrl::SelectAll()
{
	if (GetItemCount())
		SetMulSel(0, GetItemCount() - 1, TRUE);

	// if we have the focus then update parent 
	if (GetFocus() == this)
		NotifySelChange();
}

void CEnListCtrl::ClearAll()
{
	if (GetItemCount())
		SetMulSel(0, GetItemCount() - 1, FALSE);

	// if we have the focus then update parent 
	if (GetFocus() == this)
		NotifySelChange();
}

int CEnListCtrl::SetCurSel(int nIndex, bool bNotifyParent)
{
	ASSERT (nIndex >= -1 && nIndex < GetItemCount());
	
	int nCurSel, nRes;
	UINT nState, nMask;
	CRect rItem;
	
	nState = nMask = LVIS_SELECTED | LVIS_FOCUSED;

	nCurSel = GetFirstSel();

	if (nCurSel != -1)
	{
		SetItemState(nCurSel, 0, nMask);
		Update(nCurSel);
	}
	SetItemState(nIndex, nState, nMask);
	nRes = Update(nIndex);

	// if we have the focus then update parent 
	if (GetFocus() == this && bNotifyParent)
		NotifySelChange();

	return nRes;
}

int CEnListCtrl::GetCountPerPage() const
{
	CRect rList, rItem;

	if (GetItemCount() == 0)
		return 0;

	GetClientRect(&rList);
	GetItemRect(0, &rItem, LVIR_BOUNDS);
	
	ASSERT (!rList.IsRectEmpty() && !rItem.IsRectEmpty());

	return	rList.Height() / rItem.Height();
}

void CEnListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC;
	CRect rText, rItem, rClient, rHeader;
	COLORREF crOldText, crOldBack;
	CSize sizeText;
	LV_COLUMN lvc;
//	CPen* pOldPen;
	CImageList* pImageList;
	CImageList* pStateList;
	int nImage = -1;
	BOOL bItemFocused, bListFocused, bSelected, bDropHighlighted, bSelAlways;
	UINT uStyle, uState;
	CSize sizeState(0, 0), sizeImage(0, 0);
	int nIndent = 0;

	// get and prepare device context
	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);//GetDC(); 
	pDC->SelectObject(GetFont());
	pDC->SetROP2(R2_COPYPEN);

	// init helper variables
	int nItem = lpDrawItemStruct->itemID;
	GetItemRect(nItem, rItem, LVIR_BOUNDS);//lpDrawItemStruct->rcItem;
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

	rText = rItem;

	uStyle = GetStyle();
	uState = GetItemState(nItem, LVIS_DROPHILITED | LVIS_SELECTED);
	bDropHighlighted = (uState & LVIS_DROPHILITED);
	bItemFocused = (GetFocusedItem() == nItem);
	bListFocused = (GetFocus() == this);
	bSelected = (uState & LVIS_SELECTED);
	bSelAlways = ((uStyle & LVS_SHOWSELALWAYS) == LVS_SHOWSELALWAYS);
	bSelected = bSelected && (bListFocused || bSelAlways);

	crOldText = pDC->SetTextColor(COLORREF(0)); // this will be overwritten on a per subitem basis
	crOldBack = pDC->SetBkColor(GetItemBackColor(nItem, bSelected, bDropHighlighted, bListFocused));

	// images and indentation
	pImageList = GetImageList(LVSIL_SMALL);

	if (pImageList)
	{
		nImage = GetImageIndex(nItem, 0); 
		ImageList_GetIconSize(pImageList->m_hImageList, (int*)&sizeImage.cx, (int*)&sizeImage.cy);

		nIndent = GetItemIndent(nItem) * sizeImage.cx;

		rText.left += nIndent;
		rItem.left += nIndent;

//		if (pImageList == &s_ilIndent)
//			pImageList = NULL;
	}

	// state
	pStateList = GetImageList(LVSIL_STATE);

	if (pStateList)
		ImageList_GetIconSize(pStateList->m_hImageList, (int*)&sizeState.cx, (int*)&sizeState.cy);

	if (lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT))
	{
		// setup colors and pens
		int nImageStyle = GetImageStyle(bSelected, bDropHighlighted, bListFocused);

		// draw item images if required
		int nImageWidth = 0;

		// make sure there is enough space
		lvc.mask = LVCF_WIDTH | LVCF_FMT;
		int nCol = 0;
		BOOL bRes = GetColumn(nCol, &lvc);
		
		// must paint the background of column 0 before the icons
		if (bRes && (pStateList || pImageList))
			pDC->ExtTextOut(0, rItem.top, ETO_CLIPPED | ETO_OPAQUE, CRect(0, rItem.top, lvc.cx, rItem.bottom), _T(""), NULL);

		// state
		if (pStateList && bRes)
		{
			int nState = (GetItemState(nItem, LVIS_STATEIMAGEMASK) & LVIS_STATEIMAGEMASK);
			nState = nState >> 12;

			if (lvc.cx > sizeState.cx)
				pStateList->Draw(pDC, nState, CPoint(rText.left + 1, rText.top), ILD_TRANSPARENT); 

			nImageWidth = sizeState.cx + 2; // 1 pixel border either side
		}

		// normal
		if (pImageList && bRes && nImage != -1)
		{
			if (lvc.cx > nImageWidth + sizeImage.cx)
				pImageList->Draw(pDC, nImage, CPoint(rText.left + 1 + nImageWidth, rText.top), nImageStyle); 

			nImageWidth += sizeImage.cx + 2; // 1 pixel border either side
		}

		// cycle thru columns formatting and drawing each subitem
		rText.left += nImageWidth;

		while (bRes)
		{
			// save width and format because GetItem overwrites
			// if first column deduct width of image if exists
			int nWidth = (nCol == 0) ? lvc.cx - nImageWidth - nIndent : lvc.cx;
			int nFormat = (lvc.fmt & LVCFMT_JUSTIFYMASK);

			// get next item
			bRes = GetColumn(nCol + 1, &lvc);

			// set pos of text rect
			if (nWidth < 0)
			{
				rText.left += nWidth;
				nWidth = 0;
			}
			rText.right = max(rText.left, rText.left + nWidth);

			// get item text and output
			CEnString sText(GetItemText(nItem, nCol));
			sizeText = sText.FormatDC(pDC, nWidth, GetColumnFormat(nCol));

			// set y pos of first char
			int nYPos = rText.top + (rText.Height() - sizeText.cy) / 2;

			// set x pos of first char
			int nXPos = 0;

			switch (nFormat)
			{
				case LVCFMT_CENTER:
					nXPos = rText.left + (rText.Width() - sizeText.cx) / 2;
					break;

				case LVCFMT_RIGHT:
					nXPos = rText.right - 2 - sizeText.cx;
					break;

				case LVCFMT_LEFT:
				default:
					nXPos = rText.left + 2;
					break;
			}

			// setup font
			CFont* pFont = GetItemFont(nItem, nCol);
			CFont* pOldFont = NULL;

			if (pFont)
				pOldFont = pDC->SelectObject(pFont);

			// setup text colour
			pDC->SetTextColor(GetItemTextColor(nItem, nCol, bSelected, bDropHighlighted, bListFocused));
			pDC->ExtTextOut(nXPos, nYPos, ETO_CLIPPED | ETO_OPAQUE, rText, sText, NULL);

			// reset font
			if (pFont)
				pDC->SelectObject(pOldFont);

			// draw vert grid if required
			if (m_bVertGrid && nCol > 0)
			{
				pDC->MoveTo(rText.left - 1, rItem.top);
				pDC->LineTo(rText.left - 1, rItem.bottom);
			}

			rText.left = rText.right;

			// next column
			nCol++;
		}

		// draw horz grid lines if required
		if (m_bHorzGrid)
		{
			pDC->MoveTo(rClient.left, rItem.bottom - 1);
			pDC->LineTo(rClient.right, rItem.bottom - 1);
		}

		// focus rect: normal method doesn't work because we are focusing whole line
		// note: if we're scrolled to the right the image may not be visible
		if (bItemFocused && bListFocused)
		{
			rItem.left += nImageWidth;
			pDC->DrawFocusRect(rItem);
		}

		pDC->SetTextColor(crOldText);
		pDC->SetBkColor(crOldBack);
//		pDC->SelectObject(pOldPen);
	}

//	ReleaseDC(pDC);
}

COLORREF CEnListCtrl::GetItemTextColor(int /*nItem*/, int nSubItem, 
									   BOOL bSelected, BOOL bDropHighlighted, BOOL bWndFocus) const
{
	if (bSelected)
	{
		if (bWndFocus)
			return ::GetSysColor(COLOR_HIGHLIGHTTEXT); 
		else
			return ::GetSysColor(COLOR_WINDOWTEXT);
	}
	else if (bDropHighlighted)
		return ::GetSysColor(COLOR_HIGHLIGHTTEXT);

	// else
	return GetColumnColor(nSubItem);
}

COLORREF CEnListCtrl::GetItemBackColor(int /*nItem*/, BOOL bSelected, 
									   BOOL bDropHighlighted, BOOL bWndFocus) const
{
	if (bSelected)
	{
		if (bWndFocus)
			return ::GetSysColor(COLOR_HIGHLIGHT);
		else
			return ::GetSysColor(COLOR_BTNFACE);
	} 
	else if (bDropHighlighted)
		return ::GetSysColor(COLOR_HIGHLIGHT);

	// else
	COLORREF crBack = GetTextBkColor(); 

	crBack = (crBack != 0xff000000) ? crBack : ::GetSysColor(COLOR_WINDOW);

//	if (m_bAlternateRowColoring && nItem % 2)
//		crBack = BdeTools::Darker(crBack, 0.95f);

	return crBack;
}

int CEnListCtrl::GetImageStyle(BOOL bSelected, BOOL bDropHighlighted, BOOL bWndFocus) const
{
	int nStyle = ILD_TRANSPARENT;

	if (bSelected && bWndFocus)
		nStyle |= ILD_BLEND25;

	else if (bDropHighlighted)
		nStyle |= ILD_BLEND25;
	
	// else
	return nStyle;
}

void CEnListCtrl::EnableHeaderTracking(BOOL bAllow)
{
	ASSERT (m_hWnd);

	if (m_header.m_hWnd == NULL)
		VERIFY(m_header.SubclassDlgItem(0, this));

	m_header.EnableTracking(bAllow);
}

void CEnListCtrl::EnableTooltipCtrl(BOOL bEnable)
{
	ASSERT (m_hWnd);

	if (m_ttCtrl.m_hWnd == NULL)
		m_ttCtrl.Create(this);
	
	m_bTooltipsEnabled = bEnable;
	m_ttCtrl.Activate(bEnable);
}

BOOL CEnListCtrl::SetTooltipCtrlText(CString sText)
{
	ASSERT (m_hWnd);

	if (sText.IsEmpty())
		return FALSE;

	if (m_ttCtrl.m_hWnd == NULL)
		m_ttCtrl.Create(this);
	
	m_ttCtrl.AddTool(this, sText);

	return TRUE;
}

void CEnListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nSel;
	BOOL bChangedEditLabels = FALSE;

	// if the list is in report mode AND supports label editing then
	// clicking anywhere on the item when its already selected will
	// edit the label of column one which is not what we want. 
	// so we selectively disable it
	if ((nSel = HitTest(point)) != -1 && m_nCurView == LVS_REPORT)
	{
		if (GetStyle() & LVS_EDITLABELS)
		{
			int nCol0Start, nCol0End;
			GetColumnExtents(0, nCol0Start, nCol0End);

			if (GetImageList(LVSIL_SMALL))
				nCol0Start += 19;

			if (GetImageList(LVSIL_STATE))
				nCol0Start += 19;

			if (point.x < nCol0Start || point.x > nCol0End)
			{
				ModifyStyle(LVS_EDITLABELS, 0);
				bChangedEditLabels = TRUE;
			}
		}
	}


	CListCtrl::OnLButtonDown(nFlags, point);

	if (nSel != -1) // user clicked on an item
	{
		// determine whether a sel change has occured
		// and tell our parent if it has
		NotifySelChange();
	}

	if (bChangedEditLabels)
		ModifyStyle(0, LVS_EDITLABELS);
}

void CEnListCtrl::SetColumnColor(int nCol, COLORREF color)
{
	CColumnData* pData = CreateColumnData(nCol);

	if (pData)
	{
		pData->crText = color;

		if (m_hWnd)
			Invalidate(); // redraw
	}
}

COLORREF CEnListCtrl::GetColumnColor(int nCol) const
{
	const CColumnData* pData = GetColumnData(nCol);

	if (pData)
		return pData->crText;

	// else
	COLORREF crBack = GetTextBkColor();
	return crBack != 0x00E0E0E0 ? crBack : ::GetSysColor(COLOR_WINDOWTEXT);
}

void CEnListCtrl::SetColumnFormat(int nCol, int nFormat)
{
	CColumnData* pData = CreateColumnData(nCol);

	if (pData)
	{
		pData->nFormat = nFormat;

		if (m_hWnd)
			Invalidate(); // redraw
	}
}

int CEnListCtrl::GetColumnFormat(int nCol) const
{
	const CColumnData* pData = GetColumnData(nCol);

	if (pData)
		return pData->nFormat;

	// else return most appropriate
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT;
		
	if (GetColumn(nCol, &lvc))
	{
		if ((lvc.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
			return ES_START;
	}

	// else
	return ES_END;
}

void CEnListCtrl::ShowGrid(BOOL bVert, BOOL bHorz)
{
	m_bVertGrid = bVert;
	m_bHorzGrid = bHorz;

	if (m_hWnd)
		Invalidate(); // redraw
}

void CEnListCtrl::SetView(int nView)
{
	CRect rClient;

	ASSERT (nView == LVS_ICON || nView == LVS_LIST ||
			nView == LVS_SMALLICON || nView == LVS_REPORT);

	ASSERT (m_hWnd);
	
	if (!(nView == LVS_ICON || nView == LVS_LIST ||
		nView == LVS_SMALLICON || nView == LVS_REPORT))
		return;

	// Get the current window style. 
	DWORD dwStyle = GetWindowLong(GetSafeHwnd(), GWL_STYLE); 
 
    // Only set the window style if the view bits have changed. 
    if ((dwStyle & LVS_TYPEMASK) != (DWORD)nView) 
		SetWindowLong(GetSafeHwnd(), GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | (DWORD)nView); 

	// if we are in report view then we want to be ownerdraw 
	// so we can take advatage of the special drawing
	if (nView == LVS_REPORT)
	{
		// make ownerdraw
		if (!(dwStyle & LVS_OWNERDRAWFIXED))
			ModifyStyle(0, LVS_OWNERDRAWFIXED); 
	}
	// else we want default drawing
	else
	{
		// make non-ownerdraw
		if (dwStyle & LVS_OWNERDRAWFIXED)
			ModifyStyle(LVS_OWNERDRAWFIXED, 0); 
	}

	m_nCurView = nView;
	Invalidate(FALSE);

	// force a resize to ensure that the column headers are correct
	WINDOWPOS wp = { GetSafeHwnd(), NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER };
	PostMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp); 
}

int CEnListCtrl::GetImageIndex(int nItem, int nSubItem) const
{
	LV_ITEM lvi;

	lvi.mask = LVIF_IMAGE;
	lvi.iItem = nItem;
	lvi.iSubItem = nSubItem;
	
	GetItem(&lvi);

	return lvi.iImage;
}

void CEnListCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL bCtrlDown;

	// check if user did a cut, copy, paste or delete and we're not readonly query parent
	// if parent returns TRUE then do action
	bCtrlDown = ((::GetKeyState(VK_CONTROL) & 0x8000) == 0x8000);
	m_nmhdr.hwndFrom = m_hWnd;
	m_nmhdr.idFrom = GetDlgCtrlID();

	switch (nChar)
	{
		// copy = ctrl + 'c'
		case 'C':
		case 'c':
		{
			if (!bCtrlDown)
				return;

			m_nmhdr.code = LVN_COPY;

			if (!IsReadOnly() && !GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&m_nmhdr))
				Copy();
			break;
		}

		// paste = ctrl + 'v'
		case 'V':
		case 'v':
		{
			if (!bCtrlDown)
				return;

			m_nmhdr.code = LVN_PASTE;

			if (!IsReadOnly() && !GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&m_nmhdr))
				Paste();
			break;
		}

		// cut = ctrl + 'x'
		case 'X':
		case 'x':
		{
			if (!bCtrlDown)
				return;

			m_nmhdr.code = LVN_CUT;

			if (!IsReadOnly() && !GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&m_nmhdr))
				Cut();
			break;
		}

		// delete
		case VK_DELETE:
		{	
			m_nmhdr.code = LVN_DELETE;

			if (!IsReadOnly() && !GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&m_nmhdr))
				Delete();
			break;
		}
	}
	
	// otherwise do default processing 
	CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
	UpdateWindow();
	
	// and determine whether a sel change has occured
	// and tell our parent if it has
	if (GetCurSel() != -1)
		NotifySelChange();
}

UINT CEnListCtrl::OnGetDlgCode()
{
	return CListCtrl::OnGetDlgCode();//DLGC_WANTCHARS;
}

void CEnListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CListCtrl::OnKillFocus(pNewWnd);
}

void CEnListCtrl::SetLastColumnStretchy(BOOL bStretchy)
{
//	ASSERT (m_nCurView == LVS_REPORT);

	m_bLastColStretchy = bStretchy;
	m_bFirstColStretchy = FALSE;

	// invoke a resize to update last column
	if (m_bLastColStretchy && m_nCurView == LVS_REPORT && GetSafeHwnd())
	{
		WINDOWPOS wp = { GetSafeHwnd(), NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER };
		PostMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp); 
	}
}

void CEnListCtrl::SetFirstColumnStretchy(BOOL bStretchy)
{
//	ASSERT (m_nCurView == LVS_REPORT);

	m_bFirstColStretchy = bStretchy;
	m_bLastColStretchy = FALSE;

	// invoke a resize to update first column
	if (m_bFirstColStretchy && m_nCurView == LVS_REPORT && GetSafeHwnd())
	{
		WINDOWPOS wp = { GetSafeHwnd(), NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER };
		PostMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp); 
	}
}

void CEnListCtrl::OnTimer(UINT nIDEvent)
{
	CListCtrl::OnTimer(nIDEvent);
}

void CEnListCtrl::OnSize(UINT nType, int cx, int cy)
{
	static BOOL bInOnSize = FALSE;

	if (bInOnSize)
		return;

	bInOnSize = TRUE;

	CRect rClient, rPrev;
//	HD_LAYOUT   hdLayout;
	WINDOWPOS   wpos;

	GetClientRect(rPrev);

	CListCtrl::OnSize(nType, cx, cy);

	// get header state
/*
	if (m_header.GetSafeHwnd())
	{
		GetClientRect(rClient);
		hdLayout.prc = &rClient;
		hdLayout.pwpos = &wpos;
		Header_Layout(GetHeader()->GetSafeHwnd(), &hdLayout);
	}
*/

	// stretch the appropriate columns if in report mode
	if (m_nCurView == LVS_REPORT)
	{
		int nCol, nNumCols, nColStart = 0, nColEnd = 0;
		GetClientRect(rClient);

		// get the start of the last column
		if (m_bLastColStretchy)
		{
			nNumCols = GetColumnCount();

			for (nCol = 0; nCol < nNumCols - 1; nCol++)
				nColStart += GetColumnWidth(nCol);
		
			// if its reasonably less than the client area adjust else do nothing
			if (nColStart < rClient.Width() - 20 || nNumCols == 1)
				SetColumnWidth(nNumCols - 1, rClient.Width() - nColStart - 2);
		}
		// get the end of the first column
		else if (m_bFirstColStretchy)
		{
			nNumCols = GetColumnCount();

			for (nCol = 1; nCol < nNumCols; nCol++)
				nColEnd += GetColumnWidth(nCol);

			nColEnd = rClient.Width() - nColEnd;
		
			// if its reasonably less than the client area adjust else do nothing
			if (nColEnd < rClient.Width() - 20 || nNumCols == 1)
				SetColumnWidth(0, nColEnd - 2);
		}

		// show header ctrl
		wpos.flags |= SWP_SHOWWINDOW;
	}
	else // hide header ctrl
		wpos.flags |= SWP_HIDEWINDOW;
	
	// ensure the header ctrl is correctly positioned
/*
	if (m_header.GetSafeHwnd())
	{
		::SetWindowPos(GetHeader()->GetSafeHwnd(), wpos.hwndInsertAfter, wpos.x, wpos.y,
		wpos.cx, wpos.cy, wpos.flags);
	}
*/

	// windows does not send wm_paint messages if the window size decreased
	// and we have no elements, so...
	if (!GetItemCount())
		Invalidate(FALSE);

	bInOnSize = FALSE;
}

int CEnListCtrl::FindItemFromData(DWORD dwItemData) const
{
	if (!GetItemCount())
		return -1;

	LV_FINDINFO lvfi;

	lvfi.flags = LVFI_PARAM;
	lvfi.lParam = (LPARAM)dwItemData;

	return FindItem(&lvfi, -1);

	/*
	int nNumItems = GetItemCount();
	TRACE("\n");

	while (nNumItems--)
	{
		DWORD dwID = GetItemData(nNumItems);
		TRACE ("CEnListCtrl::FindItemFromData(%d)::GetItemData(%d) == %d\n", dwItemData, nNumItems, dwID);

		if (dwID == dwItemData)
		{
			TRACE ("Found. Item index = %d\n", nNumItems);
			return nNumItems;
		}
	}

	TRACE ("Found. Item not found");
	return -1;
	*/
}

int CEnListCtrl::FindItemFromLabel(CString sLabel, BOOL bExact, int nFromIndex) const
{
	if (!GetItemCount())
		return -1;

	LV_FINDINFO lvfi;

	lvfi.flags = LVFI_STRING | LVFI_WRAP;

	if (!bExact)
		lvfi.flags |= LVFI_PARTIAL;

	lvfi.psz = static_cast<LPCTSTR>(sLabel);

	return FindItem(&lvfi, nFromIndex ? -1 : nFromIndex);
}

BOOL CEnListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if (::IsWindow(m_ttCtrl.m_hWnd) && pMsg->hwnd == m_hWnd && m_bTooltipsEnabled)
	{
		switch (pMsg->message)
		{
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_MOUSEMOVE:
				m_ttCtrl.RelayEvent(pMsg);
				break;
		}
	}
	
	return CListCtrl::PreTranslateMessage(pMsg);
}

void CEnListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	LOGFONT lf;
    GetFont()->GetLogFont( &lf );
	m_nItemHeight = max(m_nItemHeight, abs(lf.lfHeight));
    
    lpMeasureItemStruct->itemHeight = m_nItemHeight; 
}

BOOL CEnListCtrl::SetItemHeight(int nHeight)
{
	ASSERT (nHeight >= 10 && nHeight <= 40); // abitrary limits

	if (nHeight >= 10 && nHeight <= 40)
	{
		m_nItemHeight = nHeight;

		if (GetSafeHwnd())
		{
			CRect rWindow;
			GetWindowRect(rWindow);
			SetWindowPos(NULL, 0, 0, rWindow.Width()-1, rWindow.Height(), SWP_NOMOVE | SWP_NOZORDER);
		}

		return TRUE;
	}
	// else
	return FALSE; // invalid size 
}

void CEnListCtrl::DeleteAllColumns()
{
	int nNumCols;
	
	nNumCols = GetColumnCount();

	while (nNumCols--)
		DeleteColumn(nNumCols);
}

void CEnListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int nSel;

	CListCtrl::OnLButtonDblClk(nFlags, point);

	nSel = GetCurSel();
	
	if (nSel != -1) // user clicked on an item
	{
		// determine whether a sel change has occured
		// and tell our parent if it has
		m_nmhdr.hwndFrom = m_hWnd;
		m_nmhdr.idFrom = GetDlgCtrlID();
		m_nmhdr.code = LVN_USERSELCHANGEDBLCLK;
		GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&m_nmhdr);
	}
}

BOOL CEnListCtrl::SelectDropTarget(int nItem)
{
	int nCurDropHilited;

	// remove current drop selection provided item was not 
	// originally selected
	nCurDropHilited = GetNextItem(-1, LVNI_DROPHILITED);

	if (nCurDropHilited != -1 && nCurDropHilited != nItem)
	{
		SetItemState(nCurDropHilited, 0, LVIS_DROPHILITED);
		RedrawItems(nCurDropHilited, nCurDropHilited);
		UpdateWindow();
	}

	if (nItem < 0 || nItem >= GetItemCount())
		return FALSE;

	// set new drop selection provided hilite has changed and
	// the item isn't already selected
	if ((nCurDropHilited != nItem) && 
		((GetItemState(nItem, LVIS_SELECTED) & LVIS_SELECTED) != LVIS_SELECTED))
	{
		SetItemState(nItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
		RedrawItems(nItem, nItem);
		UpdateWindow();
	}

	return TRUE;
}

void CEnListCtrl::SetItemImage(int nItem, int nImage)
{
	if (nItem < 0 || nItem >= GetItemCount())
		return;

	SetItem(nItem, 0, LVIF_IMAGE, _T(""), nImage, 0, 0, 0L);
}

BOOL CEnListCtrl::IsItemSelected(int nItem) const
{
	ASSERT (nItem >= 0 && nItem < GetItemCount());

	return (GetItemState(nItem, LVIS_SELECTED) == LVIS_SELECTED);
}

void CEnListCtrl::GetCellRect(int nRow, int nCol, CRect& rCell) const
{
	CRect rItem;
	GetItemRect(nRow, rItem, LVIR_BOUNDS); // item rect in current view

	int nColStart, nColEnd;
	GetColumnExtents(nCol, nColStart, nColEnd); // relative positions of column in client coords

	rCell.SetRect(nColStart, rItem.top, nColEnd, rItem.bottom);
}

void CEnListCtrl::GetCellEditRect(int nRow, int nCol, CRect& rCell) const
{
	GetCellRect(nRow, nCol, rCell);
	rCell.OffsetRect(-2, 0);
}

void CEnListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// do this first to ensure that ctrl is correctly prepared
	// before passing on in case parent is dealing with context menu
	ClientToScreen(&point);
	OnContextMenu(this, point); 

	CListCtrl::OnRButtonDown(nFlags, point);
}

/*
long CEnListCtrl::OnShowPopupMenu(WPARAM wParam, LPARAM lParam)
{
	m_popupMenu.DoPopup(m_ptPopupPos, this);

	return 0L;
}

*/
void CEnListCtrl::NotifySelChange()
{
	m_bUserSelChange = TRUE;

	m_nmhdr.hwndFrom = m_hWnd;
	m_nmhdr.idFrom = GetDlgCtrlID();
	m_nmhdr.code = LVN_USERSELCHANGE;
	GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetDlgCtrlID(), (LPARAM)&m_nmhdr);

	m_bUserSelChange = FALSE;
}

int CEnListCtrl::GetColumnAtPoint(CPoint point) const
{
	int nCol, nNumCols;
	CPoint ptItem;

	// iterate thru columns until we reach the one which was clicked
	nNumCols = GetColumnCount();

	for (nCol = 0; nCol < nNumCols; nCol++)
	{
		int nColStart, nColEnd;
		GetColumnExtents(nCol, nColStart, nColEnd);

		if (point.x >= nColStart && point.x < nColEnd)
			return nCol;
	}

	// not on a column
	return -1;
}

void CEnListCtrl::GetColumnExtents(int nCol, int& nColStart, int& nColEnd) const
{
	int nColItem;
	CPoint ptItem;

	ASSERT (nCol >= 0 && nCol < GetColumnCount());
	
	if (nCol >= 0 && nCol < GetColumnCount())
	{
		// iterate thru columns until we reach the one we want
		CRect rItem;

		GetItemRect(0, rItem, LVIR_BOUNDS); // item rect in current view
		nColEnd = nColStart = rItem.left - 2;

		for (nColItem = 0; nColItem <= nCol; nColItem++)
		{
			// calc col end in client coordinates
			nColStart = nColEnd;
			nColEnd += GetColumnWidth(nColItem);
		}

		return;
	}

	// nCol invalid
	nColStart = -1;
	nColEnd = -1;
}

void CEnListCtrl::OnColumnClick(NMHDR* pNMHDR, LPARAM* /*lParam*/)
{
	NM_LISTVIEW* pNMLV = (NM_LISTVIEW*)pNMHDR;

	if (m_bSortingEnabled)
		SetSortColumn(pNMLV->iSubItem);
}

void CEnListCtrl::SetSortColumn(int nColumn, BOOL bResort)
{
	if (!m_bSortingEnabled)
		return;

	if (nColumn != m_nSortColumn)
	{
		m_nSortColumn = nColumn;
		m_bSortAscending = TRUE;
	}
	else
		m_bSortAscending = !m_bSortAscending;

	if (bResort)
		Sort();
}

void CEnListCtrl::Sort()
{
	// rebuild sort map
	BuildSortMap(m_nSortColumn);

	// do sort
	SortItems(CompareProc, (LPARAM)this);

	// cleanup
	m_mapSortStrings.RemoveAll();
}

int CALLBACK CEnListCtrl::CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParam)
{
	CEnListCtrl* pListCtrl;
	int nResult;

	pListCtrl = (CEnListCtrl*)lParam;
	ASSERT (pListCtrl->IsKindOf(RUNTIME_CLASS(CEnListCtrl)));

	nResult = pListCtrl->CompareItems(lParam1, lParam2, pListCtrl->m_nSortColumn);

	if (!pListCtrl->m_bSortAscending)
		nResult = -nResult;

	return nResult;
}

void CEnListCtrl::BuildSortMap(int nCol)
{
	// because we can't reliably get back from the itemdata to the item index
	// during a sort, we map the itemdata of each item index directly to
	// the column string
	m_mapSortStrings.RemoveAll();
	int nNumItems = GetItemCount();

#pragma warning(disable: 4239)
	while (nNumItems--)
		m_mapSortStrings.SetAt(GetItemData(nNumItems), GetItemText(nNumItems, nCol));
#pragma warning(default: 4239)
}

int CEnListCtrl::CompareItems(DWORD dwItemData1, DWORD dwItemData2, int /*nSortColumn*/)
{
	// -1 if dwItemData1 should go BEFORE dwItemData2
	//  1 if dwItemData1 should go AFTER dwItemData2
	//  0 if it doesn't matter

	// default comparison just compares text
	CString sItem1, sItem2;

	m_mapSortStrings.Lookup(dwItemData1, sItem1); // this is quicker than helper method
	m_mapSortStrings.Lookup(dwItemData2, sItem2);

	// empty items always appear AFTER others
	if (sItem1.IsEmpty())
		return m_bSortAscending ? 1 : -1;

	else if (sItem2.IsEmpty())
		return m_bSortAscending ? -1 : 1;

	// else
	return sItem1.CompareNoCase(sItem2);
}

CString CEnListCtrl::GetSortText(DWORD dwItemData) const // this is for derived classes
{
	CString sText;

	m_mapSortStrings.Lookup(dwItemData, sText);

	return sText;
}

BOOL CEnListCtrl::SetColumnText(int nCol, LPCTSTR szText)
{
	LV_COLUMN lvc;
	ZeroMemory(&lvc, sizeof(lvc));

	lvc.mask = LVCF_TEXT;
	lvc.pszText = const_cast<LPTSTR>(szText);

	return SetColumn(nCol, &lvc);
}

void CEnListCtrl::OnDestroy() 
{
	CListCtrl::OnDestroy();
	
	m_bInitColumns = FALSE;
}

/*
void CEnListCtrl::Print(CDC* pDC, PRINT_ATTRIBUTES* pPrintSettings, int nNumCols, UINT* pColumns, UINT* pColWidths)
{
	int nRes;
	DOCINFO di;
	CFont font;
	CFont* pOldFont;
	TEXTMETRIC tm;
	int nIndexItem, nPage, nLine, nCopy, nPrintedLines;
	int nLineHeight;
	int nNumPages, nNumLinesPage;
	CPoint ptPrint;
	CString sPrint, sMessage;
	int nEndLine;
	CCancelDlg dialogCancel;
	int nNumCopies;
	BOOL bCollate;
	BOOL bSelection;
	int nStartLine, nStartItem;
	int nPrintFlags;
	int nLinesToPrint;
	CSize sizePrint, sElement;
	CRect rText;
	BOOL bDCDelete = FALSE;

	ASSERT (!nNumCols || pColumns || (pColWidths && nNumCols == GetColumnCount()));

	if( pDC == NULL)
	{

		// set print flags
		nPrintFlags = PD_NOPAGENUMS | PD_HIDEPRINTTOFILE | PD_NONETWORKBUTTON;

		//If no rows selected then disable the selection on the dlg
		if ( GetSelectedCount() == 0)
			nPrintFlags |= PD_NOSELECTION;
		else
			nPrintFlags |= PD_SELECTION; 

		// show dialog
		CPrintDialog dialogPrint(FALSE, nPrintFlags);

		nRes = dialogPrint.DoModal();

		if (nRes != IDOK)
			return;

		// get printer device context
		pDC = CDC::FromHandle(dialogPrint.GetPrinterDC());

		if (!pDC)
		{
			ASSERT (0);
			return;
		}
		else
			bDCDelete = TRUE;

		// get number of required copies and collation and Selection
		nNumCopies = dialogPrint.m_pd.nCopies;
		bCollate = dialogPrint.m_pd.Flags & PD_COLLATE;
		bSelection = dialogPrint.PrintSelection();
		pDC->m_bPrinting = TRUE;


		// this is very important
		::GlobalFree(dialogPrint.m_pd.hDevMode);
		::GlobalFree(dialogPrint.m_pd.hDevNames);
	}
	else
	{
		ASSERT(pPrintSettings!=NULL);

		nNumCopies = pPrintSettings->nNumCopies;
		bCollate = pPrintSettings->bCollate;
		bSelection = pPrintSettings->bSelection;
	}

	// create 8 point Arial font to be used for printing
	font.CreatePointFont(80, "Arial", pDC);

	// create cancel dialog
	dialogCancel.Create(this);
	dialogCancel.SetWindowText("Printing");
	dialogCancel.CenterWindow();
	dialogCancel.SetFirstTextLine("Printing...");
	dialogCancel.Update();

	AfxGetMainWnd()->EnableWindow(FALSE); // why?

	// get printable size in mm
	sizePrint.cx = pDC->GetDeviceCaps(HORZRES);
	sizePrint.cy = pDC->GetDeviceCaps(VERTRES);

	//Trim Print area to within Margins
	sizePrint.cx -= 2 * PRINTING_MARGIN;
	sizePrint.cy -= 2 * PRINTING_MARGIN;
	sizePrint.cy -=	HEADER_SIZE;
	sizePrint.cy -= FOOTER_SIZE;

	//Set the print region
	CRect rPrintRng(PRINTING_MARGIN, PRINTING_MARGIN, 
			sizePrint.cx + PRINTING_MARGIN,sizePrint.cy + PRINTING_MARGIN);

	CRgn rgnClip;
	rgnClip.CreateRectRgnIndirect(rPrintRng);
//	pDC->SelectClipRgn(&rgnClip);
	rgnClip.DeleteObject();

	// calculate line height and various page aspects
	pOldFont = pDC->SelectObject(&font);
	pDC->GetTextMetrics(&tm);
	pDC->SetTextAlign(TA_LEFT | TA_TOP);

	nLineHeight = tm.tmHeight + tm.tmExternalLeading;

	if(bSelection)
		//selected text only
		nLinesToPrint = GetSelectedCount();
	else							
		//All the text
		nLinesToPrint = GetItemCount();

	nNumLinesPage = (sizePrint.cy - (2 * nLineHeight)) / nLineHeight;
	nNumPages = nLinesToPrint / nNumLinesPage;

	if (nLinesToPrint % nNumLinesPage)
		nNumPages++;

	// start document
	CString sAppName = AfxGetAppName();
	// remove any words in parenthesis
	int nFind = sAppName.Find('(');

	if (nFind != -1)
		sAppName = sAppName.Left(nFind - 1);

	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = (sAppName += " - Message List");
	di.lpszOutput = NULL;
	di.fwType = 0;
	nRes = pDC->StartDoc(&di);

	int nNumCollationCopies,nColCopy;

	// printing WITH collation ----------------------------------
	if (bCollate)
	{
		nNumCollationCopies = nNumCopies;
		nNumCopies = 1;
	}
	// printing WITHOUT collation------------------------------
	else
		nNumCollationCopies = 1;
	
	//Calculate relative width of columns
	LV_COLUMN lvColumn;
	char strColumnBuffer[MAX_HEADING_SIZE]; 
	int nListWidth = 0,nColCount=0;
				
	lvColumn.mask = LVCF_TEXT;
	lvColumn.pszText = (char*)&strColumnBuffer;
	lvColumn.cchTextMax = MAX_HEADING_SIZE;

	while( GetColumn(nNumCols && pColumns ? pColumns[nColCount] : nColCount , &lvColumn ) && (nNumCols==0 || nColCount < nNumCols) )
	{
		if (!pColWidths)
			nListWidth += GetColumnWidth(  nNumCols ? pColumns[nColCount] : nColCount);
		else
			nListWidth += pColWidths[nColCount];
		
		nColCount++;
	}
				
	ASSERT(nListWidth>0);
	double dRelWidth =  double(sizePrint.cx) / nListWidth;

	for (nColCopy = 0; nColCopy < nNumCollationCopies && !dialogCancel.Cancelled(); nColCopy++)
	{
		nStartItem =  bSelection ? 	GetFirstSel() : 0;
		nStartLine = 0;
		nEndLine = nNumLinesPage;
	
		for (nPage = 0; nPage < nNumPages && !dialogCancel.Cancelled(); nPage++)
		{
			// update cancel dialog box
			sMessage.Format("Printing page %d", nPage + 1);
			dialogCancel.SetFirstTextLine(sMessage);

			for (nCopy = 0; nCopy < nNumCopies && !dialogCancel.Cancelled(); nCopy++)
			{
				// update cancel dialog box
				sMessage.Format("Copy %d", nCopy + 1);
				dialogCancel.SetSecondTextLine(sMessage);

				nIndexItem = nStartItem;
				nLine = nPrintedLines = nStartLine;

				nRes = pDC->StartPage();
				pDC->SelectObject(&font);
				ptPrint.y = PRINTING_MARGIN + HEADER_SIZE;

				CString strHeading;
				GetPrintHeadingText(strHeading);

				// Print list headings
				for (int nPos = PRINTING_MARGIN,nSubItem = 0; GetColumn( nNumCols && pColumns ? pColumns[nSubItem] : nSubItem , &lvColumn ) && (nNumCols==0 || nSubItem < nNumCols) ;  nSubItem++) // ignore description
				{
					int nColWidth = 0;
					
					if (!pColWidths)
						nColWidth = GetColumnWidth(  nNumCols ? pColumns[nSubItem] : nSubItem);
					else
						nColWidth = pColWidths[nSubItem];

					nColWidth = int(  dRelWidth * nColWidth );

					rText.SetRect(nPos, ptPrint.y, nColWidth + nPos, ptPrint.y + nLineHeight);
					pDC->ExtTextOut(nPos, ptPrint.y, ETO_CLIPPED, rText, lvColumn.pszText  , NULL);

					nPos += nColWidth;
				}
				ptPrint.y += 2 * nLineHeight;

				//Now Print the List
				while (nLine < nEndLine && nPrintedLines < nLinesToPrint &&	!dialogCancel.Cancelled())
				{
					// do the print 
					for (int nPos = PRINTING_MARGIN,nSubItem = 0; GetColumn( nNumCols && pColumns ? pColumns[nSubItem] : nSubItem , &lvColumn ) && (nNumCols==0 || nSubItem < nNumCols); nSubItem++) // ignore description
					{
						// Calculate the Column relative position and size
						int nColWidth = 0;
						
						if (!pColWidths)
							nColWidth = GetColumnWidth(  nNumCols ? pColumns[nSubItem] : nSubItem);
						else
							nColWidth = pColWidths[nSubItem];

						nColWidth = int(  dRelWidth * nColWidth );

						//Create the rect and Print to DC
						CString sItem = GetItemText(nIndexItem, nNumCols && pColumns ? pColumns[nSubItem] : nSubItem);
						rText.SetRect(nPos, ptPrint.y, nColWidth + nPos, ptPrint.y + nLineHeight);
						pDC->ExtTextOut(nPos, ptPrint.y, ETO_CLIPPED, rText, sItem, NULL);
			
						//increment position
						nPos += nColWidth;
					}

#ifdef _DEBUG
					CString sItem = GetItemText(nIndexItem, 0);
#endif
					nLine++;nPrintedLines++;
					bSelection ? nIndexItem = GetNextSel(nIndexItem): nIndexItem++;
					ptPrint.y += nLineHeight;
				}

				BdeTools::PrintHeader(pDC, strHeading , nPage + 1 );
				BdeTools::PrintFooter(pDC, nPage+1, nNumPages);

				pDC->EndPage();
			}

			nStartLine = nEndLine;
			nStartItem = nIndexItem;
			nEndLine += nNumLinesPage;
		}
	}

	// abort print job if user clicked cancel
	if (!dialogCancel.Cancelled())
		pDC->EndDoc();
	else
		pDC->AbortDoc();

	// restore device context
	pDC->SelectObject(pOldFont);

	AfxGetMainWnd()->EnableWindow(TRUE);
	SetFocus();

	// this is very important
	font.DeleteObject();

	if(bDCDelete)
		pDC->DeleteDC();
}
*/


BOOL CEnListCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	
	return CListCtrl::PreCreateWindow(cs);
}

int CEnListCtrl::GetColumnCount() const
{
	if (GetHeader())
		return GetHeader()->GetItemCount();

	// else
	LV_COLUMN lvc;
	int nCol = 0;

	while (GetColumn(nCol, &lvc))
		nCol++;

	return nCol;
}

void CEnListCtrl::SetItemIndent(int nItem, int nIndent)
{
//	if (!GetImageList(LVSIL_SMALL))
//		SetImageList(&s_ilIndent, LVSIL_SMALL);

	LV_ITEM lvi;
	
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_INDENT;
	lvi.iIndent = nIndent;
	
	VERIFY(SetItem(&lvi));
}

int CEnListCtrl::GetItemIndent(int nItem) const
{
	LV_ITEM lvi;
	
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_INDENT;
	
	return GetItem(&lvi) ? lvi.iIndent : 0;
}

void CEnListCtrl::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();

	SetItemHeight(CDlgUnits(this).ToPixelsY(9));
	
	GetHeader(); // to initialize
	

}

BOOL CEnListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	if (GetItemCount() && GetView() != LVS_REPORT)
		return CListCtrl::OnEraseBkgnd(pDC);
	
	else // we do all the work in OnPaint
		return TRUE;
}

void CEnListCtrl::EnableAlternateRowColoring(BOOL bEnable)
{
	if (m_bAlternateRowColoring != bEnable)
	{
		m_bAlternateRowColoring = bEnable;

		if (GetSafeHwnd())
			Invalidate(FALSE);
	}
}
