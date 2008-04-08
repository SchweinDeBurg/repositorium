/*
 *	mouse.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"
#include "LangRes.rh"

BOOL MouseToCaret( LPCLASSDATA lpcd, int x, int y, LPPOINT pt )
{
	LPLINE		lpLine;
	int		nPos, nLastLine = Brainchild_ArrayGetSize( lpcd->lpLines ) - 1;
	BOOL		bRC = TRUE, bTruncated;

	/*
	 *	Compute text position.
	 */ 
	pt->x = lpcd->ptViewPos.x + ( x / Parser->szCharSize.cx );
	pt->y = lpcd->ptViewPos.y + ( y / Parser->szCharSize.cy );

	/*
	 *	Valid line?
	 */
	if ( pt->y > nLastLine ) 
	{ 
		pt->y = nLastLine; 
		bRC = FALSE; 
	}
	else if ( pt->y < 0 )    
	{
		pt->y = 0;
		bRC = FALSE;
	}

	/*
	 *	Valid column?
	 */
	if ( pt->x < 0 ) 
	{
		pt->x = 0;
		bRC = FALSE;
	}

	/*
	 *	Get a pointer to the line.
	 */
	lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, pt->y );

	/*
	 *	Get the text offset of the
	 *	mouse horizontal position.
	 */
	nPos = TextOffsetLine( lpcd, pt->y, pt->x, &bTruncated );
	if ( bTruncated ) bRC = FALSE;

	/*
	 *	Did it pass the end of the line?
	 */
	if ( nPos > lpLine->nLength ) 
	{
		pt->x = lpLine->nLength;
		bRC = FALSE;
	}
	else			      
		pt->x = nPos;

	return bRC;
}

BOOL MouseOnSelection( LPCLASSDATA lpcd )
{
	/*
	 *	Valid markers?
	 */
	if ( HasMark( lpcd ))
	{
		POINT		ptMousePos;

		/*
		 *	Get mouse position and convert
		 *	to client coordinates.
		 */
		GetCursorPos( &ptMousePos );
		ScreenToClient( lpcd->hWnd, &ptMousePos );

		/*
		 *	Skip selection margin.
		 */
		ptMousePos.x -= ( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ));

		/*
		 *	Is the mouse located on the selection
		 *	margin?
		 */
		if ( ptMousePos.x < 0 )
			return FALSE;

		/*
		 *	Passed the text?
		 */
		if ( lpcd->ptViewPos.y + ( ptMousePos.y / Parser->szCharSize.cy ) > Brainchild_ArrayGetSize( lpcd->lpLines ) -1 )
			return FALSE;

		/*
		 *	Get mouse position.
		 */
		MouseToCaret( lpcd, ptMousePos.x, ptMousePos.y, &ptMousePos );

		/*
		 *	Selection start and end on
		 *	the same line?
		 */
		if ( lpcd->ptSelStart.y == lpcd->ptSelEnd.y )
		{
			/*
			 *	Yes. Is the mouse located on this
			 *	line and inside the start and
			 *	end column?
			 */
			if ( ptMousePos.y == lpcd->ptSelStart.y && ptMousePos.x >= lpcd->ptSelStart.x && ptMousePos.x < lpcd->ptSelEnd.x )
				/*
				 *	Yes.
				 */
				return TRUE;
		}
		else if ( ptMousePos.y == lpcd->ptSelStart.y && ptMousePos.x >= lpcd->ptSelStart.x )
			/*
			 *	Mouse on the top line right
			 *	of the start mark.
			 */
			return TRUE;
		else if ( ptMousePos.y == lpcd->ptSelEnd.y && ptMousePos.x < lpcd->ptSelEnd.x )
			/*
			 *	Mouse on the bottom line
			 *	left of the end mark.
			 */
			return TRUE;
		else if ( ptMousePos.y > lpcd->ptSelStart.y && ptMousePos.y < lpcd->ptSelEnd.y )
			/*
			 *	Mouse in between the top
			 *	and bottom lines.
			 */
			return TRUE;
	}
	return FALSE;
}


