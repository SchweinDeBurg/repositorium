// FlatTab.cpp : implementation file
//
// Copyright © 1999 Ian Brumby
//
// This source code may be used in compiled form in any way you desire. 
// Source file(s) may be redistributed unmodified by any means PROVIDING
// they are not sold for profit without the authors expressed written consent,
// and providing that this notice and the authors name and all copyright
// notices remain intact.
//
// ==========================================================================  
// HISTORY:	  
// ==========================================================================  
//			1.4 	21 Jul 1999	- Initial release.
//			1.5		30 Aug 1999 - Several enhancements by Phil Baxter 
//					<Phil.Baxter@mrjinx.demon.co.uk>
// ==========================================================================  

// Excel uses FF_ROMAN and Font Height = ~(control's height - 4)
// Excel draws its tabs right to left
// Excel has 3DLIGHT highlighting on the left diagonal
// Excel has home/end buttons
// Excel's buttons change width as the control's height changes
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 5/03/01 15:50 $
 * $Revision: 2 $
 * $Archive: /Projects/AddIns/ResOrg/CJLibrary/CJLibrary/CJFlatTabCtrl.cpp $
 *
 * $History: CJFlatTabCtrl.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 5/03/01    Time: 15:50
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/CJLibrary
 * Bug fixes
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:52p
 * Updated in $/CodeJock/CJLibrary
 * Fixed bug with flat tab where in some cases tabs were not completely
 * painted.
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 10/25/99   Time: 10:52p
 * Updated in $/CodeJock/CJLibrary
 * Modified resource include for static builds.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:15a
 * Updated in $/CodeJock/CJLibrary
 * Fixed minor bug with tab font creation.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/CJLibrary
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 11:37p
 * Updated in $/CodeJock/CJLibrary
 * Fixed bug with flat tab control that would have kept one from disabling
 * the l/r arrow buttons, which caused an exception to be thrown. Thanks
 * to Ted Crow [crowtc@surge.net] for help with this.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:25p
 * Updated in $/CodeJock/CJLibrary
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJResource.h"
#include "CJFlatTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJFlatTabCtrl

CCJFlatTabCtrl::CCJFlatTabCtrl()
{
	m_bHasArrows          = false;
	m_bHasHomeEnd         = false;
	m_bTabsOnBottom       = false;
	m_iCurSel             = -1;
	m_pLeftButton         = NULL;
	m_pRightButton        = NULL;
	m_pHomeButton         = NULL;
	m_pEndButton          = NULL;
	m_hLeftArrow          = NULL;
	m_hLeftArrowDisabled  = NULL;
	m_hRightArrow         = NULL;
	m_hRightArrowDisabled = NULL;
	m_hHomeArrow          = NULL;
	m_hHomeArrowDisabled  = NULL;
	m_hEndArrow           = NULL;
	m_hEndArrowDisabled   = NULL;
	m_iXOffset            = 0;
}

CCJFlatTabCtrl::~CCJFlatTabCtrl()
{
	_delete(m_pLeftButton);
	_delete(m_pRightButton);
	_delete(m_pHomeButton);
	_delete(m_pEndButton);

	// free HENHMETAFILE handles.
	if(m_hLeftArrow) {
		::DeleteEnhMetaFile(m_hLeftArrow);
	}
	if(m_hLeftArrowDisabled) {
		::DeleteEnhMetaFile(m_hLeftArrowDisabled);
	}
	if(m_hRightArrow) {
		::DeleteEnhMetaFile(m_hRightArrow);
	}
	if(m_hRightArrowDisabled) {
		::DeleteEnhMetaFile(m_hRightArrowDisabled);
	}
	if(m_hHomeArrow) {
		::DeleteEnhMetaFile(m_hHomeArrow);
	}
	if(m_hHomeArrowDisabled) {
		::DeleteEnhMetaFile(m_hHomeArrowDisabled);
	}
	if(m_hEndArrow) {
		::DeleteEnhMetaFile(m_hEndArrow);
	}
	if(m_hEndArrowDisabled) {
		::DeleteEnhMetaFile(m_hEndArrowDisabled);
	}
	while(!m_TabList.IsEmpty()) {
		CString* pItem = m_TabList.RemoveTail();
		_delete(pItem);
	}
	while(!m_TabTipList.IsEmpty()) {
		CString* pItem = m_TabTipList.RemoveTail();
		_delete(pItem);
	}

	// fix potential resource leak - KStowell - 10-21-99
	m_Font.DeleteObject();
	m_BoldFont.DeleteObject();
}

