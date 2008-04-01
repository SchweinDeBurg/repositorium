#ifndef _STATIC_H_
#define _STATIC_H_
//
// static.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../window.h"
#include "../../coords/rect.h"

// A ClsWindow derived class handling static controls.
class ClsStatic : public ClsWindow
{
	_NO_COPY( ClsStatic );
public:
	// Constructor. Does nothing.
	ClsStatic()
	{;}

	// Constructor. Initializes to the handle.
	ClsStatic( HWND hWnd )
	{
		// Attach the handle so that it will
		// not be destroyed when the object is destroyed.
		Attach( hWnd );
	}

	// Destructor. Does nothing.
	virtual ~ClsStatic()
	{;}

	// Interface.
	inline BOOL Create( ClsWindow *parent, const ClsRect& rBounds, UINT nID, LPCTSTR pszName, UINT nStyle = WS_CHILD | WS_VISIBLE | SS_LEFT )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsWindow::Create( 0, _T( "STATIC" ), pszName, nStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Set a new bitmap.
	inline HBITMAP SetBitmap( HBITMAP hBitmap )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the new bitmap.
		return ( HBITMAP )SendMessage( STM_SETIMAGE, IMAGE_BITMAP, ( LPARAM )hBitmap );
	}

	// Get the bitmap.
	inline HBITMAP GetBitmap() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the bitmap.
		return ( HBITMAP )::SendMessage( m_hWnd, STM_GETIMAGE, IMAGE_BITMAP, 0 );
	}

	// Set a new icon.
	inline HICON SetIcon( HICON hIcon )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the new icon.
		return ( HICON )SendMessage( STM_SETIMAGE, IMAGE_ICON, ( LPARAM )hIcon );
	}

	// Get the icon.
	inline HICON GetIcon() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the icon.
		return ( HICON )::SendMessage( m_hWnd, STM_GETIMAGE, IMAGE_ICON, 0 );
	}

	// Set a new cursor.
	inline HCURSOR SetCursor( HCURSOR hCursor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the new cursor.
		return ( HCURSOR )SendMessage( STM_SETIMAGE, IMAGE_CURSOR, ( LPARAM )hCursor );
	}

	// Get the cursor.
	inline HCURSOR GetCursor() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the cursor.
		return ( HCURSOR )::SendMessage( m_hWnd, STM_GETIMAGE, IMAGE_CURSOR, 0 );
	}

	// Set a new enhanced metafile.
	inline HENHMETAFILE SetEnhMetaFile( HENHMETAFILE hEnhMetaFile )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the new enhanced metafile.
		return ( HENHMETAFILE )SendMessage( STM_SETIMAGE, IMAGE_ENHMETAFILE, ( LPARAM )hEnhMetaFile );
	}

	// Get the enhanced metafile.
	inline HENHMETAFILE GetEnhMetaFile() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// return the enhanced metafile.
		return ( HENHMETAFILE )::SendMessage( m_hWnd, STM_GETIMAGE, IMAGE_ENHMETAFILE, 0 );
	}

protected:
	// Reflected WM_COMMAND notification handlers.
	virtual LRESULT OnClicked( BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnDblclk(  BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnDisable( BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnEnable(  BOOL& bNotifyParent ) { return 0; }

	// Handle the reflected WM_COMMAND messages.
	virtual LRESULT OnReflectedCommand( UINT nNotifyCode, BOOL& bNotifyParent )
	{
		switch ( nNotifyCode )
		{
			case	STN_CLICKED:
				return OnClicked( bNotifyParent );

			case	STN_DBLCLK:
				return OnDblclk( bNotifyParent );

			case	STN_DISABLE:
				return OnDisable( bNotifyParent );

			case	STN_ENABLE:
				return OnEnable( bNotifyParent );
		}
		return 1;
	}
};

#endif // _STATIC_H_