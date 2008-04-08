/*
 *	caret.c
 *
 *	(C) Copyright 1993-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Caret related routines.
 */

#include "stdafx.h"

#include "defs.h"

void CreateTheCaret( LPCLASSDATA lpcd )
{
	int	cx, cy;

	/*
	 *	Make sure the character size is
	 *	present.
	 */
	GetCharSize( NULL, lpcd );

	/*
	 *	Setup caret height.
	 */
	cy = Parser->szCharSize.cy;

	/*
	 *	When in insert mode we switch to the
	 *	block style caret.
	 */
	if ( lpcd->bOverwrite ) cx = Parser->szCharSize.cx;
	else
	{
		/*
		 *	Default width.
		 */
		cx = GetSystemMetrics( SM_CXBORDER );

		/*
		 *	What type of caret?
		 */
		if (      Parser->nCaretType == CARET_WIDE  )  cx *= 2;
		else if ( Parser->nCaretType == CARET_BLOCK ) cx = Parser->szCharSize.cx;
		else if ( Parser->nCaretType == CARET_HORIZONTAL )
		{
			cx = Parser->szCharSize.cx;
			cy = GetSystemMetrics( SM_CYBORDER ) * 2;
		}
	}

	/* 
	 *	Create and update the caret.
	 */
	CreateCaret( lpcd->hWnd, NULL, cx, cy );
	UpdateCaret( lpcd );
}

/*
 *	Send a caret position message.
 */
void SendCaretMessage( LPCLASSDATA lpcd )
{
	NMCARETPOSITION		nmcp;
	HWND			hParent;

	/*
	 *	Do we have a parent?
	 */
	if (( hParent = GetParent( lpcd->hWnd )) != NULL )
	{
		/*
		 *	Fill in the structure.
		 */
		nmcp.hdr.code		= NMBC_CARETPOSITION;
		nmcp.hdr.hwndFrom	= lpcd->hWnd;
		nmcp.hdr.idFrom		= GetWindowLong( lpcd->hWnd, GWL_ID );
		nmcp.ptCaretPos		= lpcd->ptCaretPos;

		/*
		 *	Convert to screen coordinates.
		 */
		nmcp.ptCaretPos.x = GetCaretOffset( lpcd, nmcp.ptCaretPos.x );
		nmcp.ptCaretPos.x++;
		nmcp.ptCaretPos.y++;

		/*
		 *	Send the notification if the
		 *	position really changed.
		 */
		if ( nmcp.ptCaretPos.x != lpcd->ptLastPositionSent.x ||
		     nmcp.ptCaretPos.y != lpcd->ptLastPositionSent.y )
		{
			SendMessage( hParent, WM_NOTIFY, nmcp.hdr.idFrom, ( LPARAM )&nmcp );
			lpcd->ptLastPositionSent = nmcp.ptCaretPos;
		}
	}


	/*
	 *	Re-render the line if current line
	 *	highlighting is on.
	 */
	if ( Parser->bHighlightCurrentLine )
	{
		if ( lpcd->ptCaretPos.y != Parser->nHighlighted ) RenderLine( lpcd, Parser->nHighlighted );
		RenderLine( lpcd, lpcd->ptCaretPos.y );
		Parser->nHighlighted = lpcd->ptCaretPos.y;
	}

	/*
	 *	Higlight brackets?
	 */
	if ( Parser->bColorBracketMatches )
	{
		/*
		 *	Render previous match ( if any )
		 */
		if ( lpcd->ptBracket1.y >= 0 ) RenderLine( lpcd, lpcd->ptBracket1.y );
		if ( lpcd->ptBracket2.y != lpcd->ptBracket1.y && lpcd->ptBracket2.y >= 0 ) RenderLine( lpcd, lpcd->ptBracket2.y );

		/*
		 *	Find match.
		 */
		if ( FindMatches( lpcd, &lpcd->ptBracket1, &lpcd->ptBracket2, TRUE ) < 0 )
		{
			/*
			 *	No match found. 
			 */
			lpcd->ptBracket1.x = lpcd->ptBracket1.y = -1;
			lpcd->ptBracket2.x = lpcd->ptBracket2.y = -1;
		}

		/*
		 *	Render the lines on which the
		 *	matches are found.
		 */
		if ( lpcd->ptBracket1.y >= 0 ) RenderLine( lpcd, lpcd->ptBracket1.y );
		if ( lpcd->ptBracket2.y != lpcd->ptBracket1.y && lpcd->ptBracket2.y >= 0 ) RenderLine( lpcd, lpcd->ptBracket2.y );
	}
}

LRESULT OnSetFocus( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	We have the focus.
	 */
	lpcd->bHasFocus = TRUE;

	/*
	 *	Compute view.
	 */
	SetupViewSize( lpcd );

	/*
	 *	Make the caret.
	 */
	CreateTheCaret( lpcd );

	/*
	 *	Send a status message.
	 */
	SendStatusMessage( lpcd );

	/*
	 *	Refresh the view.
	 */
	if ( HasMark( lpcd ))
		InvalidateRect( hWnd, NULL, FALSE );

	/*
	 *	Is the search or replace window active?
	 */
	if ( lpcd->hReplaceWnd && ! IsWindowVisible( lpcd->hReplaceWnd )) ShowWindow( lpcd->hReplaceWnd, SW_SHOW );
	if ( lpcd->hFindWnd && ! IsWindowVisible( lpcd->hFindWnd )) ShowWindow( lpcd->hFindWnd, SW_SHOW );
	return 0;
}

