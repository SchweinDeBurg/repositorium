/*
 *	bookmarks.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"

void SetBookmark( LPARAM lParam, LPCLASSDATA lpcd )
{
	LPLINE		lpLine = GETLINE( lpcd );

	/*
	 *	Bookmark already set?
	 */
	if (( lpLine->dwFlags & LNF_BOOKMARK ) != LNF_BOOKMARK )
	{
		/*
		 *	Set the bit.
		 */
		lpLine->dwFlags |= LNF_BOOKMARK;

		/*
		 *	Increase the bookmark counter.
		 */
		lpcd->nBookmarks++;

		/*
		 *	Do we need to send a status message?
		 */
		if ( lpcd->nBookmarks == 1 )
			SendStatusMessage( lpcd );

		/*
 		 *	Re-render the line.
		 */
		RenderLine( lpcd, lpcd->ptCaretPos.y );
	}
}

void ClearBookmark( LPARAM lParam, LPCLASSDATA lpcd )
{
	LPLINE		lpLine = GETLINE( lpcd );

	/*
	 *	Bookmark bit set?
	 */
	if (( lpLine->dwFlags & LNF_BOOKMARK ) == LNF_BOOKMARK )
	{
		/*
 		 *	Clear the bit.
		 */
		lpLine->dwFlags &= ~LNF_BOOKMARK;

		/*
		 *	Decrease the bookmark counter.
		 */
		lpcd->nBookmarks--;

		/*
		 *	Do we need to send a status message?
		 */
		if ( lpcd->nBookmarks == 0 )
			SendStatusMessage( lpcd );

		/*
		 *	Re-render the line.
		 */
		RenderLine( lpcd, lpcd->ptCaretPos.y );
	}
}

void ToggleBookmark( LPARAM lParam, LPCLASSDATA lpcd )
{
	LPLINE		lpLine = GETLINE( lpcd );
	BOOL		bStatus = FALSE;

	/*
	 *	Is the bookmark set or cleared?
	 */
	if (( lpLine->dwFlags & LNF_BOOKMARK ) == LNF_BOOKMARK ) 
	{
		/*
		 *	Decrease the bookmark counter.
		 */
		lpcd->nBookmarks--;
		if ( lpcd->nBookmarks == 0 )
			bStatus = TRUE;
	}
	else
	{
		/*
		 *	Increase the bookmark counter.
		 */
		lpcd->nBookmarks++;
		if ( lpcd->nBookmarks == 1 )
			bStatus = TRUE;
	}

	/*
	 *	Flip the bookmark
	 *	bit.
	 */
	lpLine->dwFlags ^= LNF_BOOKMARK;

	/*
	 *	Send the status message.
	 */
	if ( bStatus )
		SendStatusMessage( lpcd );

	/*
	 *	Re-render the line.
	 */
	RenderLine( lpcd, lpcd->ptCaretPos.y );
}

void ClearAllBookmarks( LPARAM lParam, LPCLASSDATA lpcd )
{
	LPLINE		lpLine;
	BOOL		bStatus = FALSE;
	int		i;

	/*
	 *	Iterate lines.
	 */
	for ( i = 0; i < Brainchild_ArrayGetSize( lpcd->lpLines ); i++ )
	{
		/*
		 *	Get line pointer.
		 */
		lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, i );

		/*
		 *	Bookmark?
		 */
		if (( lpLine->dwFlags & LNF_BOOKMARK ) == LNF_BOOKMARK )
		{
			/*
			 *	We must send a status message.
			 */
			bStatus = TRUE;

			/*
			 *	Clear the bit.
			 */
			lpLine->dwFlags &= ~LNF_BOOKMARK;

			/*
			 *	Is it visible?
			 */
			if ( i >= lpcd->ptViewPos.y && i < lpcd->ptViewPos.y + lpcd->szViewSize.cy )
				/*
				 *	Render the line.
				 */
				RenderLine( lpcd, i );
		}
	}

	/*
	 *	No more bookmarks.
	 */
	lpcd->nBookmarks = 0;

	/*
	 *	Send status message if necessary.
	 */
	if ( bStatus )
		SendStatusMessage( lpcd );
}

