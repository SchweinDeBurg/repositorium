// ******************************************************************************
// BalloonHelp.cpp : implementation file
// Copyright 2001-2002, Joshua Heyer
//  You are free to use this code for whatever you want, provided you
// give credit where credit is due.  (I seem to get a lot of questions
// about that statement...  All i mean is, don't copy huge bits of code
// and then claim you wrote it.  You don't have to put my name in an about
// box or anything.  Though i'm not going to stop you if that's really what
// you want :~) )
//  I'm providing this code in the hope that it is useful to someone, as i have
// gotten much use out of other peoples code over the years.
//  If you see value in it, make some improvements, etc., i would appreciate it 
// if you sent me some feedback.
//
// ******************************************************************************
//
// Implementation for this class-library by Jan van den Baard (JBa).
//
// 19-Aug-2002 (JBa) - Removed the usage of the BHCOOKIE. Replaced by a ClsWindow
//		       based solution to close the popups.
//
// 16-Aug-2002 (JBa) - Updated source to Release #3 by Joshua Heyer.
//
// 18-Feb-2002 (JBa) - Added the unCLOSE_ON_KEYPRESS flag as suggested by the original 
//		       author Joshua Heyer.
//
//  3-Feb-2002 (JBa) - Fixed a small rendering problem which occured when the control
//		       frame re-rendered.
//
//  5-Jan-2002 (JBa) - Removed the ID_TIMER_SHOW, ID_TIMER_HIDE and ID_TIMER_HOTTRACK
//		       dependencies. Fading is now done using the AnimateWindow() API.
//		       Hottracking is done by handling the NCHITTEST message.
//
//  5-Jan-2002 (JBa) - Added the HideBalloon() method which hides and destroys the 
//		       balloon and releases capture if we had it.
//
// 30-Dec-2001 (JBa) - Added XP theming. The only thing themed is the close button
//		       when using unSHOW_CLOSE_BUTTON.
//
// 11-Dec-2001 (JBa) - The control now uses a "ClsInfoControl" object to display
//		       the content text. This means support for colors and text
//		       styles.
//
// 12-Dec-2001 (JBa) - LaunchBalloon() now also checks to see if there already is a
//		       balloon active. If so it destroys it before displaying the
//		       new one. A value BHCOOKIE in the control handle it's userdata
//		       field is used to identify the balloon handles.
//
// 15-Dec-2001 (JBa) - Added the unCLOSE_ON_ACTIVATE option. Not to be used in combination
//		       with unCLOSE_ON_LBUTTON_UP and unCLOSE_ON_RBUTTON_UP.

#include "../../stdafx.h"

#include "BalloonHelp.h"
#include "../../gdi/all.h"
#include "../../tools/color.h"
#include "../../tools/multimonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// constants that may not be defined if you don't have the latest SDK
// (but i like to use them anyway)
//

#ifndef DFCS_HOT
#define DFCS_HOT 0x1000
#endif

#ifndef AW_HIDE
#define AW_HIDE 0x00010000
#define AW_BLEND 0x00080000
#endif

#ifndef CS_DROPSHADOW
#define CS_DROPSHADOW   0x00020000
#endif

#ifndef SPI_GETDROPSHADOW
#define SPI_GETDROPSHADOW  0x1024
#endif

#ifndef SPI_GETTOOLTIPANIMATION
#define SPI_GETTOOLTIPANIMATION 0x1016
#endif

#ifndef SPI_GETTOOLTIPFADE
#define SPI_GETTOOLTIPFADE 0x1018
#endif

#ifndef SPI_GETMOUSEHOVERWIDTH
#define SPI_GETMOUSEHOVERWIDTH 0x0062
#endif

// Magic cookie to recognize windows of
// this class.
//#define BHCOOKIE	0xDEADBEEF

/////////////////////////////////////////////////////////////////////////////
// CBalloonHelp

// option constants (bits)
const unsigned int   ClsBalloonHelp::unCLOSE_ON_LBUTTON_UP	= 0x00000001;
const unsigned int   ClsBalloonHelp::unCLOSE_ON_MBUTTON_UP	= 0x00000002;
const unsigned int   ClsBalloonHelp::unCLOSE_ON_RBUTTON_UP	= 0x00000004;
const unsigned int   ClsBalloonHelp::unCLOSE_ON_LBUTTON_DOWN	= 0x00000008;
const unsigned int   ClsBalloonHelp::unCLOSE_ON_MBUTTON_DOWN	= 0x00000010;
const unsigned int   ClsBalloonHelp::unCLOSE_ON_RBUTTON_DOWN	= 0x00000020;
const unsigned int   ClsBalloonHelp::unCLOSE_ON_MOUSE_MOVE	= 0x00000040;
const unsigned int   ClsBalloonHelp::unCLOSE_ON_KEYPRESS	= 0x00000080;
const unsigned int   ClsBalloonHelp::unCLOSE_ON_ANYTHING	= ClsBalloonHelp::unCLOSE_ON_LBUTTON_UP | ClsBalloonHelp::unCLOSE_ON_MBUTTON_UP | ClsBalloonHelp::unCLOSE_ON_RBUTTON_UP | ClsBalloonHelp::unCLOSE_ON_LBUTTON_DOWN | ClsBalloonHelp::unCLOSE_ON_MBUTTON_DOWN | ClsBalloonHelp::unCLOSE_ON_RBUTTON_DOWN | ClsBalloonHelp::unCLOSE_ON_MOUSE_MOVE | ClsBalloonHelp::unCLOSE_ON_KEYPRESS;
const unsigned int   ClsBalloonHelp::unDELAY_CLOSE              = 0x00000100;
const unsigned int   ClsBalloonHelp::unDELETE_THIS_ON_CLOSE	= 0x00000200;
const unsigned int   ClsBalloonHelp::unSHOW_CLOSE_BUTTON	= 0x00000400;
const unsigned int   ClsBalloonHelp::unSHOW_INNER_SHADOW	= 0x00000800;
const unsigned int   ClsBalloonHelp::unSHOW_TOPMOST		= 0x00001000;
const unsigned int   ClsBalloonHelp::unDISABLE_XP_SHADOW	= 0x00002000;
const unsigned int   ClsBalloonHelp::unDISABLE_FADEIN		= 0x00004000;
const unsigned int   ClsBalloonHelp::unDISABLE_FADEOUT		= 0x00008000;
const unsigned int   ClsBalloonHelp::unDISABLE_FADE		= ClsBalloonHelp::unDISABLE_FADEIN | ClsBalloonHelp::unDISABLE_FADEOUT;
const unsigned int   ClsBalloonHelp::unCLOSE_ON_ACTIVATE	= 0x00010000;

// layout constants (should prolly be configurable)
const int            ClsBalloonHelp::nTIP_TAIL             = 20;
const int            ClsBalloonHelp::nTIP_MARGIN           = 8;

// Kill timer
#define ID_TIMER_CLOSE  1

