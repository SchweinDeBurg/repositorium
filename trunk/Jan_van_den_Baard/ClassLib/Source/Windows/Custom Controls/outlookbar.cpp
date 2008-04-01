//
// outlookbar.cpp
//
// (C) Copyright 2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "../../windows/window.h"
#include "../../gdi/paintdc.h"
#include "../../gdi/getdc.h"
#include "../../gdi/bufferdc.h"
#include "../../gdi/selector.h"
#include "../../tools/drawtools.h"
#include "outlookbar.h"

#include "../../tools/xpcolors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// OutlookButton structure size (V1)
#define ODS_SIZE_V1 sizeof( int ) + sizeof( LPCTSTR ) + sizeof( int ) + sizeof( UINT ) + ( 2 * sizeof( ClsRect )) + ( 2 * sizeof( DWORD ))

// Mouse tracking timer.
#define MOUSE_TRACK	0xDEAD
#define MOUSE_TIMEOUT	50

ClsOutlookBar::ClsOutlookBar()
{
	// Initialize data.
	m_crLabelColor		= ::GetSysColor( COLOR_CAPTIONTEXT );
	m_nLabelSysColor	= COLOR_CAPTIONTEXT;
	m_bUseSysColor		= TRUE;
	m_crBackgroundColor	= ::GetSysColor( COLOR_APPWORKSPACE );
	m_nBackgroundSysCol	= COLOR_APPWORKSPACE;
	m_bUseSysBgColor	= TRUE;
	m_bSmallImages		= FALSE;
	m_bTracking		= FALSE;
	m_nTopIndex		= 0;
}

ClsOutlookBar::~ClsOutlookBar()
{
	// Deallocate all captions.
	for ( int i = 0; i < m_Buttons.GetSize(); i++ )
	{
		if ( m_Buttons[ i ].m_pszButtonTitle )
			delete[] m_Buttons[ i ].m_pszButtonTitle;
	}
}

COLORREF ClsOutlookBar::SetLabelColor( COLORREF crColor, BOOL bRedraw /*= TRUE*/ )
{
	COLORREF crOldColor = m_crLabelColor;
	m_bUseSysColor = FALSE;
	m_crLabelColor = crColor;

	// Redraw if necessary and possible.
	if ( bRedraw && GetSafeHWND()) Invalidate();
	return crOldColor;
}

int ClsOutlookBar::SetLabelSysColor( int nSysColor, BOOL bRedraw /*= TRUE*/ )
{
	int nOldSysColor = m_nLabelSysColor;
	m_bUseSysColor = TRUE;
	m_nLabelSysColor = nSysColor;

	// Redraw if necessary and possible.
	if ( bRedraw && GetSafeHWND()) Invalidate();
	return nOldSysColor;
}

COLORREF ClsOutlookBar::SetBackgroundColor( COLORREF crColor, BOOL bRedraw /*= TRUE*/ )
{
	COLORREF crOldColor = m_crBackgroundColor;
	m_bUseSysBgColor = FALSE;
	m_crBackgroundColor = crColor;

	// Redraw if necessary and possible.
	if ( bRedraw && GetSafeHWND()) Invalidate();
	return crOldColor;
}

int ClsOutlookBar::SetBackgroundSysColor( int nSysColor, BOOL bRedraw /*= TRUE*/ )
{
	int nOldSysColor = m_nBackgroundSysCol;
	m_bUseSysBgColor = TRUE;
	m_nBackgroundSysCol = nSysColor;

	// Redraw if necessary and possible.
	if ( bRedraw && GetSafeHWND()) Invalidate();
	return nOldSysColor;
}

HIMAGELIST ClsOutlookBar::SetImageList( HIMAGELIST hImages, BOOL bSmallImages /*= FALSE*/, BOOL bRedraw /*= TRUE*/ )
{
	HIMAGELIST hOldImages;
	
	// Setup the new image list.
	if ( bSmallImages ) hOldImages = m_SmallImages.Attach( hImages );
	else		    hOldImages = m_LargeImages.Attach( hImages );

	// Redraw if necessary and possible.
	if ( bRedraw && GetSafeHWND()) Invalidate();
	return hOldImages;
}

