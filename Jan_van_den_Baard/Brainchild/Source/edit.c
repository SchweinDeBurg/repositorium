/*
 *	edit.c
 *
 *	(C) Copyright 1993-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Text editing routines.
 */

#include "stdafx.h"

#include "defs.h"
#include "LangRes.rh"

/*
 *	Remove blanks between the cursor
 *	position and the next non-blank.
 */
void Pull( LPCLASSDATA lpcd )
{
	LPLINE	lpLine = GETLINE( lpcd );
	POINT	ptStart, ptEnd;

	/*
	 *	Read-only.
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Clear markers.
	 */
	ClearMark( lpcd );

	/*
	 *	Are we on the end of the line?
	 */
	if ( lpcd->ptCaretPos.x == lpLine->nLength )
		return;

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Setup the points.
	 */
	ptStart = ptEnd = lpcd->ptCaretPos;

	/*
	 *	Find the first non-blank.
	 */
	while ( _istspace( lpLine->pcText[ ptEnd.x ] ))
	{
		/*
		 *	The end?
		 */
		if ( ptEnd.x == lpLine->nLength )
			break;

		/*
		 *	Next...
		 */
		ptEnd.x++;
	}

	/*
	 *	Delete the characters.
	 */
	DeleteText( lpcd, &ptStart, &ptEnd, TRUE );

	/*
	 *	Move caret.
	 */
	lpcd->ptCaretPos = ptStart;

	/*
	 *	Caret still inside the view?
	 */
	if ( CaretInView( lpcd ))
	{
		/*
		 *	Simply re-render the line.
		 */
		RenderLine( lpcd, lpcd->ptCaretPos.y );
	}
	else
		/*
		 *	Move the view.
		 */
		MakeCaretVisible( lpcd );

	/*
	 *	We are modified.
	 */
	SetModified( lpcd, TRUE );

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );
}

/*
 *	Delete to the end of the line.
 */
void DeleteEol( LPCLASSDATA lpcd )
{
	LPLINE	lpLine = GETLINE( lpcd );
	POINT	ptStart, ptEnd;

	/*
	 *	Read-only.
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Clear markers.
	 */
	ClearMark( lpcd );

	/*
	 *	Are we alreay on the end of the line?
	 */
	if ( lpcd->ptCaretPos.x == lpLine->nLength )
		return;

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Setup the points.
	 */
	ptStart = ptEnd = lpcd->ptCaretPos;
	ptEnd.x = lpLine->nLength;

	/*
	 *	Delete the characters.
	 */
	DeleteText( lpcd, &ptStart, &ptEnd, TRUE );

	/*
	 *	Move caret.
	 */
	lpcd->ptCaretPos = ptStart;

	/*
	 *	Caret still inside the view?
	 */
	if ( CaretInView( lpcd ))
	{
		/*
		 *	Simply re-render the line.
		 */
		RenderLine( lpcd, lpcd->ptCaretPos.y );
	}
	else
		/*
		 *	Move the view.
		 */
		MakeCaretVisible( lpcd );

	/*
	 *	We are modified.
	 */
	SetModified( lpcd, TRUE );

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );
}

/*
 *	Back space a character.
 */
