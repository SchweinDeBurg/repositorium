/*
 *	goto.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"
#include "LangRes.rh"

static BOOL GotoLine( LPCLASSDATA lpcd, int nLine )
{
	LPLINE		lpLine;

	/*
	 *	Valid line?
	 */
	nLine--;
	if ( nLine >= 0 && nLine < Brainchild_ArrayGetSize( lpcd->lpLines ))
	{
		/*
		 *	Hide the caret.
		 */
		DisplayCaret( lpcd, FALSE );

		/*
		 *	Setup the line.
		 */
		lpcd->ptCaretPos.y = nLine;

		/*
		 *	Get a pointer to it.
		 */
		lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, nLine );

		/*
		 *	Setup column position.
		 */
		if ( lpcd->ptCaretPos.x > lpLine->nLength )
			/*
			 *	Keep it in range.
			 */
			lpcd->ptCaretPos.x = lpLine->nLength;

		/*
		 *	Make the caret visible.
		 */
		MakeCaretVisible( lpcd );
		DisplayCaret( lpcd, TRUE );
		return TRUE;
	}
	return FALSE;
}

LRESULT OnGotoLine( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Go to the line.
	 */
	return GotoLine( lpcd, ( int )wParam );
}

LRESULT OnGoto( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	LPPOINT		pPosition = ( LPPOINT )lParam;

	/*
	 *	Valid input?
	 */
	if ( pPosition && CheckPoint( lpcd, pPosition ))
	{
		/*
		 *	Hide the caret.
		 */
		DisplayCaret( lpcd, FALSE );
		
		/*
		 *	Setup the position.
		 */
		lpcd->ptCaretPos.x = pPosition->x;
		lpcd->ptCaretPos.y = pPosition->y;

		/*
		 *	Make the caret visible.
		 */
		MakeCaretVisible( lpcd );
		DisplayCaret( lpcd, TRUE );
		return TRUE;
	}
	return FALSE;
}

static BOOL CALLBACK GotoProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case	WM_INITDIALOG:
		{
			LPCLASSDATA	lpcd = ( LPCLASSDATA )lParam;
			HWND		hUpDown = GetDlgItem( hDlg, IDC_GOTO_SPIN );
			TCHAR		szBuffer[ 128 ];

			/*
			 *	Setup class data pointer as
			 *	the dialog user data.
			 */
			SetWindowLong( hDlg, DWL_USER, ( LONG )lpcd );

			/*
			 *	Format min/max display string.
			 */
			_stprintf( szBuffer, GetString( IDS_GOTO_MINMAX ), Brainchild_ArrayGetSize( lpcd->lpLines ));

			/*
			 *	Setup the up/down rangle.
			 */
			SendMessage( hUpDown, UDM_SETRANGE, 0, MAKELPARAM( Brainchild_ArrayGetSize( lpcd->lpLines ), 1 ));

			/*
			 *	Show the current line number
			 *	and setup the min/max static control.
			 */
			SetDlgItemInt( hDlg, IDC_GOTO_LINE, lpcd->ptCaretPos.y + 1, FALSE );
			SetDlgItemText( hDlg, IDC_GOTO_MINMAX, szBuffer );

			/*
			 *	Focus the edit control.
			 */
			return TRUE;
		}

		case	WM_CLOSE:
			/*
			 *	Bye...
			 */
			EndDialog( hDlg, 0 );
			break;

		case	WM_COMMAND:
			switch ( LOWORD( wParam ))
			{
				case	IDC_GOTO_LINE:
				{
					/*
					 *	Lost focus on the edit control?
					 */
					if ( HIWORD( wParam ) == EN_KILLFOCUS )
					{
						/*
						 *	Get data.
						 */
						LPCLASSDATA	lpcd = ( LPCLASSDATA )GetWindowLong( hDlg, DWL_USER );
						int		nLine = GetDlgItemInt( hDlg, IDC_GOTO_LINE, NULL, FALSE );
						int		nLines = Brainchild_ArrayGetSize( lpcd->lpLines );

						/*
						 *	Clamp entered value between 1 and the
						 *	number of lines + 1.
						 */
						if (      nLine < 1      ) nLine = 1;
						else if ( nLine > nLines ) nLine = nLines;

						/*
						 *	Set this value to the 
						 *	edit control.
						 */
						SetDlgItemInt( hDlg, IDC_GOTO_LINE, ( UINT )nLine, FALSE );
					}
					break;
				}

				case	IDOK:
				{
					/*
					 *	Get data.
					 */
					LPCLASSDATA	lpcd = ( LPCLASSDATA )GetWindowLong( hDlg, DWL_USER );
					int		nLine = GetDlgItemInt( hDlg, IDC_GOTO_LINE, NULL, FALSE );
					int		nLines = Brainchild_ArrayGetSize( lpcd->lpLines );

					/*
					 *	Clamp entered value between 1 and the
					 *	number of lines + 1.
					 */
					if (      nLine < 1      ) nLine = 1;
					else if ( nLine > nLines ) nLine = nLines;

					/*
					 *	Setup the line.
					 */
					GotoLine( lpcd, nLine );

					/*
					 *	Fall through...
					 */
				}

				case	IDCANCEL:
					/*
					 *	Bye...
					 */
					EndDialog( hDlg, 0 );
					break;
			}

	}
	return FALSE;
}

void GotoDialog( LPCLASSDATA lpcd )
{
	/*
	 *	Clear qualifiers.
	 */
	lpcd->cQualifier = 0;

	/*
	 *	Popup the dialog.
	 */
	DialogBoxParam( hDLL, MAKEINTRESOURCE( IDD_GOTO ), lpcd->hWnd, GotoProc, ( LPARAM )lpcd );
}

LRESULT OnGotoDialog( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Popup the goto dialog.
	 */
	GotoDialog( lpcd );
	return 0;
}
