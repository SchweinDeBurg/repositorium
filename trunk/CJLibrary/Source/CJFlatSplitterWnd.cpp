// CJFlatSplitterWnd.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Based on the CSplitterWndEx class written by Oleg G. Galkin.
// mailto:OlegGa@vest.msk.ru
// http://www.codeguru.com/splitter/show_hide_static_panes.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 24/05/02 13:50 $
 * $Revision: 2 $
 * $Archive: /Projects/AddIns/ResOrg/CJLibrary/CJLibrary/CJFlatSplitterWnd.cpp $
 *
 * $History: CJFlatSplitterWnd.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 24/05/02   Time: 13:50
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/CJLibrary
 * Modifications for compatibility with Visual Studio.NET
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 9:25p
 * Updated in $/CodeJock/CJLibrary
 * Fixed bug with hiding or showing panes where the incorrect splitter
 * control id was used.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 9/13/99    Time: 5:44a
 * Updated in $/CodeJockey/CJLibrary
 * Fixed assertion when using ReplaceView to display a CFormView derived
 * class in a debug build - Tom
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 9/13/99    Time: 5:40a
 * Updated in $/CodeJockey/CJLibrary
 * Fixed bug with GetPane() call in ShowRow() where row and col arguments
 * were switched. - Francis
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
 * User: Kirk Stowell Date: 7/14/99    Time: 10:00p
 * Created in $/CodeJockey/CJLibrary
 * Class added to support flat splitters, removable splitter views and
 * view manipulation.
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CJFlatSplitterWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if (_MFC_VER >= 0x0900)
#define CX_BORDER AFX_CX_BORDER
#define CY_BORDER AFX_CY_BORDER
#endif   // _MFC_VER

/////////////////////////////////////////////////////////////////////////////
// CCJFlatSplitterWnd

CCJFlatSplitterWnd::CCJFlatSplitterWnd()
{
	m_bFlatSplitter = TRUE;
	m_nHiddenCol	= -1;
	m_nHiddenRow	= -1;
}

CCJFlatSplitterWnd::~CCJFlatSplitterWnd()
{

}

BEGIN_MESSAGE_MAP(CCJFlatSplitterWnd, CSplitterWnd)
	//{{AFX_MSG_MAP(CCJFlatSplitterWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJFlatSplitterWnd message handlers

void CCJFlatSplitterWnd::OnDrawSplitter(CDC* pDC, ESplitType nType,
	const CRect& rectArg)
{
	if( m_bFlatSplitter )
	{
		// if pDC == NULL, then just invalidate
		if (pDC == NULL)
		{
			RedrawWindow(rectArg, NULL, RDW_INVALIDATE|RDW_NOCHILDREN);
			return;
		}
		ASSERT_VALID(pDC);

		// otherwise, actually draw
		CRect rect = rectArg;
		switch (nType)
		{
		case splitBorder:
			#if _MSC_VER < 1300			// VC 5.0/6.0 only
				ASSERT(afxData.bWin4);
			#endif

			pDC->Draw3dRect(rect, afxData.clrBtnFace, afxData.clrBtnFace);

			rect.InflateRect(-CX_BORDER, -CY_BORDER);
			pDC->Draw3dRect(rect, afxData.clrBtnShadow, afxData.clrBtnHilite);
			return;

		case splitIntersection:
			#if _MSC_VER < 1300		// VC 5.0/6.0 only
				ASSERT(!afxData.bWin4);
			#endif

			break;

		case splitBox:
			#if _MSC_VER >= 1300		// VC 7.0 (.NET)
				pDC->Draw3dRect(rect, afxData.clrBtnFace, afxData.clrWindowFrame);
				rect.InflateRect(-CX_BORDER, -CY_BORDER);
				pDC->Draw3dRect(rect, afxData.clrBtnHilite, afxData.clrBtnShadow);
				rect.InflateRect(-CX_BORDER, -CY_BORDER);
				break;
			#else						// VC 5.0/6.0
				if (afxData.bWin4)
				{
					pDC->Draw3dRect(rect, afxData.clrBtnFace, afxData.clrWindowFrame);
					rect.InflateRect(-CX_BORDER, -CY_BORDER);
					pDC->Draw3dRect(rect, afxData.clrBtnHilite, afxData.clrBtnShadow);
					rect.InflateRect(-CX_BORDER, -CY_BORDER);
					break;
				}
			#endif
			// fall through...
		case splitBar:
			#if _MSC_VER >= 1300		// VC 7.0 (.NET)
				pDC->Draw3dRect(rect, afxData.clrBtnHilite, afxData.clrBtnShadow);
				rect.InflateRect(-CX_BORDER, -CY_BORDER);
			#else						// VC 5.0/6.0
				if (!afxData.bWin4)
				{
					pDC->Draw3dRect(rect, afxData.clrBtnHilite, afxData.clrBtnShadow);
					rect.InflateRect(-CX_BORDER, -CY_BORDER);
				}
			#endif
			break;

		default:
			ASSERT(FALSE);  // unknown splitter type
		}

		// fill the middle
		pDC->FillSolidRect(rect, afxData.clrBtnFace);
	}

	else {
		CSplitterWnd::OnDrawSplitter(pDC, nType, rectArg);
	}
}