void BackSpace( LPCLASSDATA lpcd )
{
	POINT	ptStart, ptEnd;
	BOOL	bChangeLines = FALSE;

	/*
	 *	Read-only.
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Delete only the text selection
	 *	if the markers are valid.
	 */
	if ( HasMark( lpcd ))
	{
		Delete( lpcd );
		return;
	}

	/*
	 *	If we are at the start of the text
	 *	we do nothing.
	 */
	if ( lpcd->ptCaretPos.y == 0 && lpcd->ptCaretPos.x == 0 )
		return;

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Setup the points.
	 */
	ptStart = ptEnd = lpcd->ptCaretPos;

	/*
	 *	Are we on the start of the line?
	 */
	if ( lpcd->ptCaretPos.x == 0 )
	{
		/*
		 *	Join the previous and this line.
		 */
		ptStart.y--;
		ptStart.x = (( LPLINE )ArrayGetAt( lpcd->lpLines, ptStart.y ))->nLength;

		/*
		 *	Re-render whole view.
		 */
		bChangeLines = TRUE;
	}
	else
		/*
		 *	One character.
		 */
		ptStart.x--;

	/*
	 *	Delete the character.
	 */
	DeleteText( lpcd, &ptStart, &ptEnd, TRUE );

	/*
	 *	Move caret.
	 */
	lpcd->ptCaretPos = ptStart;

	/*
	 *	Update column position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Caret still inside the view?
	 */
	if ( ! CaretInView( lpcd ))
	{
		/*
		 *	Full redraw...
		 */
		bChangeLines = TRUE;
		MakeCaretVisibleNoRedraw( lpcd );
	}

	/*
	 *	Any changes in the line count?
	 */
	if ( bChangeLines )
	{
		/*
		 *	Re-render and setup the scrollers.
		 */
		InvalidateRect( lpcd->hWnd, NULL, TRUE );
		SetupHScroller( lpcd );
		SetupVScroller( lpcd );
	}
	else
		/*
		 *	Simply re-render the line.
		 */
		RenderLine( lpcd, lpcd->ptCaretPos.y );

	/*
	 *	We are modified.
	 */
	SetModified( lpcd, TRUE );

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );
}

/*
 *	Delete a character from the text.
 */
void DeleteChar( LPCLASSDATA lpcd )
{
	LPLINE	lpLine = GETLINE( lpcd );
	POINT	ptStart, ptEnd;
	BOOL	bChangeLines = FALSE;

	/*
	 *	Read-only.
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Delete only the text selection
	 *	if the markers are valid.
	 */
	if ( HasMark( lpcd ))
	{
		Delete( lpcd );
		return;
	}

	/*
	 *	If we are at the end of the text
	 *	we do nothing.
	 */
	if ( lpcd->ptCaretPos.y == ArrayGetSize( lpcd->lpLines ) - 1 && lpcd->ptCaretPos.x == lpLine->nLength )
		return;

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Setup the points.
	 */
	ptStart = ptEnd = lpcd->ptCaretPos;

	/*
	 *	Are we on the end of the line?
	 */
	if ( lpcd->ptCaretPos.x == lpLine->nLength )
	{
		/*
		 *	Join this and the next lines.
		 */
		ptEnd.x = 0;
		ptEnd.y++;

		/*
		 *	Re-render whole view.
		 */
		bChangeLines = TRUE;
	}
	else
		/*
		 *	One character.
		 */
		ptEnd.x++;

	/*
	 *	Delete the character.
	 */
	DeleteText( lpcd, &ptStart, &ptEnd, TRUE );

	/*
	 *	Any changes in the line count?
	 */
	if ( bChangeLines )
	{
		/*
		 *	Re-render and setup the scrollers.
		 */
		InvalidateRect( lpcd->hWnd, NULL, TRUE );
		SetupHScroller( lpcd );
		SetupVScroller( lpcd );
	}
	else
		RenderLine( lpcd, lpcd->ptCaretPos.y );

	/*
	 *	We are modified.
	 */
	SetModified( lpcd, TRUE );

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );
}

/*
 *	Delete current line.
 */
