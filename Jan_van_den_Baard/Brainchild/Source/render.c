/*
 *	render.c
 *
 *	(C) Copyright 1993-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Graphical stuff.
 */

#include "stdafx.h"

#include "defs.h"

extern LRESULT OnSyntaxColoring( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	All languages?
	 */
	if ( lParam )
	{
		/*
		 *	Did the value change?
		 */
		if (( BOOL )wParam != lpcd->bSyntaxColoring )
		{
			/*
			 *	Change value.
			 */
			lpcd->bSyntaxColoring = ( BOOL )wParam;

			/*
			 *	Re-render.
			 */
			InvalidateRect( hWnd, NULL, FALSE );
		}
	}
	else
	{
		if (( BOOL )wParam != lpcd->lpCurrParser->bSyntaxColoring )
		{
			/*
			 *	Set value.
			 */
			lpcd->lpCurrParser->bSyntaxColoring = ( BOOL )wParam;

			/*
			 *	Re-render.
			 */
			InvalidateRect( hWnd, NULL, FALSE );
		}
	}
	return 0;
}

void VScroll( LPCLASSDATA lpcd, int nLines )
{
	RECT		rcView;

	/*
	 *	More than a view?
	 */
	if ( abs( nLines ) < lpcd->szViewSize.cy )
	{
		/*
		 *	Get client rectangle.
		 */
		GetClientRect( lpcd->hWnd, &rcView );

		/*
		 *	Adjust for a view size.
		 */
		/*rcView.right  = rcView.left + ( lpcd->szViewSize.cx  * Parser->szCharSize.cx ) + GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd );*/
		rcView.bottom = rcView.top  + ( lpcd->szViewSize.cy  * Parser->szCharSize.cy );

		/*
		 *	Scroll the contents.
		 */
		ScrollWindow( lpcd->hWnd, 0 , nLines * Parser->szCharSize.cy , NULL, &rcView );
	}
	else
		/*
		 *	Re-render the whole view.
		 */
		InvalidateRect( lpcd->hWnd, NULL, FALSE );
}

void HScroll( LPCLASSDATA lpcd, int nCols )
{
	RECT		rcView;

	/*
	 *	More than a view?
	 */
	if ( abs( nCols ) < lpcd->szViewSize.cx )
	{
		/*
		 *	Get client rectangle.
		 */
		GetClientRect( lpcd->hWnd, &rcView );

		/*
		 *	Adjust for a view size.
		 */
		rcView.left  += ( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ));
		rcView.right  = rcView.left + ( lpcd->szViewSize.cx  * Parser->szCharSize.cx );
		rcView.bottom = rcView.top  + ( lpcd->szViewSize.cy  * Parser->szCharSize.cy );

		/*
		 *	Scroll the contents.
		 */
		ScrollWindow( lpcd->hWnd, nCols * Parser->szCharSize.cx, 0, NULL, &rcView );
	}
	else
		/*
		 *	Re-render the whole view.
		 */
		InvalidateRect( lpcd->hWnd, NULL, FALSE );
}

void GetCharSize( HDC hDC, LPCLASSDATA lpcd )
{
	TEXTMETRIC	tm;
	HFONT		hOldFont;
	BOOL		bGetDC = FALSE;

	/*
	 *	Character size unknown?
	 */
	if ( Parser->szCharSize.cx == -1 )
	{
		/*
		 *	Input DC valid?
		 */
		if ( hDC == NULL )
		{
			/*
			 *	No, get it.
			 */
			if (( hDC = GetDC( lpcd->hWnd )) == NULL )
				return;

			/*
			 *	We had to get it.
			 */
			bGetDC = TRUE;
		}


		/*
		 *	Setup the font.
		 */
		hOldFont = SelectObject( hDC, Parser->hScreenFont );

		/*
		 *	Get the text metrics.
		 */
		GetTextMetrics( hDC, &tm );

		/*
		 *	Store the values.
		 */
		Parser->szCharSize.cx = tm.tmAveCharWidth + tm.tmOverhang;
		Parser->szCharSize.cy = tm.tmHeight + tm.tmExternalLeading;

		/*
		 *	Put back the old font.
		 */
		SelectObject( hDC, hOldFont );

		/*
		 *	Did we get the DC?
		 */
		if ( bGetDC )
			ReleaseDC( lpcd->hWnd, hDC );
	}
}

/*
 *	Compute the line number margin size.
 */
int GetLineMarginWidth( LPCLASSDATA lpcd )
{
	int	nMargin = 0;

	/*
	 *	Line-numbers switch on?
	 */
	if ( Parser->bLineNumbers )
	{
		/*
		 *	Yes. Compute character size.
		 */
		GetCharSize( NULL, lpcd );

		/*
		 *	The line number margin is 5 times the character
		 *	width.
		 */
		nMargin = ( Parser->szCharSize.cx * 5 ) + 6;
	}

	return nMargin;
}

/*
 *	Compute the selection margin size.
 */
int GetMarginWidth( LPCLASSDATA lpcd )
{
	int	nMargin = 0;

	/*
	 *	Margin switch on?
	 */
	if ( Parser->bSelectionMargin )
	{
		/*
		 *	Yes. Compute character size.
		 */
		GetCharSize( NULL, lpcd );

		/*
		 *	The margin is twice the character
		 *	width.
		 */
		nMargin = Parser->szCharSize.cx * 2;
	}

	return nMargin;
}

/*
 *	Is it a fixed width font?
 */