void CCJFlatSplitterWnd::OnPaint() 
{
	if( m_bFlatSplitter )
	{
		ASSERT_VALID(this);
		CPaintDC dc(this);
		
		CRect rectClient;
		GetClientRect(&rectClient);
		
		CRect rectInside;
		GetInsideRect(rectInside);
		rectInside.InflateRect(4,4);
		
		// draw the splitter boxes
		if (m_bHasVScroll && m_nRows < m_nMaxRows)
		{
			OnDrawSplitter(&dc, splitBox,
				CRect(rectInside.right, rectClient.top,
				rectClient.right, rectClient.top + m_cySplitter));
		}
		
		if (m_bHasHScroll && m_nCols < m_nMaxCols)
		{
			OnDrawSplitter(&dc, splitBox,
				CRect(rectClient.left, rectInside.bottom,
				rectClient.left + m_cxSplitter, rectClient.bottom));
		}
		
		// extend split bars to window border (past margins)
		DrawAllSplitBars(&dc, rectInside.right, rectInside.bottom);
		
		// draw splitter intersections (inside only)
		GetInsideRect(rectInside);
		dc.IntersectClipRect(rectInside);
		CRect rect;
		rect.top = rectInside.top;
		for (int row = 0; row < m_nRows - 1; row++)
		{
			rect.top += m_pRowInfo[row].nCurSize + m_cyBorderShare;
			rect.bottom = rect.top + m_cySplitter;
			rect.left = rectInside.left;
			for (int col = 0; col < m_nCols - 1; col++)
			{
				rect.left += m_pColInfo[col].nCurSize + m_cxBorderShare;
				rect.right = rect.left + m_cxSplitter;
				OnDrawSplitter(&dc, splitIntersection, rect);
				rect.left = rect.right + m_cxBorderShare;
			}
			rect.top = rect.bottom + m_cxBorderShare;
		}
	}
	else {
		CSplitterWnd::OnPaint();
	}
}

