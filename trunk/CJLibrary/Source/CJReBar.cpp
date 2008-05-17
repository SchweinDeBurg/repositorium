// CJReBar.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Copied from MFC, Visual C++ 6, to extend functionality to Visual C++ 5 
// users, with some ideas taken from Luis Barreira's article 'Classes for new IE4 controls'
// http://www.codeguru.com/controls/ie4_controls_classes.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 8/31/99 1:11a $
 * $Revision: 8 $
 * $Archive: /CodeJockey/CJLibrary/CJReBar.cpp $
 *
 * $History: CJReBar.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 7/31/99    Time: 4:26p
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:13a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:51a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 10:29p
 * Created in $/CodeJockey/CJLibrary
 * Copied from MFC v6 and techno preview for v5. Added to extend
 * functionality to Visual C++ 5.0 users.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJReBar.h"
#include "CJToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define _CjxGetDlgCtrlID(hWnd)          ((UINT)(WORD)::GetDlgCtrlID(hWnd))

static HWND _CjxChildWindowFromPoint(HWND hWnd, POINT pt)
{
  ASSERT(hWnd != NULL);

  // check child windows
  ::ClientToScreen(hWnd, &pt);
  HWND hWndChild = ::GetWindow(hWnd, GW_CHILD);
  for (; hWndChild != NULL; hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT))
  {
    if (_CjxGetDlgCtrlID(hWndChild) != (WORD)-1 &&
      (::GetWindowLong(hWndChild, GWL_STYLE) & WS_VISIBLE))
    {
      // see if point hits the child window
      CRect rect;
      ::GetWindowRect(hWndChild, rect);
      if (rect.PtInRect(pt))
        return hWndChild;
    }
  }

  return NULL;    // not found
}

/////////////////////////////////////////////////////////////////////////////
// CCJReBar

BEGIN_MESSAGE_MAP(CCJReBar, CCJToolBarBase)
	//{{AFX_MSG_MAP(CCJReBar)
	ON_WM_NCCREATE()
	ON_WM_PAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_ERASEBKGND()
	ON_WM_NCPAINT()
	ON_NOTIFY_REFLECT(RBN_HEIGHTCHANGE, OnHeightChange)
	ON_NOTIFY_REFLECT(RBN_ENDDRAG, OnHeightChange)
	ON_MESSAGE(RB_SHOWBAND, OnShowBand)
	ON_MESSAGE_VOID(WM_RECALCPARENT, OnRecalcParent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CCJReBar::CCJReBar()
{
	SetBorders();
}

void CCJReBar::OnRecalcParent()
{
	CFrameWnd* pFrameWnd = GetParentFrame();
	ASSERT(pFrameWnd != NULL);
	pFrameWnd->RecalcLayout();
}

void CCJReBar::OnHeightChange(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	CFrameWnd* pFrameWnd = GetParentFrame();
	if (pFrameWnd != NULL)
		pFrameWnd->RecalcLayout();
	*pResult = 0;
}

LRESULT CCJReBar::OnShowBand(WPARAM wParam, LPARAM)
{
	LRESULT lResult = Default();
	if (lResult)
	{
		// keep window visible state in sync with band visible state
		REBARBANDINFO rbBand;
		rbBand.cbSize = sizeof(rbBand);
		rbBand.fMask = RBBIM_CHILD|RBBIM_STYLE;
		VERIFY(DefWindowProc(RB_GETBANDINFO, wParam, (LPARAM)&rbBand));
		CCJToolBarBase* pBar = DYNAMIC_DOWNCAST(CCJToolBarBase, CWnd::FromHandlePermanent(rbBand.hwndChild));
		BOOL bWindowVisible;
		if (pBar != NULL)
			bWindowVisible = pBar->IsVisible();
		else
			bWindowVisible =  (::GetWindowLong(rbBand.hwndChild, GWL_STYLE) & WS_VISIBLE) != 0;
		BOOL bBandVisible = (rbBand.fStyle & RBBS_HIDDEN) == 0;
		if (bWindowVisible != bBandVisible)
			VERIFY(::ShowWindow(rbBand.hwndChild, bBandVisible ? SW_SHOW : SW_HIDE));
	}
	return lResult;
}

BOOL CCJReBar::_AddBar(CWnd* pBar, REBARBANDINFO* pRBBI)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pBar != NULL);
	ASSERT(::IsWindow(pBar->m_hWnd));
	
	pRBBI->cbSize = sizeof(REBARBANDINFO);
	pRBBI->fMask |= RBBIM_CHILD | RBBIM_CHILDSIZE;
	pRBBI->hwndChild = pBar->m_hWnd;
	
	CSize size;
	CCJToolBarBase* pTemp = DYNAMIC_DOWNCAST(CCJToolBarBase, pBar);
	if (pTemp != NULL)
	{
		pTemp->m_bInReBar = true;
		size = pTemp->CalcFixedLayout(FALSE, m_dwStyle & CBRS_ORIENT_HORZ);
	}
	else
	{
		CRect rect;
		pBar->GetWindowRect(&rect);
		size = rect.Size();
	}
	//WINBUG: COMCTL32.DLL is off by 4 pixels in its sizing logic.  Whatever
	//  is specified as the minimum size, the system rebar will allow that band
	//  to be 4 actual pixels smaller!  That's why we add 4 to the size here.
	pRBBI->cxMinChild = size.cx + 4;
	pRBBI->cyMinChild = size.cy;
	BOOL bResult = (BOOL)DefWindowProc(RB_INSERTBAND, (WPARAM)-1, (LPARAM)pRBBI);
	
	CFrameWnd* pFrameWnd = GetParentFrame();
	if (pFrameWnd != NULL)
		pFrameWnd->RecalcLayout();
	
	return bResult;
}