static ExtendWordLeft( LPCLASSDATA lpcd, LPPOINT lpPoint )
{
	LPLINE		lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, lpPoint->y );

	/*
	 *	Start of the line?
	 */
	if ( lpPoint->x > 0 )
	{
		/*
		 *	No. Are we on a space?
		 */
		if ( _istspace( lpLine->pcText[ lpPoint->x ] ))
		{
			/*
			 *	Move to the first non-space.
			 */
			while ( lpPoint->x > 0 && _istspace( lpLine->pcText[ lpPoint->x ] ))
				lpPoint->x--;
		}
		else if ( IsDelimiter( lpcd, lpLine->pcText[ lpPoint->x ] ))
		{
			/*
			 *	Move to the first non-delimiter.
			 */
			while ( lpPoint->x > 0 && IsDelimiter( lpcd, lpLine->pcText[ lpPoint->x ] ))
				lpPoint->x--;
		}
		else
		{
			/*
			 *	Move to the first delimiter or
			 *	white space.
			 */
			while ( lpPoint->x > 0 && ! IsDelimiter( lpcd, lpLine->pcText[ lpPoint->x ] ) && ! _istspace( lpLine->pcText[ lpPoint->x ] ))
				lpPoint->x--;
		}
	}

	/*
	 *	Did we go to far?
	 */
	if ( lpPoint->x > 0 )
		lpPoint->x++;
}

static ExtendWordRight( LPCLASSDATA lpcd, LPPOINT lpPoint )
{
	LPLINE		lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, lpPoint->y );

	/*
	 *	End of the line?
	 */
	if ( lpPoint->x < lpLine->nLength )
	{
		/*
		 *	No. Are we on a space?
		 */
		if ( _istspace( lpLine->pcText[ lpPoint->x ] ))
		{
			/*
			 *	Move to the first non-space.
			 */
			while ( lpPoint->x < lpLine->nLength && _istspace( lpLine->pcText[ lpPoint->x ] ))
				lpPoint->x++;
		}
		else if ( IsDelimiter( lpcd, lpLine->pcText[ lpPoint->x ] ))
		{
			/*
			 *	Move to the first non-delimiter.
			 */
			while ( lpPoint->x < lpLine->nLength && IsDelimiter( lpcd, lpLine->pcText[ lpPoint->x ] ))
				lpPoint->x++;
		}
		else
		{
			/*
			 *	Move to the first delimiter or
			 *	white space.
			 */
			while ( lpPoint->x < lpLine->nLength && ! IsDelimiter( lpcd, lpLine->pcText[ lpPoint->x ] ) && ! _istspace( lpLine->pcText[ lpPoint->x ] ))
				lpPoint->x++;
		}
	}
}