LRESULT OnKillFocus( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Hide tooltip.
	 */
	HideTooltip( lpcd );

	/*
	 *	We no longer have the focus.
	 */
	lpcd->bHasFocus = lpcd->bCaretVisible = FALSE;
	lpcd->cQualifier = 0;
	DestroyCaret();
	if ( HasMark( lpcd ))
		InvalidateRect( hWnd, NULL, FALSE );

	/*
	 *	Is the search or replace window active?
	 */
	if ( lpcd->hReplaceWnd && ! IsChild( lpcd->hReplaceWnd, GetFocus())) ShowWindow( lpcd->hReplaceWnd, SW_HIDE );
	if ( lpcd->hFindWnd && ! IsChild( lpcd->hFindWnd, GetFocus())) ShowWindow( lpcd->hFindWnd, SW_HIDE );
	return 0;
}

int CaretOffsetLine( LPCLASSDATA lpcd, int nLine, int nColumn )
{
	LPLINE		lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, nLine );
	int		nPos = 0, i;

	if ( lpLine && lpLine->pcText )
	{
		/*
		 *	Iterate characters.
		 */
		for ( i = 0; i < lpLine->nLength && i < nColumn; i++ )
		{
			/*
			 *	A tab?
			 */
			if ( lpLine->pcText[ i ] == _T( '\t' ))
				/*
				 *	Add number of spaces to the next tab-stop.
				 */
				nPos += Parser->nTabSize - ( nPos % Parser->nTabSize );
			else
				/*
				 *	One more.
				 */
				nPos++;
		}
	}
	return nPos;
}

int GetCaretOffset( LPCLASSDATA lpcd, int nColumn )
{
	return CaretOffsetLine( lpcd, lpcd->ptCaretPos.y, nColumn );
}

int TextOffsetLine( LPCLASSDATA lpcd, int nLine, int nColumn, BOOL *pTruncated )
{
	LPLINE	lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, nLine );
	int i = 0, nPos = 0;

	/*
	 *	Any text?
	 */
	if ( lpLine->pcText )
	{
	        /*
		 *	Iterate characters.
		 */
		for ( i = 0; i < lpLine->nLength; i++ )
	        {
		        /*
			 *	Is this a tab?
			 */
		        if ( lpLine->pcText[ i ] == _T( '\t' ))
			        /*
				 *	Add number of spaces to the next tab stop.
				 */
		                nPos += Parser->nTabSize - ( nPos % Parser->nTabSize );
			else
				/*
				 *	One more.
				 */
			        nPos++;

			/*
			 *	If we passed the requested column we can
			 *	stop here since we have reached the text
			 *	offset we are looking for.
			 */
			if ( nPos > nColumn )
				break;
		}
		/*
		 *	The contents of the pTruncated pointer, if valid, 
		 *	will be set to TRUE if the position computed is short
		 *	of the column requested.
		 */
		if ( pTruncated ) *pTruncated = ( BOOL )( nPos < nColumn ? TRUE : FALSE );
        }
        return i;
}

int GetTextOffset( LPCLASSDATA lpcd, int nColumn )
{
	return TextOffsetLine( lpcd, lpcd->ptCaretPos.y, nColumn, NULL );
}

void DisplayCaret( LPCLASSDATA lpcd, BOOL bShow )
{
	/*
	 *	Only when we have the focus.
	 */
	if ( lpcd->bHasFocus == FALSE )
		return;

	/*
	 *	Anything changed?
	 */
	if ( bShow != lpcd->bCaretVisible )
	{
		/*
		 *	Change it.
		 */
		if (( lpcd->bCaretVisible = bShow ) == TRUE )
		{
			int cy = ( Parser->nCaretType == CARET_HORIZONTAL && ! lpcd->bOverwrite ) ? Parser->szCharSize.cy - ( 2 * GetSystemMetrics( SM_CYBORDER )) : 0;
			SetCaretPos( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ) + (( GetCaretOffset( lpcd, lpcd->ptCaretPos.x ) - lpcd->ptViewPos.x ) * Parser->szCharSize.cx ), ( lpcd->ptCaretPos.y - lpcd->ptViewPos.y ) * Parser->szCharSize.cy + cy );
			ShowCaret( lpcd->hWnd );
		}
		else
			HideCaret( lpcd->hWnd );
	}

	/*
	 *	Send caret position message.
	 */
	SendCaretMessage( lpcd );
}

BOOL CaretInView( LPCLASSDATA lpcd )
{
	int	nRealPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Is the current caret position inside
	 *	the view area?
	 */
	if ( nRealPos >= lpcd->ptViewPos.x && nRealPos <= ( lpcd->ptViewPos.x + ( lpcd->szViewSize.cx - 1 )) &&
	     lpcd->ptCaretPos.y >= lpcd->ptViewPos.y && lpcd->ptCaretPos.y <= ( lpcd->ptViewPos.y + ( lpcd->szViewSize.cy - 1 )))
		return TRUE;

	return FALSE;
}

