// CJTabCtrlBar.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 29/08/04 22:54 $
 * $Revision: 2 $
 * $Archive: /Projects/Libraries/CJLibrary/CJLibrary/CJTabCtrlBar.cpp $
 *
 * $History: CJTabCtrlBar.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Anna         Date: 29/08/04   Time: 22:54
 * Updated in $/Projects/Libraries/CJLibrary/CJLibrary
 * 1.  Added CCJTabCtrlBar::AddView(lpszLabel, pView, nImage, nIndex)
 * 2.  Added CCJTabCtrlBar::SetViewName()
 * 3.  Added CCJTabCtrlBar::GetViewIndex()
 * 4.  Fixed a bug in CCJTabCtrlBar::GetTabFromPoint()
 * 
 * *****************  Version 22  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:29p
 * Updated in $/CodeJock/CJLibrary
 * Enabled OnInitialUpdate() to be called for tab views added to
 * CCJTabCtrlBar whenever the control bar is floating.
 * 
 * Fixed potential resource and memory leak problems.
 * 
 * Fixed problems with AddView(...) setting the wrong view index to
 * active.
 * 
 * Added  EnableToolTips method which enables and disables the tooltips.
 * Thanks to Phil Baxter for help with this.
 * 
 * Fixed problem with dangling pointer in DeleteView. Thanks to Gero Kuehn
 * [gero.kuehn@isc-soft.de] for help with this.
 * 
 * Fixed potential bug with CCJTabControlBar returning image index, fixed
 * bug with GetTabFromPoint(...) returning invalid index.
 * 
 * Made class methods virtual for inhertance purposes.
 * 
 * *****************  Version 20  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 19  *****************
 * User: Kirk Stowell Date: 8/30/99    Time: 12:46a
 * Updated in $/CodeJockey/CJLibrary
 * Gero Kuehn <gero.kuehn@isc-soft.de> - Added fixes for tab view
 * activation and removal.
 * 
 * *****************  Version 18  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:54p
 * Updated in $/CodeJockey/CJLibrary
 * Fixed debug assert when adding a view with no associated document.
 * 
 * *****************  Version 17  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:14p
 * Updated in $/CodeJockey/CJLibrary
 * Added Unicode compliance, thanks to Barry Burton for his help with
 * this.
 * 
 * 
 * *****************  Version 16  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 15  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 9:43p
 * Updated in $/CodeJockey/CJLibrary
 * Added _DYNAMIC macros for access to the CObject::IsKindOf to determine
 * the class object at runtime. Added method GetTabFromPoint which returns
 * the tab index based upon cursor position. Thanks to Sid Price
 * [sidprice@softtools.com] for suggesting this.
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 6/11/99    Time: 10:17p
 * Updated in $/CodeJockey/CJ60Lib
 * Minor modifications to fix level 4 compilier warnings.
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 5/17/99    Time: 9:07p
 * Updated in $/CodeJockey/CJ60Lib
 * Paul [RowingWolf@aol.com] - Added the following support methods:
 * void SetActiveView(CView* pUserView)
 * void AssociateImageIndexWithRuntimeClass(int nIndex, CRuntimeClass*
 * pRuntimeClass)
 * void AssociateImageIndexWithRuntimeClass(int nIndex, LPCTSTR
 * lpszRuntimeClassName)
 * short GetImageIndexFromRuntimeClass(CRuntimeClass* pRuntimeClass)
 * BOOL RemoveAllTabs().
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 5/12/99    Time: 5:32a
 * Updated in $/CodeJockey/CJ60Lib
 * Added support for vc5 compatibility
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 5/11/99    Time: 10:02p
 * Updated in $/CodeJockey/CJ60Lib
 * After much frustration, a class re-write. Fixed many of the issues with
 * painting, invalid pointers and memory leaks. Added document support for
 * views associated with the tab control.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 4/14/99    Time: 11:53p
 * Updated in $/CodeJockey/CJ60Lib
 * Added context popupMenu for CJTabCtrl to use Brent Corkum's BCMenu (CCJMenu).
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 11:52p
 * Updated in $/CodeJockey/CJ60Lib
 * Added comments and cleaned up code, modified AddView(...) to allow a
 * CDocument to be associated with the view added to the tab control.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 3/14/99    Time: 11:51p
 * Updated in $/CodeJockey/CJ60Lib
 * Gonzalo Pereyra [ persys@adinet.com.uy ] - I wanted to trap NM_RETURN
 * notifications in my CTreeView Class. I couldn't... It turns out that
 * CControlbar::PreTranslateMessage() will eat all dialog messages...It
 * will never get dispatched to my window. So I Overrided it in :
 * CCJTabCtrlBar::PreTranslateMessage(). this will allow for message
 * reflection and we will get NM_RETURN. I don't know if this a good
 * solution but for now , it works.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 2/24/99    Time: 11:49p
 * Updated in $/CodeJockey/CJ60Lib
 * Phil Baxter [pgb@mrjinx.demon.co.uk] - Fixed the following problems
 * when tabs are added after creation:
 * 1. The tab was created, but the view that was showing on the tab was
 * not visible.
 * 2. Get an assertion when the AddView function attempts to perform the
 * AddTool function.
 * 3. When a new view is added, although you set it to be the active view,
 * it isn't raised to the top of the stack in the display.
 * 4. The Tab name as stored in the TCB_ITEM structure is now a CString,
 * this removes the 32 byte limit that it previously had.
 * 5. New methods for obtaining the name as stored in the TCB_ITEM
 * structure.
 * 6. New Methods for deleting a view from the control, this removes the
 * view entirely including its 	associated tab.
 * 7. Some minor fixes around assertions when trying to manipulate the
 * control when it has not had any tabs created.
 * 8. Added two new member functions:
 * void SetViewToolTip(int nView, LPCTSTR lpszLabel);
 * void SetViewToolTip(CRuntimeClass *pViewClass, LPCTSTR lpszLabel);
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 1/31/99    Time: 11:48p
 * Updated in $/CodeJockey/CJ60Lib
 * Commented out 2 calls to SetActiveView(). This somewhat fixes the
 * problem with CTRL+TAB in activatation of MDI child windows, however if
 * you click inside the view, this breaks CTRL+TAB again, will look at
 * this when I get some free time. Thanks to Pierre Guimont
 * (Pierre_Guimont@mitel.com) for pointing this out.
 * 
 * Added m_TabCtrl.GetRowCount() as per Rashid Thadha
 * rashidt@consort.co.uk) request.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 1/17/99    Time: 11:46p
 * Updated in $/CodeJockey/CJ60Lib
 * Added helper class CCJTabCtrl to eliminate re-painting problems such as
 * when the app is minimized then restored.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 1/03/99    Time: 11:46p
 * Updated in $/CodeJockey/CJ60Lib
 * Application freezing bug fixed by
 * LiangYiBin.Donald(mailto:lybd@yahoo.com)
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/17/98   Time: 11:45p
 * Created in $/CodeJockey/CJ60Lib
 * Initial re-write and release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJTabCtrlBar.h"
