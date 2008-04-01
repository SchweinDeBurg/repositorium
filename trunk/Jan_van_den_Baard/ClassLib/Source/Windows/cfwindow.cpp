//
// cfwindow.cpp.
//
// (C) Copyright 2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "cfwindow.h"
#include "../tools/drawtools.h"
#include "../tools/color.h"
#include "../tools/xpcolors.h"
#include "../gdi/windowdc.h"
#include "../gdi/bufferdc.h"
#include "../gdi/selector.h"

#define DCB_SIZE_V1 sizeof( int ) + ( 2 * sizeof( UINT )) + sizeof( RECT ) + sizeof( DWORD ) // DRAWCAPTIONBUTTON size (v1)
#define TT_TIMER 0xDEAD // Timer for the tooltip.

// Constructor.
ClsCFWindow::ClsCFWindow()
{
	// Default datamembers.
	m_nHotButton	     = -1;
	m_bTracking	     = FALSE;
	m_bIsHot	     = FALSE;
	m_bTipVisible	     = FALSE;
	m_bDoCustomFrames    = TRUE;
	m_bOldStyle	     = FALSE;
	m_CaptionButtons.SetSize( 0, 10 );
	CreateCaptionFonts();
}

// Destructor.
ClsCFWindow::~ClsCFWindow()
{
}

// Add a single caption button.
BOOL ClsCFWindow::AddCaptionButton( UINT nType, UINT nID, BOOL bCheckable /* = FALSE */, BOOL bLeft /* = FALSE */, LPTSTR pszTooltip /* = NULL */, DWORD dwData /* = 0L */ )
{
	// Does the button already exist?
	for ( int i = 0 ; i < m_CaptionButtons.GetSize(); i++ )
	{
		// There can be only one...
		if ( m_CaptionButtons[ i ] .nID == nID )
			return FALSE;
	}

	// Create the new button.
	StrCapButton button;
	button.bCheckable    = bCheckable;
	button.bLeftAlign    = bLeft;
	button.dwState       = CBS_NORMAL;
	button.nID	     = nID;
	button.nType	     = nType;
	button.nImage	     = ( UINT )-1;
	button.nCheckedImage = ( UINT )-1;
	button.pszTooltip    = pszTooltip;
	button.dwButtonData  = dwData;

	// What type of button are we talking about?
	// If it is not a CBT_CUSTOM button 
	// we will determine system images and
	// check type here.
	switch ( nType )
	{
		case CBT_CLOSE: button.nImage	     = ClsApp::CII_CLOSE; 
				button.bCheckable    = FALSE;
				if ( ! button.pszTooltip ) button.pszTooltip = _T( "Close" );
				break;

		case CBT_MENU:  button.nImage	     = ClsApp::CII_DROPDOWN_SMALL; 
				button.bCheckable    = FALSE; 
				if ( ! button.pszTooltip ) button.pszTooltip = _T( "Options" );
				break;

		case CBT_PIN:   button.nImage        = ClsApp::CII_PIN;
				button.nCheckedImage = ClsApp::CII_PINNED; 
				button.bCheckable    = TRUE; 
				if ( ! button.pszTooltip ) button.pszTooltip = _T( "Hide" );
				break;

		case CBT_UP:    button.nImage	     = ClsApp::CII_UP; 
				button.bCheckable    = FALSE; 
				if ( ! button.pszTooltip ) button.pszTooltip = _T( "Up" );
				break;

		case CBT_DOWN:  button.nImage	     = ClsApp::CII_DOWN; 
				button.bCheckable    = FALSE; 
				if ( ! button.pszTooltip ) button.pszTooltip = _T( "Down" );
				break;

		case CBT_LEFT:  button.nImage	     = ClsApp::CII_LEFT; 
				button.bCheckable    = FALSE; 
				if ( ! button.pszTooltip ) button.pszTooltip = _T( "Previous" );
				break;

		case CBT_RIGHT: button.nImage	     = ClsApp::CII_RIGHT; 
				button.bCheckable    = FALSE; 
				if ( ! button.pszTooltip ) button.pszTooltip = _T( "Next" );
				break;
	}

	// Add it to the array.
	return ( BOOL )( m_CaptionButtons.Add( button ) >= 0 ? TRUE : FALSE );
}