BOOL IsFixedWidth( HWND hWnd, HFONT hFont )
{
	HDC	hDC;
	HFONT	hOFont;
	BOOL	bRC = FALSE;

	/*
	 *	Get DC.
	 */
	if (( hDC = GetDC( hWnd )) != NULL )
	{
		TEXTMETRIC	tm;

		/*
		 *	Set font.
		 */
		hOFont = SelectObject( hDC, hFont );

		/*
		 *	Get text metrics.
		 */
		if ( GetTextMetrics( hDC, &tm ))
			/*
			 *	Fixed width?
			 */
			 bRC = ( BOOL )( ! ( tm.tmPitchAndFamily & TMPF_FIXED_PITCH ));

		/*
		 *	Reset font.
		 */
		SelectObject( hDC, hOFont );

		/*
		 *	Release DC.
		 */
		ReleaseDC( hWnd, hDC );
	}
	return bRC;
}

LRESULT OnSetFont( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Is it a fixed width font?
	 */
	if ( IsFixedWidth( hWnd, ( HFONT )wParam ))
	{
		/*
		 *	Destroy the previous font.
		 */
		DeleteObject( Parser->hScreenFont );

		/*
		 *	Save the font handle.
		 */
		Parser->hScreenFont = ( HFONT )wParam;

		/*
		 *	Re-compute character size.
		 */
		Parser->szCharSize.cx = Parser->szCharSize.cy = -1;
		GetCharSize( NULL, lpcd );

		/*
		 *	Setup view size.
		 */
		SetupViewSize( lpcd );

		/*
		 *	Re-render.
		 */
		InvalidateRect( hWnd, NULL, TRUE );
	}

	/*
	 *	Call the superclass.
	 */
	return DefWindowProc( hWnd, WM_SETFONT, wParam, lParam );
}

LRESULT OnSetLogFont( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	HFONT		hFont;

	/*
	 *	Create the font.
	 */
	if (( hFont = CreateFontIndirect(( LOGFONT * )lParam )) != NULL )
	{
		/*
		 *	Is it fixed width?
		 */
		if ( IsFixedWidth( hWnd, hFont ))
		{
			/*
			 *	Setup the font.
			 */
			SendMessage( hWnd, WM_SETFONT, ( WPARAM )hFont, MAKELPARAM( TRUE, 0 ));
			return TRUE;
		}

		/*
		 *	Destroy the font.
		 */
		DeleteObject( hFont );
	}
	return FALSE;
}

void SetupViewSize( LPCLASSDATA lpcd )
{
	RECT		rcClient;

	/*
	 *	Make sure the character size
	 *	is known.
	 */
	GetCharSize( NULL, lpcd );

	/*
	 *	Get client rectangle.
	 */
	GetClientRect( lpcd->hWnd, &rcClient );

	/*
	 *	Compute view size.
	 */
	lpcd->szViewSize.cx = ( RCWIDTH( rcClient ) - ( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ))) / Parser->szCharSize.cx;
	lpcd->szViewSize.cy = RCHEIGHT( rcClient ) / Parser->szCharSize .cy;

	/*
	 *	Setup scrollers.
	 */
	SetupHScroller( lpcd );
	SetupVScroller( lpcd );
}

LRESULT OnSize( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	int nLines = Brainchild_ArrayGetSize( lpcd->lpLines ) - 1;

	/*
	 *	Compute view size.
	 */
	SetupViewSize( lpcd );

	/*
	 *	Keep the last line in range.
	 */
	if (( lpcd->ptViewPos.y + lpcd->szViewSize.cy - 1 ) > nLines )
		lpcd->ptViewPos.y = max( 0, nLines - lpcd->szViewSize.cy + 1 );

	/*
	 *	Delete the compatible DC.
	 */
	if ( lpcd->hdcCompat )
	{
		SelectObject( lpcd->hdcCompat, lpcd->hbmOld );
		DeleteDC( lpcd->hdcCompat );
		lpcd->hdcCompat = NULL;
		lpcd->hbmOld = NULL;
	}

	/*
	 *	And the compatible bitmap.
	 */
	if ( lpcd->hbmCompat )
	{
		DeleteObject( lpcd->hbmCompat );
		lpcd->hbmCompat = NULL;
	}

	return 0;
}