void UpdateCaret( LPCLASSDATA lpcd )
{
	/*
	 *	Do we have the focus?
	 */
	if ( lpcd->bHasFocus == TRUE || lpcd->bDragOver == TRUE )
	{
		/*
		 *	Caret inside the view?
		 */
		if ( CaretInView( lpcd ))
		{
			/*
			 *	Already visible?
			 */
			if ( lpcd->bCaretVisible == FALSE )
				/*
				 *	No make it visible.
				 */
				DisplayCaret( lpcd, TRUE );
			else
			{
				/*
				 *	Re-position.
				 */
				int cy = ( Parser->nCaretType == CARET_HORIZONTAL && ! lpcd->bOverwrite ) ? Parser->szCharSize.cy - ( 2 * GetSystemMetrics( SM_CYBORDER )) : 0;
				SetCaretPos( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ) + (( GetCaretOffset( lpcd, lpcd->ptCaretPos.x ) - lpcd->ptViewPos.x ) * Parser->szCharSize.cx ), ( lpcd->ptCaretPos.y - lpcd->ptViewPos.y ) * Parser->szCharSize.cy + cy );
			}
		}
		else if ( lpcd->bCaretVisible == TRUE )
			/*
			 *	Hide the caret.
			 */
			DisplayCaret( lpcd, FALSE );
	}

	/*
	 *	Send caret position message.
	 */
	SendCaretMessage( lpcd );
}

static void MakeVisible( LPCLASSDATA lpcd, BOOL bRedraw )
{
	int	nDiffY = lpcd->ptViewPos.y;
	BOOL	bRender = FALSE;

	/*
	 *	Is the caret visible?
	 */
	if ( CaretInView( lpcd ) == FALSE )
	{
		/*
		 *	Get "real" caret position.
		 */
		int	nXPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

		/*
		 *	Is it in the view horizontally?
		 */
		if (( nXPos < lpcd->ptViewPos.x ) || ( nXPos > lpcd->ptViewPos.x + ( lpcd->szViewSize.cx - 1 )))
		{
			lpcd->ptViewPos.x = max( 0, nXPos - ( lpcd->szViewSize.cx ) / 2 );
			bRender = TRUE;
		}

		/*
		 *	Is it in the view vertically?
		 */
		if ( lpcd->ptCaretPos.y < lpcd->ptViewPos.y )
		{
			lpcd->ptViewPos.y = lpcd->ptCaretPos.y;
		}
		else if ( lpcd->ptCaretPos.y > lpcd->ptViewPos.y + ( lpcd->szViewSize.cy - 1 ))
		{
			lpcd->ptViewPos.y = max( 0, lpcd->ptCaretPos.y - ( lpcd->szViewSize.cy - 1 ));
		}

		/*
		 *	Redraw?
		 */
		if ( bRedraw )
		{
			/*
			 *	Compute difference in Y direction.
			 */
			nDiffY = nDiffY - lpcd->ptViewPos.y;

			/*
			 *	View moved?
			 */
			if ( nDiffY != 0 && bRender == FALSE )
			{
				/*
				 *	Scroll view.
				 */
				VScroll( lpcd, nDiffY );

				/*
				 *	Setup vertical scroller.
				 */
				SetupVScroller( lpcd );
			}
			else if ( bRender == TRUE )
			{
				/*
				 *	Setup scrollers.
				 */
				SetupHScroller( lpcd );
				SetupVScroller( lpcd );

				/*
				 *	Re-render.
				 */
				InvalidateRect( lpcd->hWnd, NULL, TRUE );
			}
		}
	}
}

void MakeCaretVisible( LPCLASSDATA lpcd )
{
	MakeVisible( lpcd, TRUE );
}

void MakeCaretVisibleNoRedraw( LPCLASSDATA lpcd )
{
	MakeVisible( lpcd, FALSE );
}

static void MoveRight( LPCLASSDATA lpcd )
{
        /*
	 *	Caret at the end of the line?
	 */
	if ( lpcd->ptCaretPos.x == GETLINE( lpcd )->nLength )
        {
	        /*
		 *	Yes. Are we at the last line?
		 */
                if ( lpcd->ptCaretPos.y < Brainchild_ArrayGetSize( lpcd->lpLines ) - 1 )
	        {
		        /*
			 *	Go down one.
			 */
		        lpcd->ptCaretPos.y++;
			lpcd->ptCaretPos.x = 0;
                }
	}
	else
	        /*
		 *	One right.
		 */
	        lpcd->ptCaretPos.x++;

        /*
	 *	Save off this position.
	 */
        lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

        /*
	 *	Make sure the caret is in the view.
	 */
        MakeCaretVisible( lpcd );

        /*
	 *	Update caret position.
         */
	UpdateCaret( lpcd );
}

void CaretRight( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Block marker set?
	 */
	if ( HasMark( lpcd ))
	{
		/*
		 *	Move to the end of the marker.
		 */
		lpcd->ptCaretPos = lpcd->ptSelEnd;

		/*
		 *	Clear marker.
		 */
		ClearMark( lParam, lpcd );

		/*
		 *	Update position.
		 */
		UpdateCaret( lpcd );
		return;
	}
	/*
	 *	Move the caret right.
	 */
	MoveRight( lpcd );
}