ClsBalloonHelp::ClsBalloonHelp() :  
	m_unOptions( 0 ),
	m_unTimeout( 0 ),
	m_unTimerClose( 0 ),
	m_strURL( _T( "" )),
	m_ptAnchor( 0, 0 ),
	m_hwndAnchor( NULL ),
	m_screenRect( 0, 0, 0, 0 ),
	m_strContent( _T( "")),
	m_nMouseMoveTolerance( 3 ),     // later retrieved from system
	m_ptMouseOrig( 0, 0 ),
	m_uCloseState( 0 ),
	m_pTitleFont( NULL ),
	m_pContentFont( NULL ),
	m_crBackground( ::GetSysColor( COLOR_INFOBK )),
	m_hIcon( NULL ),
	m_hKeyboardHook( NULL ),
	m_hMouseHook( NULL ),
	m_hCallWndRetHook( NULL ),
	m_hTheme( NULL )
{
	// get system tolerance values
	int nTol = 0;
	if ( ::SystemParametersInfo( SPI_GETMOUSEHOVERWIDTH, 0, &nTol, 0 ) && nTol > 0 )
		m_nMouseMoveTolerance = nTol;

	// setup hook procedures
	BHKeybHookThunk<ClsBalloonHelp>::InitThunk(( TMFP )&ClsBalloonHelp::KeyboardHookProc, this);
	BHMouseHookThunk<ClsBalloonHelp>::InitThunk(( TMFP )&ClsBalloonHelp::MouseHookProc, this);
	BHCallWndRetHookThunk<ClsBalloonHelp>::InitThunk(( TMFP )&ClsBalloonHelp::CallWndRetProc, this);
}

ClsBalloonHelp::~ClsBalloonHelp()
{
	if ( NULL != m_pTitleFont )
		delete m_pTitleFont;
	m_pTitleFont = NULL;
	if ( NULL != m_pContentFont )
		delete m_pContentFont;
	m_pContentFont = NULL;
	
	// Destroy theme.
	if ( m_hTheme ) ThemingAPI.CloseThemeData( m_hTheme );
}


// Sets the font used for drawing the balloon title.  Deleted by balloon, do not use CFont* after passing to this function.
void ClsBalloonHelp::SetTitleFont( ClsFont* pFont )
{
	if ( NULL != m_pTitleFont )
		delete m_pTitleFont;
	m_pTitleFont = pFont;
	// if already visible, resize & move
	if ( NULL != m_hWnd )
		PositionWindow();
}

// Sets the font used for drawing the balloon content.  Deleted by balloon, do not use CFont* after passing to this function.
void ClsBalloonHelp::SetContentFont( ClsFont* pFont )
{
	if ( NULL != m_pContentFont )
		delete m_pContentFont;
	m_pContentFont = pFont;
	// if already visible, resize & move
	if ( NULL != m_hWnd )
		PositionWindow();
}

// Sets the icon displayed in the top left of the balloon (pass NULL to hide icon)
void ClsBalloonHelp::SetIcon( HICON hIcon )
{
	if ( NULL != ( HIMAGELIST )m_ilIcon )
		m_ilIcon.Destroy();
	ICONINFO iconinfo;
	if ( NULL != hIcon && ::GetIconInfo( hIcon, &iconinfo ))
	{
		SetIcon( iconinfo.hbmColor, iconinfo.hbmMask );
		::DeleteObject( iconinfo.hbmColor );
		::DeleteObject( iconinfo.hbmMask );
	}
	// if already visible, resize & move (icon size may have changed)
	if ( NULL != m_hWnd )
		PositionWindow();
}

// Sets the icon displayed in the top left of the balloon (pass NULL to hide icon)
void ClsBalloonHelp::SetIconScaled( HICON hIcon, int cx, int cy )
{
	// i now have two device contexts and two bitmaps.
	// i will select a bitmap in each device context,
	// draw the icon into the first one,
	// scale it into the second one,
	// and set the second one as the balloon icon.
	// This is a rather long process to get a scaled icon,
	// but ensures maximum compatibility between different
	// versions of Windows, while producing the best possible
	// results on each version (quite good in WinNT and better, sorta ok in Win9x).
	ICONINFO iconinfo;
	if ( NULL != hIcon && ::GetIconInfo( hIcon, &iconinfo ))
	{
		// Wrap returned bitmaps.
		ClsBitmap bmpColor; 
		ClsBitmap bmpMask; 
		bmpColor.Attach( iconinfo.hbmColor );
		bmpMask.Attach( iconinfo.hbmMask );

		// Get bitmap information of the color planes.
		BITMAP bm;
		if ( bmpColor.GetBitmap( &bm ))
		{
			ClsBitmap bmpIcon;
			ClsBitmap bmpIconScaled;

			// Get screen DC.
			ClsGetDC dc( NULL );
			if ( dc.IsValid())
			{
				// Create original size DC.
				ClsDC dcTmp1;
				if ( dcTmp1.CreateCompatibleDC( &dc ))
				{
					// And the scaled size DC.
					ClsDC dcTmp2;
					if ( dcTmp2.CreateCompatibleDC( &dc ))
					{
						// Also create the bitmaps.
						ClsBitmap bmpIcon, bmpIconScaled;
						if ( bmpIcon.CreateCompatibleBitmap( dc, bm.bmWidth, bm.bmHeight ) &&
						     bmpIconScaled.CreateCompatibleBitmap( dc, cx, cy ))
						{
							// Code is braced so the ClsSelector object go out of
							// scope at the appropiate time...
							{
								// Select bitmaps.
								ClsSelector sel1( &dcTmp1, bmpIcon );
								ClsSelector sel2( &dcTmp2, bmpIconScaled );

								// Draw original icon.
								dcTmp1.FillSolidRect( 0, 0, bm.bmWidth, bm.bmHeight, m_crBackground );
								dcTmp1.DrawIconEx( 0, 0, hIcon, bm.bmWidth, bm.bmHeight, 0, NULL, DI_NORMAL );

								// And the scaled icon.
								if ( ClsGetApp()->GetPlatformID() == VER_PLATFORM_WIN32_NT ) dcTmp2.SetStretchBltMode( HALFTONE );
								dcTmp2.StretchBlt( 0, 0, cx, cy, &dcTmp1, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY );
							}
							// Setup the resulting icon.
							SetIcon( bmpIconScaled, m_crBackground );
						}
						// Delete dc2
						dcTmp2.DeleteDC();
					}
					// Delete dc1
					dcTmp1.DeleteDC();
				}
			}
		}
	}
}

// Sets the icon displayed in the top left of the balloon (pass NULL hBitmap to hide icon)
void ClsBalloonHelp::SetIcon( HBITMAP hBitmap, COLORREF crMask )
{
	if ( NULL != ( HIMAGELIST )m_ilIcon )
		m_ilIcon.Destroy();

	if ( NULL != hBitmap )
	{
		BITMAP bm;
		if ( ::GetObject( hBitmap, sizeof( bm ), ( LPVOID )&bm ))
		{
			if ( m_ilIcon.Create( bm.bmWidth, bm.bmHeight, ILC_COLOR24 | ILC_MASK, 1, 0 ))
				m_ilIcon.AddMasked( hBitmap, crMask );
		}
	}
	// if already visible, resize & move (icon size may have changed)
	if ( NULL != m_hWnd )
		PositionWindow();
}

// Sets the icon displayed in the top left of the balloon
void ClsBalloonHelp::SetIcon( HBITMAP hBitmap, HBITMAP hMask )
{
	if ( NULL != ( HIMAGELIST )m_ilIcon )
		m_ilIcon.Destroy();

	_ASSERT_VALID( hBitmap );
	_ASSERT_VALID( hMask );

	BITMAP bm;
	if ( ::GetObject( hBitmap, sizeof( bm ), ( LPVOID )&bm ))
	{
		if ( m_ilIcon.Create( bm.bmWidth, bm.bmHeight, ILC_COLOR24 | ILC_MASK, 1, 0 ))
			m_ilIcon.Add( hBitmap, hMask );
	}
	// if already visible, resize & move (icon size may have changed)
	if ( NULL != m_hWnd )
		PositionWindow();
}

