// CJFrameWnd.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/31/99 11:04p $
 * $Revision: 16 $
 * $Archive: /CodeJock/CJLibrary/CJFrameWnd.cpp $
 *
 * $History: CJFrameWnd.cpp $
 * 
 * *****************  Version 16  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:04p
 * Updated in $/CodeJock/CJLibrary
 * Minor update to PreCreateWindow(...) for flicker free drawing.
 * 
 * *****************  Version 15  *****************
 * User: Kirk Stowell Date: 10/29/99   Time: 12:43a
 * Updated in $/CodeJock/CJLibrary
 * Overloaded PreCreateWindow(...) for flicker free drawing.
 * 
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 6:15p
 * Updated in $/CodeJock/CJLibrary
 * Fixed bug with a document becomes the active document when the user
 * clicks inside a tab in the CCJTabControlBar to perform some operation.
 * When closing the application, MFC decides in CFrameWnd::OnClose() to
 * check if the 'active' document is modified before checking all other
 * documuents. If the active document is not modified and can close its
 * frame, it does not check other documents individually and shuts down
 * without asking the important 'modified save' questions. To fix this,
 * OnClose() was overriden to add a call to "SetActiveView(NULL)" just
 * before the base class OnClose is called. This will check all documents
 * asking the save question before closing the application. Thanks to
 * Simon Clark [spc@otleda.com] for help with this.
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 10:05a
 * Updated in $/CodeJock/CJLibrary
 * Overrode OnEraseBkgnd(...) to help to eliminate screen flicker.
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 9/16/99    Time: 2:35p
 * Updated in $/CodeJockey/CJLibrary
 * Fixed bug with save / restore bar state, and with saved bar size
 * changing whenever the application starts.
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 6/11/99    Time: 8:25p
 * Updated in $/CodeJockey/CJ60Lib
 * Slight modification made to popup menu code.
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
 * User: Kirk Stowell Date: 5/02/99    Time: 5:23p
 * Updated in $/CodeJockey/CJ60Lib
 * Further modifications for CCJMenu support.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 2:18a
 * Updated in $/CodeJockey/CJ60Lib
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 2:13a
 * Updated in $/CodeJockey/CJ60Lib
 * Re-wrote CCJMenu initialization routines with a more cleaner approach.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 4:47p
 * Updated in $/CodeJockey/CJ60Lib
 * Added comments and cleaned up code.  Added support for Brent Corkum's 
 * BCMenu (CCJMenu), which is a replacement for the CCoolMenu class.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 1/12/99    Time: 4:46p
 * Updated in $/CodeJockey/CJ60Lib
 * Total re-write to add side-by-side docking support for CCJControlBar
 * class.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/17/98   Time: 4:46p
 * Created in $/CodeJockey/CJ60Lib
 * Initial re-write and release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJFrameWnd.h"
#include "CJDockBar.h"
#include "CJSizeDockBar.h"
#include "CJControlBar.h"
#include "CJMiniDockFrameWnd.h"
#include "CJToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJFrameWnd

CCJFrameWnd::CCJFrameWnd()
{

}

CCJFrameWnd::~CCJFrameWnd()
{
}

IMPLEMENT_DYNCREATE(CCJFrameWnd, CFrameWnd)

BEGIN_MESSAGE_MAP(CCJFrameWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CCJFrameWnd)
	ON_WM_MEASUREITEM()
	ON_WM_MENUCHAR()
	ON_WM_INITMENUPOPUP()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJFrameWnd message handlers

// dwDockBarMap
DWORD CCJFrameWnd::dwDockBarMap[4][2] =
{
	{ AFX_IDW_DOCKBAR_TOP,      CBRS_TOP    },
	{ AFX_IDW_DOCKBAR_BOTTOM,   CBRS_BOTTOM },
	{ AFX_IDW_DOCKBAR_LEFT,     CBRS_LEFT   },
	{ AFX_IDW_DOCKBAR_RIGHT,    CBRS_RIGHT  },
};

void CCJFrameWnd::DockControlBarLeftOf(CControlBar* Bar, CControlBar* LeftOf)
{
	CRect rect;
	DWORD dw;
	UINT n;
	
	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout(TRUE);
	
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP)				 ? AFX_IDW_DOCKBAR_TOP		: n;
	n = (dw&CBRS_ALIGN_BOTTOM	&& n==0) ? AFX_IDW_DOCKBAR_BOTTOM	: n;
	n = (dw&CBRS_ALIGN_LEFT		&& n==0) ? AFX_IDW_DOCKBAR_LEFT		: n;
	n = (dw&CBRS_ALIGN_RIGHT	&& n==0) ? AFX_IDW_DOCKBAR_RIGHT	: n;
	
	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line. By calculating a rectangle, we in effect
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(Bar,n,&rect);
}

