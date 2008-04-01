//
// infocontrol.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "infocontrol.h"
#include "../../gdi/paintdc.h"
#include "../../gdi/bufferdc.h"
#include "../../gdi/getdc.h"
#include "../../gdi/selector.h"
#include "../../coords/rect.h"
#include "../../strings/string.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Alignment flags.
#define TF_CENTER		(1<<0)	// Center text.
#define TF_RIGHT		(1<<1)	// Right align text.

// Font flags.
#define TF_BOLD			(1<<2)	// Add bold style.
#define TF_ITALIC		(1<<3)	// Add italic style.
#define TF_UNDERLINE		(1<<4)	// Add underline style.
#define TF_STYLEMASK		( TF_BOLD | TF_ITALIC | TF_UNDERLINE )

// Misc. flags.
#define TF_NOCOLOR		(1<<5)	// Do not change colors.

// Constructor. Defaults the data.
ClsInfoControl::ClsInfoControl()
{
	// Setup default values.
	m_bVertCenter = TRUE;

	// Setup default background color.
	m_crBkColor = ::GetSysColor( COLOR_BTNFACE );
}

// Destructor. Cleans up...
ClsInfoControl::~ClsInfoControl()
{
	// Destroy fonts in the array.
	for ( int i = 0; i < m_aFonts.GetSize(); i++ )
	{
		// Valid?
		if ( m_aFonts[ i ].m_hFont )
			// Destroy it.
			::DeleteObject( m_aFonts[ i ].m_hFont );
	}
}

// Compute the total height of the text.
int ClsInfoControl::TotalHeight( ClsDC *pDC, LPCTSTR pszText )
{
	_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

	int		nNumLines = 1;
	ClsSize		size;
	ClsString	str;

	// If there is no input we use the control text.
	if ( pszText == NULL )
	{
		str = m_hWnd;
		pszText = str;
	}

	// Setup the font.
	ClsFont font;
	GetFont( font );
	ClsSelector sel( pDC, font );

	// Count the number of lines in the text.
	while ( *pszText )
	{
		if ( *pszText == _T( '\n' ))
			nNumLines++;
		pszText = _tcsinc( pszText );
	}

	// Compute the height of  a single line.
	size = pDC->GetTextExtent( _T( " " ), 1 );
	
	// Return the total height.
	return ( int )( nNumLines * size.CY()) + 4;
}

// Compute the total width of the text.
int ClsInfoControl::TotalWidth( ClsDC *pDC, LPCTSTR pszText )
{
	_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

	int		nWidth = 0, nLineWidth = 0;
	DWORD		dwFlags = 0;
	ClsString	str;

	// Use the control text if no text
	// was given.
	if ( pszText == NULL )
	{
		str = m_hWnd;
		pszText = str;
	}

	// Snapshot the DC.
	int nSDC = pDC->SaveDC();

	// Get the font to use.
	GetFont( m_cfUseFont );
	m_hInitFont = m_cfUseFont;

	// Valid font?
	if ( m_hInitFont == NULL )
	{
		// Use the system GUI font.
		m_hInitFont = ( HFONT )::GetStockObject( DEFAULT_GUI_FONT );
		m_cfUseFont.Attach( m_hInitFont, FALSE );
	}

	// Setup the font.
	HFONT hOrigFont = ( HFONT )pDC->SelectObject( m_cfUseFont );

	// Iterate text.
	while ( *pszText )
	{
		// Evaluate character.
		switch ( *pszText )
		{
			case	_T( '\33' ):
				// Command sequence.
				pszText = _tcsinc( pszText );
				pszText = ParseCommandSequence( pDC, pszText, dwFlags );
				break;

			default:
				// Reset counter.
				int i = 0;
				LPCTSTR pszTmp = pszText;

				// Count the printable characters.
				while ( *pszTmp && ( *pszTmp != _T( '\33' )) && ( *pszTmp != _T( '\n' ))) { pszTmp = _tcsinc( pszTmp ); i++; }

				// Any printable characters?
				if ( i )
				{
					// Determine their width.
					ClsSize size = pDC->GetTextExtent( pszText, i );
					nLineWidth += size.CX();
				}

				// Newline or end of text?
				if ( *pszTmp == _T( '\n' ) || pszTmp == _T( '\0' ))
				{
					// Is it wider than the previous ones?
					if ( nLineWidth > nWidth )
						nWidth = nLineWidth;

					// Continue if this is a newline.
					if ( *pszTmp == _T( '\n' ))
						pszTmp = _tcsinc( pszTmp );
					
					// Reset line width.
					nLineWidth = 0;
				}

				// Adjust pointer.
				pszText = pszTmp;
				break;
		}
	}

	// Make sure we got the last line aswell.
	if ( nLineWidth > nWidth )
		nWidth = nLineWidth;
	
	// Restore the original font.
	pDC->SelectObject( hOrigFont );

	// Detach the used font.
	m_cfUseFont.Detach();

	// Restore the device context.
	pDC->RestoreDC( nSDC );

	// Return the width.
	return nWidth + 4;
}