// Remove a single caption button.
BOOL ClsCFWindow::RemoveCaptionButton( UINT nID )
{
	// Find the button with the ID.
	for ( int i = 0; i < m_CaptionButtons.GetSize(); i++ )
	{
		// Is this the one?
		if ( m_CaptionButtons[ i ].nID == nID )
		{
			// Remove it and return success.
			m_CaptionButtons.RemoveAt( i, 1 );
			return TRUE;
		}
	}
	// Not found.
	return FALSE;
}

// (Un)Check a caption button.
BOOL ClsCFWindow::CheckCaptionButton( UINT nID, BOOL bChecked /* = TRUE */ )
{
	// Find the button.
	for ( int i = 0; i < m_CaptionButtons.GetSize(); i++ )
	{
		// Is this the one?
		if ( m_CaptionButtons[ i ].nID == nID && m_CaptionButtons[ i ].bCheckable == TRUE )
		{
			// Note current state.
			DWORD dwState = m_CaptionButtons[ i ].dwState;

			// Check or uncheck the button.
			if ( bChecked ) m_CaptionButtons[ i ].dwState |= CBS_CHECKED;
			else		m_CaptionButtons[ i ].dwState &= ~CBS_CHECKED;

			// Re-render the button if the
			// state actually changed and
			// we have a control window.
			if ( m_hWnd && m_CaptionButtons[ i ].dwState != dwState )
			{
				ClsWindowDC dc( this );
				if ( dc.IsValid())
					RenderButton( &dc, i );
			}
			return TRUE;
		}
	}
	return FALSE;
}

// Is the caption button checked?
BOOL ClsCFWindow::IsCaptionButtonChecked( UINT nID ) const
{
	// Browse the buttons.
	for ( int i = 0; i < m_CaptionButtons.GetSize(); i++ )
	{
		// Is this the one?
		if ( m_CaptionButtons[ i ].nID == nID )
			// Return checked status.
			return ( BOOL )( m_CaptionButtons[ i ].dwState & CBS_CHECKED ? TRUE : FALSE );
	}
	return FALSE;
}

// Turn custom frames on or off.
void ClsCFWindow::DoCustomFrames( BOOL bDoCustomFrames )
{
	// Did the value change?
	if ( bDoCustomFrames != m_bDoCustomFrames )
	{
		// Set the flags and re-do the frame if the
		// window is valid.
		m_bDoCustomFrames = bDoCustomFrames;
		if ( GetSafeHWND()) 
			SetWindowPos( NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE /*| SWP_NOREDRAW*/ );
	}
}

// Old-style rendering?
void ClsCFWindow::SetOldStyle( BOOL bOldStyle /*= TRUE */ )
{
	// Did the style change?
	if ( m_bOldStyle != bOldStyle )
	{
		// Set the new style.
		m_bOldStyle = bOldStyle;

		// Refresh the window frame.
		if ( GetSafeHWND()) 
			SetWindowPos( NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE /*| SWP_NOREDRAW*/ );
	}
}

// Compute frame sizes.
void ClsCFWindow::OnComputeFrameSize( ClsRect& rcFrame, LONG& nCaptionSize )
{
	// Frame sizes.
	if ( m_bOldStyle )
	{
		// "Old"-style.
		rcFrame.Left() = rcFrame.Right() = ::GetSystemMetrics( SM_CXDLGFRAME );
		rcFrame.Top() = rcFrame.Bottom() = ::GetSystemMetrics( SM_CYDLGFRAME );
	}
	else
	{
		// "XP"-style.
		rcFrame.Left() = rcFrame.Right() = 3;
		rcFrame.Top() = rcFrame.Bottom() = 3;
	}

	// Caption size.
	nCaptionSize = ::GetSystemMetrics( GetExStyle() & WS_EX_TOOLWINDOW ? SM_CYSMCAPTION : SM_CYCAPTION );
}