static void ShowSelection( LPCLASSDATA lpcd, HDC hDC, int nLine, int nLength, TCHAR *pcBuf )
{
	int	nSCol, nECol, nAdd = 0, nLeft, nWidth, nSX, nEX;
	RECT	rcRect;

	/*
	 *	Do we have a marker?
	 */
	if ( ! HasMark( lpcd ))
		return;

	/*
	 *	Is this a marked line.
	 */
	if ( nLine >= lpcd->ptSelStart.y && nLine <= lpcd->ptSelEnd.y )
	{
		/*
		 *	Get rid of the offset.
		 */
		nLength -= lpcd->ptViewPos.x;

		/*
		 *	Adjust columns.
		 */
		nSX = CaretOffsetLine( lpcd, lpcd->ptSelStart.y, lpcd->ptSelStart.x );
		nEX = CaretOffsetLine( lpcd, lpcd->ptSelEnd.y,   lpcd->ptSelEnd.x   );

		/*
		 *	Get client rectangle.
		 */
		GetClientRect( lpcd->hWnd, &rcRect );

		/*
		 *	Adjust for the view...
		 */
		rcRect.left  += ( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ));
		rcRect.right  = rcRect.left + ( lpcd->szViewSize.cx * Parser->szCharSize.cx );
		rcRect.bottom = rcRect.top  + ( lpcd->szViewSize.cy * Parser->szCharSize.cy );

		/*
		 *	Is this the top marked line.
		 */
		if ( nLine == lpcd->ptSelStart.y )
		{
			/*
			 *	If the starting column of the block is equal to
			 *	or less than the view x position we start at offset
			 *	0. Otherwise we compute the offset.
			 */
			if ( nSX <= lpcd->ptViewPos.x ) nSCol = 0;
			else			        nSCol = nSX - lpcd->ptViewPos.x;

			/*
			 *	Do not go to far.
			 */
			if ( nSCol > nLength )
				nSCol = nLength;


			/*
			 *	Is the top line the same
			 *	as the bottom line?
			 */
			if ( nLine == lpcd->ptSelEnd.y )
			{
				/*
				 *	Compute the end column.
				 */
				nECol = nEX - lpcd->ptViewPos.x;

				/*
				 *	Including the line break?
				 */
				if ( nECol > nLength )
				{
					nAdd = Parser->szCharSize.cx / 2;
					nECol = nLength;
				}

				/*
				 *	No more than will fit.
				 */
				nECol = nECol > lpcd->szViewSize.cx ? lpcd->szViewSize.cx : nECol;
			}
			else
			{
				/*
				 *	Maximum is the end of the line
				 *	or the end of the view.
				 */
				nECol = nLength > lpcd->szViewSize.cx ? lpcd->szViewSize.cx : nLength;
				nAdd = Parser->szCharSize.cx / 2;
			}
		}
		else if ( nLine == lpcd->ptSelEnd.y )
		{
			/*
			 *	Bottom line. Start at offset 0 and end at
			 *	the marked column.
			 */
			nSCol = 0;
			nECol = nEX - lpcd->ptViewPos.x;

			/*
			 *	Including the line break?
			 */
			if ( nECol > nLength )
			{
				nECol = nLength;
				nAdd = Parser->szCharSize.cx / 2;
			}

			/*
			 *	No more than will fit.
			 */
			nECol = nECol > lpcd->szViewSize.cx ? lpcd->szViewSize.cx : nECol;
		}
		else
		{
			/*
			 *	All other lines are done
			 *	completely.
			 */
			nSCol = 0;
			nECol = nLength > lpcd->szViewSize.cx ? lpcd->szViewSize.cx : nLength;
			nAdd = Parser->szCharSize.cx / 2;
		}

		if ( nECol == nLength + 1 )
			nAdd = Parser->szCharSize.cx / 2;

		/*
		 *	Compute the left edge and width
		 *	of the inversion we need to show.
		 */
		nLeft  = rcRect.left + ( nSCol * Parser->szCharSize.cx );
		nWidth = ( nECol - nSCol ) * Parser->szCharSize.cx;

		/*
		 *	Will the line break inverion still
		 *	fit inside the view bounds.
		 */
		if (( nLeft + nWidth + nAdd ) <= rcRect.right )
			nWidth += nAdd;

		/*
		 *	Only render the marked part
		 *	of the line if it is really visible.
		 */
		if ( nWidth + nLeft > rcRect.left )
		{
			/*
			 *	Make sure we can render.
			 */
			if ( nLeft > rcRect.right )
				return;

			/*
			 *	Compute the selection rectangle.
			 */
			rcRect.left   = nLeft;
			rcRect.top    = ( nLine - lpcd->ptViewPos.y ) * Parser->szCharSize.cy;
			rcRect.right  = nLeft + nWidth;
			rcRect.bottom = rcRect.top + Parser->szCharSize.cy;

			/*
			 *	Render the selection rectangle.
			 */
			if ( lpcd->bHasFocus || lpcd->hFindWnd || lpcd->hReplaceWnd ) FillRect( hDC, &rcRect, lpcd->hbSelBkgnd );
			else							      FillRect( hDC, &rcRect, ( HBRUSH )( COLOR_BTNFACE + 1 ));

			{
				/*
				 *	Obtain the maximum number of characters
				 *	we can render inside the selection box.
				 */
				int nNumC, nOffset = lpcd->ptViewPos.x + nSCol;
				SIZE szDummy;
				if ( GetTextExtentExPoint( hDC, &pcBuf[ nOffset ], nECol - nSCol, nWidth, &nNumC, NULL, &szDummy ))
				{
					/*
					 *	Any characters to render?
					 */
					if ( nNumC )
					{
						/*
						 *	Setup the text color and render
						 *	the text without disturbing the
						 *	the background.
						 */
						COLORREF crOldTC = SetTextColor( hDC, ( lpcd->bHasFocus || lpcd->hFindWnd || lpcd->hReplaceWnd ) ? Parser->crColors[ CARR_SELECTED_TEXT ] : GetSysColor( COLOR_GRAYTEXT ));
						int nOldBkMode = SetBkMode( hDC, TRANSPARENT );

						/*
						 *	Render the characters...
						 */
						TextOut( hDC, rcRect.left, rcRect.top, &pcBuf[ nOffset ], nNumC );

						/*
						 *	Reset text color and BkMode.
						 */
						SetBkMode( hDC, nOldBkMode );
						SetTextColor( hDC, crOldTC );
					}
				}
			}
		}
	}
}

SYNTAX_COLOR *GetColorBlocks( LPCLASSDATA lpcd, int nBlocks )
{
	/*
	 *	Are there enough already?
	 */
	if ( lpcd->lpscColorArray && lpcd->nColorBlocks >= nBlocks )
		return lpcd->lpscColorArray;

	/*
	 *	Free previous allocation...
	 */
	if ( lpcd->lpscColorArray )
		Brainchild_FreePooled( lpcd->pMemPool, lpcd->lpscColorArray );

	/*
	 *	Allocate color blocks.
	 */
	if (( lpcd->lpscColorArray = Brainchild_AllocPooled( lpcd->pMemPool, nBlocks * sizeof( SYNTAX_COLOR ))) != NULL )
		/*
		 *	Save number of blocks.
		 */
		lpcd->nColorBlocks = nBlocks;

	/*
	 *	Return array pointer.
	 */
	return lpcd->lpscColorArray;
}

