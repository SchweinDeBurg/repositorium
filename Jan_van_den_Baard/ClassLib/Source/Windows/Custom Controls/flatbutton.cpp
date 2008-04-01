//
// flatbutton.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "flatbutton.h"
#include "../../gdi/paintdc.h"
#include "../../gdi/bufferdc.h"
#include "../../gdi/getdc.h"
#include "../../gdi/font.h"
#include "../../gdi/selector.h"
#include "../../tools/drawstate.h"
#include "../../tools/theming.h"
#include "../../tools/xpcolors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor.
ClsFlatButton::ClsFlatButton()
{
	// Defaults...
	m_aImageIndex[ 0 ] = 0;
	m_aImageIndex[ 1 ] = 1;
	m_bXPStyle = FALSE;
	m_bXPDefault = FALSE;
}

// Destructor.
ClsFlatButton::~ClsFlatButton()
{
}

// Send a WM_COMMAND message to the parent.
void ClsFlatButton::Trigger()
{
	// Get a pointer to the parent window.
	ClsWindow *pParent = GetParent();

	// Do we have a parent?
	if ( pParent )
		// Send the message.
		pParent->SendMessage( WM_COMMAND, MAKEWPARAM( GetWindowLong( GWL_ID ), FBN_CLICKED ), ( LPARAM )m_hWnd );
}

// Return the tooltip string.
BOOL ClsFlatButton::TipString( LPNMTTDISPINFO lpDispInfo )
{
	// Any text?
	if ( m_Tip.GetStringLength())
	{
		// Set the string.
		lpDispInfo->lpszText = m_Tip;
		return TRUE;
	}
	return FALSE;
}

// Window procedure override.
LRESULT ClsFlatButton::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// The one we need?
	if ( uMsg == WM_PRINTCLIENT )
	{
		// Render what?
		if ( lParam & PRF_ERASEBKGND ) SendMessage( WM_ERASEBKGND, wParam );
		if ( lParam & PRF_CLIENT     ) SendMessage( WM_PAINT, wParam );
		return 0;
	}
	// Base class.
	return ClsHotButton::WindowProc( uMsg, wParam, lParam );
}