// Render the window frame.
BOOL ClsCFWindow::OnRenderWindowFrame( ClsDC *pDC, ClsRect rcWindowRect )
{
	// "Old"-style?
	if ( m_bOldStyle )
	{
		ClsDrawTools::DrawRectangle( *pDC, rcWindowRect, ::GetSysColor( COLOR_BTNFACE ), LeftFrameSize());
		pDC->DrawEdge( rcWindowRect, EDGE_RAISED, BF_RECT );
	}
	else
	{
		// Draw the outer and inner rectangle.
		COLORREF crFrame = GetForegroundWindow() == this ? XPColors.GetXPColor( ClsXPColors::XPC_TOOLWINDOW_FRAME_ACTIVE ) : XPColors.GetXPColor( ClsXPColors::XPC_TOOLWINDOW_FRAME );
		ClsDrawTools::DrawRectangle( *pDC, rcWindowRect, crFrame, m_rcFrameSize.Left() - 1 );
		rcWindowRect.Deflate( m_rcFrameSize.Left() - 1, m_rcFrameSize.Top() - 1 );
		ClsDrawTools::DrawRectangle( *pDC, rcWindowRect, XPColors.GetXPColor( ClsXPColors::XPC_TOOLWINDOW_BACKGROUND ));

		// Render the corners of the inner frame in the
		// outer frame colors.
		pDC->SetPixelV( rcWindowRect.TopLeft(), crFrame );
		pDC->SetPixelV( rcWindowRect.Right() - 1, rcWindowRect.Bottom() - 1, crFrame );
		pDC->SetPixelV( rcWindowRect.Right() - 1, rcWindowRect.Top(), crFrame );
		pDC->SetPixelV( rcWindowRect.Left(), rcWindowRect.Bottom() - 1, crFrame );
	}
	return TRUE;
}

// Render the caption background.
BOOL ClsCFWindow::OnRenderCaptionBackground( ClsDC *pDC, ClsRect rcCaption )
{
	// "Old"-style?
	if ( m_bOldStyle )
	{
		// Fill the caption rectangle.
		if ( GetForegroundWindow() == this )
			ClsDrawTools::DrawGradient( *pDC, rcCaption, ::GetSysColor( COLOR_ACTIVECAPTION ), ::GetSysColor( COLOR_GRADIENTACTIVECAPTION ));
		else
			ClsDrawTools::DrawGradient( *pDC, rcCaption, ::GetSysColor( COLOR_INACTIVECAPTION ), ::GetSysColor( COLOR_GRADIENTINACTIVECAPTION ));
	}
	else
	{
		// Fill the caption rectangle.
		pDC->FillSolidRect( rcCaption, XPColors.GetXPColor( GetForegroundWindow() == this ? ClsXPColors::XPC_TOOLWINDOW_CAPTION_ACTIVE : ClsXPColors::XPC_TOOLWINDOW_CAPTION ));

		// Create the pen to render the line at the bottom
		// of the caption rectangle.
		ClsPen pen( PS_SOLID, 1, XPColors.GetXPColor( ClsXPColors::XPC_TOOLWINDOW_BACKGROUND ));

		// Brace the code. This makes sure that the pen is
		// unselected from the DC before it goes out of scope.
		{
			// Select Gfx objects.
			ClsSelector sel1( pDC, pen );

			// Render the line.
			pDC->MoveTo( rcCaption.Left(), rcCaption.Bottom() -1 );
			pDC->LineTo( rcCaption.Right(), rcCaption.Bottom() -1 );
		}
	}
	return TRUE;
}

// Render the caption text.
BOOL ClsCFWindow::OnRenderCaptionText( ClsDC *pDC, ClsRect rcCaptionText )
{
	// Do we have a caption?
	ClsString str( GetSafeHWND());
	if ( str.GetStringLength())
	{
		// Save the DC state.
		int sDC = pDC->SaveDC();

		// Setup the font.
		pDC->SelectObject( GetExStyle() & WS_EX_TOOLWINDOW ? m_SmCaptionFont : m_CaptionFont );

		// Setup the DC.
		if ( m_bOldStyle ) pDC->SetTextColor( ::GetSysColor( COLOR_CAPTIONTEXT ));
		else pDC->SetTextColor( GetForegroundWindow() == this ? XPColors.GetXPColor( ClsXPColors::XPC_TOOLWINDOW_CAPTION_TEXT_ACTIVE ) : XPColors.GetXPColor( ClsXPColors::XPC_TOOLWINDOW_CAPTION_TEXT ));
		pDC->SetBkMode( TRANSPARENT );

		// Render the caption.
		rcCaptionText.Bottom()--;
		ClsDrawTools::RenderText( *pDC, str, rcCaptionText, ClsDrawTools::CDSF_LEFTALIGN | ClsDrawTools::CDSF_END_ELLIPSIS );

		// Restore DC state.
		pDC->RestoreDC( sDC );
		return TRUE;
	}
	return FALSE;
}

