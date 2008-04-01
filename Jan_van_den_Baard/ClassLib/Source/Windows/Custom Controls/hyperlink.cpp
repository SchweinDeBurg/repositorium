//
// hyperlink.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "hyperlink.h"
#include "../../gdi/getdc.h"
#include "../../gdi/paintdc.h"
#include "../../gdi/bufferdc.h"
#include "../../gdi/font.h"
#include "../../gdi/selector.h"
#include "../../tools/drawstate.h"
#include "../../application.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor. Setup defaults.
ClsHyperLink::ClsHyperLink()
{
	// Setup default colors.
	m_crNormal	= RGB( 0,   0, 255 ); // Blue
	m_crVisited	= RGB( 128, 0, 128 ); // Purple
	m_crHot		= RGB( 255, 0, 0   ); // Red

	// Place images on the left and center the
	// label by by default.
	m_nImagePlace = IMPLACE_LEFT;
	m_nLabelPlace = LBPLACE_CENTER;

	// Not visited by default.
	m_bIsVisited = FALSE;

	// Clear imagelist.
	m_hImages = NULL;

	// Get the hand cursor.
	m_curHand = ClsGetCursor( ClsApp::STC_HAND );
}

// Destructor.
ClsHyperLink::~ClsHyperLink()
{
}

// Browse the URL.
void ClsHyperLink::Trigger()
{
	// Browse the URL.
	if ( m_strURL.GetStringLength())
	{
		// Obtain current directory.
		TCHAR	szDir[ MAX_PATH ];
		::GetCurrentDirectory( MAX_PATH, szDir );

		// Open the URL.
		::ShellExecute( NULL, NULL, m_strURL, NULL, szDir, SW_SHOWNORMAL );

		// Visited the URL...
		m_bIsVisited = TRUE;

		// Re-render.
		Invalidate();
	}
}

// Render control.
void ClsHyperLink::PaintControl( ClsDC *pDC )
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

		// Are we transparent?
		if ( ! m_bTransparent )
			// No. Erase the background with the COLOR_BTNFACE color.
			::FillRect( dc, rc, ( HBRUSH )( COLOR_BTNFACE + 1 ));
		else
		{
			// Get the parent window.
			ClsWindow *pParent = GetParent();
			if ( pParent )
			{
				// Get it's DC.
				ClsGetDC dcp( pParent );
				if ( dcp.IsValid())
				{
					// Compute the rectangle of the control in respect to the
					// parent it's client area.
					ClsRect rcw = GetWindowRect();
					pParent->ScreenToClient( rcw );

					// Copy the current contents of the rectangle into
					// the buffer DC. This will "emulate" transparency.
					dc.BitBlt( 0, 0, rcw.Width(), rcw.Height(), &dcp, rcw.Left(), rcw.Top(), SRCCOPY );
				}
			}
		}

		// Do we have any images?
		if ( m_hImages )
		{
			// Get the number of images.
			int nImageCount = ::ImageList_GetImageCount( m_hImages );

			// Any images present?
			if ( nImageCount )
			{
				// Copy the client rectangle.
				ClsRect rcimg( rc );

				// We need to know the size of the images
				// in the image list.
				int cx, cy;
				::ImageList_GetIconSize( m_hImages, &cx, &cy );

				// Determine the place at which we render the images.
				switch ( m_nImagePlace )
				{
					case	IMPLACE_LEFT:
						// Create image rectangle.
						rcimg.Left()++;
						rcimg.Top()++;
						rcimg.Right() = rcimg.Left() + cx;

						// Adjust label rectangle.
						rc.Left() += cx + 4;
						break;

					case	IMPLACE_TOP:
						// Create image rectangle.
						rcimg.Top()++;

						// Adjust the label rectangle.
						rc.Top() += cy + 4;
						break;
					
					case	IMPLACE_RIGHT:
						// Create image rectangle.
						rcimg.Left() = rcimg.Right() - ( cx + 1 );
						rcimg.Top()++;
						rcimg.Right() = rcimg.Left() + cx;

						// Adjust the label rectangle.
						rc.Right() -= cx + 4;
						break;

					case	IMPLACE_BOTTOM:
						// Create image rectangle.
						rcimg.Top() = rcimg.Bottom() - ( cy + 1 );

						// Adjust the label rectangle.
						rc.Bottom() -= cy + 4;
						break;
				}
				
				// Set the height of the image rectangle to
				// the height of the image unless the image
				// place is left or right.
				if ( m_nImagePlace != IMPLACE_LEFT && m_nImagePlace != IMPLACE_RIGHT )
					rcimg.Bottom() = rcimg.Top() + cy;

				// Render the image.
				ClsDrawTools::RenderBitmap( dc, m_hImages, nImageCount == 1 ? 0 : (( IsHot() || IsDown()) ? 1 : 0 ), rcimg, IsWindowEnabled() ? 0 : ClsDrawTools::CDSF_DISABLED );
			}
		}

		// Do we have the focus and do we show it?
		if ( m_bRenderFocus && HasFocus() && ! ( GetUIState() & UISF_HIDEFOCUS )) dc.DrawFocusRect( rc );
		
		// Render the caption or, if there is no caption, the URL.
		// If there is also no URL we render nothing...
		ClsString str( m_hWnd );
		if ( str.GetStringLength() == 0 ) str = m_strURL;

		// Anything to render?
		if ( str.GetStringLength())
		{
			// Is the font valid?
			if ( m_urlFont.IsValid() == FALSE )
			{
				// No. Create an underlined version of
				// the system GUI font.
				ClsFont font;
				font.GetStockFont( DEFAULT_GUI_FONT );

				// Get the font information.
				LOGFONT lf;
				font.GetLogFont( &lf );

				// Make sure it is underlined.
				lf.lfUnderline = TRUE;

				// Create the font.
				m_urlFont.CreateFontIndirect(&lf);
			}

			// Select the font.
			ClsSelector sel( &dc, m_urlFont );

			// Render transparently.
			dc.SetBkMode( TRANSPARENT );

			// Set text color.
			dc.SetTextColor(( IsHot() || IsDown()) ? m_crHot : (( IsDown() || m_bIsVisited ) ? m_crVisited : m_crNormal ));

			// Determine alignment.
			DWORD dwFlags = 0L;
			if ( m_nLabelPlace == LBPLACE_LEFT ) dwFlags = ClsDrawTools::CDSF_LEFTALIGN;
			else if ( m_nLabelPlace == LBPLACE_RIGHT ) dwFlags = ClsDrawTools::CDSF_RIGHTALIGN;

			// Render the caption.
			ClsDrawTools::RenderText( dc, str, rc, ( IsWindowEnabled() ? 0 : ClsDrawTools::CDSF_DISABLED ) | ( GetUIState() & UISF_HIDEACCEL ? ClsDrawTools::CDSF_HIDEACCEL : 0 ) | dwFlags );
		}

		// Restore device context.
		dc.RestoreDC( sDC );
	}
}

