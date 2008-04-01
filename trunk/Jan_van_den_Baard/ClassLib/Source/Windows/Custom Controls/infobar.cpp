//
// infobar.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "infobar.h"
#include "../../gdi/getdc.h"
#include "../../gdi/paintdc.h"
#include "../../gdi/bufferdc.h"
#include "../../gdi/selector.h"
#include "../../gdi/brush.h"
#include "../../gdi/selector.h"
#include "../../coords/rect.h"
#include "../../tools/module.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor. Setup the defaults.
ClsInfoBar::ClsInfoBar()
{
	// Setup default values.
	m_hIcon			= NULL;
	m_crTitleColor		= ::GetSysColor( COLOR_CAPTIONTEXT );
	m_crBackgroundColor	= ::GetSysColor( COLOR_INACTIVECAPTION );
	m_crBackgroundGradient	= ::GetSysColor(
						 #ifndef COLOR_GRADIENTINACTIVECAPTION
						 28
						 #else
						 COLOR_GRADIENTINACTIVECAPTION
						 #endif
						 );
	m_bUseGradient		= FALSE;
	m_bIsVertical		= FALSE;
	m_bIsClipped		= FALSE;
	m_bCompactAsPath	= FALSE;
	m_bIconRight		= TRUE;
	m_hToolTip		= NULL;

	// By default this control uses the system caption
	// font.
	NONCLIENTMETRICS	ncm = { sizeof( NONCLIENTMETRICS ), 0 };
	if ( ::SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( ncm ), ( void * )&ncm, 0 ))
	{
		// Create the fonts.
		m_fNormal = CreateFontIndirect( &ncm.lfCaptionFont );
		ncm.lfCaptionFont.lfEscapement = ncm.lfCaptionFont.lfOrientation = 900;
		ncm.lfCaptionFont.lfOutPrecision = OUT_TT_ONLY_PRECIS;
		m_fVertical = CreateFontIndirect( &ncm.lfCaptionFont );
	}
}

// Destructor.
ClsInfoBar::~ClsInfoBar()
{
	// Free the icon and tooltip.
	if ( m_hIcon ) DestroyIcon( m_hIcon );
	if ( m_hToolTip ) DestroyWindow( m_hToolTip );
}

// Set the icon.
BOOL ClsInfoBar::SetIcon( LPCTSTR pszIconName, BOOL bRepaint /* = TRUE */ )
{
	// Load up the icon.
	HICON hIcon = ( HICON )::LoadImage( ClsGetResourceHandle(), pszIconName, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );

	// Did the icon load OK?
	if ( pszIconName && hIcon == NULL )
		return FALSE;

	// OK?
	// Destroy the previous icon.
	if ( m_hIcon ) ::DestroyIcon( m_hIcon );

	// Setup the new handle.
	m_hIcon = hIcon;

	// Re-render.
	if ( bRepaint && GetSafeHWND()) Invalidate();

	return TRUE;
}

// Set the icon.
BOOL ClsInfoBar::SetSystemIcon( LPCTSTR pszIconName, BOOL bRepaint /* = TRUE */ )
{
	// Load up the icon.
	HICON hIcon = ( HICON )::LoadImage( NULL, pszIconName, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );

	// OK?
	if ( hIcon )
	{
		// Destroy the previous icon.
		if ( m_hIcon ) ::DestroyIcon( m_hIcon );

		// Setup the new handle.
		m_hIcon = hIcon;

		// Re-render.
		if ( bRepaint && GetSafeHWND()) Invalidate();

		return TRUE;
	}
	return FALSE;
}