void NextBookmark( LPARAM lParam, LPCLASSDATA lpcd )
{
	LPLINE		lpLine;
	int		i, nLines = Brainchild_ArrayGetSize( lpcd->lpLines ) - 1, nLine = lpcd->ptCaretPos.y;

	/*
	 *	Only one line?
	 */
	if ( nLines == 0 )
		return;

	/*
	 *	On the last line?
	 */
	if ( lpcd->ptCaretPos.y == nLines )
	{
		/*
		 *	Set anchor.
		 */
		lpcd->nBookmarkAnchor = lpcd->ptCaretPos.y;

		/*
		 *	Go to the start of the first line.
		 */
		lpcd->ptCaretPos.y = lpcd->ptCaretPos.x = lpcd->nLastColumnPos = 0;

		/*
		 *	Call ourselves.
		 */
		NextBookmark( lParam, lpcd );
	}

	/*
	 *	If we are located on a bookmark
	 *	and the anchor is not set yet
	 *	we start looking on the next line.
	 */
	lpLine = GETLINE( lpcd );
	if ((( lpLine->dwFlags & LNF_BOOKMARK ) == LNF_BOOKMARK ) && lpcd->nBookmarkAnchor == -1 )
		nLine++;

	/*
	 *	Iterate lines.
	 */
	for ( i = nLine; i <= nLines; i++ )
	{
		lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, i );

		/*
		 *	Bookmark?
		 */
		if ((( lpLine->dwFlags & LNF_BOOKMARK ) == LNF_BOOKMARK ) || i == lpcd->nBookmarkAnchor )
		{
		 	/*
			 *	On the anchor?
			 */
			if ( i == lpcd->nBookmarkAnchor )
			{
				/*
				 *	Done...
				 */
				MessageBeep( 0xFFFFFFFF );
			}

			/*
			 *	Hide the caret.
			 */
			DisplayCaret( lpcd, FALSE );

			/*
			 *	Yes. Move the caret here.
			 */
			lpcd->ptCaretPos.y = i;
			lpcd->ptCaretPos.x = lpcd->nLastColumnPos = 0;

			/*
			 *	Make the caret visible.
			 */
			MakeCaretVisible( lpcd );

			/*
			 *	Reset anchor.
			 */
			lpcd->nBookmarkAnchor = -1;

			/*
			 *	Show the caret.
			 */
			DisplayCaret( lpcd, TRUE );
			return;
		}
	}

	/*
	 *	Reaching this means that we are
	 *	on the last line or that there
	 *	are no bookmarks.
	 */

	/*
	 *	Set anchor.
	 */
	lpcd->nBookmarkAnchor = nLine;

	/*
	 *	Go to the start of the file and try again.
	 */
	lpcd->ptCaretPos.y = lpcd->ptCaretPos.x = lpcd->nLastColumnPos = 0;

	/*
	 *	Call ourselves.
	 */
	NextBookmark( lParam, lpcd );
}