static void FixCasing( LPCLASSDATA lpcd, LPLINE lpLine, int nLine, TCHAR *pszBuffer, SYNTAX_COLOR *scArray, int nBlocks )
{
	/*
	 *	Does this line need case-fixing?
	 *	This flag is set for each line that
	 *	is edited in some way when the case
	 *	fixing flag is turned on.
	 */
	if ( lpLine->dwFlags & LNF_FIXCASING )
	{
		/*
		 *	Loop through the syntax color blocks.
		 */
		int i;
		for ( i = 0; i < nBlocks; i++ )
		{
			/*
			 *	If there is a keyword hash in this
			 *	syntax color block we know it is a
			 *	keyword and we will update the casing.
			 */
			if ( scArray[ i ].pHash != NULL )
			{
				/*
				 *	Offset 0 or the end of the previous SYNTAX_COLOR
				 *	block is where the case corrected keyword must
				 *	be inserted.
				 */
				int nOffset = i ? scArray[ i - 1 ].nColumn : 0;

				/*
				 *	Only correct casing where necessary.
				 */
				if ( memcmp( &pszBuffer[ nOffset ], scArray[ i ].pHash->pszKeyword, REAL_SIZE( scArray[ i ].pHash->nLength )))
				{
					/*
					 *	Simply copy the case correction directly
					 *	into the line and output buffers. Note that
					 *	we use offset 0 or the offset in the previous
					 *	SYNTAX_COLOR block since that is the offset
					 *	at which the keyword starts.
					 */
					memcpy( &lpLine->pcText[ TextOffsetLine( lpcd, nLine, nOffset, NULL ) ], scArray[ i ].pHash->pszKeyword, REAL_SIZE( scArray[ i ].pHash->nLength ));
					memcpy( &pszBuffer[ nOffset ], scArray[ i ].pHash->pszKeyword, REAL_SIZE( scArray[ i ].pHash->nLength ));
				}
			}
		}
		/*
		 *	Casing has been fixed.
		 */
		lpLine->dwFlags &= ~LNF_FIXCASING;
	}
}