// Show or hide tooltips.
BOOL ClsInfoBar::TipClippedCaption( BOOL bClip /* = TRUE */ )
{
	_ASSERT_VALID( GetSafeHWND()); // Must be valid.

	// Must we show tool-tips?
	if ( bClip && m_hToolTip == NULL )
	{
		// Make sure we are notified!
		ModifyStyle( 0, SS_NOTIFY );

		// Create the tooltip.
		m_hToolTip = CreateWindowEx( WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP,
					     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWnd,
					     ( HMENU )NULL, ClsGetInstanceHandle(), NULL );

		// OK?
		if ( m_hToolTip == NULL ) return FALSE;

		// Initalize the TOOLINFO structure.
		TOOLINFO ti;
		ti.cbSize	= sizeof( TOOLINFO );
		ti.uFlags	= TTF_IDISHWND | TTF_SUBCLASS;
		ti.hwnd		= m_hWnd;
		ti.hinst	= ClsGetInstanceHandle();
		ti.uId		= ( UINT )( UINT_PTR )m_hWnd;
		ti.lpszText	= ( LPTSTR )LPSTR_TEXTCALLBACK;

		// Add the tooltip.
		if ( ! ::SendMessage( m_hToolTip, TTM_ADDTOOL, 0, ( LPARAM )&ti ))
		{
			::DestroyWindow( m_hToolTip );
			m_hToolTip = NULL;
			return FALSE;
		}
	}
	else if ( bClip == FALSE )
	{
		// Do we have a tooltip window.
		if ( m_hToolTip )
		{
			// Destroy it.
			::DestroyWindow( m_hToolTip );
			m_hToolTip = NULL;
		}
	}
	return TRUE;
}

// Notification handler.
LRESULT ClsInfoBar::OnNotify( LPNMHDR pNMHDR )
{
	// Correct notification?
	if ( pNMHDR->code == TTN_GETDISPINFO  && m_bIsClipped )
	{
		m_Title = m_hWnd;
		(( LPNMTTDISPINFO )pNMHDR )->lpszText = m_Title;
		return TRUE;
	}
	return FALSE;
}

// Window procedure override. Need to intercept the WM_SETFONT message.
LRESULT ClsInfoBar::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		// WP_PRINT
		case	WM_PRINTCLIENT:
			// Render what?
			if ( lParam & PRF_ERASEBKGND ) SendMessage( WM_ERASEBKGND, wParam );
			if ( lParam & PRF_CLIENT     ) SendMessage( WM_PAINT, wParam );
			return 0;
			
		// WM_SETTEXT?
		case	WM_SETTEXT:
		{
			// Let the base have a go at it.
			LRESULT rc = ClsWindow::WindowProc( uMsg, wParam, lParam );
			Invalidate();
			UpdateWindow();
			return rc;
		}

		// WM_SETFONT?
		case	WM_SETFONT:
		{
			// Get the font.
			ClsFont *pFont = ClsFont::FromHandle(( HFONT )wParam );

			// Get the font information.
			LOGFONT lf;
			pFont->GetLogFont( &lf );

			// Create the normal font.
			m_fNormal.CreateFontIndirect( &lf );

			// Create the vertical font.
			lf.lfEscapement = lf.lfOrientation = 900;
			lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
			m_fVertical = CreateFontIndirect( &lf );
			break;
		}

		// WM_GETFONT?
		case	WM_GETFONT:
		{
			// Return the correct handle.
			if ( m_bIsVertical ) return ( LRESULT )(( HFONT )m_fVertical );
			return ( LRESULT )(( HFONT )m_fNormal );
		}
	}

	// Pass to the base class.
	return ClsWindow::WindowProc( uMsg, wParam, lParam );
}