void PrevBookmark( LPARAM lParam, LPCLASSDATA lpcd )
{
	LPLINE		lpLine;
	int		i, nLines = Brainchild_ArrayGetSize( lpcd->lpLines ) - 1, nLine = lpcd->ptCaretPos.y;

	/*
	 *	Only one line?
	 */
	if ( nLines== 0 )
		return;

	/*
	 *	On the first line?
	 */
	if ( lpcd->ptCaretPos.y== 0 )
	{
		/*
		 *	Set anchor.
		 */
		lpcd->nBookmarkAnchor = lpcd->ptCaretPos.y;

		/*
		 *	Go to the start of the last line.
		 */
		lpcd->ptCaretPos.y = nLines;
		lpcd->ptCaretPos.x = lpcd->nLastColumnPos = 0;

		/*
		 *	Call ourselves.
		 */
		PrevBookmark( lParam, lpcd );
	}

	/*
	 *	If we are located on a bookmark
	 *	and the anchor is not set yet
	 *	we start looking on the previous line.
	 */
	lpLine = GETLINE( lpcd );
	if ((( lpLine->dwFlags & LNF_BOOKMARK ) == LNF_BOOKMARK ) && lpcd->nBookmarkAnchor == -1 )
		nLine--;

	/*
	 *	Iterate lines.
	 */
	for ( i = nLine; i >= 0; i-- )
	{
		lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, i );

		/*
		 *	Bookmark?
		 */
		if ((( lpLine->dwFlags & LNF_BOOKMARK ) == LNF_BOOKMARK ) || i == lpcd->nBookmarkAnchor )
		{
			/*
			 *	On the anchor?
			 */
			if ( i == lpcd->nBookmarkAnchor )
			{
				/*
				 *	Done...
				 */
				MessageBeep( 0xFFFFFFFF );
			}

			/*
			 *	Hide the caret.
			 */
			DisplayCaret( lpcd, FALSE );

			/*
			 *	Yes. Move the caret here.
			 */
			lpcd->ptCaretPos.y = i;
			lpcd->ptCaretPos.x=lpcd->nLastColumnPos = 0;

			/*
			 *	Make the caret visible.
			 */
			MakeCaretVisible( lpcd );

			/*
			 *	Reset anchor.
			 */
			lpcd->nBookmarkAnchor = -1;

			/*
			 *	Show the caret.
			 */
			DisplayCaret( lpcd, TRUE );
			return;
		}
	}

	/*
	 *	Reaching this means that we are
	 *	on the last line or that there
	 *	are no bookmarks.
	 */

	/*
	 *	Set anchor.
	 */
	lpcd->nBookmarkAnchor = nLine;

	/*
	 *	Go to the end of the file and try again.
	 */
	lpcd->ptCaretPos.y = nLines;
	lpcd->ptCaretPos.x = lpcd->nLastColumnPos = 0;

	/*
	 *	Call ourselves.
	 */
	PrevBookmark( lParam, lpcd );
}

void FirstBookmark( LPARAM lParam, LPCLASSDATA lpcd )
{
	LPLINE		lpLine;
	int		i;

	/*
	 *	Iterate...
	 */
	for ( i = 0; i < Brainchild_ArrayGetSize( lpcd->lpLines ); i++ )
	{
		lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, i );

		/*
		 *	Bookmark?
		 */
		if (( lpLine->dwFlags & LNF_BOOKMARK ) == LNF_BOOKMARK )
		{
			/*
			 *	Hide the caret.
			 */
			DisplayCaret( lpcd, FALSE );

			/*
			 *	Yes. Move the caret here.
			 */
			lpcd->ptCaretPos.y = i;
			lpcd->ptCaretPos.x = lpcd->nLastColumnPos = 0;

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
	}

	/*
	 *	No bookmark found.
	 */
	MessageBeep( 0xFFFFFFFF );
}

void LastBookmark( LPARAM lParam, LPCLASSDATA lpcd )
{
	LPLINE		lpLine;
	int		i;

	/*
	 *	Iterate...
	 */
	for ( i = Brainchild_ArrayGetSize( lpcd->lpLines ) - 1; i >= 0; i-- )
	{
		lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, i );

		/*
		 *	Bookmark?
		 */
		if (( lpLine->dwFlags & LNF_BOOKMARK ) == LNF_BOOKMARK )
		{
			/*
			 *	Hide the caret.
			 */
			DisplayCaret( lpcd, FALSE );

			/*
			 *	Yes. Move the caret here.
			 */
			lpcd->ptCaretPos.y = i;
			lpcd->ptCaretPos.x = lpcd->nLastColumnPos = 0;

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
	}

	/*
	 *	No bookmark found.
	 */
	MessageBeep( 0xFFFFFFFF );
}