static void OutputLine( LPCLASSDATA lpcd, HDC hDC, int nLine, BOOL bRender )
{
	/*
	 *	Is the line visible?
	 */
	if ( bRender && nLine >= lpcd->ptViewPos.y && nLine <= ( lpcd->ptViewPos.y + lpcd->szViewSize.cy - 1 ))
	{
		TCHAR	       *pcBuf;
		LPLINE		lpLine;
		int		nNewLen = 0, nPos = 0, i, nBlock = -1;

		/*
		 *	Get a pointer to the line structure.
		 */
		if (( lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, nLine )) != NULL )
		{
			/*
			 *	Compute the "real" size.
			 */
			for ( i = 0; i < lpLine->nLength; i++ )
			{
				/*
				 *	Is it a tab?
				 */
				if ( lpLine->pcText[ i ] == _T( '\t' ))
					/*
					 *	Add the number of spaces to
					 *	the next tab stop.
					 */
					nNewLen += ( Parser->nTabSize - ( nNewLen % Parser->nTabSize ));
				else
					/*
					 *	One more.
					 */
					nNewLen++;
			}

			/*
			 *	Is it necessary to allocate a buffer?
			 */
			if ( lpcd->pszBuffer == NULL || lpcd->nBufLen < nNewLen )
			{
				/*
				 *	Free the current buffer if it exists
				 *	and allocate the new one.
				 */
				if ( lpcd->pszBuffer ) free( lpcd->pszBuffer );
				lpcd->pszBuffer = malloc( REAL_SIZE( max( 1024, nNewLen + 1 )));
				lpcd->nBufLen = max( 1024, nNewLen + 1 );
			}

			/*
			 *	OK?
			 */
			if (( pcBuf = lpcd->pszBuffer ) != NULL )
			{
				/*
				 *	Expand tabs.
				 */
				for ( i = 0; i < lpLine->nLength; i++ )
				{
					/*
					 *	What's the character?
					 */
					switch ( lpLine->pcText[ i ] )
					{
						case	_T( '\t' ):
						{
							/*
							 *	Compute the number of spaces to the next
							 *	tab-stop.
							 */
							int nSpace = ( Parser->nTabSize - ( nPos % Parser->nTabSize ));

							/*
							 *	Add the spaces.
							 */
							while ( nSpace )
							{
								pcBuf[ nPos ] = _T( ' ' );
								nSpace--;
								nPos++;
							}
							break;
						}

						default:
							/*
							 *	Copy the character.
							 */
							pcBuf[ nPos++ ] = lpLine->pcText[ i ];
					}
				}

				/*
				 * 	Make sure the buffer is truncated.
				 */
				pcBuf[ nNewLen ] = 0;

				{
					/*
					 *	Syntax coloring structures. For each character
					 *	in the text one of these will be allocated plus
					 *	two extra ones.
					 *
					 *	If this fails one default structure is used
					 *	which simply makes the text use the
					 *	window text color.
					 */
					SYNTAX_COLOR	*scArray;
					SYNTAX_COLOR	 scDefault = { ( COLORREF )CARR_TEXT, CLR_DEFAULT, nNewLen, NULL };
					BOOL		 bBreak = FALSE;
					int		 nBlocks = 1;

					/*
					 *	Get block.
					 */
					if ( nLine > 0 )
						nBlock = (( LPLINE )Brainchild_ArrayGetAt( lpcd->lpLines, nLine - 1 ))->nBlock;

					/*
					 *	Get the array of syntax
					 *	coloring structures.
					 */
					if ( lpcd->bSyntaxColoring && lpcd->lpCurrParser->bSyntaxColoring && (( scArray = GetColorBlocks( lpcd, nNewLen + 2 )) != NULL ))
					{
						/*
						 *	Parse this line.
						 */
						if (( nBlock = ParseLine( lpcd, nBlock, pcBuf, nNewLen, scArray, &nBlocks, nNewLen + 2, nLine )) != lpLine->nBlock )
						{
							/*
							 *	Store new block number.
							 */
							lpLine->nBlock = nBlock;

							/*
							 *	The block differed so we render
							 *	the lines below this one to make
							 *	sure they are updated aswell.
							 */
							if ( nLine < Brainchild_ArrayGetSize( lpcd->lpLines ) - 1 )
								/*
								 *	Setting this boolean will force
								 *	the WM_DRAW message handler to
								 *	render or parse the line below
								 *	this one aswell until we reach
								 *	the end of the block or	the last
								 *	line in the file.
								 */
								lpcd->bRefreshBelow = TRUE;
						}
					}
					else
						/*
						 *	Use the default array.
						 */
						scArray = &scDefault;

					/*
					 *	Visible spaces?
					 */
					if ( Parser->bVisibleSpaces )
					{
						/*
						 *	Iterate the text.
						 */
						for ( i = 0, nPos = 0; i < lpLine->nLength; i++ )
						{
							/*
							 *	What's the character?
							 */
							switch ( lpLine->pcText[ i ] )
							{
								case	_T( '\t' ):
								{
									/*
									 *	Compute the number of spaces to the next
									 *	tab-stop.
									 */
									/*int nSpace =*/ ( Parser->nTabSize - ( nPos % Parser->nTabSize ));

									/*
									 *	Add tab-indicator.
									 */
									pcBuf[ nPos ] = _T( '¤' );
									nPos += ( Parser->nTabSize - ( nPos % Parser->nTabSize ));
									break;
								}

								case	_T( ' ' ):
									/*
									 *	Add space-indicator.
									 */
									pcBuf[ nPos++ ] = _T( '·' );
									break;

								default:
									/*
									 *	Increase buffer position.
									 */
									nPos++;
									break;
							}
						}
					}

					{
						/*
						 *	Get current offset.
						 */
						int	nIndex = lpcd->ptViewPos.x;

						/*
						 *	Compute the initial
						 *	rendering offsets.
						 */
						int	 cx = GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ), cy = ( nLine - lpcd->ptViewPos.y ) * Parser->szCharSize.cy;
						int	 nWillFit;
						int	 nOldMode = GetBkMode( hDC );
						COLORREF crOldFg = GetTextColor( hDC );
						COLORREF crOldBg = GetBkColor( hDC );

						/*
						 *	Fix casing.
						 */
						FixCasing( lpcd, lpLine, nLine, pcBuf, scArray, nBlocks );

						/*
						 *	Bookmark?
						 */
						if (( lpLine->dwFlags & LNF_BOOKMARK ) == LNF_BOOKMARK )
						{
							RECT	rcClient;

							/*
							 *	Get client rectangle.
							 */
							GetClientRect( lpcd->hWnd, &rcClient );

							/*
							 *	Margin?
							 */
							if ( Parser->bSelectionMargin )
							{
								/*
								 *	Make round rect.
								 */
								SelectObject( hDC, lpcd->hbBookmark );
								RoundRect( hDC, rcClient.top, cy, GetMarginWidth( lpcd ) - 1, cy + Parser->szCharSize.cy, Parser->szCharSize.cx, Parser->szCharSize.cy / 2 );
							}
							else
							{
								/*
								 *	Adjust rectangle.
								 */
								RECT rc;
								rcClient.top   += cy;
								rcClient.bottom = rcClient.top + Parser->szCharSize.cy;

								/*
								 *	Fill line.
								 */
								rc = rcClient;
								rc.left += GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd );
								FillRect( hDC, &rc, lpcd->hbBookmark );
							}
						}

						/*
						 *	Line-numbers?
						 */
						if ( Parser->bLineNumbers )
						{
							/*
							 *	Line numbers larger than 99999 will be converted to
							 *	 ----- for now. Should not be a big problem.
							 */
							int	 nLength;
							TCHAR    sz[ 32 ];
							if ( nLine > 99998 ) _tcscpy( sz, _T( "-----" ));
							else		     _itot( nLine + 1, sz, 10 );

							/*
							 *	Make sure the correct font is
							 *	set.
							 */
							if (( HFONT )GetCurrentObject( hDC, OBJ_FONT ) != Parser->hScreenFont )
								SelectObject( hDC, Parser->hScreenFont );

							/*
							 *	Render the line number.
							 */
							SetBkMode( hDC, TRANSPARENT );
							SetTextColor( hDC, Parser->crColors[ CARR_LINE_NUMBERS ] );
							nLength = _tcslen( sz );
							TextOut( hDC, GetMarginWidth( lpcd ) + ((  5 - nLength ) * Parser->szCharSize.cx ), cy, sz, nLength );
						}

						/*
						*	Is this the line under the caret and are we
						*	highlighting the current line?
						*/
						if ( nLine == lpcd->ptCaretPos.y && Parser->bHighlightCurrentLine )
						{
							/*
							 *	Obtain client rectangle.
							 */
							RECT rcClient;
							GetClientRect( lpcd->hWnd, &rcClient );

							/*
							 *	Adjust for this line.
							 */
							rcClient.left   = GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd );
							rcClient.top    = cy;
							rcClient.bottom = rcClient.top + Parser->szCharSize.cy;

							/*
							 *	Render highlight color.
							 */
							FillRect( hDC, &rcClient, lpcd->hbLineHighlight );
						}

						/*
						 *	Go through the syntax
						 *	coloring blocks.
						 */
						for ( i = 0; i < nBlocks; i++ )
						{
							/*
							 *	Compute the number of characters
							 *	to render in this block.
							 */
							int nChars = scArray[ i ].nColumn - nIndex;

							/*
							 *	Any visible characters
							 *	in this block?
							 */
							if ( nChars > 0 )
							{
								/*
								 *	Check if we need to change the font.
								 */
								if ( scArray[ i ].wFlags & SCF_HYPERLINK )
								{
									/*
									 *	If the underlined font is not
									 *	set we do so now.
									 */
									if (( HFONT )GetCurrentObject( hDC, OBJ_FONT ) != Parser->hScreenFontUL )
										SelectObject( hDC, Parser->hScreenFontUL );
								}
								else
								{
									/*
									 *	If the normal font is not set
									 *	we do so now.
									 */
									if (( HFONT )GetCurrentObject( hDC, OBJ_FONT ) != Parser->hScreenFont )
										SelectObject( hDC, Parser->hScreenFont );
								}

								/*
								 *	Setup the text color, background color
								 *	and rendering mode.
								 */
								SetTextColor( hDC, scArray[ i ].crColor );

								/*
								 *	If the background color is CLR_DEFAULT
								 *	we render transparent. If it is not
								 *	we render opaque with the given
								 *	background color.
								 */
								if ( scArray[ i ].crBgColor != CLR_DEFAULT )
								{
									SetBkMode( hDC, OPAQUE );
									SetBkColor( hDC, scArray[ i ].crBgColor );
								}
								else
									SetBkMode( hDC, TRANSPARENT );

								/*
								 *	Compute the number of characters
								 *	we can render.
								 */
								if ((( nChars + nIndex ) - lpcd->ptViewPos.x ) > lpcd->szViewSize.cx )
								{
									/*
									 *	The text we render will go off
									 *	the right side of the view so we
									 *	render as much as will fit and
									 *	break the rendering loop.
									 */
									nWillFit = lpcd->szViewSize.cx - ( nIndex - lpcd->ptViewPos.x );
									bBreak = TRUE;
								}
								else
									/*
									 *	All characters will fit.
									 */
									nWillFit = nChars;

								/*
								 *	Render the text.
								 */
								if ( nIndex < nNewLen )
									TextOut( hDC, cx, cy, &pcBuf[ nIndex ], nWillFit );

								/*
								 *	Did we pass the view it's
								 *	right side?
								 */
								if ( bBreak )
									break;

								/*
								 *	Adjust index and the
								 *	horizontal rendering offset.
								 */
								nIndex += nChars;
								cx += nChars * Parser->szCharSize.cx;
							}
						}

						/*
						 *	Restore mode etc.
						 */
						SetTextColor( hDC, crOldFg );
						SetBkColor( hDC, crOldBg );
						SetBkMode( hDC, nOldMode );
					}
				}

				/*
				 *	Show selection.
				 */
				ShowSelection( lpcd, hDC, nLine, nNewLen, pcBuf );
			}
		}
	}
	else if ( ! bRender )
	{
		LPLINE		lpLine;
		int		nBlock = -1, nDummy;

		/*
		 *	Syntax coloring on?
		 */
		if ( lpcd->bSyntaxColoring && lpcd->lpCurrParser->bSyntaxColoring )
		{
			/*
			 *	Get block.
			 */
			if ( nLine > 0 )
				nBlock = (( LPLINE )Brainchild_ArrayGetAt( lpcd->lpLines, nLine - 1 ))->nBlock;

			/*
			 *	Get line pointer.
			 */
			lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, nLine );

			/*
			 *	Parse the line to see if the block has changed...
			 */
			if (( nBlock = ParseLine( lpcd, nBlock, lpLine->pcText, lpLine->nLength, NULL, &nDummy, 0, nLine )) != lpLine->nBlock )
			{
				/*
				 *	Setup new block value.
				 */
				lpLine->nBlock = nBlock;
				lpcd->bRefreshBelow = TRUE;
			}
		}
	}
}