void CCJFrameWnd::EnableDocking(DWORD dwDockStyle)
{
	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);
	CFrameWnd::EnableDocking(dwDockStyle);
	
	for (int i = 0; i < 4; i++) {
		if (dwDockBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY) {
			CDockBar* pDock = (CDockBar*)GetControlBar(dwDockBarMap[i][0]);
			
			// make sure the dock bar is of correct type
			if( pDock || !pDock->IsKindOf(RUNTIME_CLASS(CCJDockBar)) ) {
				BOOL bNeedDelete = ! pDock->m_bAutoDelete;
				pDock->m_pDockSite->RemoveControlBar(pDock);
				pDock->m_pDockSite = 0;	// avoid problems in destroying the dockbar
				pDock->DestroyWindow();
				if( bNeedDelete )
					delete pDock;
				pDock = 0;
			}
			
			if( pDock == 0 ) {
				pDock = new CCJDockBar;
				if (!pDock->Create(this,
					WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE |
					dwDockBarMap[i][1], dwDockBarMap[i][0])) {
					AfxThrowResourceException();
				}
			}
		}
	}
}

void CCJFrameWnd::RecalcLayout(BOOL bNotify) 
{
	CFrameWnd::RecalcLayout(bNotify);
	RecalcAllExcept(NULL);
}

// Yiannhs changed this
void CCJFrameWnd::EnableDockingSizeBar(DWORD dwDockStyle, DockFlatBorders FlatFlags )
{
	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);

	m_pFloatingFrameClass = RUNTIME_CLASS(CCJMiniDockFrameWnd);
	for (int i = 0; i < 4; i++)
	{
		if (dwSizeBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY)
		{
			CCJSizeDockBar* pDock = (CCJSizeDockBar*)GetControlBar(dwSizeBarMap[i][0]);
			if (pDock == NULL)
			{
				pDock = new CCJSizeDockBar;

				if (!pDock->Create(this,
					WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE |
						dwSizeBarMap[i][1], dwSizeBarMap[i][0]))
				{
					AfxThrowResourceException();
				}
                //Yiannhs enable Flat Borders For dock Bars
                pDock->FlatBorder( (FlatFlags >> i) & 0x0001 ); 
			}
		}
	}
}

void CCJFrameWnd::RecalcAllExcept(CCJSizeDockBar *pBar)
{
	for (int i = 0; i < 4; i++)
	{
		CCJSizeDockBar* pDock = (CCJSizeDockBar*)GetControlBar(dwSizeBarMap[i][0]);
		if (pDock != NULL && pDock != pBar)
		{
			pDock->CalcSizeBarLayout();	
		}
	}
}

void CCJFrameWnd::DockSizeBar(CControlBar * pBar,CCJSizeDockBar *pDockBar, LPRECT lpRect)
{
	ASSERT(pBar != NULL);
	ASSERT(pBar->m_pDockContext != NULL);
	
	if( pDockBar == NULL )
	{
		for (int i = 0; i < 4; i++)
		{
			if ((dwSizeBarMap[i][1] & CBRS_ALIGN_ANY) == (pBar->m_dwStyle & CBRS_ALIGN_ANY))
			{
				pDockBar = (CCJSizeDockBar*)GetControlBar(dwSizeBarMap[i][0]);
				ASSERT(pDockBar != NULL);

				int nSize = 0;

				switch(i)
				{
				case 0: // horizontal dock
				case 1:
					nSize = ((CCJControlBar*)pBar)->GetHorzSize();
					break;
				case 2: // vertical dock
				case 3:
					nSize = ((CCJControlBar*)pBar)->GetVertSize();
					break;
				}

				pDockBar->SetActualSize( nSize );
				break;
			}
		}
	}
	
	ASSERT(pDockBar != NULL);
	ASSERT(m_listControlBars.Find(pBar) != NULL);
	ASSERT(pBar->m_pDockSite == this);
	((CCJControlBar*)pBar)->Normalize();
	pDockBar->DockControlBar(pBar, lpRect);
}

HMENU CCJFrameWnd::NewMenu(UINT nMenuID, UINT nToolbarID)
{
	// Load the menu and toolbar resource.
	m_menu.LoadMenu(nMenuID);
	if (nToolbarID) {
		m_menu.LoadToolbar(nToolbarID);
	}

	// Detach the menu.
	return(m_menu.Detach());
}