void CCJFlatSplitterWnd::HideRow(int nRowHide)
{
	ASSERT_VALID( this );
	ASSERT( m_nRows > 1 );
	ASSERT( nRowHide < m_nRows );
	ASSERT( m_nHiddenRow == -1 );
	m_nHiddenRow = nRowHide;
	
	int nActiveRow, nActiveCol;

    // if the nRow has an active window -- change it
	if( GetActivePane( &nActiveRow, &nActiveCol ) != NULL )
	{
		if( nActiveRow == nRowHide )
		{
			if( ++nActiveRow >= m_nRows )
				nActiveRow = 0;
			SetActivePane( nActiveRow, nActiveCol );
		}
	}

    // hide all nRow panes.
	for( int nCol = 0; nCol < m_nCols; ++nCol )
	{
		CWnd* pPaneHide = GetPane( nRowHide, nCol );
		ASSERT( pPaneHide != NULL );

		pPaneHide->ShowWindow( SW_HIDE );
		pPaneHide->SetDlgCtrlID( AFX_IDW_PANE_FIRST+nCol + 16*m_nRows );
		
		for( int nRow = nRowHide+1; nRow < m_nRows; ++nRow )
		{
			CWnd* pPane = GetPane( nRow, nCol );
			ASSERT( pPane != NULL );

			pPane->SetDlgCtrlID( IdFromRowCol( nRow-1, nCol ));
		}
	}

	m_nRows--;
	m_pRowInfo[m_nRows].nCurSize = m_pRowInfo[nRowHide].nCurSize;
	RecalcLayout();
}

void CCJFlatSplitterWnd::ShowRow()
{
	ASSERT_VALID( this );
	ASSERT( m_nRows < m_nMaxRows );
	ASSERT( m_nHiddenRow != -1 );

	int nShowRow = m_nHiddenRow;
	m_nHiddenRow = -1;

	int cyNew = m_pRowInfo[m_nRows].nCurSize;
	m_nRows++;  // add a nRow
	
	ASSERT( m_nRows == m_nMaxRows );
	
	int nRow;

    // Show the hidden nRow
	for( int nCol = 0; nCol < m_nCols; ++nCol )
	{
		CWnd* pPaneShow = GetDlgItem( AFX_IDW_PANE_FIRST+nCol + 16*m_nRows );
		ASSERT( pPaneShow != NULL );
		pPaneShow->ShowWindow( SW_SHOWNA );

		for( nRow = m_nRows - 2; nRow >= nShowRow; --nRow )
		{
			CWnd* pPane = GetPane( nRow, nCol );
			ASSERT( pPane != NULL );
			pPane->SetDlgCtrlID( IdFromRowCol( nRow + 1, nCol ));
		}

		pPaneShow->SetDlgCtrlID( IdFromRowCol( nShowRow, nCol ));
	}

    // new panes have been created -- recalculate layout
	for( nRow = nShowRow+1; nRow < m_nRows; nRow++ )
		m_pRowInfo[nRow].nIdealSize = m_pRowInfo[nRow - 1].nCurSize;

	m_pRowInfo[nShowRow].nIdealSize = cyNew;
	RecalcLayout();
}

void CCJFlatSplitterWnd::HideColumn(int nColHide)
{
	ASSERT_VALID( this );
	ASSERT( m_nCols > 1 );
	ASSERT( nColHide < m_nCols );
	ASSERT( m_nHiddenCol == -1 );
	m_nHiddenCol = nColHide;
	
    // if the column has an active window -- change it
	int nActiveRow, nActiveCol;
	if( GetActivePane( &nActiveRow, &nActiveCol ) != NULL )
	{
		if(	nActiveCol == nColHide )
		{
			if( ++nActiveCol >= m_nCols )
				nActiveCol = 0;
			SetActivePane( nActiveRow, nActiveCol );
		}
	}

    // hide all column panes
	for( int nRow = 0; nRow < m_nRows; nRow++)
	{
		CWnd* pPaneHide = GetPane(nRow, nColHide);
		ASSERT( pPaneHide != NULL );

		pPaneHide->ShowWindow(SW_HIDE);
		pPaneHide->SetDlgCtrlID( AFX_IDW_PANE_FIRST+m_nCols + 16*nRow );
		
		for( int nCol = nColHide + 1; nCol < m_nCols; nCol++ )
		{
			CWnd* pPane = GetPane( nRow, nCol );
			ASSERT( pPane != NULL );

			pPane->SetDlgCtrlID( IdFromRowCol( nRow, nCol - 1 ));
		}
	}

	m_nCols--;
	m_pColInfo[m_nCols].nCurSize = m_pColInfo[nColHide].nCurSize;
	RecalcLayout();
}