// Set icon displayed in the top left of the balloon to image # nIconIndex from pImageList
void ClsBalloonHelp::SetIcon( ClsImageList* pImageList, int nIconIndex )
{
	// sanity checks
	_ASSERT_VALID( pImageList );
	_ASSERT( nIconIndex >= 0 && nIconIndex < pImageList->GetImageCount());

	HICON hIcon = NULL;
	if ( NULL != pImageList && nIconIndex >= 0 && nIconIndex < pImageList->GetImageCount())
		hIcon = pImageList->ExtractIcon( nIconIndex );
	SetIcon( hIcon );
	if ( NULL != hIcon )
		::DestroyIcon( hIcon );
	// if already visible, resize & move (icon size may have changed)
	if ( NULL != m_hWnd )
		PositionWindow();
}

// Sets the URL to be opened when balloon is clicked.  Pass "" to disable.
void ClsBalloonHelp::SetURL( const ClsString& strURL )
{
	m_strURL = strURL;
}

// Sets the number of milliseconds the balloon can remain open.  Set to 0 to disable timeout.
void ClsBalloonHelp::SetTimeout( unsigned int unTimeout )
{
	m_unTimeout = unTimeout;
	// if timer is already set, reset.
	if ( NULL != m_hWnd )
	{
		if ( m_unTimeout )
		{
			m_unTimerClose = SetTimer( ID_TIMER_CLOSE, m_unTimeout, NULL );
		}
		else
		{
			KillTimer( m_unTimerClose );
		}
	}
}

// Sets the point to which the balloon is "anchored"
void ClsBalloonHelp::SetAnchorPoint( ClsPoint ptAnchor, ClsWindow *pWndAnchor /* = NULL */ )
{
	m_ptAnchor = ptAnchor;
	if ( pWndAnchor ) m_hwndAnchor = pWndAnchor->GetSafeHWND();

	// if we're anchored to a window, set hook
	if ( NULL != m_hwndAnchor )
		SetCallWndRetHook();
	else
		RemoveCallWndRetHook();

	// if already visible, move
	if ( NULL != m_hWnd )
	{
		// reposition
		PositionWindow();
	}
}

// Sets the title of the balloon
void ClsBalloonHelp::SetTitle( const ClsString& strTitle )
{
	SetWindowText( strTitle );
	// if already visible, resize & move
	if ( NULL != m_hWnd )
		PositionWindow();
}

// Sets the content of the balloon (plain text only)
void ClsBalloonHelp::SetContent( const ClsString& strContent )
{
	m_strContent = strContent;
	// if already visible, resize & move
	if ( NULL != m_hWnd )
		PositionWindow();
}

// Sets the background color of the balloon
void ClsBalloonHelp::SetBackgroundColor( COLORREF crBackground )
{
	m_crBackground = crBackground;
	// repaint if visible
	if ( NULL != m_hWnd )
		Invalidate( FALSE );
}

// Sets the distance the mouse must move before the balloon closes when the unCLOSE_ON_MOUSE_MOVE option is set.
void ClsBalloonHelp::SetMouseMoveTolerance( int nTolerance )
{
	m_nMouseMoveTolerance = nTolerance;
}

//
// creates a new balloon window
// Parameters:
//    strTitle    |  Title of balloon
//    strContent  |  Content of balloon
//    ptAnchor    |  point tail of balloon will be "anchor"ed to
//    unOptions   |  One or more of:
//                :     unCLOSE_ON_LBUTTON_UP   |  closes window on WM_LBUTTON_UP
//                :     unCLOSE_ON_MBUTTON_UP   |  closes window on WM_MBUTTON_UP
//                :     unCLOSE_ON_RBUTTON_UP   |  closes window on WM_RBUTTON_UP
//                :     unCLOSE_ON_LBUTTON_DOWN |  closes window on WM_LBUTTON_DOWN
//                :     unCLOSE_ON_MBUTTON_DOWN |  closes window on WM_MBUTTON_DOWN
//                :     unCLOSE_ON_RBUTTON_DOWN |  closes window on WM_RBUTTON_DOWN
//                :     unCLOSE_ON_MOUSE_MOVE   |  closes window when user moves mouse past threshhold
//                :     unCLOSE_ON_KEYPRESS     |  closes window on the next keypress message sent to this thread.
//                :     unCLOSE_ON_ANYTHING     |  all of the above.
//                :     unDELAY_CLOSE           |  when a user action triggers the close, begins timer.  closes when timer expires.
//                :     unDELETE_THIS_ON_CLOSE  |  deletes object when window is closed.  Used by LaunchBalloon(), use with care
//                :     unSHOW_CLOSE_BUTTON     |  shows close button in upper right
//                :     unSHOW_INNER_SHADOW     |  draw inner shadow in balloon
//                :     unSHOW_TOPMOST          |  place balloon above all other windows
//                :     unDISABLE_XP_SHADOW     |  disable Windows XP's drop-shadow effect (overrides system and user settings)
//                :     unDISABLE_FADE          |  disable the fade-in/fade-out effects (overrides system and user settings)
//                :     unDISABLE_FADEIN        |  disable the fade-in effect
//                :     unDISABLE_FADEOUT       |  disable the fade-out effect
//    pParentWnd  |  Parent window.  If NULL will be set to AfxGetMainWnd()
//    strURL      |  If not empty, when the balloon is clicked ShellExecute() will
//                |  be called, with strURL passed in.
//    unTimeout   |  If not 0, balloon will automatically close after unTimeout milliseconds.
//    hIcon       |  If not NULL, the icon indicated by hIcon will be displayed at top-left of the balloon.
//
// Returns:
//    TRUE if successful, else FALSE
//
BOOL ClsBalloonHelp::Create( const ClsString& strTitle, const ClsString& strContent, 
			     const ClsPoint& ptAnchor, unsigned int unOptions,
			     ClsWindow* pParentWnd /* =NULL */,
			     const ClsString strURL /* = _T( "" ) */,
			     unsigned int unTimeout /* = 0 */,
			     HICON hIcon /* = NULL */ )
{
	m_strContent   = strContent;
	SetAnchorPoint( ptAnchor, pParentWnd );
	m_unOptions    = unOptions;
	m_strURL       = strURL;
	m_unTimeout    = unTimeout;

	if ( NULL != hIcon )
		SetIcon( hIcon );
   
	if ( NULL == pParentWnd )
		return FALSE;

	// Close open popups...
	ClosePopups();

	// if no fonts set, use defaults
	if ( NULL == m_pContentFont )
	{
		m_pContentFont = new ClsFont;
		if ( ! m_pContentFont->GetStockFont( DEFAULT_GUI_FONT ))
			return FALSE;
	}

	// title font defaults to bold version of content font
	if ( NULL == m_pTitleFont )
	{
		m_pTitleFont = new ClsFont;
		LOGFONT LogFont;
		m_pContentFont->GetLogFont( &LogFont );
		LogFont.lfWeight = FW_BOLD;
		if ( ! m_pTitleFont->CreateFontIndirect( &LogFont ))
			return FALSE;
	}

	// check system settings: if fade effects are disabled or unavailable, disable here too
	BOOL bFade = FALSE;
	::SystemParametersInfo( SPI_GETTOOLTIPANIMATION, 0, &bFade, 0 );
	if ( bFade )
		::SystemParametersInfo( SPI_GETTOOLTIPFADE, 0, &bFade, 0 );
	if ( ! bFade )
		m_unOptions |= unDISABLE_FADE;

	// create invisible at arbitrary position; then position, set region, and finally show

	// the idea with WS_EX_TOOLWINDOW is, you can't switch to this using alt+tab
	// create invisible at arbitrary position; then position, set region, and finally show
	DWORD dwExStyle = WS_EX_TOOLWINDOW;
	if ( m_unOptions & unSHOW_TOPMOST )      // make topmost, if requested
		dwExStyle |= WS_EX_TOPMOST;
	if ( ! ClsWindow::Create( dwExStyle, ( m_unOptions & unDISABLE_XP_SHADOW ) ? _T( "ClsWindowClass" ) : _T( "ClsDSWindowClass" ), strTitle, WS_POPUP, ClsRect( 0, 0, 10, 10 ), pParentWnd->GetSafeHWND(), ( HMENU )NULL ))
		return FALSE;

	// JBa
	// Set cookie and create the info control. This replaces the content rendering
	// code.
	//SetWindowLongPtr( GWL_USERDATA, ( LONG_PTR )BHCOOKIE );
	if ( ! m_Body.Create( this, ClsRect( 0, 0, 0, 0 ), 0, strContent ))
	{
		// Destroy the object and return FALSE...
		Destroy();
		return FALSE;
	}

	// Setup the info control background color.
	m_Body.BackgroundColor() = m_crBackground;
	PositionWindow();

	if (( m_unOptions & unCLOSE_ON_MOUSE_MOVE   ) ||
	    ( m_unOptions & unCLOSE_ON_LBUTTON_UP   ) ||
	    ( m_unOptions & unCLOSE_ON_LBUTTON_DOWN ) ||
            ( m_unOptions & unCLOSE_ON_MBUTTON_UP   ) ||
            ( m_unOptions & unCLOSE_ON_MBUTTON_DOWN ) ||
            ( m_unOptions & unCLOSE_ON_RBUTTON_UP   ) ||
            ( m_unOptions & unCLOSE_ON_RBUTTON_DOWN ))
	{
		::GetCursorPos( m_ptMouseOrig );
		SetMouseHook();
	}

	// these need to take effect even if the window receiving them
	// is not owned by this process.  So, if this process does not
	// already have the mouse captured, capture it!
	if (( m_unOptions & unCLOSE_ON_LBUTTON_UP ) ||
	    ( m_unOptions & unCLOSE_ON_MBUTTON_UP ) ||
	    ( m_unOptions & unCLOSE_ON_RBUTTON_UP ))
	{
		// no, i don't particularly need or want to deal with a situation
		// where a balloon is being created and another program has captured
		// the mouse.  If you need it, it shouldn't be too hard, just do it here.
		if ( NULL == GetCapture())
			SetCapture();
	}

	if ( m_unOptions&unCLOSE_ON_KEYPRESS )
		SetKeyboardHook();

	ShowBalloon();
	return TRUE;
}