BOOL CCJReBar::AddBar(CWnd* pBar, LPCTSTR pszText, CBitmap* pbmp, DWORD dwStyle)
{
	REBARBANDINFO rbBand;
	rbBand.fMask = RBBIM_STYLE;
	rbBand.fStyle = dwStyle;
	if (pszText != NULL)
	{
		rbBand.fMask |= RBBIM_TEXT;
		rbBand.lpText = const_cast<LPTSTR>(pszText);
	}
	if (pbmp != NULL)
	{
		rbBand.fMask |= RBBIM_BACKGROUND;
		rbBand.hbmBack = (HBITMAP)*pbmp;
	}
	return _AddBar(pBar, &rbBand);
}

BOOL CCJReBar::AddBar(CWnd* pBar, COLORREF clrFore, COLORREF clrBack, LPCTSTR pszText, DWORD dwStyle)
{
	REBARBANDINFO rbBand;
	rbBand.fMask = RBBIM_STYLE | RBBIM_COLORS;
	rbBand.fStyle = dwStyle;
	rbBand.clrFore = clrFore;
	rbBand.clrBack = clrBack;
	if (pszText != NULL)
	{
		rbBand.fMask |= RBBIM_TEXT;
		rbBand.lpText = const_cast<LPTSTR>(pszText);
	}
	return _AddBar(pBar, &rbBand);
}

CSize CCJReBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));
	
	// the union of the band rectangles is the total bounding rect
	int nCount = DefWindowProc(RB_GETBANDCOUNT, 0, 0);
	REBARBANDINFO rbBand;
	rbBand.cbSize = sizeof(rbBand);
	int nTemp;
	
	// sync up hidden state of the bands
	for (nTemp = nCount; nTemp--; )
	{
		rbBand.fMask = RBBIM_CHILD|RBBIM_STYLE;
		VERIFY(DefWindowProc(RB_GETBANDINFO, nTemp, (LPARAM)&rbBand));
		CCJToolBarBase* pBar = DYNAMIC_DOWNCAST(CCJToolBarBase, CWnd::FromHandlePermanent(rbBand.hwndChild));
		BOOL bWindowVisible;
		if (pBar != NULL)
			bWindowVisible = pBar->IsVisible();
		else
			bWindowVisible =  (::GetWindowLong(rbBand.hwndChild, GWL_STYLE) & WS_VISIBLE) != 0;
		BOOL bBandVisible = (rbBand.fStyle & RBBS_HIDDEN) == 0;
		if (bWindowVisible != bBandVisible)
			VERIFY(DefWindowProc(RB_SHOWBAND, nTemp, bWindowVisible));
	}
	
	// determine bounding rect of all visible bands
	CRect rectBound; rectBound.SetRectEmpty();
	for (nTemp = nCount; nTemp--; )
	{
		rbBand.fMask = RBBIM_STYLE;
		VERIFY(DefWindowProc(RB_GETBANDINFO, nTemp, (LPARAM)&rbBand));
		if ((rbBand.fStyle & RBBS_HIDDEN) == 0)
		{
			CRect rect;
			VERIFY(DefWindowProc(RB_GETRECT, nTemp, (LPARAM)&rect));
			rectBound |= rect;
		}
	}
	
	// add borders as part of bounding rect
	if (!rectBound.IsRectEmpty())
	{
		CRect rect; rect.SetRectEmpty();
		CalcInsideRect(rect, bHorz);
		rectBound.right -= rect.Width();
		rectBound.bottom -= rect.Height();
	}
	
	return CSize((bHorz && bStretch) ? 32767 : rectBound.Width(),
		(!bHorz && bStretch) ? 32767 : rectBound.Height());
}

