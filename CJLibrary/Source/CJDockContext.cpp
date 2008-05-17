// CJDockContext.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 11/15/01 1:50p $
 * $Revision: 2 $
 * $Archive: /Projects/AddIns/ResOrg/CJLibrary/CJLibrary/CJDockContext.cpp $
 *
 * $History: CJDockContext.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 11/15/01   Time: 1:50p
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/CJLibrary
 * Added some #defines from AfxPriv.h (the Aug 2001 Platform SDK broke it)
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:25p
 * Updated in $/CodeJockey/CJLibrary
 * Added vc5 compatiblity
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 6/09/99    Time: 8:33p
 * Updated in $/CodeJockey/CJ60Lib
 * Ioannhs Stamatopoulos (Yiannhs) [ystamat@mail.datamedia.gr] - Extended
 * docking windows, removed ::GetSysColor(...) calls and WM_SYSCOLORCHANGE
 * message handler, this is now automatically handled by the base class.
 * Added redraw flags to the create method and modified paint and layout
 * handlers to reduce flicker and un-necessary command handling. Dragging
 * rect now reflects actual control bar docked size. Added "flat" look to
 * the control bars, the control bars can now use the classic DevStudio
 * look, or use flat gradient gripper, borders and frame buttons. Overall,
 * docking windows now handle dragging, docking and floating much better,
 * and behave more like DevStudio.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 5/12/99    Time: 5:32a
 * Updated in $/CodeJockey/CJ60Lib
 * Added support for vc5 compatibility
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 4/14/99    Time: 12:57p
 * Updated in $/CodeJockey/CJ60Lib
 * Daniel Harth [danielh@mail.matco.ro] - Added support for diagonal
 * sizing. 
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 12:56p
 * Updated in $/CodeJockey/CJ60Lib
 * Added comments and cleaned up code.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 3/10/99    Time: 12:56p
 * Updated in $/CodeJockey/CJ60Lib
 * Added support for toggle docking.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 3/03/99    Time: 12:54p
 * Updated in $/CodeJockey/CJ60Lib
 * Scott Killen [s_killen@yahoo.com] - If your frame is not your main
 * window in your app, you cannot dock a floating CCJControlBar. Changed
 * (CFrameWnd*)AfxGetApp()->m_pMainWnd to (CFrameWnd*)m_pDockSite in
 * CanDockDockBar() and DockSizeBar()
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 1/12/99    Time: 12:53p
 * Created in $/CodeJockey/CJ60Lib
 * Initial creation to add side-by-side docking support for CCJControlBar
 * class.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJDockContext.h"
#include "CJControlBar.h"
#include "CJSizeDockBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define HORZF(dw) (dw & CBRS_ORIENT_HORZ)
#define VERTF(dw) (dw & CBRS_ORIENT_VERT)

/////////////////////////////////////////////////////////////////////////////
// mirroring support from AfxPriv.h (the Aug 2001 Platform SDK broke it)

//WINBUG: some mirroring stuff will be in wingdi.h someday
#ifndef LAYOUT_LTR
#define LAYOUT_LTR                         0x00000000
#endif
#ifndef LAYOUT_RTL
#define LAYOUT_RTL                         0x00000001
#endif
#ifndef NOMIRRORBITMAP
#define NOMIRRORBITMAP                     0x80000000
#endif

//WINBUG: some mirroring stuff will be in winuser.h someday
#ifndef WS_EX_LAYOUTRTL
#define WS_EX_LAYOUTRTL                    0x00400000L
#endif


static void AdjustRectangle(CRect& rect, CPoint pt)
{
	int nXOffset = (pt.x < rect.left) ? (pt.x - rect.left) :
					(pt.x > rect.right) ? (pt.x - rect.right) : 0;
	int nYOffset = (pt.y < rect.top) ? (pt.y - rect.top) :
					(pt.y > rect.bottom) ? (pt.y - rect.bottom) : 0;
	rect.OffsetRect(nXOffset, nYOffset);
}

/////////////////////////////////////////////////////////////////////////////
// CCJDockContext

CCJDockContext::CCJDockContext(CControlBar* pBar) : CDockContext(pBar)
{
	// TODO: add construction code here.
}