static void MoveLeft( LPCLASSDATA lpcd )
{
        /*
	 *	Caret at the start of the line?
	 */
	if ( lpcd->ptCaretPos.x == 0 )
        {
	        /*
		 *	Yes. Are we at the first line?
		 */
	        if ( lpcd->ptCaretPos.y > 0 )
		{
                        /*
			 *	Go up one.
			 */
			lpcd->ptCaretPos.y--;

                        /*
			 *	Go to the end of the line.
			 */
			lpcd->ptCaretPos.x = GETLINE( lpcd )->nLength;
                }
	}
	else
	        /*
		 *	One left.
		 */
	        lpcd->ptCaretPos.x--;

        /*
	 *	Save off this position.
	 */
        lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

        /*
	 *	Make sure the care is in the view.
	 */
        MakeCaretVisible( lpcd );

        /*
	 *	Update caret position.
	 */
        UpdateCaret( lpcd );
}

void CaretLeft( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Do we have a block marker?
	 */
	if ( HasMark( lpcd ))
	{
		/*
		 *	Move to the start of the block.
		 */
		lpcd->ptCaretPos = lpcd->ptSelStart;

		/*
		 *	Clear the marker.
		 */
		ClearMark( lParam, lpcd );

		/*
		 *	Update position.
		 */
		UpdateCaret( lpcd );
		return;
	}
	/*
	 *	Move left....
	 */
	MoveLeft( lpcd );
}

static void MoveUp( LPCLASSDATA lpcd )
{
	/*
	 *	Caret at the first line?
	 */
	if ( lpcd->ptCaretPos.y > 0 )
        {
		/*
		 *	Fast scrolling?
		 */
		if ( Parser->bFastScroll && lpcd->bRepeatMode )
		{
			/* 
			 *	Are we at the top of the view?
			 */
			if ( lpcd->ptCaretPos.y == lpcd->ptViewPos.y )
				/*
				 *	Setup new position.
				 */
				lpcd->ptCaretPos.y = max( lpcd->ptCaretPos.y - 2, 0 );
			else
				/*
				 *	One line up.
				 */
				 lpcd->ptCaretPos.y--;
		}
		else
			/*
			 *	Go one up.
			 */
			lpcd->ptCaretPos.y--;

                /*
		 *	Try to locate the caret as close to the
		 *	last column position as possible since
                 *	the user changed the column.
		 */
		lpcd->ptCaretPos.x = GetTextOffset( lpcd,  lpcd->nLastColumnPos );

                /*
		 *	Are we passed the end?
		 */
                if ( lpcd->ptCaretPos.x > GETLINE( lpcd )->nLength )
	                /*
			 *	Set us at the end.
			 */
			lpcd->ptCaretPos.x = GETLINE( lpcd )->nLength;
	}

        /*
	 *	Make sure the care is in the view.
	 */
        MakeCaretVisible( lpcd );

        /*
	 *	Update caret position.
	 */
        UpdateCaret( lpcd );
}

void CaretUp( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Do we have a marker?
	 */
	if ( HasMark( lpcd ))
	{
		/*
		 *	Move to start marker.
		 */
		lpcd->ptCaretPos = lpcd->ptSelStart;

		/*
		 *	Clear marker.
		 */
		ClearMark( lParam, lpcd );
	}

	/*
	 *	Move up.
	 */
	MoveUp( lpcd );
}

static void MoveDown( LPCLASSDATA lpcd )
{
	/*
	 *	Caret at the last line?
	 */
	if ( lpcd->ptCaretPos.y < Brainchild_ArrayGetSize( lpcd->lpLines ) - 1 )
	{
		/*
		 *	Fast scrolling?
		 */
		if ( Parser->bFastScroll && lpcd->bRepeatMode )
		{
			/* 
			 *	Are we at the top of the view?
			 */
			if ( lpcd->ptCaretPos.y == ( lpcd->ptViewPos.y + lpcd->szViewSize.cy - 1 ))
				/*
				 *	Setup new position.
				 */
				lpcd->ptCaretPos.y = min( lpcd->ptCaretPos.y + 2, Brainchild_ArrayGetSize( lpcd->lpLines ) - 1 );
			else
				/*
				 *	One line up.
				 */
				 lpcd->ptCaretPos.y++;
		}
		else
			/*
			 *	Go one down.
			 */
			lpcd->ptCaretPos.y++;

                /*
		 *	Try to locate the cursor as close to the
		 *	last column position as possible since
                 *	the user changed the column.
		 */
		lpcd->ptCaretPos.x = GetTextOffset( lpcd, lpcd->nLastColumnPos );

		/*
		 *	Are we passed the end?
		 */
                if ( lpcd->ptCaretPos.x > GETLINE( lpcd )->nLength )
	                /*
			 *	Set us at the end.
			 */
                        lpcd->ptCaretPos.x = GETLINE( lpcd )->nLength;
        }
	else
	{
		/*
		 *	Caret at the end of the line?
		 */
		if ( lpcd->ptCaretPos.x < GETLINE( lpcd )->nLength )
			lpcd->ptCaretPos.x = GETLINE( lpcd )->nLength;
	}

        /*
	 *	Make sure the care is in the view.
	 */
        MakeCaretVisible( lpcd );

        /*
	 *	Update caret position.
	 */
        UpdateCaret( lpcd );
}

void CaretDown( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Do we have a marker?
	 */
	if ( HasMark( lpcd ))
	{
		/*
		 *	Move to end marker.
		 */
		lpcd->ptCaretPos = lpcd->ptSelEnd;

		/*
		 *	Clear marker.
		 */
		ClearMark( lParam, lpcd );
	}

	/*
	 *	Move down.
	 */
	MoveDown( lpcd );
}