#include "CJResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJTabCtrlBar

CCJTabCtrlBar::CCJTabCtrlBar()
{
	m_nActiveTab		= 0;
    m_nNextToolTipId	= 0;
	m_bToolTipsEnabled  = TRUE;
}

CCJTabCtrlBar::~CCJTabCtrlBar()
{
	while(!m_viewList.IsEmpty())
	{
		TCB_ITEM *pMember = m_viewList.RemoveHead();
		_delete(pMember);
	}
}

IMPLEMENT_DYNAMIC(CCJTabCtrlBar, CCJControlBar)

BEGIN_MESSAGE_MAP(CCJTabCtrlBar, CCJControlBar)
	//{{AFX_MSG_MAP(CCJTabCtrlBar)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONTROL, OnTabSelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJTabCtrlBar message handlers

void CCJTabCtrlBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	// Get a pointer to the parent frame.
	CFrameWnd* pFrame = GetParentFrame();
	ASSERT(pFrame != NULL);

	POSITION pos = m_viewList.GetHeadPosition();
	while( pos != NULL )
	{
		// Get the item for this position.
		TCB_ITEM *pItem = (TCB_ITEM*)m_viewList.GetNext(pos);
		ASSERT(pItem != NULL);

		// To avoid mainwindow freezing, we must
		// deativate the view, it's not visible now.
		if (pFrame->GetActiveView() == pItem->pWnd) {
			pFrame->SetActiveView(NULL);
		}

		// Resize the tab view.
		SizeTabView(pItem->pWnd);
	}
	
	CCJControlBar::OnWindowPosChanged(lpwndpos);
}

int CCJTabCtrlBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCJControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create the Tab Control
	if (!m_TabCtrl.Create(this, IDC_TAB_CONTROL))
	{
		TRACE0( "Unable to create tab control bar\n" );
		return -1;
	}

	// Associate the child window.
	SetChild(&m_TabCtrl);

	return 0;
}

BOOL CCJTabCtrlBar::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			m_pActiveView->SendMessage(pMsg->message,
				pMsg->wParam, pMsg->lParam); 
			return TRUE;
		}
	} 

	return CCJControlBar::PreTranslateMessage(pMsg);
}

void CCJTabCtrlBar::OnTabSelChange(NMHDR* /*pNMHDR*/, LRESULT* /*pResult*/)
{
	SetActiveView(m_TabCtrl.GetCurSel());
}

void CCJTabCtrlBar::GetChildRect(CRect &rect) 
{
	CCJControlBar::GetChildRect(rect);

	if (IsFloating()) {
		rect.DeflateRect(2,2);
	}
}

BOOL CCJTabCtrlBar::CreateTabView(CRuntimeClass *pViewClass, CDocument *pDocument, CCreateContext *pContext)
{
	CWnd* pWnd = NULL;

	// if no context specified, generate one 
	// from the currently selected client if possible...
	if (pContext == NULL)
	{
		CCreateContext context;
		
		context.m_pNewViewClass		= pViewClass;
		context.m_pCurrentDoc		= pDocument;
		context.m_pNewDocTemplate	= pDocument?pDocument->GetDocTemplate() : NULL;
		context.m_pLastView			= GetParentFrame()->GetActiveView();
		context.m_pCurrentFrame		= GetParentFrame();

		pContext = &context;
	}

	TRY
	{
		// Create the object.
		pWnd = DYNAMIC_DOWNCAST(CWnd, pViewClass->CreateObject());
		if (!pWnd) AfxThrowMemoryException();
	}
	CATCH_ALL(e)
	{
		TRACE0( "Out of memory creating a view.\n" );
		// Note: DELETE_EXCEPTION(e) not required
		return FALSE;
	}
	END_CATCH_ALL
		
	ASSERT_KINDOF(CWnd, pWnd);
	ASSERT(pWnd->m_hWnd == NULL); // not yet created.

	// Create with the right size and position
	if (!pWnd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0,0,0,0), this, 0, pContext))
	{
		TRACE0( "Warning: couldn't create client pane for view.\n" );
		// pWnd will be cleaned up by PostNcDestroy
		return FALSE;
	}

	// Set the newly create view as the active view.
	m_pActiveView = DYNAMIC_DOWNCAST(CView, pWnd);
	ASSERT_KINDOF(CView, m_pActiveView);

	// If we were floating last, call OnInitialUpdate for the view
	// since CMiniDockFrameWnd does not do this for us.
	if(	m_bFloating && m_pActiveView ) {
		m_pActiveView->OnInitialUpdate();
	}

	return TRUE;
}