CCJDockContext::~CCJDockContext()
{
	// TODO: add destruction code here.
}

/////////////////////////////////////////////////////////////////////////////

//1999-05-19 Yiannhs
void CCJDockContext::StartDragDockBar(CPoint pt)
{
	ASSERT_VALID(m_pBar);
	m_bDragging = TRUE;

	InitLoop();
	
	if (m_pBar->m_dwStyle & CBRS_SIZE_DYNAMIC)
	{
		CRect rect;
		m_pBar->GetWindowRect(rect);
        // Yiannhs setup and the new member
		m_ptOrig = m_ptLast = pt;
		CSize sizeHorz = m_pBar->CalcDynamicLayout(0, LM_HORZ | LM_HORZDOCK);
		CSize sizeVert = m_pBar->CalcDynamicLayout(0, LM_VERTDOCK);
		CSize sizeFloat = m_pBar->CalcDynamicLayout(0, LM_HORZ | LM_MRUWIDTH);
		
		m_rectDragHorz = CRect(rect.TopLeft(), sizeHorz);
		m_rectDragVert = CRect(rect.TopLeft(), sizeVert);
		
		// calculate frame dragging rectangle
		m_rectFrameDragHorz = CRect(rect.TopLeft(), sizeFloat);
		m_rectFrameDragVert = CRect(rect.TopLeft(), sizeFloat);
		
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragHorz);
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragVert);
		
		m_rectFrameDragHorz.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
		m_rectFrameDragVert.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
	}
	
	else if (m_pBar->m_dwStyle & CBRS_SIZE_FIXED)
	{
		CRect rect;
		m_pBar->GetWindowRect(rect);
        //Yiannhs setup and the new member
		m_ptOrig = m_ptLast = pt;
		CSize sizeHorz = m_pBar->CalcDynamicLayout(-1, LM_HORZ | LM_HORZDOCK);
		CSize sizeVert = m_pBar->CalcDynamicLayout(-1, LM_VERTDOCK);
		
		m_rectFrameDragHorz = m_rectDragHorz = CRect(rect.TopLeft(), sizeHorz);
		m_rectFrameDragVert = m_rectDragVert = CRect(rect.TopLeft(), sizeVert);
		
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragHorz);
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragVert);
		m_rectFrameDragHorz.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
		m_rectFrameDragVert.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
	}
	
	else
	{
		CRect rect;
		m_pBar->GetWindowRect(rect);
        //Yiannhs setup and the new member
		m_ptOrig = m_ptLast = pt;
		BOOL bHorz = HORZF(m_dwStyle);
		DWORD dwMode = !bHorz ? (LM_HORZ | LM_HORZDOCK) : LM_VERTDOCK;
		CSize size = m_pBar->CalcDynamicLayout(-1, dwMode);
		
		if (bHorz)
		{
			m_rectDragHorz = rect;
			m_rectDragVert = CRect(CPoint(pt.x - rect.Height()/2, rect.top), size);
		}
		
		else 
		{
			m_rectDragVert = rect;
			m_rectDragHorz = CRect(CPoint(rect.left, pt.y - rect.Width()/2), size);
		}
		
		m_rectFrameDragHorz = m_rectDragHorz;
		m_rectFrameDragVert = m_rectDragVert;
		
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragHorz);
		CMiniFrameWnd::CalcBorders(&m_rectFrameDragVert);
		m_rectFrameDragHorz.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
		m_rectFrameDragVert.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
	}
	
	AdjustRectangle(m_rectDragHorz, pt);
	AdjustRectangle(m_rectDragVert, pt);
	AdjustRectangle(m_rectFrameDragHorz, pt);
	AdjustRectangle(m_rectFrameDragVert, pt);
	
	m_dwOverDockStyle = CanDockDockBar();
	MoveDockBar(pt);   
	TrackDockBar();
}