LRESULT OnLButtonDown( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	int	x = ( int )( short )( GET_X_LPARAM( lParam ) - ( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd )));
	int	y = ( int )( short )( GET_Y_LPARAM( lParam ));

	/*
	 *	Do we have the focus?
	 */
	if ( GetFocus() != hWnd )
		/*
		 *	No. Make sure we get it...
		 */
		SetFocus( hWnd );

	/*
	 *	Hide the tooltip.
	 */
	HideTooltip( lpcd );

	/*
	 *	Check if we Ctrl-Clicked on a
	 *	hyperlink.
	 */
	if ( Parser->bParseHyperLinks && ( wParam & MK_CONTROL ) == MK_CONTROL && RunHyperlink( lpcd ))
		return 0;

	/*
	 *	Are we located on a selection and
	 *	is drag and drop enabled?
	 */
	if ( MouseOnSelection( lpcd ) && Parser->bDnDEnabled ) {
		DWORD	dwDummy;

		/*
		 *	We are the drag source.
		 */
		lpcd->bDragSource = TRUE;

		/*
		 *	Initiate the DnD procedure.
		 */
		DoDragDrop(( LPDATAOBJECT )&lpcd->ifDataObject, ( LPDROPSOURCE )&lpcd->ifDropSource, DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwDummy );

		/*
		 *	No longer the drag source.
		 */
		lpcd->bDragSource = FALSE;
		return 0;
	}

	/*
	 *	Shift key pressed? 
	 */
	if (( wParam & MK_SHIFT ) == MK_SHIFT )
	{
		/*
		 *	Do we already have a valid
		 *	selection start position?
		 */
		if ( lpcd->ptStartPos.x == -1 )
			/*
			 *	No. We setup the current caret
			 *	position as the selection start
			 *	position.
			 */
			lpcd->ptStartPos = lpcd->ptSelStart = lpcd->ptCaretPos;

		/*
		 *	Make the click position the current end 
		 *	position of the selection.
		 */
		MouseToCaret( lpcd, x, y, &lpcd->ptSelEnd );

		/*
		 *	If the control key is pressed we
		 *	set the end position at the start or end of the word.
		 */
		if (( wParam & MK_CONTROL ) == MK_CONTROL )
		{
			/*
			 *	End point berfore the start point?
			 */
			if ( lpcd->ptSelEnd.y < lpcd->ptStartPos.y || lpcd->ptSelEnd.y == lpcd->ptStartPos.y && lpcd->ptSelEnd.x < lpcd->ptStartPos.x )
				/*
				 *	Extend to the left.
				 */
				ExtendWordLeft( lpcd, &lpcd->ptSelEnd );
			else
				/*
				 *	Extend to the right.
				 */
				ExtendWordRight( lpcd, &lpcd->ptSelEnd );

			/*
			 *	Word selection on...
			 */
			lpcd->bWordSelect = TRUE;
		}

		/*
		 *	Make sure the selection
		 *	points are valid.
		 */
		if ( lpcd->ptSelEnd.y < lpcd->ptStartPos.y || lpcd->ptSelEnd.y == lpcd->ptStartPos.y && lpcd->ptSelEnd.x < lpcd->ptStartPos.x )
		{
			/*
			 *	Move the start of the
			 *	selection.
			 */
			lpcd->ptSelStart = lpcd->ptCaretPos = lpcd->ptSelEnd;
			lpcd->ptSelEnd   = lpcd->ptStartPos;
		}
		else
		{
			/*
			 *	Move the end of the selection.
			 */
			lpcd->ptCaretPos = lpcd->ptSelEnd;
			lpcd->ptSelStart = lpcd->ptStartPos;
		}
		
		/*
		 *	Update caret position.
		 */
		UpdateCaret( lpcd );

		/*
		 *	Make sure the visuals are updated.
		 */
		InvalidateRect( hWnd, NULL, FALSE );
	}
	else
	{
		/*
		 *	Invalidate current marker.
		 */
		ClearMark( lParam, lpcd );

		/*
		 *	Are we in the selection margin?
		 */
		if ( x < 0 )
		{
			/*
			 *	Control key pressed?
			 */
			if (( wParam & MK_CONTROL ) == MK_CONTROL )
			{
				/*
				 *	Select the whole text.
				 */
				MarkAll( lParam, lpcd );

				/*
				 *	Do not change selection.
				 */
				lpcd->bAllSelect = TRUE;
			}
			else
			{
				/*
				 *	Convert mouse position to
				 *	a text position.
				 */
				MouseToCaret( lpcd, x, y, &lpcd->ptStartPos );

				/*
				 *	We start at the beginning 
				 *	of this line.
				 */
				lpcd->ptStartPos.x = 0;

				/*
				 *	Save this position.
				 */
				lpcd->ptSelStart = lpcd->ptSelEnd = lpcd->ptStartPos;
				
				/*
				 *	Are we on the last line?
				 */
				if ( lpcd->ptSelStart.y < Brainchild_ArrayGetSize( lpcd->lpLines ) - 1 )
				{
					/*
					 *	No. Go down one and move
					 *	to the start of it.
					 */
					lpcd->ptSelEnd.y++;
					lpcd->ptSelEnd.x = 0;
				}
				else
					/*
					 *	The end of this line.
					 */
					lpcd->ptSelEnd.x = (( LPLINE )Brainchild_ArrayGetAt( lpcd->lpLines, lpcd->ptSelEnd.y ))->nLength;

				/*
				 *	Make the end position the
				 *	caret position.
				 */
				lpcd->ptCaretPos = lpcd->ptSelEnd;

				/*
				 *	We are selecting lines.
				 */
				lpcd->bLineSelect = TRUE;
			}
		}
		else
		{
			/*
			 *	Convert mouse position to
			 *	a text position.
			 */
			MouseToCaret( lpcd, x, y, &lpcd->ptStartPos );

			/*
			 *	Control key pressed?
			 */
			if (( wParam & MK_CONTROL ) == MK_CONTROL )
			{
				/*
				 *	Move to the click position.
				 */
				lpcd->ptCaretPos = lpcd->ptStartPos;

				/*
				 *	Mark the current word.
				 */
				MarkWord( lParam, lpcd );

				/*
				 *	We are word-selecting.
				 */
				lpcd->bWordSelect = TRUE;
			}
			else
				/*
				 *	Save this position.
				 */
				lpcd->ptCaretPos = lpcd->ptSelStart = lpcd->ptSelEnd = lpcd->ptStartPos;
		}		
	
		/*
		 *	Caret visible?
		 */
		if ( CaretInView( lpcd ))
			/*
			 *	Yes. Re-render the marked line.
			 */
			RenderLine( lpcd, lpcd->ptSelStart.y );
		else
			/*
			 *	Move the view.
			 */
			MakeCaretVisible( lpcd );
	}

	/*
	 *	Update caret.
	 */
	UpdateCaret( lpcd );

	/*
	 *	Mark position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Setup dragging timer.
	 */
	lpcd->nDragTimerID = SetTimer( hWnd, IDT_DRAGTIMER, 50, NULL );

	/*
	 *	Capture mouse.
	 */
	SetCapture( hWnd );

	return 0;
}