// Get the digits from the text.
LPCTSTR ClsInfoControl::GetDigits( LPCTSTR pszText, int& nNum )
{
	TCHAR		tcNumber[ 4 ];
	int		i = 0;

	// End of the string?
	if ( *pszText == _T( '\0' ))
	{
		// Set the number to 0.
		nNum = 0;
		return pszText;
	}

	// Scan for digits.
	while ( _istdigit( *pszText ) && i < 3 )
	{
		// Store the number in the conversion string.
		tcNumber[ i ] = *pszText;
		pszText = _tcsinc( pszText );
		i++;
	}

	// 0-terminate the number string.
	tcNumber[ i ] = '\0';

	// Convert the string to an integer.
	nNum = ( int )_ttoi( tcNumber );

	// Return the current string position.
	return pszText;
}

// Change the current font style.
BOOL ClsInfoControl::ChangeFontStyle( ClsDC *pDC, DWORD dwFlags )
{
	// No styles? If there are no styles specified
	// we use the original font.
	if (( dwFlags & TF_STYLEMASK ) == 0 )
	{
		// Setup input font if necessary.
		if ( m_cfUseFont != m_hInitFont )
		{
			// Set it up.
			m_cfUseFont.Attach( m_hInitFont, FALSE );
			pDC->SelectObject( m_hInitFont );
		}
		return TRUE;
	}

	// First we look if we have created such a font
	// already.
	for ( int i = 0; i < m_aFonts.GetSize(); i++ )
	{
		// Is this the one?
		if ( m_aFonts[ i ].m_dwStyles == ( dwFlags & TF_STYLEMASK ))
		{
			// Did it really change?
			if ( m_cfUseFont != m_aFonts[ i ].m_hFont )
			{
				// Set it up.
				m_cfUseFont.Attach( m_aFonts[ i ].m_hFont, FALSE );
				pDC->SelectObject( m_aFonts[ i ].m_hFont );
			}
			return TRUE;
		}
	}

	// Get the current font.
	ClsFont *pFont = pDC->GetCurrentFont();

	// Valid?
	if ( pFont )
	{
		// Got font information.
		LOGFONT		lf;
		if ( pFont->GetLogFont( &lf ))
		{
			// Change the style accoording to the flags.
			lf.lfItalic	    = ( BYTE )( dwFlags & TF_ITALIC    ? TRUE : FALSE );
			lf.lfUnderline	    = ( BYTE )( dwFlags & TF_UNDERLINE ? TRUE : FALSE );
			lf.lfWeight	    = dwFlags & TF_BOLD ? FW_BOLD : FW_NORMAL;

			// Setup data structure.
			FontData fd;
			fd.m_dwStyles = ( dwFlags & TF_STYLEMASK );
			fd.m_hFont    = ::CreateFontIndirect( &lf );

			// Font created OK?
			if ( fd.m_hFont )
			{
				// Add it to the array.
				m_aFonts.Add( fd );

				// Setup new font.
				m_cfUseFont.Attach( fd.m_hFont, FALSE );
				pDC->SelectObject( fd.m_hFont );
				return TRUE;
			}
		}
	}
	return FALSE;
}