// Render a caption button.
BOOL ClsCFWindow::OnRenderCaptionButton( ClsDC *pDC, LPDRAWCAPTIONBUTTON pDrawCaptionButton )
{
	// Determine image color.
	COLORREF crImageColor = RGB( 0, 0, 0 );
	if ( ! m_bOldStyle )
	{
		if (( pDrawCaptionButton->nState & CBS_HOT ) != CBS_HOT || pDrawCaptionButton->nState & CBS_CLICKED )
			crImageColor = GetForegroundWindow() == this ? XPColors.GetXPColor( ClsXPColors::XPC_TOOLWINDOW_CAPTION_TEXT_ACTIVE ) : XPColors.GetXPColor( ClsXPColors::XPC_TOOLWINDOW_CAPTION_TEXT );
	}

	// Find the button in the array.
	int nImage;
	for ( int i = 0; i < m_CaptionButtons.GetSize(); i++ )
	{
		// Is this the one?
		if ( m_CaptionButtons[ i ].nID == pDrawCaptionButton->nID )
		{
			// Select the image to render.
			nImage = ( pDrawCaptionButton->nState & CBS_CHECKED ? m_CaptionButtons[ i ].nCheckedImage : m_CaptionButtons[ i ].nImage );

			// Check the state.
			ClsRect rc = pDrawCaptionButton->rcButton;
			rc.Bottom()--;
			ClsRect rci( rc );
			if ( rci.Width()  & 1 ) rci.Offset( 1, 0 );
			if ( rci.Height() & 1 ) rci.Offset( 0, 1 );
			
			// "Old"-style?
			if ( m_bOldStyle )
			{
				// Render button background.
				rc.Deflate( 1, 1 );
				pDC->FillSolidRect( rc, ::GetSysColor( COLOR_BTNFACE ));

				// Render using the correct state.
				if (( pDrawCaptionButton->nState & CBS_CHECKED ) != CBS_CHECKED )
					pDC->DrawEdge( rc, ( ! ( pDrawCaptionButton->nState & CBS_CLICKED )) ? EDGE_RAISED : EDGE_SUNKEN, BF_RECT );
				else
					pDC->DrawEdge( rc, ( pDrawCaptionButton->nState & CBS_CLICKED ) ? EDGE_SUNKEN : BDR_SUNKENOUTER, BF_RECT );

				// Render the image.
				if ( nImage != -1 ) ClsDrawTools::RenderMonoBitmap( *pDC, ClsGetApp()->GetClsImages(), nImage, rci, crImageColor, 0 );
			}
			else
			{
				// Render using the correct state.
				if ( pDrawCaptionButton->nState & CBS_HOT ) 
				{
					pDC->OutlinedRectangle( rc, XPColors.GetXPColor( ClsXPColors::XPC_OUTER_SELECTION ), XPColors.GetXPColor( ClsXPColors::XPC_INNER_SELECTION ));
					if ( nImage != -1 ) ClsDrawTools::RenderMonoBitmap( *pDC, ClsGetApp()->GetClsImages(), nImage, rci, crImageColor, 0 );
				}
				else if ( pDrawCaptionButton->nState & CBS_CLICKED )
				{
					pDC->OutlinedRectangle( rc, XPColors.GetXPColor( ClsXPColors::XPC_OUTER_SELECTION ), XPColors.GetXPColor( ClsXPColors::XPC_INNER_CHECKED_SELECTED ));
					if ( nImage != -1 ) ClsDrawTools::RenderMonoBitmap( *pDC, ClsGetApp()->GetClsImages(), nImage, rci, crImageColor, 0 );
				}
				else
				{
					pDC->FillSolidRect( rc, GetForegroundWindow() == this ? XPColors.GetXPColor( ClsXPColors::XPC_TOOLWINDOW_CAPTION_ACTIVE ) : XPColors.GetXPColor( ClsXPColors::XPC_TOOLWINDOW_CAPTION ));
					if ( nImage != -1 ) ClsDrawTools::RenderMonoBitmap( *pDC, ClsGetApp()->GetClsImages(), nImage, rci, crImageColor, 0 );
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

// Called when a caption button was clicked. Should be
// overridden.
void ClsCFWindow::OnCaptionButtonClicked( UINT nID )
{
}

// (Re)Create the caption fonts.
void ClsCFWindow::CreateCaptionFonts()
{
	// Determine the caption font.
	NONCLIENTMETRICS ncm = { sizeof( NONCLIENTMETRICS ), 0 };
	if ( ::SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( ncm ), ( void * )&ncm, 0 ))
	{
		// Create the fonts.
		m_CaptionFont.CreateFontIndirect( &ncm.lfCaptionFont );
		m_SmCaptionFont.CreateFontIndirect( &ncm.lfSmCaptionFont );
	}
}

// Position the caption buttons.
void ClsCFWindow::PositionCaptionButtons()
{
	// Compute the caption rectangle.
	ClsRect rc;
	::GetWindowRect( m_hWnd, rc );
	rc.Offset( -rc.Left(), -rc.Top());
	rc.Left()  += m_rcFrameSize.Left();
	rc.Right() -= m_rcFrameSize.Right();
	rc.Top()   += m_rcFrameSize.Top();
	rc.Bottom() = rc.Top() + m_lCaptionSize;

	// Start at the left/right of the rectangle.
	int rOff = rc.Right(), lOff = rc.Left(), nSize = rc.Height();

	// Iterate the buttons.
	for ( int i = 0; i < m_CaptionButtons.GetSize(); i++ )
	{
		// Fixed values.
		m_CaptionButtons[ i ].rcButton.Top()    = rc.Top();
		m_CaptionButtons[ i ].rcButton.Bottom() = rc.Bottom();

		// Left aligned?
		if ( m_CaptionButtons[ i ].bLeftAlign == TRUE )
		{
			// Position at the next left-side slot.
			m_CaptionButtons[ i ].rcButton.Left()  = lOff;
			m_CaptionButtons[ i ].rcButton.Right() = lOff + nSize;
			lOff += nSize;
		}
		else
		{
			// Position at the next right-side slot.
			m_CaptionButtons[ i ].rcButton.Right() = rOff;
			m_CaptionButtons[ i ].rcButton.Left()  = rOff - nSize;
			rOff -= nSize;
		}
	}

	// Setup the captiontext rectangle.
	rc.Left()  = lOff;
	rc.Right() = rOff;
	rc.Deflate( 2, 0 );
	m_Caption  = rc;
}

// Return the index of the caption button which the
// mousepointer or point is located in. Returns -1
// if the point in not located in a button.
int ClsCFWindow::HitTestCaptionButton()
{
	// Get mouse position.
	ClsPoint pt;
	::GetCursorPos( pt );
	return HitTestCaptionButton( pt );
}
int ClsCFWindow::HitTestCaptionButton( ClsPoint pt )
{
	// Offset the coordinates with the window
	// position.
	ClsRect rc;
	::GetWindowRect( m_hWnd, rc );
	pt.Subtract( rc.TopLeft());

	// Iterate the caption buttons.
	for ( int i = 0; i < m_CaptionButtons.GetSize(); i++ )
	{
		// This one?
		if ( m_CaptionButtons[ i ].rcButton.PtInRect( pt ))
			return i;
	}
	return -1;
}

// Render a single button.
void ClsCFWindow::RenderButton( ClsDC *pDC, int nIndex )
{
	// Build structure.
	DRAWCAPTIONBUTTON dcb;
	dcb.cbSize	= sizeof( dcb );
	dcb.nID		= m_CaptionButtons[ nIndex ].nID;
	dcb.nState	= m_CaptionButtons[ nIndex ].dwState;
	dcb.rcButton	= m_CaptionButtons[ nIndex ].rcButton;
	dcb.dwData	= m_CaptionButtons[ nIndex ].dwButtonData;

	// Call the overridable.
	OnRenderCaptionButton( pDC, &dcb );
}

// Render all the buttons.
void ClsCFWindow::RenderAllButtons( ClsDC *pDC )
{
	// Iterate buttons.
	PositionCaptionButtons();
	for ( int i = 0; i < m_CaptionButtons.GetSize(); i++ )
	{
		// Normal state...
		m_CaptionButtons[ i ].dwState &= ~( CBS_HOT | CBS_CLICKED );
		RenderButton( pDC, i );
	}
}

// Set the button state.
void ClsCFWindow::SetButtonState( int index, DWORD dwState )
{
	// What is the state?
	switch ( dwState )
	{
		case	CBS_NORMAL:
			// Clear other states.
			m_CaptionButtons[ index ].dwState &= ~( CBS_HOT | CBS_CLICKED );
			break;

		case	CBS_HOT:
		{
			// Show the tooltip. If it is already
			// visible we show it directly with the
			// new text. Otherwise it is shown after
			// GetDoubleClickTime() microseconds.
			if ( m_bTipVisible == FALSE )
				SetTimer( TT_TIMER, GetDoubleClickTime(), NULL );
			else
			{
				ClsPoint pt;
				::GetCursorPos( pt );
				ShowToolTip( pt, m_CaptionButtons[ index ].pszTooltip );
			}

			// Clear other states.
			m_CaptionButtons[ index ].dwState &= ~( CBS_CLICKED );
			m_CaptionButtons[ index ].dwState |= CBS_HOT;
			break;
		}

		case	CBS_CLICKED:
			// Clear other states.
			m_CaptionButtons[ index ].dwState &= ~( CBS_HOT );
			m_CaptionButtons[ index ].dwState |= CBS_CLICKED;
			break;
	}

	// Render the button.
	ClsWindowDC dc( this );
	if ( dc.IsValid())
		RenderButton( &dc, index );
}

// Hide the tooltip.
void ClsCFWindow::HideToolTip()
{
	// Tooltip available?
	if ( m_Tip.GetSafeHWND())
	{
		// Fill structure.
		TOOLINFO ti = { 0 };
		ti.cbSize   = sizeof( ti );
		ti.hwnd     = m_hWnd;
		ti.uId = ( UINT_PTR )( HWND )m_hWnd;

		// Hide the tip.
		m_Tip.TrackActivate( FALSE, ti );
		KillTimer( TT_TIMER );
		m_bTipVisible = FALSE;
	}
}

// Show the tooltip.
void ClsCFWindow::ShowToolTip( LPPOINT pt, LPTSTR pszTip )
{
	// Tooltip available?
	if ( m_Tip.GetSafeHWND() && pszTip )
	{
		// Fill structure.
		TOOLINFO ti = { 0 };
		ti.cbSize   = sizeof( ti );
		ti.hwnd     = m_hWnd;
		ti.uId      = ( UINT_PTR )( HWND )m_hWnd;
		ti.hinst    = ClsGetInstanceHandle();
		ti.lpszText = pszTip;

		// Hide the tip.
		m_Tip.UpdateTipText( ti );
		m_Tip.TrackPosition( pt->x, pt->y );
		m_Tip.TrackActivate( TRUE, ti );
		m_bTipVisible = TRUE;
	}
}

// Render the complete frame.
void ClsCFWindow::RenderWholeFrame( ClsDC *pDC )
{
	// Get window rectangle.
	ClsRect rc = GetWindowRect();

	// Adjust as frame rectangle.
	rc.Offset( -rc.Left(), -rc.Top() );

	// Render the frame.
	OnRenderWindowFrame( pDC, rc );

	{
		// Adjust rectangle for caption and render the background.
		rc.Left()  += m_rcFrameSize.Left();
		rc.Top()   += m_rcFrameSize.Top();
		rc.Right() -= m_rcFrameSize.Right();
		rc.Bottom() = rc.Top() + m_lCaptionSize;

		// Render the caption background.
		OnRenderCaptionBackground( pDC, rc );

		// The buttons.
		RenderAllButtons( pDC );

		// The title.
		OnRenderCaptionText( pDC, m_Caption );
	}
}

// Erase client background.
LRESULT ClsCFWindow::OnEraseBkgnd( ClsDC *pDC )
{
	pDC->FillSolidRect( GetClientRect(), m_bOldStyle ? ::GetSysColor( COLOR_BTNFACE ) : XPColors.GetXPColor( ClsXPColors::XPC_TOOLWINDOW_BACKGROUND ));
	return 1;
}

// Window procedure.
LRESULT ClsCFWindow::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Only if we are doing frames...
	if ( m_bDoCustomFrames )
	{
		// What's up?
		switch ( uMsg )
		{
			case	WM_SETTINGCHANGE:
				// (Re)Create the fonts.
				CreateCaptionFonts();
				break;

			case	WM_GETMINMAXINFO:
			{
				// Adjust minimum tracking size if neacessary.
				LPMINMAXINFO pInfo = reinterpret_cast< LPMINMAXINFO >( lParam );
				pInfo->ptMinTrackSize.x = max(( m_CaptionButtons.GetSize() * CaptionSize()) + CaptionSize(), pInfo->ptMinTrackSize.x );
				pInfo->ptMinTrackSize.y = max( TopFrameSize() + BottomFrameSize() + ( CaptionSize() * 2 ), pInfo->ptMinTrackSize.y );
				break;
			}

			case	WM_TIMER:
				// Tooltip message?
				if ( wParam == TT_TIMER )
				{
					// Is there a hot button?
					if ( m_nHotButton != -1 )
					{
						// Yes. Show the tooltip at the current
						// mouse position.
						ClsPoint pt;
						::GetCursorPos( pt );
						ShowToolTip( pt, m_CaptionButtons[ m_nHotButton ].pszTooltip );
					}

					// Stop the timer event.
					KillTimer( TT_TIMER );
					break;
				}

			case	WM_CREATE:
				// Create the tooltip-control.
				if ( m_Tip.Create( this ))
				{
					TOOLINFO ti = { 0 };
					ti.cbSize   = sizeof( ti );
					ti.uFlags   = TTF_IDISHWND;
					ti.hwnd     = m_hWnd;
					ti.hinst    = ClsGetInstanceHandle();
					ti.lpszText = NULL;
					ti.uId	    = ( UINT_PTR )m_hWnd;
					m_Tip.AddTool( ti );
				}
				break;

			case	WM_EXITSIZEMOVE:
			{
				// Reset cursor.
				::SetCursor( ::LoadCursor( NULL, IDC_ARROW ));
				break;
			}

			case	WM_LBUTTONUP:
				// If we are not tracking then
				// we do nothing.
				if ( ! m_bTracking )
					break;

			case	WM_NCLBUTTONUP:
			{
				// If we have capture we release it
				// here.
				if ( GetCapture() == this ) 
					ReleaseCapture();

				// Do we have a valid hot button?
				if ( m_nHotButton != -1 )
				{
					// Restore the current button to
					// normal.
					if ( m_nHotButton != -1 )
						SetButtonState( m_nHotButton, CBS_NORMAL );

					// Is the mouse located on the hot
					// button?
					if ( HitTestCaptionButton() == m_nHotButton && m_nHotButton != -1 )
					{
						// Toggle checked state if this is a checkable button.
						if ( m_CaptionButtons[ m_nHotButton ].bCheckable )
							m_CaptionButtons[ m_nHotButton ].dwState ^= CBS_CHECKED;

						// Call the overidable.
						OnCaptionButtonClicked( m_CaptionButtons[ m_nHotButton ].nID );
					}
				}

				// Reset data.
				m_nHotButton = -1;
				m_bTracking  = FALSE;
				m_bIsHot     = FALSE;
				break;
			}

			case	WM_NCLBUTTONDBLCLK:
			case	WM_NCLBUTTONDOWN:
			{
				// Are we on the caption?
				if ( SendMessage( WM_NCHITTEST ) == HTCAPTION )
					::SetCursor( ::LoadCursor( NULL, IDC_SIZEALL ));

				// What button are we hovering (if any)?
				int i;
				if (( i = HitTestCaptionButton()) != -1 )
				{
					// Obtain capture.
					HideToolTip();
					SetCapture();
					m_nHotButton = i;
					m_bTracking  = TRUE;
					m_bIsHot     = TRUE;

					// Render the button in the clicked state.
					if ( m_nHotButton != -1 )
						SetButtonState( m_nHotButton, CBS_CLICKED );
				}
				break;
			}

			case	WM_NCCALCSIZE:
			{
				// Call the overidable.
				OnComputeFrameSize( m_rcFrameSize, m_lCaptionSize );

				// Adjust frame sizes.
				LPRECT pClient = reinterpret_cast< LPRECT >( lParam );
				pClient->left   += m_rcFrameSize.Left();
				pClient->top    += m_rcFrameSize.Top() + m_lCaptionSize;
				pClient->right  -= m_rcFrameSize.Right();
				pClient->bottom -= m_rcFrameSize.Bottom();
				return 0;
			}

			case	WM_KILLFOCUS:
			case	WM_ACTIVATE:
			case	WM_NCACTIVATE:
			{
				// First the base class.
				LRESULT rc = ClsWindow::WindowProc( uMsg, wParam, lParam );

				// Render the caption frame.
				ClsWindowDC dc( this );
				if ( dc.IsValid())
					RenderWholeFrame( &dc );
				return rc;
			}

			case	WM_NCMOUSELEAVE:
				// Stop if we are tracking...
				if ( m_bTracking )
					break;

				// Reset hot button.
				m_nHotButton = -1;
				HideToolTip();

			case	WM_NCPAINT:
			{
				// Re-render the entire frame.
				ClsWindowDC dc( this );
				if ( dc.IsValid())
					RenderWholeFrame( &dc );
				return TRUE;
			}

			case	WM_NCHITTEST:
			{
				// Get cursor position.
				ClsPoint pt;
				::GetCursorPos( pt );

				// If it is located on a button we say it has
				// hit the border.
				if ( HitTestCaptionButton( pt ) != -1 )
					return HTBORDER;

				// Check if it is located in the
				// client rectangle.
				ClsRect rc = GetClientRect();
				ClientToScreen( rc );
				if ( rc.PtInRect( pt ))
					return HTCLIENT;

				// On the caption area?
				rc = m_Caption;
				rc.Offset( GetWindowRect().TopLeft());
				if ( rc.PtInRect( pt ))
					return HTCAPTION;

				// If the system says it is in the border or nowhere
				// we might make it a caption hit.
				LRESULT hit = ClsWindow::WindowProc( uMsg, wParam, lParam );
				if ( hit == HTBORDER || hit == HTNOWHERE )
				{
					// If we are not resizable or we are but the
					// left, or right if swapped, mouse button is
					// not down we translate it to a caption hit.
					if ( ! ( GetStyle() & WS_THICKFRAME ) || 
					     ! ( GetAsyncKeyState( ::GetSystemMetrics( SM_SWAPBUTTON ) ? VK_RBUTTON : VK_LBUTTON ) & ( 1 << 15 )))
						return HTCAPTION;
				}
				// Let the system decide...
				break;
			}

			case	WM_MOUSEMOVE:
			{
				// Are we tracking?
				if ( m_bTracking == TRUE )
				{
					// Get the button under the pointer.
					int i;
					if (( i = HitTestCaptionButton()) == m_nHotButton )
					{
						// Are we rendered hot?
						if ( m_bIsHot == FALSE )
						{
							// No. Make it hot.
							m_bIsHot = TRUE;
							if ( m_nHotButton != -1 )
								SetButtonState( m_nHotButton, CBS_CLICKED );
						}
					}
					else
					{
						// Are we rendered hot?
						if ( m_bIsHot == TRUE )
						{
							// Yes. Restore it.
							m_bIsHot = FALSE;
							if ( m_nHotButton != -1 )
								SetButtonState( m_nHotButton, CBS_NORMAL );
						}
					}
				}
				break;
			}

			case	WM_NCMOUSEMOVE:
			{
				// Setup non-client mouse tracking.
				TRACKMOUSEEVENT tme = { 0 };
				tme.cbSize    = sizeof( tme );
				tme.dwFlags   = TME_LEAVE | TME_NONCLIENT;
				tme.hwndTrack = GetSafeHWND();
				_TrackMouseEvent( &tme );

				// Check the button under the pointer.
				int nButton = HitTestCaptionButton();
				ClsWindowDC dc( this );
				if ( dc.IsValid())
				{
					// Restore the "old" hot button if it exists
					// and is different from the new one.
					if ( m_nHotButton != -1 && m_nHotButton != nButton )
					{
						SetButtonState( m_nHotButton, CBS_NORMAL );
						m_nHotButton = -1;
					}

					// Make the "new" button hot if it is not
					// hot already.
					if ( nButton != -1 && m_nHotButton != nButton )
					{
						m_nHotButton = nButton;
						SetButtonState( m_nHotButton, CBS_HOT );
					}

					// Hide the tooltip if it is up and
					// there is not hot button.
					if ( m_nHotButton == -1 && m_bTipVisible )
						HideToolTip();
				}
				break;
			}
		}
	}
	// Call the base class.
	return ClsWindow::WindowProc( uMsg, wParam, lParam );
}