static void MoveEol( LPCLASSDATA lpcd )
{
	/*
	 *	Are we already there?
	 */
	if ( lpcd->ptCaretPos.x < GETLINE( lpcd )->nLength )
	{
		/*
		 *	No, move to the end.
		 */
		lpcd->ptCaretPos.x = GETLINE( lpcd )->nLength;

		/*
		 *	Save this position.
		 */
		lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );
	}

	/*
	 *	Make sure the care is in the view.
	 */
	MakeCaretVisible( lpcd );

	/*
	 *	Update caret position.
	 */
	UpdateCaret( lpcd );
}

void CaretEol( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Clear marker.
	 */
	ClearMark( lParam, lpcd );

	/*
	 *	Move to eol.
	 */
	MoveEol( lpcd );
}

static void MoveSol( LPCLASSDATA lpcd )
{
	int nFirst = 0;
	LPLINE pLine = GETLINE( lpcd );

	/*
	 *	Any characters on the line?
	 */
	if ( pLine->nLength == 0 ) return;

	/*
	 *	Find first character offset.
	 */
	while ( nFirst < pLine->nLength && _istspace( pLine->pcText[ nFirst ] ) ) nFirst++;

	/*
	 *	Are we already there?
	 */
	if ( lpcd->ptCaretPos.x > 0 && lpcd->ptCaretPos.x <= nFirst )
		/*
		 *	No, move to the start.
		 */
		lpcd->ptCaretPos.x = 0;
	else
		/*
		 *	No, move to the start.
		 */
		lpcd->ptCaretPos.x = nFirst;

	/*
	 *	Save this position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Make sure the care is in the view.
	 */
	MakeCaretVisible( lpcd );

	/*
	 *	Update caret position.
	 */
	UpdateCaret( lpcd );
}

void CaretSol( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Clear marker.
	 */
	ClearMark( lParam, lpcd );

	/*
	 *	Move to sol.
	 */
	MoveSol( lpcd );
}

static void MoveNextWord( LPCLASSDATA lpcd )
{
	/*
	 *	Obtain current caret column and a
	 *	pointer to the line.
	 */
	int	nPos = lpcd->ptCaretPos.x;
	LPLINE	lpLine = GETLINE( lpcd );

	/*
	 *	Are we at the end of the line?
	 */
	if ( nPos == lpLine->nLength )
	{
		/*
		 *	Yes go down one line and to the
		 *	first word on that line.
		 */
		if ( lpcd->ptCaretPos.y < Brainchild_ArrayGetSize( lpcd->lpLines ) - 1 )
		{
			/*
			 *	Start at 0
			 */
			nPos = 0;

			/*
			 *	Go down.
			 */
			lpcd->ptCaretPos.y++;

			/*
			 *	Skip spaces.
			 */
			lpLine = GETLINE( lpcd );

			if ( lpLine->pcText )
			{
				while ( _istspace( lpLine->pcText[ nPos ] ) && nPos < GETLINE( lpcd )->nLength )
					nPos++;
			}

			/*
			 *	Store new column position.
			 */
			lpcd->ptCaretPos.x = nPos;
		}
	}
	else
	{
		BOOL	bBreakOnNonSpace = FALSE;

		/*
		 *	Are we on a white space?
		 */
		if ( _istspace( lpLine->pcText[ nPos ] ))
		{
			/*
			 *	Skip spaces...
			 */
			while ( _istspace( lpLine->pcText[ nPos ] ) && nPos < lpLine->nLength )
				nPos++;
		}
		else if ( IsDelimiter( lpcd, lpLine->pcText [ nPos ] )) /* A delimiter? */
		{
			/*
			 *	Skip all delimiters and white spaces.
			 */
			while (( IsDelimiter( lpcd, lpLine->pcText[ nPos ] ) || _istspace( lpLine->pcText[ nPos ] )) && nPos < lpLine->nLength )
			{
				/*
				 *	White space?
				 */
				if ( _istspace( lpLine->pcText[ nPos ] ))
					/*
					 *	Yes. Break on the next non-white space.
					 */
					bBreakOnNonSpace = TRUE;
				else if ( bBreakOnNonSpace )
					/*
					 *	Stop at this non-white space.
					 */
					break;
				nPos++;
			}
		}
		else
		{
			/*
			 *	Skip all non delimiters and white spaces.
			 */
			while (( IsDelimiter( lpcd, lpLine->pcText[ nPos ] ) == FALSE || _istspace( lpLine->pcText[ nPos ] )) && nPos < lpLine->nLength )
			{
				/*
				 *	White space?
				 */
				if ( _istspace( lpLine->pcText[ nPos ] ))
					/*
					 *	Yes. Break on the next non-white space.
					 */
					bBreakOnNonSpace = TRUE;
				else if ( bBreakOnNonSpace )
					/*
					 *	Stop at this non-white space.
					 */
					break;
				nPos++;
			}
		}

		/*
		 *	Store position.
		 */
		lpcd->ptCaretPos.x = nPos;
	}

	/*
	 *	Save this position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Make sure the caret is in the view.
	 */
	MakeCaretVisible( lpcd );

	/*
	 *	Update caret position.
	 */
	UpdateCaret( lpcd );
}

void CaretNextWord( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Clear marker.
	 */
	ClearMark( lParam, lpcd );

	/*
	 *	Caret to the next word.
	 */
	MoveNextWord( lpcd );
}

