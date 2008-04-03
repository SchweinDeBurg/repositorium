/*
 *	clip.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"

LRESULT OnReplaceSelection( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	LPCTSTR		lpszText = ( LPCTSTR )lParam;
	BOOL		bDeleted = FALSE;

	/*
	 *	Are we read-only?
	 */
	if ( ISREADONLY )
		return FALSE;

	/*
	 *	Any text?
	 */
	if ( lpszText == NULL || *lpszText == _T( '\0' ))
		return FALSE;

	/*
	 *	Any marks set? If not return FALSE since we need
	 *	to replace a text selection.
	 */
	if ( HasMark( lpcd ) == FALSE )
		return FALSE;

	/*
	 *	Delete the curren selection from the text.
	 */
	bDeleted = Delete( lpcd );

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Insert the clipboard contents
	 *	into the text.
	 */
	InsertText( lpcd, lpcd->ptCaretPos.y, lpcd->ptCaretPos.x, lpszText, &lpcd->ptCaretPos, ! bDeleted );

	/*
	 *	Update column position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Is the caret inside
	 *	the view?
	 */
	if ( CaretInView( lpcd ) == FALSE )
		/*
		 *	No. Move the view to
		 *	make it visible.
		 */
		MakeCaretVisibleNoRedraw( lpcd );

	/*
	 *	Re-render.
	 */
	InvalidateRect( lpcd->hWnd, NULL, FALSE );

	/*
	 *	Setup scrollers.
	 */
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
	return TRUE;
}

LRESULT OnCanCutDelete( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	return ( BOOL )( ! ISREADONLY && HasMark( lpcd ));
}

LRESULT OnCanCopy( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	return HasMark( lpcd );
}

LRESULT OnCanPaste( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	return ( BOOL )(( ! ISREADONLY ) && IsClipboardFormatAvailable( CF_TEXT ));
}

LRESULT OnCut( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	Cut( lpcd );
	return 0;
}

LRESULT OnCopy( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	Copy( lpcd );
	return 0;
}

LRESULT OnPaste( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	Paste( lpcd );
	return 0;
}

LRESULT OnClear( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	Delete( lpcd );
	return 0;
}

void ClearClipboard( LPCLASSDATA lpcd )
{
	/*
	 *	Open the clipboard.
	 */
	if ( OpenClipboard( lpcd->hWnd ))
	{
		/*
		 *	Empty it.
		 */
		 EmptyClipboard();

		/*
		 *	Send status message.
		 */
		 SendStatusMessage( lpcd );

		/*
		 *	Close the clipboard.
		 */
		CloseClipboard();
	}
}

