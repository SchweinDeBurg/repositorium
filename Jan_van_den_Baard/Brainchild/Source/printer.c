/*
 *	printer.c
 *
 *	(C) Copyright 1993-2005 Jan van den Baard,
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"
#include "LangRes.rh"

/*
 *	Data.
 */
static HWND		hAbortBox = NULL;
static BOOL		bAbort = FALSE;
static TCHAR	       *pcBuffer = NULL;
static int		nLineHeight, nCharWidth, nPageHeight, nPageWidth, nNumLines, nNumColumns;
static int		nPageXSize, nPageYSize, nPixXmm, nPixYmm, nLineMargin;
static LPARRAY		lpaHeader, lpaFooter;
static RECT		rtMargin;
static BOOL		bInitialized = FALSE;

/*
 *	Abort dialog message processing.
 */
static BOOL CALLBACK AbortDlg( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg ) 
	{
		case	WM_INITDIALOG:
			return TRUE;

       case	WM_COMMAND:
         /*
			 *	"Abort" button selected?
       	 */
			if ( LOWORD( wParam ) == IDC_PRINT_ABORT ) 
			{
				/*
       				 *	Yes...
                       		 */
				return bAbort = TRUE;
               		}
               		break;
        }
	return FALSE;
}

/*
 *	The abort message processing code.
 */