LRESULT OnLButtonUp( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	if ( lpcd->nDragTimerID != 0 )
	{
		/*
		 *	Valid block?
		 */
		if ( HasMark( lpcd ))
			SendStatusMessage( lpcd );

		/*
		 *	Release capture.
		 */
		ReleaseCapture();

		/*
		 *	Release the timer.
		 */
		KillTimer( hWnd, lpcd->nDragTimerID );
		lpcd->nDragTimerID = 0;

		/*
		 *	Mark position.
		 */
		lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

		/*
		 *	No line or word selecting.
		 */
		lpcd->bLineSelect = lpcd->bWordSelect = lpcd->bAllSelect = FALSE;

	}

	return 0;
}

LRESULT OnMouseLeave( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Hide the tooltip if the mouse did
	 *	indeen leave the client area.
	 */
	POINT pt;
	RECT rc;

	/*
	 *	Obtain the client rectangle and cursor
	 *	position.
	 */
	GetCursorPos( &pt );
	GetClientRect( hWnd,  &rc );

	/*
	 *	Convert the cursor position to client
	 *	coordinates.
	 */
	ScreenToClient( hWnd, &pt );

	/*
	 *	Only hide the tooltip if we
	 *	really left the client area.
	 */
	if ( ! PtInRect( &rc, pt ))
		HideTooltip( lpcd );

	/*
	 *	Tracking must be re-initialized. 
	 */
	lpcd->bHoverTracking = FALSE;
	return 0;
}

static void DoTooltip( LPCLASSDATA lpcd, BOOL bShow )
{
	/*
	 *	Obtain the hyperlink text which is
	 *	currently located under the cursor.
	 */
	TCHAR *pszLink = GetHyperlink( lpcd );
	if ( pszLink )
	{
		LPCTSTR  pszFormat = GetString( IDS_LINK_CLICK );
		TCHAR	 szBuffer[ 1024 ];

		/*
		 *	Will it fit in the buffer?
		 */
		if ( _tcslen( pszFormat ) + _tcslen( pszLink ) < 1024 )
		{
			/*
			 *	Format the output.
			 */
			_stprintf( szBuffer, pszFormat, pszLink );

			/*
			 *	Show the tooltip.
			 */
			if ( bShow ) ShowTooltip( lpcd, szBuffer );
			else SetTooltipText( lpcd, szBuffer );
		}
		/*
		 *	Free the URL.
		 */
		Brainchild_FreePooled( lpcd->pMemPool, pszLink );
	}
}

LRESULT OnMouseHover( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Show the tooltip.
	 */
	if ( ! lpcd->bTipShowing )
		DoTooltip( lpcd, TRUE );

	/*
	 *	Tracking must be re-initialized.
	 */
	lpcd->bHoverTracking = FALSE;
	return 0;
}