void ClsOutlookBar::UseSmallImages( BOOL bUse /*= TRUE*/, BOOL bRedraw /*= TRUE*/ )
{
	// Did the flag actually change?
	if ( bUse != m_bSmallImages )
	{
		// If we want to use the small images they do need
		// to be present.
		if ( bUse == TRUE && ( HIMAGELIST )m_SmallImages == NULL )
			m_bSmallImages = FALSE;
		else
			// Do not use the small images.
			m_bSmallImages = bUse;

		// Redraw if necessary and possible.
		if ( bRedraw && GetSafeHWND()) Invalidate();
	}
}

BOOL ClsOutlookBar::InsertOutlookButton( LPCTSTR pszTitle, int nImageIndex, UINT nID, UINT nIDpred /*= 0*/, BOOL bRedraw /*= TRUE*/, DWORD dwData /*= 0L*/ )
{
	OutlookData	data;
	BOOL		bInsert = FALSE;
	int		nPos = -1;

	// Do we need to insert the button after another
	// button?
	if ( nIDpred )
	{
		// Look for the button with the given ID.
		for ( nPos = 0; nPos < m_Buttons.GetSize(); nPos++ )
		{
			// Is this the one?
			if ( m_Buttons[ nPos ].m_nButtonID == nIDpred )
			{
				// Yes. We must insert the new button.
				bInsert = TRUE;
				break;
			}
		}

		// Was the given predecessor found?
		if ( bInsert == FALSE )
			return FALSE;
	}

	// Allocate the caption.
	if ( pszTitle )
	{
		data.m_pszButtonTitle = new TCHAR[ _tcslen( pszTitle ) + 1 ];
		if ( data.m_pszButtonTitle == NULL )
			return FALSE;

		// Copy the title.
		_tcscpy(( TCHAR * )data.m_pszButtonTitle, pszTitle );
	}
	else
		data.m_pszButtonTitle = NULL;

	// Setup button structure.
	data.m_cbSize		 = sizeof( data );
	data.m_nButtonImageIndex = nImageIndex;
	data.m_nButtonID	 = nID;
	data.m_dwState		 = 0L;
	data.m_dwData		 = dwData;
	
	// Insert or add the button.
	if ( bInsert ) nPos = m_Buttons.InsertAt( data, nPos + 1 );
	else nPos = m_Buttons.Add( data );

	// OK?
	if ( nPos < 0 )
		return FALSE;

	// Redraw if necessary and possible.
	if ( bRedraw && GetSafeHWND()) Invalidate();
	return TRUE;
}

BOOL ClsOutlookBar::RemoveOutlookButton( UINT nID, BOOL bRedraw /*= TRUE*/ )
{
	// Look up the button to remove.
	int nIndex = GetButtonIndex( nID );
	if ( nIndex >= 0 )
	{
		// Free it's caption.
		if ( m_Buttons[ nIndex ].m_pszButtonTitle ) delete[] m_Buttons[ nIndex ].m_pszButtonTitle;

		// Remove it from the array.
		m_Buttons.RemoveAt( nIndex, 1 );

		// Redraw in necessary and possible.
		if ( bRedraw && GetSafeHWND()) Invalidate();
		return TRUE;
	}
	return FALSE;
}

BOOL ClsOutlookBar::SetOutlookButtonLabel( UINT nID, LPCTSTR pszTitle, BOOL bRedraw /*= TRUE*/ )
{
	// Look up the button.
	int nIndex = GetButtonIndex( nID );
	if ( nIndex >= 0 )
	{
		// Allocate the new caption.
		LPTSTR pszNew = NULL;
		if ( pszTitle )
		{
			pszNew = new TCHAR[ _tcslen( pszTitle ) + 1 ];
			if ( pszNew == NULL )
				return FALSE;

			// Copy the title.
			_tcscpy( pszNew, pszTitle );
		}

		// Free the old title if available.
		if ( m_Buttons[ nIndex ].m_pszButtonTitle ) delete[] m_Buttons[ nIndex ].m_pszButtonTitle;

		// Setup the new title.
		m_Buttons[ nIndex ].m_pszButtonTitle = pszNew;

		// Redraw if necessary and possible.
		if ( bRedraw && GetSafeHWND()) Invalidate();
		return TRUE;
	}
	return FALSE;
}

