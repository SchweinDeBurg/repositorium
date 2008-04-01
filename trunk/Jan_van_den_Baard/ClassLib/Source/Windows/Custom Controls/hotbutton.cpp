//
// hotbutton.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//
// Parts of this code (COddButton) have the following copyright:
//
// Copyright (c) 2001 Paolo Messina and Jerzy Kaczorowski
// 
// The contents of this file are subject to the Artistic License (the "License").
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.opensource.org/licenses/artistic-license.html
// 
// THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED 
// WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF 
// MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//

#include "../../stdafx.h"

#include "hotbutton.h"
#include "../../gdi/dc.h"
#include "../../gdi/selector.h"
#include "../../coords/point.h"
#include "../../coords/rect.h"
#include "../../tools/theming.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// COddButton mask for control's type
#define OBS_TYPEMASK SS_TYPEMASK

// Repeat timer IDs.
#define RPT_DELAY	0xDEAD
#define RPT_SPEED	0xDEAD + 1


// Constructor.
ClsHotButton::ClsHotButton()
{
	// Clear data.
	m_dwFlags		= 0;
	m_hToolTip		= NULL;
	m_hImages		= NULL;
	m_crBkColor		= CLR_NONE;
	m_crHotBkColor		= CLR_NONE;
	m_crLabelColor		= CLR_NONE;
	m_crHotLabelColor	= CLR_NONE;
	m_bTransparent		= FALSE;
	m_hTheme		= NULL;
	m_bRepeatTrigger	= FALSE;
	m_bPanelHeader		= FALSE;

	// COddButton
	m_bDefault		= FALSE;
	
	// COddButton.
	m_bCanBeDefault		= FALSE;
	
	// invalid value, since type still unknown
	m_nTypeStyle		= OBS_TYPEMASK;
}

// Destructor.
ClsHotButton::~ClsHotButton()
{
	// Destroy tooltip window and the theme handle.
	if ( m_hToolTip ) ::DestroyWindow( m_hToolTip );
	if ( m_hTheme ) ThemingAPI.CloseThemeData( m_hTheme );
}

// Called just before the control is sub-classed.
void ClsHotButton::PreSubclassWindow() 
{
	// set initial control type
	m_nTypeStyle = GetStyle() & OBS_TYPEMASK;

	// set initial default state flag
	if ( m_nTypeStyle == BS_DEFPUSHBUTTON )
	{
		// enable default state handling for push buttons
		m_bCanBeDefault = TRUE;

		// set default state for a default button
		m_bDefault = TRUE;

		// adjust style for default button
		m_nTypeStyle = BS_PUSHBUTTON;
	}
	else if ( m_nTypeStyle == BS_PUSHBUTTON )
	{
		// enable default state handling for push buttons
		m_bCanBeDefault = TRUE;
	}

	// you should not set the Owner Draw before this call
	// (don't use the resource editor "Owner Draw" or
	// ModifyStyle(0, BS_OWNERDRAW) before calling PreSubclassWindow() )
	_ASSERT( m_nTypeStyle != BS_OWNERDRAW );

	// switch to owner-draw
	ModifyStyle( OBS_TYPEMASK, BS_OWNERDRAW, SWP_FRAMECHANGED );

	// base class.
	ClsButton::PreSubclassWindow();
}

// Derived classes should override this.
void ClsHotButton::Trigger()
{
}

// Derived classes should override this.
BOOL ClsHotButton::TipString( LPNMTTDISPINFO )
{
	// No tip by default.
	return FALSE;
}

