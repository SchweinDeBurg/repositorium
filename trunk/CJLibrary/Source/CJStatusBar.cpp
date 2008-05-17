// CJStatusBar.cpp : implementation file
//
// Copyright © 1999 Pierre MELLINAND
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 24/05/02 13:52 $
 * $Revision: 2 $
 * $Archive: /Projects/AddIns/ResOrg/CJLibrary/CJLibrary/CJStatusBar.cpp $
 *
 * $History: CJStatusBar.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 24/05/02   Time: 13:52
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/CJLibrary
 * Modifications for compatibility with Visual Studio.NET
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 1:58p
 * Updated in $/CodeJock/CJLibrary
 * Minor fix for UNICODE builds.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 11:08p
 * Created in $/CodeJock/CJLibrary
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CJStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJStatusBarPaneControlInfo

CCJStatusBarPaneControlInfo::CCJStatusBarPaneControlInfo()
{
	m_pWnd		  = NULL;
	m_bAutodelete = TRUE;
	m_nPaneID	  = 0;
}

CCJStatusBarPaneControlInfo::~CCJStatusBarPaneControlInfo()
{
	// TODO: add destruction code here.
}

IMPLEMENT_DYNCREATE(CCJStatusBarPaneControlInfo, CObject)

/////////////////////////////////////////////////////////////////////////////
// CCJStatusBarPane

CCJStatusBarPane::CCJStatusBarPane()
{
	// TODO: add construction code here.
}

CCJStatusBarPane::CCJStatusBarPane(CCJStatusBarPane& rPane)
{
	m_nID	  = rPane.m_nID;
	m_cxText  = rPane.m_cxText;
	m_nStyle  = rPane.m_nStyle;
	m_nFlags  = rPane.m_nFlags;
	m_strText = rPane.m_strText;
}

CCJStatusBarPane::~CCJStatusBarPane()
{
	// TODO: add destruction code here.
}

