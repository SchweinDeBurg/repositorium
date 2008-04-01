#ifndef _SIZEDIALOG_H_
#define _SIZEDIALOG_H_
//
// sizedialog.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../windows/dialog.h"
#include "../gdi/all.h"
#include "../tools/theming.h"

// A dialog class which handles a size-grip at
// it's bottom right corner.
class ClsSizeDialog : public ClsDialog
{
	_NO_COPY( ClsSizeDialog );
public:
	// Construction.
	ClsSizeDialog() : m_hTheme( NULL )
	{;}

	// Destruction.
	virtual ~ClsSizeDialog()
	{ 
		// Destroy theme.
		if ( m_hTheme ) ThemingAPI.CloseThemeData( m_hTheme );
	}

protected:
	// Theme...
	HANDLE	m_hTheme;

	// WM_PAINT override. Render the size-grip.
	virtual LRESULT OnPaint( ClsDC *pDC )
	{
		// Are we zoomed?
		if ( ! IsZoomed())
		{
			// Theming?
			if ( ThemingAPI.IsThemingEnabled() && ! m_hTheme )
				m_hTheme = ThemingAPI.OpenThemeData( GetSafeHWND(), L"SCROLLBAR" );

			// Get the paint DC.
			ClsPaintDC dc( this );

			// Get the client rectangle.
			ClsRect rc = GetClientRect();

			// Adjust to fit the gripper.
			rc.Top() = rc.Bottom() - ::GetSystemMetrics( SM_CYHSCROLL );
			rc.Left() = rc.Right() - ::GetSystemMetrics( SM_CXVSCROLL );

			// Render the gripper.
			if ( ! m_hTheme ) dc.DrawFrameControl( rc, DFC_SCROLL, DFCS_SCROLLSIZEGRIP );
			else		  ThemingAPI.DrawThemeBackground( m_hTheme, dc, SBP_SIZEBOX, SZB_RIGHTALIGN, rc, NULL );
		}
		// Call the base class.
		return ClsDialog::OnPaint( pDC );
	}

	// Handle the WM_NCHITTEST message.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		// What's up?
		switch ( uMsg )
		{
			case	WM_THEMECHANGED:
				// Theming API available?
				if ( ThemingAPI.IsThemingEnabled())
				{
					// Destroy old theme ang load the new one.
					if ( m_hTheme ) ThemingAPI.CloseThemeData( m_hTheme );
					m_hTheme = ThemingAPI.OpenThemeData( GetSafeHWND(), L"SCROLLBAR" );
				}
				break;

			case	WM_NCHITTEST:
				{
					// Get the grip rectangle.
					ClsRect rc = GetWindowRect();
					
					// Adjust the rectangle to only include
					// the area of the sizing grip.
					rc.Left() = rc.Right() - ::GetSystemMetrics( SM_CXVSCROLL );
					rc.Top() = rc.Bottom() - ::GetSystemMetrics( SM_CYHSCROLL );

					// Is the cursor inside the size grip?
					if ( rc.PtInRect( ClsPoint( LOWORD( lParam ), HIWORD( lParam ))))
					{
						// Set the result to HTBOTTOMRIGHT.
						SetWindowLong64( DWL_MSGRESULT, ( LONG_PTR )HTBOTTOMRIGHT );
						return TRUE;
					}
				}
				break;
		}
		// Call the base class.
		return ClsDialog::WindowProc( uMsg, wParam, lParam );
	}
};

#endif // _SIZEDIALOG_H_