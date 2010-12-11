// Original file written by Ali Rafiee (http://www.codeproject.com/KB/combobox/TransListBox.aspx)
// Rewritten for the ToDoList (http://www.codeproject.com/KB/applications/todolist2.aspx)
// Design and latest version - http://www.codeproject.com/KB/applications/TDL_Calendar.aspx
// by demon.code.monkey@googlemail.com

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - taken out from the original TDL_Calendar package for better sharing
// - adjusted #include's paths
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

#include "StdAfx.h"
#include "TransparentListBox.h"
#include "GraphicsMisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TASKS_FONT_NAME         _T("MS Shell Dlg 2")
#define TASKS_FONT_SIZE         7

/////////////////////////////////////////////////////////////////////////////
// CTransparentListBox

CTransparentListBox::CTransparentListBox():
m_bHasBackground(FALSE),
m_bCompletedItemsGrey(FALSE),
m_bCompletedItemsStrikethru(FALSE)
{
	m_colorTextFG = ::GetSysColor(COLOR_WINDOWTEXT);
	m_colorSelectedTextFG = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_colorSelectedTextBG = ::GetSysColor(COLOR_HIGHLIGHT);
	m_colorSelectedTextOutline = ::GetSysColor(COLOR_3DSHADOW);
	m_colorTextDisabled = RGB(200, 200, 200);
}

CTransparentListBox::~CTransparentListBox()
{
	m_font.DeleteObject();
	m_fontStrikethru.DeleteObject();
	m_bmp.DeleteObject();

	int nItems = m_arrItems.GetSize();
	for (int i = 0; i < nItems; i++)
	{
		delete m_arrItems[i];
	}
}

BEGIN_MESSAGE_MAP(CTransparentListBox, CListBox)
	//{{AFX_MSG_MAP(CTransparentListBox)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_CONTROL_REFLECT_EX(LBN_SELCHANGE, OnLbnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransparentListBox message handlers
//
BOOL CTransparentListBox::OnEraseBkgnd(CDC* /*pDC*/)
{
	if (!m_bHasBackground)
	{
		CWnd* pParent = GetParent();
		if (pParent)
		{
			CRect rc;
			GetClientRect(&rc);
			ClientToScreen(&rc);
			pParent->ScreenToClient(&rc);
			CDC* pDC = pParent->GetDC();
			CDC dcMem;
			dcMem.CreateCompatibleDC(pDC);
			CBitmap* pbmpOld = dcMem.SelectObject(&m_bmp);
			dcMem.BitBlt(0, 0, rc.Width(), rc.Height(), pDC, rc.left, rc.top, SRCCOPY);
			dcMem.SelectObject(pbmpOld);
			m_bHasBackground = TRUE;
			pParent->ReleaseDC(pDC);
		}
	}
	return TRUE;
}

void CTransparentListBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect(&rc);

	int nWidth = rc.Width();
	int nHeight = rc.Height();

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CBitmap bmpMem;
	bmpMem.CreateCompatibleBitmap(&dc, nWidth, nHeight);

	CBitmap* pbmpMemOld = dcMem.SelectObject(&bmpMem);

	CDC dcMem2;
	dcMem2.CreateCompatibleDC(&dc);
	CBitmap* pbmpOld = dcMem2.SelectObject(&m_bmp);
	dcMem.BitBlt(0, 0, nWidth, nHeight, &dcMem2, 0, 0, SRCCOPY);
	dcMem2.SelectObject(pbmpOld);

	rc.top = 0;
	rc.left = 0;
	rc.bottom = rc.top + GetItemHeight(0);
	rc.right = nWidth;

	int nSize = GetCount();
	for (int i = GetTopIndex(); (i < nSize) && (rc.top <= nHeight); i++)
	{
		BOOL bSelected = GetSel(i);
		DrawItem(dcMem, i, rc, bSelected);
		int nCurHeight = GetItemHeight(i);
		rc.OffsetRect(0, nCurHeight);
	}

	dc.BitBlt(0, 0, nWidth, nHeight, &dcMem, 0, 0, SRCCOPY);

	dcMem.SelectObject(pbmpMemOld);
}

void CTransparentListBox::OnSize(UINT nType, int cx, int cy)
{
	CListBox::OnSize(nType, cx, cy);

	CWnd* pParent = GetParent();
	if (pParent)
	{
		CRect rc;
		GetClientRect(&rc);
		CDC* pDC = pParent->GetDC();
		m_bmp.DeleteObject();
		m_bmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	}

	ShowWindow(SW_HIDE);
	m_bHasBackground = FALSE;
	Invalidate();
	UpdateWindow();
	ShowWindow(SW_SHOW);
}

void CTransparentListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SetRedraw(FALSE);
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
	SetRedraw(TRUE);
}

void CTransparentListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SetRedraw(FALSE);
	CListBox::OnChar(nChar, nRepCnt, nFlags);
	SetRedraw(TRUE);
}

void CTransparentListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonDown(nFlags, point);
	BOOL bOutside = FALSE;
	int nItem = ItemFromPoint(point, bOutside);
	if (bOutside)
	{
		nItem = -1;
	}
	SetCurSel(nItem);
}

void CTransparentListBox::OnRButtonDown(UINT nFlags, CPoint point)
{
	CListBox::OnRButtonDown(nFlags, point);
	BOOL bOutside = FALSE;
	int nItem = ItemFromPoint(point, bOutside);
	if (bOutside)
	{
		nItem = -1;
	}
	SetCurSel(nItem);
	Invalidate();
}