void Paste( LPCLASSDATA lpcd )
{
	HANDLE		hData;
	LPCTSTR		lpszText;
	BOOL		bDeleted = FALSE;

	/*
	 *	Are we read-only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Valid format on the clipboard?
	 */
	if ( IsClipboardFormatAvailable( CF_TEXT ) == FALSE )
		return;

	/*
	 *	Any marks set?
	 */
	if ( HasMark( lpcd ))
		bDeleted = Delete( lpcd );

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Open the clipboard.
	 */
	if ( OpenClipboard( lpcd->hWnd ))
	{
		/*
		 *	Get data handle.
		 */
		if (( hData = GetClipboardData( CF_TEXT )) != NULL )
		{
			/*
			 *	Lock the data.
			 */
			if (( lpszText = GlobalLock( hData )) != NULL )
			{
				/*
				 *	Insert the clipboard contents
				 *	into the text.
				 */
				InsertText( lpcd, lpcd->ptCaretPos.y, lpcd->ptCaretPos.x, lpszText, &lpcd->ptCaretPos, ! bDeleted );

				/*
				 *	Unlock the data handle.
				 */
				GlobalUnlock( hData );
			}
		}
		/*
		 *	Close the clipboard.
		 */
		CloseClipboard();
	}

	/*
	 *	Update column position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Is the caret inside
	 *	the view?
	 */
	if ( CaretInView( lpcd ) == FALSE )
		/*
		 *	No. Move the view to
		 *	make it visible.
		 */
		MakeCaretVisibleNoRedraw( lpcd );

	/*
	 *	Re-render.
	 */
	InvalidateRect( lpcd->hWnd, NULL, FALSE );

	/*
	 *	Setup scrollers.
	 */
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

BOOL Copy( LPCLASSDATA lpcd )
{
	HANDLE		hData;
	BOOL		bRC = FALSE, bClearMark = FALSE;

	/*
	 *	Do we have a mark and, if so,
	 *	is it valid?
	 */
	if ( HasMark( lpcd ) == FALSE )
	{
		/*
		 *	We have no marker so we simply
		 *	copy the current line to the
		 *	clipboard.
		 */
		if ( ! MarkWholeLine( lpcd, TRUE ))
		{
			/*
			 *	Probably an empty line. Clear the
			 *	clipboard contents.
			 */
			if ( OpenClipboard( lpcd->hWnd ))
			{
				EmptyClipboard();
				CloseClipboard();
			}

			/*
			 *	Update the status.
			 */
			SendStatusMessage( lpcd );
			return FALSE;
		}
		bClearMark = TRUE;
	}

	/*
	 *	Open the clipboard.
	 */
	if ( OpenClipboard( lpcd->hWnd ))
	{
		/*
		 *	Empty the clipboard.
		 */
		EmptyClipboard();

		/*
		 *	Get the marked text.
		 */
		if (( hData = GetTextGlobal( lpcd, &lpcd->ptSelStart, &lpcd->ptSelEnd )) != NULL )
		{
			/*
			 *	Set data to the clipboard.
			 */
			if ( SetClipboardData( CF_TEXT, hData ) == hData )
				bRC = TRUE;
		}
		/*
		 *	Close the clipboard.
		 */
		CloseClipboard();
	}

	/*
	 *	Clear markers?
	 */
	if ( bClearMark )
		ClearMark( lpcd );
	return bRC;
}

BOOL Delete( LPCLASSDATA lpcd )
{
	/*
	 *	Read only?
	 */
	if ( ISREADONLY )
		return TRUE;

	/*
	 *	Do we have a mark and
	 *	is it valid?
	 */
	if ( HasMark( lpcd ))
	{
		/*
		 *	Remove the text.
		 */
		DeleteText( lpcd, &lpcd->ptSelStart, &lpcd->ptSelEnd, TRUE );

		/*
		 *	We _must_ have atleast one
		 *	empty line.
		 */
		if ( ArrayGetSize( lpcd->lpLines ) == 0 )
		{
			if ( InsertLine( lpcd, NULL, 0, -1 ) == FALSE )
				return FALSE;
		}

		/*
		 *	Hide the caret.
		 */
		DisplayCaret( lpcd, FALSE );

		/*
		 *	Move to the start position.
		 */
		lpcd->ptCaretPos = lpcd->ptSelStart;

		/*
		 *	Update column position.
		 */
		lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

		/*
		 *	Invalidate marks.
		 */
		lpcd->ptSelStart.x = lpcd->ptSelStart.y = -1;
		lpcd->ptSelEnd.x   = lpcd->ptSelEnd.y   = -1;

		/*
		 *	Is the caret inside
		 *	the view?
		 */
		if ( CaretInView( lpcd ) == FALSE )
			/*
			 *	No. Move the view to
			 *	make it visible.
			 */
			MakeCaretVisibleNoRedraw( lpcd );

		/*
		 *	Re-render.
		 */
		InvalidateRect( lpcd->hWnd, NULL, FALSE );

		/*
		 *	Setup scrollers.
		 */
		SetupHScroller( lpcd );
		SetupVScroller( lpcd );

		/*
		 *	We are modified.
		 */
		SetModified( lpcd, TRUE );

		/*
		 *	Send statrus message.
		 */
		SendStatusMessage( lpcd );

		/*
		 *	Show the caret.
		 */
		DisplayCaret( lpcd, TRUE );
	}
	return TRUE;
}

void Cut( LPCLASSDATA lpcd )
{
	/*
	 *	Read only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Do we have a mark and
	 *	is it valid?
	 */
	if ( HasMark( lpcd ))
	{
		/*
		 *	Copy the text.
		 */
		if ( Copy( lpcd ))
			/*
			 *	And delete it.
			 */
			Delete( lpcd );
	}
}

void CopyClipLine( LPCLASSDATA lpcd )
{
	/*
	 *	Mark the line.
	 */
	MarkLine( lpcd );

	/*
	 *	Valid marker?
	 */
	if ( HasMark( lpcd ))
		/*
		 *	Copy the line...
		 */
		 Copy( lpcd );
}

void CutLine( LPCLASSDATA lpcd )
{
	/*
	 *	Are we read-only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Mark the line.
	 */
	MarkLine( lpcd );

	/*
	 *	Valid marker?
	 */
	if ( HasMark( lpcd ))
		/*
		 *	Cut the line...
		 */
		 Cut( lpcd );
}

BOOL CopyAppend( LPCLASSDATA lpcd )
{
	HGLOBAL		hCombination, hClip;
	LPTSTR		pszSelection, pszClip, pszCombination;

	/*
	 *	Any text?
	 */
	if ( HasMark( lpcd ))
	{
		/*
		 *	Open the clipboard.
		 */
		if ( OpenClipboard( lpcd->hWnd ))
		{
			/*
			 *	Get the clip.
			 */
			if (( hClip = GetClipboardData( CF_TEXT )) != NULL )
			{
				/*
				 *	Lock it...
				 */
				if (( pszClip = GlobalLock( hClip )) != NULL )
				{
					/*
					 *	Get the selection.
					 */
					if (( pszSelection = GetText( lpcd, &lpcd->ptSelStart, &lpcd->ptSelEnd )) != NULL )
					{
						/*
						 *	Allocate combination...
						 */
						if (( hCombination = GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, REAL_SIZE( _tcslen( pszClip ) + _tcslen( pszSelection ) + 1 ))) != NULL )
						{
							/*
							 *	Lock the memory..
							 */
							if (( pszCombination = GlobalLock( hCombination )) != NULL )
							{
								/*
								 *	Copy the texts.
								 */
								_tcscpy( pszCombination, pszClip );
								_tcscat( pszCombination, pszSelection );

								/*
								 *	Unlock the memory.
								 */
								GlobalUnlock( hCombination );
								GlobalUnlock( hClip );

								/*
								 *	Free the selection.
								 */
								ArrayFreeMem(lpcd->lpUndoRedo, pszSelection );

								/*
								 *	Empty the clipboard and
								 *	set the combination.
								 */
								EmptyClipboard();

								/*
								 *	Unlock the data.
								 */
								GlobalUnlock( hCombination );

								/*
								 *	Put the data on the clipboard.
								 */
								if ( SetClipboardData( CF_TEXT, hCombination ) == hCombination )
									return TRUE;
							}
							GlobalFree( hCombination );
						}
						ArrayFreeMem( lpcd->lpUndoRedo, pszSelection );
					}
					GlobalUnlock( hClip );
				}
			}
			else
			{
				CloseClipboard();
				Copy( lpcd );
				return TRUE;
			}
			CloseClipboard();
		}
	}
	return FALSE;
}

void CutAppend( LPCLASSDATA lpcd )
{
	/*
	 *	Append the selection to the clipboard.
	 */
	if ( CopyAppend( lpcd ))
		/*
		 *	Cut it.
		 */
		 Delete( lpcd );
}