void DeleteLine( LPCLASSDATA lpcd )
{
	LPLINE	lpLine = GETLINE( lpcd );
	POINT	ptStart, ptEnd;

	/*
	 *	Read-only.
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Clear markers.
	 */
	ClearMark( lpcd );

	/*
	 *	Any text to delete?
	 */
	if ( ArrayGetSize( lpcd->lpLines ) == 1 && lpLine->nLength == 0 )
		return;

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Setup the points.
	 */
	ptStart = ptEnd = lpcd->ptCaretPos;

	/*
	 *	Are we on the last line?
	 */
	if ( lpcd->ptCaretPos.y == ArrayGetSize( lpcd->lpLines ) - 1 )
	{
		/*
		 *	If this is no the first
		 *	line we go one up.
		 */
		if ( lpcd->ptCaretPos.y > 0 )
		{
			/*
			 *	Go to the end.
			 */
			ptStart.y--;
			ptStart.x = (( LPLINE )ArrayGetAt( lpcd->lpLines, ptStart.y ))->nLength;
		}
		else
			/*
			 *	Move to the start of the line.
			 */
			ptStart.x = 0;

		/*
		 *	End at the end of the current line.
		 */
		ptEnd.x = lpLine->nLength;
	}
	else
	{
		/*
		 *	Move to the start of the next line.
		 */
		ptEnd.y++;
		ptEnd.x = 0;

		/*
		 *	And the start of the current.
		 */
		ptStart.x = 0;
	}

	/*
	 *	Delete the characters.
	 */
	DeleteText( lpcd, &ptStart, &ptEnd, TRUE );

	/*
	 *	Set caret at the start position.
	 */
	lpcd->ptCaretPos = ptStart;

	/*
	 *	Update column position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Caret still inside the view?
	 */
	if ( ! CaretInView( lpcd ))
		MakeCaretVisibleNoRedraw( lpcd );

	/*
	 *	Re-render and setup the scrollers.
	 */
	InvalidateRect( lpcd->hWnd, NULL, TRUE );
	SetupHScroller( lpcd );
	SetupVScroller( lpcd );

	/*
	 *	We are modified.
	 */
	SetModified( lpcd, TRUE );

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );
}

/*
 *      Automatic bracket matching.
 */
static void AutoMatch( LPCLASSDATA lpcd, TCHAR cChar )
{
	/*
	 *	Is it a closing bracket?
	 */
	if ( cChar == _T( ')' ) || cChar == _T( ']' ) || cChar == _T( '}' ) || cChar == _T( '>' ) || cChar == _T( '\'' ) || cChar == _T( '»' ))
	{
		/*
		 *	Save caret position.
		 */
		POINT	ptCaret = lpcd->ptCaretPos;

		/*
		 *	Go back one.
		 */
		lpcd->ptCaretPos.x--;

		/*
		 *	No dialogs, no visual
		 *	updates...
		 */
		lpcd->bSilentMode = TRUE;

		/*
		 *      Match it.
		 */
		if ( ! MatchBracket( lpcd ))
		{
			/*
			 *	Visible alarm?
			 */
			if ( Parser->bVisibleMatchError )
				/*
				 *	Display error dialog.
				 */
				OkError( lpcd, lpcd->hWnd, GetString( IDS_AUTOMATCH_FAULT ));
			else
				/*
				 *	Sound the alarm...
				 */
				MessageBeep( 0xFFFFFFFF );
		}

		/*
		 *	Silent mode off.
		 */
		lpcd->bSilentMode = FALSE;

		/*
		 *	Restore caret position.
		 */
		lpcd->ptCaretPos = ptCaret;
        }
}

/*
 *	Insert a character into the text.
 */
void InsertChar( LPCLASSDATA lpcd, TCHAR cChar )
{
	TCHAR	szBuf[ 2 ] = { 0 };
	BOOL	bDeleted = FALSE;

	/*
	 *	Read-only.
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Delete selection
	 */
	if ( HasMark( lpcd ))
		bDeleted = Delete( lpcd );

	/*
	 *	Construct text to insert.
	 */
	szBuf[ 0 ] = cChar;

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Overwrite?
	 */
	if ( lpcd->bOverwrite )
	{
		LPLINE		lpLine = GETLINE( lpcd );

		/*
		 *	Are we at the end of the line?
		 */
		if ( lpcd->ptCaretPos.x < lpLine->nLength )
		{
			/*
			 *	No. Delete the next character.
			 */
			POINT		ptStart, ptEnd;

			/*
			 *	Setup points.
			 */
			ptStart = ptEnd = lpcd->ptCaretPos;
			ptEnd.x++;

			/*
			 *	Delete the text.
			 */
			if ( DeleteText( lpcd, &ptStart, &ptEnd, ! bDeleted ) == FALSE )
				return;

			/*
			 *	Make sure the undo group is continued.
			 */
			bDeleted = TRUE;
		}
	}

	/*
	 *	Insert the text.
	 */
	InsertText( lpcd, lpcd->ptCaretPos.y, lpcd->ptCaretPos.x, szBuf, &lpcd->ptCaretPos, ! bDeleted );

	/*
	 *	Update column position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Caret still inside the view?
	 */
	if ( CaretInView( lpcd ))
		/*
		 *	Yes, simply re-render the line.
		 */
		RenderLine( lpcd, lpcd->ptCaretPos.y );
	else
		/*
		 *	Move the view.
		 */
		MakeCaretVisible( lpcd );

	/*
	 *	We are modified.
	 */
	SetModified( lpcd, TRUE );

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );

	/*
	 *	Auto-match?
	 */
	if ( Parser->bAutoMatch )
		AutoMatch( lpcd, cChar );
}