// Parse a command sequence.
LPCTSTR ClsInfoControl::ParseCommandSequence( ClsDC *pDC, LPCTSTR pszText, DWORD& dwFlags )
{
	int	r = 0, g = 0, b = 0;
	DWORD	dwOldFlags = dwFlags & ( TF_BOLD | TF_ITALIC | TF_UNDERLINE );

	// Check command character.
	switch ( *pszText )
	{
		case	_T( '\0' ):
			// End of string...
			break;

		case	_T( 'c' ):
			// Center
			dwFlags &= ~TF_RIGHT;
			dwFlags |= TF_CENTER;
			pszText = _tcsinc( pszText );
			break;

		case	_T( 'r' ):
			// Right
			dwFlags &= ~TF_CENTER;
			dwFlags |= TF_RIGHT;
			pszText = _tcsinc( pszText );
			break;
		
		case	_T( 'l' ):
			// Left
			dwFlags &= ~( TF_RIGHT | TF_CENTER );
			pszText = _tcsinc( pszText );
			break;

		case	_T( 't' ):
			// Text color RGB sequence.
			pszText = _tcsinc( pszText );
			pszText = GetDigits( pszText, r );
			if ( *pszText == _T( '\0' )) break;
			pszText = _tcsinc( pszText );
			pszText = GetDigits( pszText, g );
			if ( *pszText == _T( '\0' )) break;
			pszText = _tcsinc( pszText );
			pszText = GetDigits( pszText, b );

			// Setup the color.
			if ( ! ( dwFlags & TF_NOCOLOR ))
				pDC->SetTextColor( RGB( r, g, b ));
			r = g = b = 0;
			break;


		case	_T( 'b' ):
			// Background color RGB sequence.
			pszText = _tcsinc( pszText );
			pszText = GetDigits( pszText, r );
			if ( *pszText == _T( '\0' )) break;
			pszText = _tcsinc( pszText );
			pszText = GetDigits( pszText, g );
			if ( *pszText == _T( '\0' )) break;
			pszText = _tcsinc( pszText );
			pszText = GetDigits( pszText, b );

			// Setup the color.
			if ( ! ( dwFlags & TF_NOCOLOR ))
			{
				pDC->SetBkMode( OPAQUE );
				pDC->SetBkColor( RGB( r, g, b ));
			}
			r = g = b = 0;
			break;

		case	_T( 's' ):
			// Transparent mode.
			pszText = _tcsinc( pszText );
			if ( ! ( dwFlags & TF_NOCOLOR ))
				pDC->SetBkMode( TRANSPARENT );
			break;

		case	_T( 'n' ):
			// Reset styles.
			pszText = _tcsinc( pszText );
			dwFlags &= ~( TF_BOLD | TF_ITALIC | TF_UNDERLINE );
			break;

		case	_T( '+' ):
			pszText = _tcsinc( pszText );
			// Add which mode?
			switch ( *pszText )
			{
				case	_T( '\0' ):
					break;

				case	_T( 'b' ):
					// Bold
					pszText = _tcsinc( pszText );
					dwFlags |= TF_BOLD;
					break;

				case	_T( 'i' ):
					// Italic
					pszText = _tcsinc( pszText );
					dwFlags |= TF_ITALIC;
					break;

				case	_T( 'u' ):
					// Underline
					pszText = _tcsinc( pszText );
					dwFlags |= TF_UNDERLINE;
					break;
			}
			break;

		case	_T( '-' ):
			pszText = _tcsinc( pszText );
			// Add which mode?
			switch ( *pszText )
			{
				case	_T( '\0' ):
					break;

				case	_T( 'b' ):
					// Bold
					pszText = _tcsinc( pszText );
					dwFlags &= ~TF_BOLD;
					break;

				case	_T( 'i' ):
					// Italic
					pszText = _tcsinc( pszText );
					dwFlags &= ~TF_ITALIC;
					break;

				case	_T( 'u' ):
					// Underline
					pszText = _tcsinc( pszText );
					dwFlags &= ~TF_UNDERLINE;
					break;
			}
			break;

		case	_T( 'g' ):
			pszText = _tcsinc( pszText );
			switch ( *pszText )
			{
				case	_T( '\0' ):
					break;

				case	_T( 't' ):
					pszText = _tcsinc( pszText );
					pszText = GetDigits( pszText, r );
		                        if ( ! ( dwFlags & TF_NOCOLOR ))
						pDC->SetTextColor( ::GetSysColor( r ));
                                        r = 0;
                                        break;

				case    _T( 'b' ):
					pszText = _tcsinc( pszText );
					pszText = GetDigits( pszText, r );
					if ( ! ( dwFlags & TF_NOCOLOR ))
					{
						pDC->SetBkMode( OPAQUE );
						pDC->SetBkColor( ::GetSysColor( r ));
					}
                                        r = 0;
                                        break;
                        }
                        break;
	}

	// Change font if necessary.
	if (( dwFlags & ( TF_BOLD | TF_ITALIC | TF_UNDERLINE )) != dwOldFlags )
		ChangeFontStyle( pDC, dwFlags );

	// Return text pointer.
	return pszText;
}