TCB_ITEM* CCJTabCtrlBar::CreateTabListItem(LPCTSTR lpszLabel)
{
	// Allocate a new TCB_ITEM object.
	TCB_ITEM* pMember = new TCB_ITEM;
	
	pMember->pWnd			= m_pActiveView;
	pMember->szTabLabel		= lpszLabel;
	pMember->szToolTipLabel = lpszLabel;
    pMember->uiToolTipId	= m_nNextToolTipId++;

	// Add the new TCB_ITEM to the tab item list.
	m_viewList.AddTail(pMember);

	// Create the ToolTip for the tab.
	AddToolTip(pMember, lpszLabel);

	return pMember;
}

CToolTipCtrl* CCJTabCtrlBar::GetToolTip()
{
	// Get a pointer to the tab control's CToolTipCtrl.
#ifdef _VC_VERSION_5
	CToolTipCtrl* pToolTip = m_TabCtrl.GetTooltips();
	ASSERT(pToolTip != NULL);
#else
	CToolTipCtrl* pToolTip = m_TabCtrl.GetToolTips();
	ASSERT(pToolTip != NULL);
#endif // _VC_VERSION_5

	return pToolTip;
}

void CCJTabCtrlBar::AddToolTip(TCB_ITEM *pMember, LPCTSTR lpszLabel)
{
	// Get the tool tips for the tab control.
	CToolTipCtrl* pToolTip = GetToolTip();

	// Add the tool tip.
	if( pMember->uiToolTipId == 0 ) {
		pToolTip->AddTool( &m_TabCtrl, lpszLabel,
			NULL, pMember->uiToolTipId );
	}
	else {
		pToolTip->AddTool( &m_TabCtrl, lpszLabel,
			CRect(0,0,0,0), pMember->uiToolTipId );
	}
}

void CCJTabCtrlBar::ResetToolTips(CToolTipCtrl* pToolTip)
{
	if (pToolTip)
	{
		int nIndex = 0;
		POSITION pos;
		for( pos = m_viewList.GetHeadPosition(); pos; m_viewList.GetNext(pos) )
		{
			// Get the next item in the list.
			TCB_ITEM* pMember = m_viewList.GetAt( pos );
			
			// Update the tooltip for the view
			pMember->uiToolTipId = nIndex;
			pToolTip->UpdateTipText( pMember->szToolTipLabel, &m_TabCtrl, 
				pMember->uiToolTipId );
			
			nIndex++;
		}
		
		m_nNextToolTipId = ( nIndex == 0 )?1:nIndex;
	}
}

BOOL CCJTabCtrlBar::AddView(LPCTSTR lpszLabel, CRuntimeClass *pViewClass,
	CDocument* pDoc/*=NULL*/, UINT nIndex/*=-1*/, CCreateContext *pContext/*=NULL*/)
{	
	OverkillAssert(pViewClass);
	
	// Create the view to be associate with tab.
	if (!CreateTabView(pViewClass, pDoc, pContext))
		return FALSE;

	// Create the tab list item.
	if (CreateTabListItem(lpszLabel) == NULL)
		return FALSE;

	// Since the tool tip count has changed,
	// re-initialize the tooltips.
	ResetToolTips(GetToolTip());

	int nViews = ( nIndex == -1 )?m_viewList.GetCount():nIndex;
	if( nViews != 1 ) {
		m_pActiveView->EnableWindow(FALSE);
		m_pActiveView->ShowWindow(SW_HIDE);
	}

	// Insert the new tab item.
	TC_ITEM tci;
	tci.mask	= TCIF_TEXT | TCIF_IMAGE;
	tci.pszText = (LPTSTR)(LPCTSTR)lpszLabel;
	tci.iImage	= nViews-1;
	m_TabCtrl.InsertItem(m_viewList.GetCount()-1, &tci);

	// Resize the tab view.
	SizeTabView(m_pActiveView);

	return TRUE;
}


