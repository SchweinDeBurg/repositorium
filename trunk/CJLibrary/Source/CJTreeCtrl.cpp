// CJTreeCtrl.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Based on the CTreeView articles from http://www.codeguru.com/treeview
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/30/99 1:54p $
 * $Revision: 6 $
 * $Archive: /CodeJock/CJLibrary/CJTreeCtrl.cpp $
 *
 * $History: CJTreeCtrl.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 1:54p
 * Updated in $/CodeJock/CJLibrary
 * Overrode OnEraseBkgnd(...) to handle flicker free drawing.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/25/99   Time: 10:52p
 * Updated in $/CodeJock/CJLibrary
 * 21. Fixed bug with CCJTreeCtrl where searching for a string in the tree
 * and this string is currently selected, the method will return NULL.
 * Fixed bug where tree control item font was changed to a smaller font
 * than the previous font, part of the old font would still show.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/CJLibrary
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:06a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Created in $/CodeJockey/CJLibrary
 * Initial creation.
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJResource.h"
#include "CJTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJTreeCtrl

CCJTreeCtrl::CCJTreeCtrl()
{
	m_hItemFirstSel = NULL;
	m_dwDragStart   = NULL;
	m_bMultiSelect = FALSE;
}

CCJTreeCtrl::~CCJTreeCtrl()
{
}

IMPLEMENT_DYNAMIC(CCJTreeCtrl, CTreeCtrl)