// Compute horizontal position to render at.
int ClsInfoControl::XPos( ClsDC *pDC, LPCTSTR pszText, ClsRect *pDomain, DWORD dwFlags )
{
	int		nXPos, nWidth = 0;

	// Do not change colors.
	dwFlags |= TF_NOCOLOR;

	// Save the current font style flags so we
	// reset it after the text processing is done.
	DWORD dwCurrFlags = ( dwFlags & TF_STYLEMASK );
	
	// Iterate string.
	while ( *pszText && ( *pszText != _T( '\n' )))
	{
		// Evaluate character.
		switch ( *pszText )
		{
			case	_T( '\33' ):
				pszText = _tcsinc( pszText );
				pszText = ParseCommandSequence( pDC, pszText, dwFlags );
				break;

			default:
				// Get base.
				int i = 0;
				LPCTSTR pszTmp = pszText;

				// Count printable characters.
				while ( *pszTmp && ( *pszTmp != _T( '\33' )) && ( *pszTmp != _T( '\n' ))) { pszTmp = _tcsinc( pszTmp ); i++; }

				// Any printable characters?
				if ( i )
				{
					// Get size of the printable characters.
					ClsSize size = pDC->GetTextExtent( pszText, i );
					nWidth += size.CX();

					// Adjust text pointer.
					pszText = pszTmp;
				}
				break;
		}
	}

	// Reset font styles.
	ChangeFontStyle( pDC, dwCurrFlags );

	// We can change the colors again...
	dwFlags &= ~TF_NOCOLOR;

	// Compute horizontal offset.
	if (	  dwFlags & TF_CENTER ) nXPos = pDomain->Left() + (( pDomain->Width() / 2 ) - ( nWidth / 2 ));
	else if ( dwFlags & TF_RIGHT  ) nXPos = pDomain->Right() - nWidth - 2;
	else				nXPos = pDomain->Left();

	// Make sure we do not pass the left edge.
	if ( nXPos < pDomain->Left()) nXPos = pDomain->Left();

	// Return the value.
	return nXPos;
}