BOOL CTransparentListBox::OnLbnSelchange()
{
	RedrawWindow(0, 0, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
	return FALSE;
}

void CTransparentListBox::PreSubclassWindow()
{
	CListBox::PreSubclassWindow();

	ModifyStyle(0, SS_NOTIFY);

	CWnd* pParent = GetParent();
	if (pParent)
	{
		CRect rc;
		GetClientRect(&rc);
		CDC* pDC = pParent->GetDC();
		m_bmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());

		GraphicsMisc::CreateFont(m_font, TASKS_FONT_NAME, TASKS_FONT_SIZE, MFS_BOLD);
		GraphicsMisc::CreateFont(m_fontStrikethru, TASKS_FONT_NAME, TASKS_FONT_SIZE, MFS_BOLD | MFS_STRIKETHRU);
	}
}

void CTransparentListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CDC* pDC = GetDC();
	CRect rc;
	rc.SetRectEmpty();

	CFont* pfontOld = pDC->SelectObject(&m_font);
	pDC->DrawText(_T("XYZ"), &rc, DT_CALCRECT | DT_EXPANDTABS | DT_NOPREFIX);
	lpMeasureItemStruct->itemHeight = static_cast<long>(rc.Height()) + 1;
	pDC->SelectObject(pfontOld);
	ReleaseDC(pDC);
}

void CTransparentListBox::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
}

void CTransparentListBox::DrawItem(CDC& _dc, int _nIndex, CRect& _rc, BOOL _bSelected)
{
	if ((_nIndex == LB_ERR) || (_nIndex >= GetCount()))
	{
		return;
	}

	CRect rcItem = _rc;
	rcItem.left += 3;
	_dc.SetBkMode(TRANSPARENT);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&_dc);

	CFont* pfontOld = _dc.SelectObject(&m_font);
	CBitmap* pbmpOld = dcMem.SelectObject(&m_bmp);
	_dc.BitBlt(rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), &dcMem, rcItem.left, rcItem.top, SRCCOPY);

	CString strText;
	GetText(_nIndex, strText);

	if (IsWindowEnabled())
	{
		SItem* pItem = (SItem*)GetItemData(_nIndex);

		if (pItem->bCompleted && m_bCompletedItemsStrikethru)
		{
			_dc.SelectObject(&m_fontStrikethru);
		}

		if (_bSelected)
		{
			CRect rcSelection(_rc);
			_dc.SetTextColor(m_colorSelectedTextFG);
			CBrush brush(m_colorSelectedTextBG);
			CPen pen(PS_SOLID, 1, m_colorSelectedTextOutline);
			CBrush* pbrushOld = _dc.SelectObject(&brush);
			CPen* ppenOld = _dc.SelectObject(&pen);
			_dc.Rectangle(rcSelection);
			_dc.SelectObject(pbrushOld);
			_dc.SelectObject(ppenOld);
		}
		else
		{
			COLORREF crTextFG = m_colorTextFG;
			if (pItem->bCompleted && m_bCompletedItemsGrey)
			{
				crTextFG = m_colorTextDisabled;
			}
			_dc.SetTextColor(crTextFG);
		}
	}
	else
	{
		_dc.SetTextColor(m_colorTextDisabled);
	}

	_dc.DrawText(strText, rcItem, DT_LEFT | DT_EXPANDTABS | DT_NOPREFIX);

	_dc.SelectObject(pfontOld);
	dcMem.SelectObject(pbmpOld);
}

int CTransparentListBox::GetTotalItemHeight() const
{
	int nTotalHeight = 0;
	int nSize = GetCount();
	if (nSize > 0)
	{
		for (int i = 0; i < nSize; i++)
		{
			int nCurHeight = GetItemHeight(i);
			nTotalHeight += nCurHeight;
		}
	}
	return nTotalHeight;
}

//ensures top item is visible
void CTransparentListBox::EnsureTopItemVisible()
{
	if (GetCount() > 0)
	{
		if (GetTopIndex() != 0)
		{
			SetTopIndex(0);
		}
	}
}

int CTransparentListBox::AddString(const CString& _strText, DWORD _dwItemData, BOOL _bCompleted)
{
	SetRedraw(FALSE);
	int nIndexNew = CListBox::AddString(_strText);

	SItem item = {_dwItemData, _bCompleted};
	SItem* pItem = new SItem;
	*pItem = item;
	m_arrItems.Add(pItem);
	SetItemData(nIndexNew, (DWORD)pItem);

	SetRedraw(TRUE);
	return nIndexNew;
}

int CTransparentListBox::InsertString(int _nIndex, const CString& _strText, DWORD _dwItemData, BOOL _bCompleted)
{
	SetRedraw(FALSE);
	int nIndexNew = CListBox::InsertString(_nIndex, _strText);

	SItem item = {_dwItemData, _bCompleted};
	SItem* pItem = new SItem;
	*pItem = item;
	m_arrItems.Add(pItem);
	SetItemData(nIndexNew, (DWORD)pItem);

	SetRedraw(TRUE);
	return nIndexNew;
}

int CTransparentListBox::SetCurSel(int nSelect)
{
	int nReturn = CListBox::SetCurSel(nSelect);

	ShowWindow(SW_HIDE);
	m_bHasBackground = FALSE;
	Invalidate();
	UpdateWindow();
	ShowWindow(SW_SHOW);

	return nReturn;
}

void CTransparentListBox::SetCompletedItemDisplayStyle(BOOL _bGrey, BOOL _bStrikethru)
{
	m_bCompletedItemsGrey = _bGrey;
	m_bCompletedItemsStrikethru = _bStrikethru;
}