DWORD CCJDockContext::CanDockDockBar()
{
	BOOL bStyleHorz;
	DWORD dwDock = 0; 
	DWORD dwCurr = 0; 
	
	ASSERT(m_dwStyle != 0);
	ASSERT_KINDOF(CFrameWnd, m_pDockSite);

	bStyleHorz = HORZF(m_dwStyle);
	bStyleHorz = m_bFlip ? !bStyleHorz : bStyleHorz;
	
	if (bStyleHorz && HORZF(m_dwDockStyle))
	{
		dwDock = CanDockDockBar(m_rectDragHorz,
			m_dwDockStyle & ~CBRS_ORIENT_VERT);
	}
	
	else if (VERTF(m_dwDockStyle))
	{
		dwDock = CanDockDockBar(m_rectDragVert,
			m_dwDockStyle & ~CBRS_ORIENT_HORZ);
	}
	
	if (!m_bFlip)
	{
		if (dwDock == 0 && HORZF(m_dwDockStyle))
		{
			dwCurr = CanDockDockBar(m_rectDragVert,
				m_dwDockStyle & ~CBRS_ORIENT_VERT);
			dwDock = CanDockDockBar(m_rectDragHorz,
				m_dwDockStyle & ~CBRS_ORIENT_VERT);
			dwDock = (dwDock == dwCurr) ? dwDock : 0;
		}
		
		if (dwDock == 0 && VERTF(m_dwDockStyle))
		{
			dwCurr = CanDockDockBar(m_rectDragHorz,
				m_dwDockStyle & ~CBRS_ORIENT_HORZ);
			dwDock = CanDockDockBar(m_rectDragVert,
				m_dwDockStyle & ~CBRS_ORIENT_HORZ);
			dwDock = (dwDock == dwCurr) ? dwDock : 0;
		}
	}
	
	return dwDock;
}

DWORD CCJDockContext::CanDockDockBar(CRect rect, DWORD dwDockStyle, CDockBar** ppDockBar)
{
	CFrameWnd *pFrame = (CFrameWnd*)m_pDockSite;
	ASSERT_VALID(pFrame);

	dwDockStyle &= CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI;

	if (ppDockBar != NULL)
		*ppDockBar = NULL;
	POSITION pos = pFrame->m_listControlBars.GetHeadPosition();
	while (pos != NULL)
	{
		CCJSizeDockBar* pDockBar = (CCJSizeDockBar*)pFrame->m_listControlBars.GetNext(pos);
		if( !pDockBar->IsKindOf(RUNTIME_CLASS(CCJSizeDockBar)) )
			continue;
		if (pDockBar->IsDockSizeBar() && pDockBar->IsWindowVisible() &&
			(pDockBar->m_dwStyle & dwDockStyle & CBRS_ALIGN_ANY) &&
			(!pDockBar->m_bFloating ||
				(dwDockStyle & pDockBar->m_dwStyle & CBRS_FLOAT_MULTI)))
		{
			CRect rectBar;
			pDockBar->GetWindowRect(&rectBar);
			if (rectBar.Width() == 0)
				rectBar.right++;
			if (rectBar.Height() == 0)
				rectBar.bottom++;
			if (rectBar.IntersectRect(rectBar, rect))
			{
				if (ppDockBar != NULL)
					*ppDockBar = pDockBar;
				return pDockBar->m_dwStyle & dwDockStyle;
			}
		}
	}
	return 0;
}