HMENU CCJFrameWnd::NewMenu(UINT nMenuID, UINT* nToolbarIDs, int nSize)
{
	// Load the menu and toolbar resources.
	m_menu.LoadMenu(nMenuID);
	if (nToolbarIDs) {
		m_menu.LoadToolbars(nToolbarIDs, nSize);
	}

	// Detach the menu.
	return(m_menu.Detach());
}

BOOL CCJFrameWnd::InitializeMenu(UINT nMenuID, UINT nToolBarID/*=NULL*/)
{
	ASSERT(nMenuID);	 // must be valid.

	// Get a pointer to the existing menu, and destroy it.
	CMenu* pMenu = GetMenu();
	ASSERT_VALID(pMenu);
	pMenu->DestroyMenu();

	// Set the default menu with the newly created Menu.
	m_hMenuDefault = NewMenu(nMenuID, nToolBarID);
	SetMenu(CMenu::FromHandle(m_hMenuDefault));
	
	return TRUE;
}

BOOL CCJFrameWnd::InitializeMenu(UINT nMenuID, UINT* nToolbarIDs/*=NULL*/, int nSize/*=NULL*/)
{
	ASSERT(nMenuID);	 // must be valid.

	// Get a pointer to the existing menu, and destroy it.
	CMenu* pMenu = GetMenu();
	ASSERT_VALID(pMenu);
	pMenu->DestroyMenu();
	
	// Set the default menu with the newly created Menu.
	m_hMenuDefault = NewMenu(nMenuID, nToolbarIDs, nSize);
	SetMenu(CMenu::FromHandle(m_hMenuDefault));
	
	return TRUE;
}

void CCJFrameWnd::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	if (lpMeasureItemStruct->CtlType == ODT_MENU)
	{
		if (IsMenu((HMENU)lpMeasureItemStruct->itemID)) {
			CMenu* pMenu = CMenu::FromHandle((HMENU)lpMeasureItemStruct->itemID);
			
			if(m_menu.IsMenu(pMenu)) {
				m_menu.MeasureItem(lpMeasureItemStruct);
				return;
			}
		}
	}

	CFrameWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

LRESULT CCJFrameWnd::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) 
{
	if(m_menu.IsMenu(pMenu)) {
		return CCJMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
	}

	return CFrameWnd::OnMenuChar(nChar, nFlags, pMenu);
}

void CCJFrameWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	if(!bSysMenu)
	{
		if(m_menu.IsMenu(pPopupMenu)) {
			CCJMenu::UpdateMenu(pPopupMenu);
		}
	}
}

BOOL CCJFrameWnd::PreTranslateMessage(MSG* pMsg) 
{
	if( m_wndMenuBar.GetSafeHwnd()) {
		if( m_wndMenuBar.TranslateFrameMessage(pMsg)) {
			return TRUE;
		}
	}

	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CCJFrameWnd::LoadBarState(LPCTSTR lpszProfileName)
{
	CCJSizeDockBar::m_bIsDockBar = true;
	CFrameWnd::LoadBarState(lpszProfileName);
	CCJSizeDockBar::m_bIsDockBar = false;
}

void CCJFrameWnd::SaveBarState(LPCTSTR lpszProfileName) const
{
	CCJSizeDockBar::m_bIsDockBar = true;
	CFrameWnd::SaveBarState(lpszProfileName);
	CCJSizeDockBar::m_bIsDockBar = false;
}

void CCJFrameWnd::OnClose() 
{
	// Clear active view before close
	SetActiveView (NULL);
	CFrameWnd::OnClose();
}

// Default implementation.
BOOL CCJFrameWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CFrameWnd::PreCreateWindow(cs);
}

////////////////
// Overloaded for flicker-free drawing with no CS_VREDRAW and CS_HREDRAW.
// Override in CMainFrame and call this method, rather than the default. Pass
// in your apps icon resource usually IDR_MAINFRAME.
//
BOOL CCJFrameWnd::PreCreateWindow(CREATESTRUCT& cs, UINT nIconID) 
{
	// Register wnd class with no redraw, cursor (use default), background
	// brush and load your apps icon resource.
	cs.lpszClass = AfxRegisterWndClass( 0, NULL, NULL,
		AfxGetApp()->LoadIcon(nIconID));
	ASSERT(cs.lpszClass);
	
	return CFrameWnd::PreCreateWindow(cs);
}