static BOOL CALLBACK AbortProc( HDC hDC, int nCode )
{
	MSG	msg;

	/*
	 *	No dialog? Continue...
	 */
	if ( hAbortBox == NULL )
		return TRUE;

	/*
	 *	Process messages.
	 */
	while ( ! bAbort && PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
	{
		if ( ! IsDialogMessage( hAbortBox, &msg ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	return ! bAbort;
}

/*
 *	Compute page values.
 */
static HFONT CalcPageSize( LPCLASSDATA lpcd, HDC hDC )
{
	TEXTMETRIC	tm;
	HFONT		hSaveFont;
	TCHAR		szLN[ 25 ];
	float		xFact, yFact;

	/*
	 *	Setup the printer font.
	 */
	hSaveFont = SelectObject( hDC, Parser->hPrinterFont );

	/*
	 *	Get the text metrics.
	 */
	GetTextMetrics( hDC, &tm );

	/*
	 *	Get the line height
	 *	and character width
	 */
	nLineHeight = tm.tmHeight + tm.tmExternalLeading;
	nCharWidth  = tm.tmAveCharWidth + tm.tmOverhang;

	/*
	 *	Compute the page size in milimeters.
	 */
	nPageXSize  = GetDeviceCaps( hDC, HORZSIZE );
	nPageYSize  = GetDeviceCaps( hDC, VERTSIZE );

	/*
	 *	Compute the amount of pixels we
	 *	can get on one milimeter of paper.
	 */
	nPageWidth  = GetDeviceCaps( hDC, HORZRES );
	nPageHeight = GetDeviceCaps( hDC, VERTRES );
	xFact = ( float )( nPageWidth  ) / ( float )( nPageXSize );
	yFact = ( float )( nPageHeight ) / ( float )( nPageYSize );

	/*
	 *	Store the number of pixels per
	 *	millimeter.
	 */
	nPixXmm = ( int )xFact;
	nPixYmm = ( int )yFact;

	/*
	 *	Convert the margins into pixels. By default the
	 *	margins are in one hundredths of a milimeter.
	 */
	rtMargin.left   = ( long )((( float )pdPage.rtMargin.left   / 100.0 ) * xFact );
	rtMargin.right  = ( long )((( float )pdPage.rtMargin.right  / 100.0 ) * xFact );
	rtMargin.top    = ( long )((( float )pdPage.rtMargin.top    / 100.0 ) * yFact );
	rtMargin.bottom = ( long )((( float )pdPage.rtMargin.bottom / 100.0 ) * yFact );

	/*
	 *	Do we print line numbers?
	 */
	if ( Parser->bPrintLineNumbers )
	{
		/*
		 *	Determine the size of the linenumber 
		 *	margin. We do this simply by converting 
		 *	the highest linenumber to ASCII and 
		 *	multiplying the length of the resulting
		 *	string by the character width.
		 *
		 *	We also add a two-millimeter space.
		 */
		_itot( Brainchild_ArrayGetSize( lpcd->lpLines ), szLN, 10 );
		nLineMargin = ( _tcslen( szLN ) * nCharWidth ) + ( 2 * nPixXmm );
	}
	else
		/*
		 *	No line number margin.
		 */
		nLineMargin = 0;

	/*
	 *	Compute the page size. The size is adjusted
	 *	to not include the margins.
	 */
	nPageWidth  -= rtMargin.left + rtMargin.right;
	nPageHeight -= rtMargin.top  + rtMargin.bottom;

	/*
	 *	Compute the number of lines
	 *	and columns.
	 */
	nNumLines   = nPageHeight / nLineHeight;
	nNumColumns = nPageWidth  / nCharWidth;

	/*
	 *	Line numbers?
	 */
	if ( Parser->bPrintLineNumbers )
		/*
		 *	Adjust the number of columns on a single
		 *	page accoording to the linenumber margin.
		 */
		nNumColumns -= _tcslen( szLN );

	/*
	 *	Return old font.
	 */
	return hSaveFont;
}

/*
 *	Append string and return
 *	new index.
 */
static int AppendString( LPTSTR pszBuffer, LPTSTR pszAdd, int nIndex )
{
	/*
	 *	Append the string to the
	 *	buffer.
	 */
	_tcsncpy( &pszBuffer[ nIndex ], pszAdd, 1024 - nIndex );
	return _tcslen( pszBuffer );
}

/*
 *	Parser header/footer string.
 *
 *	Currently it recognizes the following
 *	command codes:
 *
 *	%% - Insert a % sign in the output.
 *	%f - Insert the filename without path.
 *	%F - Insert the filename with full path.
 *	%p - The page number currently being printed.
 *	%P - The number of pages to print.
 *	%d - The print date formatted as set by the configurator.
 *	%t - The print time formatted as set by the configurator.
 */
static LPTSTR ParseString( LPCLASSDATA lpcd, LPTSTR pszInput, LPTSTR pszBuffer, int nPage, int nTotalPages )
{
	int		i, a;

	/*
	 *	Go through the string...
	 */
	for ( i = 0, a = 0; a < 1024; i++ )
	{
		if ( pszInput[ i ] == _T( '%' ))
		{
			/*
			 *	Next character.
			 */
			i++;

			/*
			 *	What's the command code?
			 */
			switch ( pszInput[ i ] )
			{
				case	_T( '%' ):
					/*
					 *	Simply copy this character.
					 */
					pszBuffer[ a++ ] = pszInput[ i++ ];
					break;
	
				case	_T( 'f' ):
				{
					/*
					 *	Add the filename without
					 *	the path.
					 */
					LPTSTR	pszFilename = _tcsrchr( lpcd->szFileName, _T( '\\' ));

					/*
					 *	Was there a path?
					 */
					if ( pszFilename == NULL ) pszFilename = lpcd->szFileName;
					else			   pszFilename++;

					/*
					 *	Append it and adjust
					 *	the buffer index.
					 */
					a = AppendString( pszBuffer, pszFilename, a );

					/*
					 *	Next...
					 */
					i++;
					break;
				}

				case	_T( 'F' ):
				{
					/*
					 *	Append the path and adjust
					 *	the buffer index.
					 */
					a = AppendString( pszBuffer, lpcd->szFileName, a );

					/*
					 *	Next...
					 */
					i++;
					break;
				}

				case	_T( 'p' ):
				{
					/*
					 *	Append the page number.
					 */
					TCHAR	szNumber[ 20 ];

					/*
					 *	Convert the number string.
					 */
					_stprintf( szNumber, _T( "%ld " ), nPage );
					
					/*
					 *	Append it and adjust
					 *	the buffer index.
					 */
					a = AppendString( pszBuffer, szNumber, a );

					/*
					 *	Next...
					 */
					i++;
					break;
				}

				case	_T( 'P' ):
				{
					/*
					 *	Append the number of pages number.
					 */
					TCHAR	szNumber[ 20 ];

					/*
					 *	Convert the number string.
					 */
					_stprintf( szNumber, _T( "%ld " ), nTotalPages );
					
					/*
					 *	Append it and adjust
					 *	the buffer index.
					 */
					a = AppendString( pszBuffer, szNumber, a );

					/*
					 *	Next...
					 */
					i++;
					break;
				}

				case	_T( 'd' ):
				{
					/*
					 *	Convert the date.
					 */
					TCHAR		szBuf[ 1024 ];

					/*
					 *	Format output.
					 */
					if ( GetDateFormat( LOCALE_USER_DEFAULT, Parser->pszDateFormat ? 0 : LOCALE_NOUSEROVERRIDE, 0, Parser->pszDateFormat, szBuf, 1024 ) > 0 )
						/*
						 *	Append it and adjust
						 *	the buffer index.
						 */
						a = AppendString( pszBuffer, szBuf, a );

					/*
					 *	Next...
					 */
					i++;
					break;
				}

				case	_T( 't' ):
				{
					/*
					 *	Convert the time.
					 */
					TCHAR		szBuf[ 1024 ];

					/*
					 *	Format output.
					 */
					if ( GetTimeFormat( LOCALE_USER_DEFAULT, Parser->pszTimeFormat ? 0 : LOCALE_NOUSEROVERRIDE, 0, Parser->pszTimeFormat, szBuf, 1024 ) > 0 )
						/*
						 *	Append it and adjust
						 *	the buffer index.
						 */
						a = AppendString( pszBuffer, szBuf, a );

					/*
					 *	Next...
					 */
					i++;
					break;
				}
			}
		}
		else
		{
			/*
			 *	Simply copy the input character to the output.
			 */
			if (( pszBuffer[ a++ ] = pszInput[ i ] ) == 0 )
				break;
		}
	}

	/*
	 *	Remove trailing blanks.
	 */
	a--;
	while( _istspace(  pszBuffer[ a - 1 ] )) a--;

	/*
	 *	0-terminate.
	 */
	pszBuffer[ a ] = 0;

	return pszBuffer;
}

/*
 *	Header/footer buffers.
 */
static TCHAR szLeft[ 1024 ], szCenter[ 1024 ], szRight[ 1024 ];

/*
 *	Draw a horizontal line.
 */
static void DrawHLine( HDC hDC, int x1, int x2, int y )
{
	/*
	 *	Draw a horizontal line one millimeter
	 *	below the header text.
	 */
	HPEN hOld = SelectObject( hDC, ( HPEN )GetStockObject( BLACK_PEN ));
	MoveToEx( hDC, x1, y, NULL );
	LineTo( hDC, x2, y );
	SelectObject( hDC, hOld );
}

/*
 *	Print the header.
 */
static int SendHeader( LPCLASSDATA lpcd, HDC hDC, int nPage, int nNumPages )
{
	LPTSTR	pszHeader;
	SIZE	sSize;
	int	i = 0;

	/*
	 *	Print the header?
	 */
	if ( Parser->bPrintHeader == FALSE )
		return 0;

	/*
	 *	Use the normal text color.
	 */
	SetTextColor( hDC, Parser->crColors[ CARR_TEXT ] );

	/*
	 *	Left-aligned data?
	 */
	if ( Parser->pszHeaderLeft )
	{
		/*
		 *	Parse the string.
		 */
		pszHeader = ParseString( lpcd, Parser->pszHeaderLeft, szLeft, nPage, nNumPages );

		/*
		 *	Send the result to the
		 *	printer.
		 */
		TextOut( hDC, rtMargin.left, rtMargin.top, pszHeader, _tcslen( pszHeader ));

		/*
		 *	Skip first two lines.
		 */
		i = 2;
	}

	/*
	 *	Centered data?
	 */
	if ( Parser->pszHeaderCenter )
	{
		/*
		 *	Parse the string.
		 */
		pszHeader = ParseString( lpcd, Parser->pszHeaderCenter, szCenter, nPage, nNumPages );

		/*
		 *	Compute the string length.
		 */
		GetTextExtentPoint32( hDC, pszHeader, _tcslen( pszHeader ), &sSize );

		/*
		 *	Send the result to the
		 *	printer.
		 */
		TextOut( hDC, rtMargin.left + ( nPageWidth / 2 ) - ( sSize.cx / 2 ), rtMargin.top, pszHeader, _tcslen( pszHeader ));

		/*
		 *	Skip first two lines.
		 */
		i = 2;
	}

	/*
	 *	Right-aligned data?
	 */
	if ( Parser->pszHeaderRight )
	{
		/*
		 *	Parse the string.
		 */
		pszHeader = ParseString( lpcd, Parser->pszHeaderRight, szRight, nPage, nNumPages );

		/*
		 *	Compute the string length.
		 */
		GetTextExtentPoint32( hDC, pszHeader, _tcslen( pszHeader ), &sSize );

		/*
		 *	Send the result to the
		 *	printer.
		 */
		TextOut( hDC, rtMargin.left + ( nPageWidth - sSize.cx ), rtMargin.top, pszHeader, _tcslen( pszHeader ));

		/*
		 *	Skip first two lines.
		 */
		i = 2;
	}

	/*
	 *	Draw the separator line.
	 */
	DrawHLine( hDC, rtMargin.left, rtMargin.left + nPageWidth, rtMargin.top + nLineHeight + nPixYmm );

	return i;
}

/*
 *	Print the footer.
 */
static void SendFooter( LPCLASSDATA lpcd, HDC hDC, int nPage, int nNumPages )
{
	LPTSTR	pszFooter;
	SIZE	sSize;

	/*
	 *	Print the footer?
	 */
	if ( Parser->bPrintFooter == FALSE )
		return;

	/*
	 *	Use the normal text color.
	 */
	SetTextColor( hDC, Parser->crColors[ CARR_TEXT ] );

	/*
	 *	Left-aligned data?
	 */
	if ( Parser->pszFooterLeft )
	{
		/*
		 *	Parse the string.
		 */
		pszFooter = ParseString( lpcd, Parser->pszFooterLeft, szLeft, nPage, nNumPages );

		/*
		 *	Send the result to the
		 *	printer.
		 */
		TextOut( hDC, rtMargin.left, rtMargin.top + ( nPageHeight - nLineHeight ), pszFooter, _tcslen( pszFooter ));
	}

	/*
	 *	Centered data?
	 */
	if ( Parser->pszFooterCenter )
	{
		/*
		 *	Parse the string.
		 */
		pszFooter = ParseString( lpcd, Parser->pszFooterCenter, szCenter, nPage, nNumPages );

		/*
		 *	Compute the string length.
		 */
		GetTextExtentPoint32( hDC, pszFooter, _tcslen( pszFooter ), &sSize );

		/*
		 *	Send the result to the
		 *	printer.
		 */
		TextOut( hDC, rtMargin.left + ( nPageWidth / 2 ) - ( sSize.cx / 2 ), rtMargin.top + ( nPageHeight - nLineHeight ), pszFooter, _tcslen( pszFooter ));
	}

	/*
	 *	Right-aligned data?
	 */
	if ( Parser->pszFooterRight )
	{
		/*
		 *	Parse the string.
		 */
		pszFooter = ParseString( lpcd, Parser->pszFooterRight, szRight, nPage, nNumPages );

		/*
		 *	Compute the string length.
		 */
		GetTextExtentPoint32( hDC, pszFooter, _tcslen( pszFooter ), &sSize );

		/*
		 *	Send the result to the
		 *	printer.
		 */
		TextOut( hDC, rtMargin.left + ( nPageWidth - sSize.cx ), rtMargin.top + ( nPageHeight - nLineHeight ), pszFooter, _tcslen( pszFooter ));
	}

	/*
	 *	Draw the separator line.
	 */
	DrawHLine( hDC, rtMargin.left, rtMargin.left + nPageWidth, rtMargin.top + ( nPageHeight - ( nLineHeight + nPixYmm )));
}

/*
 *	Count the number of lines required
 *	to print this text.
 */
static int Count( int nStart, int nEnd )
{
	int		nNumFit, nLength = nEnd - nStart, nCount = 0;

	/*
	 *	Count the number of lines necessary
	 *	to print the text.
	 */
	do
	{
		/*
		 *	How many characters will fit?
		 */
		if ( nLength > nNumColumns ) nNumFit = nNumColumns;
		else			     nNumFit = nLength;

		/*
		 *	Adjust counters.
		 */
		nCount++;
		nLength -= nNumFit;
	} while ( nLength > 0 );

	/*
	 *	Return the line count.
	 */
	return nCount;
}

/*
 *	Send a line to the printer.
 */
static int Send( LPCLASSDATA lpcd, LPLINE lpLine, HDC hDC, int nStart, int nEnd, int nCount, TCHAR *pcText, int nTotalLength, int *lpnBlock, int nLine )
{
	SYNTAX_COLOR   *scArray, scDefault = { CARR_TEXT, CLR_DEFAULT, nTotalLength, NULL };
	BOOL		bBreak = FALSE;
	int		nWillFit, nLength = nEnd - nStart, nBlocks = 1, i = 0, nXOff, nNumDone;

	/*
	 *	Syntax coloring printing?
	 */
	if ( Parser->bPrintSyntaxColoring )
	{
		/*
		 *	Allocate color blocks.
		 */
		if (( scArray = GetColorBlocks( lpcd, nTotalLength + 2 )) != NULL )
			/*
			 *	Parse the line.
			 */
			*lpnBlock = ParseLine( lpcd, *lpnBlock, pcText, nTotalLength, scArray, &nBlocks, nTotalLength + 2, nLine );
		else
			/*
			 *	Use the default text
			 *	color text block.
			 */
			scArray = &scDefault;
	}
	else
		/*
		 *	Use the default text
		 *	color text block.
		 */
		scArray = &scDefault;

	/*
	 *	Make sure all text is printed.
	 */
	do 
	{
		/*
		 *	Go through the coloring
		 *	blocks.
		 */
		for ( nNumDone = 0, nXOff = 0; i < nBlocks; i++ )
		{
			/*
			 *	How many characters in this block?
			 */
			int	nChars = scArray[ i ].nColumn - nStart;

			/*
			 *	Any visible ones?
			 */
			if ( nChars > 0 )
			{
				/*
				 *	Setup the colors.
				 */
				SetTextColor( hDC, scArray[ i ].crColor );

				/*
				 *	Background color?
				 */
				if ( scArray[ i ].crBgColor != CLR_DEFAULT )
				{
					/*
					 *	Setup the correct background color.
					 */
					SetBkMode( hDC, OPAQUE );
					SetBkColor( hDC, scArray[ i ].crBgColor );
				}
				else
					/*
					 *	We render transparent. Note that printing
					 *	syntax coloring will NOT render the set
					 *	standard text background color.
					 */
					SetBkMode( hDC, TRANSPARENT );

				/*
				 *	Compute the number of characters
				 *	that will fit.
				 */
				if (( nChars + nXOff ) > nNumColumns )
				{
					/*
					 *	The text we need to render does not
					 *	fit so we print as much as possible 
					 *	and start a new line.
					 */
					nWillFit = nNumColumns - nXOff;
					bBreak = TRUE;
				}
				else
					/*
					 *	All of 'm will fit.
					 */
					nWillFit = nChars;
				
				/*
				 *	Do not print past what we
				 *	are supposed to print.
				 */
				if (( nWillFit + nStart ) > nEnd )
				{
					/*
					 *	No more than necessary...
					 */
					nWillFit -= ( nWillFit + nStart ) - nEnd;
					bBreak = TRUE;
				}

				/*
				 *	Output the text to the printer DC.
				 */
				if ( nWillFit > 0 )
				{
					/*
					 *	Send text to the printer.
					 */
					TextOut( hDC, rtMargin.left  + nLineMargin + ( nXOff * nCharWidth ), rtMargin.top + ( nCount * nLineHeight ), pcText + nStart, nWillFit );

					/*
					 *	Adjust counter.
					 */
					nNumDone += nWillFit;

					/*
					 *	Increase offsets.
					 */
					nStart += nWillFit;
					nXOff += nWillFit;
				}

				/*
				 *	New line?
				 */
				if ( bBreak )
					break;
			}
		}

		/*
		 *	Adjust length counter.
		 */
		nLength -= nNumDone;

		/*
		 *	Any characters left? If so this means that the line did not
		 *	fit and we need to print a marker in the right margin.
		 */
		if ( nLength > 0 )
		{
			/*
			 *	Setup a black pen.
			 */
			HPEN hPen = SelectObject( hDC, ( HPEN )GetStockObject( BLACK_PEN ));

			/*
			 *	Setup the top-left coordinates where the
			 *	marker should be printed.
			 */
			int nX = rtMargin.left + nPageWidth + ( 2 * nPixXmm );
			int nY = rtMargin.top + ( nCount * nLineHeight );

			/*
			 *	Render the marker.
			 */
			MoveToEx( hDC, nX, nY + ( nLineHeight / 2 ), NULL );
			LineTo( hDC, nX + ( nLineHeight / 2 ), nY + nLineHeight );
			LineTo( hDC, nX, nY + nLineHeight );
			LineTo( hDC, nX, nY + ( nLineHeight / 2 ));

			/*
			 *	Restore the pen.
			 */
			SelectObject( hDC, hPen );
		}

		/*
		 *	One more line.
		 */
		nCount++;
		bBreak = FALSE;

	} while ( nLength > 0 );
	
	/*
	 *	Returns the lines already printed on
	 *	the current page.
	 */
	return( nCount );
}

/*
 *	Convert tabs to spaces.
 */
static void ConvertForPrint( LPCLASSDATA lpcd, TCHAR *pcInput, int nLength )
{
	int	 i, nPos = 0;

	/*
	 *	Get all chars.
	 */
	for ( i = 0; i < nLength; i++ )
	{
		/*
		 *	A tab character?
		 */
		if ( pcInput[ i ] == _T( '\t' ))
		{
			int	nSpace = ( Parser->nTabSize - ( nPos % Parser->nTabSize ));

			/*
			 *	Insert spaces until we reach
			 *	the next tab stop.
			 */
			while ( nSpace )
			{
				pcBuffer[ nPos++ ] = _T( ' ' );
				nSpace--;
			}
		}
		else
			/*
			 *	Copy the character
			 *	into the conversion buffer.
			 */
			pcBuffer[ nPos++ ] = pcInput[ i ];
	}
	/*
	 *	0-terminate the buffer.
	 */
	pcBuffer[ nPos ] = _T( '\0' );
}

/*
 *	Compute how many pages are
 *	required to print the text.
 */
static int CountPages( LPCLASSDATA lpcd, HDC hDC, int nFirst, int nLast, BOOL bSelection )
{
	LPLINE		lpLine;
	TCHAR	       *pcText;
	HFONT		hSaveFont;
	int		nLineCount, nIndex, nSkip = 0, nPages = 1;

	/*
	 *	Compute page size and setup
	 *	the printer font.
	 */
	hSaveFont = CalcPageSize( lpcd, hDC );

	/*
	 *	Header/footer?
	 */
	if ( Parser->bPrintHeader && ( Parser->pszHeaderLeft || Parser->pszHeaderCenter || Parser->pszHeaderRight )) nSkip += 2;
	if ( Parser->bPrintFooter && ( Parser->pszFooterLeft || Parser->pszFooterCenter || Parser->pszFooterRight )) nSkip += 2;

	/*
	 *	Add header/footer...
	 */
	nLineCount = nSkip;

	/*
	 *	Get all lines.
	 */
	for ( nIndex = nFirst; nIndex <= nLast; nIndex++ )
	{
		int	nStart = 0, nEnd = 0, nLength, nCount;
		BOOL	bNewPage = FALSE;

		/*
		 *	Get the line.
		 */
		lpLine  = Brainchild_ArrayGetAt( lpcd->lpLines, nIndex );
		pcText  = lpLine->pcText;
		nLength = CaretOffsetLine( lpcd, nIndex, lpLine->nLength );

		/*
		 *	Selection?
		 */
		if ( bSelection )
		{
			/*
			 *	Convert start and end column positions
			 *	to visual values.
			 */
			nStart = CaretOffsetLine( lpcd, lpcd->ptSelStart.y, lpcd->ptSelStart.x );
			nEnd   = CaretOffsetLine( lpcd, lpcd->ptSelEnd.y, lpcd->ptSelEnd.x );
		}

		/*
		 *	Convert the tabs to spaces.
		 */
		ConvertForPrint( lpcd, lpLine->pcText, lpLine->nLength );

		/*
		 *	Selection?
		 */
		if ( bSelection )
		{
			/*
			 *	Count the number of lines on paper
			 *	required to print this line.
			 */
			if ( nFirst == nLast )
			{
				/*
				 *	Will the line fit?
				 */
				nCount = Count( nStart, nEnd );
				if ( nLineCount + nCount <= nNumLines )
					/*
					 *	Yes. Count it.
					 */
					nLineCount += nCount;
				else 
				{
					/*
					 *	Force a new page.
					 */
					nIndex--; 
					bNewPage = TRUE; 
				}
			}
			else if ( nIndex == nFirst ) 
			{
				/*
				 *	Will the line fit?
				 */
				nCount = Count( nStart, nLength );
				if ( nLineCount + nCount <= nNumLines )
					/*
					 *	Yes. Count it.
					 */
					nLineCount += nCount;
				else
				{
					/*
					 *	Force a new page.
					 */
					nIndex--; 
					bNewPage = TRUE; 
				}
			}
			else if ( nIndex == nLast )  
			{
				/*
				 *	Will the line fit?
				 */
				nCount = Count( 0, nEnd );
				if ( nLineCount + nCount <= nNumLines )
					/*
					 *	Yes. Count it.
					 */
					nLineCount += nCount;
				else 
				{
					/*
					 *	Force a new page.
					 */
					nIndex--; 
					bNewPage = TRUE; 
				}
			}
			else	
			{
				/*
				 *	Will the line fit?
				 */
				nCount = Count( 0, nLength );
				if ( nLineCount + nCount <= nNumLines )
					/*
					 *	Yes. Count it.
					 */
					nLineCount += nCount;
				else 
				{
					/*
					 *	Force a new page.
					 */
					nIndex--; 
					bNewPage = TRUE; 
				}
			}
		}
		else
		{
			/*
			 *	Will the line fit?
			 */
			nCount = Count( 0, nLength );
			if ( nLineCount + nCount <= nNumLines )
				/*
				 *	Yes. Count it.
				 */
				nLineCount += nCount;
			else 
			{
				/*
				 *	Force a new page.
				 */
				nIndex--; 
				bNewPage = TRUE; 
			}
		}

		/*
		 *	Room enough?
		 */
		if ((( nLineCount ) >= nNumLines ) || bNewPage )
		{
			/*
			 *	Clear new page flag.
			 */
			bNewPage = FALSE;

			/*
			 *	Reset counter.
			 */
			nLineCount = nSkip;

			/*
			 *	Increase page counter.
			 */
			if ( nIndex < Brainchild_ArrayGetSize( lpcd->lpLines ) - 1 )
				nPages++;
		}
	}

	/*
	 *	Reset old font.
	 */
	SelectObject( hDC, hSaveFont );

	/*
	 *	Return the page counter.
	 */
	return nPages;
}

/*
 *	Print the line number margin for a
 *	whole page.
 */
static void RenderLineNumberMargin( LPCLASSDATA lpcd, HDC hDC, int nTop, int nBottom )
{
	RECT		rcMargin;

	/*
	 *	Compute the margin rectangle.
	 */
	rcMargin.left	= rtMargin.left;
	rcMargin.right  = rcMargin.left + nLineMargin - nPixXmm;
	rcMargin.top    = rtMargin.top + nTop * nLineHeight;
	rcMargin.bottom = rcMargin.top + (( nNumLines - nTop - nBottom ) * nLineHeight );

	/*
	 *	Are we rendering syntax colors?
	 */
	if ( Parser->bPrintSyntaxColoring )
	{
		/*
		 *	Render the background color.
		 */
		if ( Parser->crColors[ CARR_BACKGROUND_LINE_NUMBERS ] != CLR_DEFAULT )
			FillRect( hDC, &rcMargin, lpcd->hbLineMarginBkgnd );

		/*
		 *	Render the devider line.
		 */
		rcMargin.left = rcMargin.right - 1;
		FillRect( hDC, &rcMargin, lpcd->hbLineMargin );
	}
	else
	{
		/*
		 *	Render the devider line.
		 */
		rcMargin.left = rcMargin.right - 1;
		FillRect( hDC, &rcMargin, ( HBRUSH )GetStockObject( BLACK_BRUSH ));
	}
}

/*
 *	Output the text to the printer.
 */
static void OutputText( LPCLASSDATA lpcd, HDC hDC, int nFirst, int nLast, int nCopies, BOOL bSelection )
{
	LPLINE		lpLine = NULL;
	TCHAR	       *pcText = NULL;
	HFONT		hSaveFont = NULL;
	BOOL		bNewPage = FALSE;
	int		nLines = 0, nIndex, nSkip = 0, nPages = CountPages( lpcd, hDC, nFirst, nLast, bSelection );
	int		nPage = 1, nBlock = -1;

	/*
	 *	All copies.
	 */
	while ( nCopies-- > 0 )
	{
		/*
		 *	Open up the page.
		 */
		StartPage( hDC );

		/*
		 *	Compute page size.
		 */
		hSaveFont = CalcPageSize( lpcd, hDC );

		/*
		 *	Footer?
		 */
		if ( Parser->bPrintFooter && ( Parser->pszFooterLeft || Parser->pszFooterCenter || Parser->pszFooterRight )) nSkip += 2;

		/*
		 *	Send the header string...
		 */
		nLines = SendHeader( lpcd, hDC, nPage, nPages );

		/*
		 *	Get the block number of the first
		 *	line if we are printing a selection.
		 */
		if ( bSelection )
			nBlock = (( LPLINE )Brainchild_ArrayGetAt( lpcd->lpLines, nFirst ))->nBlock;


		/*
		 *	Print line numbers?
		 */
		if ( Parser->bPrintLineNumbers )
			RenderLineNumberMargin( lpcd, hDC, nLines, nSkip );

		/*
		 *	Get all lines.
		 */
		for ( nIndex = nFirst; nIndex <= nLast; nIndex++ )
		{
			int	nStart = 0, nEnd = 0, nLength;

			/*
			 *	Get the line.
			 */
			lpLine  = Brainchild_ArrayGetAt( lpcd->lpLines, nIndex );
			pcText  = lpLine->pcText;
			nLength = CaretOffsetLine( lpcd, nIndex, lpLine->nLength );

			/*
			 *	Print line numbers?
			 */
			if ( Parser->bPrintLineNumbers )
			{
				TCHAR	 szLN[ 25 ] = { 0 };
				COLORREF crColor = 0;
				int	 nOldMode = 0;

				/*
				 *	Are we printing in color?
				 */
				if ( Parser->bPrintSyntaxColoring ) 
				{
					/*
					 *	Setup the text color and mode. 
					 */
					crColor = SetTextColor( hDC, Parser->crColors[ CARR_LINE_NUMBERS ] );
					nOldMode = SetBkMode( hDC, TRANSPARENT );
				}

				/*
				 *	Convert the line number and print it.
				 */
				_itot( nIndex + 1, szLN, 10 );
				TextOut( hDC, 
					 rtMargin.left + ( nLineMargin - ( 2 * nPixXmm ) - ( _tcslen( szLN ) * nCharWidth )), 
					 rtMargin.top  + ( nLines * nLineHeight ), 
					 szLN, 
					 _tcslen( szLN ));

				/*
				 *	Restore the color and mode if necessary.
				 */
				if ( Parser->bPrintSyntaxColoring )
				{
					SetTextColor( hDC, crColor );
					SetBkMode( hDC, nOldMode );
				}
			}

			/*
			 *	Selection?
			 */
			if ( bSelection )
			{
				/*
				 *	Convert start and end column positions
				 *	to visual values.
				 */
				nStart = CaretOffsetLine( lpcd, lpcd->ptSelStart.y, lpcd->ptSelStart.x );
				nEnd   = CaretOffsetLine( lpcd, lpcd->ptSelEnd.y, lpcd->ptSelEnd.x );
			}

			/*
			 *	Convert the line for printing.
			 */
			ConvertForPrint( lpcd, lpLine->pcText, lpLine->nLength );

			/*
			 *	Selection?
			 */
			if ( bSelection )
			{
				if ( nFirst == nLast )
				{
					/*
					 *	Will it fit?
					 */
					if (( nLines + Count( nStart, nEnd )) <= ( nNumLines - nSkip ))
						/*
						 *	Print the text between
						 *	the first and last column.
						 */
						nLines = Send( lpcd, lpLine, hDC, nStart, nEnd, nLines, pcBuffer, nLength, &nBlock, nIndex );
					else
						/*
						 *	New page please...
						 */
						bNewPage = TRUE;
				}
				else if ( nIndex == nFirst )
				{
					/*
					 *	Will it fit?
					 */
					if (( nLines + Count( nStart, nLength )) <= ( nNumLines - nSkip ))
						/*
						 *	Print the line from the
						 *	start column on.
						 */
						nLines = Send( lpcd, lpLine, hDC, nStart, nLength, nLines, pcBuffer, nLength, &nBlock, nIndex );
					else
						/*
						 *	New page please...
						 */
						bNewPage = TRUE;
				}
				else if ( nIndex == nLast )
				{
					/*
					 *	Will it fit?
					 */
					if (( nLines + Count( 0, nEnd )) <= ( nNumLines - nSkip ))
						/*
						 *	Print the line until
						 *	the last column.
						 */
						nLines = Send( lpcd, lpLine, hDC, 0, nEnd, nLines, pcBuffer, nLength, &nBlock, nIndex );
					else
						/*
						 *	New page please...
						 */
						bNewPage = TRUE;
				}
				else
				{
					/*
					 *	Will it fit?
					 */
					if (( nLines + Count( 0, nLength )) <= ( nNumLines - nSkip ))
						/*
						 *	Print the whole line.
						 */
						nLines = Send( lpcd, lpLine, hDC, 0, nLength, nLines, pcBuffer, nLength, &nBlock, nIndex );
					else
						/*
						 *	New page please...
						 */
						bNewPage = TRUE;
				}
			}
			else
			{
				/*
				 *	Will it fit?
				 */
				if (( nLines + Count( 0, nLength )) <= ( nNumLines - nSkip ))
					/*
					 *	Print the whole line.
					 */
					nLines = Send( lpcd, lpLine, hDC, 0, nLength, nLines, pcBuffer, nLength, &nBlock, nIndex );
				else
					/*
					 *	New page please...
					 */
					bNewPage = TRUE;
			}

			/*
			 *	Are we done?
			 */
			if ( nIndex >= Brainchild_ArrayGetSize( lpcd->lpLines ) - 1 )
				break;

			/*
			 *	Page full?
			 */
			if (( nLines >= ( nNumLines - nSkip )) || bNewPage )
			{
				/*
				 *	Send the footer.
				 */
				SendFooter( lpcd, hDC, nPage++, nPages );

				/*
				 *	Yep. End this page
				 */
				EndPage( hDC );

				/*
				 *	Reset counter.
				 */
				nLines = 0;

				/*
				 *	Reset old font.
				 */
				SelectObject( hDC, hSaveFont );
				
				/*
				 *	Start a new page.
				 */
				StartPage( hDC );
				
				/*
				 *	Compute size.
				 */
				CalcPageSize( lpcd, hDC );

				/*
				 *	Send the header string...
				 */
				nLines = SendHeader( lpcd, hDC, nPage, nPages );
				
				/*
				 *	Compute page size.
				 */
				hSaveFont = CalcPageSize( lpcd, hDC );

				/*
				 *	Reset new page flag.
				 */
				bNewPage = FALSE;
				
				/*
				 *	Print line numbers?
				 */
				if ( Parser->bPrintLineNumbers )
					RenderLineNumberMargin( lpcd, hDC, nLines, nSkip );
			}
		}
		/*
		 *	Send the last footer.
		 */
		SendFooter( lpcd, hDC, nPage, nPages );

		/*
		 *	End the last page.
		 */
		EndPage( hDC );
	}
	/*
	 *	Restore font.
	 */
	SelectObject( hDC, hSaveFont );
}

/*
 *	Convert screen font to a suitable
 *	sized printer font.
 */
static HFONT MakeFont( LPCLASSDATA lpcd, HDC hPrinterDC )
{
	LOGFONT	lf = { 0 };
	HFONT	hNewFont = NULL;
	HDC		hDC;

	/*
	 *	Get the display DC.
	 */
	if (( hDC = GetDC( NULL )) != NULL ) 
	{
		/*
		 *	Get font information.
		 */
		if ( GetObject( Parser->hScreenFont, sizeof( LOGFONT ), ( LPVOID )&lf ) == sizeof( LOGFONT ))
		{
			/*
			 *	Setup font size.
			 */
			lf.lfHeight = MulDiv( lf.lfHeight, GetDeviceCaps( hPrinterDC, LOGPIXELSY ), GetDeviceCaps( hDC, LOGPIXELSY ));
			lf.lfWidth  = MulDiv( lf.lfWidth,  GetDeviceCaps( hPrinterDC, LOGPIXELSX ), GetDeviceCaps( hDC, LOGPIXELSX ));

			/*
			 *	Create font.
			 */
			hNewFont = CreateFontIndirect( &lf );
		}
		/*
		 *	Release the window DC.
		 */
		ReleaseDC( NULL, hDC );
	}
	return( hNewFont );
}

static BOOL ConvertFont( LPCLASSDATA lpcd, HDC hPrinterDC )
{
	/*
	 *	Setup the new font.
	 */
	if (( Parser->hPrinterFont = MakeFont( lpcd, hPrinterDC )) != NULL )
		return TRUE;
	return FALSE;
}

/*
 *	Print...
 */
static void PrintText( LPCLASSDATA lpcd, HDC hDC, BOOL bSelection, int nCopies )
{
	HFONT		hOrigFont = NULL;
	DOCINFO		di = { 0 };
	int		nFirst, nLast;
	BOOL		bFontChanged = FALSE;

	/*
	 *	Use the screen font for printing?
	 */
	if ( Parser->bPrintScreenFont )
	{
		/*
		 *	Save of the old font.
		 */
		hOrigFont = Parser->hPrinterFont;

		/*
		 *	Setup new font.
		 */
		bFontChanged = ConvertFont( lpcd, hDC );
	}

	/*
	 *	Not aborted by default.
	 */
	bAbort = FALSE;

	/*
	 *	Open the abort dialog.
	 */
	if (( hAbortBox = CreateDialog( hDLL, MAKEINTRESOURCE( IDD_PRINTER ), lpcd->hWnd, AbortDlg )) != NULL )
		ShowWindow( hAbortBox, SW_NORMAL );
	else
		return;

	/*
	 *	Setup the abort procedure.
	 */
	if ( SetAbortProc( hDC, ( ABORTPROC )AbortProc ) == SP_ERROR )
	{
		DestroyWindow( hAbortBox );
		return;
	}

	/*
	 *	Setup the document.
	 */
	di.cbSize = sizeof( DOCINFO );
	di.lpszDocName = _T("Brainchild Text");

	/*
	 *	Start the document.
	 */
	if ( StartDoc( hDC, &di ) <= 0 )
	{
		DestroyWindow( hAbortBox );
		return;
	}

	/*
	 *	Allocate a conversion buffer.
	 */
	if (( pcBuffer = Brainchild_AllocPooled( lpcd->pMemPool, REAL_SIZE( lpcd->nLongestLine + 1 ))) == NULL )
	{
		DestroyWindow( hAbortBox );
		return;
	}

	/*
	 *	Get start and end lines.
	 */
	if ( bSelection )
	{
		nFirst = lpcd->ptSelStart.y;
		nLast  = lpcd->ptSelEnd.y;
	}
	else
	{
		nFirst = 0;
		nLast  = Brainchild_ArrayGetSize( lpcd->lpLines ) - 1;
	}

	/*
	 *	Output the text.
	 */
	OutputText( lpcd, hDC, nFirst, nLast, nCopies, bSelection );

	/*
	 *	End document.
	 */
	EndDoc( hDC );

	/*
	 *	Free the conversion buffer.
	 */
	Brainchild_FreePooled( lpcd->pMemPool, pcBuffer );

	/*
	 *	Destroy the abort dialog.
	 */
	DestroyWindow( hAbortBox );

	/*
	 *	Reset font...
	 */
	if ( bFontChanged )
	{
		/*
		 *	Destroy changed font...
		 */
		DeleteObject( Parser->hPrinterFont );

		/*
		 *	Setup original.
		 */
		Parser->hPrinterFont = hOrigFont;
	}
}

/*
 *	Display the Page setup data or simply
 *	get the default settings.
 */
extern LRESULT OnPageSetupDialog( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	PageSetupBody( lpcd, TRUE );
	return 0;
}

void PageSetup( LPARAM lParam, LPCLASSDATA lpcd )
{
	PageSetupBody( lpcd, TRUE );
}

void PageSetupDefault( LPCLASSDATA lpcd )
{
	PageSetupBody( lpcd, FALSE );
}

void PageSetupBody( LPCLASSDATA lpcd, BOOL bShowDialog )
{
	/*
	 *	Clear qualifiers.
	 */
	lpcd->cQualifier = 0;

	/*
	 *	Setup the print dialog
	 *	structure.
	 */
	pdPage.lStructSize = sizeof( pdPage );
	pdPage.hwndOwner   = lpcd->hWnd;
	pdPage.Flags       = PSD_INHUNDREDTHSOFMILLIMETERS | PSD_MARGINS | ( bShowDialog ? 0 : PSD_RETURNDEFAULT );
	pdPage.hInstance   = NULL;

	/*
	 *	If this is the first time a page setup
	 *	is requested we setup a default margin
	 *	of 10 milimeters.
	 */
	if ( bInitialized == FALSE )
	{
		pdPage.rtMargin.left = pdPage.rtMargin.top = pdPage.rtMargin.right = pdPage.rtMargin.bottom = 1000;
		bInitialized = TRUE;
	}

	/*
	 *	Open the dialog or get
	 *	the defaults.
	 */
	PageSetupDlg( &pdPage );
}

/*
 *	Print the data...
 */
void Print( LPARAM lParam, LPCLASSDATA lpcd )
{
	PRINTDLG	pd = { 0 };
	DWORD		dwFlags = PD_RETURNDC | PD_DISABLEPRINTTOFILE;

	/*
	 *	Clear qualifiers.
	 */
	lpcd->cQualifier = 0;

	/*
	 *	Do we have a selection?
	 */
	if ( HasMark( lpcd )) dwFlags &= ~PD_NOSELECTION;
	else		      dwFlags |= PD_NOSELECTION;

	/*
	 *	Setup the print dialog
	 *	structure.
	 */
	pd.lStructSize = sizeof( pd );
	pd.hwndOwner   = lpcd->hWnd;
	pd.Flags       = dwFlags;
	pd.hInstance   = NULL;
	pd.nCopies     = 1;

	/*
	 *	Do we have a page setup?
	 */
	if ( bInitialized == FALSE )
		/*
		 *	Get the default page settings.
		 */
		PageSetupDefault( lpcd );

	/*
	 *	Store these values.
	 */
	pd.hDevMode  = pdPage.hDevMode;
	pd.hDevNames = pdPage.hDevNames;

	/*
	 *	Open the dialog.
	 */
	if ( PrintDlg( &pd ))
	{
		/*
		 *	Print...
		 */
		PrintText( lpcd, pd.hDC, ( BOOL )( pd.Flags & PD_SELECTION ), pd.nCopies );

		/*
		 *	Destroy the device
		 *	context.
		 */
		DeleteDC( pd.hDC );
	}
}