void InsertCharacters( LPCLASSDATA lpcd, LPCTSTR lpszText )
{
	/*
	 *	Read only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Clear markers.
	 */
	ClearMark( lpcd );

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Insert the text.
	 */
	InsertText( lpcd, lpcd->ptCaretPos.y, lpcd->ptCaretPos.x, lpszText, &lpcd->ptCaretPos, TRUE );

	/*
	 *	Update column position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Caret in the view?
	 */
	if ( ! CaretInView( lpcd ))
		MakeCaretVisibleNoRedraw( lpcd );

	/*
	 *	Re-render and setup the scrollers.
	 */
	InvalidateRect( lpcd->hWnd, NULL, TRUE );
	SetupHScroller( lpcd );
	SetupVScroller( lpcd );

	/*
	 *	We are modified.
	 */
	SetModified( lpcd, TRUE );

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );
}

/*
 *	Check if the line we have just
 *	split has only blanks left on
 *	it.
 */
static void CheckPreviousLine( LPCLASSDATA lpcd )
{
	/*
	 *	Is there a previous line?
	 */
	if ( lpcd->ptCaretPos.y > 0 )
	{
		/*
		 *	Get a pointer to it.
		 */
		LPLINE	lpPrev = ArrayGetAt( lpcd->lpLines, lpcd->ptCaretPos.y - 1 );
		int	nBlanks = 0;

		/*
		 *	Valid text?
		 */
		if ( lpPrev->pcText )
		{
			/*
			 *	Count spaces.
			 */
			while ( _istspace( lpPrev->pcText[ nBlanks ] ) && nBlanks < lpPrev->nLength ) nBlanks++;

			/*
			 *	Does the whole line consist
			 *	out of blanks?
			 */
			if ( nBlanks == lpPrev->nLength )
				/*
				 *	Yes. Clear it.
				 */
				 lpPrev->nLength = 0;
		}
	}
}

/*
 *	Split up the current line.
 */
void SplitLine( LPCLASSDATA lpcd )
{
	TCHAR	*szText = _T("\r\n"), *pcBlanks = NULL;
	int	nPosX = 0;
	BOOL	bDeleted = FALSE;

	/*
	 *	Read-only.
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Delete selection
	 */
	if ( HasMark( lpcd ))
		bDeleted = Delete( lpcd );

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Overwrite mode?
	 */
	if ( lpcd->bOverwrite )
	{
		/*
		 *	Are we before the last line?
		 */
		if ( lpcd->ptCaretPos.y < ArrayGetSize( lpcd->lpLines ) - 1 )
		{
			/*
			 *	One down.
			 */
			lpcd->ptCaretPos.y++;
			lpcd->ptCaretPos.x = 0;

			/*
			 *	Make the caret visible.
			 */
			MakeCaretVisible( lpcd );

			/*
			 *	Show the caret.
			 */
			DisplayCaret( lpcd, TRUE );
			return;
		}
		else
			/*
			 *	Go to the end of the last line.
			 */
			lpcd->ptCaretPos.x = (( LPLINE )GETLINE( lpcd ))->nLength;
	}

	/*
	 *	Auto indent?
	 */
	if ( Parser->bAutoIndent )
		/*
		 *	Get leading blanks.
		 */
		pcBlanks = CopyLeadingBlanks( lpcd, &nPosX );

	/*
	 *	Insert a line-break.
	 */
	if ( InsertText( lpcd, lpcd->ptCaretPos.y, lpcd->ptCaretPos.x, szText, &lpcd->ptCaretPos, ! bDeleted ) == FALSE )
		return;

	/*
	 *	Indent?
	 */
	if ( pcBlanks )
	{
		/*
		 *	Insert blanks.
		 */
		InsertText( lpcd, lpcd->ptCaretPos.y, lpcd->ptCaretPos.x, pcBlanks, &lpcd->ptCaretPos, FALSE );

		/*
		 *	Put the caret at the
		 *	correct position.
		 */
		lpcd->ptCaretPos.x = nPosX;

		/*
		 *	Check if the line we split has
		 *	only blanks left.
		 */
		CheckPreviousLine( lpcd );

		/*
		 *	Free the string.
		 */
		FreePooled( lpcd->pMemPool, pcBlanks );
	}

	/*
	 *	Update column position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Caret in the view?
	 */
	if ( ! CaretInView( lpcd ))
		MakeCaretVisibleNoRedraw( lpcd );

	/*
	 *	Re-render and setup the scrollers.
	 */
	InvalidateRect( lpcd->hWnd, NULL, TRUE );
	SetupHScroller( lpcd );
	SetupVScroller( lpcd );

	/*
	 *	We are modified.
	 */
	SetModified( lpcd, TRUE );

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );
}

