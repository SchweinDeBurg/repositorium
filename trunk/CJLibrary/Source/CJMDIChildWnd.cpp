// CJMDIChildWnd.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/31/99 11:02p $
 * $Revision: 6 $
 * $Archive: /CodeJock/CJLibrary/CJMDIChildWnd.cpp $
 *
 * $History: CJMDIChildWnd.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:02p
 * Updated in $/CodeJock/CJLibrary
 * Overrode OnEraseBkgnd(...) for flicker free drawing.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:56p
 * Updated in $/CodeJock/CJLibrary
 * Added variables to hold menu and toolbar resource ids for use with menu
 * bar initialization.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 10/06/99   Time: 6:21a
 * Updated in $/CodeJockey/CJLibrary
 * Added cool menu support for MDI applications that do not have a
 * document/view architecture. Added GetInitializedMenu() to
 * CCJMDIChildWnd which will initialize the menu for a given view,  and
 * overloaded InitializeMenu() in CCJMDIFrameWnd. Thanks to Golovanov
 * Michael [Golovanov@SoftLab.RU] for help with this.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 6/26/99    Time: 8:30p
 * Created in $/CodeJockey/CJLibrary
 * Initial creation. Added to library to allow CCJControlBar and
 * CCJToolBar docking.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJMDIChildWnd.h"
#include "CJDockBar.h"
#include "CJSizeDockBar.h"
#include "CJControlBar.h"
#include "CJMiniDockFrameWnd.h"
#include "CJToolBar.h"
#include "CJMDIFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJMDIChildWnd

IMPLEMENT_DYNCREATE(CCJMDIChildWnd, CMDIChildWnd)

CCJMDIChildWnd::CCJMDIChildWnd()
{
	m_hMenu		  = NULL;
	m_nToolBarIDs = NULL;
	m_nMenuID	  = 0;
	m_nToolBarID  = 0;
	m_nCount	  = 0;
}

CCJMDIChildWnd::~CCJMDIChildWnd()
{
}


BEGIN_MESSAGE_MAP(CCJMDIChildWnd, CMDIChildWnd)
	//{{AFX_MSG_MAP(CCJMDIChildWnd)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJMDIChildWnd message handlers

// dwDockBarMap
DWORD CCJMDIChildWnd::dwDockBarMap[4][2] =
{
	{ AFX_IDW_DOCKBAR_TOP,      CBRS_TOP    },
	{ AFX_IDW_DOCKBAR_BOTTOM,   CBRS_BOTTOM },
	{ AFX_IDW_DOCKBAR_LEFT,     CBRS_LEFT   },
	{ AFX_IDW_DOCKBAR_RIGHT,    CBRS_RIGHT  },
};

void CCJMDIChildWnd::DockControlBarLeftOf(CControlBar* Bar, CControlBar* LeftOf)
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

void CCJMDIChildWnd::EnableDocking(DWORD dwDockStyle)
{
	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);
	CMDIChildWnd::EnableDocking(dwDockStyle);
	
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

void CCJMDIChildWnd::RecalcLayout(BOOL bNotify) 
{
	CMDIChildWnd::RecalcLayout(bNotify);
	RecalcAllExcept(NULL);
}

// Yiannhs changed this
void CCJMDIChildWnd::EnableDockingSizeBar(DWORD dwDockStyle, DockFlatBorders FlatFlags )
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

void CCJMDIChildWnd::RecalcAllExcept(CCJSizeDockBar *pBar)
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

void CCJMDIChildWnd::DockSizeBar(CControlBar * pBar,CCJSizeDockBar *pDockBar, LPRECT lpRect)
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

HMENU CCJMDIChildWnd::GetInitializedMenu(UINT nMenuID, UINT nToolBarID)
{
	if( m_hMenu ) {
		return m_hMenu;
	}
	
	CCJMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST(CCJMDIFrameWnd, GetParentFrame());
	if( pMainFrame ) {
		pMainFrame->InitializeMenu( &m_hMenu, nMenuID, nToolBarID );
		m_nMenuID	  = nMenuID;
		m_nToolBarID  = nToolBarID;
	}

	return m_hMenu;
}

HMENU CCJMDIChildWnd::GetInitializedMenu(UINT nMenuID, UINT* nToolbarIDs, int nSize)
{
	if( m_hMenu ) {
		return m_hMenu;
	}
	
	CCJMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST(CCJMDIFrameWnd, GetParentFrame());
	if( pMainFrame ) {
		pMainFrame->InitializeMenu( &m_hMenu, nMenuID, nToolbarIDs, nSize);
		m_nMenuID	  = nMenuID;
		m_nToolBarIDs = nToolbarIDs;
		m_nCount      = nSize;
	}

	return m_hMenu;
}

BOOL CCJMDIChildWnd::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetClientRect(&rect);
	pDC->ExcludeClipRect(rect);
	
	return CMDIChildWnd::OnEraseBkgnd(pDC);
}