BOOL ClsOutlookBar::SetOutlookButtonImage( UINT nID, int nImageIndex, BOOL bRedraw /*= TRUE*/ )
{
	// Look up the button.
	int nIndex = GetButtonIndex( nID );
	if ( nIndex >= 0 )
	{
		// Setup the new title.
		m_Buttons[ nIndex ].m_nButtonImageIndex = nImageIndex;

		// Redraw if necessary and possible.
		if ( bRedraw && GetSafeHWND()) Invalidate();
		return TRUE;
	}
	return FALSE;
}

BOOL ClsOutlookBar::EnableOutlookButton( UINT nID, BOOL bEnable /*= TRUE*/, BOOL bRedraw /*= TRUE*/ )
{
	// Look up the button.
	int nIndex = GetButtonIndex( nID );
	if ( nIndex >= 0 )
	{
		// Setup the new state.
		if ( bEnable ) m_Buttons[ nIndex ].m_dwState &= ~OBS_DISABLED;
		else	       m_Buttons[ nIndex ].m_dwState |= OBS_DISABLED;

		// Redraw if necessary and possible.
		if ( bRedraw && GetSafeHWND()) Invalidate();
		return TRUE;
	}
	return FALSE;
}


int ClsOutlookBar::GetButtonIndex( UINT nID ) const
{
	// Iterate the button array.
	for ( int i = 0; i < m_Buttons.GetSize(); i++ )
	{
		// Is this the one?
		if ( m_Buttons[ i ].m_nButtonID == nID )
			return i;
	}
	// Not found.
	return -1;
}

LRESULT ClsOutlookBar::OnEraseBkgnd( ClsDC *pDC )
{
	// All background rendering is done in the
	// OnPaint() method.
	return 1;
}

void ClsOutlookBar::OnDrawOutlookButton( ClsDC *pDC, LPOUTLOOKDATA pData )
{
	HFONT oldFont = NULL;
	ClsGetDC *dc = NULL;
	ClsDC *pdc = pDC;

	// If a DC was not passed we create one
	// ourselves.
	if ( pdc == NULL )
	{
		// Get the DC and setup the font.
		dc	= new ClsGetDC( this );
		oldFont = ( HFONT )dc->SelectObject(( HFONT )SendMessage( WM_GETFONT ));
		pdc	= dc;
	}

	// Setup the background color.
	COLORREF crOld = pdc->SetBkColor( m_bUseSysBgColor ? ::GetSysColor( m_nBackgroundSysCol ) : m_crBackgroundColor );

	// Render the edge or, if were not hot, the rectangle background. 
	if ( pData->m_dwState & OBS_ISHOT ) pdc->DrawEdge( pData->m_IconBounds, Tracking() ? BDR_SUNKENINNER : BDR_RAISEDOUTER, BF_RECT );
	else ClsDrawTools::DrawRectangle( *pdc, pData->m_IconBounds, m_bUseSysBgColor ? ::GetSysColor( m_nBackgroundSysCol ) : m_crBackgroundColor );

	// Determine the color of the image and label if the
	// button is disabled.
	ClsColor col( m_bUseSysBgColor ? ::GetSysColor( m_nBackgroundSysCol ) : m_crBackgroundColor );
	if ( pData->m_dwState & OBS_DISABLED )
	{
		// Depending on the luminance we darken or
		// brighten the background color which we
		// then use for disabled rendering.
		if ( col.Lightness() >= 110 ) col.DarkenColor( 0.3, 0.3, 0.3 );
		else col.LightenColor( 0.3, 0.3, 0.3 );
	}
	else
		// Standard label color.
		col = m_bUseSysColor ? ::GetSysColor( m_nLabelSysColor ) : m_crLabelColor;

	// Disabled image?
	if ( pData->m_dwState & OBS_DISABLED )
		// Render using the computed color.
		ClsDrawTools::RenderMonoBitmap( *pdc, m_bSmallImages ? m_SmallImages : m_LargeImages, pData->m_nButtonImageIndex, pData->m_IconBounds, col, 0 );
	else
		// Full color image.
		ClsDrawTools::RenderBitmap( *pdc, m_bSmallImages ? m_SmallImages : m_LargeImages, pData->m_nButtonImageIndex, pData->m_IconBounds, pData->m_dwState & OBS_DISABLED ? ClsDrawTools::CDSF_DISABLED : 0 );

	// Do we have a label?
	if ( pData->m_pszButtonTitle )
	{
		// Set the text color.
		DWORD dwFlags;
		COLORREF crOldText = pdc->SetTextColor( col );

		// Small images?
		if ( m_bSmallImages ) 
		{
			pData->m_TitleBounds.Deflate( 2, 0 );
			dwFlags = DT_EXPANDTABS | DT_WORDBREAK | DT_VCENTER;
		}
		else 
			dwFlags = DT_EXPANDTABS | DT_WORDBREAK | DT_CENTER | DT_VCENTER;

		// Render the label.
		pdc->DrawText( pData->m_pszButtonTitle, _tcslen( pData->m_pszButtonTitle ), pData->m_TitleBounds, dwFlags );

		// Small images?
		if ( m_bSmallImages ) pData->m_TitleBounds.Inflate( 2, 0 );

		// Restore the color.
		pdc->SetTextColor( crOldText );
	}

	// Restore the DC.
	pdc->SetBkColor( crOld );

	// If we created the DC ourselves we
	// destroy it now.
	if ( dc ) 
	{
		dc->SelectObject( oldFont );
		delete dc;
	}
}