BOOL CCJTabCtrlBar::AddView(LPCTSTR lpszLabel,
							CView* pView,
							int nImage /*= -1*/,
							UINT nIndex /*=-1*/)
{	
	m_pActiveView = pView;

	pView->SetParent(this);

	// Create the tab list item.
	if (CreateTabListItem(lpszLabel) == NULL)
		return FALSE;

	// Since the tool tip count has changed,
	// re-initialize the tooltips.
	ResetToolTips(GetToolTip());

	int nViews = ( nIndex == -1 ) ? m_viewList.GetCount() : nIndex;

	if (nViews != 1)
	{
		m_pActiveView->EnableWindow(FALSE);
		m_pActiveView->ShowWindow(SW_HIDE);
	}

	// Insert the new tab item.
	TC_ITEM tci;
	tci.mask	= TCIF_TEXT | TCIF_IMAGE;
	tci.pszText = (LPTSTR)(LPCTSTR)lpszLabel;

	if (nImage >= 0)
	{
		tci.iImage	= nImage;
	}
	else
	{
		tci.iImage	= nViews-1;
	}
	m_TabCtrl.InsertItem(m_viewList.GetCount()-1, &tci);

	// Resize the tab view.
	SizeTabView(pView);

	return TRUE;
}


void CCJTabCtrlBar::SizeTabView(CWnd *pWnd)
{
	// Get the new window size.
	CRect rcChild;
	GetChildRect(rcChild);
	rcChild.DeflateRect(3,3);
	rcChild.bottom -= (21 * m_TabCtrl.GetRowCount());

	// Resize the window.
	pWnd->MoveWindow(rcChild);
	pWnd->SetWindowPos(NULL, 0,0,0,0,
		SWP_NOSIZE|SWP_NOMOVE);
}

void CCJTabCtrlBar::OverkillAssert(CRuntimeClass *pViewClass)
{
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
	ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass), FALSE));
#else
	UNUSED_ALWAYS( pViewClass );
#endif
}

void CCJTabCtrlBar::RemoveListItem(POSITION pos)
{
    if( pos != NULL )
	{
	    // remove the page from internal list
        TCB_ITEM *pMember = m_viewList.GetAt(pos);
	    ASSERT(pMember);

        // And delete the member window, freeing our stored
        // values relating to it
        if (pMember->pWnd != NULL) {
            pMember->pWnd->DestroyWindow();
            pMember->pWnd = NULL;
        }

		// Remove from list and free memory.
	    m_viewList.RemoveAt(pos);
		_delete(pMember);
    }
}

void CCJTabCtrlBar::DeleteView(int nView)
{
	ASSERT_VALID( this );
	ASSERT(( nView >= 0 ) && (m_TabCtrl.GetItemCount() > nView));

	TRACE2("***DeleteView: %d ( /%d )\n", nView,m_viewList.GetCount());

    // Now find the view we want to delete and remove it
    POSITION pos = m_viewList.FindIndex(nView);
    if( pos != NULL )
    {
		CView *pOldActiveView;
		pOldActiveView = m_pActiveView;

		// Ensure that we get no dangling pointers
		// DO NOT (RE-)MOVE THIS CODE ! Gero Kuehn [gero.kuehn@isc-soft.de]
		if(GetParentFrame()->GetActiveView() == DYNAMIC_DOWNCAST( CView, m_viewList.GetAt(pos)->pWnd))
			GetParentFrame()->SetActiveView( NULL , FALSE);

		// Remove item from list, and free memory.
		RemoveListItem( pos );

        // Remove it from the tab control
  	    m_TabCtrl.DeleteItem(nView);

        // Finally, if we have just deleted the active view, reset the
        // active tab to be the first view in the list
        if (nView == m_nActiveTab)
        {
            m_nActiveTab = -1;
            m_pActiveView = NULL; // Gero Kuehn [gero.kuehn@isc-soft.de]
        }
        else
        {
			// ** GK/ISC
			if((m_nActiveTab - 1)>=0) {
				SetActiveView(m_nActiveTab - 1);
			}
        }

		// Reset the tooltips for the views we have left...
		ResetToolTips(GetToolTip());
    }
}

