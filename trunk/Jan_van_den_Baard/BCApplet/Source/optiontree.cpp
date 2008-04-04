//
//	optiontree.cpp
//
//	(C) Copyright 2002 by Jan van den Baard.
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
OptionTree::OptionTree()
{
}

// Destructor.
OptionTree::~OptionTree()
{
}

// Initialize tree.
BOOL OptionTree::Initialize()
{
	// Like the MSDN docs say, first create the control, then
	// set the TVS_CHECKBOXES style and then populate the tree.
	ModifyStyle( 0, TVS_CHECKBOXES );

	// Add the options.
	AddOption( ClsString( MAKEINTRESOURCE( IDS_AUTOMATCH )));
	AddSubOption( ClsString( MAKEINTRESOURCE( IDS_AUTOMATCH )), ClsString( MAKEINTRESOURCE( IDS_VISIBLE_ERROR )));
	AddOption( ClsString( MAKEINTRESOURCE( IDS_COLOR_MATCH )));
	AddOption( ClsString( MAKEINTRESOURCE( IDS_CLEAR_SPACES )));
	AddOption( ClsString( MAKEINTRESOURCE( IDS_ENABLE_DND )));
	AddOption( ClsString( MAKEINTRESOURCE( IDS_SHOW_MARGIN )));
	AddSubOption( ClsString( MAKEINTRESOURCE( IDS_SHOW_MARGIN )), ClsString( MAKEINTRESOURCE( IDS_SOLID_MARGIN )));
	AddOption( ClsString( MAKEINTRESOURCE( IDS_LINE_NUMBERS )));
	AddOption( ClsString( MAKEINTRESOURCE( IDS_FAST_SCROLL )));
	AddOption( ClsString( MAKEINTRESOURCE( IDS_CLEAR_UNDO )));
	AddOption( ClsString( MAKEINTRESOURCE( IDS_VISIBLE_SPACE )));
	AddOption( ClsString( MAKEINTRESOURCE( IDS_CASE_FIXING )));
	AddOption( ClsString( MAKEINTRESOURCE( IDS_HIGHLIGHT_LINE )));
	AddOption( ClsString( MAKEINTRESOURCE( IDS_PARSEHYPERLINKS )));
	AddSubOption( ClsString( MAKEINTRESOURCE( IDS_PARSEHYPERLINKS )), ClsString( MAKEINTRESOURCE( IDS_SHOWHYPERTIPS )));
	return TRUE;
}

// Setup the selections.
BOOL OptionTree::SetOptions( LPPARSER lpp )
{
	// Store parser pointer.
	m_pParser = lpp;

	// Set the options.
	SetOption( ClsString( MAKEINTRESOURCE( IDS_AUTOMATCH )), &lpp->bAutoMatch );
	SetOption( ClsString( MAKEINTRESOURCE( IDS_VISIBLE_ERROR )), &lpp->bVisibleMatchError );
	SetOption( ClsString( MAKEINTRESOURCE( IDS_COLOR_MATCH )), &lpp->bColorBracketMatches );
	SetOption( ClsString( MAKEINTRESOURCE( IDS_CLEAR_SPACES )), &lpp->bClearTrailingSpaces );
	SetOption( ClsString( MAKEINTRESOURCE( IDS_ENABLE_DND )), &lpp->bDnDEnabled );
	SetOption( ClsString( MAKEINTRESOURCE( IDS_SHOW_MARGIN )), &lpp->bSelectionMargin );
	SetOption( ClsString( MAKEINTRESOURCE( IDS_SOLID_MARGIN )), &lpp->bSolidMargin );
	SetOption( ClsString( MAKEINTRESOURCE( IDS_LINE_NUMBERS )), &lpp->bLineNumbers );
	SetOption( ClsString( MAKEINTRESOURCE( IDS_FAST_SCROLL )), &lpp->bFastScroll );
	SetOption( ClsString( MAKEINTRESOURCE( IDS_CLEAR_UNDO )), &lpp->bClearUndoSave );
	SetOption( ClsString( MAKEINTRESOURCE( IDS_VISIBLE_SPACE )), &lpp->bVisibleSpaces );
	SetOption( ClsString( MAKEINTRESOURCE( IDS_CASE_FIXING )), &lpp->bCaseFixing );
	SetOption( ClsString( MAKEINTRESOURCE( IDS_HIGHLIGHT_LINE )), &lpp->bHighlightCurrentLine );
	SetOption( ClsString( MAKEINTRESOURCE( IDS_PARSEHYPERLINKS )), &lpp->bParseHyperLinks );
	SetOption( ClsString( MAKEINTRESOURCE( IDS_SHOWHYPERTIPS )), &lpp->bShowHyperTips );
	return TRUE;
}

// Add an option.
BOOL OptionTree::AddOption( LPCTSTR pszName )
{
	TVINSERTSTRUCT	tvs;
	tvs.hParent		= TVI_ROOT;
	tvs.hInsertAfter	= TVI_LAST;
	tvs.item.mask		= TVIF_TEXT;
	tvs.item.pszText	= ( LPTSTR )pszName;
	tvs.item.cchTextMax	= _tcslen( pszName );
	return ( BOOL )InsertItem( tvs );
}