void CCJFlatSplitterWnd::ShowColumn()
{
	ASSERT_VALID( this );
	ASSERT( m_nCols < m_nMaxCols );
	ASSERT( m_nHiddenCol != -1 );

	int nShowCol = m_nHiddenCol;
	m_nHiddenCol = -1;

	int cxNew = m_pColInfo[m_nCols].nCurSize;
	m_nCols++;  // add a column
	
	ASSERT( m_nCols == m_nMaxCols );
	
	int nCol;

    // Show the hidden column
	for( int nRow = 0; nRow < m_nRows; ++nRow )
	{
		CWnd* pPaneShow = GetDlgItem( AFX_IDW_PANE_FIRST+m_nCols + 16*nRow );
		ASSERT( pPaneShow != NULL );
		pPaneShow->ShowWindow( SW_SHOWNA );

		for( nCol = m_nCols - 2; nCol >= nShowCol; --nCol )
		{
			CWnd* pPane = GetPane( nRow, nCol );
			ASSERT( pPane != NULL );
			pPane->SetDlgCtrlID( IdFromRowCol( nRow, nCol + 1 ));
		}

		pPaneShow->SetDlgCtrlID( IdFromRowCol( nRow, nShowCol ));
	}

    // new panes have been created -- recalculate layout
	for( nCol = nShowCol+1; nCol < m_nCols; nCol++ )
		m_pColInfo[nCol].nIdealSize = m_pColInfo[nCol - 1].nCurSize;

	m_pColInfo[nShowCol].nIdealSize = cxNew;
	RecalcLayout();
}

BOOL CCJFlatSplitterWnd::ReplaceView(int nRow, int nCol, CRuntimeClass *pViewClass)
{
	CView *pView = DYNAMIC_DOWNCAST( CView, GetPane( nRow, nCol ));
	ASSERT_KINDOF( CView, pView );

	if( pView->IsKindOf( pViewClass ))
		return FALSE;

	// Get pointer to CDocument object so that it can be used in the creation
	// process of the new view
	CDocument *pDoc = pView->GetDocument();

	int nID = pView->GetDlgCtrlID();
	int nWidth, nHeight, nMinWidth, nMinHeight;

	GetRowInfo( nRow, nWidth, nMinWidth );
	GetColumnInfo( nCol, nHeight, nMinHeight );

	// Delete existing view
	DeleteView(nRow, nCol);

	// Create new view
	CCreateContext context;
	context.m_pCurrentDoc = pDoc;
	CreateView( nRow, nCol, pViewClass, CSize( nWidth, nHeight ), &context );
	pView = DYNAMIC_DOWNCAST( CView, GetPane( nRow, nCol ));
	ASSERT_KINDOF( CView, pView );

	pView->SetDlgCtrlID( nID );

	SetRowInfo( nRow, nWidth, nMinWidth );
	SetColumnInfo( nCol, nHeight, nMinHeight );
	RecalcLayout();

	return TRUE;
}

BOOL CCJFlatSplitterWnd::SwitchView(int nRow, int nCol, CView *pNewView)
{
	CView *pOldView = DYNAMIC_DOWNCAST( CView, GetPane( nRow, nCol ));
	ASSERT_KINDOF( CView, pOldView );

	if( pOldView == pNewView )
		return FALSE;

	int	nOldID, nNewID;

	nOldID = pOldView->GetDlgCtrlID();
	nNewID = pNewView->GetDlgCtrlID();

	pOldView->ShowWindow(SW_HIDE);
	pNewView->ShowWindow(SW_SHOW);

	pOldView->SetDlgCtrlID(nNewID);
	pNewView->SetDlgCtrlID(nOldID);

	RecalcLayout();

	return TRUE;
}