// Show or hide tooltips.
BOOL ClsHotButton::ShowToolTip( BOOL bShowTip /* = TRUE */ )
{
	_ASSERT_VALID( GetSafeHWND()); // Must be valid.

	// Must we show tool-tips?
	if ( bShowTip && m_hToolTip == NULL )
	{
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
	else
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
LRESULT ClsHotButton::OnNotify( LPNMHDR pNMHDR )
{
	// Correct notification?
	if ( pNMHDR->code == TTN_GETDISPINFO )
		// Call the override.
		return TipString(( LPNMTTDISPINFO )pNMHDR );
	return FALSE;
}

// Render button frame.
void ClsHotButton::RenderFrame( ClsDC *pDC, LPRECT lpRect )
{
	// Are themes enabled?
	if ( ThemingAPI.IsThemingEnabled())
	{
		// If the theme is not yet loaded we send a WM_THEMECHANGED
		// message to the control which will automatically load the
		// button theme.
		if ( ! m_hTheme ) m_hTheme = ThemingAPI.OpenThemeData( GetSafeHWND(), L"BUTTON" );
		if ( m_hTheme )
		{
			// Determine rendering flags.
			DWORD dwState = PBS_NORMAL;
			if ( IsWindowEnabled() == FALSE ) dwState = PBS_DISABLED;
			else if ( IsDefault())		  dwState = PBS_DEFAULTED;
			if ( IsHot())			  dwState = PBS_HOT;
			if ( IsDown())			  dwState = PBS_PRESSED;

			// First the parent background.
			ThemingAPI.DrawThemeParentBackground( GetSafeHWND(), *pDC, lpRect );
			
			// Render the background.
			ThemingAPI.DrawThemeBackground( m_hTheme, *pDC, BP_PUSHBUTTON, dwState, lpRect, NULL );
			return;
		}
	}

	// Brush to render background.
	ClsBrush brush( ::GetSysColor( COLOR_BTNFACE ));
 
	// Hot?
	if ( m_dwFlags & HBF_NOHOT )
	{
		// Transparent?
		if ( m_bTransparent == FALSE )
		{
			// Fill background.
			if ( m_crBkColor != CLR_NONE ) brush.CreateSolidBrush( m_crBkColor );
			pDC->FillRect( lpRect, brush );
		}

		// Are we the default?
		if ( IsDefault() && ! m_bPanelHeader )
		{
			// Render default choice frame.
			pDC->DrawEdge( lpRect, EDGE_RAISED, BF_MONO | BF_RECT );
			::InflateRect( lpRect, -1, -1 );
		}
		// Normal border rendering.
		pDC->DrawEdge( lpRect, EDGE_RAISED, BF_RECT | ( IsDown() ? BF_FLAT : 0 ));
	}
	else
	{
		// Transparent?
		if ( m_bTransparent == FALSE )
		{
			if (( IsHot() || IsDown()) && IsWindowEnabled())
			{
				// Fill background.
				if ( m_crHotBkColor != CLR_NONE ) brush.CreateSolidBrush( m_crHotBkColor );
				pDC->FillRect( lpRect, brush );
			}
			else
			{
				// Fill background.
				if ( m_crBkColor != CLR_NONE ) brush.CreateSolidBrush( m_crBkColor );
				pDC->FillRect( lpRect, brush );
			}
		}

		// Do we have a hot frame?
		if (( m_dwFlags & HBF_HOTFRAME ) != HBF_HOTFRAME )
		{
			// If we are hot we render the flat edges.
			if (( IsHot() || IsDown()) && IsWindowEnabled())
				pDC->DrawEdge( lpRect, IsDown() ? BDR_SUNKENOUTER : BDR_RAISEDINNER, BF_RECT );
		}
		else
		{
			// Are we the default?
			if ( IsDefault() && ! m_bPanelHeader )
			{
				// Render default choice frame.
				pDC->DrawEdge( lpRect, EDGE_RAISED, BF_MONO | BF_RECT );
				::InflateRect( lpRect, -1, -1 );
			}

			// Are we hot?
			if (( IsHot() || IsDown()) && IsWindowEnabled())
				pDC->DrawEdge( lpRect, EDGE_RAISED, BF_RECT );
			else
				pDC->DrawEdge( lpRect, BDR_RAISEDINNER, BF_RECT );
		}
	}
}

// COddButton
UINT ClsHotButton::GetControlType()
{
	return m_nTypeStyle;
}

// COddButton
BOOL ClsHotButton::IsDefault()
{
	// if we have default state, we must like it!
	_ASSERT(( m_bCanBeDefault && m_bDefault ) == m_bDefault );

	return m_bDefault;
}

// COddButton
void ClsHotButton::EnableDefault(BOOL bEnable)
{
	m_bCanBeDefault = bEnable;

	// disabling default when control has default state
	// needs removing the default state
	if ( ! bEnable && m_bDefault )
	{
		// remove default state
		SendMessage( BM_SETSTYLE, ( GetStyle() & ~OBS_TYPEMASK ) | BS_PUSHBUTTON, TRUE );
		_ASSERT( m_bDefault == FALSE );

		// update default button
		LRESULT lRes = GetParent()->SendMessage( DM_GETDEFID );
		if ( HIWORD( lRes ) == DC_HASDEFID )
			GetParent()->SendMessage( DM_SETDEFID, LOWORD( lRes ));
	}
}

// Set or clear the RepeatTrigger functionality.
BOOL ClsHotButton::RepeatTrigger( BOOL bRepeat /*= TRUE*/ )
{
	// Do we need to change the trigger functionality?
	if ( m_bRepeatTrigger != bRepeat )
	{
		// Setup the flag.
		m_bRepeatTrigger = bRepeat;

		// Compute the delay and speed variables.
		SystemParametersInfo( SPI_GETKEYBOARDDELAY, 0, &m_dwDelay, 0 );
		m_dwDelay = 250 + ( m_dwDelay * 250 );
		SystemParametersInfo( SPI_GETKEYBOARDSPEED, 0, &m_dwSpeed, 0 );
		m_dwSpeed = ( DWORD )(( float )400 - (( float )m_dwSpeed * 11.80 ));
		return ! bRepeat;
	}
	return bRepeat;
}

// Handle WM_COMMAND of this button.
LRESULT ClsHotButton::OnReflectedCommand( UINT nNotifyCode, BOOL& bNotifyParent )
{
	// Clicked?
	if ( nNotifyCode == BN_CLICKED )
	{
		// No need to notify the parent.
		bNotifyParent = FALSE;

		// Call the overidable.
		Trigger();
		return 0;
	}
	// Base class.
	return ClsButton::OnReflectedCommand( nNotifyCode, bNotifyParent );
}

// Window procedure override. This code will take care of
// hot-tracking the derived buttons.
LRESULT ClsHotButton::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
        switch ( uMsg ) 
	{
		case	WM_GETFONT:
			// Return font handle. If none exists we create the font
			// which is used on the control parent.
			if ( m_Font.IsValid() == FALSE )
			{
				// Create a copy of the parent font. On NT4 we use the system messagebox
				// font...
				if ( ClsGetApp()->GetPlatformID() == VER_PLATFORM_WIN32_NT && ClsGetApp()->GetMajorVersion() <= 4 )
				{
					NONCLIENTMETRICS ncm = { sizeof( NONCLIENTMETRICS ), 0 };
					if ( ::SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( ncm ), ( void * )&ncm, 0 ))
						m_Font.CreateFontIndirect( &ncm.lfMessageFont );
				}
				else
				{
					// Use the parent font...
					ClsWindow *pParent = GetParent();
					if ( pParent )
					{
						// Get it's font.
						HFONT hParentFont = ( HFONT )pParent->SendMessage( WM_GETFONT );
						if ( hParentFont )
						{
							// Wrap the handle.
							ClsFont *pFont = ClsFont::FromHandle( hParentFont );

							// Get the font information.
							LOGFONT         lf;
							if ( pFont->GetLogFont( &lf ))
								// Create the font.
								m_Font.CreateFontIndirect( &lf );
						}
					}
				}
			}

			// Return the font handle.
			return ( LRESULT )(( HFONT )m_Font );

		case	WM_SETFONT:
		{
			// Did we see a valid font?
			if ( wParam )
			{
				// Create a copy of the font.
				ClsFont *pFont = ClsFont::FromHandle(( HFONT )wParam );

				// Get the font information.
				LOGFONT         lf;
				if ( pFont->GetLogFont( &lf ))
					// Create the font.
					m_Font.CreateFontIndirect( &lf );
			}
			break;
		}

		case	WM_SETTEXT:
		{
			// First the base class the re-render.
			LRESULT rc = ClsButton::WindowProc( uMsg, wParam, lParam );
			Invalidate();
			return rc;
		}

		case	WM_TIMER:
			// Delay or speed?
			if ( wParam == RPT_DELAY || wParam == RPT_SPEED )
			{
				// Delay timer?
				if ( wParam == RPT_DELAY )
				{
					// Kill it and go onto the
					// speed timer.
					KillTimer( RPT_DELAY );
					SetTimer( RPT_SPEED, m_dwSpeed );
				}

				// Trigger the event if necessary.
				if ( m_dwFlags & HBF_DOWN )
					Trigger();
			}
			break;

		case	WM_LBUTTONDBLCLK:
                case    WM_LBUTTONDOWN:
			// Show use pressed and track mouse
			// movements. Swith of hot mode.
			m_dwFlags |= ( HBF_DOWN | HBF_TRACK );
			m_dwFlags &= ~HBF_ISHOT;
			Invalidate();

			// Capture mouse events.
			SetCapture();

			// Start repeat timer if
			// appropiate.
			if ( m_bRepeatTrigger )
			{
				Trigger();
				SetTimer( RPT_DELAY, m_dwDelay );
			}

			// Steal the focus.
			SetFocus();
			return 0;

                case    WM_MOUSEMOVE:
		{
			// Get mouse points.
			ClsPoint pt( LOWORD( lParam ), HIWORD( lParam ));

			// Are we tracking?
			if ( m_dwFlags & HBF_TRACK ) 
			{
				// Is the mouse still located
				// inside the controls rectangle?
				ClsRect rc = GetClientRect();
				if ( rc.PtInRect( pt )) 
				{
					// Down-pressed state?
					if (( m_dwFlags & HBF_DOWN ) != HBF_DOWN ) 
					{
						// We are not yet in down-pressed state
						// so we set it now.
						m_dwFlags |= HBF_DOWN;
						Invalidate();
					}
				} 
				else 
				{
					// Down-pressed state?
					if (( m_dwFlags & HBF_DOWN ) == HBF_DOWN ) 
					{
						// We are in down-pressed state
						// so we clear it now.
						m_dwFlags &= ~HBF_DOWN;
						Invalidate();
					}
				}
			} 
			else 
			{
				// Down pressed mode?
				if ((( m_dwFlags & HBF_DOWN  ) != HBF_DOWN  ) &&
				    (( m_dwFlags & HBF_NOHOT ) != HBF_NOHOT ))
				{
					// Cursor inside the control?
					ClsRect rc = GetClientRect();
					if ( rc.PtInRect( pt )) 
					{
						// Are we hot?
						if (( m_dwFlags & HBF_ISHOT ) != HBF_ISHOT ) 
						{
							// No, make us hot.
							SetCapture();
							m_dwFlags |= HBF_ISHOT;
							Invalidate();
						}
					} 
					else 
					{
						// Are we hot?
						if (( m_dwFlags & HBF_ISHOT ) == HBF_ISHOT ) 
						{
							// Not hot anymore.
							if ( GetCapture() == this )
								::ReleaseCapture();
							m_dwFlags &= ~HBF_ISHOT;
							Invalidate();
						}
					}
				} 
				else if (( m_dwFlags & HBF_ISHOT ) == HBF_ISHOT ) 
				{
					// Release the capture if
					// we got it.
					if ( GetCapture() == this )
						::ReleaseCapture();

					// No more hot mode.
					m_dwFlags &= ~HBF_ISHOT;
					
					// Re-render.
					Invalidate();
				}
			}
			break;
		}
		
		case	WM_CAPTURECHANGED:
			// Clear flags.
			if (( HWND )lParam != m_hWnd ) 
			{
				// Kill the timers.
				KillTimer( RPT_DELAY );
				KillTimer( RPT_SPEED );

				// Re-render.
				m_dwFlags &= ~( HBF_TRACK | HBF_ISHOT );
				Invalidate();
			}
			return 0;

		case	WM_CANCELMODE:
                case    WM_LBUTTONUP:
			// Normal mouse cature.
			if ( GetCapture() == this )
				::ReleaseCapture();

			// Clear track/hot mode.
			m_dwFlags &= ~( HBF_TRACK | HBF_ISHOT );

			// Kill the timers.
			KillTimer( RPT_DELAY );
			KillTimer( RPT_SPEED );

			// Down-pressed mode?
			if ( m_dwFlags & HBF_DOWN ) 
			{
				// Clear down-pressedmode.
				m_dwFlags &= ~HBF_DOWN;

				// Re-render.
				Invalidate();

				// Trigger button action if we are
				// not in repeat mode.
				if ( ! m_bRepeatTrigger )
					Trigger();
			} 
			else
				// Just re-render.
				Invalidate();
			return 0;

		case    WM_SETFOCUS:
			// Setup focus bit.
			m_dwFlags |= HBF_FOCUS;

			// Re-render.
			Invalidate();
			return 0;

		case    WM_KILLFOCUS:
			// Did we get the focus?
			if ( wParam != ( WPARAM )GetSafeHWND())
			{
				// Remove flags.
				m_dwFlags &= ~( HBF_FOCUS | HBF_DOWN | HBF_TRACK );

				// Release capture if we have it.
				if ( GetCapture() == this )
					::ReleaseCapture();

				// Kill the timers.
				KillTimer( RPT_DELAY );
				KillTimer( RPT_SPEED );
			}

			// Fall through.

                case    WM_ENABLE:
                        // Refresh.
			Invalidate();
			return 0;

		case	WM_THEMECHANGED:
			// Theming available?
			if ( ThemingAPI.IsThemingEnabled())
			{
				// Destroy old theme and create a new one.
				if ( m_hTheme ) ThemingAPI.CloseThemeData( m_hTheme );
				m_hTheme = ThemingAPI.OpenThemeData( GetSafeHWND(), L"BUTTON" );
				return 0;
			}
			break;

		case	WM_GETDLGCODE:
		{
			// COddButton.
			UINT nCode = ( UINT )ClsButton::WindowProc( uMsg, wParam, lParam );

			// handle standard control types
			switch ( GetControlType())
			{
				case BS_RADIOBUTTON:
				case BS_AUTORADIOBUTTON:
					nCode |= DLGC_RADIOBUTTON;
					break;

				case BS_GROUPBOX:
					nCode = DLGC_STATIC;
					break;
			}

			// tell the system if we want default state handling
			// (losing default state always allowed)
			if ( m_bCanBeDefault || m_bDefault )
				nCode |= ( m_bDefault ? DLGC_DEFPUSHBUTTON : DLGC_UNDEFPUSHBUTTON );

			return nCode;
		}

		case	BM_SETSTYLE:
		{
			// COddButton
			UINT nNewType = ( UINT )( wParam & OBS_TYPEMASK );

			// update default state flag
			if ( nNewType == BS_DEFPUSHBUTTON )
			{
				// we must like default state at this point
				_ASSERT( m_bCanBeDefault );

				m_bDefault = TRUE;
			}
			else if ( nNewType == BS_PUSHBUTTON )
			{
				// losing default state always allowed
				m_bDefault = FALSE;
			}

			// Re-render the control to show it's current state.
			Invalidate();

			// can't change control type after owner-draw is set.
			// let the system process changes to other style bits
			// and redrawing, while keeping owner-draw style
			return ::DefWindowProc( GetSafeHWND(), BM_SETSTYLE, ( wParam & ~OBS_TYPEMASK ) | BS_OWNERDRAW, lParam );
		}
	}

	// Call the base class.
	return ClsButton::WindowProc( uMsg, wParam, lParam );
}