BEGIN_MESSAGE_MAP(CCJTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CCJTreeCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJTreeCtrl message handlers

// Taken from: Get next item in outline by Zafir Anjum.
// http://www.codeguru.com/treeview/get_next.shtml

// GetNextItem  - Get next item as if outline was completely expanded
// Returns      - The item immediately below the reference item
// hItem        - The reference item
HTREEITEM CCJTreeCtrl::GetNextItem( HTREEITEM hItem )
{
	HTREEITEM hti = NULL;
	
	if (ItemHasChildren(hItem))
		hti = GetChildItem(hItem);
	
	if (hti == NULL) {
		while ((hti = GetNextSiblingItem(hItem)) == NULL) {
			if ((hItem = GetParentItem(hItem)) == NULL)
				return NULL;
		}
	}
	return hti;
}

// Taken from: Get previous item in outline by Zafir Anjum.
// http://www.codeguru.com/treeview/get_prev.shtml

// GetPrevItem  - Get previous item as if outline was completely expanded
// Returns      - The item immediately above the reference item
// hItem        - The reference item
HTREEITEM CCJTreeCtrl::GetPrevItem( HTREEITEM hItem )
{
	HTREEITEM       hti;
	
	hti = GetPrevSiblingItem(hItem);
	if( hti == NULL )
		hti = GetParentItem(hItem);
	else
		hti = GetLastItem(hti);
	return hti;
}

// Taken from: Get last item in branch by Zafir Anjum.
// http://www.codeguru.com/treeview/get_last.shtml

// GetLastItem  - Gets last item in the branch
// Returns      - Last item
// hItem        - Node identifying the branch. NULL will 
//                return the last item in outine
HTREEITEM CCJTreeCtrl::GetLastItem( HTREEITEM hItem )
{
	//	Temporary used variable
	HTREEITEM	htiNext;
	
	if( hItem == NULL ) {
		// Get the last item at the top level
		hItem = GetRootItem();
	}
	
	while( ItemHasChildren( hItem ) != NULL ) {
		//	Find the last child of hItem
		htiNext = GetChildItem( hItem );
		while( htiNext != NULL ) {
			hItem = htiNext;
			htiNext = GetNextSiblingItem( htiNext );
		}
	}
	
	return hItem;
	
}

// Taken from: Finding an item by Zafir Anjum.
// http://www.codeguru.com/treeview/find_item.shtml

// FindItem		- Finds an item that contains the search string
// Returns		- Handle to the item or NULL
// str			- String to search for
// bCaseSensitive	- Should the search be case sensitive
// bDownDir		- Search direction - TRUE for down
// bWholeWord		- True if search should match whole words
// hItem		- Item to start searching from. NULL for
//			- currently selected item
HTREEITEM CCJTreeCtrl::FindItem(LPCTSTR lpszSearch, 
				BOOL bCaseSensitive /*= FALSE*/, 
				BOOL bDownDir /*= TRUE*/, 
				BOOL bWholeWord /*= FALSE*/, 
				HTREEITEM hItem /*= NULL*/)
{
	CString str = lpszSearch;

	int lenSearchStr = str.GetLength();
	if( lenSearchStr == 0 ) return NULL;

	// For the first pass, set the current item equal to the selection
	HTREEITEM htiSel = hItem ? hItem : GetSelectedItem();
	HTREEITEM htiCur = htiSel;
	CString sSearch = str;

	// make sure it ends if we started with no selection
	if(( htiCur == NULL ) && ( htiSel != NULL ))
	{
		if( bDownDir )
			htiCur = GetRootItem();
		else
			htiCur = GetLastItem( NULL );
	}

	if( !bCaseSensitive )
		sSearch.MakeLower();

	// For the first pass only, we check to see if it 
	// is the item we're looking for.
	BOOL bFirstPass = TRUE;

	while( htiCur && (htiCur != htiSel || bFirstPass) )
	{
		bFirstPass = FALSE;

		CString sItemText = GetItemText( htiCur );
		if( !bCaseSensitive )
			sItemText.MakeLower();

		int n;
		while( (n = sItemText.Find( sSearch )) != -1 )
		{
			// Search string found
			if( bWholeWord )
			{
				// Check preceding char
				if( n != 0 )
				{
					if( isalpha(sItemText[n-1]) || 
					    	sItemText[n-1] == '_' ){
						// Not whole word
						sItemText = sItemText.Right(
							sItemText.GetLength() - n - 
							lenSearchStr );
						continue;
					}
				}

				// Check succeeding char
				if( sItemText.GetLength() > n + lenSearchStr
					&& ( isalpha(sItemText[n+lenSearchStr]) ||
					sItemText[n+lenSearchStr] == '_' ) )
				{
					// Not whole word
					sItemText = sItemText.Right( sItemText.GetLength() 
							- n - sSearch.GetLength() );
					continue;
				}
			}
			
			if( IsFindValid( htiCur ) )
				return htiCur;
			else break;
		}


		htiCur = bDownDir ? GetNextItem( htiCur ) : GetPrevItem( htiCur );
		if( htiCur == NULL )
		{
			if( bDownDir )  htiCur = GetRootItem();
			else htiCur = GetLastItem( NULL );
		}
	}
	return NULL;
}

// Taken from: Finding an item by Zafir Anjum.
// http://www.codeguru.com/treeview/find_item.shtml

// IsFindValid	- Virtual function used by FindItem to allow this
//		  function to filter the result of FindItem
// Returns	- True if item matches the criteria
// Arg		- Handle of the item
BOOL CCJTreeCtrl::IsFindValid( HTREEITEM )
{
	return TRUE;
}

BOOL CCJTreeCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( !CTreeCtrl::PreCreateWindow(cs))
		return FALSE;

	cs.style |= TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS;

	return TRUE;
}

// Taken from: Setting color and font attribute for individual items by Zafir Anjum.
// http://www.codeguru.com/treeview/item_color_font.shtml

void CCJTreeCtrl::SetItemFont(HTREEITEM hItem, LOGFONT& logfont)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		cf.color = (COLORREF)-1;
	cf.logfont = logfont;
	m_mapColorFont[hItem] = cf;
}

void CCJTreeCtrl::SetItemBold(HTREEITEM hItem, BOOL bBold)
{
	SetItemState( hItem, bBold ? TVIS_BOLD: 0, TVIS_BOLD );
}

void CCJTreeCtrl::SetItemColor(HTREEITEM hItem, COLORREF color)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		cf.logfont.lfFaceName[0] = '\0';
	cf.color = color;
	m_mapColorFont[hItem] = cf;
}

BOOL CCJTreeCtrl::GetItemFont(HTREEITEM hItem, LOGFONT * plogfont)
{
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		return FALSE;
	if( cf.logfont.lfFaceName[0] == '\0' ) 
		return FALSE;
	*plogfont = cf.logfont;
	return TRUE;

}

BOOL CCJTreeCtrl::GetItemBold(HTREEITEM hItem)
{
	return GetItemState( hItem, TVIS_BOLD ) & TVIS_BOLD;
}