LRESULT OnMouseMove( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	POINT	ptMousePos;
	int	x = ( int )( short )( GET_X_LPARAM( lParam ) - ( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd )));
	int	y = ( int )( short )( GET_Y_LPARAM( lParam )), nOldStart, nOldEnd;

	/*
	 *	Are we tracking the mouse?
	 */
	if ( Parser->bParseHyperLinks && ! lpcd->bTipShowing && ! lpcd->bHoverTracking && lpcd->dwPrevPos != (DWORD)lParam )
	{
		/*
		 *	Setup for hover and leave tracking.
		 */
		TRACKMOUSEEVENT tme;
		tme.cbSize	= sizeof( tme );
		tme.dwFlags	= TME_HOVER | TME_LEAVE;
		tme.hwndTrack	= hWnd;
		tme.dwHoverTime	= HOVER_DEFAULT;

		/*
		 *	Activate mouse tracking.
		 */
		lpcd->bHoverTracking = _TrackMouseEvent( &tme );
		lpcd->dwPrevPos = lParam;
	}

	/*
	 *	See if we are on a hyperlink.
	 */
	if ( Parser->bParseHyperLinks && PointOnHyperlink( lpcd, x, y ) == FALSE && lpcd->bTipShowing )
	{
		/*
		 *	Hide the tooltip.
		 */
		HideTooltip( lpcd );
		lpcd->bHoverTracking = FALSE;
	}
	else if ( Parser->bParseHyperLinks && lpcd->bTipShowing )
		/*
		 *	Setup the tooltip text.
		 */
		DoTooltip( lpcd, FALSE );

	/*
	 *	Are we dragging?
	 */
	if ( lpcd->nDragTimerID != 0 )
	{
		/*
		 *	Hide the caret.
		 */
		DisplayCaret( lpcd, FALSE );

		/*
		 *	Convert mouse to text
		 *	coordinates.
		 */
		MouseToCaret( lpcd, x, y, &ptMousePos );

		/*
		 *	Save current selection
		 *	lines so we can update
		 *	it when done.
		 */
		nOldStart = lpcd->ptSelStart.y;
		nOldEnd   = lpcd->ptSelEnd.y;
			
		/*
		 *	Are we selecting lines?
		 */
		if ( lpcd->bLineSelect )
		{
			/*
			 *	Did we leave the margin?
			 */
			if ( x > 0 )
			{
				/*
				 *	Yes. Continue with normal
				 *	drag-selecting.
				 */
				SetCursor( lpcd->hIBeam );
				lpcd->bLineSelect = FALSE;
			}
			else
			{
				/*
				 *	Are we before the point
				 *	at which we initiated
				 *	the drag-selection?
				 */
				if ( ptMousePos.y < lpcd->ptStartPos.y || ptMousePos.y == lpcd->ptStartPos.y && ptMousePos.x < lpcd->ptStartPos.x )
				{
					/*
					 *	Setup new start position.
					 */
					lpcd->ptSelStart.y = ptMousePos.y;
					lpcd->ptSelStart.x = 0;

					/*
					 *	The position we started on will become the
					 *	end position.
					 */
					lpcd->ptSelEnd = lpcd->ptStartPos;

					/*
					 *	Is it on the last line?
					 */
					if ( lpcd->ptSelEnd.y == Brainchild_ArrayGetSize( lpcd->lpLines ) - 1 )
						/*
						 *	Put it at the end
						 *	of this line.
						 */
						lpcd->ptSelEnd.x = (( LPLINE )Brainchild_ArrayGetAt( lpcd->lpLines, lpcd->ptSelEnd.y ))->nLength;
					else
					{
						/*
						 *	One down and at the start.
						 */
						lpcd->ptSelEnd.y++;
						lpcd->ptSelEnd.x = 0;
					}

					/*
					 *	Move the caret to the start
					 *	of the line.
					 */
					lpcd->ptCaretPos = lpcd->ptSelStart;

				}
				else
				{
					/*
					 *	Setup new end position.
					 */
					lpcd->ptSelEnd.y = ptMousePos.y;
					lpcd->ptSelEnd.x = (( LPLINE )Brainchild_ArrayGetAt( lpcd->lpLines, ptMousePos.y ))->nLength;

					/*
					 *	The position we started on will become
					 *	the start position.
					 */
					lpcd->ptSelStart   = lpcd->ptStartPos;
					lpcd->ptSelStart.x = 0;

					/*
					 *	Is it on the last line?
					 */
					if ( lpcd->ptSelEnd.y == Brainchild_ArrayGetSize( lpcd->lpLines ) - 1 )
						/*
						 *	Put it at the end
						 *	of this line.
						 */
						lpcd->ptSelEnd.x = (( LPLINE )Brainchild_ArrayGetAt( lpcd->lpLines, lpcd->ptSelEnd.y ))->nLength;
					else
					{
						/*
						 *	One down and at the start.
						 */
						lpcd->ptSelEnd.y++;
						lpcd->ptSelEnd.x = 0;
					}

					/*
					 *	Move the caret.
					 */
					lpcd->ptCaretPos = lpcd->ptSelEnd;
				}
			}
		}
		else if ( lpcd->bWordSelect )
		{
			/*
			 *	Are we before the point
			 *	at which we initiated
			 *	the drag-selection?
			 */
			if ( ptMousePos.y < lpcd->ptStartPos.y || ptMousePos.y == lpcd->ptStartPos.y && ptMousePos.x < lpcd->ptStartPos.x )
			{
				/*
				 *	Make the position at which
				 *	we started the drag selection
				 *	the current end position.
				 */
				lpcd->ptSelEnd = lpcd->ptStartPos;

				/*
				 *	Extend the mark to the right
				 *	of the word.
				 */
				ExtendWordRight( lpcd, &lpcd->ptSelEnd );

				/*
				 *	Setup the start position
				 *	at the current mouse position.
				 */
				lpcd->ptSelStart = ptMousePos;

				/*
				 *	Extend the mark to the
				 *	start of the word.
				 */
				ExtendWordLeft( lpcd, &lpcd->ptSelStart );

				/*
				 *	Move the caret there.
				 */
				lpcd->ptCaretPos = lpcd->ptSelStart;
			}
			else
			{
				/*
				 *	Make the position at which 
				 *	we started the drag selection
				 *	the current start position.
				 */
				lpcd->ptSelStart = lpcd->ptStartPos;

				/*
				 *	Extend the mark to the left
				 *	of the word.
				 */
				ExtendWordLeft( lpcd, &lpcd->ptSelStart );

				/*
				 *	Setup the end position at
				 *	the current mouse position.
				 */
				lpcd->ptSelEnd = ptMousePos;

				/*
				 *	Extend the mark to the end
				 *	of the word.
				 */
				ExtendWordRight( lpcd, &lpcd->ptSelEnd );

				/*
				 *	Move the caret there.
				 */
				lpcd->ptCaretPos = lpcd->ptSelEnd;
			}
		}
		else if ( lpcd->bAllSelect == FALSE )
		{
			/*
			 *	Are we before the point
			 *	at which we initiated
			 *	the drag-selection?
			 */
			if ( ptMousePos.y < lpcd->ptStartPos.y || ptMousePos.y == lpcd->ptStartPos.y && ptMousePos.x < lpcd->ptStartPos.x )
			{
				/*
				 *	Move the start of the
				 *	selection.
				 */
				lpcd->ptSelStart = lpcd->ptCaretPos = ptMousePos;
				lpcd->ptSelEnd   = lpcd->ptStartPos;
			}
			else
			{
				/*
				 *	Move the end of the selection.
				 */
				lpcd->ptSelEnd   = lpcd->ptCaretPos = ptMousePos;
				lpcd->ptSelStart = lpcd->ptStartPos;
			}
		}

		/*
		 *	Update visuals.
		 */
		RenderLines( lpcd, nOldStart, lpcd->ptSelStart.y );
		RenderLines( lpcd, nOldEnd,   lpcd->ptSelEnd.y   );

		/*
		 *	Show the caret.
		 */
		DisplayCaret( lpcd, TRUE );
	}
	return 0;
}