// calculate anchor position (adjust for client coordinates if used)
ClsPoint ClsBalloonHelp::GetAnchorPoint()
{
	ClsPoint ptAnchor = m_ptAnchor;
	// assume if window was given, point is in client coords
	if ( NULL != m_hwndAnchor )
		::ClientToScreen( m_hwndAnchor, ptAnchor );
	return ptAnchor;
}


// determine bounds of screen anchor is on (Multi-Monitor compatibility)
void ClsBalloonHelp::GetAnchorScreenBounds( ClsRect& rect )
{
	if ( m_screenRect.IsEmpty())
	{     
		// get the nearest monitor to the anchor
		ClsMultiMon mon;
		int nMonitor;
		mon.MonitorNumberFromPoint( GetAnchorPoint(), MONITOR_DEFAULTTONEAREST, nMonitor );
		mon.GetMonitorRect( nMonitor, m_screenRect, TRUE );
	}
	rect = m_screenRect;
}

// Displays the balloon on the screen, performing fade-in if enabled.
void ClsBalloonHelp::ShowBalloon()
{
	ShowWindow( SW_SHOWNOACTIVATE );
	if ( ! ( m_unOptions & unDELAY_CLOSE ))
		SetTimeout( m_unTimeout );     // start close timer
}

// Removes the balloon from the screen, performing the fade-out if enabled
void ClsBalloonHelp::HideBalloon()
{
	if ( m_unOptions & unDELAY_CLOSE )
	{
		m_unOptions &= ~( unDELAY_CLOSE | unCLOSE_ON_ANYTHING );  // close only via timer or button
		SetTimeout( m_unTimeout );     // start close timer
		return;
	}

	// Hide the window, release capture if we have it and
	// destroy the window.
	ShowWindow( SW_HIDE );
	if ( GetCapture() == this ) ::ReleaseCapture();
	Destroy();
}

// calculates the area of the screen the balloon falls into
// this determins which direction the tail points
ClsBalloonHelp::BALLOON_QUADRANT ClsBalloonHelp::GetBalloonQuadrant()
{
	ClsRect rectDesktop;
	GetAnchorScreenBounds( rectDesktop );
	ClsPoint ptAnchor = GetAnchorPoint();

	if ( ptAnchor.Y() < rectDesktop.Top() + rectDesktop.Height() / 2 )
	{
		if ( ptAnchor.X() < rectDesktop.Left() + rectDesktop.Width() / 2 )
		{
			return BQ_TOPLEFT;
		}
		else
		{
			return BQ_TOPRIGHT;
		}
	}
	else
	{
		if ( ptAnchor.X() < rectDesktop.Left() + rectDesktop.Width() / 2 )
		{
			return BQ_BOTTOMLEFT;
		}
		else
		{
			return BQ_BOTTOMRIGHT;
		}
	}
   // unreachable
}

// calculates the client size necessary based on content
ClsSize ClsBalloonHelp::CalcClientSize()
{
	_ASSERT_VALID( GetSafeHWND());
	ClsWindowDC dc( this );

	// JBa
	// Set the font to use to the info control.
	m_Body.SetFont( m_pContentFont );

	ClsSize sizeHeader = CalcHeaderSize( &dc );
	ClsSize sizeContent( m_Body.TotalWidth( &dc, m_strContent ), m_Body.TotalHeight( &dc, m_strContent ));
	
	// Move contents.
	m_Body.MoveWindow( 0, sizeHeader.CY() + nTIP_MARGIN, max( sizeHeader.CX(), sizeContent.CX()), sizeContent.CY());

	return ClsSize( max( sizeHeader.CX(), sizeContent.CX()), sizeHeader.CY() + nTIP_MARGIN + sizeContent.CY());
}

// calculates the size for the entire window based on content size
ClsSize ClsBalloonHelp::CalcWindowSize()
{
	ClsSize size = CalcClientSize();
	size.CX() += nTIP_MARGIN * 2;
	size.CY() += nTIP_TAIL + nTIP_MARGIN * 2;
	//size.CX() = max( size.CX(), nTIP_MARGIN * 2 + nTIP_TAIL * 4 );
	return size;
}