// Paint the control.
void ClsInfoBar::PaintControl( ClsDC *pDC )
{
	// Get the client rectangle.
	ClsRect	rcClient = GetClientRect();

	// Any room to render in?
	if ( ! rcClient.IsEmpty())
	{
		// Setup an off-screen buffer.
		ClsBufferDC	dc( *pDC, rcClient );

		// Snapshot the DC.
		int sDC = dc.SaveDC();

		// See if we can use a gradient
		// fill for the background.
		TRIVERTEX       trVert[ 2 ];
		GRADIENT_RECT   gRect;
		COLORREF	crColor = m_bIsVertical ? m_crBackgroundGradient : m_crBackgroundColor;

		// Setup the top (or left) side of the rectangle.
		trVert[ 0 ].x      = rcClient.Left();
		trVert[ 0 ].y      = rcClient.Top();
		trVert[ 0 ].Red    = ( COLOR16 )( GetRValue( crColor ) << 8 );
		trVert[ 0 ].Green  = ( COLOR16 )( GetGValue( crColor ) << 8 );
		trVert[ 0 ].Blue   = ( COLOR16 )( GetBValue( crColor ) << 8 );
		trVert[ 0 ].Alpha  = 0x0000;

		// Setup the bottom (or right) size of the rectangle.
		crColor		   = m_bIsVertical ? m_crBackgroundColor : m_crBackgroundGradient;
		trVert[ 1 ].x      = rcClient.Right();
		trVert[ 1 ].y      = rcClient.Bottom();
		trVert[ 1 ].Red    = ( COLOR16 )( GetRValue( crColor ) << 8 );
		trVert[ 1 ].Green  = ( COLOR16 )( GetGValue( crColor ) << 8 );
		trVert[ 1 ].Blue   = ( COLOR16 )( GetBValue( crColor ) << 8 );
		trVert[ 1 ].Alpha  = 0x0000;

		// Tell the function where the upper-left
		// and lower-right corners are located in the
		// TRIVERTEXT array.
		gRect.UpperLeft  = 0;
		gRect.LowerRight = 1;

		// Fill the area.
		if ( m_bUseGradient == FALSE || dc.GradientFill( trVert, 2, &gRect, 1, m_bIsVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H ) == FALSE )
		{
			// Simple fill the rectangle with the
			// background color.
			ClsBrush brush( m_crBackgroundColor );
			dc.FillRect( rcClient, &brush );
		}

		// Do we have an icon?
		if ( m_hIcon )
		{
			ClsPoint	pt;

			// Center the icon.
			if ( m_bIsVertical )
			{
				// Center horizontally.
				pt.X() = ( rcClient.Width() / 2 ) - 8;

				// Bottom or top?
				if ( m_bIconRight )
				{
					pt.Y() = rcClient.Top();
					rcClient.Top() += 20;
					rcClient.Bottom() -= 4;
				}
				else
				{
					pt.Y() = rcClient.Bottom() - 18;
					rcClient.Bottom() -= 20;
				}
			}
			else
			{
				// Center vertically.
				pt.Y() = ( rcClient.Height() / 2 ) - 8;

				// Left or right?
				if ( m_bIconRight )
				{
					pt.X() = rcClient.Right() - 18;
					rcClient.Right() -= 20;
					rcClient.Left() += 4;
				}
				else
				{
					pt.X() = rcClient.Left() + 2;
					rcClient.Left() += 20;
				}
			}

			// Render it at the left side of the area.
			dc.DrawIconEx( pt, m_hIcon, ClsSize( 16, 16 ), 0, NULL, DI_NORMAL );
		}
		else
		{
			// Shove the text 4 pixels to the right or up.
			if ( m_bIsVertical ) rcClient.Bottom() -= 4;
			else		     rcClient.Left()   += 4;
		}

		// Any room left?
		if ( ! rcClient.IsEmpty())
		{
			// Get the control title.
			ClsString title = m_hWnd;
			if( title.GetStringLength())
			{
				// Setup the color and draw mode.
				dc.SetTextColor( m_crTitleColor );
				dc.SetBkMode( TRANSPARENT );

				// Select the font to use.
				ClsSelector sel( &dc, m_bIsVertical ? m_fVertical : m_fNormal );

				// Get the size of the title string and
				// at the same time see how many characters
				// we can render.
				int nNum, nCX = m_bIsVertical ? rcClient.Height() : rcClient.Width();
				ClsSize	size = dc.GetTextExtentExPoint( title, nCX, &nNum );

				// The unicode version (XP Home SP1a) seems to return 6 on a 3 character
				// string?? This kludge fixes that problem.
				nNum = min( nNum, title.GetStringLength());

				// Will all characters fit?
				if ( nNum < title.GetStringLength())
				{
					// Below I use my own code to add the ellipsis to the text
					// to output instead of letting the system do it for me
					// with DT_PATH_ELLIPSIS and DT_END_ELLIPSIS.
					//
					// The reason for this is that I had too much trouble getting
					// it to work with vertically printed text.

					// Do we need to compact the caption as a path name?
					if ( m_bCompactAsPath )
					{
						// Get the current contents of the caption.
						TCHAR	sz[ MAX_PATH ];
						GetWindowText( sz, MAX_PATH );

						// Compact it.
						PathCompactPath( dc, sz, m_bIsVertical ? rcClient.Height() : rcClient.Width());

						// Assign the title object to the result
						// and specify the length of the result.
						title = sz;
						nNum = title.GetStringLength();
					}
					else
					{
						// Get the size of an ellipsis. I do not use the DT_END_ELLIPSIS
						// flag with DrawText() since I want to know when the text is
						// clipped so that I can show the tooltip when appropiate.
						ClsSize szell = dc.GetTextExtent( _T( "..." ), 3 );

						// Compute how many character will fit in the area excluding
						// the area the ellipsis takes up.
						dc.GetTextExtentExPoint( title, max( 0, nCX - szell.CX()), &nNum );

						// We want atleast one character to remain.
						nNum = max( nNum, 1 );

						// Setup the title string to contain the actual string
						// we render.
						title.SetStringLength( nNum );
						title += _T( "..." );

						// Add the length of the ellipsis.
						nNum += 3;
					}

					// We are clipped.
					m_bIsClipped = TRUE;
				}
				else
					// We are not clipped.
					m_bIsClipped = FALSE;

				// Render the title.
				if ( ! m_bIsVertical ) dc.TextOut( rcClient.Left(), rcClient.Top() + (( rcClient.Height() / 2 ) - ( size.CY() / 2 )), title, nNum );
				else		       dc.TextOut( rcClient.Left() + (( rcClient.Width() / 2 ) - ( size.CY() / 2 )), rcClient.Bottom(), title, nNum );
			}
		}
		else
			m_bIsClipped = TRUE;

		// Restore the DC.
		dc.RestoreDC( sDC );
	}
}