// Render the info text.
void ClsInfoControl::RenderInfoText( ClsDC *pDC, LPCTSTR pszText, ClsRect *pDomain )
{
	int		nXPos, nYPos = pDomain->Top(), nNumLines;
	DWORD		dwFlags = 0;

	// Save device context.
	int nSDC = pDC->SaveDC();

	// Get the font to use.
	GetFont( m_cfUseFont );
	m_hInitFont = m_cfUseFont;

	// Valid font?
	if ( m_hInitFont == NULL )
	{
		// Use the system GUI font.
		m_hInitFont = ( HFONT )::GetStockObject( DEFAULT_GUI_FONT );
		m_cfUseFont.Attach( m_hInitFont, FALSE );
	}

	// Setup the font.
	HFONT hOrigFont = ( HFONT )pDC->SelectObject( m_cfUseFont );

	// Center vertically?
	if ( m_bVertCenter == TRUE )
	{
		// Get the height of the text.
		nNumLines = TotalHeight( pDC, pszText );

		// Is it smaller than the domain?
		if ( nNumLines < pDomain->Height())
			// Yes. Adjust the top value so that it
			// get's centered.
			nYPos = ( ::GetSystemMetrics( SM_CYEDGE ) * 2 ) + (( pDomain->Height() - nNumLines ) / 2 );
	}

	// Make sure we stay in range...
	if ( nYPos < pDomain->Top()) nYPos = pDomain->Top();

	// Get the height of a single line.
	ClsSize size = pDC->GetTextExtent( _T( " " ), 1 );

	// Any room at all?
	if (( nYPos + size.CY()) < pDomain->Bottom())
	{
		// Start rendering transparent.
		pDC->SetBkMode( TRANSPARENT );

		// Default text color.
		pDC->SetTextColor( ::GetSysColor( COLOR_BTNTEXT ));

		// Find out the starting position.
		nXPos = XPos( pDC, pszText, pDomain, dwFlags );

		// Setup break flag.
		BOOL bContinue = TRUE;

		// Iterate text.
		while ( *pszText && bContinue )
		{
			// Evaluate character.
			switch ( *pszText )
			{
				case	_T( '\33' ):
					// Command sequence
					pszText = _tcsinc( pszText );
					pszText = ParseCommandSequence( pDC, pszText, dwFlags );
					break;

				case	_T( '\n' ):
					{
					// Newline.
					pszText = _tcsinc( pszText );

					// New position.
					nXPos = XPos( pDC, pszText, pDomain, dwFlags );

					// Are we passing the bottom of the
					// rendering area?
					nYPos += size.CY();
					if ( nYPos + size.CY() > pDomain->Bottom())
					{
						// Stop parsing loop.
						bContinue = FALSE;
						continue;
					}
					}
					break;

				default:
					// Reset counter.
					int i = 0, nNumC;
					LPCTSTR pszTmp = pszText;

					// Count the printable characters.
					while ( *pszTmp && ( *pszTmp != _T( '\33' )) && ( *pszTmp != _T( '\n' ))) { pszTmp = _tcsinc( pszTmp ); i++; }

					// Any printable characters?
					if ( i )
					{
						// How many can we print?
						size = pDC->GetTextExtentExPoint( pszText, i, max( pDomain->Left(), pDomain->Width() - nXPos ), &nNumC, NULL );

						if ( nNumC )
						{
							// Output text.
							pDC->TextOut( nXPos, nYPos, pszText, nNumC );

							// Adjust x position.
							nXPos += size.CX();
						}

						// Adjust pointer.
						pszText = pszTmp;
					}
					break;
			}
		}
	}

	// Restore the original font.
	pDC->SelectObject( hOrigFont );

	// Detach the used font.
	m_cfUseFont.Detach();

	// Reset the DC.
	pDC->RestoreDC( nSDC );
}

// Window procedure override.
LRESULT ClsInfoControl::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
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
	return ClsStatic::WindowProc( uMsg, wParam, lParam );
}

// Paint the control.
void ClsInfoControl::PaintControl( ClsDC *pDC )
{
	// Get the painting area.	
	ClsRect	rcRect = GetClientRect();

	// Any room to render in?
	if ( ! rcRect.IsEmpty())
	{
		// Create a buffer DC with the size of the
		// client rectangle.
		ClsBufferDC dc( *pDC, rcRect );
		
		// Fill it.
		ClsBrush brush( m_crBkColor );
		dc.FillRect( rcRect, &brush );

		// Get the window text.
		ClsString wText( m_hWnd );

		// Adjust text area.
		if ( GetStyle() & SS_SUNKEN )
			rcRect.Deflate( 2, 2 );

		// Any text?
		if ( wText.GetStringLength())
			// Render...
			RenderInfoText( &dc, wText, &rcRect );
	}
}

// WM_PAINT message handler.
LRESULT ClsInfoControl::OnPaint( ClsDC *pDC )
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
BOOL ClsInfoControl::OnGetMinSize( ClsSize& szMinSize )
{
	// Sunken edge?
	DWORD dwStyle = GetStyle();
        if ( dwStyle & SS_SUNKEN ) 
	{
		szMinSize.CX() += ::GetSystemMetrics( SM_CXEDGE ) * 4;
		szMinSize.CY() += ::GetSystemMetrics( SM_CYEDGE ) * 4;
	}

	// Any text?
	ClsString str( GetSafeHWND());
	if ( str.GetStringLength()) 
	{
		// Get a device context.
		ClsGetDC dc( this );

		// Add text dimensions.
		szMinSize.CX() += TotalWidth( &dc, str );
		szMinSize.CY() += TotalHeight( &dc, str );
	}
	return TRUE;
}