void ClsOutlookBar::OnGetOutlookButtonSize( ClsDC *pDC, ClsSize& szIcon, ClsSize& szText, LPOUTLOOKDATA pData )
{
	// Determine the icon size.
	if ( m_bSmallImages ) m_SmallImages.GetIconSize( szIcon );
	else m_LargeImages.GetIconSize( szIcon );	

	// Measure the text. When the text is wider than
	// the client area minus the offsets when we measure
	// it multi line.
	if ( pData->m_pszButtonTitle )
	{
		ClsRect rc = GetClientRect();
		rc.Deflate( 6, 0 );

		// When using small images the label is printed
		// right of the image...
		if ( m_bSmallImages ) rc.Right() -= 4 + szIcon.CX();

		// Measure the rectangle.
		pDC->DrawText( pData->m_pszButtonTitle, rc, DT_CALCRECT | DT_WORDBREAK );
		szText.CX() = rc.Width();
		szText.CY() = rc.Height();
	}
	else
		// No label.
		szText.CX() = szText.CY() = 0;
}

void ClsOutlookBar::OnDrawOutlookBackground( ClsDC *pDC, ClsRect& rc )
{
	// Render the background.
	pDC->FillSolidRect( rc, m_bUseSysBgColor ? ::GetSysColor( m_nBackgroundSysCol ) : m_crBackgroundColor );
} 