BOOL CCJDockContext::TrackDockBar()
{
	if (::GetCapture() != NULL)
		return FALSE;
	m_pBar->SetCapture();
	ASSERT(m_pBar == CWnd::GetCapture());
	
	while (CWnd::GetCapture() == m_pBar)
	{
		MSG msg;
		if (!::GetMessage(&msg, NULL, 0, 0))
		{
			AfxPostQuitMessage(msg.wParam);
			break;
		}
		
		switch (msg.message)
		{
		case WM_LBUTTONUP:
			if (m_bDragging)
				EndDragDockBar();
			else
				EndResize();
			return TRUE;
		case WM_MOUSEMOVE:
			if (m_bDragging)
				MoveDockBar(msg.pt);
			else
				Stretch(msg.pt);
			break;
		case WM_KEYUP:
			if (m_bDragging)
				OnKey((int)msg.wParam, FALSE);
			break;
		case WM_KEYDOWN:
			if (m_bDragging)
				OnKey((int)msg.wParam, TRUE);
			if (msg.wParam == VK_ESCAPE)
			{
				CancelLoop();
				return FALSE;
			}
			break;
		case WM_RBUTTONDOWN:
			CancelLoop();
			return FALSE;
			
			// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}
	
	CancelLoop();
	return FALSE;
}

void CCJDockContext::EndDragDockBar()
{
	CancelLoop();
    //Yiannhs: reduce some more flickering, and unexpected behaviour in floating bars
    //         ok did the expected.
	if( m_ptLast == m_ptOrig )
        return;

	if (m_dwOverDockStyle != 0)
	{
		CCJSizeDockBar* pDockBar = GetDockBar(m_dwOverDockStyle);
		if( pDockBar == NULL ) // John Armstrong
			return; 
		
		CRect rect = (m_dwOverDockStyle & CBRS_ORIENT_VERT) ?
			m_rectDragVert : m_rectDragHorz;
		
		UINT uID = _AfxGetDlgCtrlID(pDockBar->m_hWnd);
		m_uMRUDockID = uID;
		m_rectMRUDockPos = rect;
		pDockBar->ScreenToClient(&m_rectMRUDockPos);
		DockSizeBar(m_pBar, pDockBar, &rect);

        //yiannhs. after testing the posibilities I think this is the best way
        m_pDockSite->RecalcLayout( FALSE );
	}
	
	else if ((m_dwStyle & CBRS_SIZE_DYNAMIC) || (HORZF(m_dwStyle) && !m_bFlip) ||
		(VERTF(m_dwStyle) && m_bFlip))
	{
		m_dwMRUFloatStyle = CBRS_ALIGN_TOP | (m_dwDockStyle & CBRS_FLOAT_MULTI);
		m_ptMRUFloatPos = m_rectFrameDragHorz.TopLeft();
		m_pDockSite->FloatControlBar(m_pBar, m_ptMRUFloatPos, m_dwMRUFloatStyle);

        //yiannhs. after testing the posibilities I think this is the best way  
        m_pDockSite->RecalcLayout( FALSE );
	}
	
	else 
	{
		m_dwMRUFloatStyle = CBRS_ALIGN_LEFT | (m_dwDockStyle & CBRS_FLOAT_MULTI);
		m_ptMRUFloatPos = m_rectFrameDragVert.TopLeft();
		m_pDockSite->FloatControlBar(m_pBar, m_ptMRUFloatPos, m_dwMRUFloatStyle);

        //yiannhs. after testing the posibilities I think this is the best way
        m_pDockSite->RecalcLayout( FALSE );
	}
}

CCJSizeDockBar* CCJDockContext::GetDockBar(DWORD dwOverDockStyle)
{
	DWORD dw = 0;
	CDockBar* pBar;
	if (HORZF(dwOverDockStyle))
	{
		ASSERT_KINDOF(CFrameWnd, m_pDockSite);
		dw = CanDockDockBar(m_rectDragHorz,
			dwOverDockStyle & ~CBRS_ORIENT_VERT, &pBar);
		ASSERT(dw != 0);
		ASSERT(pBar != NULL);
		return (CCJSizeDockBar*)pBar;
	}
	
	if (VERTF(dwOverDockStyle))
	{
		dw = CanDockDockBar(m_rectDragVert,
			dwOverDockStyle & ~CBRS_ORIENT_HORZ, &pBar);
		ASSERT(dw != 0);
		ASSERT(pBar != NULL);
		return (CCJSizeDockBar*)pBar;
	}
	
	return NULL;
}

void CCJDockContext::MoveDockBar(CPoint pt)
{
	CPoint ptOffset = pt - m_ptLast;
	
	m_rectDragHorz.OffsetRect(ptOffset);
	m_rectFrameDragHorz.OffsetRect(ptOffset);
	m_rectDragVert.OffsetRect(ptOffset);
	m_rectFrameDragVert.OffsetRect(ptOffset);
	m_ptLast = pt;
	m_dwOverDockStyle = m_bForceFrame ? 0 : CanDockDockBar();
	DrawFocusRect();
}

void CCJDockContext::DockSizeBar(CControlBar * pBar,CCJSizeDockBar *pDockBar, LPRECT lpRect)
{
	CFrameWnd *pFrame = (CFrameWnd*)m_pDockSite;
	ASSERT_VALID(pFrame);

	ASSERT(pBar != NULL);
	ASSERT(pBar->m_pDockContext != NULL);
	
	if( pDockBar == NULL )
	{
		for (int i = 0; i < 4; i++)
		{
			if ((dwSizeBarMap[i][1] & CBRS_ALIGN_ANY) ==
				(pBar->m_dwStyle & CBRS_ALIGN_ANY))
			{
				pDockBar = (CCJSizeDockBar*)pFrame->GetControlBar(dwSizeBarMap[i][0]);
				ASSERT(pDockBar != NULL);
				break;
			}
		}
	}
	
	ASSERT(pDockBar != NULL);
	ASSERT(pFrame->m_listControlBars.Find(pBar) != NULL);
	ASSERT(pBar->m_pDockSite == pFrame);
	((CCJControlBar*)pBar)->Normalize();
	pDockBar->DockControlBar(pBar, lpRect);
}

void CCJDockContext::ToggleDocking()
{
	if (m_pBar->IsKindOf(RUNTIME_CLASS(CCJControlBar)))
	{
		if (m_pBar->IsFloating())
		{
			// Dock it only if is allowed to be docked
			if (m_pBar->m_dwDockStyle & CBRS_ALIGN_ANY)
			{
				ASSERT((m_uMRUDockID >= AFX_IDW_DOCKBAR_TOP && m_uMRUDockID <= AFX_IDW_SIZEBAR_BOTTOM) ||
					m_uMRUDockID == 0);

				CRect rect = m_rectMRUDockPos;
				CCJSizeDockBar* pDockBar = NULL;
				if (m_uMRUDockID != 0)
				{
					pDockBar = (CCJSizeDockBar*)m_pDockSite->GetControlBar(m_uMRUDockID);
					pDockBar->ClientToScreen(&rect);
				}

				// dock it at the specified position, RecalcLayout will snap
				DockSizeBar(m_pBar, pDockBar, &rect);
				m_pDockSite->RecalcLayout();
			}
		}
		else
		{
			CPoint ptFloat = m_ptMRUFloatPos;
			if (ptFloat.x < 0 || ptFloat.y < 0)
			{
				ptFloat = m_rectMRUDockPos.TopLeft();
				m_pBar->GetParent()->ClientToScreen(&ptFloat);
			}
			m_pDockSite->FloatControlBar(m_pBar, ptFloat, m_dwMRUFloatStyle);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDockContext Resize Operations

#define m_rectRequestedSize     m_rectDragHorz
#define m_rectActualSize        m_rectDragVert
#define m_rectActualFrameSize   m_rectFrameDragHorz
#define m_rectFrameBorders      m_rectFrameDragVert

void CCJDockContext::StartResize(int nHitTest, CPoint pt)
{
    ASSERT_VALID(m_pBar);
    ASSERT(m_pBar->m_dwStyle & CBRS_SIZE_DYNAMIC);
    m_bDragging = FALSE;
	
    InitLoop();
	
    // GetWindowRect returns screen coordinates(not mirrored)
    // So if the desktop is mirrored then turn off mirroring
    // for the desktop dc so that we draw correct focus rect
#ifndef _VC_VERSION_5
    if (m_pDC->GetLayout() & LAYOUT_RTL)
        m_pDC->SetLayout(LAYOUT_LTR);
#endif // _VC_VERSION_5

    // get true bar size (including borders)
    CRect rect;
    m_pBar->GetWindowRect(rect);
    m_ptLast = pt;
    m_nHitTest = nHitTest;
	
    CSize size = m_pBar->CalcDynamicLayout(0, LM_HORZ | LM_MRUWIDTH);
    m_rectRequestedSize = CRect(rect.TopLeft(), size);
    m_rectActualSize = CRect(rect.TopLeft(), size);
    m_rectActualFrameSize = CRect(rect.TopLeft(), size);
	
    // calculate frame rectangle
    CMiniFrameWnd::CalcBorders(&m_rectActualFrameSize);
    m_rectActualFrameSize.InflateRect(-afxData.cxBorder2, -afxData.cyBorder2);
	
    m_rectFrameBorders = CRect(CPoint(0,0),
        m_rectActualFrameSize.Size() - m_rectActualSize.Size());
	
    // initialize tracking state and enter tracking loop
    m_dwOverDockStyle = 0;
    Stretch(pt);   // call it here to handle special keys
    TrackDockBar();
}

void CCJDockContext::Stretch(CPoint pt)
{
    CPoint ptOffset = pt - m_ptLast;
    CSize size;
	
    // offset all drag rects to new position
    if (m_nHitTest != HTTOP && m_nHitTest != HTBOTTOM)
    {
        if (m_nHitTest == HTLEFT || m_nHitTest == HTTOPLEFT || m_nHitTest == HTBOTTOMLEFT)
            m_rectRequestedSize.left += ptOffset.x;
        else
            m_rectRequestedSize.right += ptOffset.x;
        size = m_pBar->CalcDynamicLayout(m_rectRequestedSize.Width(), LM_HORZ);
    }
	
    if (m_nHitTest != HTLEFT && m_nHitTest != HTRIGHT)
    {
        if (m_nHitTest == HTTOP || m_nHitTest == HTTOPLEFT || m_nHitTest == HTTOPRIGHT)
            m_rectRequestedSize.top += ptOffset.y;
        else
            m_rectRequestedSize.bottom += ptOffset.y;
        size = m_pBar->CalcDynamicLayout(m_rectRequestedSize.Height(), LM_HORZ|LM_LENGTHY);
    }
	
    CRect rectDesk;
    HWND hWndDesk = ::GetDesktopWindow();
    ::GetWindowRect(hWndDesk, &rectDesk);
    CRect rectTemp = m_rectActualFrameSize;
	
    // left, top, top left
    if (m_nHitTest == HTLEFT || m_nHitTest == HTTOP || m_nHitTest == HTTOPLEFT)
    {
        rectTemp.left = rectTemp.right -
            (size.cx + m_rectFrameBorders.Width());
        rectTemp.top = rectTemp.bottom -
            (size.cy + m_rectFrameBorders.Height());
        CRect rect;
        if (rect.IntersectRect(rectDesk, rectTemp))
        {
            m_rectActualSize.left = m_rectActualSize.right - size.cx;
            m_rectActualSize.top = m_rectActualSize.bottom - size.cy;
            m_rectActualFrameSize.left = rectTemp.left;
            m_rectActualFrameSize.top = rectTemp.top;
        }
    } else if (m_nHitTest == HTTOPRIGHT) // top right
    {
        rectTemp.top = rectTemp.bottom -
            (size.cy + m_rectFrameBorders.Height());
        rectTemp.right = rectTemp.left +
            (size.cx + m_rectFrameBorders.Width());
        CRect rect;
        if (rect.IntersectRect(rectDesk, rectTemp))
        {
            m_rectActualSize.left = m_rectActualSize.right - size.cx;
            m_rectActualSize.bottom = m_rectActualSize.top + size.cy;
            m_rectActualFrameSize.right = rectTemp.right;
            m_rectActualFrameSize.top = rectTemp.top;
        }
    } else if (m_nHitTest == HTBOTTOMLEFT) // bottom left
    {
        rectTemp.bottom = rectTemp.top +
            (size.cy + m_rectFrameBorders.Height());
        rectTemp.left = rectTemp.right -
            (size.cx + m_rectFrameBorders.Width());
        CRect rect;
        if (rect.IntersectRect(rectDesk, rectTemp))
        {
            m_rectActualSize.right = m_rectActualSize.left + size.cx;
            m_rectActualSize.top = m_rectActualSize.bottom - size.cy;
            m_rectActualFrameSize.left = rectTemp.left;
            m_rectActualFrameSize.bottom = rectTemp.bottom;
        }
    } else// bottom, right, bottom right
    {
        rectTemp.right = rectTemp.left +
            (size.cx + m_rectFrameBorders.Width());
        rectTemp.bottom = rectTemp.top +
            (size.cy + m_rectFrameBorders.Height());
        CRect rect;
        if (rect.IntersectRect(rectDesk, rectTemp))
        {
            m_rectActualSize.right = m_rectActualSize.left + size.cx;
            m_rectActualSize.bottom = m_rectActualSize.top + size.cy;
            m_rectActualFrameSize.right = rectTemp.right;
            m_rectActualFrameSize.bottom = rectTemp.bottom;
        }
    }
    m_ptLast = pt;
	
    // update feedback
    DrawFocusRect();
}