// WM_PAINT message handler.
LRESULT ClsHyperLink::OnPaint( ClsDC *pDC )
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

// return the tooltip string.
BOOL ClsHyperLink::TipString( LPNMTTDISPINFO lpDispInfo )
{
	// Setup the correct text.
	if ( m_strURL.GetStringLength())
	{
		lpDispInfo->lpszText = m_strURL;
		return TRUE;
	}
	return FALSE;
}

// Window procedure.
LRESULT ClsHyperLink::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		// Print control.
		case	WM_PRINTCLIENT:
			// Render what?
			if ( lParam & PRF_ERASEBKGND ) SendMessage( WM_ERASEBKGND, wParam );
			if ( lParam & PRF_CLIENT     ) SendMessage( WM_PAINT, wParam );
			return 0;

		// We need to set the cursor.
		case	WM_SETCURSOR:
			// Mouse on the client?
			if ( LOWORD( lParam ) == HTCLIENT )
			{
				// Set the hand cursor.
				if ( m_curHand ) ::SetCursor( m_curHand );
			}
			return 0;
		
		// Setup the font.
		case	WM_SETFONT:
			// Get the font.
			ClsFont *pFont = ClsFont::FromHandle(( HFONT )wParam );

			// Get the font information.
			LOGFONT         lf;
			if ( pFont->GetLogFont( &lf ))
			{
				// Set the underline style.
				lf.lfUnderline = TRUE;

				// Create the font.
				m_urlFont.CreateFontIndirect( &lf );
			}
			break;
	}

	// Call the base class.
	return ClsHotButton::WindowProc( uMsg, wParam, lParam );
}

// For the layout engine.
BOOL ClsHyperLink::OnGetMinSize( ClsSize& szMinSize )
{
	// Get the DC.
	ClsGetDC dc( this );

	// Setup the font to use.
	ClsSelector sel( &dc, m_urlFont );

	// Do we have a caption?
	ClsString str( GetSafeHWND());
	if ( str.GetStringLength() == 0 ) str = m_strURL;

	// Any text?
	if ( str.GetStringLength())
	{
		ClsRect rc;
		dc.DrawText( str, rc, DT_CALCRECT );
		szMinSize.Add( rc.Size());
	}

	// Any images?
	if ( m_hImages && ImageList_GetImageCount( m_hImages )) 
	{
		// Get the size of the bitmaps.
		int cx, cy;
		if ( ImageList_GetIconSize( m_hImages, &cx, &cy ))
		{
			// Make room.
			cx += 2;
			cy += 2;

			// What place do we render the bitmap(s)?
			if ( m_nImagePlace == IMPLACE_LEFT || m_nImagePlace == IMPLACE_RIGHT ) {
				// Add the bitmap width to the minimum size
				// and check to see if the bitmap height is
				// higher than the font.
				szMinSize.CX() += cx + 4;
				if ( cy > szMinSize.CY()) 
					szMinSize.CY() = cy;
			} 
			else 
			{
				// Add the bitmap height to the minimum size
				// and check to see if the bitmap width is
				// larger than the label width.
				szMinSize.CY() += cy + 4;
				if ( cx > szMinSize.CX())
					szMinSize.CX() = cx;
			}
		}
	}
	return TRUE;
}