BOOL GetWordPoints( LPCLASSDATA lpcd, LPPOINT lpStart, LPPOINT lpEnd )
{
	LPLINE	lpLine = GETLINE( lpcd );
	int	nIndex = lpcd->ptCaretPos.x;

	/*
	 *	Any text?
	 */
	if ( AnyText( lpcd ) == FALSE || lpLine->pcText == NULL )
		return FALSE;

	/*
	 *	Are we on a word?
	 */
	if ( _istspace( lpLine->pcText[ nIndex ] ))
		return FALSE;

	/*
	 *	Setup line.
	 */
	lpStart->y = lpEnd->y = lpcd->ptCaretPos.y;

	/*
	 *	Are we on a delimiter?
	 */
	if ( IsDelimiter( lpcd, lpLine->pcText[ nIndex ] ))
	{
		/*
		 *	The delimiter is the whole word.
		 */
		lpStart->x = nIndex;
		lpEnd->x   = nIndex + 1;

		return TRUE;
	}

	/*
	 *	Find the start of the word.
	 */
	while ( nIndex > 0 && ! _istspace( lpLine->pcText[ nIndex ] ) && ! IsDelimiter( lpcd, lpLine->pcText[ nIndex ] ))
		nIndex--;

	/*
	 *	Did we stop on a space or a delimiter?
	 */
	if ( _istspace( lpLine->pcText[ nIndex ] ) || IsDelimiter( lpcd, lpLine->pcText[ nIndex ] ))
	{
		if ( nIndex < lpLine->nLength )
			nIndex++;
	}

	/*
	 *	Start column.
	 */
	lpStart->x = nIndex;

	/*
	 *	Find the end of the word.
	 */
	while ( nIndex < lpLine->nLength && ! _istspace( lpLine->pcText[ nIndex ] ) && ! IsDelimiter( lpcd, lpLine->pcText[ nIndex ] ))
		nIndex++;

	/*
	 *	End column.
	 */
	lpEnd->x = nIndex;

	return TRUE;
}

/*
 *	Convert casing between two
 *	points.
 */