void CCJFlatTabCtrl::CreateTabFont(const int iTabHeight)
{
	int iHeight = -(iTabHeight - 6);

	m_Font.DeleteObject();
	m_Font.CreateFont(iHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY, FF_SWISS, NULL);

	m_BoldFont.DeleteObject();
	m_BoldFont.CreateFont(iHeight, 0, 0, 0, FW_EXTRABOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, PROOF_QUALITY, FF_SWISS, NULL);
}

int CCJFlatTabCtrl::GetTabWidth(const int nItem) const
{
	CDC dc;
	dc.CreateCompatibleDC(NULL);
	CFont* pOldFont = dc.SelectObject((CFont*)&m_BoldFont);
	CSize size(dc.GetTextExtent(*m_TabList.GetAt(m_TabList.FindIndex(nItem))));
	
	// fix potential resource leak - KStowell - 10-20-99.
	dc.SelectObject(pOldFont);
	dc.DeleteDC();
	
	return size.cx + m_iClientHeight + (m_iClientHeight / 2);
}

int CCJFlatTabCtrl::GetTotalTabWidth(void) const
{
	int iWidth = 0;
	for (int i = 0; i < m_TabList.GetCount(); i++)
	{
		iWidth += GetTabWidth(i);
#if 0
		if (i != m_TabList.GetCount())
			iWidth -= (m_iClientHeight / 2);
#else
		// Andy Metcalfe 28/10/99
		// Fix for painting bug reported in the CodeJock Message Board Developers Forum
		// in the posting "[Bug] CCJFlatTabCtrl is not perfectly painted"
		// by "sbkang" on 4/10/99
		//
		//
		// This fix was posted by "uniken" on 27/10/99
		if (i != m_TabList.GetCount() - 1)
			iWidth -= (m_iClientHeight / 2);
#endif
	}
	return iWidth + 2;
}


int CCJFlatTabCtrl::GetTotalArrowWidth(void) const
{
	int iWidth = 1;
	if (m_bHasArrows)
		iWidth += 32;
	if (m_bHasHomeEnd)
		iWidth += 32;
	return iWidth;
}

void CCJFlatTabCtrl::InvalidateTabs(void)
{
	if (GetSafeHwnd())
	{
		// invalidate the visible tab area
		// to minimise flicker - don't erase the background
		CRect rcTabs;

		rcTabs.left = GetTotalArrowWidth();
		rcTabs.top = 0;
		rcTabs.right = rcTabs.left + (m_iTotalTabWidth - m_iXOffset);
		rcTabs.bottom = m_iClientHeight;
		InvalidateRect(&rcTabs, false);
		// invalidate the blank area to the right of the tabs
		if (rcTabs.right < m_iClientWidth)
		{
			rcTabs.left = rcTabs.right;
			rcTabs.right = m_iClientWidth;
			InvalidateRect(&rcTabs, true);
		}
	}
}

void CCJFlatTabCtrl::EnableButtons(void)
{
	if (m_bHasArrows)  //TCC: ADDED to handle situations where ther are no arrows
	{
		m_pLeftButton->EnableWindow(m_iXOffset);
		if (m_iClientWidth + m_iXOffset < m_iTotalTabWidth + GetTotalArrowWidth() + 1)
			m_pRightButton->EnableWindow(true);
		else
			m_pRightButton->EnableWindow(false);
		if (m_bHasHomeEnd)
		{
			m_pHomeButton->EnableWindow(m_iXOffset);
			m_pEndButton->EnableWindow(m_pRightButton->IsWindowEnabled());
		}
	}
}