COLORREF CCJTreeCtrl::GetItemColor(HTREEITEM hItem)
{
	// Returns (COLORREF)-1 if color was not set
	Color_Font cf;
	if( !m_mapColorFont.Lookup( hItem, cf ) )
		return (COLORREF)-1;
	return cf.color;

}

void CCJTreeCtrl::OnPaint() 
{
	CPaintDC dc(this);
	
	// Create a memory DC compatible with the paint DC
	CDC memDC;
	memDC.CreateCompatibleDC( &dc );
	
	CRect rcClip, rcClient;
	dc.GetClipBox( &rcClip );
	GetClientRect(&rcClient);
	
	// Select a compatible bitmap into the memory DC
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
	CBitmap* pOldBitmap = memDC.SelectObject( &bitmap );
	
	// Set clip region to be same as that in paint DC
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcClip );
	memDC.SelectClipRgn(&rgn);
	rgn.DeleteObject();
	
	// First let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );
	
	HTREEITEM hItem = GetFirstVisibleItem();
	
	int n = GetVisibleCount()+1;
	while( hItem && n--)
	{
		CRect rect;
		
		// Do not meddle with selected items or drop highlighted items
		UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;
		Color_Font cf;
		
		if ( !(GetItemState( hItem, selflag ) & selflag ) 
			&& m_mapColorFont.Lookup( hItem, cf ))
		{
			CFont *pFontDC;
			CFont fontDC;
			LOGFONT logfont;
			
			if( cf.logfont.lfFaceName[0] != '\0' ) 
			{
				logfont = cf.logfont;
			}
			else
			{
				// No font specified, so use window font
				CFont *pFont = GetFont();
				pFont->GetLogFont( &logfont );
			}

			CString sItem = GetItemText( hItem );
			GetItemRect( hItem, &rect, TRUE );

			// cover up over old font.
			CFont* pOldFont = memDC.SelectObject(GetFont());
			memDC.SetTextColor(::GetSysColor( COLOR_WINDOW ));
			memDC.SetBkColor( GetSysColor( COLOR_WINDOW ) );
			memDC.TextOut( rect.left+2, rect.top+1, sItem );
			memDC.SelectObject(pOldFont);

			if( GetItemBold( hItem ) )
				logfont.lfWeight = 700;
			fontDC.CreateFontIndirect( &logfont );
			pFontDC = memDC.SelectObject( &fontDC );
			
			if( cf.color != (COLORREF)-1 )
				memDC.SetTextColor( cf.color );
			
			memDC.SetBkColor( GetSysColor( COLOR_WINDOW ) );
			memDC.TextOut( rect.left+2, rect.top+1, sItem );
			
			memDC.SelectObject( pFontDC );

			// fix potential resource leak - KStowell - 10-22-99.
			fontDC.DeleteObject();
		}
		hItem = GetNextVisibleItem( hItem );
	}
	
	
	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, 
		rcClip.left, rcClip.top, SRCCOPY );

	// fix potential resource leak - KStowell - 10-21-99
	memDC.SelectObject( pOldBitmap );
	memDC.DeleteDC();
	bitmap.DeleteObject();
}

// Taken from: Allowing multiple selection by Zafir Anjum.
// http://www.codeguru.com/treeview/multiple_sel.shtml

void CCJTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bMultiSelect)
	{
		// Set focus to control if key strokes are needed.
		// Focus is not automatically given to control on lbuttondown
		
		m_dwDragStart = GetTickCount();
		
		if(nFlags & MK_CONTROL ) 
		{
			// Control key is down
			UINT flag;
			HTREEITEM hItem = HitTest( point, &flag );
			if( hItem )
			{
				// Toggle selection state
				UINT uNewSelState = 
					GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED ? 
					0 : TVIS_SELECTED;
            
				// Get old selected (focus) item and state
				HTREEITEM hItemOld = GetSelectedItem();
				UINT uOldSelState  = hItemOld ? 
					GetItemState(hItemOld, TVIS_SELECTED) : 0;
            
				// Select new item
				if( GetSelectedItem() == hItem )
					SelectItem( NULL );		// to prevent edit
				CTreeCtrl::OnLButtonDown(nFlags, point);
				
				// Set proper selection (highlight) state for new item
				SetItemState(hItem, uNewSelState,  TVIS_SELECTED);
				
				// Restore state of old selected item
				if (hItemOld && hItemOld != hItem)
					SetItemState(hItemOld, uOldSelState, TVIS_SELECTED);
				
				m_hItemFirstSel = NULL;
				
				return;
			}
		} 
		else if(nFlags & MK_SHIFT)
		{
			// Shift key is down
			UINT flag;
			HTREEITEM hItem = HitTest( point, &flag );
			
			// Initialize the reference item if this is the first shift selection
			if( !m_hItemFirstSel )
				m_hItemFirstSel = GetSelectedItem();
			
			// Select new item
			if( GetSelectedItem() == hItem )
				SelectItem( NULL );			// to prevent edit
			CTreeCtrl::OnLButtonDown(nFlags, point);
			
			if( m_hItemFirstSel )
			{
				SelectItems( m_hItemFirstSel, hItem );
				return;
			}
		}
		else
		{
			// Normal - remove all selection and let default 
			// handler do the rest
			ClearSelection();
			m_hItemFirstSel = NULL;
		}
	}
	
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CCJTreeCtrl::ClearSelection()
{
	if(m_bMultiSelect)
	{
		// This can be time consuming for very large trees 
		// and is called every time the user does a normal selection
		// If performance is an issue, it may be better to maintain 
		// a list of selected items
		for ( HTREEITEM hItem=GetRootItem(); hItem!=NULL; hItem=GetNextItem( hItem ) )
			if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
				SetItemState( hItem, 0, TVIS_SELECTED );
	}
}

// SelectItems	- Selects items from hItemFrom to hItemTo. Does not
//		- select child item if parent is collapsed. Removes
//		- selection from all other items
// hItemFrom	- item to start selecting from
// hItemTo	- item to end selection at.
BOOL CCJTreeCtrl::SelectItems(HTREEITEM hItemFrom, HTREEITEM hItemTo)
{
	if(m_bMultiSelect)
	{
		HTREEITEM hItem = GetRootItem();

		// Clear selection upto the first item
		while ( hItem && hItem!=hItemFrom && hItem!=hItemTo )
		{
			hItem = GetNextVisibleItem( hItem );
			SetItemState( hItem, 0, TVIS_SELECTED );
		}

		if ( !hItem )
			return FALSE;	// Item is not visible

		SelectItem( hItemTo );

		// Rearrange hItemFrom and hItemTo so that hItemFirst is at top
		if( hItem == hItemTo )
		{
			hItemTo = hItemFrom;
			hItemFrom = hItem;
		}


		// Go through remaining visible items
		BOOL bSelect = TRUE;
		while ( hItem )
		{
			// Select or remove selection depending on whether item
			// is still within the range.
			SetItemState( hItem, bSelect ? TVIS_SELECTED : 0, TVIS_SELECTED );

			// Do we need to start removing items from selection
			if( hItem == hItemTo ) 
				bSelect = FALSE;

			hItem = GetNextVisibleItem( hItem );
		}

		return TRUE;
	}

	return FALSE;
}

HTREEITEM CCJTreeCtrl::GetFirstSelectedItem()
{
	if(m_bMultiSelect)
	{
		for ( HTREEITEM hItem = GetRootItem(); hItem!=NULL; hItem = GetNextItem( hItem ) )
			if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
				return hItem;
	}

	return NULL;
}

HTREEITEM CCJTreeCtrl::GetNextSelectedItem(HTREEITEM hItem)
{
	if(m_bMultiSelect)
	{
		for ( hItem = GetNextItem( hItem ); hItem!=NULL; hItem = GetNextItem( hItem ) )
			if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
				return hItem;
	}

	return NULL;
}

HTREEITEM CCJTreeCtrl::GetPrevSelectedItem(HTREEITEM hItem)
{
	if(m_bMultiSelect)
	{
		for ( hItem = GetPrevItem( hItem ); hItem!=NULL; hItem = GetPrevItem( hItem ) )
			if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
				return hItem;
	}

	return NULL;
}

BOOL CCJTreeCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// KStowell - overridden for flicker-free drawing.
	UNUSED_ALWAYS(pDC);
	return TRUE;
}