void ClsOutlookBar::PaintControl( ClsDC *pDC )
{
	// Determine the "real" client size which is the
	// client size without the caption height.
	ClsRect rc = GetClientRect();
	rc.Top() += m_nCaptionHeight;

	// Determine client bottom.
	int nBottom = rc.Bottom();

	// Setup a buffer DC in which all rendering 
	// will take place.
	ClsBufferDC bdc( pDC, rc, TRUE );

	// Render the background.
	OnDrawOutlookBackground( &bdc, rc );

	// Setup the font.
	ClsSelector sel( &bdc, ( HFONT )SendMessage( WM_GETFONT ));
	ClsSize szIcon, szText;

	// We start six pixels down from the top.
	int nYPos = rc.Top() + 6;

	// Do we start at the first button or
	// an offset?
	if ( m_nTopIndex )
	{
		// Invalidate the positions of the buttons
		// above the top index.
		for ( int i = 0; i < m_nTopIndex; i++ )
		{
			m_Buttons[ i ].m_IconBounds.Top()     = -1;
			m_Buttons[ i ].m_TitleBounds.Top()    = -1;
			m_Buttons[ i ].m_IconBounds.Bottom()  = -1;
			m_Buttons[ i ].m_TitleBounds.Bottom() = -1;
		}
	}

	// Iterate the buttons starting at the
	// top index.
	for ( int i = m_nTopIndex; i < m_Buttons.GetSize(); i++ )
	{
		// Obtain sizes.
		OnGetOutlookButtonSize( &bdc, szIcon, szText, &m_Buttons[ i ] );

		// Do we need to use the small images?
		if ( m_bSmallImages )
		{
			// Increase the size of the icons by
			// two pixels in each direction.
			szIcon.CX() += 2;
			szIcon.CY() += 2;

			// Compute the icon bounds.
			m_Buttons[ i ].m_IconBounds.Left()   = 6;
			m_Buttons[ i ].m_IconBounds.Top()    = nYPos;
			m_Buttons[ i ].m_IconBounds.Right()  = m_Buttons[ i ].m_IconBounds.Left() + szIcon.CX();
			m_Buttons[ i ].m_IconBounds.Bottom() = m_Buttons[ i ].m_IconBounds.Top() + max( szIcon.CY(), szText.CY());

			// And the title bounds.
			m_Buttons[ i ].m_TitleBounds.Left()   = m_Buttons[ i ].m_IconBounds.Right();
			m_Buttons[ i ].m_TitleBounds.Top()    = nYPos;
			m_Buttons[ i ].m_TitleBounds.Right()  = m_Buttons[ i ].m_TitleBounds.Left() + szText.CX() + 4;
			m_Buttons[ i ].m_TitleBounds.Bottom() = m_Buttons[ i ].m_TitleBounds.Top() + max( szText.CY(), szIcon.CY());

			// Increase the vertical position. Spacing is
			// six pixels.
			nYPos += max( szIcon.CY(), szText.CY()) + 6;
		}
		else
		{
			// Increase icon size by four pixels
			// in each direction.
			szIcon.CX() += 4;
			szIcon.CY() += 4;

			// Compute icon bounds.
			m_Buttons[ i ].m_IconBounds.Left()   = ( rc.Width() / 2 ) - ( szIcon.CX() / 2 );
			m_Buttons[ i ].m_IconBounds.Top()    = nYPos;
			m_Buttons[ i ].m_IconBounds.Right()  = m_Buttons[ i ].m_IconBounds.Left() + szIcon.CX();
			m_Buttons[ i ].m_IconBounds.Bottom() = m_Buttons[ i ].m_IconBounds.Top() + szIcon.CY();

			// Increase vertical offset for the title.
			nYPos += szIcon.CY();
			
			// Compute the title bounds.
			m_Buttons[ i ].m_TitleBounds.Left()   = ( rc.Width() / 2 ) - ( szText.CX() / 2 );
			m_Buttons[ i ].m_TitleBounds.Top()    = nYPos;
			m_Buttons[ i ].m_TitleBounds.Right()  = m_Buttons[ i ].m_TitleBounds.Left() + szText.CX();
			m_Buttons[ i ].m_TitleBounds.Bottom() = m_Buttons[ i ].m_TitleBounds.Top() + szText.CY();

			// Increase the vertical position. Spacing is
			// six pixels.
			nYPos += szText.CY() + 6;
		}

		// Is the icon still in the visible area
		// of the control?
		if ( m_Buttons[ i ].m_IconBounds.Top() < rc.Bottom() && m_Buttons[ i ].m_TitleBounds.Bottom() > rc.Top())
			OnDrawOutlookButton( &bdc, &m_Buttons[ i ] );
	}

	// Is the top index the last button in the
	// array?
	if ( m_nTopIndex == m_Buttons.GetSize() - 1 )
		// Yes. Hide the scroll-down button.
		m_sbDown.ShowWindow( SW_HIDE );
	else
	{
		// Find out the "lowest" part of the
		// button.
		int nLastIndex = m_Buttons.GetSize() - 1;
		int nButtonBottom = m_Buttons[ nLastIndex ].m_TitleBounds.Bottom();
		if ( m_Buttons[ nLastIndex ].m_IconBounds.Bottom() > nButtonBottom )
			nButtonBottom = m_Buttons[ nLastIndex ].m_IconBounds.Bottom();

		// Determine whether or not to show
		// the scroll down button.
		m_sbDown.ShowWindow(( nButtonBottom <= nBottom ) ? SW_HIDE : SW_SHOW );
	}

	// Determine whether or not to show the
	// scroll up button.
	m_sbUp.ShowWindow( m_nTopIndex ? SW_SHOW : SW_HIDE );
}

LRESULT ClsOutlookBar::OnPaint( ClsDC *pDC )
{
	// Is the passed DC valid?
	if ( pDC ) PaintControl( pDC );
	else
	{
		// Create a DC and paint the control.
		ClsPaintDC pdc( this );
		PaintControl( &pdc );
	}
	return 0;
}

