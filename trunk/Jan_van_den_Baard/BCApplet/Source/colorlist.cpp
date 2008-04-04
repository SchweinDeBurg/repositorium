//
//	colorlist.cpp
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

// Common colors string IDs.
int ColorList::m_nColors[] =
{
	IDS_TEXT,
	IDS_BACKGROUND,
	IDS_BACKGROUND_READONLY,
	IDS_MARGIN,
	IDS_BOOKMARK,
	IDS_NUMBER,
	IDS_DELIMITER,
	IDS_SELTEXT,
	IDS_SELTEXTBKGND,
	IDS_LINENUMBER,
	IDS_LINENUMBERBKGND,
	IDS_BACKGROUND_NUMBER,
	IDS_BACKGROUND_DELIM,
	IDS_BRACKET_MATCH,
	IDS_LINE_HIGHLIGHT,
	IDS_HYPERLINK,
	IDS_HYPERLINK_BKGND

};

// Default color flags.
DWORD ColorList::m_dwFlags[] =
{
	CF_DEFAULT_TEXT,
	CF_DEFAULT_BACKGROUND,
	CF_DEFAULT_BACKGROUND_RO,
	CF_DEFAULT_MARGIN,
	CF_DEFAULT_BOOKMARK,
	CF_DEFAULT_NUMBER,
	CF_DEFAULT_DELIMITER,
	CF_DEFAULT_SELECTED_TEXT,
	CF_DEFAULT_SELECTED_BKGND,
	CF_DEFAULT_LINE_NUMBERS,
	CF_DEFAULT_LINE_NUMBERS_BKGND,
	CF_DEFAULT_BACKGROUND_NUMBER,
	CF_DEFAULT_BACKGROUND_DELIMITER,
	CF_DEFAULT_BRACKET_MATCH,
	CF_DEFAULT_LINE_HIGHLIGHT,
	CF_DEFAULT_HYPERLINK,
	CF_DEFAULT_BACKGROUND_HYPERLINK
};