void CCJTabCtrlBar::DeleteView(CRuntimeClass *pViewClass)
{
	OverkillAssert(pViewClass);

    int nIndex = 0;
	for( POSITION pos = m_viewList.GetHeadPosition(); pos; m_viewList.GetNext( pos ))
	{
		TCB_ITEM *pMember = m_viewList.GetAt(pos);
		ASSERT(pMember);

		if (pMember->pWnd->IsKindOf(pViewClass)) {
            DeleteView(nIndex);
            return;
		}
        nIndex++;
    }
}

void CCJTabCtrlBar::RemoveView(int nView)
{
	ASSERT_VALID( this );
	ASSERT(( nView >= 0 ) && (m_TabCtrl.GetItemCount() > nView));
    RemoveListItem( m_viewList.FindIndex( nView ));
}

void CCJTabCtrlBar::SetViewToolTip(int nView, LPCTSTR lpszLabel)
{
	ASSERT_VALID(this);
	ASSERT(( nView >= 0 ) && (m_TabCtrl.GetItemCount() > nView));

	// Find the view we want to change the tooltip for and
	// reset its tooltip text
	POSITION pos = m_viewList.FindIndex( nView );
	if( pos != NULL )
	{
		// Get the private data for the view
		TCB_ITEM *pMember = m_viewList.GetAt(pos);
		
		// Reset the tooltip text to the new value
		pMember->szToolTipLabel = lpszLabel;
		
		// Update the tooltip for the view
		GetToolTip()->UpdateTipText( pMember->szToolTipLabel,
			&m_TabCtrl, pMember->uiToolTipId );
	}
}

void CCJTabCtrlBar::SetViewToolTip(CRuntimeClass *pViewClass, LPCTSTR lpszLabel)
{
	OverkillAssert(pViewClass);
	
    int nIndex=0;
	for (POSITION pos=m_viewList.GetHeadPosition(); pos; m_viewList.GetNext(pos))
	{
		TCB_ITEM *pMember=m_viewList.GetAt(pos);
		if (pMember->pWnd->IsKindOf(pViewClass))
		{
            SetViewToolTip(nIndex, lpszLabel);
            break;
		}
        nIndex++;
    }
	return;
}

CView* CCJTabCtrlBar::GetView(int nView)
{
	ASSERT_VALID(this);
	ASSERT(( nView >= 0 ) && ( m_TabCtrl.GetItemCount() > nView ));
	
	if( nView != -1 )
	{
        POSITION pos = m_viewList.FindIndex( nView );
        if( pos != NULL )
		{
            TCB_ITEM* pMember = m_viewList.GetAt( pos );
            return ( pMember )?(CView*)pMember->pWnd:NULL;
        }

		return NULL;
	}

	return NULL;
}

CView* CCJTabCtrlBar::GetView(CRuntimeClass *pViewClass)
{
	OverkillAssert(pViewClass);
	
	for( POSITION pos = m_viewList.GetHeadPosition(); pos; m_viewList.GetNext( pos ))
	{
		TCB_ITEM *pMember = m_viewList.GetAt( pos );
		if( pMember->pWnd->IsKindOf( pViewClass )) {
			return (CView*)pMember->pWnd;
		}
    }
	return NULL;
}


int CCJTabCtrlBar::GetViewIndex(CView* pView) const
{
	ASSERT_VALID(this);

	ASSERT(NULL != pView);
	if (NULL != pView)
	{
		for (int nIndex = 0; nIndex < m_viewList.GetCount(); nIndex++)
		{
            POSITION pos = m_viewList.FindIndex(nIndex);
			if (NULL != pos)
			{
				TCB_ITEM* pMember = m_viewList.GetAt(pos);

				if (static_cast<CView*>(pMember->pWnd) == pView)
				{
					return nIndex;
				}
			}
        }
	}

	return -1;
}