LRESULT OnTimer( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Correct timer ID?
	 */
	if ( wParam == IDT_DRAGTIMER )
	{
		POINT	ptView = lpcd->ptViewPos;
		POINT	ptMouse;
		RECT	rcClient;
		BOOL	bChange = FALSE;
		int	nLines = Brainchild_ArrayGetSize( lpcd->lpLines ) - 1;

		/*
		 *	Get client rectabgle.
		 */
		GetClientRect( hWnd, &rcClient );

		/*
		 *	Get mouse position and convert
		 *	it to client coordinates.
		 */
		GetCursorPos( &ptMouse );
		ScreenToClient( hWnd, &ptMouse );

		/*
		 *	Are we located Outside the client
		 *	area?
		 */
		if (      ptMouse.y < rcClient.top    )	ptView.y--;
		else if ( ptMouse.y > rcClient.bottom ) ptView.y++;

		/*
		 *	Validate.
		 */
		if ( ptView.y < 0 ) ptView.y = 0;
		else if (( ptView.y + lpcd->szViewSize.cy - 1 ) > nLines ) ptView.y = max( 0, nLines - lpcd->szViewSize.cy + 1 );

		/*
		 *	Are we located outside the
		 *	client area?
		 */
		if (      ptMouse.x < ( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ))) ptView.x--;
		else if ( ptMouse.x > rcClient.right )					      ptView.x++;

		/*
		 *	Validate.
		 */
		if ( ptView.x < 0 ) ptView.x = 0;
		else if ( ptView.x > ( lpcd->nLongestLine - lpcd->szViewSize.cx )) ptView.x = max( 0, lpcd->nLongestLine - lpcd->szViewSize.cx );
		
		/*
		 *	Did the column position change?
		 */
		if ( ptView.x != lpcd->ptViewPos.x )
		{
			/*
			 *	Re-render the view.
			 */
			HScroll( lpcd, lpcd->ptViewPos.x - ptView.x );

			/*
			 *	Position change.
			 */
			bChange = TRUE;
		}
		else if ( ptView.y != lpcd->ptViewPos.y )
		{
			/*
			 *	Scroll.
			 */
			VScroll( lpcd, lpcd->ptViewPos.y - ptView.y );

			/*
			 *	Position change.
			 */
			bChange = TRUE;
		}

		/*
		 *	Update visuals.
		 */
		if ( bChange )
		{
			/*
			 *	Store changes.
			 */
			lpcd->ptViewPos = ptView;

			/*
			 *	Setup scrollers.
			 */
			SetupHScroller( lpcd );
			SetupVScroller( lpcd );

			/*
			 *	Simulate a mouse move...
			 */
			OnMouseMove( hWnd, 0, MAKELPARAM( ptMouse.x, ptMouse.y ), lpcd );
		}
	}
	else if ( wParam == IDT_PARSETIMER )
	{
		/*
		 *	Parse another 2000 lines...
		 */
		if ( lpcd->dwParsedUpTo != 0xFFFFFFFF )
			PreParseTo( lpcd,  lpcd->dwParsedUpTo + 2000 );
		else
			ErrorMsg( _T("Internal Error:\nExecuting parsing timer while all lines are parsed.") );
	}

	return 0;
}