static void MovePrevWord( LPCLASSDATA lpcd )
{
	/*
	 *	Obtain current caret column and
	 *	the line were on.
	 */
	int	nPos = lpcd->ptCaretPos.x;
	LPLINE	lpLine = GETLINE( lpcd );

	/*
	 *	Are we at the start of the line?
	 */
	if ( nPos == 0 )
	{
		/*
		 *	Yes go down one line and to the
		 *	end of that line.
		 */
		if ( lpcd->ptCaretPos.y > 0 )
		{
			/*
			 *	Go up.
			 */
			lpcd->ptCaretPos.y--;

			/*
			 *	Put the caret at the end.
			 */
			lpcd->ptCaretPos.x = GETLINE( lpcd )->nLength;

			if ( lpcd->ptCaretPos.x )
				/*
				 *	Recursivly call ourselves.
				 */
				MovePrevWord( lpcd );
		}
	}
	else
	{
		/*
		 *	Previous character.
		 */
		nPos--;

		/*
		 *	Are we on a white space?
		 */
		if ( _istspace( lpLine->pcText[ nPos ] ))
		{
			/*
			 *	Skip spaces...
			 */
			while ( _istspace( lpLine->pcText[ nPos ] ) && nPos )
				nPos--;
		}

		/*
		 *	Start of the line?
		 */
		if ( nPos )
		{
			/*
			 *	Did we reach a delimiter?
			 */
			if ( IsDelimiter( lpcd, lpLine->pcText[ nPos ] )) /* A delimiter? */
			{
				/*
				 *	Skip all delimiters.
				 */
				while ( IsDelimiter( lpcd, lpLine->pcText[ nPos ] ))
				{
					/*
					 *	White space?
					 */
					if ( _istspace( lpLine->pcText[ nPos ] ))
						break;

					/*
					 *	Start of the line?
					 */
					if ( nPos == 0 )
					{
						/*
						 *	Will force the caret at position 0
						 */
						nPos = -1;
						break;
					}
					nPos--;
				}
				nPos++;
			}
			else
			{
				/*
				 *	Skip all non delimiters.
				 */
				while ( IsDelimiter( lpcd, lpLine->pcText[ nPos ] ) == FALSE )
				{
					/*
					 *	White space?
					 */
					if ( _istspace( lpLine->pcText[ nPos ] ))
						break;

					/*
					 *	Start of the line
					 */
					if ( nPos == 0 )
					{
						/*
						 *	Will force the caret at position 0
						 */
						nPos = -1;
						break;
					}
					nPos--;
				}
				nPos++;
			}
		}

		/*
		 *	Store position.
		 */
		lpcd->ptCaretPos.x = nPos;
	}

	/*
	 *	Save this position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Make sure the care is in the view.
	 */
	MakeCaretVisible( lpcd );

	/*
	 *	Update caret position.
	 */
	UpdateCaret( lpcd );
}

void CaretPrevWord( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Clear marker.
	 */
	ClearMark( lParam, lpcd );

	/*
	 *	Caret to the previous word.
	 */
	MovePrevWord( lpcd );
}

static void MoveHome( LPCLASSDATA lpcd )
{
	/*
	 *	Are we already there?
	 */
	if ( lpcd->ptCaretPos.x > 0 || lpcd->ptCaretPos.y > 0 )
	{
		/*
		 *	No. Put us there.
		 */
		lpcd->ptCaretPos.x = lpcd->ptCaretPos.y = 0;

		/*
		 *	Save this position.
		 */
		lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

		/*
		 *	Make sure the care is in the view.
		 */
		MakeCaretVisible( lpcd );

		/*
		 *	Update caret position.
		 */
		UpdateCaret( lpcd );
	}
}

void CaretHome( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Clear marker.
	 */
	ClearMark( lParam, lpcd );

	/*
	 *	Move the caret home.
	 */
	MoveHome( lpcd );
}

static void MoveEnd( LPCLASSDATA lpcd )
{
	/*
	 *	Are we there yet?
	 */
	if ( lpcd->ptCaretPos.y == Brainchild_ArrayGetSize( lpcd->lpLines ) - 1 && lpcd->ptCaretPos.x == GETLINE( lpcd )->nLength )
		/*
		 *	Yes.
		 */
		return;

	/*
	 *	Put us on the last line.
	 */
	lpcd->ptCaretPos.y = Brainchild_ArrayGetSize( lpcd->lpLines ) - 1;

	/*
	 *	Move us to the end.
	 */
	lpcd->ptCaretPos.x = GETLINE( lpcd )->nLength;

	/*
	 *	Save this position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Make sure the care is in the view.
	 */
	MakeCaretVisible( lpcd );

	/*
	 *	Update caret position.
	 */
	UpdateCaret( lpcd );
}

void CaretEnd( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Clear marker.
	 */
	ClearMark( lParam, lpcd );

	/*
	 *	Move the caret to the end.
	 */
	MoveEnd( lpcd );
}

static void MoveTabBack( LPCLASSDATA lpcd )
{
	int	nScreenPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x ), nPos;

	/*
	 *	Are we at the start of the line?
	 */
	if ( lpcd->ptCaretPos.x == 0 )
		return;

	/*
	 *	Compute the number of characters we are
	 *	right of a tab-stop.
	 */
	nPos = nScreenPos % Parser->nTabSize;

	/*
	 *	Go left the amount of position computed above
	 *	or a whole tab-stop.
	 */
	nPos = nScreenPos - ( nPos ? nPos : Parser->nTabSize );

	/*
	 *	Position the caret.
	 */
	lpcd->ptCaretPos.x = GetTextOffset( lpcd, nPos );

	/*
	 *	Save this position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Make sure the care is in the view.
	 */
	MakeCaretVisible( lpcd );

	/*
	 *	Update caret position.
	 */
	UpdateCaret( lpcd );
}

