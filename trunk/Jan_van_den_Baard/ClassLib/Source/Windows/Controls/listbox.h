#ifndef _LISTBOX_H_
#define _LISTBOX_H_
//
// listbox.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../window.h"
#include "../../coords/rect.h"

// A ClsWindow derived class handling listbox controls.
class ClsListBox : public ClsWindow
{
	_NO_COPY( ClsListBox );
public:
	// Default constructor. Does nothing.
	ClsListBox()
	{;}

	// Constructor. Initializes to the passed
	// handle.
	ClsListBox( HWND hWnd )
	{
		// Attach the handle so that it will not
		// get destroyed when the object is destroyed.
		Attach( hWnd );
	}

	// Destructor. Does nothing.
	virtual ~ClsListBox()
	{;}

	// Create a combobox control.
	inline BOOL Create( ClsWindow *parent, const ClsRect& rBounds, UINT nID, LPCTSTR pszName, UINT nStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE | LBS_STANDARD )
	{
		_ASSERT( m_hWnd == NULL ); // May not exist already.

		// Create the control.
		return ClsWindow::Create( 0, _T( "LISTBOX" ), pszName, nStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Add a file to the listbox.
	inline int AddFile( LPCTSTR pszFilename )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )SendMessage( LB_ADDFILE, 0, ( LPARAM )pszFilename );
	}

