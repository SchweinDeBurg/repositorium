// CJExplorerBar.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/25/99 10:52p $
 * $Revision: 12 $
 * $Archive: /CodeJock/CJLibrary/CJExplorerBar.cpp $
 *
 * $History: CJExplorerBar.cpp $
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 10/25/99   Time: 10:52p
 * Updated in $/CodeJock/CJLibrary
 * Modified resource include for static builds.
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/CJLibrary
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:14p
 * Updated in $/CodeJockey/CJLibrary
 * Added Unicode compliance, thanks to Barry Burton for his help with
 * this.
 * 
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 7:24a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 2:07a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:26p
 * Updated in $/CodeJockey/CJLibrary
 * Cleaned up caption creation.
 * 
 * *****************  Version 4  *****************
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
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 4:23p
 * Updated in $/CodeJockey/CJ60Lib
 * Added comments and cleaned up code.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 1/31/99    Time: 4:23p
 * Updated in $/CodeJockey/CJ60Lib
 * Made some cosmetic enhancements to more closely match the windows
 * explorer bar.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 1/16/99    Time: 4:22p
 * Created in $/CodeJockey/CJ60Lib
 * Initial release
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJResource.h"
#include "CJExplorerBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJExplorerBar

CCJExplorerBar::CCJExplorerBar()
{
	m_hIcon = NULL;
	m_strCaption = _T("");
}

CCJExplorerBar::~CCJExplorerBar()
{
	// fix potential resource leak - KStowell - 10-22-99.
	_destroyIcon(m_hIcon);
}

IMPLEMENT_DYNAMIC(CCJExplorerBar, CCJControlBar)

BEGIN_MESSAGE_MAP(CCJExplorerBar, CCJControlBar)
	//{{AFX_MSG_MAP(CCJExplorerBar)
	ON_WM_CREATE()
	ON_COMMAND(IDC_BAR_BUTTON, OnButtonClose)
	ON_UPDATE_COMMAND_UI(IDC_BAR_BUTTON, OnUpdateButtonClose)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJExplorerBar message handlers

BOOL CCJExplorerBar::Create(CWnd *pParentWnd, UINT nID, LPCTSTR lpszWindowName, CSize sizeDefault, DWORD dwStyle)
{
	m_strCaption = lpszWindowName;

	if(!CCJControlBar::Create(pParentWnd, nID, lpszWindowName, sizeDefault, dwStyle)) {
		return FALSE;
	}

	ShowFrameControls(FALSE, FALSE);
	return TRUE;
}

int CCJExplorerBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCJControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Create the caption.
	if (!m_Caption.Create(this, m_strCaption))
	{
		TRACE0( "Unable to create caption.\n" );
		return -1;
	}

	// Create the caption button.
	if (!m_CaptionButton.Create(NULL, WS_VISIBLE|BS_ICON|BS_OWNERDRAW|BS_CENTER|BS_VCENTER,
		CRect(0,0,0,0), this, IDC_BAR_BUTTON))
	{
		TRACE0( "Unable to create caption button.\n" );
		return -1;
	}
	
	// Create the image list used by frame buttons.
	CImageList imageList;
	imageList.Create(IDB_PUSHPIN, 16, 1, RGB(255,0,255));
	m_hIcon = imageList.ExtractIcon(2);

	// Associate the button icon
	m_CaptionButton.SetIcon(m_hIcon, CSize(16,15));
	imageList.DeleteImageList();

	return 0;
}

void CCJExplorerBar::OnButtonClose()
{
	GetDockingFrame()->ShowControlBar(this, FALSE, FALSE);
}

void CCJExplorerBar::OnUpdateButtonClose(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CCJExplorerBar::GetChildRect(CRect &rect)
{
	GetClientRect(&rect);
	
	if (!IsFloating())
	{
		rect.left	+= 1;
		rect.right  -= 1;
		rect.top	+= (IsVertDocked()?26:22);
	}

	else
		rect.DeflateRect(2,2);
}

void CCJExplorerBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CControlBar::OnWindowPosChanged(lpwndpos);
	
	int nCmdShow = IsFloating() ? SW_HIDE:SW_SHOW;
	m_Caption.ShowWindow(nCmdShow);
	m_CaptionButton.ShowWindow(nCmdShow);

	if (!IsFloating())
	{
		if (IsVertDocked())
		{
			m_Caption.MoveWindow(1,4,lpwndpos->cx-1,22);
			m_CaptionButton.MoveWindow(CRect(lpwndpos->cx-25,
				7, lpwndpos->cx-5, 24));
		}
		else
		{
			m_Caption.MoveWindow(1,0,lpwndpos->cx-1,22);
			m_CaptionButton.MoveWindow(CRect(lpwndpos->cx-25,
				3, lpwndpos->cx-5, 20));
		}
	}

	if (m_pChildWnd->GetSafeHwnd())
	{
		CRect rc;
		GetChildRect(rc);
		m_pChildWnd->MoveWindow(rc);
	}
}

BOOL CCJExplorerBar::OnEraseBkgnd(CDC* pDC) 
{
	int result = CControlBar::OnEraseBkgnd(pDC);

	CRect rect;
	GetClientRect(rect);
	rect.top+=(IsVertDocked()?3:-1);
	rect.bottom += 1;
    //Yiannhs This was a side effect of removing the variables
    pDC->Draw3dRect(rect, afxData.clrBtnShadow, afxData.clrBtnShadow);
	//pDC->Draw3dRect(rect, m_clrBtnShadow, m_clrBtnShadow);
	return result;
}

void CCJExplorerBar::OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/) 
{
	// bypass CCJControlBar.
}

void CCJExplorerBar::OnLButtonUp(UINT /*nFlags*/, CPoint /*point*/) 
{
	// bypass CCJControlBar.
}