void RenderLine( LPCLASSDATA lpcd, int nLine )
{
	RECT		rcClient;

	/*
	 *	Get client rectangle.
	 */
	GetClientRect( lpcd->hWnd, &rcClient );

	/*
	 *	Adjust rectangle top and bottom.
	 */
	rcClient.top    = ( rcClient.top + ( nLine - lpcd->ptViewPos.y ) * Parser->szCharSize.cy );
	rcClient.bottom = rcClient.top + Parser->szCharSize.cy;

	/*
	 *	Force the line to
	 *	re-render.
	 */
	InvalidateRect( lpcd->hWnd, &rcClient, FALSE );
}

void RenderLines( LPCLASSDATA lpcd, int nFrom, int nTo )
{
	int	i;

	/*
	 *	This is inefficient code since it will
	 *	invalidate the rectangle for each line
	 *	it needs to re-render one-by-one instead
	 *	of invalidating the rectangle off all
	 *	lines at once.
	 */
	if ( nFrom <= nTo )
	{
		/*
		 *	Make sure the nFrom and nTo values
		 *	do not fall outside of the screen
		 *	area.
		 */
		if ( nFrom < lpcd->ptViewPos.y			     ) nFrom = lpcd->ptViewPos.y;
		if ( nTo   > lpcd->ptViewPos.y + lpcd->szViewSize.cy ) nTo   = lpcd->ptViewPos.y + lpcd->szViewSize.cy;

		/*
		 *	Render lines "nFrom" through "nTo"
		 */
		for ( i = nFrom; i <= nTo; i++ )
			RenderLine( lpcd, i );
	}
	else
	{
		/*
		 *	Make sure the nFrom and nTo values
		 *	do not fall outside of the screen
		 *	area.
		 */
		if ( nTo   < lpcd->ptViewPos.y			     ) nFrom = lpcd->ptViewPos.y;
		if ( nFrom > lpcd->ptViewPos.y + lpcd->szViewSize.cy ) nTo   = lpcd->ptViewPos.y + lpcd->szViewSize.cy;

		/*
		 *	Render lines "nTo" through "nFrom"
		 */
		for ( i = nTo; i <= nFrom; i++ )
			RenderLine( lpcd, i );
	}
}