LRESULT ClsOutlookBar::OnSize( UINT nSizeType, int nWidth, int nHeight )
{
	// The base class first.
	LRESULT res = ClsPanel::OnSize( nSizeType, nWidth, nHeight );

	// Get the client rectangle and adjust
	// for the caption and the 4 pixel margin.
	ClsRect  rc = GetClientRect();
	rc.Deflate( 4, 4 );
	rc.Top() += m_nCaptionHeight;

	// Adjust for the "Up" button size.
	rc.Left() = rc.Right() - ::GetSystemMetrics( SM_CXHSCROLL );
	rc.Bottom() = rc.Top() + ::GetSystemMetrics( SM_CYHSCROLL );
	m_sbUp.MoveWindow( rc );

	// Get the client rectangle and adjust
	// for the caption and the 4 pixel margin.
	rc = GetClientRect();
	rc.Deflate( 4, 4 );
	rc.Top() += m_nCaptionHeight;

	// Adjust for the "Down" button size.
	rc.Top() = rc.Bottom() - ::GetSystemMetrics( SM_CXHSCROLL );
	rc.Left() = rc.Right() - ::GetSystemMetrics( SM_CYHSCROLL );
	m_sbDown.MoveWindow( rc );
	return res;
}

LRESULT ClsOutlookBar::OnCreate( LPCREATESTRUCT pCS )
{
	// Call the base class first.
	LRESULT res = ClsPanel::OnCreate( pCS );
	if ( res != -1 )
	{
		// Create the scroll buttons.
		if ( m_sbUp.Create( this, ClsRect( -1, -1, -1, -1 ), 0xDEAD, NULL ) == FALSE )
			return -1;
		m_sbUp.Up() = TRUE;
		if ( m_sbDown.Create( this, ClsRect( -1, -1, -1, -1 ), 0xDEAD, NULL ) == FALSE )
			return -1;
	}
	return res;
}

LRESULT ClsOutlookBar::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// Was it the scroll up button?
	if ( hWndCtrl == m_sbUp.GetSafeHWND())
	{
		// Adjust the top index.
		if (( m_nTopIndex-- ) < 0 ) m_nTopIndex = 0;
		Invalidate();
	}
	// Was it the scroll down button?
	else if ( hWndCtrl == m_sbDown.GetSafeHWND())
	{
		// Adjust the top index.
		m_nTopIndex++;
		Invalidate();
	}
	// Pass to the base class.
	return ClsPanel::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