LPCTSTR CCJTabCtrlBar::GetViewName(int nView)
{
	ASSERT_VALID(this);
	ASSERT(( nView >= 0 ) && ( m_TabCtrl.GetItemCount() > nView ));
	
	if( nView != -1 )
	{
        POSITION pos = m_viewList.FindIndex( nView );
        if( pos != NULL )
		{
            TCB_ITEM* pMember = m_viewList.GetAt( pos );
			return ( pMember )?(LPCTSTR)pMember->szTabLabel:NULL;
        }
		
        return NULL;
	}
	
	return NULL;
}


LPCTSTR CCJTabCtrlBar::GetViewName(CRuntimeClass *pViewClass)
{
	OverkillAssert(pViewClass);
	
	for( POSITION pos = m_viewList.GetHeadPosition(); pos; m_viewList.GetNext( pos ))
	{
		TCB_ITEM *pMember = m_viewList.GetAt( pos );
		if( pMember->pWnd->IsKindOf( pViewClass )) {
    		return (LPCTSTR)pMember->szTabLabel;
		}
    }
	return NULL;
}


BOOL CCJTabCtrlBar::SetViewName(int nView, LPCTSTR pszViewName)
{
	ASSERT_VALID(this);
	ASSERT(( nView >= 0 ) && ( m_TabCtrl.GetItemCount() > nView ));
	
	if (-1 != nView)
	{
        POSITION pos = m_viewList.FindIndex(nView);
        if (NULL != pos)
		{
            TCB_ITEM* pMember = m_viewList.GetAt( pos );
			if (NULL != pMember)
			{
				pMember->szTabLabel = pszViewName;

				TC_ITEM tci;
				tci.mask	= TCIF_TEXT;
				tci.pszText = (LPTSTR)(LPCTSTR)pMember->szTabLabel;

				return m_TabCtrl.SetItem(nView, &tci);
			}
        }
	}
	return false;
}


void CCJTabCtrlBar::SetActiveView(int nView)
{
	ASSERT_VALID(this);
	ASSERT(( nView >= 0 ) && ( m_TabCtrl.GetItemCount() > nView ));
	
	if(( nView != -1 ) && ( nView != m_nActiveTab ))
	{
        POSITION pos = m_viewList.FindIndex( nView );
        if( pos != NULL )
		{
			// Hide the old view we are changing.
			// GK/ISC
			POSITION oldPos = NULL;
			if( m_nActiveTab >= 0) {
				oldPos = m_viewList.FindIndex( m_nActiveTab );
			}

			if( oldPos )
			{
				TCB_ITEM* pOldItem = m_viewList.GetAt( oldPos );
				pOldItem->pWnd->EnableWindow(FALSE);
				pOldItem->pWnd->ShowWindow(SW_HIDE);
			}

			// Show the newly selected view.
            TCB_ITEM* pMember = m_viewList.GetAt( pos );
            pMember->pWnd->EnableWindow(TRUE);
            pMember->pWnd->ShowWindow(SW_SHOW);
            pMember->pWnd->SetFocus();
		    
		    // select the tab (if tab programmatically changed)
            m_nActiveTab = nView;
		    m_TabCtrl.SetCurSel( m_nActiveTab );

            // Set the active view.
			m_pActiveView = DYNAMIC_DOWNCAST( CView, pMember->pWnd );
			ASSERT_KINDOF( CView, m_pActiveView );
		    GetParentFrame()->SetActiveView( m_pActiveView );
        }
    }
}

