//
// colorpicker.cpp.
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "colorpicker.h"
#include "colorpopup.h"
#include "../common dialogs/colordialog.h"
#include "../../gdi/paintdc.h"
#include "../../gdi/bufferdc.h"
#include "../../gdi/getdc.h"
#include "../../gdi/brush.h"
#include "../../gdi/selector.h"
#include "../../tools/drawstate.h"
#include "../../tools/theming.h"
#include "../../tools/xpcolors.h"
#include "../../strings/string.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor. Set's up defaults.
ClsColorPicker::ClsColorPicker()
{
	// Setup defaults.
	m_crColor = RGB( 0, 0, 0 );
	m_crTempColor = CLR_NONE;
	m_bFullOpen = TRUE;
	m_bRenderArrow = FALSE;
	m_bXPPopup = FALSE;
	m_bXPControl = FALSE;
	m_bIsDropped = FALSE;
	m_strDefault = _T( "Default Color" );
	m_strCustom  = _T( "Custom Color..." );
	m_crDefaultColor = CLR_NONE;
}

// Destructor. Does nothing.
ClsColorPicker::~ClsColorPicker()
{;}

// Trigger the action.
void ClsColorPicker::Trigger()
{
	// Get the control bounds.
	ClsRect rc = GetWindowRect();

	// Update control visuals.
	UpdateWindow();

	// Create the popup. The popup will automatically destroy
	// itself.
	new ClsColorPopup( ClsPoint( rc.Left(), m_bXPControl ? rc.Bottom() - 1 : rc.Bottom()), m_crColor, this, 0, m_strDefault, m_strCustom, NULL, m_bFullOpen, m_bXPPopup );
}

// Capture popup window messages.
LRESULT ClsColorPicker::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case	WM_NCACTIVATE:
			{
				// Pass this on if we have a parent.
				ClsWindow *pParent = GetParent();
				if ( pParent )
				{
					pParent->SendMessage( WM_NCACTIVATE, TRUE );
					return 0;
				}
			}
			break;

		// Render control.
		case	WM_PRINTCLIENT:
			if ( lParam & PRF_ERASEBKGND ) SendMessage( WM_ERASEBKGND, wParam );
			if ( lParam & PRF_CLIENT     ) PaintControl( ClsDC::FromHandle(( HDC )wParam ));
			return 0;

		case	CPN_SELCHANGE:
			// Set the temporary color so that the PaintControl()
			// method knows it only needs to refresh the
			// color rectangle.
			m_crTempColor = ( COLORREF )wParam;
			Invalidate();
			break;

		// Selection ended successfully?
		case	CPN_SELENDOK:
		{
			// Store selected color.
			m_crColor = ( COLORREF )wParam;

			// Re-render control.
			Invalidate();

			// Send a message to our parent.
			GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM( GetWindowLong( GWL_ID ), COLPN_COLORCHANGED ), ( LPARAM )m_hWnd );
		}
		break;

		// Colorpopup opened?
		case	CPN_DROPDOWN:
			m_bIsDropped = TRUE;
			Invalidate();
			UpdateWindow();
			break;

		// Colorpopup closed?
		case	CPN_CLOSEUP:
			m_bIsDropped = FALSE;
			ClsWindow *pParent = GetParent();
			if ( pParent )
			{
				pParent->Invalidate();
				pParent->UpdateWindow();
			}
			else
			{
				Invalidate();
				UpdateWindow();
			}
			break;
	}

	// Pass to the base class.
	return ClsHotButton::WindowProc( uMsg, wParam, lParam );
}

// Return the tip string.
extern ColorTableEntry crDefaultColors[];
BOOL ClsColorPicker::TipString( LPNMTTDISPINFO lpDispInfo )
{
	// Default color?
	if ( m_crColor == CLR_DEFAULT )
	{
		// Make it so...
		lpDispInfo->lpszText = m_strDefault;
		return TRUE;
	}

	// Try to find it in the default color table.
	for ( int i = 0; crDefaultColors[ i ].crColor != RGB_END; i++ )
	{
		// Is this the one?
		if ( crDefaultColors[ i ].crColor == m_crColor )
		{
			// Show it's name.
			lpDispInfo->lpszText = crDefaultColors[ i ].szName;
			return TRUE;
		}
	}

	// Format the tip.
	m_strTip.Format( _T( "%s (R=%ld, G=%ld, B=%ld)" ), ( LPCTSTR )m_strCustom, GetRValue( m_crColor ), GetGValue( m_crColor ), GetBValue( m_crColor ));

	// Any available?
	if ( m_strTip.GetStringLength())
	{
		// Set it up.
		lpDispInfo->lpszText = m_strTip;
		return TRUE;
	}
	return FALSE;
}