CSize CCJReBar::CalcDynamicLayout(int /*nLength*/, DWORD dwMode)
{
	if (dwMode & LM_HORZDOCK)
		ASSERT(dwMode & LM_HORZ);
	return CalcFixedLayout(dwMode & LM_STRETCH, dwMode & LM_HORZ);
}

BOOL CCJReBar::Create(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, UINT nID)
{
	ASSERT_VALID(pParentWnd);   // must have a parent
	ASSERT (!((dwStyle & CBRS_SIZE_FIXED) && (dwStyle & CBRS_SIZE_DYNAMIC)));
	
	// save the style
	m_dwStyle = (dwStyle & CBRS_ALL);
	if (nID == AFX_IDW_REBAR)
		m_dwStyle |= CBRS_HIDE_INPLACE;
	
	dwStyle &= ~CBRS_ALL;
	dwStyle |= CCS_NOPARENTALIGN|CCS_NOMOVEY|CCS_NODIVIDER|CCS_NORESIZE|RBS_VARHEIGHT;
	dwStyle |= dwCtrlStyle;
	
	// initialize common controls
#if _MSC_VER >= 1200
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_COOL_REG));
#else
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_COOL_CLASSES;
	VERIFY(InitCommonControlsEx(&icex));
#endif

	_GetComCtlVersion();
	ASSERT(_ComCtlVersion != -1);
	
	// create the HWND
	CRect rect; rect.SetRectEmpty();
	if (!CWnd::Create(REBARCLASSNAME, NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;
	
	// Note: Parent must resize itself for control bar to be resized
	m_bInReBar = true;
	
	return TRUE;
}

void CCJReBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler)
{
	UpdateDialogControls(pTarget, bDisableIfNoHandler);
}

BOOL CCJReBar::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!CCJToolBarBase::OnNcCreate(lpCreateStruct))
		return FALSE;
	
	// if the owner was set before the rebar was created, set it now
	if (m_hWndOwner != NULL)
		DefWindowProc(RB_SETPARENT, (WPARAM)m_hWndOwner, 0);
	
	return TRUE;
}

BOOL CCJReBar::OnEraseBkgnd(CDC*)
{
	return (BOOL)Default();
}

void CCJReBar::OnNcCalcSize(BOOL /*bCalcValidRects*/, NCCALCSIZE_PARAMS* lpncsp)
{
	// calculate border space (will add to top/bottom, subtract from right/bottom)
	CRect rect; rect.SetRectEmpty();
	BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) != 0;
	CCJToolBarBase::CalcInsideRect(rect, bHorz);
	
	// adjust non-client area for border space
	lpncsp->rgrc[0].left += rect.left;
	lpncsp->rgrc[0].top += rect.top;
	lpncsp->rgrc[0].right += rect.right;
	lpncsp->rgrc[0].bottom += rect.bottom;
}

void CCJReBar::OnNcPaint()
{
	EraseNonClient();
}

void CCJReBar::OnPaint()
{
	Default();
}

int CCJReBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));
	
	HWND hWndChild = _CjxChildWindowFromPoint(m_hWnd, point);
	CWnd* pWnd = CWnd::FromHandlePermanent(hWndChild);
	if (pWnd == NULL)
		return CCJToolBarBase::OnToolHitTest(point, pTI);
	
	ASSERT(pWnd->m_hWnd == hWndChild);
	return pWnd->OnToolHitTest(point, pTI);
}

LRESULT CCJReBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// special handling for certain messages (forwarding to owner/parent)
	switch (message)
	{
	case WM_POPMESSAGESTRING:
	case WM_SETMESSAGESTRING:
		return GetOwner()->SendMessage(message, wParam, lParam);
	}
	return CCJToolBarBase::WindowProc(message, wParam, lParam);
}

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

IMPLEMENT_DYNAMIC(CCJReBar, CCJToolBarBase)

/////////////////////////////////////////////////////////////////////////////