static HDC CreateOSEnvironment( LPCLASSDATA lpcd, HDC hDC )
{
	HDC		hdcCompat = NULL;
	HBITMAP		hbmCompat;
	RECT		rcClient;

	/*
	 *	Get client rectangle.
	 */
	GetClientRect( lpcd->hWnd, &rcClient );

	/*
	 *	Create compatible DC.
	 */
	if (( hdcCompat = CreateCompatibleDC( hDC )) != NULL )
	{
		/*
		 *	Create compatible bitmap.
		 */
		if (( hbmCompat = CreateCompatibleBitmap( hDC, RCWIDTH( rcClient ), RCHEIGHT( rcClient ))) != NULL )
		{
			/*
			 *	Set 'm up.
			 */
			lpcd->hbmOld = SelectObject( hdcCompat, hbmCompat );

			lpcd->hdcCompat = hdcCompat;
			lpcd->hbmCompat = hbmCompat;
		}
		else
		{
			/*
			 *	Delete DC.
			 */
			DeleteDC( hdcCompat );
			hdcCompat = NULL;
		}
	}
	return hdcCompat;
}

/*
 *	Lighten the input color.
 */
COLORREF LightenColor( COLORREF cr, double dRed, double dGreen, double dBlue )
{
	BYTE r, g, b;

	r = GetRValue( cr );
	g = GetGValue( cr );
	b = GetBValue( cr );

	r = ( BYTE )(( dRed   * ( 255 - r )) + r );
	g = ( BYTE )(( dGreen * ( 255 - g )) + g );
	b = ( BYTE )(( dBlue  * ( 255 - b )) + b );
	return RGB( r, g, b );
}

/*
 *	Darken the input color.
 */
COLORREF DarkenColor( COLORREF cr, double dRed, double dGreen, double dBlue )
{
	BYTE r, g, b;

	r = GetRValue( cr );
	g = GetGValue( cr );
	b = GetBValue( cr );

	r = ( BYTE )( r - ( dRed   * r ));
	g = ( BYTE )( g - ( dGreen * g ));
	b = ( BYTE )( b - ( dBlue  * b ));
	return RGB( r, g, b );
}