// this routine calculates the size and position of the window relative
// to it's anchor point, and moves the window accordingly.  The region is also
// created and set here.
void ClsBalloonHelp::PositionWindow()
{
	ClsSize sizeWnd = CalcWindowSize();

	POINT    ptTail[ 3 ];
	ClsPoint ptTopLeft( 0, 0 );
	ClsPoint ptBottomRight( sizeWnd.CX(), sizeWnd.CY());

	// force recalculation of desktop
	m_screenRect.Empty();

	switch ( GetBalloonQuadrant())
	{
		case BQ_TOPLEFT:
			ptTopLeft.Y() = nTIP_TAIL;
			ptTail[ 0 ].x = ( sizeWnd.CX() - nTIP_TAIL ) / 4 + nTIP_TAIL;
			ptTail[ 0 ].y = nTIP_TAIL + 1;
			ptTail[ 2 ].x = ( sizeWnd.CX() - nTIP_TAIL ) / 4;
			ptTail[ 2 ].y = ptTail[ 0 ].y;
			ptTail[ 1 ].x = ptTail[ 2 ].x;
			ptTail[ 1 ].y = 1;
			break;

		case BQ_TOPRIGHT:
			ptTopLeft.Y() = nTIP_TAIL;
			ptTail[ 0 ].x = ( sizeWnd.CX() - nTIP_TAIL ) / 4 * 3;
			ptTail[ 0 ].y = nTIP_TAIL + 1;
			ptTail[ 2 ].x = ( sizeWnd.CX() - nTIP_TAIL ) / 4 * 3 + nTIP_TAIL;
			ptTail[ 2 ].y = ptTail[ 0 ].y;
			ptTail[ 1 ].x = ptTail[ 2 ].x;
			ptTail[ 1 ].y = 1;
			break;

		case BQ_BOTTOMLEFT:
			ptBottomRight.Y() = sizeWnd.CY() - nTIP_TAIL;
			ptTail[ 0 ].x = ( sizeWnd.CX() - nTIP_TAIL ) / 4 + nTIP_TAIL;
			ptTail[ 0 ].y = sizeWnd.CY() - nTIP_TAIL - 2;
			ptTail[ 2 ].x = ( sizeWnd.CX() - nTIP_TAIL ) / 4;
			ptTail[ 2 ].y = ptTail[ 0 ].y;
			ptTail[ 1 ].x = ptTail[ 2 ].x;
			ptTail[ 1 ].y = sizeWnd.CY() - 2;
			break;

		case BQ_BOTTOMRIGHT:
			ptBottomRight.Y() = sizeWnd.CY() - nTIP_TAIL;
			ptTail[ 0 ].x = ( sizeWnd.CX() - nTIP_TAIL ) / 4 * 3;
			ptTail[ 0 ].y = sizeWnd.CY() - nTIP_TAIL - 2;
			ptTail[ 2 ].x = ( sizeWnd.CX() - nTIP_TAIL ) / 4 * 3 + nTIP_TAIL;
			ptTail[ 2 ].y = ptTail[ 0 ].y;
			ptTail[ 1 ].x = ptTail[ 2 ].x;
			ptTail[ 1 ].y = sizeWnd.CY() - 2;
			break;
	}

	// adjust for very narrow balloons
	if ( ptTail[ 0 ].x < nTIP_MARGIN )
		ptTail[ 0 ].x = nTIP_MARGIN;
	if ( ptTail[ 0 ].x > sizeWnd.CX() - nTIP_MARGIN )
		ptTail[ 0 ].x = sizeWnd.CX() - nTIP_MARGIN;
	if ( ptTail[ 1 ].x < nTIP_MARGIN )
		ptTail[ 1 ].x = nTIP_MARGIN;
	if ( ptTail[ 1 ].x > sizeWnd.CX() - nTIP_MARGIN )
		ptTail[ 1 ].x = sizeWnd.CX() - nTIP_MARGIN;
	if ( ptTail[ 2 ].x < nTIP_MARGIN )
		ptTail[ 2 ].x = nTIP_MARGIN;
	if ( ptTail[ 2 ].x > sizeWnd.CX() - nTIP_MARGIN )
		ptTail[ 2 ].x = sizeWnd.CX() - nTIP_MARGIN;

	// get window position
	ClsPoint ptAnchor = GetAnchorPoint();
	ClsPoint ptOffs( ptAnchor.X() - ptTail[ 1 ].x, ptAnchor.Y() - ptTail[ 1 ].y );

	// adjust position so all is visible
	ClsRect rectScreen;
	GetAnchorScreenBounds( rectScreen );
	int nAdjustX = 0;
	int nAdjustY = 0;
	if ( ptOffs.X() < rectScreen.Left())
		nAdjustX = rectScreen.Left() - ptOffs.X();
	else if ( ptOffs.X() + sizeWnd.CX() >= rectScreen.Right())
		nAdjustX = rectScreen.Right() - ( ptOffs.X() + sizeWnd.CX());
	if ( ptOffs.Y() + nTIP_TAIL < rectScreen.Top())
		nAdjustY = rectScreen.Top() - ( ptOffs.Y() + nTIP_TAIL );
	else if ( ptOffs.Y() + sizeWnd.CY() - nTIP_TAIL >= rectScreen.Bottom())
		nAdjustY = rectScreen.Bottom() - ( ptOffs.Y() + sizeWnd.CY() - nTIP_TAIL );

	// reposition tail
	// uncomment two commented lines below to move entire tail 
	// instead of just anchor point

	//ptTail[ 0 ].x -= nAdjustX;
	ptTail[ 1 ].x -= nAdjustX;
	//ptTail[ 2 ].x -= nAdjustX;
	ptOffs.X()    += nAdjustX;
	ptOffs.Y()    += nAdjustY;

	// place window
	MoveWindow( ptOffs.X(), ptOffs.Y(), sizeWnd.CX(), sizeWnd.CY(), TRUE );

	// apply region
	ClsRgn region;
	ClsRgn regionRound;
	ClsRgn regionComplete;
	region.CreatePolygonRgn( &ptTail[ 0 ], 3, ALTERNATE );
	regionRound.CreateRoundRectRgn( ptTopLeft.X(), ptTopLeft.Y(), ptBottomRight.X(), ptBottomRight.Y(), nTIP_MARGIN * 2, nTIP_MARGIN * 2 );
	regionComplete.CreateRectRgn( 0, 0, 1, 1 );
	regionComplete.CombineRgn( &regionRound, &region, RGN_OR );
	//regionComplete.CopyRgn( &region );
	if ( NULL == ( HGDIOBJ )m_rgnComplete )
		m_rgnComplete.CreateRectRgn( 0, 0, 1, 1 );
	
	if ( !m_rgnComplete.EqualRgn(&regionComplete) )
	{
		m_rgnComplete.CopyRgn( &regionComplete );
		SetWindowRgn(( HRGN )regionComplete.Detach(), TRUE );
		// There is a bug with layered windows and NC changes in Win2k
		// As a workaround, redraw the entire window if the NC area changed.
		// Changing the anchor point is the ONLY thing that will change the
		// position of the client area relative to the window during normal
		// operation.
		RedrawWindow(NULL, NULL, RDW_UPDATENOW| RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
	}
}

//
// Show a help balloon on screen
// Parameters:
//    strTitle    |  Title of balloon
//    strContent  |  Content of balloon
//    ptAnchor    |  point tail of balloon will be "anchor"ed to
//    szIcon      |  One of:
//                   IDI_APPLICATION
//                   IDI_INFORMATION IDI_ASTERISK (same)
//                   IDI_ERROR IDI_HAND (same)
//                   IDI_EXCLAMATION IDI_WARNING (same)
//                   IDI_QUESTION
//                   IDI_WINLOGO
//		     NULL (no icon)
//    unIconID    |  ID of icon to display (loaded from resources)
//                :     unCLOSE_ON_LBUTTON_UP   |  closes window on WM_LBUTTON_UP
//                :     unCLOSE_ON_MBUTTON_UP   |  closes window on WM_MBUTTON_UP
//                :     unCLOSE_ON_RBUTTON_UP   |  closes window on WM_RBUTTON_UP
//                :     unCLOSE_ON_LBUTTON_DOWN |  closes window on WM_LBUTTON_DOWN
//                :     unCLOSE_ON_MBUTTON_DOWN |  closes window on WM_MBUTTON_DOWN
//                :     unCLOSE_ON_RBUTTON_DOWN |  closes window on WM_RBUTTON_DOWN
//                :     unCLOSE_ON_MOUSE_MOVE   |  closes window when user moves mouse past threshhold
//                :     unCLOSE_ON_KEYPRESS     |  closes window on the next keypress message sent to this thread.
//                :     unCLOSE_ON_ANYTHING     |  all of the above.
//                :     unDELAY_CLOSE           |  when a user action triggers the close, begins timer.  closes when timer expires.
//                :     unSHOW_CLOSE_BUTTON     |  shows close button in upper right
//                :     unSHOW_INNER_SHADOW     |  draw inner shadow in balloon
//                :     unSHOW_TOPMOST          |  place balloon above all other windows
//                :     unDISABLE_XP_SHADOW     |  disable Windows XP's drop-shadow effect (overrides system and user settings)
//                :     unDISABLE_FADE          |  disable the fade-in/fade-out effects (overrides system and user settings)
//                :     unDISABLE_FADEIN        |  disable the fade-in effect
//                :     unDISABLE_FADEOUT       |  disable the fade-out effect
//    pParentWnd  |  Parent window.  If NULL will be set to AfxGetMainWnd()
//    strURL      |  If not empty, when the balloon is clicked ShellExecute() will
//                |  be called, with strURL passed in.
//    unTimeout   |  If not 0, balloon will automatically close after unTimeout milliseconds.
//
void ClsBalloonHelp::LaunchBalloon( const ClsString& strTitle, const ClsString& strContent, 
				    const ClsPoint& ptAnchor, 
				    LPCTSTR szIcon /* = IDI_EXCLAMATION */,
				    unsigned int unOptions /* = unSHOW_CLOSE_BUTTON */,
				    ClsWindow* pParentWnd /* = NULL */,
				    const ClsString strURL /* = _T( "" ) */,
				    unsigned int unTimeout /* = 10000 */ )
{
	ClsBalloonHelp* pbh = new ClsBalloonHelp;
	if ( NULL != szIcon )
	{
		ClsSize sizeIcon( max( GetCloseButtonWidth(), ::GetSystemMetrics( SM_CXSMICON )), max( GetCloseButtonHeight(), ::GetSystemMetrics( SM_CYSMICON )));
		HICON hIcon = ( HICON )::LoadImage( NULL, szIcon, IMAGE_ICON, sizeIcon.CX(), sizeIcon.CY(), LR_SHARED );
		if ( NULL != hIcon )
			pbh->SetIconScaled( hIcon, sizeIcon.CX(), sizeIcon.CY());
	}
	if ( pbh->Create( strTitle, strContent, ptAnchor, unOptions | unDELETE_THIS_ON_CLOSE, 
			  pParentWnd, strURL, unTimeout, NULL ) == FALSE )
		delete pbh;
}


LRESULT ClsBalloonHelp::OnEraseBkgnd( ClsDC* pDC ) 
{
	pDC->FillRect( GetClientRect(), ( HBRUSH )( COLOR_INFOBK + 1 ));
	return TRUE;
}

void ClsBalloonHelp::DrawClientArea( ClsDC *pDC )
{
	ClsSize sizeHeader = DrawHeader( pDC );
}

// Calculate the dimensions and draw the balloon header
ClsSize ClsBalloonHelp::DrawHeader( ClsDC* pDC, bool bDraw )
{
	ClsSize sizeHdr( 0, 0 );
	ClsRect rectClient;
	GetClientRect( rectClient );   // use this for positioning when drawing
					// else if content is wider than title, centering wouldn't work

	// calc & draw icon
	if ( NULL != ( HIMAGELIST )m_ilIcon )
	{
		int x = 0;
		int y = 0;
		m_ilIcon.GetIconSize( x, y );
		sizeHdr.CX() += x;
		sizeHdr.CY() = max( sizeHdr.CY(), y );
		m_ilIcon.SetBkColor( m_crBackground );
		if ( bDraw )
			m_ilIcon.Draw( 0, *pDC, 0, 0, ILD_NORMAL );//ILD_TRANSPARENT);
		rectClient.Left() += x;
	}

	// calc & draw close button
	if ( m_unOptions & unSHOW_CLOSE_BUTTON )
	{
		int nBtnWidth = GetCloseButtonWidth();
		// if something is already in the header (icon) leave space
		if ( sizeHdr.CX() > 0 )
			sizeHdr.CX() += nTIP_MARGIN;
		sizeHdr.CX() += nBtnWidth;
		sizeHdr.CY() = max( sizeHdr.CY(), GetCloseButtonHeight());
		if ( bDraw )
		{
			ClsRect rcClose( rectClient.Right() - nBtnWidth, 0, rectClient.Right(), GetCloseButtonHeight());
			if ( ! m_hTheme ) pDC->DrawFrameControl( rcClose, DFC_CAPTION, DFCS_CAPTIONCLOSE | DFCS_FLAT );
			else		  ThemingAPI.DrawThemeBackground( m_hTheme, *pDC, TTP_CLOSE, TTCS_NORMAL, rcClose, NULL );
		}
		rectClient.Right() -= nBtnWidth;
	}

	// calc title size
	ClsString strTitle;
	GetWindowText( strTitle );
	if ( ! strTitle.IsEmpty())
	{
		ClsSelector sel1( pDC, m_pTitleFont );

		// if something is already in the header (icon or close button) leave space
		if ( sizeHdr.CX() > 0 ) 
			sizeHdr.CX() += nTIP_MARGIN;
		ClsRect rectTitle( 0, 0, 0, 0 );
		pDC->DrawText( strTitle, rectTitle, DT_CALCRECT | DT_NOPREFIX | DT_EXPANDTABS | DT_SINGLELINE );
		sizeHdr.CX() += rectTitle.Width();
		sizeHdr.CY() = max( sizeHdr.CY(), rectTitle.Height());

		// draw title
		if ( bDraw )
		{
			pDC->SetBkMode( TRANSPARENT );
			pDC->SetTextColor( ::GetSysColor( COLOR_INFOTEXT ));
			pDC->DrawText( strTitle, rectClient, DT_CENTER | DT_NOPREFIX  | DT_EXPANDTABS | DT_SINGLELINE );
		}
	}
	return sizeHdr;
}

LRESULT ClsBalloonHelp::OnPaint( ClsDC *pDC )
{
	// Theming?
	if ( ThemingAPI.IsThemingEnabled() && ! m_hTheme )
		m_hTheme = ThemingAPI.OpenThemeData( GetSafeHWND(), L"TOOLTIP" );

	if ( pDC ) DrawClientArea( pDC );
	else
	{
		ClsPaintDC dc( this );
		DrawClientArea( &dc );
	}
	return 0;
}

void ClsBalloonHelp::PostNcDestroy()
{
	ClsWindow::PostNcDestroy();

	// be careful with this one :D
	if ( m_unOptions & unDELETE_THIS_ON_CLOSE )
		delete this;
}

void ClsBalloonHelp::DrawNonClientArea( ClsDC *pDC )
{
	ClsRect rect = GetWindowRect();
	ScreenToClient( rect );
	ClsRect rectClient = GetClientRect();
	rectClient.Offset( -rect.Left(), -rect.Top());
	rect.Offset( -rect.Left(), -rect.Top());
	//pDC->ExcludeClipRect( rectClient );
	pDC->FillSolidRect( rect, m_crBackground );

	_ASSERT( NULL != ( HRGN )m_rgnComplete );
	ClsBrush brushFg;
	brushFg.CreateSysColorBrush( COLOR_INFOTEXT );
	if ( m_unOptions & unSHOW_INNER_SHADOW )
	{
		ClsBrush   brushHL;
		// standard colors
		COLORREF crFG = ::GetSysColor( COLOR_INFOTEXT );
		// slightly lighter color
		char red = ( char )( 170 + GetRValue( m_crBackground ) / 3 );
		char green = ( char )( 170 + GetGValue( m_crBackground ) / 3 );
		char blue = ( char )( 170 + GetBValue( m_crBackground ) / 3 );
		brushHL.CreateSolidBrush( RGB( red, green, blue ));
		m_rgnComplete.OffsetRgn( 1, 1 );
		pDC->FrameRgn( &m_rgnComplete, &brushHL, 2, 2 );

		// slightly darker color
		red = ( char )( GetRValue( crFG ) / 3 + GetRValue( m_crBackground ) / 3 * 2 );
		green = ( char )( GetGValue( crFG ) / 3 + GetGValue( m_crBackground ) / 3 * 2 );
		blue = ( char )( GetBValue( crFG ) / 3 + GetBValue( m_crBackground ) / 3 * 2 );
		brushHL.Delete();
		m_rgnComplete.OffsetRgn( -2, -2 );
		brushHL.CreateSolidBrush( RGB( red, green, blue ));
		pDC->FrameRgn( &m_rgnComplete, &brushHL, 2, 2 );
		m_rgnComplete.OffsetRgn( 1, 1 );
	}
	// outline
	pDC->FrameRgn( &m_rgnComplete, &brushFg, 1, 1 );
	Invalidate();
}

// Sets up the keyboard hook
void ClsBalloonHelp::SetKeyboardHook()
{
	// if hook is not yet set, set
	if ( NULL == m_hKeyboardHook )
		m_hKeyboardHook = ::SetWindowsHookEx( WH_KEYBOARD, ( HOOKPROC )BHKeybHookThunk<ClsBalloonHelp>::GetThunk(), NULL, ::GetCurrentThreadId());
}

// removes hook.
void ClsBalloonHelp::RemoveKeyboardHook()
{
	if ( NULL != m_hKeyboardHook )
	{
		::UnhookWindowsHookEx( m_hKeyboardHook );
		m_hKeyboardHook = NULL;
	}
}

//
// Mouse hook
//

void ClsBalloonHelp::SetMouseHook()
{
	if ( NULL == m_hMouseHook )
		m_hMouseHook = ::SetWindowsHookEx( WH_MOUSE, ( HOOKPROC )BHMouseHookThunk<ClsBalloonHelp>::GetThunk(), NULL, ::GetCurrentThreadId());
}

void ClsBalloonHelp::RemoveMouseHook()
{
	if ( NULL != m_hMouseHook )
	{
		::UnhookWindowsHookEx( m_hMouseHook );
		m_hMouseHook = NULL;
	}
}

//
// Call Window Return hook
//

void ClsBalloonHelp::SetCallWndRetHook()
{
	if ( NULL == m_hCallWndRetHook )
		m_hCallWndRetHook = ::SetWindowsHookEx( WH_CALLWNDPROCRET, ( HOOKPROC )BHCallWndRetHookThunk<ClsBalloonHelp>::GetThunk(), NULL, ::GetCurrentThreadId());
}

void ClsBalloonHelp::RemoveCallWndRetHook()
{
	if ( NULL != m_hCallWndRetHook )
	{
		::UnhookWindowsHookEx( m_hCallWndRetHook );
		m_hCallWndRetHook = NULL;
	}
}

LRESULT ClsBalloonHelp::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case	WM_CREATE:
			m_bIsPopup = TRUE;
			break;

		case	WM_SHOWWINDOW:
			// Fading effect supported by the OS?
			if ( ClsGetApp()->GetPlatformID() == VER_PLATFORM_WIN32_NT && ClsGetApp()->GetMajorVersion() >= 5 )
			{
				if ( wParam && ! ( m_unOptions & unDISABLE_FADEIN ))
				{
					// Animate the window.
					if ( AnimateWindow( 200, AW_BLEND ))
						return 0;
				}
				else if ( ! wParam && ! ( m_unOptions & unDISABLE_FADEOUT ))
				{
					// Animate the window.
					if ( AnimateWindow( 200, AW_HIDE | AW_BLEND ))
						return 0;
				}
			}
			break;

		case	WM_NCPAINT:
		{
			ClsWindowDC dc( this );
			DrawNonClientArea( &dc );
			break;
		}

		case	WM_PRINT:
		{
			ClsDC *pDC = ClsDC::FromHandle(( HDC )wParam );
			if ( lParam & PRF_NONCLIENT  ) DrawNonClientArea( pDC );
			break;
		}

		case	WM_PRINTCLIENT:
			//if ( lParam & PRF_ERASEBKGND ) SendMessage( WM_ERASEBKGND, wParam );
			if ( lParam & PRF_CLIENT ) SendMessage( WM_PAINT, wParam );
			return 0;

		case	WM_ACTIVATE:
			GetParent()->SendMessage( WM_NCACTIVATE, TRUE );
			if ( m_unOptions & unCLOSE_ON_ACTIVATE && ( wParam & ( WA_ACTIVE | WA_CLICKACTIVE )))
			{
				HideBalloon();
				return 0;
			}
			break;

		case	WM_LBUTTONDOWN:
		{
			POINT point = { LOWORD( lParam ), HIWORD( lParam ) };
			if ( m_unOptions & unSHOW_CLOSE_BUTTON )
			{
				ClsRect rect = GetClientRect(); 
				rect.Left() = rect.Right() - GetCloseButtonWidth();
				rect.Bottom() = rect.Top() + GetCloseButtonHeight();
				if ( rect.PtInRect( point ))
				{
					if ( ! m_hTheme ) m_uCloseState |= DFCS_PUSHED;
					else		  m_uCloseState = TTCS_PRESSED;
					SetCapture();
					SendMessage( WM_MOUSEMOVE, 0, lParam );
				}
			}
			break;
		}

		case	WM_LBUTTONUP:
		{
			POINT point = { LOWORD( lParam ), HIWORD( lParam ) };
			if (( m_unOptions & unSHOW_CLOSE_BUTTON ) && (( m_uCloseState & DFCS_PUSHED ) || ( m_uCloseState == TTCS_PRESSED )))
			{
				ReleaseCapture();
				if ( ! m_hTheme ) m_uCloseState &= ~DFCS_PUSHED;
				else		  m_uCloseState = TTCS_NORMAL;
				ClsRect rect = GetClientRect();
				rect.Left() = rect.Right() - GetCloseButtonWidth();
				rect.Bottom() = rect.Top() + GetCloseButtonHeight();
				if ( rect.PtInRect( point ))
				{
					m_unOptions |= unDISABLE_FADEOUT; // No fadout when using the close button.
					HideBalloon();
				}
			}
			else if ( ! m_strURL.IsEmpty())
			{
				if ( GetClientRect().PtInRect( point ))
				{
					::ShellExecute( NULL, NULL, m_strURL, NULL, NULL, SW_SHOWNORMAL );
					HideBalloon();
				}
			}
			return 0;
		}

		case	WM_NCHITTEST:
		case	WM_MOUSEMOVE:
		{
			POINT point;
			if ( uMsg == WM_MOUSEMOVE ) 
			{ 
				point.x = LOWORD( lParam ); 
				point.y = HIWORD( lParam ); 
			}
			else
			{
				::GetCursorPos( &point );
				ScreenToClient( &point );
			}
				
			if ( m_unOptions & unSHOW_CLOSE_BUTTON )
			{
				ClsRect rect = GetClientRect();
				GetClientRect( rect );
				rect.Left() = rect.Right() - GetCloseButtonWidth();
				rect.Bottom() = rect.Top() + GetCloseButtonHeight();
				ClsGetDC dc( this );
				UINT uState;
				BOOL bPushed;
				if ( ! m_hTheme ) 
				{
					uState = DFCS_CAPTIONCLOSE;
					bPushed = m_uCloseState & DFCS_PUSHED;
					m_uCloseState &= ~DFCS_PUSHED;
				}
				else
				{
					uState = TTCS_NORMAL;
					bPushed = ( BOOL )( m_uCloseState == TTCS_PRESSED );
					m_uCloseState = 0;
				}

				if ( rect.PtInRect( point ))
				{
					if ( ! m_hTheme ) uState |= DFCS_HOT;
					else		  uState = TTCS_HOT; 
					if ( bPushed )
					{
						if ( ! m_hTheme ) uState |= DFCS_PUSHED;
						else		  uState = TTCS_PRESSED;
					}
				}
				else
				{
					if ( ! m_hTheme ) uState |= DFCS_FLAT;
					else		  uState = TTCS_NORMAL;
				}

				if ( uState != m_uCloseState )
				{
					if ( ! m_hTheme ) dc.DrawFrameControl( rect, DFC_CAPTION, uState );
					else		  ThemingAPI.DrawThemeBackground( m_hTheme, dc, TTP_CLOSE, uState, rect, NULL );
					m_uCloseState = uState;
				}
				if ( bPushed )
				{
					if ( ! m_hTheme ) m_uCloseState |= DFCS_PUSHED;
					else		  m_uCloseState = TTCS_PRESSED;
				}
			}
			break;
		}

		case WM_NCCALCSIZE:
		{
			NCCALCSIZE_PARAMS FAR* lpncsp = ( NCCALCSIZE_PARAMS FAR * )lParam;

			// nTIP_MARGIN pixel margin on all sides
			::InflateRect( &lpncsp->rgrc[ 0 ], -nTIP_MARGIN, -nTIP_MARGIN );

			// nTIP_TAIL pixel "tail" on side closest to anchor
			switch ( GetBalloonQuadrant())
			{
				case BQ_TOPRIGHT:
				case BQ_TOPLEFT:
					lpncsp->rgrc[ 0 ].top += nTIP_TAIL;
					break;
				
				case BQ_BOTTOMRIGHT:
				case BQ_BOTTOMLEFT:
					lpncsp->rgrc[ 0 ].bottom -= nTIP_TAIL;
					break;
			}

			// sanity: ensure rect does not have negative size
			if ( lpncsp->rgrc[ 0 ].right < lpncsp->rgrc[ 0 ].left )
				lpncsp->rgrc[ 0 ].right = lpncsp->rgrc[ 0 ].left;
			if ( lpncsp->rgrc[ 0 ].bottom < lpncsp->rgrc[ 0 ].top )
				lpncsp->rgrc[ 0 ].bottom = lpncsp->rgrc[ 0 ].top;

			//if ( wParam )
			//{
				// determine if client position has changed relative to the window position
				// if so, don't bother presearving anything.
			//	if ( ! ::EqualRect( &lpncsp->rgrc[ 0 ], &lpncsp->rgrc[ 2 ] ))
			//	{
			//		::SetRectEmpty( &lpncsp->rgrc[ 2 ] );
			//	}
			//}
			return 0;
		}

		case	WM_TIMER:
		{
			switch ( wParam )
			{
				case ID_TIMER_CLOSE:
					KillTimer( ID_TIMER_CLOSE );
					HideBalloon();
					return 0;
			}
			return 0;
		}

		case	WM_THEMECHANGED:
			// Theming API active?
			if ( ThemingAPI.IsThemingEnabled())
			{
				// Destroy old theme and load the new.
				if ( m_hTheme ) ThemingAPI.CloseThemeData( m_hTheme );
				m_hTheme = ThemingAPI.OpenThemeData( GetSafeHWND(), L"TOOLTIP" );
			}
			break;

		case	WM_DESTROY:
			// Could use the OnDestroy() method... oh well...
			RemoveMouseHook();
			RemoveKeyboardHook();
			RemoveCallWndRetHook();
			break;

		case	WM_CLOSE:
			HideBalloon();
			return 0;
	}
	return ClsWindow::WindowProc( uMsg, wParam, lParam );
}