LRESULT ClsOutlookBar::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case	WM_PRINTCLIENT:
		{
			// Render what?
			if ( lParam & PRF_ERASEBKGND ) SendMessage( WM_ERASEBKGND, wParam );
			if ( lParam & PRF_CLIENT     ) SendMessage( WM_PAINT, wParam );
			return 0;
		}

		case	WM_LBUTTONDOWN:
		{
			// Is there a hot button?
			if ( m_nHotIndex != -1 )
			{
				// If we are not tracking yet we start
				// doing so now.
				if ( m_bTracking == FALSE )
				{
					// Setup tracked button.
					m_nTrackingIndex = m_nHotIndex;
					m_bTracking	 = TRUE;

					// Capture the mouse.
					SetCapture();

					// Render the new button state.
					OnDrawOutlookButton( NULL, &m_Buttons[ m_nHotIndex ] );
				}
			}
			break;
		}

		case	WM_LBUTTONUP:
		{
			// Do we have the capture?
			if ( GetCapture() == this || uMsg != WM_LBUTTONUP )
			{
				// Release it now.
				ReleaseCapture();
				m_bTracking = FALSE;
			}
			else
				// If we did not have the capture we do not
				// notify a button click.
				break;

			// Do we have a hot button?
			if ( m_nHotIndex != -1 )
			{
				// Get mouse position and
				// client rectangle.
				ClsPoint pt( LOWORD( lParam ), HIWORD( lParam ));
				ClsRect rc = GetClientRect();
				rc.Top() += m_nCaptionHeight;

				// No longer hot.
				m_Buttons[ m_nHotIndex ].m_dwState &= ~OBS_ISHOT;

				// Re-render the button.
				OnDrawOutlookButton( NULL, &m_Buttons[ m_nHotIndex ] );

				// Is the mouse still located over the client area and
				// are we not canceled?
				if ( uMsg == WM_LBUTTONUP && pt.Y() <= rc.Bottom())
				{
					// Save the index of the hot button here because
					// the event might trigger something which
					// resets the member.
					int nIndex = m_nHotIndex;

					// Notify the button click.
					OnOutlookButtonClicked( m_Buttons[ nIndex ].m_nButtonID );
				}
			}
			break;
		}

		case	WM_TIMER:
			// Our tracking timer?
			if ( wParam == MOUSE_TRACK )
			{
				// Get the mouse position.
				ClsPoint pt;
				::GetCursorPos( pt );
				ScreenToClient( pt );

				// And the client rectangle.
				ClsRect rc = GetClientRect();
				rc.Top() += m_nCaptionHeight;

				// Is the mouse still located over the
				// client area of the control?
				if ( ! rc.PtInRect( pt ))
				{
					// Release the capture if we own it.
					if ( GetCapture() == this )
					{
						ReleaseCapture();
						m_bTracking = FALSE;
					}

					// Do we have a hot button?
					if ( m_nHotIndex != -1 )
					{
						// It no longer is hot.
						m_Buttons[ m_nHotIndex ].m_dwState &= ~OBS_ISHOT;
						OnDrawOutlookButton( NULL, &m_Buttons[ m_nHotIndex ] );
						m_nHotIndex = -1;
					}

					// Stop the timer.
					KillTimer( MOUSE_TRACK );
				}
			}
			break;

		case	WM_MOUSEMOVE:
		{
			// Wrap the mouse coordinates.
			ClsPoint pt( LOWORD( lParam ), HIWORD( lParam ));

			// Start the tracking timer.
			SetTimer( MOUSE_TRACK, MOUSE_TIMEOUT );

			// Start with no hot button.
			m_nHotIndex = -1;
			for ( int i = 0; i < m_Buttons.GetSize(); i++ )
			{
				// Is the mouse located over this button?
				if (( m_Buttons[ i ].m_IconBounds.PtInRect( pt ) ||
				      m_Buttons[ i ].m_TitleBounds.PtInRect( pt )) &&
				      ! ( m_Buttons[ i ].m_dwState & OBS_DISABLED ))
				{
					// Are we not tracking the mouse or is
					// this the tracked button?
					if ( m_bTracking == FALSE || i == m_nTrackingIndex )
					{
						// If it is not marked as hot we make
						// it hot now.
						if ( ! ( m_Buttons[ i ].m_dwState & OBS_ISHOT ))
						{
							m_Buttons[ i ].m_dwState |= OBS_ISHOT;
							OnDrawOutlookButton( NULL, &m_Buttons[ i ] );
						}

						// Mark the index.
						m_nHotIndex = i;
					}
				}
				else
				{
					// Is this button marked as hot?
					if ( m_Buttons[ i ].m_dwState & OBS_ISHOT )
					{
						// It no longer is.
						m_Buttons[ i ].m_dwState &= ~OBS_ISHOT;
						OnDrawOutlookButton( NULL, &m_Buttons[ i ] );
					}
				}
			}
		}
		break;
	}

	// Call the baseclass.
	return ClsPanel::WindowProc( uMsg, wParam, lParam );
}

void ClsOutlookBar::OnOutlookButtonClicked( UINT nID )
{
	// Do we have a parent? If so
	// notify it.
	ClsWindow *pParent = GetParent();
	if ( pParent ) 
		pParent->SendMessage( WM_COMMAND, MAKEWPARAM( nID, 0 ), ( LPARAM )GetSafeHWND());
}

// Return the label of the button with
// the specified ID.
LPCTSTR ClsOutlookBar::GetOutlookButtonLabel( UINT nID ) const
{
	int nIndex = GetButtonIndex( nID );
	if ( nIndex >= 0 )
		return m_Buttons[ nIndex ].m_pszButtonTitle;
	return NULL;
}

// Returns the image index of the button with
// the specified ID.
int ClsOutlookBar::GetOutlookButtonImage( UINT nID ) const
{
	int nIndex = GetButtonIndex( nID );
	if ( nIndex >= 0 )
		return m_Buttons[ nIndex ].m_nButtonImageIndex;
	return -1;
}