int CCJFlatTabCtrl::DrawTab(CDC &dc, const int x, const int y, const int iHeight, bool bSelected, bool bBottom, LPCTSTR lpszTab) const
{
	CFont* pOldFont = dc.SelectObject((CFont*)&m_BoldFont);
	CSize size(dc.GetTextExtent(lpszTab));
	CRect rcText;
	rcText.left = iHeight + x - 4;
	rcText.top = 2 + y;
	rcText.right = size.cx + iHeight + x - 4;
	rcText.bottom = size.cy + 2 + y;
	
	int iHalf = iHeight / 2;
	int iWidth = iHeight + iHalf;
	
	COLORREF crBack;
	COLORREF crFore;
	if (bSelected) {
		crBack = GetSysColor(COLOR_WINDOW);
		crFore = GetSysColor(COLOR_WINDOWTEXT);
	} else {
		crBack = GetSysColor(COLOR_3DFACE);
		crFore = GetSysColor(COLOR_BTNTEXT);
	}

	CPen penOutline(PS_SOLID, 1, GetSysColor(COLOR_BTNTEXT));
	CPen penShadow(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
	CBrush brush;
	brush.CreateSolidBrush(crBack);
	
	POINT points[4];
	points[0].x = x;                     points[0].y = iHeight + y - 1;
	points[1].x = iHalf + x;             points[1].y = y;
	points[2].x = size.cx + iHeight + x; points[2].y = y;
	points[3].x = size.cx + iWidth + x;  points[3].y = iHeight + y - 1;
	
	if (bBottom) {
		// swap vertical coordinates
		points[0].y = points[1].y;
		points[2].y = points[3].y;
		points[1].y = points[2].y;
		points[3].y = points[0].y;
	}

	CPen* pOldPen = dc.SelectObject(&penOutline);
	dc.SetBkColor(crBack);
	CBrush* pOldBrush = dc.SelectObject(&brush);
	dc.Polygon(points, 4);
	dc.SelectObject(bSelected ? (CFont*)&m_BoldFont : (CFont*)&m_Font);
	dc.SetTextColor(crFore);
	dc.DrawText(lpszTab, rcText, DT_CENTER);
	dc.SelectObject(&penShadow);
	
	if (bSelected) {
		dc.MoveTo(iHalf + x + 1, points[1].y);
		dc.LineTo(size.cx + iHeight + x, points[1].y);
		CPen penBack(PS_SOLID, 1, crBack);
		dc.SelectObject(&penBack);
		dc.MoveTo(x + 1, points[0].y);
		dc.LineTo(size.cx + iWidth + x, points[0].y);
	} else {
		dc.MoveTo(iHalf + x + 1, points[1].y);
		dc.LineTo(size.cx + iHeight + x - 1, points[1].y);
		dc.LineTo(size.cx + iWidth + x - 1, points[0].y);
	}

	// fix potential resource leak - KStowell - 10-21-99
	dc.SelectObject(pOldFont);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	penOutline.DeleteObject();
	penShadow.DeleteObject();
	brush.DeleteObject();

	return size.cx + iWidth;
}

BOOL CCJFlatTabCtrl::InsertItem(const int nItem, LPCTSTR lpszItem)
{
	if (nItem < 0 || nItem > m_TabList.GetCount())
		return -1;
	
	// add tab to our list
	CString* pItem = new CString;
	ASSERT(pItem);

	*pItem = lpszItem;
	m_TabList.AddTail(pItem);
	
    // PGB - Add the tabs tip text to the list

	CString* pTipItem = new CString;
	ASSERT(pTipItem);

	*pTipItem = lpszItem;
	m_TabTipList.AddTail(pTipItem);

    //

	if (m_TabList.GetCount() == 1)
		m_iCurSel = 0;

   	m_iTotalTabWidth = GetTotalTabWidth();      // PGB - Recalculate the total tab width now we have added a new item
	
    EnableButtons();
	InvalidateTabs();

	return nItem;
}

BOOL CCJFlatTabCtrl::DeleteItem(int nItem)
{
	if (nItem < 0 || nItem >= m_TabList.GetCount())
		return FALSE;
	
	POSITION pos = m_TabList.FindIndex(nItem);
	ASSERT(pos);
	
	CString* pItem = m_TabList.GetAt(pos);
	m_TabList.RemoveAt(pos);
	_delete(pItem);

    // PGB - Stole the following code from the OnLButtonDown() function
    // in order to cause the tab control to show the newly selected item
    // after deleting an item from the list

	// warn parent that the selection is about to change
	int id = GetDlgCtrlID();
	NMHDR hdr;
	hdr.hwndFrom = m_hWnd;
	hdr.idFrom = id;
	hdr.code = TCN_SELCHANGING;
	if (GetParent()->SendMessage(WM_NOTIFY, id, (LPARAM)&hdr) == 0)
	{
	    if (m_iCurSel >= m_TabList.GetCount())
        {
            SetCurSel(m_TabList.GetCount() - 1); // PGB - Reset the currently selected tab to the last one in the list
			InvalidateTabs();
        }

		// notify parent that the selection has changed
		hdr.hwndFrom = m_hWnd;
		hdr.idFrom = id;
		hdr.code = TCN_SELCHANGE;
		GetParent()->SendMessage(WM_NOTIFY, id, (LPARAM)&hdr);
	}

   	m_iTotalTabWidth = GetTotalTabWidth();      // PGB - Recalculate the total tab width now we have deleted an item

    EnableButtons();
	InvalidateTabs();

	return TRUE;
}

BOOL CCJFlatTabCtrl::DeleteAllItems()
{
	while(!m_TabList.IsEmpty()) {
		CString* pItem = m_TabList.RemoveTail();
		_delete(pItem);
	}

	while(!m_TabTipList.IsEmpty()) {
		CString* pItem = m_TabTipList.RemoveTail();
		_delete(pItem);
	}

	m_iCurSel = -1;                             // PGB - Reset the currently selected tab to -1 as we have no tabs in our list now.

   	m_iTotalTabWidth = GetTotalTabWidth();      // PGB - Recalculate the total tab width now we have deleted all items

    EnableButtons();
	InvalidateTabs();

	return TRUE;
}

BOOL CCJFlatTabCtrl::GetItemRect(int nItem, LPRECT lpRect)
{
	if (nItem < 0 || nItem >= m_TabList.GetCount())
		return FALSE;
	
	int x = GetTotalArrowWidth();
	for (int i = 0; i < nItem; i++)
	{
		x += GetTabWidth(i);
		if (i != m_TabList.GetCount())
			x -= (m_iClientHeight / 2) + 2;
	}
	lpRect->left = x - m_iXOffset;
	lpRect->top = 0;
	lpRect->right = lpRect->left + GetTabWidth(nItem);
	lpRect->bottom = m_iClientHeight;
	return TRUE;
}

int CCJFlatTabCtrl::HitTest(TCHITTESTINFO *pHitTestInfo) const
{
	// ignore hits on the buttons
	int iHitX = pHitTestInfo->pt.x;
	if (iHitX < GetTotalArrowWidth())
		return -1;
	
	// check if any tabs were hit
	int x = GetTotalArrowWidth() - m_iXOffset;
	for (int i = 0; i < m_TabList.GetCount(); i++)
	{
		int iTabWidth = GetTabWidth(i);
		if (i != m_TabList.GetCount())
			iTabWidth -= (m_iClientHeight / 2) + 2;
		if ((x <= iHitX) && (iHitX <= x + iTabWidth))
			return i;
		x += iTabWidth;
	}
	
	// hit point is right of rightmost tab
	return -1;
}

int CCJFlatTabCtrl::SetCurSel(int nItem)
{
	if (nItem < 0 || nItem >= m_TabList.GetCount())
		return -1;
	
	int iPrevSel = m_iCurSel;
	m_iCurSel = nItem;
	
	// test if we need to center on the selected tab
	CRect rcItem;
	if (GetItemRect(nItem, &rcItem))
	{
		// test if the tab is off on the left
		int iTotalArrowWidth = GetTotalArrowWidth();
		rcItem.left -= iTotalArrowWidth;
		if (rcItem.left < 0)
			m_iXOffset += rcItem.left;
		else
		{
			// test if the tab is off on the right
			rcItem.right -= iTotalArrowWidth;
			int iTabAreaWidth = m_iClientWidth - iTotalArrowWidth;
			if (rcItem.right > iTabAreaWidth)
				m_iXOffset += (rcItem.right - iTabAreaWidth);
		}
	}
	
	EnableButtons();
	InvalidateTabs();

	return iPrevSel;
}

BEGIN_MESSAGE_MAP(CCJFlatTabCtrl, CWnd)
	//{{AFX_MSG_MAP(CCJFlatTabCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_LEFTBUTTON, OnLeftArrow)
	ON_BN_CLICKED(IDC_RIGHTBUTTON, OnRightArrow)
	ON_BN_CLICKED(IDC_HOMEBUTTON, OnHomeArrow)
	ON_BN_CLICKED(IDC_ENDBUTTON, OnEndArrow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJFlatTabCtrl message handlers

BOOL CCJFlatTabCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;
	
	cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS,
            ::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
	
	return TRUE;
}

BOOL CCJFlatTabCtrl::PreTranslateMessage(MSG* pMsg)
{
	ASSERT_VALID(this);

    if (::IsWindow(m_ToolTip.m_hWnd))
                                                                                                                                                                                                                                                                                                                                                                                                                           {
	    if(pMsg->message == WM_MOUSEMOVE && pMsg->hwnd == m_hWnd)
        {
		    CPoint Point(LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));

	        TCHITTESTINFO test;
	        test.pt = Point;
	        int iTab = HitTest(&test);

            if (iTab >= 0)
            {
                CString strText;

                strText = (LPCTSTR) *m_TabTipList.GetAt(m_TabTipList.FindIndex(iTab));

                TRACE2("Mouse over tab %d : %s\n", iTab, strText);

			    m_ToolTip.UpdateTipText(strText, this);
			    m_ToolTip.RelayEvent(pMsg);
            } else  {
				// VC5 compatibility - Kirk Stowell - 10-21-99
				::SendMessage(m_ToolTip.m_hWnd, TTM_POP, 0, 0L);
            }
        }
        else {
			// VC5 compatibility - Kirk Stowell - 10-21-99
			::SendMessage(m_ToolTip.m_hWnd, TTM_POP, 0, 0L);
        }
    }

	return CWnd::PreTranslateMessage(pMsg);
}

void CCJFlatTabCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CBrush brush;
	brush.CreateSysColorBrush(COLOR_3DFACE);
	CPen penBlack(PS_SOLID, 1, RGB(0,0,0));
	
	// draw the black line along the left of the control
	CPen* pOldPen = dc.SelectObject(&penBlack);
	dc.MoveTo(0, 0);
	dc.LineTo(0, m_iClientHeight);
	
	// draw the black line along the top/bottom of the control
	int iTotalArrowWidth = GetTotalArrowWidth();
	int iEndOfTab = iTotalArrowWidth + m_iTotalTabWidth - m_iXOffset;
	dc.MoveTo(iEndOfTab, m_bTabsOnBottom ? 0 : m_iClientHeight - 1);
	dc.LineTo(m_iClientWidth, m_bTabsOnBottom ? 0 : m_iClientHeight - 1);
	
	// fill the empty background area
	int iFillHeight = m_iClientHeight - (m_bTabsOnBottom ? 0 : 1);
	CRect rcBack(iEndOfTab, m_bTabsOnBottom ? 1 : 0, m_iClientWidth, iFillHeight);
	dc.FillRect(rcBack, &brush);
	
	// create a bitmap of all the tabs
	CDC dcMem;
	dcMem.CreateCompatibleDC(NULL);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, m_iTotalTabWidth, m_iClientHeight);
	CBitmap* pOldBitmap = dcMem.SelectObject(&bitmap);
	CBrush* pOldBrush = dcMem.SelectObject(&brush);
	CRect rc(0, m_bTabsOnBottom ? 1 : 0, m_iTotalTabWidth, iFillHeight);
	dcMem.FillRect(rc, &brush);
	
	int iOverlap = (m_iClientHeight / 2) + 2;
	int x = 0;
	int iSelX = 0;
	for (int i = 0; i < m_TabList.GetCount(); i++)
	{
		if (i != m_iCurSel)
			x += DrawTab(dcMem, x, 0, m_iClientHeight, false, m_bTabsOnBottom, *m_TabList.GetAt(m_TabList.FindIndex(i))) - iOverlap;
		else
		{
			iSelX = x;
			x += GetTabWidth(i) - iOverlap;
		}
	}
    if (m_iCurSel >= 0)
    	DrawTab(dcMem, iSelX, 0, m_iClientHeight, true, m_bTabsOnBottom, *m_TabList.GetAt(m_TabList.FindIndex(m_iCurSel)));
	
	// blit the bitmap onto the control
	dc.BitBlt(iTotalArrowWidth, 0, m_iClientWidth - iTotalArrowWidth, m_iClientHeight, &dcMem, m_iXOffset, 0, SRCCOPY);

	// fix potential resource leak - KStowell - 10-21-99.
	dcMem.SelectObject(pOldPen);
	dcMem.SelectObject(pOldBitmap);
	dcMem.SelectObject(pOldBrush);
	dcMem.DeleteDC();
	brush.DeleteObject();
	penBlack.DeleteObject();
	bitmap.DeleteObject();
}

void CCJFlatTabCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	TCHITTESTINFO test;
	test.pt = point;
	int iTab = HitTest(&test);
	if ((iTab != -1) && (iTab != m_iCurSel))
	{
		// warn parent that the selection is about to change
		int id = GetDlgCtrlID();
		NMHDR hdr;
		hdr.hwndFrom = m_hWnd;
		hdr.idFrom = id;
		hdr.code = TCN_SELCHANGING;
		if (GetParent()->SendMessage(WM_NOTIFY, id, (LPARAM)&hdr) == 0)
		{
			// parent has given permission for the selection to change
			SetCurSel(iTab);
			InvalidateTabs();
			
			// notify parent that the selection has changed
			hdr.hwndFrom = m_hWnd;
			hdr.idFrom = id;
			hdr.code = TCN_SELCHANGE;
			GetParent()->SendMessage(WM_NOTIFY, id, (LPARAM)&hdr);
		}
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CCJFlatTabCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (m_bHasHomeEnd) {
		m_pHomeButton->MoveWindow(1, 0, 16, cy);
		m_pLeftButton->MoveWindow(17, 0, 16, cy);
		m_pRightButton->MoveWindow(33, 0, 16, cy);
		m_pEndButton->MoveWindow(49, 0, 16, cy);
	} else {
		if (m_bHasArrows) { //TCC: ADDED to handle situations where ther are no arrows
			m_pLeftButton->MoveWindow(1, 0, 16, cy);
			m_pRightButton->MoveWindow(17, 0, 16, cy);
		}
	}
	
	m_iClientWidth  = cx;
	m_iClientHeight = cy;

	CreateTabFont(cy);
	
	m_iTotalTabWidth = GetTotalTabWidth();
	EnableButtons();
}

int CCJFlatTabCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (lpCreateStruct->style & FTS_BOTTOM) {
		m_bTabsOnBottom = true;
	}

    if (lpCreateStruct->style & TCS_TOOLTIPS) {
	    m_ToolTip.Create(this);
	    m_ToolTip.Activate(TRUE);
	    m_ToolTip.AddTool(this, _T("Flat Tab Tool Tip"));
        m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX); // Allow multiline tooltips
    }

	if (lpCreateStruct->style & FTS_HASARROWS)
	{
		m_bHasArrows = true;
		POINT points[4];
		points[0].x = 100; points[0].y = 0;
		points[1].x = 0;   points[1].y = 50;
		points[2].x = 100; points[2].y = 100;
		points[3].x = 100; points[3].y = 0;

		////////////////////////////////////////////////////////////////////////////////
		// This portion of the code was re-written to help eliminate a nasty resource
		// leak. It would seem that the CMetaFileDC was not freeing the GDI resources
		// completely whenever CMetaFileDC::CloseEnhanced() was called. This may be due 
		// to a BUG in MFC or perhaps this class, in any event, this seems to resolve 
		// the issue. - KStowell - 10-23-99.
		////////////////////////////////////////////////////////////////////////////////
		HPEN   hPen			  = ::CreatePen( PS_SOLID, 1, RGB( 0,0,0 ));
		HPEN   hPenDisabled   = ::CreatePen( PS_SOLID, 1, RGB( 128,128,128 ));
		HBRUSH hBrush		  = ::CreateSolidBrush( RGB( 0,0,0 ));
		HBRUSH hBrushDisabled = ::CreateSolidBrush( RGB( 128,128,128 ));;
		
		// create left arrow metafile
		HDC hMetaDC	     = ::CreateEnhMetaFile( NULL, NULL, NULL, NULL );
		HPEN hOldPen     = (HPEN)::SelectObject( hMetaDC, hPen );
		HBRUSH hOldBrush = (HBRUSH)::SelectObject( hMetaDC, hBrush );
		::Polygon( hMetaDC, points, 4 );
		::SelectObject( hMetaDC, hOldPen );
		::SelectObject( hMetaDC, hOldBrush );
		m_hLeftArrow = ::CloseEnhMetaFile( hMetaDC );

		// create disabled left arrow metafile
		hMetaDC	  = ::CreateEnhMetaFile( NULL, NULL, NULL, NULL );
		hOldPen   = (HPEN)::SelectObject( hMetaDC, hPenDisabled );
		hOldBrush = (HBRUSH)::SelectObject( hMetaDC, hBrushDisabled );
		::Polygon( hMetaDC, points, 4 );
		::SelectObject( hMetaDC, hOldPen );
		::SelectObject( hMetaDC, hOldBrush );
		m_hLeftArrowDisabled = ::CloseEnhMetaFile( hMetaDC );

		points[0].x = 0;   points[0].y = 0;
		points[1].x = 100; points[1].y = 50;
		points[2].x = 0;   points[2].y = 100;
		points[3].x = 0;   points[3].y = 0;
		
		// create right arrow metafile
		hMetaDC	  = ::CreateEnhMetaFile( NULL, NULL, NULL, NULL );
		hOldPen   = (HPEN)::SelectObject( hMetaDC, hPen );
		hOldBrush = (HBRUSH)::SelectObject( hMetaDC, hBrush );
		::Polygon( hMetaDC, points, 4 );
		::SelectObject( hMetaDC, hOldPen );
		::SelectObject( hMetaDC, hOldBrush );
		m_hRightArrow = ::CloseEnhMetaFile( hMetaDC );
		
		// create disabled right arrow metafile
		hMetaDC	  = ::CreateEnhMetaFile( NULL, NULL, NULL, NULL );
		hOldPen   = (HPEN)::SelectObject( hMetaDC, hPenDisabled );
		hOldBrush = (HBRUSH)::SelectObject( hMetaDC, hBrushDisabled );
		::Polygon( hMetaDC, points, 4 );
		::SelectObject( hMetaDC, hOldPen );
		::SelectObject( hMetaDC, hOldBrush );
		m_hRightArrowDisabled = ::CloseEnhMetaFile( hMetaDC );
		
		CRect rcButton(0, 0, 16, 16);
		m_pLeftButton = new CCJMetaFileButton;
		m_pLeftButton->Create(NULL, WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_OWNERDRAW, rcButton, this, IDC_LEFTBUTTON);
		m_pLeftButton->SetMetaFiles(m_hLeftArrow, 0, 0, m_hLeftArrowDisabled);
		
		m_pRightButton = new CCJMetaFileButton;
		m_pRightButton->Create(NULL, WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_OWNERDRAW, rcButton, this, IDC_RIGHTBUTTON);
		m_pRightButton->SetMetaFiles(m_hRightArrow, 0, 0, m_hRightArrowDisabled);
		
		if (lpCreateStruct->style & FTS_HASHOMEEND)
		{
			m_bHasHomeEnd = true;

            // Setup the Home arrow button
       		points[0].x = 100;   points[0].y = 0;
		    points[1].x = 0;     points[1].y = 50;
		    points[2].x = 100;   points[2].y = 100;
		    points[3].x = 100;   points[3].y = 0;

			// create right arrow metafile
			HDC hMetaDC	     = ::CreateEnhMetaFile( NULL, NULL, NULL, NULL );
			HPEN hOldPen     = (HPEN)::SelectObject( hMetaDC, hPen );
			HBRUSH hOldBrush = (HBRUSH)::SelectObject( hMetaDC, hBrush );
			::Polygon( hMetaDC, points, 4 );
			::MoveToEx( hMetaDC, 0, 0, NULL );
			::LineTo( hMetaDC, 0, 100 );
			::SelectObject( hMetaDC, hOldPen );
			::SelectObject( hMetaDC, hOldBrush );
			m_hHomeArrow = ::CloseEnhMetaFile( hMetaDC );
		    
			// create disabled right arrow metafile
			hMetaDC	  = ::CreateEnhMetaFile( NULL, NULL, NULL, NULL );
			hOldPen   = (HPEN)::SelectObject( hMetaDC, hPenDisabled );
			hOldBrush = (HBRUSH)::SelectObject( hMetaDC, hBrushDisabled );
			::Polygon( hMetaDC, points, 4 );
			::MoveToEx( hMetaDC, 0, 0, NULL );
			::LineTo( hMetaDC, 0, 100 );
			::SelectObject( hMetaDC, hOldPen );
			::SelectObject( hMetaDC, hOldBrush );
			m_hHomeArrowDisabled = ::CloseEnhMetaFile( hMetaDC );

			m_pHomeButton = new CCJMetaFileButton;
			m_pHomeButton->Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rcButton, this, IDC_HOMEBUTTON);
			m_pHomeButton->SetMetaFiles(m_hHomeArrow, 0, 0, m_hHomeArrowDisabled);

            // Setup the End arrow button
       		points[0].x = 0;   points[0].y = 0;
		    points[1].x = 100; points[1].y = 50;
		    points[2].x = 0;   points[2].y = 100;
		    points[3].x = 0;   points[3].y = 0;

			// create right arrow metafile
			hMetaDC	  = ::CreateEnhMetaFile( NULL, NULL, NULL, NULL );
			hOldPen   = (HPEN)::SelectObject( hMetaDC, hPen );
			hOldBrush = (HBRUSH)::SelectObject( hMetaDC, hBrush );
			::Polygon( hMetaDC, points, 4 );
			::MoveToEx( hMetaDC, 100, 0, NULL );
			::LineTo( hMetaDC, 100, 100 );
			::SelectObject( hMetaDC, hOldPen );
			::SelectObject( hMetaDC, hOldBrush );
			m_hEndArrow = ::CloseEnhMetaFile( hMetaDC );
		    
			// create disabled right arrow metafile
			hMetaDC	  = ::CreateEnhMetaFile( NULL, NULL, NULL, NULL );
			hOldPen   = (HPEN)::SelectObject( hMetaDC, hPenDisabled );
			hOldBrush = (HBRUSH)::SelectObject( hMetaDC, hBrushDisabled );
			::Polygon( hMetaDC, points, 4 );
			::MoveToEx( hMetaDC, 100, 0, NULL );
			::LineTo( hMetaDC, 100, 100 );
			::SelectObject( hMetaDC, hOldPen );
			::SelectObject( hMetaDC, hOldBrush );
			m_hEndArrowDisabled = ::CloseEnhMetaFile( hMetaDC );

			m_pEndButton = new CCJMetaFileButton;
			m_pEndButton->Create(NULL, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, rcButton, this, IDC_ENDBUTTON);
			m_pEndButton->SetMetaFiles(m_hEndArrow, 0, 0, m_hEndArrowDisabled);
			
			m_pHomeButton->MoveWindow(1, 0, 16, 16);
			m_pLeftButton->MoveWindow(17, 0, 16, 16);
			m_pRightButton->MoveWindow(33, 0, 16, 16);
			m_pEndButton->MoveWindow(49, 0, 16, 16);

		} else {
			m_pLeftButton->MoveWindow(1, 0, 16, 16);
			m_pRightButton->MoveWindow(17, 0, 16, 16);
		}

		// fix potential resource leak - KStowell - 10-23-99
		::DeleteObject(hPen);
		::DeleteObject(hPenDisabled);
		::DeleteObject(hBrush);
		::DeleteObject(hBrushDisabled);
	}

	return 0;
}

