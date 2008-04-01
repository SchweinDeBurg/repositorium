#ifndef _STATUSBAR_H_
#define _STATUSBAR_H_
//
// statusbar.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"
#include "../../strings/string.h"

// A wrapper for the Statusbar common control.
class ClsStatusbar : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsStatusbar, ClsCommonControl );
public:
	// Constructor.
	ClsStatusbar() : ClsCommonControl( ICC_WIN95_CLASSES )
	{;}

	// Destructor.
	virtual ~ClsStatusbar()
	{;}

	// Create a statusbar control.
	inline BOOL Create( ClsWindow *parent, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsCommonControl::Create( 0, STATUSCLASSNAME, NULL, dwStyle, ClsRect( 0, 0, 0, 0 ), parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Get borders.
	inline BOOL GetBorders( LPINT pBorders ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, SB_GETBORDERS, 0, ( LPARAM )pBorders );
	}

	// Get icon.
	inline HICON GetIcon( int iPart ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HICON )::SendMessage( m_hWnd, SB_GETICON, iPart, 0 );
	}

	// Get parts.
	inline int GetParts( int nParts, LPINT pPartsArray ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, SB_GETPARTS, nParts, ( LPARAM )pPartsArray );
	}

	// Get part bounds.
	inline BOOL GetRect( int iPart, LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, SB_GETRECT, iPart, ( LPARAM )pRect );
	}

	// Get part text.
	inline DWORD GetText( int iPart, LPSTR pszText ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( DWORD )::SendMessage( m_hWnd, SB_GETTEXT, iPart, ( LPARAM )pszText );
	}

	// Get part text length.
	inline UINT GetTextLength( int iPart ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( UINT )::SendMessage( m_hWnd, SB_GETTEXTLENGTH, iPart, 0 );
	}

	// Get the tooltip text.
	inline void GetTipText( int iPart, int nSize, LPTSTR pszTipText ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		::SendMessage( m_hWnd, SB_GETTIPTEXT, MAKEWPARAM( iPart, nSize ), ( LPARAM )pszTipText );
	}

	// Get unicode flag.
	inline BOOL GetUnicodeFormat() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, SB_GETUNICODEFORMAT, 0, 0 );
	}

	// Get simple flag.
	inline BOOL IsSimple() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, SB_ISSIMPLE, 0, 0 );
	}

	// Set the background color.
	inline COLORREF SetBkColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( COLORREF )SendMessage( SB_SETBKCOLOR, 0, crColor );
	}

	// Set the icon.
	inline BOOL SetIcon( int iPart, HICON hIcon )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( SB_SETICON, iPart, ( LPARAM )hIcon );
	}

	// Set the minimum height.
	inline void SetMinHeight( int nHeight )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( SB_SETMINHEIGHT, nHeight );
	}

	// Set the parts.
	inline BOOL SetParts( int nParts, LPINT pPartArray )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( SB_SETPARTS, nParts, ( LPARAM )pPartArray );
	}

	// Set part text.
	inline BOOL SetText( int iPart, UINT uType, LPSTR pszText )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( SB_SETTEXT, iPart | uType, ( LPARAM )pszText );
	}

	// Set the tip text.
	inline void SetTipText( int iPart, LPCTSTR pszTipText )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( SB_SETTIPTEXT, iPart, ( LPARAM )pszTipText );
	}

	// Set the unicode flag.
	inline BOOL SetUnicodeFlag( BOOL bUnicode )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( SB_SETUNICODEFORMAT, bUnicode );
	}

	// Set simple flag.
	inline BOOL Simple( BOOL bSimple )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( SB_SIMPLE, bSimple );
	}
protected:
	// Overidables.
	virtual LRESULT OnSimpleModeChange( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	// Reflected notification handler which will call
	// the virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// Evaluate notification code.
		if ( pNMHDR->code == SBN_SIMPLEMODECHANGE )
			return OnSimpleModeChange( pNMHDR, bNotifyParent );

		// Call the base class.
		return ClsCommonControl::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _STATUSBAR_H_