LRESULT OnLButtonDblClk( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Mimic a Ctrl+Lbutton.
	 */
	OnLButtonDown( hWnd, MK_CONTROL, lParam, lpcd );

	/*
	 *	Send status message.
	 */
	SendStatusMessage( lpcd );
	return 0;
}

/*
 *	Mouse wheel support.
 */
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

LRESULT OnMouseWheel(  HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Add delta to the previous value.
	 */
	lpcd->nWheelDelta += ( short )HIWORD( wParam );

	/*
	 *	Did it reach at least the WHEEL_DELTA value?
	 */
	if ( abs( lpcd->nWheelDelta ) >= 120 )
	{
		/*
		 *	Determine the amount of lines which where scrolled.
		 */
		int nLines = lpcd->nWheelDelta / WHEEL_DELTA;

		/*
		 *	Get the current scroller positon of the
		 *	vertical scroller.
		 */
		SCROLLINFO si;
		si.cbSize = sizeof( si );
		si.fMask  = SIF_POS;
		if ( GetScrollInfo( hWnd, SB_VERT, &si ))
		{
			/*
			 *	Adjust the scroller position and send
			 *	a vscroll message to the control.
			 */
			si.nPos -= nLines;
			SetScrollInfo( hWnd, SB_VERT, &si, TRUE );
			SendMessage( hWnd, WM_VSCROLL, MAKEWPARAM( SB_THUMBTRACK, si.nPos ), 0 );
		}

		/*
		 *	Reset delta counter.
		 */
		lpcd->nWheelDelta = 0;
	}
	return 0;
}