void ClsColorPicker::PaintControl( ClsDC *pDC )
{
	// Pre-set flags.
	DWORD dwFlags = 0;
	if ( ! IsWindowEnabled())			   dwFlags |= ClsDrawTools::CDSF_DISABLED;
	if ( IsDown() && ! ThemingAPI.IsThemingEnabled())  dwFlags |= ClsDrawTools::CDSF_DOWNPRESSED;
	if ( GetUIState() & UISF_HIDEACCEL )		   dwFlags |= ClsDrawTools::CDSF_HIDEACCEL;

	// Get frame sizes.
	int cx = GetSystemMetrics( SM_CXEDGE );
	int cy = GetSystemMetrics( SM_CYEDGE );
	int nWidth = 0;
	COLORREF crFg, crBg = ::GetSysColor( COLOR_BTNFACE );

        // Get the client rectangle.
	ClsRect	rc = GetClientRect();

	// Any space to render in?
	if ( ! rc.IsEmpty())
	{
		// Setup off-screen buffer.
		ClsBufferDC	dc( *pDC, rc );

		// Snapshot the DC.
		int sDC = dc.SaveDC();

		// Draw the button edge.
		if ( ! m_bXPControl ) RenderFrame( &dc, rc );
		else 
		{
			// Only when were hot, down or the popup
			// is open.
			if (( IsHot() || IsDown() || m_bIsDropped ) && IsWindowEnabled())
			{
				// Determine the fill and outline colors to use.
				if ( m_bIsDropped )
				{
					crFg = XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_DISABLED );
					crBg = XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_BACKGROUND );
				}
				else
				{
					crFg = XPColors.GetXPColor( ClsXPColors::XPC_OUTER_SELECTION );
					crBg = XPColors.GetXPColor( IsDown() ? ClsXPColors::XPC_INNER_CHECKED_SELECTED : ClsXPColors::XPC_INNER_SELECTION );
				}
			}
			else
			{
				// Default colors...
				crFg = ( IsDefault() && IsWindowEnabled()) ? XPColors.GetXPColor( ClsXPColors::XPC_OUTER_SELECTION ) : XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_DISABLED );
			}

			// Render rectangle.
			dc.OutlinedRectangle( rc, crFg, crBg );

			// Render the shadow.
			if ( m_bIsDropped && m_crTempColor == CLR_NONE )
			{
				ClsWindow *pParent = GetParent();
				if ( pParent )
				{
					double factor;
					ClsColor col;
					GetWindowRect( rc );
					pParent->ScreenToClient( rc );
					if ( rc.IsEmpty() == FALSE )
					{
						// Get the parent DC.
						ClsGetDC wdc( pParent );
						if ( wdc.IsValid())
						{
							// Copy the parent rectangle and adjust the width
							// for the shadow.
							rc.Right() += 4;

							// Render the shadow at rhe right side of the rectangle.
							for ( int x = 0; x < 5; x++ )
							{
								for ( int y = 0; y <= rc.Height() - 6; y++ )
								{
									col = wdc.GetPixel( x + ( rc.Right() - 4 ), y + rc.Top() + 4 );
									if ( y  < 4 ) factor = y * 0.02;
									else factor = 0.1;
									factor = max( 0.01, min( 0.01 + (( 4 - x ) * factor ), 1 ));
									col.DarkenColor( factor, factor, factor );				
									wdc.SetPixelV( x + ( rc.Right() - 4 ), y + rc.Top() + 4 , col );
								}
							}
						}
					}
				}
			}
		}

		// Get client rectangle.
		GetClientRect( rc );

		// Do we need to render a drop-down arrow?
		if ( m_bRenderArrow )
		{
			// Space needed by the drop-down arrow.
			nWidth = ::GetSystemMetrics( SM_CXHSCROLL );

			// Copy client rectangle and adjust it to fit the
			// dropdwon arrow.
			ClsRect rdd = rc;
			rdd.Left() = rdd.Right() - ( nWidth + 4 );
			rdd.Deflate( 0, 4 );

			// Render the separator.
			dc.DrawEdge( rdd, m_bXPControl ? BDR_SUNKENOUTER : EDGE_ETCHED, BF_LEFT );

			// Offset position.
			rdd.Offset( 0, 2 );

			// Render the drop-down arrow.
			ClsDrawTools::RenderBitmap( dc, ClsGetApp()->GetClsImages(), ClsApp::CII_DROPDOWN, rdd, dwFlags & ~ClsDrawTools::CDSF_DOWNPRESSED );
			rc.Right() -= nWidth + 3;
		}

		// Get the caption.
		ClsString str( m_hWnd );

		// Get box size.
		int nBoxSize = max( 8, ( GetSystemMetrics( SM_CXEDGE ) * 2 ) + (( rc.Width() / 10 ) * 2 ));

		// Adjust to create the color rectangle.
		if ( str.GetStringLength() == 0 ) rc.Left() = cx + 2;
		else				  rc.Left() = rc.Right() - nBoxSize;
	
		rc.Right()  -= cx + 2;
		rc.Top()     = cy + 2;
		rc.Bottom() -= cy + 2;

		// Render the frame around the
		// color box.
		if ( ! m_bXPControl ) dc.DrawEdge( rc, IsDown() ? BDR_RAISEDINNER : BDR_SUNKENOUTER, BF_RECT );
		else dc.OutlinedRectangle( rc, XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_DISABLED ), XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_BACKGROUND ));

		// Adjust the rectangle to fit the
		// actual color box.
		rc.Left()   += 2;
		rc.Top()    += 2;
		rc.Right()  -= 2;
		rc.Bottom() -= 2;

		// Render the color box.
		COLORREF cr = ( m_crTempColor == CLR_NONE ) ? m_crColor : m_crTempColor;
		if ( cr == CLR_DEFAULT ) cr = m_crDefaultColor;
		if ( IsWindowEnabled())
		{
			// Do we have a color to render?
			ClsBrush brush;
			if ( cr != CLR_NONE )
			{
				// Fill with the selected color.
				brush.CreateSolidBrush( cr );
			}
			else
			{
				// Fill as transparent.
				brush.CreateHatchBrush( HS_DIAGCROSS, ::GetSysColor( COLOR_BTNTEXT ));
				dc.SetBkColor( crBg );
			}
			// Fill the colorbox.
			dc.FillRect( rc, &brush );
		}
		else
		{
			dc.SetTextColor( cr );
			dc.FillRect( rc, ClsGetApp()->GetPatternBrush());
		}

		// Get client rectangle.
		GetClientRect( rc );
		if ( m_bRenderArrow ) rc.Right() -= nWidth + 2 ;

		// Adjust rectangle.
		if ( str.GetStringLength() == 0 ) 
		{
			rc.Left()    = cx + 4;
			rc.Top()     = cy + 4;
			rc.Right()  -= cx + 4;
			rc.Bottom() -= cy + 4;
		} 
		else 
		{
			rc.Right()  -= nBoxSize + 3;
			rc.Left()    = cx + 2;
			rc.Top()     = cy + 2;
			rc.Bottom() -= cy + 2;
		}

		// Do we have the keyboard focus?
		if ( HasFocus() && ! ( GetUIState() & UISF_HIDEFOCUS ) && ! m_bIsDropped )
			dc.DrawFocusRect( rc );

		// Render caption.
		if ( str.GetStringLength())
		{
			// Setup font.
			ClsFont font;
			GetFont( font );
			ClsSelector sel( &dc, font );

			// Render transparent.
			dc.SetBkMode( TRANSPARENT );

			// Setup color.
			dc.SetTextColor( ::GetSysColor( COLOR_BTNTEXT ));

			// Render the caption.
			ClsDrawTools::RenderText( dc, str, rc, dwFlags );
		}

		// Clear temprary color.
		m_crTempColor = CLR_NONE;

		// Restore device context.
		dc.RestoreDC( sDC );
	}
}

LRESULT ClsColorPicker::OnPaint( ClsDC *pDC )
{
	// Device context passed?
	if ( pDC ) PaintControl( pDC );
	else
	{
		// Create DC.
		ClsPaintDC dc( this );
		PaintControl( &dc );
	}
	return 0;
}

// For the layout engine.
BOOL ClsColorPicker::OnGetMinSize( ClsSize& szMinSize )
{
	// First we have the frame sizes and the minimum.
	// size of the color box.
	szMinSize.CX() = ( ::GetSystemMetrics( SM_CXFRAME ) * 3 ) + 20;
	szMinSize.CY() = ( ::GetSystemMetrics( SM_CYFRAME ) * 3 );

	// Any caption?
	ClsString str( GetSafeHWND());
	if ( str.GetStringLength())
	{
		// Get the dimensions of the control
		// caption.
		ClsGetDC dc( this );
		ClsFont font;
		GetFont( font );
		ClsSelector sel( &dc, font );

		// Measure the text.
		ClsRect rc;
		dc.DrawText( str, rc, DT_CALCRECT );

		// Add size.
		szMinSize.Add( rc.Size());
	}
	return TRUE;
}