#ifndef _COMBOBOX_H_
#define _COMBOBOX_H_
//
// combobox.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../window.h"
#include "../../coords/rect.h"

// A ClsWindow derived class handling combobox controls.
class ClsComboBox : public ClsWindow
{
	_NO_COPY( ClsComboBox );
public:
	// Default constructor. Does nothing.
	ClsComboBox()
	{;}

	// Constructor. Initializes to the passed
	// handle.
	ClsComboBox( HWND hWnd )
	{
		// Attach the handle so that it will not
		// get destroyed when the object is destroyed.
		Attach( hWnd );
	}

	// Destructor. Does nothing.
	virtual ~ClsComboBox()
	{;}

	// Create a combobox control.
	inline BOOL Create( ClsWindow *parent, const ClsRect& rBounds, UINT nID, LPCTSTR pszName, UINT nStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE | CBS_AUTOHSCROLL | CBS_DROPDOWN | CBS_NOINTEGRALHEIGHT )
	{
		_ASSERT( m_hWnd == NULL ); // May not exist already.

		// Create the control.
		return ClsWindow::Create( 0, _T( "COMBOBOX" ), pszName, nStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Add a string to the combobox.
	inline int AddString( LPCTSTR pszString )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Add the string.
		return ( int )SendMessage( CB_ADDSTRING, 0, ( LPARAM )pszString );
	}

	// Delete a string from the combobox.
	inline int DeleteString( int nIndex )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Delete the string.
		return ( int )SendMessage( CB_DELETESTRING, nIndex );
	}

	// Display files in the combobox.
	inline int Dir( UINT nAttrs, LPCTSTR pszFileSpec )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Fill the combobox.
		return ( int )SendMessage( CB_DIR, nAttrs, ( LPARAM )pszFileSpec );
	}

	// Find a string.
	inline int FindString( int nStartIndex, LPCTSTR pszString ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Find the string.
		return ( int )::SendMessage( m_hWnd, CB_FINDSTRING, nStartIndex, ( LPARAM )pszString );
	}

	// Find a string exact.
	inline int FindStringExact( int nStartIndex, LPCTSTR pszString ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Find the string.
		return ( int )::SendMessage( m_hWnd, CB_FINDSTRINGEXACT, nStartIndex, ( LPARAM )pszString );
	}

	// Get the number of entries.
	inline int GetCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the count.
		return ( int )::SendMessage( m_hWnd, CB_GETCOUNT, 0, 0 );
	}

	// Get the current selection.
	inline int GetCurSel() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the selection index.
		return ( int )::SendMessage( m_hWnd, CB_GETCURSEL, 0, 0 );
	}

	// Get the bounds of the dropped list.
	inline BOOL GetDroppedControlRect( LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the bounds.
		return ( BOOL )::SendMessage( m_hWnd, CB_GETDROPPEDCONTROLRECT, 0, ( LPARAM )pRect );
	}

	// Is the list dropped?
	inline BOOL GetDroppedState() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the state.
		return ( BOOL )::SendMessage( m_hWnd, CB_GETDROPPEDSTATE, 0, 0 );
	}

	// Get minimum combobox width.
	inline int GetDroppedWidth() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the width.
		return ( int )::SendMessage( m_hWnd, CB_GETDROPPEDWIDTH, 0, 0 );
	}

	// Get the selection of the edit control.
	inline DWORD GetEditSel( LPDWORD pdwStart, LPDWORD pdwEnd ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the range.
		return ( DWORD )::SendMessage( m_hWnd, CB_GETEDITSEL, ( WPARAM )pdwStart, ( LPARAM )pdwEnd );
	}

	// Extended or normal UI?
	inline BOOL GetExtendedUI() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the UI state.
		return ( BOOL )::SendMessage( m_hWnd, CB_GETEXTENDEDUI, 0, 0 );
	}

	// Get the scrollable width.
	inline int GetHorizontalExtent() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the extent.
		return ( int )::SendMessage( m_hWnd, CB_GETHORIZONTALEXTENT, 0, 0 );
	}

	// Get the item data.
	inline DWORD GetItemData( int nIndex ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the data.
		return ( DWORD )::SendMessage( m_hWnd, CB_GETITEMDATA, nIndex, 0 );
	}

	// Get the item height.
	inline int GetItemHeight( int nIndex ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the height.
		return ( int )::SendMessage( m_hWnd, CB_GETITEMHEIGHT, nIndex, 0 );
	}

	// Get the text of an item.
	inline int GetLBText( int nIndex, LPTSTR pszBuffer ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the text.
		return ( int )::SendMessage( m_hWnd, CB_GETLBTEXT, nIndex, ( LPARAM )pszBuffer );
	}

	// Get the text of an item.
	inline int GetLBText( int nIndex, ClsString& str ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the text.
		return ( int )::SendMessage( m_hWnd, CB_GETLBTEXT, nIndex, ( LPARAM )str.GetBuffer( GetLBTextLen( nIndex )));
	}

	// Get the length of the text of an item.
	inline int GetLBTextLen( int nIndex ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the length.
		return ( int )::SendMessage( m_hWnd, CB_GETLBTEXTLEN, nIndex, 0 );
	}

	// Get the locale.
	inline WORD GetLocale() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the locale.
		return ( WORD )::SendMessage( m_hWnd, CB_GETLOCALE, 0, 0 );
	}

	// Get the index of the first visible item.
	inline int GetTopIndex() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the index.
		return ( int )::SendMessage( m_hWnd, CB_GETTOPINDEX, 0, 0 );
	}

	// Insert a string.
	inline int InsertString( int nIndex, LPCTSTR pszString )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Insert the string.
		return ( int )SendMessage( CB_INSERTSTRING, nIndex, ( LPARAM )pszString );
	}

	// Limit the typable text.
	inline void LimitText( int nChars )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Limit the text.
		SendMessage( CB_LIMITTEXT, nChars );
	}

	// Clear the list.
	inline void ResetContent()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Clear the content.
		SendMessage( CB_RESETCONTENT );
	}

	// Select the string.
	inline int SelectString( int nStartIndex, LPCTSTR pszSelect )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Select the string.
		return ( int )SendMessage( CB_SELECTSTRING, nStartIndex, ( LPARAM )pszSelect );
	}

	// Set the current selection.
	inline int SetCurSel( int nIndex )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the selection.
		return ( int )SendMessage( CB_SETCURSEL, nIndex );
	}
	
	// Set the maximum allowable width of the list box.
	inline int SetDroppedWidth( int nWidth )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the width.
		return ( int )SendMessage( CB_SETDROPPEDWIDTH, nWidth );
	}

	// Set the selection in the edit control.
	inline int SetEditSel( int nStart, int nEnd )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the selection.
		return ( int )SendMessage( CB_SETEDITSEL, 0, MAKELPARAM( nStart, nEnd ));
	}

	// Set the extended UI.
	inline int SetExtendedUI( BOOL bExtended )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the value.
		return ( int )SendMessage( CB_SETEXTENDEDUI, bExtended );
	}

	// Set the horizontal extent.
	inline void SetHorizontalExtent( int nExtent )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the extent.
		SendMessage( CB_SETHORIZONTALEXTENT, nExtent );
	}

	// Set the item data.
	inline int SetItemData( int nIndex, DWORD dwData )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the data.
		return ( int )SendMessage( CB_SETITEMDATA, nIndex, dwData );
	}

	// Set the item height.
	inline int SetItemHeight( int nIndex, int nHeight )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the item height.
		return ( int )SendMessage( CB_SETITEMHEIGHT, nIndex, nHeight );
	}

	// Set the locale.
	inline WORD SetLocale( WORD wLocale )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the locale.
		return ( WORD )SendMessage( CB_SETLOCALE, wLocale );
	}

	// Set the index of the first visible item.
	inline int SetTopIndex( int nIndex )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the index.
		return ( int )SendMessage( CB_SETTOPINDEX, nIndex );
	}

	// Show or hide the dropdown list.
	inline void ShowDropDown( BOOL bShow )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Show or hide the dropdown list.
		SendMessage( CB_SHOWDROPDOWN, bShow );
	}