// Default colors.
COLORREF	ColorList::m_crDefaults[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Constructor.
ColorList::ColorList()
{
	// Setup defaults.
	m_bBlockList = FALSE;
	m_pKeys = NULL;
	m_pSmart = NULL;
}

// Destructor.
ColorList::~ColorList()
{
}

// Setup the list to display and act on the
// blocks array.
BOOL ColorList::SetBlockList()
{
	// Reset current contents.
	ResetContent();

	// Add each entry from the array.
	for ( int i = 0; i < ArrayGetSize( m_pParser->lpaBlocks ); i++ )
	{
		// Add the entry.
		if ( AddString(( LPCTSTR )::ArrayGetAt( m_pParser->lpaBlocks, i )) == LB_ERR )
			return FALSE;
	}
	// We are using the blocks array.
	m_bBlockList = TRUE;
	return TRUE;
}

// Setup the list to display keyword groups.
BOOL ColorList::SetKeywordList( LPKEYLIST pKeys )
{
	// Save the list pointer.
	m_pKeys = pKeys;

	// Reset current contents.
	ResetContent();

	// Add entries.
	for ( LPKEYWORDS pKW = pKeys->lpFirst; pKW->lpNext; pKW = pKW->lpNext )
	{
		// Add entry.
		if ( AddString(( LPCTSTR )pKW ) == LB_ERR )
			return FALSE;
	}
	return TRUE;
}

// Setup the list to display smart indention nodes.
BOOL ColorList::SetIndentionList( LPSMARTLIST pSmart )
{
	// Save the list pointer.
	m_pSmart = pSmart;

	// Reset current contents.
	ResetContent();

	// Add entries.
	for ( LPSMART pSM = pSmart->lpFirst; pSM->lpNext; pSM = pSM->lpNext )
	{
		// Add entry.
		if ( AddString(( LPCTSTR )pSM ) == LB_ERR )
			return FALSE;
	}
	return TRUE;
}

// Setup the list to display and act on
// the common colors.
BOOL ColorList::SetCommonList()
{
	ClsColor col( ::GetSysColor( COLOR_HIGHLIGHT )), col1( ::GetSysColor( COLOR_WINDOW ));
	col.LightenColor( 0.70, 0.70, 0.70 );
	col1.DarkenColor( 0.10, 0.10, 0.10 );

	// Setup defaults.
	m_crDefaults[ 0  ] = GetSysColor( COLOR_WINDOWTEXT );
	m_crDefaults[ 1  ] = GetSysColor( COLOR_WINDOW );
	m_crDefaults[ 2  ] = GetSysColor( COLOR_3DLIGHT );
	m_crDefaults[ 3  ] = GetSysColor( COLOR_SCROLLBAR );
	m_crDefaults[ 4  ] = RGB( 0, 255, 255 );
	m_crDefaults[ 5  ] = GetSysColor( COLOR_WINDOWTEXT );
	m_crDefaults[ 6  ] = GetSysColor( COLOR_WINDOWTEXT );
	m_crDefaults[ 7  ] = GetSysColor( COLOR_HIGHLIGHTTEXT );
	m_crDefaults[ 8  ] = GetSysColor( COLOR_HIGHLIGHT );
	m_crDefaults[ 9  ] = RGB( 0, 128, 128 );
	m_crDefaults[ 10 ] = ::GetSysColor( COLOR_WINDOW );
	m_crDefaults[ 11 ] = CLR_DEFAULT;
	m_crDefaults[ 12 ] = CLR_DEFAULT;
	m_crDefaults[ 13 ] = col;
	m_crDefaults[ 14 ] = col1;
	m_crDefaults[ 15 ] = RGB( 0, 0, 255 );
	m_crDefaults[ 16 ] = CLR_DEFAULT;

	// Reset the current contents.
	ResetContent();
	for ( int i = 0; i < CARR_SIZEOF; i++ )
	{
		// Store default into the parser.
		if ( m_pParser->dwColorFlags & m_dwFlags[ i ] )
			m_pParser->crColors[ i ] = m_crDefaults[ i ];

		// Add the entry.
		if ( AddString( ClsString( MAKEINTRESOURCE( m_nColors[ i ] ))) == LB_ERR )
			return FALSE;
	}

	// We are not displaying blocks.
	m_bBlockList = FALSE;
	return TRUE;
}

// Move an entry up.
void ColorList::MoveSelUp()
{
	// Indention list?
	if ( m_pSmart )
	{
		// Get selection data.
		LPSMART		lpSmart = ( LPSMART )GetItemData( GetCurSel());
		if ( lpSmart != ( LPSMART )LB_ERR )
		{
			LPSMART		lpPrev = lpSmart->lpPrev;
			SMART		sTemp;

			// Swap both data entries.
			sTemp.lpszSmartString    = lpPrev->lpszSmartString;
			sTemp.nDirection	 = lpPrev->nDirection;
			lpPrev->lpszSmartString	 = lpSmart->lpszSmartString;
			lpPrev->nDirection	 = lpSmart->nDirection;
			lpSmart->lpszSmartString = sTemp.lpszSmartString;
			lpSmart->nDirection	 = sTemp.nDirection;

			// Select the previous item which
			// in fact is the already selected
			// item.
			SetCurSel( GetCurSel() - 1 );
		}
		else
			return;
	}
	else
	{
		// Get selection.
		int nSel = GetCurSel();
		_ASSERT( nSel != LB_ERR );
		_ASSERT( m_bBlockList == TRUE );
		_ASSERT_VALID( m_pParser );

		// Get entries.
		LPBLOCK lpBlockA = ( LPBLOCK )::ArrayGetAt( m_pParser->lpaBlocks, nSel );
		LPBLOCK lpBlockB = ( LPBLOCK )::ArrayGetAt( m_pParser->lpaBlocks, nSel - 1 );

		// Swap contents.
		BLOCK bTmp  = *lpBlockA;
		*lpBlockA   = *lpBlockB;
		*lpBlockB   = bTmp;

		// Select one up.
		SetCurSel( nSel - 1 );
	}

	// Changes made...
	pSettings->Changed( m_pParser );
}

// Move an entry down.
void ColorList::MoveSelDown()
{
	// Indention list?
	if ( m_pSmart )
	{
		// Get selection data.
		LPSMART		lpSmart = ( LPSMART )GetItemData( GetCurSel());
		if ( lpSmart != ( LPSMART )LB_ERR )
		{
			LPSMART		lpNext = lpSmart->lpNext;
			SMART		sTemp;

			// Swap both data entries.
			sTemp.lpszSmartString    = lpNext->lpszSmartString;
			sTemp.nDirection	 = lpNext->nDirection;
			lpNext->lpszSmartString	 = lpSmart->lpszSmartString;
			lpNext->nDirection	 = lpSmart->nDirection;
			lpSmart->lpszSmartString = sTemp.lpszSmartString;
			lpSmart->nDirection	 = sTemp.nDirection;

			// Select the next item which
			// in fact is the already selected
			// item.
			SetCurSel( GetCurSel() +  1 );
		}
		else
			return;
	}
	else
	{
		// Get selection.
		int nSel = GetCurSel();
		_ASSERT( nSel != LB_ERR );
		_ASSERT( m_bBlockList == TRUE );
		_ASSERT_VALID( m_pParser );

		// Get entries.
		LPBLOCK lpBlockA = ( LPBLOCK )::ArrayGetAt( m_pParser->lpaBlocks, nSel );
		LPBLOCK lpBlockB = ( LPBLOCK )::ArrayGetAt( m_pParser->lpaBlocks, nSel + 1 );

		// Swap contents.
		BLOCK bTmp = *lpBlockA;
		*lpBlockA  = *lpBlockB;
		*lpBlockB  = bTmp;

		// Select one down.
		SetCurSel( nSel + 1 );
	}

	// Changes made...
	pSettings->Changed( m_pParser );
}

// Remove current selection.
void ColorList::RemoveSel()
{
	int nSel = GetCurSel();
	int nCnt = GetCount();
	_ASSERT( nSel != LB_ERR );

	// Indention list?
	if ( m_pSmart )
	{
		// Get selection data.
		LPSMART	lpSmart = ( LPSMART )GetItemData( nSel );

		// Remove the node from the list.
		DeleteString( GetCurSel());

		// Also from the indention list.
		Remove(( LPNODE )lpSmart );

		// Free the node and it's text.
		::FreePooled( pParserPool, lpSmart->lpszSmartString );
		::FreePooled( pParserPool, lpSmart );

		// Select the next or previous
		// item.
		if ( nSel == nCnt - 1 && nSel ) nSel--;
		SetCurSel( nSel );
	}
	else
	{
		// Get selection.
		_ASSERT( m_bBlockList == TRUE );
		_ASSERT_VALID( m_pParser );

		// Get selection to remove.
		int nPos = nSel;

		// Select the next or previous
		// item.
		if ( nSel == nCnt - 1 && nSel ) nSel--;
		//else	  		        nSel++;

		// Get the data from the array.
		LPBLOCK lpBlock = ( LPBLOCK )::ArrayGetAt( m_pParser->lpaBlocks, nPos );

		// Free the strings from the
		// block structure.
		if ( lpBlock->pszStart ) ::ArrayFreeMem( m_pParser->lpaBlocks, lpBlock->pszStart );
		if ( lpBlock->pszEnd && lpBlock->pszEnd != END_OF_LINE ) ::ArrayFreeMem( m_pParser->lpaBlocks, lpBlock->pszEnd );
		if ( lpBlock->pszName ) ::ArrayFreeMem( m_pParser->lpaBlocks, lpBlock->pszName );

		// Remove the block from the array.
		::ArrayRemoveAt( m_pParser->lpaBlocks, nPos, 1 );

		// Data has shifted in the array. Re-apply it's
		// contents into the list.
		ResetContent();
		for ( int i = 0; i < ArrayGetSize( m_pParser->lpaBlocks ); i++ )
			AddString(( LPCTSTR )::ArrayGetAt( m_pParser->lpaBlocks, i ));

		// Setup selection.
		SetCurSel( nSel );
	}
	// Changes made.
	pSettings->Changed( m_pParser );
}

// Measure an item.
LRESULT ColorList::OnReflectedMeasureItem( UINT nID, LPMEASUREITEMSTRUCT pMis, BOOL& bNotifyParent )
{
	UNREFERENCED_PARAMETER( nID );

	// No need to bother the parent.
	bNotifyParent = FALSE;

	// Get a device context.
	ClsGetDC dc( this );

	// Select the font.
	ClsFont font;
	GetFont( font );
	ClsSelector sel( &dc, font );

	// Compute the height.
	pMis->itemHeight = dc.GetTextExtent( _T( " " ), 1 ).CY();
	return TRUE;
}

// Draw an item.
LRESULT ColorList::OnReflectedDrawItem( UINT nID, LPDRAWITEMSTRUCT pDis, BOOL& bNotifyParent )
{
	UNREFERENCED_PARAMETER( nID );

	// No need to bother the parent.
	bNotifyParent = FALSE;

	// Smart indention list?
	if ( m_pSmart )
	{
		// Valid data?
		if ( pDis->itemData )
		{
			// Show the text.
			DrawColorEntry( pDis, 0xFFFFFFFF, (( LPSMART )pDis->itemData )->lpszSmartString );
			return TRUE;
		}
	}
	// Keyword group list?
	else if ( m_pKeys )
	{
		// Valid data?
		if ( pDis->itemData )
		{
			// Create name.
			ClsString str;
			str.Format( ClsString( MAKEINTRESOURCE( IDS_COLOR_FMT )), GetRValue((( LPKEYWORDS )pDis->itemData )->crColor ),
										  GetGValue((( LPKEYWORDS )pDis->itemData )->crColor ),
										  GetBValue((( LPKEYWORDS )pDis->itemData )->crColor ));
			// Render entry.
			DrawColorEntry( pDis, (( LPKEYWORDS )pDis->itemData )->crColor, str );
			return TRUE;
		}
	}
	// Are we a block list?
	else if ( m_bBlockList )
	{
		// Valid data?
		if ( pDis->itemData )
		{
			// Render entry.
			DrawColorEntry( pDis, (( LPBLOCK )pDis->itemData )->crColor, (( LPBLOCK )pDis->itemData )->pszName ? (( LPBLOCK )pDis->itemData )->pszName : ClsString( MAKEINTRESOURCE( IDS_BLOCK_NONAME )));
			return TRUE;
		}
	}
	else
	{
		// Valid data?
		if ( pDis->itemData )
		{
			// Render entry.
			ClsString name( MAKEINTRESOURCE( m_nColors[ pDis->itemID ] ));
			if ( m_pParser->dwColorFlags & m_dwFlags[ pDis->itemID ] ) name += _T( '*' );
			DrawColorEntry( pDis, m_pParser->dwColorFlags & m_dwFlags[ pDis->itemID ] ? m_crDefaults[ pDis->itemID ] : m_pParser->crColors[ pDis->itemID ], name );
			return TRUE;
		}
	}
	return FALSE;
}

// Render a text item.
void ColorList::DrawTextItem( LPDRAWITEMSTRUCT pDis, LPCTSTR pszName )
{
	// Wrap the DC and snapshot it.
	ClsDC dc( pDis->hDC );
	int nSDC = dc.SaveDC();

	// Determine colors.
	HBRUSH hBrush;
	int nCol;

	// Disabled?
	if ( pDis->itemState & ODS_DISABLED )
	{
		hBrush = ( HBRUSH )( COLOR_WINDOW + 1 );
		nCol   = COLOR_GRAYTEXT;
	}
	else
	{
		// Selected or not?
		if ( pDis->itemState & ODS_SELECTED )
		{
			hBrush = ( HBRUSH )( COLOR_HIGHLIGHT + 1 );
			nCol   = COLOR_HIGHLIGHTTEXT;
		}
		else
		{
			hBrush = ( HBRUSH )( COLOR_WINDOW + 1 );
			nCol   = COLOR_WINDOWTEXT;
		}
	}

        // Do the background.
	ClsRect rc( pDis->rcItem );
	rc.Left() += 2;
        dc.FillRect( rc, hBrush );

        // Do we have the focus?
        if ( pDis->itemState & ODS_FOCUS )
                dc.DrawFocusRect( rc );

        // Get text size.
        ClsSize sz = dc.GetTextExtent( _T( " " ), 1 );

        // Vertical center.
        rc.Top() += (( rc.Bottom() - rc.Top()) - sz.CY()) / 2;
	rc.Left()++;

        // and the text..
        dc.SetBkMode( TRANSPARENT );
        dc.SetTextColor( ::GetSysColor( nCol ));
        dc.DrawText( pszName,
                     -1,
                     rc,
                     DT_LEFT );

        // Restore and detach the device context.
        dc.RestoreDC( nSDC );
	dc.Detach();
}

// Render an entry from the list.
void ColorList::DrawColorEntry( LPDRAWITEMSTRUCT pDis, COLORREF crColor, LPCTSTR pszName )
{
	// Wrap the DC.
	ClsDC	dc( pDis->hDC );

	// Snapshot...
	int sDC = dc.SaveDC();

	// Fill the background.
	ClsBrush brush( crColor == CLR_DEFAULT ? ::GetSysColor( COLOR_WINDOW ) : crColor );
	ClsRect rc( pDis->rcItem );

	// Any color rectangle?
	if ( crColor != 0xFFFFFFFF )
	{
		// Make color rectable.
		rc.Left() += 2;
		rc.Top()++;
		rc.Right() = rc.Left() + rc.Height();
		rc.Bottom()--;

		// Create pens.
		ClsPen p1( PS_SOLID, 0, RGB( 0, 0, 0 ));
		ClsPen p2( PS_SOLID, 0, RGB( 255, 255, 255 ));

		// Brace the code so that the ClsSelector objects
		// go out of context before the device context
		// does.
		{
			// Select brush and first pen.
			ClsSelector sb( &dc, brush );
			ClsSelector sp1( &dc, p1 );

			// Render first ellipse...
			dc.Ellipse( rc.Left(), rc.Top(), rc.Right(), rc.Bottom());

			// Deflate client by one pixel.
			rc.Deflate( 1, 1 );

			// Select second pen and render the other
			// ellipse.
			ClsSelector sp2( &dc, p2 );
			dc.Ellipse( rc.Left(), rc.Top(), rc.Right(), rc.Bottom());
		}

		// Adjust entry text area.
		pDis->rcItem.left += rc.Width() + 4;
	}

	// Restore snapshot.
	dc.RestoreDC( sDC );

	// Unwrap the DC.
	dc.Detach();

	// Render string.
	DrawTextItem( pDis, pszName );
}