CCJStatusBarPane& CCJStatusBarPane::operator=(CCJStatusBarPane& rPane)
{
	m_nID	  = rPane.m_nID;
	m_cxText  = rPane.m_cxText;
	m_nStyle  = rPane.m_nStyle;
	m_nFlags  = rPane.m_nFlags;
	m_strText = rPane.m_strText;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// CCJStatusBar

CCJStatusBar::CCJStatusBar()
{
	// TODO: add construction code here.
}

CCJStatusBar::~CCJStatusBar()
{
	for( int i = 0; i < m_aPans.GetSize(); i++ )
	{
		if( m_aPans[i]->m_pWnd && ::IsWindow(m_aPans[i]->m_pWnd->m_hWnd) )
			m_aPans[i]->m_pWnd->CloseWindow();
		if( m_aPans[i]->m_bAutodelete )
		{
			delete m_aPans[i]->m_pWnd;
		}
		delete m_aPans[i];
	}
}

IMPLEMENT_DYNCREATE(CCJStatusBar, CStatusBar)

BEGIN_MESSAGE_MAP(CCJStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CCJStatusBar)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJStatusBar message handlers

void CCJStatusBar::OnSize(UINT nType, int cx, int cy) 
{
	CStatusBar::OnSize(nType, cx, cy);
	PositionControls();
}

void CCJStatusBar::PositionControls()
{
	int h,v,s;
	GetStatusBarCtrl( ).GetBorders( h, v, s ) ;
	
	
	for(int i = 0; i < m_aPans.GetSize(); i++ )
	{
		CRect rect;
		
		int index = CommandToIndex( m_aPans[i]->m_nPaneID );
		GetItemRect(index,rect);
		if ( (GetPaneStyle(index) & SBPS_NOBORDERS) == SBPS_NOBORDERS)
		{
			m_aPans[i]->m_pWnd->MoveWindow(rect);
		}
		else
		{
			rect.top+=v;
			rect.bottom-=v;
			rect.left+=h+s;
			rect.right-=(h+s);
			m_aPans[i]->m_pWnd->MoveWindow(rect);
		}
		
	}
}

CCJStatusBarPaneControlInfo * CCJStatusBar::GetPanControl(int nPaneID)
{
	for(int i = 0; i < m_aPans.GetSize(); i++ )
	{
		if( m_aPans[i]->m_nPaneID == nPaneID )
			return m_aPans[i];
	}
	return NULL;
}

BOOL CCJStatusBar::AddControl(CWnd * pWnd, int paneID, BOOL bAutodeleteControl)
{
	
	CCJStatusBarPaneControlInfo * pPanInfo = GetPanControl(paneID);
	if( pPanInfo )
		return FALSE;
	
	int idx = CommandToIndex( paneID ) ;
	if( idx == -1 )
		return FALSE;
	
	CCJStatusBarPaneControlInfo * pPan = new CCJStatusBarPaneControlInfo;
	pPan->m_nPaneID =  paneID;
	pPan->m_pWnd    =  pWnd;
	pPan->m_bAutodelete = bAutodeleteControl;
	
	m_aPans.Add(pPan);
	PositionControls();
	Invalidate(TRUE);
	return TRUE;
}

void CCJStatusBar::SetPaneWidth(int index, int cxWidth)
{
	UINT nID,nStyle;
	int width;
	GetPaneInfo(index,nID,nStyle,width);
	SetPaneInfo(index,nID,nStyle,cxWidth);
}

BOOL CCJStatusBar::GetStatusPane(int nIndex, CCJStatusBarPane & rPane)
{
	if( nIndex < m_nCount  && nIndex >= 0 )
	{
		GetPaneInfo( nIndex,  rPane.m_nID, rPane.m_nStyle, rPane.m_cxText ) ;
		GetPaneText( nIndex , rPane.m_strText );
		return TRUE;
	}
	return FALSE;
}

BOOL CCJStatusBar::AddIndicator( int position, UINT paneID )
{
	CArray<CCJStatusBarPane,CCJStatusBarPane> arPanes;
	
	CCJStatusBarPane statusPane;
	int i;
	
	i = 0;
	while( GetStatusPane(i,statusPane) )
	{
		arPanes.Add( statusPane );
		i++;
	}
	if( position < 0 )
		position = 0;
	if( position > arPanes.GetSize() )
		position = arPanes.GetSize()-1;
	
	for(i = 0; i < arPanes.GetSize(); i ++ )
	{
		if( paneID == arPanes[i].m_nID )
		{
			TRACE0( "CCJStatusBar::AddIndicator(): Pane ID already exists \n" );
			return FALSE;
		}
	}
	
	CCJStatusBarPane new_statusPane;
	new_statusPane.m_nID = paneID;
	if( arPanes.GetSize() )
		arPanes.InsertAt(position,new_statusPane);
	else
		arPanes.Add(new_statusPane);
	
	UINT * pIndicators = new UINT[arPanes.GetSize()];
	for(i=0;i<arPanes.GetSize();i++)
		pIndicators[i]=arPanes[i].m_nID;
	
	SetRedraw(FALSE);
	SetIndicators(pIndicators,arPanes.GetSize());
	
	// Replace infos of indicators ..
	for(i = 0; i < arPanes.GetSize(); i++ )
	{
		if( arPanes[i].m_nID != paneID )
		{
			SetPaneInfo(i,arPanes[i].m_nID,arPanes[i].m_nStyle,arPanes[i].m_cxText);
			SetPaneText(i,arPanes[i].m_strText);
		}
		else
		{
			SetPaneWidth(i,50);
			SetPaneText(i,arPanes[i].m_strText);
		}
		
	}
	
	delete [] pIndicators;
	SetRedraw(TRUE);
	PositionControls();
	Invalidate(TRUE);	
	return TRUE;
}

void CCJStatusBar::RemovePane(int nPaneID)
{
	SetRedraw(FALSE);
	CWnd * pwnd = NULL;
	for( int i = 0; i < m_aPans.GetSize(); i++ )
	{
		if( m_aPans[i]->m_nPaneID == nPaneID )
		{
			if( m_aPans[i]->m_bAutodelete )
				pwnd = m_aPans[i]->m_pWnd;
			delete m_aPans[i];
			m_aPans.RemoveAt(i);
			break;
		}
	}
	if( pwnd )
	{
		pwnd->DestroyWindow();
		delete pwnd;
	}
	
	CArray<CCJStatusBarPane,CCJStatusBarPane> arPanes;
	CCJStatusBarPane statusPane;
	int j = 0;
	while( GetStatusPane(j,statusPane) )
	{
		if( statusPane.m_nID != (UINT)nPaneID )
			arPanes.Add( statusPane );
		j++;
	}
	// Now remove the indicator
	UINT * pIndicators = new UINT[arPanes.GetSize()];
	for(int i=0; i<arPanes.GetSize(); i++)
		pIndicators[i]=arPanes[i].m_nID;
	SetIndicators(pIndicators,arPanes.GetSize());
	
	// Replace infos of indicators ..
	for(int i = 0; i < arPanes.GetSize(); i++)
	{
		SetPaneInfo(i,arPanes[i].m_nID,arPanes[i].m_nStyle,arPanes[i].m_cxText);
		SetPaneText(i,arPanes[i].m_strText);
	}
	
	delete [] pIndicators;
	SetRedraw(TRUE);
	PositionControls();
	Invalidate(TRUE);
}
