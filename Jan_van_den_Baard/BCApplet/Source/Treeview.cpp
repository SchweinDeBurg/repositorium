//
//	treeview.cpp
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//

#include "stdafx.h"

#include "bcc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor.
SelectionTree::SelectionTree() 
{
	// Default data.
	m_pBar = NULL;
	m_pCurrPage = NULL;
}

// Destructor.
SelectionTree::~SelectionTree()
{
}

// Add an item to the tree.
HTREEITEM SelectionTree::AddStringItem( LPCTSTR pszName, Page *pPage /*= NULL*/, HTREEITEM hParent /*= TVI_ROOT*/, BOOL bFirst /* = FALSE */ )
{
	_ASSERT_VALID( GetSafeHWND() );
	
	// Imagelist valid?
	if (( HIMAGELIST )m_Images == NULL )
	{
		// No. Create and add it.
		m_Images.LoadBitmap( MAKEINTRESOURCE( IDB_MAIN ), 16, 0, CLR_NONE );
		SetImageList( m_Images, TVSIL_NORMAL );
	}

	// First we setup the string by copying it or
	// loading it from the resources.
	ClsString str( pszName );

	// Create the item structure.
	TVINSERTSTRUCT	tvs;
	tvs.hParent		= hParent;
	tvs.hInsertAfter	= bFirst ? TVI_FIRST : TVI_LAST;
	tvs.item.mask		= TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvs.item.pszText	= str;
	tvs.item.lParam		= ( LPARAM )pPage;
	tvs.item.iImage		= 0;
	tvs.item.iSelectedImage = 1;
	
	// Add the item.
	return InsertItem( tvs );
}

// Get the parser which belongs to the currently
// selected item.
LPPARSER SelectionTree::GetCurrParser()
{
	// Obtain the currently selected item.
	HTREEITEM hCurr = GetSelection();

	// Is this a sub-item or one of the language items?
	// If not we need this one to determine the parser.
	HTREEITEM hParent = GetParentItem( hCurr );
	if ( hParent ) hCurr = hParent;

	// Do we have a valid entry?
	if ( hCurr )
	{
		// Obtain the item name.
		TCHAR	szName[ 256 ];
		TVITEM tvi;
		tvi.mask       = TVIF_TEXT;
		tvi.hItem      = hCurr;
		tvi.pszText    = szName;
		tvi.cchTextMax = 256;
		if ( GetItem( tvi ))
		{
			// Obtain the parser from the name.
			LPPARSER pParser;
			for ( pParser = ( LPPARSER )plParsers.lpFirst; pParser->lpNext; pParser = pParser->lpNext )
			{
				// Is this the one?
				if ( _tcsicmp( szName, pParser->pszTitle ) == 0 )
					return pParser;
			}
		}
	}
	return NULL;
}

// Selection has changed.
LRESULT SelectionTree::OnSelChanged( LPNMTREEVIEW pNMHDR, BOOL& bNotifyParent )
{
	// Get the current page so we can show it.
	HTREEITEM hSelected = GetSelection();

	// OK?
	if ( hSelected )
	{
		// Get the item information.
		TVITEM  tvi;
		TCHAR	sz[ 256 ];
		tvi.mask       = TVIF_PARAM | TVIF_TEXT;
		tvi.hItem      = hSelected;
		tvi.pszText    = sz;
		tvi.cchTextMax = 256;

		if ( GetItem( tvi ))
		{
			// Valid?
			if ( tvi.lParam )
			{
				// Did the page change from the page which is
				// currently visible?
				Page *pPage = ( Page * )tvi.lParam;
				if ( pPage != m_pCurrPage && m_pCurrPage )
				{
					// Hide the current page and show
					// the new page.
					m_pCurrPage->ShowWindow( SW_HIDE );
					pPage->ShowWindow( SW_SHOW );
				}
				else if ( ! m_pCurrPage )
					// Show the new page.
					pPage->ShowWindow( SW_SHOW );

				// Refresh the page contents and save
				// a pointer to it.
				pPage->RefreshData( GetCurrParser());
				m_pCurrPage = pPage;
				
				// Setup the information bar icon
				// and title.
				if ( m_pBar ) 
				{
					m_pBar->SetIcon( MAKEINTRESOURCE( pPage->GetIcon()));
					m_pBar->SetWindowText( ClsString( _T( " " )) + sz );
				}
			}
		}
	}
	return 0;
}

// Item collapsing?
LRESULT SelectionTree::OnItemExpanding(  LPNMTREEVIEW pNMHDR, BOOL& bNotifyParent )
{
	// We refuse to collapse the selected item.
	if ( pNMHDR->itemNew.hItem == GetSelection() && pNMHDR->action == TVE_COLLAPSE )
		return TRUE;
	return FALSE;
}