void CCJFlatTabCtrl::OnLeftArrow() 
{
    // Move the tabs right, ensuring that we move right by one
    // whole tab each time ala Microsoft Access

	CPoint Point(GetTotalArrowWidth()+1, 1);

	TCHITTESTINFO test;
	test.pt = Point;
	int iTab = HitTest(&test);

    if (iTab != -1)
    {
        m_iXOffset = 0;
        for (int iLoop = 0; iLoop <= iTab - 1; iLoop++)
        {
            RECT rect;
            if (GetItemRect(iLoop, &rect))
            {
                m_iXOffset += rect.left;
            }
        }
        m_iXOffset -= GetTotalArrowWidth();

	    EnableButtons();
	    InvalidateTabs();
    }
}

void CCJFlatTabCtrl::OnRightArrow() 
{
    // Move the tabs left, ensuring that we move left by one
    // whole tab each time ala Microsoft Access

	CPoint Point(GetTotalArrowWidth()+1, 1);

	TCHITTESTINFO test;
	test.pt = Point;
	int iTab = HitTest(&test);

    if (iTab != -1)
    {
        m_iXOffset = 0;
        for (int iLoop = 0; iLoop <= iTab + 1; iLoop++)
        {
            RECT rect;
            if (GetItemRect(iLoop, &rect))
            {
                m_iXOffset += rect.left;
            }
        }
        m_iXOffset -= GetTotalArrowWidth();

	    EnableButtons();
	    InvalidateTabs();
    }
}