void ConvertCase( LPCLASSDATA lpcd, LPPOINT lpStart, LPPOINT lpEnd, int nType )
{
	TCHAR  *pcText;
	int	i = 0;

	/*
	 *	Read-only.
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Get text to delete.
	 */
	pcText = GetText( lpcd, lpStart, lpEnd );

	/*
	 *	Text retrieved?
	 */
	if ( pcText == NULL )
		return;

	/*
	 *	Delete the text.
	 */
	if ( DeleteText( lpcd, lpStart, lpEnd, TRUE ) == FALSE )
	{
		/*
		 *	Free the text.
		 */
		ArrayFreeMem( lpcd->lpUndoRedo, pcText );
		return;
	}

	/*
	 *	Convert the text.
	 */
	for ( i = 0; ; i++ )
	{
		/*
		 *	Stop at the terminator.
		 */
		if ( pcText[ i ] == _T( '\0' ))
			break;

		/*
		 *	Convert how?
		 */
		switch ( nType )
		{
			case	NTYPE_TOUPPER:
				/*
				 *	Convert to upper case.
				 */
				pcText[ i ] = ( TCHAR )_totupper( pcText[ i ] );
				break;

			case	NTYPE_TOLOWER:
				/*
				 *	Convert to lower case.
				 */
				pcText[ i ] = ( TCHAR )_totlower( pcText[ i ] );
				break;

			case	NTYPE_SWAP:
				/*
				 *	Swap case.
				 */
				if (	  _istlower( pcText[ i ] )) pcText[ i ] = ( TCHAR )_totupper( pcText[ i ] );
				else if ( _istupper( pcText[ i ] )) pcText[ i ] = ( TCHAR )_totlower( pcText[ i ] );
		}
	}

	/*
	 *	Insert the converted text.
	 */
	InsertText( lpcd, lpStart->y, lpStart->x, pcText, &lpcd->ptCaretPos, FALSE );

	/*
	 *	Free the text.
	 */
	ArrayFreeMem( lpcd->lpUndoRedo, pcText );

	/*
	 *	Update column position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Caret still inside the view?
	 */
	if ( CaretInView( lpcd ) == FALSE )
		/*
		 *	Move the view.
		 */
		MakeCaretVisibleNoRedraw( lpcd );

	/*
	 *	Re-render.
	 */
	InvalidateRect( lpcd->hWnd, NULL, FALSE );

	/*
	 *	We are modified.
	 */
	SetModified( lpcd, TRUE );

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );
}

static void WordToXXX( LPCLASSDATA lpcd, int nType )
{
	POINT	ptStart, ptEnd;

	/*
	 *	Clear marker.
	 */
	ClearMark( lpcd );

	/*
	 *	Get word points.
	 */
	if ( GetWordPoints( lpcd, &ptStart, &ptEnd ))
		/*
		 *	Convert word casing.
		 */
		ConvertCase( lpcd, &ptStart, &ptEnd, nType );
}

void WordToUpper( LPCLASSDATA lpcd )
{
	WordToXXX( lpcd, NTYPE_TOUPPER );
}

void WordToLower( LPCLASSDATA lpcd )
{
	WordToXXX( lpcd, NTYPE_TOLOWER );
}

void WordSwapCase( LPCLASSDATA lpcd )
{
	WordToXXX( lpcd, NTYPE_SWAP );
}

void CopyLine( LPCLASSDATA lpcd )
{
	POINT	ptStart = lpcd->ptCaretPos, ptEnd;
	LPLINE	lpLine = GETLINE( lpcd );
	LPTSTR	lpszText;
	int	nLength = lpLine->nLength + 3;

	/*
	 *	Read only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Clear markers.
	 */
	ClearMark( lpcd );

	/*
	 *	Allocate text buffer.
	 */
	if (( lpszText = AllocPooled( lpcd->pMemPool, REAL_SIZE( nLength ))) != NULL )
	{
		/*
		 *	Hide the caret.
		 */
		DisplayCaret( lpcd, FALSE );

		/*
		 *	Copy the text.
		 */
		if ( lpLine->pcText )
			_tcsncpy( lpszText, lpLine->pcText, lpLine->nLength );

		/*
		 *	Append line terminator.
		 */
		_tcscat( lpszText, _T("\r\n"));

		/*
		 *	Insert at the start of the line.
		 */
		ptStart.x = 0;

		/*
		 *	Insert the text.
		 */
		InsertText( lpcd, ptStart.y, ptStart.x, lpszText, &ptEnd, TRUE );

		/*
		 *	Copy position.
		 */
		lpcd->ptCaretPos = ptEnd;

		/*
		 *	Free the text.
		 */
		FreePooled( lpcd->pMemPool, lpszText );

		/*
		 *	Caret in the view?
		 */
		if ( ! CaretInView( lpcd ))
			MakeCaretVisibleNoRedraw( lpcd );

		/*
		 *	Re-render and setup the scrollers.
		 */
		InvalidateRect( lpcd->hWnd, NULL, TRUE );
		SetupHScroller( lpcd );
		SetupVScroller( lpcd );

		/*
		 *	We are modified.
		 */
		SetModified( lpcd, TRUE );

		/*
		 *	Show the caret.
		 */
		DisplayCaret( lpcd, TRUE );
	}
}