// OnPaint() overide.
LRESULT ClsInfoBar::OnPaint( ClsDC *pDC )
{
	// Input DC?
	if ( pDC ) PaintControl( pDC );
	else
	{
		ClsPaintDC dc( this );
		PaintControl( &dc );
	}
	return 0;
}

// For the layout engine.
BOOL ClsInfoBar::OnGetMinSize( ClsSize& szMinSize )
{
	// Do we have an icon?
	if ( m_hIcon ) szMinSize.CX() = szMinSize.CY() = 18;
	else
	{
		// Get caption height.
		int nCap = ::GetSystemMetrics( SM_CYCAPTION );
		if ( m_bIsVertical ) szMinSize.CX() = nCap;
		else		     szMinSize.CY() = nCap;
	}

	// Measure text.
	ClsString str( GetSafeHWND());

	// Any text?
	if ( str.GetStringLength())
	{
		// Get device context and setup the used
		// font.
		ClsGetDC dc( this );
		ClsSelector sel( &dc, m_bIsVertical ? m_fVertical : m_fNormal );

		// Size the text.
		ClsSize sz = dc.GetTextExtent( str );

		// Are we vertical?
		if ( m_bIsVertical )
		{
			// Add spacing.
			szMinSize.CY() += 4;

			// Adjust the minimum width if the text height is larger
			// than the icon width.
			if ( sz.CY() > szMinSize.CX()) szMinSize.CX() = sz.CY();

			// Add text width to the control height.
			szMinSize.CY() += sz.CX();
		}
		else
		{
			// Add spacing.
			szMinSize.CX() += 4;

			// Adjust the minimum height if the text height is larger
			// than the icon height.
			if ( sz.CY() > szMinSize.CY()) szMinSize.CY() = sz.CY();

			// Add text width to the control width.
			szMinSize.CX() += sz.CX();
		}
	}
	return TRUE;
}