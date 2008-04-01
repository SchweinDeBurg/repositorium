#ifndef _BUTTON_H_
#define _BUTTON_H_
//
// button.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../window.h"
#include "../../coords/rect.h"

// A ClsWindow derived class handling button controls.
class ClsButton : public ClsWindow
{
	_NO_COPY( ClsButton );
public:
	// Default constructor. Does nothing.
	ClsButton()
	{;}

	// Constructor. Initializes to the passed
	// handle.
	ClsButton( HWND hWnd )
	{
		// Attach the handle so that it will not
		// get destroyed when the object is destroyed.
		Attach( hWnd );
	}

	// Destructor. Does nothing.
	virtual ~ClsButton()
	{;}

	// Create a button control.
	inline BOOL Create( ClsWindow *parent, const ClsRect& rBounds, UINT nID, LPCTSTR pszName, UINT nStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON )
	{
		_ASSERT( m_hWnd == NULL ); // May not exist already.

		// Create the control.
		return ClsWindow::Create( 0, _T( "BUTTON" ), pszName, nStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Simulate a click on the button.
	inline void Click()
	{
		_ASSERT_VALID( GetSafeHWND()); // Must be valid.

		// Send the BM_CLICK message.
		SendMessage( BM_CLICK );
	}

	// Get the button checked state.
	inline UINT GetCheck() const
	{
		_ASSERT_VALID( GetSafeHWND()); // Must be valid.

		// Return the checked state.
		return ( UINT )::SendMessage( m_hWnd, BM_GETCHECK, 0, 0 );
	}
	
	// Set the button checked state.
	inline void SetCheck( UINT nCheck )
	{
		_ASSERT_VALID( GetSafeHWND()); // Must be valid.

		// Set the checked state.
		SendMessage( BM_SETCHECK, nCheck );
	}

	// Get the button icon.
	inline HICON GetIcon() const
	{
		_ASSERT_VALID( GetSafeHWND()); // Must be valid.

		// Return the icon.
		return ( HICON )::SendMessage( m_hWnd, BM_GETIMAGE, IMAGE_ICON, 0 );
	}

	// Set the button icon.
	inline HICON SetIcon( HICON hIcon )
	{
		_ASSERT_VALID( GetSafeHWND()); // Must be valid.

		// Set the new icon, returning the old.
		return ( HICON )SendMessage( BM_SETIMAGE, IMAGE_ICON, ( LPARAM )hIcon );
	}

	// Get the button bitmap.
	inline HBITMAP GetBitmap() const
	{
		_ASSERT_VALID( GetSafeHWND()); // Must be valid.

		// Return the bitmap.
		return ( HBITMAP )::SendMessage( m_hWnd, BM_GETIMAGE, IMAGE_BITMAP, 0 );
	}

	// Set the button bitmap.
	inline HBITMAP SetBitmap( HBITMAP hBitmap )
	{
		_ASSERT_VALID( GetSafeHWND()); // Must be valid.

		// Set the new bitmap, returning the old.
		return ( HBITMAP )SendMessage( BM_SETIMAGE, IMAGE_BITMAP, ( LPARAM )hBitmap );
	}

	// Get the button state.
	inline UINT GetState() const
	{
		_ASSERT_VALID( GetSafeHWND()); // Must be valid.

		// Return the button state.
		return ( UINT )::SendMessage( m_hWnd, BM_GETSTATE, 0, 0 );
	}

	// Set the button state.
	inline void SetState( UINT nState )
	{
		_ASSERT_VALID( GetSafeHWND()); // Must be valid.

		// Set the new state.
		SendMessage( BM_SETSTATE, nState );
	}

	// Set the button style.
	inline void SetStyle( UINT nStyle )
	{
		_ASSERT_VALID( GetSafeHWND()); // Must be valid.

		// Set the new style.
		SendMessage( BM_SETSTYLE, nStyle );
	}

protected:
	// Reflected WM_COMMAND notification handlers.
	virtual LRESULT OnClicked(   BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnSetFocus(  BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnKillFocus( BOOL& bNotifyParent ) { return 0; }

	// Handle the reflected WM_COMMAND messages.
	virtual LRESULT OnReflectedCommand( UINT nNotifyCode, BOOL& bNotifyParent )
	{
		switch ( nNotifyCode )
		{
			case	BN_CLICKED:	
				return OnClicked( bNotifyParent );
			
			case	BN_SETFOCUS:	
				return OnSetFocus( bNotifyParent );
			
			case	BN_KILLFOCUS:	
				return OnKillFocus( bNotifyParent );
		}
		return 1;
	}
};

#endif // _BUTTON_H_