void CCJFlatTabCtrl::OnHomeArrow() 
{
	// TODO: Add your control notification handler code here
	m_iXOffset = 0;
	EnableButtons();
	InvalidateTabs();
}

void CCJFlatTabCtrl::OnEndArrow() 
{
	// TODO: Add your control notification handler code here
	m_iXOffset = m_iTotalTabWidth + GetTotalArrowWidth() - m_iClientWidth + 1;
	EnableButtons();
	InvalidateTabs();
}

void CCJFlatTabCtrl::SetTipText(int nItem, LPCTSTR lpszTabTip)
{
    if (nItem < m_TabList.GetCount())
    {
	    for (int i = 0; i < m_TabList.GetCount(); i++)
	    {
            if (i == nItem)
            {
                CString *pTipItem = m_TabTipList.GetAt(m_TabTipList.FindIndex(nItem));

            	*pTipItem = lpszTabTip;

                break;
            }
	    }
    }

    return;
}

CString CCJFlatTabCtrl::GetTipText(int nItem)
{
    CString strText;

    if (nItem < m_TabList.GetCount())
    {
	    for (int i = 0; i < m_TabList.GetCount(); i++)
	    {
            if (i == nItem)
            {
                strText = *m_TabTipList.GetAt(m_TabTipList.FindIndex(nItem));
                break;
            }
	    }
    }

    return strText;
}

void CCJFlatTabCtrl::Home()
{
    OnHomeArrow();
}
