/*
 *	marks.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"

LRESULT OnGetSelectionPoints( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Storage pointer passed?
	 */
	if ( lParam )
	{
		/*
		 *	Get points.
		 */
		LPPOINT p1 = ( LPPOINT )lParam, p2 = ( LPPOINT )&(( LPRECT )lParam )->right;

		/*
		 *	Copy selection points.
		 */
		*p1 = lpcd->ptSelStart;
		*p2 = lpcd->ptSelEnd;

		/*
		 *	Increase positions by one
		 *	if there is a valid marker.
		 */
		if ( HasMark( lpcd ))
		{
			p1->x++;
			p1->y++;
			p2->x++;
			p2->y++;
			return TRUE;
		}
	}
	return FALSE;
}

LRESULT OnSetSelectionPoints( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Get the rectangle.
	 */
	LPCRECT rc  = ( LPCRECT )lParam;
	if ( rc )
	{
		/*
		 *	Convert the rectangle into
		 *	points.
		 */
		LPPOINT pt1 = ( LPPOINT )rc, pt2 = ( LPPOINT )&rc->right;

		/*
		 *	Are they valid?
		 */
		if ( CheckPoint( lpcd, pt1 ) && CheckPoint( lpcd, pt2 ))
		{
			/*
			 *	Hide the caret.
			 */
			DisplayCaret( lpcd, FALSE );

			/*
			 *	Clear current marker.
			 */
			if ( HasMark( lpcd ))
				ClearMark( lpcd );

			/*
			 *	Setup marker.
			 */
			lpcd->ptSelStart = *pt1;
			lpcd->ptSelEnd   = *pt2;
			
			/*
			 *	Position the caret.
			 */
			lpcd->ptCaretPos = *pt2;
			
			/*
			 *	Re-render the lines.
			 */
			if ( CaretInView( lpcd )) RenderLines( lpcd, pt1->y, pt2->y );
			else 			  MakeCaretVisible( lpcd );

			/*
			 *	Show the caret.
			 */
			DisplayCaret( lpcd, TRUE );

			/*
			 *	Send the status message.
			 */
			SendStatusMessage( lpcd );
			return TRUE;
		}
	}
	return FALSE;
}

/*
 *	Check if the point contains coordinates
 *	which are in range of the control it's
 *	text.
 */
BOOL CheckPoint( LPCLASSDATA lpcd, LPPOINT pt )
{
	LPLINE	lpLine;

	/*
	 *	Valid point?
	 */
	if ( pt )
	{
		/*
		 *	Decrease position by one.
		 */
		pt->x--;
		pt->y--;

		/*
		 *	Line in range?
		 */
		if ( pt->y >= 0 && pt->y < Brainchild_ArrayGetSize( lpcd->lpLines ))
		{
			/*
			 *	Get the line.
			 */
			lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, pt->y );

			/*
			 *	Is the passed column in the line?
			 */
			if ( pt->x <= CaretOffsetLine( lpcd, pt->y, lpLine->nLength ))
			{
				/*
				 *	Convert the column to the real
				 *	text offset.
				 */
				pt->x = TextOffsetLine( lpcd, pt->y, pt->x, NULL );

				/*
				 *	Point is in range.
				 */
				return TRUE;
			}
		}
	}
	/*
	 *	Point not in range.
	 */
	return FALSE;
}

static BOOL MarksDiffer( LPCLASSDATA lpcd )
{
	return ( BOOL )( lpcd->ptSelStart.y != lpcd->ptSelEnd.y || lpcd->ptSelStart.x != lpcd->ptSelEnd.x );
}

BOOL HasMark( LPCLASSDATA lpcd )
{
	/*
	 *	Valid marker?
	 */
	if ( lpcd->ptSelStart.x != -1 )
	{
		/*
		 *	Markers differ?
		 */
		if ( ! MarksDiffer( lpcd ))
		{
			/*
			 *	No. Invalidate.
			 */
			lpcd->ptSelStart.x = lpcd->ptSelStart.y = -1;
			lpcd->ptSelEnd.x   = lpcd->ptSelEnd.y   = -1;
		}
	}
	return ( BOOL )( lpcd->ptSelStart.x != -1 );
}

void ClearMark( LPCLASSDATA lpcd )
{
	/*
	 *	Is it already cleared?
	 */
	if ( lpcd->ptSelStart.x == -1 )
		return;

	/*
	 *	Clear markers.
	 */
	lpcd->ptSelStart.x = lpcd->ptSelStart.y = -1;
	lpcd->ptSelEnd.x   = lpcd->ptSelEnd.y   = -1;
	lpcd->ptStartPos.x = lpcd->ptStartPos.y = -1;

	/*
	 *	Re-render.
	 */
	InvalidateRect( lpcd->hWnd, NULL, FALSE );
	UpdateWindow( lpcd->hWnd );

	/*
	 *	Send status message.
	 */
	SendStatusMessage( lpcd );
}

void MarkAll( LPCLASSDATA lpcd )
{
	/*
	 *	Any text to select?
	 */
	if ( AnyText( lpcd ))
	{
		/*
		 *	Mark it all.
		 */
		lpcd->ptSelStart.x = lpcd->ptSelStart.y = 0;
		lpcd->ptSelEnd.y   = Brainchild_ArrayGetSize( lpcd->lpLines ) - 1;
		lpcd->ptSelEnd.x   = (( LPLINE )Brainchild_ArrayGetAt( lpcd->lpLines, lpcd->ptSelEnd.y ))->nLength;

		/*
		 *	Re-render.
		 */
		InvalidateRect( lpcd->hWnd, NULL, FALSE );

		/*
		 *	Send status message.
		 */
		SendStatusMessage( lpcd );
	}
}