static void PaintControl( HDC hDC, LPCRECT pClipRect, LPCLASSDATA lpcd )
{
	RECT		rcClient;
	HDC		osDC;
	int		sDC, i, cy, nLines = Brainchild_ArrayGetSize( lpcd->lpLines ), nLast = lpcd->ptViewPos.y + lpcd->szViewSize.cy;

	/*
	 *	Obtain client rectangle.
	 */
	GetClientRect( lpcd->hWnd, &rcClient );

	/*
	 *	Anything to render?
	 */
	if ( IsRectEmpty( &rcClient ))
		return;

	/*
	 *	Get the offscreen device
	 *	context.
	 */
	if (( osDC = lpcd->hdcCompat ) == NULL )
	{
		/*
		 *	The DC does not exist.
		 *	Create DC and Bitmap.
		 */
		if (( osDC = CreateOSEnvironment( lpcd, hDC )) == NULL )
			/*
			 *	Use input DC.
			 */
			osDC = hDC;
	}

	/*
	 *	Save device context state.
	 */
	sDC = SaveDC( osDC );

	/*
	 *	Set font.
	 */
	SelectObject( osDC, ( HFONT )SendMessage( lpcd->hWnd, WM_GETFONT, 0, 0 ));

	/*
	 *	Selection margin?
	 */
	if ( Parser->bSelectionMargin )
	{
		RECT	rcMargin = rcClient;
		HPEN	hOld;

		/*
		 *	Make sure we know the font
		 *	size.
		 */
		GetCharSize( osDC, lpcd );

		/*
		 *	Adjust margin rectangle and
		 *	the client rectangle
		 */
		rcMargin.right = GetMarginWidth( lpcd );
		rcClient.left  = rcMargin.right;

		/*
		 *	Render the margin.
		 */
		if ( Parser->bSolidMargin ) FillRect( osDC, &rcMargin, lpcd->hbSolidSelectionMargin );
		else			    FillRect( osDC, &rcMargin, lpcd->hbSelectionMargin );

		/*
		 *	Render margin right-side.
		 */
		hOld = SelectObject( osDC, lpcd->hpMargin );
		MoveToEx( osDC, rcMargin.right - 1, rcMargin.top, NULL );
		LineTo( osDC, rcMargin.right - 1, rcMargin.bottom );
		SelectObject( osDC, hOld );
	}

	/*
	 *	Do we show line-numbers?
	 */
	if ( Parser->bLineNumbers )
	{
		RECT	rcMargin = rcClient;

		/*
		 *	Make sure we know the font
		 *	size.
		 */
		GetCharSize( osDC, lpcd );

		/*
		 *	Adjust margin rectangle and
		 *	the client rectangle
		 */
		rcMargin.right = GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ) - 4;
		rcClient.left  = rcMargin.right;

		/*
		 *	Render the line number margin.
		 */
		FillRect( osDC, &rcMargin, lpcd->hbLineMarginBkgnd );

		/*
		 *	Render margin right-side.
		 */
		rcMargin.left = rcMargin.right - 1;
		FillRect( osDC, &rcMargin, lpcd->hbLineMargin );
	}

	/*
	 *	Render the background.
	 */
	FillRect( osDC, &rcClient, ISREADONLY ? lpcd->hbBackgroundRO : lpcd->hbBackground );

	/*
	 *	Setup the font.
	 */
	SelectObject( osDC, Parser->hScreenFont );

	/*
	 *	Transparent...
	 */
	SetBkColor( osDC, ISREADONLY ? Parser->crColors[ CARR_BACKGROUND_READONLY ] : Parser->crColors[ CARR_BACKGROUND ] );

	/*
	 *	Start at the client
	 *	area top-edge.
	 */
	cy = rcClient.top;

	/*
	 *	Must we render beyond the last pre-parsed line?
	 */
	if (( DWORD )nLast > lpcd->dwParsedUpTo )
		PreParseTo( lpcd, nLast );

	/*
	 *	Render lines...
	 */
	for ( i = 0; i < lpcd->szViewSize.cy; i++ )
	{
		/*
		 *	Is this line located vertically
		 *	in the area that needs to be
		 *	re-painted?
		 */
		if (( cy + Parser->szCharSize.cy ) > pClipRect->top && cy < pClipRect->bottom )
		{
			/*
			 *	There are only so many lines...
			 */
			if ( i + lpcd->ptViewPos.y >= nLines )
				break;
			else
			{
				/*
				 *	Clear the boolean.
				 */
				lpcd->bRefreshBelow = FALSE;

				/*
				 *	Render the line.
				 */
				OutputLine( lpcd, osDC, i + lpcd->ptViewPos.y, TRUE );
			}
		}
		else if (( cy + Parser->szCharSize.cy ) > pClipRect->bottom )
		{
			/*
			 *	We passed the bottom of the area to render. If the
			 *	bRefreshBelow boolean is set we must continue
			 *	calling OutputLine() so that we do set the correct
			 *	block numbers for the lines below the last line
			 *	we rendered.
			 */
			if ( lpcd->bRefreshBelow )
			{
				/*
				 *	Loop until we do not need to refresh the line below
				 *	the previous one or until we reach the last line.
				 */
				while ( lpcd->bRefreshBelow && ( i + lpcd->ptViewPos.y < nLines ))
				{
					/*
					 *	Clear the refresh boolean.
					 */
					lpcd->bRefreshBelow = FALSE;

					/*
					 *	Call the OutputLine() function which will
					 *	determine if the line below this one still
					 *	needs refreshing.
					 *
					 *	At this point OutputLine() will not actually
					 *	render anything. It will only update the block
					 *	number of the line.
					 */
					OutputLine( lpcd, osDC, i + lpcd->ptViewPos.y, FALSE );

					/*
					 *	Next...
					 */
					i++;
				}

				/*
				 *	Refresh the whole client area.
				 */
				InvalidateRect( lpcd->hWnd, NULL, FALSE );
			}
			break;
		}

		/*
		 *	Next...
		 */
		cy += Parser->szCharSize.cy;
	}

	/*
	 *	Do we need to render the column marker?
	 */
	if ( Parser->bColumnMarker && Parser->nMarkerOffset )
	{
		/*
		 *	We only need to render it when it is
		 *	visible in the current view.
		 */
		if ( lpcd->ptViewPos.x < Parser->nMarkerOffset && ( lpcd->ptViewPos.x + lpcd->szViewSize.cx ) >= Parser->nMarkerOffset )
		{
			int xPos = GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ) + (( Parser->nMarkerOffset - lpcd->ptViewPos.x ) * Parser->szCharSize.cx ) - 1;
			HPEN hOld = SelectObject( osDC, lpcd->hpColumnMarker );
			MoveToEx( osDC, xPos, 0, NULL );
			LineTo( osDC, xPos, rcClient.bottom );
			SelectObject( osDC, hOld );
		}
	}

	/*
	 *	If the DC we rendered in is the
	 *	same as we have stored in the
	 *	instance data of the control we
	 *	must blit the contents of the
	 *	DC to the on screen DC.
	 */
	if ( osDC == lpcd->hdcCompat )
		BitBlt( hDC, pClipRect->left,
			     pClipRect->top,
			     pClipRect->right - pClipRect->left,
			     pClipRect->bottom - pClipRect->top,
			     osDC,
			     pClipRect->left,
			     pClipRect->top,
			     SRCCOPY );

	/*
	 *	Restore device context.
	 */
	RestoreDC( osDC, sDC );
}

LRESULT OnPrintClient( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	HDC		hDC = ( HDC )wParam;
	RECT		rc;

	/*
	 *	Obtain clipping rectangle.
	 */
	GetClipBox( hDC, &rc );

	/*
	 *	Anything to paint?
	 */
	if ( IsRectEmpty( &rc ))
		return 0;

	/*
	 *	Paint...
	 */
	PaintControl( hDC, &rc, lpcd );
	return 0;
}

LRESULT OnPaint( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	PAINTSTRUCT	ps;
	HDC		hDC;

	/*
	 *	Any painting to do?
	 */
	if ( GetUpdateRect( hWnd, NULL, FALSE ) == 0 )
		return 0;

	/*
	 *	Begin painting...
	 */
	hDC = BeginPaint( hWnd, &ps );

	/*
	 *	Paint...
	 */
	PaintControl( hDC, &ps.rcPaint, lpcd );

	/*
	 *	End painting.
	 */
	EndPaint( hWnd, &ps );
	return 0;
}

LRESULT OnEraseBkgnd( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Background rendering is handled
	 *	by the OnPaint() message handler.
	 */
	return 0;
}