void CaretTabBack( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Clear marker.
	 */
	ClearMark( lParam, lpcd );

	/*
	 *	Move the caret back a tab stop.
	 */
	MoveTabBack( lpcd );
}

static void MoveViewUp( LPCLASSDATA lpcd )
{
	/*
	 *	Are we on the top?
	 */
	if ( lpcd->ptCaretPos.y > 0 )
	{
		/*
		 *	Hide the caret.
		 */
		DisplayCaret( lpcd, FALSE );

		/*
		 *	Are we on the top of the view?
		 */
		if ( lpcd->ptCaretPos.y == lpcd->ptViewPos.y )
			/*
			 *	Move up a view.
			 */
			lpcd->ptCaretPos.y = max( 0, lpcd->ptCaretPos.y - lpcd->szViewSize.cy );
		else
			/*
			 *	Go to the top of the view.
			 */
			 lpcd->ptCaretPos.y = lpcd->ptViewPos.y;

		/*
		 *	Make sure the caret is visible.
		 */
		MakeCaretVisible( lpcd );

		/*
		 *	Show the caret.
		 */
		DisplayCaret( lpcd, TRUE );
	}
}

void CaretViewUp( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Clear marker.
	 */
	ClearMark( lParam, lpcd );

	/*
	 *	Move the caret a view up.
	 */
	MoveViewUp( lpcd );
}

static void MoveViewDown( LPCLASSDATA lpcd )
{
	int	nLines = Brainchild_ArrayGetSize( lpcd->lpLines ) - 1;

	/*
	 *	Are we already at the end?
	 */
	if ( lpcd->ptCaretPos.y < nLines )
	{
		/*
		 *	Hide the caret.
		 */
		DisplayCaret( lpcd, FALSE );

		/*
		 *	Are we on the bottom of the view?
		 */
		if ( lpcd->ptCaretPos.y == ( lpcd->ptViewPos.y + lpcd->szViewSize.cy - 1 ))
			/*
			 *	Move down a page.
			 */
			lpcd->ptCaretPos.y = min( lpcd->ptCaretPos.y + lpcd->szViewSize.cy - 1, nLines );
		else
			/*
			 *	Move to the bottom of the page.
			 */
			lpcd->ptCaretPos.y = min( lpcd->ptViewPos.y + lpcd->szViewSize.cy - 1, nLines );

		/*
		 *	Make sure the caret is visible.
		 */
		MakeCaretVisible( lpcd );

		/*
		 *	Show the caret.
		 */
		DisplayCaret( lpcd, TRUE );
	}
}

void CaretViewDown( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Clear marker.
	 */
	ClearMark( lParam, lpcd );

	/*
	 *	Move the caret a view down.
	 */
	MoveViewDown( lpcd );
}

void ScrollViewDown( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Are we there already?
	 */
	if ( lpcd->ptViewPos.y > 0 )
	{
		/*
		 *	Go up one.
		 */
		lpcd->ptViewPos.y--;
		VScroll( lpcd, 1 );

		/*
		 *	Setup the scroller.
		 */
		SetupVScroller( lpcd );

		/*
		 *	Caret OK?
		 */
		if ( lpcd->ptCaretPos.y > ( lpcd->ptViewPos.y + lpcd->szViewSize.cy - 1 ))
		{
			/*
			 *	Make it visible.
			 */
			lpcd->ptCaretPos.y = lpcd->ptViewPos.y + lpcd->szViewSize.cy - 1;
			lpcd->ptCaretPos.x = lpcd->nLastColumnPos = 0;
			UpdateCaret( lpcd );
		}
	}
}

void ScrollViewUp( LPARAM lParam, LPCLASSDATA lpcd )
{
	int	nLines = Brainchild_ArrayGetSize( lpcd->lpLines ) - 1;

	/*
	 *	Are we there already?
	 */
	if (( lpcd->ptViewPos.y + lpcd->szViewSize.cy - 1 ) < nLines )
	{
		/*
		 *	Go down one.
		 */
		lpcd->ptViewPos.y++;
		VScroll( lpcd, -1 );

		/*
		 *	Setup the scroller.
		 */
		SetupVScroller( lpcd );

		/*
		 *	Caret OK?
		 */
		if ( lpcd->ptCaretPos.y < lpcd->ptViewPos.y )
		{
			/*
			 *	Make it visible.
			 */
			lpcd->ptCaretPos.y = lpcd->ptViewPos.y;
			lpcd->ptCaretPos.x = lpcd->nLastColumnPos = 0;
			UpdateCaret( lpcd );
		}
	}
}

void ScrollViewRight( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Are we there already?
	 */
	if ( lpcd->ptViewPos.x > 0 )
	{
		/*
		 *	Go left one.
		 */
		lpcd->ptViewPos.x--;
		InvalidateRect( lpcd->hWnd, NULL, FALSE );

		/*
		 *	Setup the scroller.
		 */
		SetupHScroller( lpcd );

		/*
		 *	Update the caret.
		 */
		UpdateCaret( lpcd );
	}
}