// Set an option.
BOOL OptionTree::SetOption( LPCTSTR pszName, LPBOOL pStorage )
{
	// Find the option.
	HTREEITEM hItem = FindOption( pszName );
	if ( hItem )
	{
		// Set the storage pointer.
		TVITEM tvi;
		tvi.hItem  = hItem;
		tvi.mask   = TVIF_PARAM;
		tvi.lParam = ( LPARAM )pStorage;
		if ( SetItem( tvi ))
		{
			// Set checked state.
			return SetCheckState( hItem, *pStorage );
		}
	}
	return FALSE;
}

// Add a sub-option.
BOOL OptionTree::AddSubOption( LPCTSTR pszParent, LPCTSTR pszName )
{
	TVINSERTSTRUCT	tvs;
	tvs.hParent		= FindOption( pszParent, GetRoot());
	tvs.hInsertAfter	= TVI_LAST;
	tvs.item.mask		= TVIF_TEXT;
	tvs.item.pszText	= ( LPTSTR )pszName;
	tvs.item.cchTextMax	= _tcslen( pszName );

	// Insert the item and expand
	// it's parent.
	if ( InsertItem( tvs ))
	{
		Expand( tvs.hParent, TVE_EXPAND );
		return TRUE;
	}
	return FALSE;
}

// Find an option by it's name. It simply goes through
// the tree using recursion.
HTREEITEM OptionTree::FindOption( LPCTSTR pszName, HTREEITEM hItem /* = NULL */ )
{
	// Does it have children?
	HTREEITEM hChild = GetChild( hItem );
	if ( hChild != NULL )
	{
		// Try to find it among it's children.
		hChild = FindOption( pszName, hChild );
		if ( hChild )
			return hChild;
	}

	// Obtain the item text
	TVITEM tvi;
	tvi.hItem      = hItem;
	tvi.mask       = TVIF_TEXT;
	tvi.pszText    = m_szText;
	tvi.cchTextMax = 4096;
	if ( GetItem( tvi ))
	{
		// Is this the option?
		if ( _tcsnicmp( m_szText, pszName, 4096 ) == 0 )
			return hItem;
		else
		{
			// Try from the next.
			hItem = GetNextSibling( hItem );
			if ( hItem )
				// Next...
				return FindOption( pszName, hItem );
		}
	}
	return NULL;
}

// Set the checked state.
BOOL OptionTree::SetCheckState( HTREEITEM hItem, BOOL bChecked )
{
	// Obtain item state.
	TVITEM tvi;
	tvi.mask      = TVIF_HANDLE | TVIF_STATE;
	tvi.hItem     = hItem;
	tvi.stateMask = TVIS_STATEIMAGEMASK;
	tvi.state     = INDEXTOSTATEIMAGEMASK(( bChecked ? 2 : 1 ));
	return SetItem( tvi );
}

// Obtain the checked state.
BOOL OptionTree::GetCheckState( HTREEITEM hItem )
{
	// Obtain item state.
	TVITEM tvi;
	tvi.mask      = TVIF_HANDLE | TVIF_STATE;
	tvi.hItem     = hItem;
	tvi.stateMask = TVIS_STATEIMAGEMASK;

	// Request the information.
	if ( GetItem( tvi ))
		// Return zero if it's not checked, or nonzero otherwise.
		return (( BOOL )( tvi.state >> 12 ) -1 );
	return FALSE;
}

// Click inside the control. If it's on an item
// we select it.
LRESULT OptionTree::OnClick( LPNMHDR pNMHDR, BOOL& bNotifyParent )
{
	// No need to bother our parent.
	bNotifyParent = FALSE;

	// Look up the clicked item.
	TVHITTESTINFO tvht;
	GetCursorPos( &tvht.pt );
	ScreenToClient( &tvht.pt );

	// Get the item under the cursor and select
	// it. 
	HTREEITEM hClicked = HitTest( tvht );
	if ( hClicked && ( tvht.flags & TVHT_ONITEM ))
	{
		// Clear the current selection.
		SelectItem( NULL );

		// Toggle it's check state if the click was not
		// on the item it's state icon.
		if ( ! ( tvht.flags & TVHT_ONITEMSTATEICON ))
			SetCheckState( hClicked, ! GetCheckState( hClicked ));

		// Posting the message will make sure the tree has time to
		// update the checkbox before we get in our TVN_SELCHANGED
		// message handler below.
		PostMessage( TVM_SELECTITEM, TVGN_CARET, ( LPARAM )hClicked );
	}
	return 0;
}

// Selection changed.
LRESULT OptionTree::OnSelChanged( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent )
{
	// No need to bother our parent.
	bNotifyParent = FALSE;

	// Obtain item lParam.
	TVITEM tvi;
	tvi.hItem = pTreeView->itemNew.hItem;
	tvi.mask  = TVIF_HANDLE | TVIF_PARAM;
	if ( GetItem( tvi ))
	{
		// Get the item it's check state.
		BOOL bCheckState = GetCheckState( pTreeView->itemNew.hItem );
		LPBOOL pStorage = ( LPBOOL )tvi.lParam;

		// Compare that with it's stored state.
		if ( pStorage && bCheckState != *pStorage )
		{
			// Change the storage state.
			*pStorage = bCheckState;
			pSettings->Changed( m_pParser );
		}
	}
	return 0;
}

// It is not allowed to collapse any part of the
// option tree.
LRESULT OptionTree::OnItemExpanding( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent )
{
	// Tried to collapse?
	if ( pTreeView->action == TVE_COLLAPSE )
	{
		bNotifyParent = FALSE;
		return TRUE;
	}

	// Default behaviour...
	return ClsTreeView::OnItemExpanding( pTreeView, bNotifyParent );
}