void MarkWord( LPCLASSDATA lpcd )
{
	/*
	 *	Get the word points.
	 */
	if ( GetWordPoints( lpcd, &lpcd->ptSelStart, &lpcd->ptSelEnd ))
	{
		/*
		 *	Hide the caret.
		 */
		DisplayCaret( lpcd, FALSE );

		/*
		 *	Move the caret.
		 */
		lpcd->ptCaretPos = lpcd->ptSelEnd;

	        /*
		 *	Save off this position.
		 */
		lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

		/*
		 *	Caret still visible?
		 */
		if ( CaretInView( lpcd ))
			/*
			 *	Simply re-render the line.
			 */
			RenderLine( lpcd, lpcd->ptCaretPos.y );
		else
			/*
			 *	Move the view.
			 */
			MakeCaretVisible( lpcd );
		
		/*
		 *	Show the caret.
		 */
		DisplayCaret( lpcd, TRUE );

		/*
		 *	Send status message.
		 */
		SendStatusMessage( lpcd );
	}
	else
		/*
		 *	Start and end at the same position.
		 */
		lpcd->ptSelStart = lpcd->ptSelEnd = lpcd->ptCaretPos;
}

void MarkLine( LPCLASSDATA lpcd )
{
	MarkWholeLine( lpcd, FALSE );
}
BOOL MarkWholeLine( LPCLASSDATA lpcd, BOOL bQuiet )
{
	LPLINE		lpLine = GETLINE( lpcd );
	int		nLine = lpcd->ptCaretPos.y;

	/*
	 *	Clear current marker.
	 */
	if ( ! bQuiet )
		ClearMark( lpcd );

	/*
	 *	Any text on the current line?
	 */
	if ( lpLine->pcText == NULL )
		return FALSE;

	/*
	 *	Hide the caret.
	 */
	if ( ! bQuiet )
		DisplayCaret( lpcd, FALSE );

	/*
	 *	Set the start and the end of the
	 *	selection.
	 */
	lpcd->ptSelStart.x = 0;
	lpcd->ptSelStart.y = lpcd->ptCaretPos.y;

	/*
	 *	If we are on the last line we
	 *	set the end on the end of the
	 *	line, otherwise we set it at
	 *	the start of the next line.
	 */
	if ( lpcd->ptCaretPos.y == Brainchild_ArrayGetSize( lpcd->lpLines ) - 1 ) 
	{
		lpcd->ptSelEnd.x = lpLine->nLength;
		lpcd->ptSelEnd.y = lpcd->ptCaretPos.y;
	}
	else
	{
		lpcd->ptSelEnd.x = 0;
		lpcd->ptSelEnd.y = lpcd->ptCaretPos.y + 1;
	}

	/*
	 *	Move the caret to the end of the
	 *	selected line.
	 */
	lpcd->ptCaretPos = lpcd->ptSelEnd;

	if ( ! bQuiet )
	{
		/*
		*	Save off this position.
		*/
		lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

		/*
		*	Caret still visible?
		*/
		if ( CaretInView( lpcd ))
			/*
			*	Simply re-render the line.
			*/
			RenderLine( lpcd, nLine );
		else
			/*
			*	Move the view.
			*/
			MakeCaretVisible( lpcd );
			
		/*
		*	Show the caret.
		*/
		DisplayCaret( lpcd, TRUE );

		/*
		*	Send status message.
		*/
		SendStatusMessage( lpcd );
	}
	return TRUE;
}

void UpperCaseSelection( LPCLASSDATA lpcd )
{
	/*
	 *	Valid selection?
	 */
	if ( HasMark( lpcd ))
	{
		POINT	ptStart = lpcd->ptSelStart, ptEnd = lpcd->ptSelEnd;

		/*
		 *	Clear markers.
		 */
		ClearMark( lpcd );

		/*
		 *	Convert selection to uppercase.
		 */
		ConvertCase( lpcd, &ptStart, &ptEnd, NTYPE_TOUPPER );
	}
}

void LowerCaseSelection( LPCLASSDATA lpcd )
{
	/*
	 *	Valid selection?
	 */
	if ( HasMark( lpcd ))
	{
		POINT	ptStart = lpcd->ptSelStart, ptEnd = lpcd->ptSelEnd;

		/*
		 *	Clear markers.
		 */
		ClearMark( lpcd );

		/*
		 *	Convert selection to lowercase.
		 */
		ConvertCase( lpcd, &ptStart, &ptEnd, NTYPE_TOLOWER );
	}
}

void SwapCaseSelection( LPCLASSDATA lpcd )
{
	/*
	 *	Valid selection?
	 */
	if ( HasMark( lpcd ))
	{
		POINT	ptStart = lpcd->ptSelStart, ptEnd = lpcd->ptSelEnd;

		/*
		 *	Clear markers.
		 */
		ClearMark( lpcd );

		/*
		 *	Swap selection casing.
		 */
		ConvertCase( lpcd, &ptStart, &ptEnd, NTYPE_SWAP );
	}
}