void DeleteWord( LPCLASSDATA lpcd )
{
	POINT	ptStart, ptEnd;

	/*
	 *	Read-only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Clear markers.
	 */
	if ( HasMark( lpcd ))
		ClearMark( lpcd );

	/*
	 *	Get the word points.
	 */
	if ( GetWordPoints( lpcd, &ptStart, &ptEnd ))
	{
		/*
		 *	Set these points up as a
		 *	text selection.
		 */
		lpcd->ptSelStart = ptStart;
		lpcd->ptSelEnd   = ptEnd;

		/*
		 *	Delete it.
		 */
		Delete( lpcd );
	}
}

void DeleteEow( LPCLASSDATA lpcd )
{
	POINT	ptStart, ptEnd;

	/*
	 *	Read-only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Clear markers.
	 */
	if ( HasMark( lpcd ))
		ClearMark( lpcd );

	/*
	 *	Get the word points.
	 */
	if ( GetWordPoints( lpcd, &ptStart, &ptEnd ))
	{
		/*
		 *	Set the caret position and
		 *	the end of the word as the
		 *	text selection.
		 */
		lpcd->ptSelStart = lpcd->ptCaretPos;
		lpcd->ptSelEnd   = ptEnd;

		/*
		 *	Delete it.
		 */
		Delete( lpcd );
	}
}

void DeleteSow( LPCLASSDATA lpcd )
{
	POINT	ptStart, ptEnd;

	/*
	 *	Read-only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Clear markers.
	 */
	if ( HasMark( lpcd ))
		ClearMark( lpcd );

	/*
	 *	Get the word points.
	 */
	if ( GetWordPoints( lpcd, &ptStart, &ptEnd ))
	{
		/*
		 *	Set the caret position and
		 *	the start of the word as the
		 *	text selection.
		 */
		lpcd->ptSelStart = ptStart;
		lpcd->ptSelEnd   = lpcd->ptCaretPos;

		/*
		 *	Delete it.
		 */
		Delete( lpcd );
	}
}

void SwapLines( LPCLASSDATA lpcd )
{
	LPLINE		lpLine = GETLINE( lpcd );
	LPTSTR		pszLine;
	POINT		ptStart, ptEnd;

	/*
	 *	Are we read-only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Are we on the first line?
	 */
	if ( lpcd->ptCaretPos.y == 0 )
	{
		/*
		 *	Beep the speaker.
		 */
		MessageBeep( 0xFFFFFFFF );
		return;
	}

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Get the start of the line.
	 */
	ptStart.x = 0;
	ptStart.y = lpcd->ptCaretPos.y;

	/*
	 *	If we are on the last line we
	 *	set the end on the end of the
	 *	line, otherwise we set it at
	 *	the start of the next line.
	 */
	if ( lpcd->ptCaretPos.y == ArrayGetSize( lpcd->lpLines ) - 1 )
	{
		ptEnd.x = lpLine->nLength;
		ptEnd.y = lpcd->ptCaretPos.y;
	}
	else
	{
		ptEnd.x = 0;
		ptEnd.y = lpcd->ptCaretPos.y + 1;
	}

	/*
	 *	Get the line.
	 */
	if (( pszLine = GetText( lpcd, &ptStart, &ptEnd )) != NULL )
	{
		/*
		 *	Delete the line.
		 */
		if ( DeleteText( lpcd, &ptStart, &ptEnd, TRUE ))
			/*
			 *	Insert the text one line up.
			 */
			 InsertText( lpcd, ptStart.y - 1, 0, pszLine, &lpcd->ptCaretPos, FALSE );

		/*
		 *	Free the text.
		 */
		ArrayFreeMem( lpcd->lpUndoRedo, pszLine );
	}

	/*
	 *	Update column position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Re-render lines.
	 */
	RenderLines( lpcd, ptStart.y - 1, ptStart.y );

	/*
	 *	Were modifieed.
	 */
	SetModified( lpcd, TRUE );

	/*
	 *	Display the caret.
	 */
	DisplayCaret( lpcd, TRUE );
}