protected:
	// Reflected WM_COMMAND notification handlers.
	virtual LRESULT OnCloseUp(      BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnDblclk(	BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnDropDown(     BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnEditChange(   BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnEditUpdate(   BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnErrSpace(     BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnKillFocus(    BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnSelChange(    BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnSelEndCancel( BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnSelEndOK(     BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnSetFocus(     BOOL& bNotifyParent ) { return 0; }

	// Handle the reflected WM_COMMAND messages.
	virtual LRESULT OnReflectedCommand( UINT nNotifyCode, BOOL& bNotifyParent )
	{
		switch ( nNotifyCode )
		{
			case	CBN_CLOSEUP:		
				return OnCloseUp( bNotifyParent );
			
			case	CBN_DBLCLK:		
				return OnDblclk( bNotifyParent );
			
			case	CBN_DROPDOWN:		
				return OnDropDown( bNotifyParent );
			
			case	CBN_EDITCHANGE:	
				return OnEditChange( bNotifyParent );

			case	CBN_EDITUPDATE:
				return OnEditUpdate( bNotifyParent );

			case	CBN_ERRSPACE:
				return OnErrSpace( bNotifyParent );

			case	CBN_KILLFOCUS:
				return OnKillFocus( bNotifyParent );

			case	CBN_SELCHANGE:
				return OnSelChange( bNotifyParent );

			case	CBN_SELENDCANCEL:
				return OnSelEndCancel( bNotifyParent );

			case	CBN_SELENDOK:
				return OnSelEndOK( bNotifyParent );

			case	CBN_SETFOCUS:
				return OnSetFocus( bNotifyParent );
		}
		return 1;
	}
};

#endif // _COMBOBOX_H_