	// Add a string to the listbox.
	inline int AddString( LPCTSTR pszString )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Add the string.
		return ( int )SendMessage( LB_ADDSTRING, 0, ( LPARAM )pszString );
	}

	// Delete a string from the combobox.
	inline int DeleteString( int nIndex )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Delete the string.
		return ( int )SendMessage( LB_DELETESTRING, nIndex );
	}

	// Display files in the combobox.
	inline int Dir( UINT nAttrs, LPCTSTR pszFileSpec )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Fill the combobox.
		return ( int )SendMessage( LB_DIR, nAttrs, ( LPARAM )pszFileSpec );
	}

	// Find a string.
	inline int FindString( int nStartIndex, LPCTSTR pszString ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Find the string.
		return ( int )::SendMessage( m_hWnd, LB_FINDSTRING, nStartIndex, ( LPARAM )pszString );
	}

	// Find a string exact.
	inline int FindStringExact( int nStartIndex, LPCTSTR pszString ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Find the string.
		return ( int )::SendMessage( m_hWnd, LB_FINDSTRINGEXACT, nStartIndex, ( LPARAM )pszString );
	}

	// Get the anchor index.
	inline int GetAnchorIndex() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the index.
		return ( int )::SendMessage( m_hWnd, LB_GETANCHORINDEX, 0, 0 );
	}
	
	// Get the caret index.
	inline int GetCaretIndex() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the index.
		return ( int )::SendMessage( m_hWnd, LB_GETCARETINDEX, 0, 0 );
	}

	// Get the number of entries.
	inline int GetCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the count.
		return ( int )::SendMessage( m_hWnd, LB_GETCOUNT, 0, 0 );
	}

	// Get the current selection.
	inline int GetCurSel() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the selection index.
		return ( int )::SendMessage( m_hWnd, LB_GETCURSEL, 0, 0 );
	}

	// Get horizontal extent.
	inline int GetHorizontalExtent() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the extent.
		return ( int )::SendMessage( m_hWnd, LB_GETHORIZONTALEXTENT, 0, 0 );
	}

	// Get the item data.
	inline DWORD GetItemData( int nIndex ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the data.
		return ( DWORD )::SendMessage( m_hWnd, LB_GETITEMDATA, nIndex, 0 );
	}

	// Get the item height.
	inline int GetItemHeight( int nIndex ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the height.
		return ( int )::SendMessage( m_hWnd, LB_GETITEMHEIGHT, nIndex, 0 );
	}

	// Get the item rectangle.
	inline int GetItemRect( int nIndex, LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the rectangle.
		return ( int )::SendMessage( m_hWnd, LB_GETITEMRECT, nIndex, ( LPARAM )pRect );
	}

	// Get the locale.
	inline WORD GetLocale() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the locale.
		return ( WORD )::SendMessage( m_hWnd, LB_GETLOCALE, 0, 0 );
	}

	// Get selection state.
	inline int GetSel( int nIndex ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the selection state.
		return ( int )::SendMessage( m_hWnd, LB_GETSEL, nIndex, 0 );
	}

	// Get the number of selected items.
	inline int GetSelCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the number of selected items.
		return ( int )::SendMessage( m_hWnd, LB_GETSELCOUNT, 0, 0 );
	}

	// Get the selected items.
	inline int GetSelItems( int nCount, LPINT pItems ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the selected items.
		return ( int )::SendMessage( m_hWnd, LB_GETSELITEMS, nCount, ( LPARAM )pItems );
	}

	// Get the text of an item.
	inline int GetText( int nIndex, LPTSTR pszBuffer ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the text.
		return ( int )::SendMessage( m_hWnd, LB_GETTEXT, nIndex, ( LPARAM )pszBuffer );
	}

	// Get the length of the text of an item.
	inline int GetTextLen( int nIndex ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the length.
		return ( int )::SendMessage( m_hWnd, LB_GETTEXTLEN, nIndex, 0 );
	}

	// Get the index of the first visible item.
	inline int GetTopIndex() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the index.
		return ( int )::SendMessage( m_hWnd, LB_GETTOPINDEX, 0, 0 );
	}

	// Insert a string.
	inline int InsertString( int nIndex, LPCTSTR pszString )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Insert the string.
		return ( int )SendMessage( LB_INSERTSTRING, nIndex, ( LPARAM )pszString );
	}

	// Get the item at the given coordinates.
	inline int ItemFromPoint( int xPos, int yPos ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the item.
		return ( int )::SendMessage( m_hWnd, LB_ITEMFROMPOINT, 0, MAKELPARAM( xPos, yPos ));
	}

	// Clear the list.
	inline void ResetContent()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Clear the content.
		SendMessage( LB_RESETCONTENT );
	}

	// Select the string.
	inline int SelectString( int nStartIndex, LPCTSTR pszSelect )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Select the string.
		return ( int )SendMessage( LB_SELECTSTRING, nStartIndex, ( LPARAM )pszSelect );
	}

	// Select a range of items.
	inline int SelItemRange( int nStart, int nEnd, BOOL bSelect = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Select the range.
		return ( int )SendMessage( LB_SELITEMRANGE, bSelect, MAKELPARAM( nStart, nEnd ));
	}

	// Select a range of items.
	inline int SelItemRangeEx( int nStart, int nEnd )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Select the range.
		return ( int )SendMessage( LB_SELITEMRANGEEX, nStart, nEnd );
	}

	// Set anchor index.
	inline int SetAnchorIndex( int nIndex )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the index.
		return ( int )SendMessage( LB_SETANCHORINDEX,nIndex, 0 );
	}

	// Set caret index.
	inline int SetCaretIndex( int nIndex, BOOL bScroll = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the index.
		return ( int )SendMessage( LB_SETCARETINDEX,nIndex, MAKELPARAM( bScroll, 0 ));
	}

	// Set column width.
	inline void SetColumnWidth( int cxWidth )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the width of all columns.
		SendMessage( LB_SETCOLUMNWIDTH, cxWidth );
	}

	// Set number of items with LBS_NODATA.
	inline void SetCount( int nCount )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the width of all columns.
		SendMessage( LB_SETCOUNT, nCount );
	}

	// Set the current selection.
	inline int SetCurSel( int nIndex )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the selection.
		return ( int )SendMessage( LB_SETCURSEL, nIndex );
	}

	// Set the horizontal extent.
	inline void SetHorizontalExtent( int nExtent )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the extent.
		SendMessage( LB_SETHORIZONTALEXTENT, nExtent );
	}

	// Set the item data.
	inline int SetItemData( int nIndex, DWORD dwData )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the data.
		return ( int )SendMessage( LB_SETITEMDATA, nIndex, dwData );
	}

	// Set the item height.
	inline int SetItemHeight( int nIndex, int nHeight )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the item height.
		return ( int )SendMessage( LB_SETITEMHEIGHT, nIndex, nHeight );
	}

	// Set the locale.
	inline WORD SetLocale( WORD wLocale )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the locale.
		return ( WORD )SendMessage( LB_SETLOCALE, wLocale );
	}

	// Set item selection.
	inline int SetSel( BOOL bSelect, int nIndex )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the selection state.
		return ( int )SendMessage( LB_SETSEL, bSelect, nIndex );
	}

	// Set tab-stops.
	inline BOOL SetTabStops( int nTabs, LPINT pTabs )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the tab stops.
		return ( BOOL )SendMessage( LB_SETTABSTOPS, nTabs, ( LPARAM )pTabs );
	}

	// Set the index of the first visible item.
	inline int SetTopIndex( int nIndex )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the index.
		return ( int )SendMessage( LB_SETTOPINDEX, nIndex );
	}

protected:
	// Reflected WM_COMMAND notification handlers.
	virtual LRESULT OnDblclk(    BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnErrSpace(  BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnKillFocus( BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnSelCancel( BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnSelChange( BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnSetFocus(  BOOL& bNotifyParent ) { return 0; }

	// Handle the reflected WM_COMMAND messages.
	virtual LRESULT OnReflectedCommand( UINT nNotifyCode, BOOL& bNotifyParent )
	{
		switch ( nNotifyCode )
		{
			case	LBN_DBLCLK:
				return OnDblclk( bNotifyParent );

			case	LBN_ERRSPACE:
				return OnErrSpace( bNotifyParent );

			case	LBN_KILLFOCUS:
				return OnKillFocus( bNotifyParent );

			case	LBN_SELCANCEL:
				return OnSelCancel( bNotifyParent );

			case	LBN_SELCHANGE:
				return OnSelChange( bNotifyParent );

			case	LBN_SETFOCUS:
				return OnSetFocus( bNotifyParent );
		}
		return 1;
	}
};

#endif // _LISTBOX_H_