void ClsFlatButton::PaintControl( ClsDC *pDC )
{
	// Get the client rectangle.
	ClsRect	rc = GetClientRect();

	// Any room to render in?
	if ( ! rc.IsEmpty())
	{
		// Create an off-screen buffer.
		ClsBufferDC	dc( *pDC, rc );

		// Snapshot the DC.
		int sDC = dc.SaveDC();

		// Render the frame.
		if ( ! m_bXPStyle ) RenderFrame( &dc, rc );
		else
		{
			// Only when were hot or down.
			COLORREF crFg, crBg;
			if (( IsHot() || IsDown()) && IsWindowEnabled())
			{
				crFg = XPColors.GetXPColor( ClsXPColors::XPC_OUTER_SELECTION );
				crBg = XPColors.GetXPColor( IsDown() ? ClsXPColors::XPC_INNER_CHECKED_SELECTED : ClsXPColors::XPC_INNER_SELECTION );
			}
			else
			{
				// Default colors...
				crFg = ( IsDefault() && IsWindowEnabled() && ! m_bXPDefault ) ? XPColors.GetXPColor( ClsXPColors::XPC_OUTER_SELECTION ) : XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_DISABLED );
				crBg = ::GetSysColor( COLOR_BTNFACE );
			}

			// Render rectangle.
			dc.OutlinedRectangle( rc, crFg, crBg );
		}

		// Determine rendering flags.
		DWORD dwFlags = 0;
		if ( ! IsWindowEnabled())			  dwFlags |= ClsDrawTools::CDSF_DISABLED;
		if ( IsDown() && ! ThemingAPI.IsThemingEnabled()) dwFlags |= ClsDrawTools::CDSF_DOWNPRESSED;
		if ( GetUIState() & UISF_HIDEACCEL )		  dwFlags |= ClsDrawTools::CDSF_HIDEACCEL;
		if ( IsHot())					  dwFlags |= ClsDrawTools::CDSF_HOT;

		// Do we have any images?
		if ( m_hImages )
		{
			// Copy the client rectangle.
			ClsRect rcimg( rc );

			// We need to know the size of the images
			// in the image list.
			int cx, cy;
			::ImageList_GetIconSize( m_hImages, &cx, &cy );

			// Determine the place at which we render the images.
			rcimg.Offset( ::GetSystemMetrics( SM_CXFRAME ), 0 );
			rcimg.Right() = rcimg.Left() + cx;

			// Adjust label rectangle.
			rc.Left() += cx + 4;
			
			// Render the image.
			if ( ! m_bXPStyle ) ClsDrawTools::RenderBitmap( dc, m_hImages, ( IsHot() || IsDown()) ? m_aImageIndex[ FIIF_HOT ] : m_aImageIndex[ FIIF_NORMAL ], rcimg, dwFlags );
			else ClsDrawTools::RenderXPBitmap( dc, m_hImages, ( IsHot() || IsDown()) ? m_aImageIndex[ FIIF_HOT ] : m_aImageIndex[ FIIF_NORMAL ], rcimg, dwFlags );
		}

		// Render the caption.
		ClsString str( m_hWnd );

		// Anything to render?
		if ( str.GetStringLength())
		{
			// Deflate the label rectangle.
			rc.Deflate( 3, 3 );

			// Do we have the focus?
			if ( HasFocus() && ! ( GetUIState() & UISF_HIDEFOCUS ) && ! m_bPanelHeader ) dc.DrawFocusRect( rc );

			// Setup the font to use.
			ClsFont font;
			GetFont( font );
			ClsSelector sel( &dc, font );

			// Render transparently.
			dc.SetBkMode( TRANSPARENT );
			
			// We must not use the disabled flag if we are
			// rendering XP style...
			if ( m_bXPStyle ) dwFlags &= ~( ClsDrawTools::CDSF_DISABLED | ClsDrawTools::CDSF_DOWNPRESSED );

			// Set text color.
			COLORREF cr = GetSysColor( IsWindowEnabled() ? COLOR_BTNTEXT : COLOR_GRAYTEXT );
			if ( IsDown() && m_bXPStyle ) cr = XPColors.GetXPColor( ClsXPColors::XPC_TEXT_BACKGROUND );

			if ( IsHot() || IsDown()) cr = m_crHotLabelColor == CLR_NONE ? cr : m_crHotLabelColor;
			else			  cr = m_crLabelColor    == CLR_NONE ? cr : m_crLabelColor;
			dc.SetTextColor( cr );

			// Render the caption.
			ClsDrawTools::RenderText( dc, str, rc, dwFlags );
		}

		// Restore device context.
		dc.RestoreDC( sDC );
	}
}

// WM_PAINT message handler.
LRESULT ClsFlatButton::OnPaint( ClsDC *pDC )
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
BOOL ClsFlatButton::OnGetMinSize( ClsSize& szMinSize )
{
	// Add room for the frame.
	szMinSize.CX() += ::GetSystemMetrics( SM_CXFRAME ) * 3;
	szMinSize.CY() += ::GetSystemMetrics( SM_CYFRAME ) * 3;

	// Images?
	int cxi = 0, cyi = 0;
	if ( m_hImages && ImageList_GetImageCount( m_hImages ))
	{
		// Get the image size.
		ImageList_GetIconSize( m_hImages, &cxi, &cyi );

		// Make room.
		cxi += 4;

		// Add sizes.
		szMinSize.CX() += cxi;
		szMinSize.CY() += cyi;
	}

	// Do we have a caption?
	ClsString str( GetSafeHWND());
	if ( str.GetStringLength())
	{
		// Setup the DC.
		ClsGetDC dc( this );
		ClsFont font;
		GetFont( font );
		ClsSelector sel( &dc, font );

		// Measure the caption.
		ClsRect rc;
		dc.DrawText( str, rc, DT_CALCRECT );

		// Add the width of the caption.
		szMinSize.CX() += rc.Width() + 4;

		// Adjust the height if necessary.
		if ( rc.Height() > cyi )
			szMinSize.CY() += rc.Height() - cyi;
	}
	return TRUE;
}