void ScrollViewLeft( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Are we there already?
	 */
	if (( lpcd->ptViewPos.x + lpcd->szViewSize.cx - 1 ) < lpcd->nLongestLine )
	{
		/*
		 *	Go right one.
		 */
		lpcd->ptViewPos.x++;
		InvalidateRect( lpcd->hWnd, NULL, FALSE );

		/*
		 *	Setup the scroller.
		 */
		SetupHScroller( lpcd );

		/*
		 *	Update the caret.
		 */
		UpdateCaret( lpcd );
	}
}

/*
 *	Extend moves. These are simple
 *	indexes into the table defined below.
 */
#define CEM_LEFT	0
#define CEM_RIGHT	1
#define CEM_UP		2
#define CEM_DOWN	3
#define CEM_EOL		4
#define CEM_SOL		5
#define CEM_NEXTWORD	6
#define CEM_PREVWORD	7
#define CEM_HOME	8
#define CEM_END		9
#define CEM_TABBACK	10
#define CEM_VIEWUP	11
#define CEM_VIEWDOWN	12

/*
 *	Callback function table.
 */
typedef void (*CALL)( LPCLASSDATA );

/*
 *	Simple call-back functions called
 *	by the CaretMoveExtend() function
 *	to extend the markers.
 */
CALL FuncTable[] =
{
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	MoveEol,
	MoveSol,
	MoveNextWord,
	MovePrevWord,
	MoveHome,
	MoveEnd,
	MoveTabBack,
	MoveViewUp,
	MoveViewDown
};

static void CaretMoveExtend( LPCLASSDATA lpcd, int nMove )
{
	int	nOldStart, nOldEnd;

	/*
	 *	Marker set?
	 */
	if ( HasMark( lpcd ) == FALSE )
		/*
		 *	Setup starting point.
		 */
		lpcd->ptStartPos = lpcd->ptSelStart = lpcd->ptSelEnd = lpcd->ptCaretPos;

	/*
	 *	Copy old values.
	 */
	nOldStart = lpcd->ptSelStart.y;
	nOldEnd   = lpcd->ptSelEnd.y;

	/*
	 *	Move the caret.
	 */
	( FuncTable[ nMove ] )( lpcd );

	/*
	 *	Where are we?
	 */
	if ( lpcd->ptCaretPos.y < lpcd->ptStartPos.y || lpcd->ptCaretPos.y == lpcd->ptStartPos.y && lpcd->ptCaretPos.x < lpcd->ptStartPos.x )
	{
		/*
		 *	Setup new start position.
		 */
		lpcd->ptSelStart = lpcd->ptCaretPos;
		lpcd->ptSelEnd   = lpcd->ptStartPos;
	}
	else
	{
		lpcd->ptSelStart = lpcd->ptStartPos;
		lpcd->ptSelEnd   = lpcd->ptCaretPos;
	}

	/*
	 *	Update visuals.
	 */
	RenderLines( lpcd, nOldStart, lpcd->ptSelStart.y );
	RenderLines( lpcd, nOldEnd,   lpcd->ptSelEnd.y   );

	/*
	 *	Send status message.
	 */
	SendStatusMessage( lpcd );
}

/*
 *	Simple CaretXXX() routines which
 *	will extend the block markers.
 */
void CaretLeftExtend( LPARAM lParam, LPCLASSDATA lpcd )
{
	CaretMoveExtend( lpcd, CEM_LEFT );
}

void CaretRightExtend( LPARAM lParam, LPCLASSDATA lpcd )
{
	CaretMoveExtend( lpcd, CEM_RIGHT );
}

void CaretUpExtend( LPARAM lParam, LPCLASSDATA lpcd )
{
	CaretMoveExtend( lpcd, CEM_UP );
}

void CaretDownExtend( LPARAM lParam, LPCLASSDATA lpcd )
{
	CaretMoveExtend( lpcd, CEM_DOWN );
}

void CaretEolExtend( LPARAM lParam, LPCLASSDATA lpcd )
{
	CaretMoveExtend( lpcd, CEM_EOL );
}

void CaretSolExtend( LPARAM lParam, LPCLASSDATA lpcd )
{
	CaretMoveExtend( lpcd, CEM_SOL );
}

void CaretNextWordExtend( LPARAM lParam, LPCLASSDATA lpcd )
{
	CaretMoveExtend( lpcd, CEM_NEXTWORD );
}

void CaretPrevWordExtend( LPARAM lParam, LPCLASSDATA lpcd )
{
	CaretMoveExtend( lpcd, CEM_PREVWORD );
}

void CaretHomeExtend( LPARAM lParam, LPCLASSDATA lpcd )
{
	CaretMoveExtend( lpcd, CEM_HOME );
}

void CaretEndExtend( LPARAM lParam, LPCLASSDATA lpcd )
{
	CaretMoveExtend( lpcd, CEM_END );
}

void CaretTabBackExtend( LPARAM lParam, LPCLASSDATA lpcd )
{
	CaretMoveExtend( lpcd, CEM_TABBACK);
}

void CaretViewUpExtend( LPARAM lParam, LPCLASSDATA lpcd )
{
	CaretMoveExtend( lpcd, CEM_VIEWUP );
}

void CaretViewDownExtend( LPARAM lParam, LPCLASSDATA lpcd )
{
	CaretMoveExtend( lpcd, CEM_VIEWDOWN );
}