// Return the enabled state of the button with
// the specified ID.
BOOL ClsOutlookBar::IsOutlookButtonEnabled( UINT nID ) const
{
	int nIndex = GetButtonIndex( nID );
	if ( nIndex >= 0 )
		return ( m_Buttons[ nIndex ].m_dwState & OBS_DISABLED ) ? FALSE : TRUE;
	return FALSE;
}

//**** ClsXPOutlookBar
void ClsXPOutlookBar::OnDrawOutlookButton( ClsDC *pDC, LPOUTLOOKDATA pData )
{
	// "Old" style rendering?
	if ( m_bOldStyle )
	{
		// Yes. Leave it up to the base class.
		ClsOutlookBar::OnDrawOutlookButton( pDC, pData );
		return;
	}

	// Local data.
	HFONT oldFont = NULL;
	ClsGetDC *dc = NULL;
	ClsDC *pdc = pDC;

	// If a DC was not passed we create one
	// ourselves.
	if ( pdc == NULL )
	{
		// Get the DC and setup the font.
		dc	= new ClsGetDC( this );
		oldFont = ( HFONT )dc->SelectObject(( HFONT )SendMessage( WM_GETFONT ));
		pdc	= dc;
	}

	// Setup the background color.
	COLORREF crOld = pdc->SetBkColor( XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_BACKGROUND ));

	// Render the edge or, if were not hot, the rectangle background. 
	if ( pData->m_dwState & OBS_ISHOT ) pdc->OutlinedRectangle( pData->m_IconBounds, XPColors.GetXPColor( ClsXPColors::XPC_OUTER_SELECTION ), Tracking() ? XPColors.GetXPColor( ClsXPColors::XPC_INNER_CHECKED_SELECTED ) : XPColors.GetXPColor( ClsXPColors::XPC_INNER_SELECTION )); 
	else pdc->FillSolidRect( pData->m_IconBounds, XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_BACKGROUND ));

	// Render the image.
	DWORD dwFlags = 0L;
	ClsRect rc = pData->m_IconBounds;
	if ( pData->m_dwState & OBS_DISABLED ) dwFlags |= ClsDrawTools::CDSF_DISABLED;
	if ( pData->m_dwState & OBS_ISHOT && ! Tracking()) dwFlags |= ClsDrawTools::CDSF_HOT;
	ClsDrawTools::RenderXPBitmap( *pdc, m_bSmallImages ? m_SmallImages : m_LargeImages, pData->m_nButtonImageIndex, rc, dwFlags );

	// Do we have a label?
	if ( pData->m_pszButtonTitle )
	{
		// Set the text color.
		DWORD dwFlags;
		COLORREF crOldText = pdc->SetTextColor( pData->m_dwState & OBS_DISABLED ? XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_DISABLED ) : XPColors.GetXPColor( ClsXPColors::XPC_TEXT ));

		// Small images?
		if ( m_bSmallImages ) 
		{
			pData->m_TitleBounds.Deflate( 2, 0 );
			dwFlags = DT_EXPANDTABS | DT_WORDBREAK | DT_VCENTER;
		}
		else 
			dwFlags = DT_EXPANDTABS | DT_WORDBREAK | DT_CENTER | DT_VCENTER;

		// Render the label.
		pdc->DrawText( pData->m_pszButtonTitle, _tcslen( pData->m_pszButtonTitle ), pData->m_TitleBounds, dwFlags );

		// Small images?
		if ( m_bSmallImages )
			pData->m_TitleBounds.Inflate( 2, 0 );

		// Restore the color.
		pdc->SetTextColor( crOldText );
	}

	// Restore the DC.
	pdc->SetBkColor( crOld );

	// If we created the DC ourselves we
	// destroy it now.
	if ( dc ) 
	{
		dc->SelectObject( oldFont );
		delete dc;
	}
}

void ClsXPOutlookBar::OnDrawOutlookBackground( ClsDC *pDC, ClsRect& rc )
{
	// Render the background.
	if ( m_bOldStyle )
	{
		// Use "Old" style rendering.
		ClsOutlookBar::OnDrawOutlookBackground( pDC, rc );
		return;
	}
	pDC->FillSolidRect( rc, XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_BACKGROUND ));
} 