void CCJTabCtrlBar::SetActiveView(CRuntimeClass *pViewClass)
{
	OverkillAssert(pViewClass);
	
	int nView = 0;
	for (POSITION pos=m_viewList.GetHeadPosition(); pos; m_viewList.GetNext(pos))
	{
		TCB_ITEM *pMember=m_viewList.GetAt(pos);
		if (pMember->pWnd->IsKindOf(pViewClass))
		{
			//first hide old first view
            m_pActiveView->EnableWindow(FALSE);
            m_pActiveView->ShowWindow(SW_HIDE);
			
			// set new active view
			m_pActiveView = (CView*)pMember->pWnd;
			
			// enable, show, set focus to new view
			m_pActiveView->EnableWindow(TRUE);
			m_pActiveView->ShowWindow(SW_SHOW);
			m_pActiveView->SetFocus();
			
			m_nActiveTab = nView;
			
			// select the tab
			m_TabCtrl.SetCurSel(m_nActiveTab);
			
			break;
		}
		nView++;
    }
}

void CCJTabCtrlBar::SetActiveView(CView* pUserView)
{
	// Walk through the tabs
	for (POSITION pos=m_viewList.GetHeadPosition(); pos; m_viewList.GetNext(pos))
	{
		// Get the tab's view
		CView* pTabView = (CView *)(m_viewList.GetAt(pos)->pWnd);
        
		// Does the tab view match the requested view?
		if (pTabView == pUserView)
		{
			// Change the active view
			m_pActiveView = pTabView;
			((CFrameWnd *)GetParent())->SetActiveView((CView *)m_pActiveView);
		}
	}
}

void CCJTabCtrlBar::AssociateImageIndexWithRuntimeClass(int nIndex, CRuntimeClass* pRuntimeClass)
{
	CString szClassName(pRuntimeClass->m_lpszClassName);
	m_ClassNameToImageIndexMap[szClassName] = (short*)nIndex;
}

void CCJTabCtrlBar::AssociateImageIndexWithRuntimeClass(int nIndex, LPCTSTR lpszRuntimeClassName)
{
	CString szClassName(lpszRuntimeClassName);
	m_ClassNameToImageIndexMap[szClassName] = (short*)nIndex;
}

short CCJTabCtrlBar::GetImageIndexFromRuntimeClass(CRuntimeClass* pRuntimeClass)
{
	ASSERT(m_ClassNameToImageIndexMap.GetCount() > 0);
	CString szClassName(pRuntimeClass->m_lpszClassName);

	// Look for class name
	short* psValue;
	if (FALSE == m_ClassNameToImageIndexMap.Lookup(szClassName, (void*&)psValue))
		return -1;
	
	// return the de-referenced pointer.
	return *psValue;

}

BOOL CCJTabCtrlBar::RemoveAllTabs()
{
	// Itterate the tab views
	while(!m_viewList.IsEmpty())
	{
		RemoveView(0);	// Remove the view from the bar
		m_TabCtrl.DeleteItem(0);
	}
	m_pActiveView = NULL;
	((CFrameWnd *)GetParent())->SetActiveView(NULL);
	Invalidate();
	return TRUE;
}


int CCJTabCtrlBar::GetTabFromPoint(CPoint point)
{
	TCHITTESTINFO tch;
	memset(&tch, 0, sizeof(TCHITTESTINFO));

	CPoint pt = point;
	if (point == CPoint(0,0) )
	{
		GetCursorPos(&pt);
	}

	m_TabCtrl.ScreenToClient(&pt);		// CTabCtrl expects client coordinates [Anna 4.11.2003]

	tch.pt = pt;
	
	int iTab = m_TabCtrl.HitTest(&tch);

	if( iTab != -1 ) {
		m_nPointTab = iTab ;
	}

	return iTab;
}


BOOL CCJTabCtrlBar::EnableToolTips(BOOL bEnable)
{
    BOOL bPrev = m_bToolTipsEnabled;

    m_bToolTipsEnabled = bEnable;

	// Get the tool tips for the tab control.
	CToolTipCtrl* pToolTip = GetToolTip();

	if (pToolTip)
        pToolTip->Activate(m_bToolTipsEnabled);
    else
        m_bToolTipsEnabled = bPrev;

	return bPrev;
}