// Keyboard hook: used to implement the unCLOSE_ON_KEYPRESS option
LRESULT ClsBalloonHelp::KeyboardHookProc( int code, WPARAM wParam, LPARAM lParam)
{
	// Skip if the key was released or if it's a repeat
	// Bit 31:  Specifies the transition state. The value is 0 if the key  
	//       is being pressed and 1 if it is being released (see MSDN).
	if ( code >= 0 && ! ( lParam & 0x80000000 ) && NULL != m_hWnd )
	{
		PostMessage( WM_CLOSE );
	}
	return ::CallNextHookEx( m_hKeyboardHook, code, wParam, lParam );
}

// Mouse hook: used to implement un-obtrusive mouse tracking
LRESULT ClsBalloonHelp::MouseHookProc( int code, WPARAM wParam, LPARAM lParam )
{
	if ( code >= 0 && NULL != m_hWnd )
	{
		switch (( UINT )wParam )
		{
			case WM_NCMOUSEMOVE:
			case WM_MOUSEMOVE:
				if (( m_unOptions & unCLOSE_ON_MOUSE_MOVE ))
				{
					ClsPoint pt;
					::GetCursorPos( pt );
					if (( abs( pt.X() - m_ptMouseOrig.X()) > m_nMouseMoveTolerance || abs( pt.Y() - m_ptMouseOrig.Y()) > m_nMouseMoveTolerance ))
						PostMessage( WM_CLOSE );
				}
				break;

			case WM_NCLBUTTONDOWN:
			case WM_LBUTTONDOWN:
				if (( m_unOptions & unCLOSE_ON_LBUTTON_DOWN ))
					PostMessage( WM_CLOSE );
				break;

			case WM_NCMBUTTONDOWN:
			case WM_MBUTTONDOWN:
				if (( m_unOptions & unCLOSE_ON_MBUTTON_DOWN ))
					PostMessage( WM_CLOSE );
				break;

			case WM_NCRBUTTONDOWN:
			case WM_RBUTTONDOWN:
				if (( m_unOptions & unCLOSE_ON_RBUTTON_DOWN ))
					PostMessage( WM_CLOSE );
				break;

			case WM_NCLBUTTONUP:
			case WM_LBUTTONUP:
				if (( m_unOptions & unCLOSE_ON_LBUTTON_UP ))
					PostMessage( WM_CLOSE );
				break;

			case WM_NCMBUTTONUP:
			case WM_MBUTTONUP:
				if (( m_unOptions & unCLOSE_ON_MBUTTON_UP ))
					PostMessage( WM_CLOSE );
				break;

			case WM_NCRBUTTONUP:
			case WM_RBUTTONUP:
				if (( m_unOptions & unCLOSE_ON_RBUTTON_UP ))
					PostMessage( WM_CLOSE );
				break;
		}
	}
	return ::CallNextHookEx( m_hMouseHook, code, wParam, lParam );
}

// Window Return hook: used to implement window following
LRESULT ClsBalloonHelp::CallWndRetProc( int code, WPARAM wParam, LPARAM lParam )
{
	if ( code >= 0 && NULL != m_hWnd )
	{
		CWPRETSTRUCT* pcwpr = ( CWPRETSTRUCT * )lParam;
		if ( WM_MOVE == pcwpr->message && pcwpr->hwnd == m_hwndAnchor )
			PositionWindow();
	}

	return ::CallNextHookEx( m_hCallWndRetHook, code, wParam, lParam );
}