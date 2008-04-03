/*
 *	misc.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Miscellanious code.
 */

#include "stdafx.h"

#include "defs.h"

LRESULT OnModified( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	return lpcd->bModified;
}

LRESULT OnOverwrite( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	return lpcd->bOverwrite;
}

LRESULT OnAnyText( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	return AnyText( lpcd );
}

LRESULT OnGetExtentions( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	return ( LRESULT )Parser->pszExtentions;
}

BOOL OpenSettingsDialog( HWND hParent )
{
	HMODULE		   hBCPL;
	FARPROC		   lpfnApplet;
	BOOL		   bRC = FALSE;

	/*
	 *	Open up the brainchild
	 *	control panel application.
	 */
	if (( hBCPL = LoadLibrary( _T( "bcc.cpl" ))) != NULL )
	{
		/*
		 *	Get procedure.
		 */
		if (( lpfnApplet = GetProcAddress( hBCPL, "CPlApplet" )) != NULL )
		{
			/*
			 *	Initialize the control panel applet.
			 */
			if (( lpfnApplet )( hParent, CPL_INIT, 0, 0 ))
			{
				/*
				 *	Since I know for sure I only have one
				 *	dialog supported in the CPL and I do
				 *	not need to inquire we can simply
				 *	pop the dialog right away.
				 */
				( lpfnApplet )( hParent, CPL_DBLCLK, 0, 0 );

				/*
				 *	Free memory etc.
				 */
				( lpfnApplet )( hParent, CPL_STOP, 0, 0 );
				( lpfnApplet )( hParent, CPL_EXIT, 0, 0 );

				/*
				 *	Success.
				 */
				bRC = TRUE;
			}
		}
		/*
		 *	Unload the library.
		 */
		FreeLibrary( hBCPL );
	}
	return bRC;
}

LRESULT OnSettingsDialog( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	return OpenSettingsDialog( hWnd );
}

/*
 *	Send a status message.
 */
void SendStatusMessage( LPCLASSDATA lpcd )
{
	NMSTATUSUPDATE		nms;
	HWND			hParent;

	/*
	 *	Do we have a parent?
	 */
	if (( hParent = GetParent( lpcd->hWnd )) != NULL )
	{
		/*
		 *	Fill in the structure.
		 */
		nms.hdr.code		= NMBC_STATUSUPDATE;
		nms.hdr.hwndFrom	= lpcd->hWnd;
		nms.hdr.idFrom		= GetWindowLong( lpcd->hWnd, GWL_ID );
		nms.bAnyText		= AnyText( lpcd );
		nms.bCanCopy		= OnCanCopy( lpcd->hWnd, 0, 0, lpcd );
		nms.bCanCut		= nms.bCanDelete = OnCanCutDelete( lpcd->hWnd, 0, 0, lpcd );
		nms.bCanPaste		= OnCanPaste( lpcd->hWnd, 0, 0, lpcd );
		nms.bCanRedo		= OnCanRedo( lpcd->hWnd, 0, 0, lpcd );
		nms.bCanUndo		= OnCanUndo( lpcd->hWnd, 0, 0, lpcd );
		nms.bModified		= lpcd->bModified;
		nms.bOverwrite		= lpcd->bOverwrite;
		nms.bReadOnly		= ( BOOL )( ISREADONLY );
		nms.bBookmarks		= ( BOOL )lpcd->nBookmarks;
		nms.nFileMode		= Parser->nFileMode;

		/*
		 *	Send the notification if it
		 *	changed.
		 */
		if ( memcmp( &lpcd->suLastSent, &nms, sizeof( nms )) != 0 )
		{
			SendMessage( hParent, WM_NOTIFY, nms.hdr.idFrom, ( LPARAM )&nms );
			memcpy( &lpcd->suLastSent, &nms, sizeof( nms ));
		}
	}
}

/*
 *	Toggle overwrite mode.
 */
void ToggleOverwrite( LPCLASSDATA lpcd )
{
	// Toggle overwrite mode.
	lpcd->bOverwrite = ! lpcd->bOverwrite;

	// Destroy the caret if we have focus and
	// the caret type is not fixed at a block cursor.
	if ( Parser->nCaretType != CARET_BLOCK && lpcd->bHasFocus ) 
	{
		// Destroy...
		DisplayCaret( lpcd, FALSE );
		DestroyCaret();
	}

	// Update status.
	SendStatusMessage( lpcd );

	// Create the caret if we have focus and
	// the caret type is not fixed at a block cursor.
	if ( Parser->nCaretType != CARET_BLOCK && lpcd->bHasFocus ) 
		// Create the caret.
		CreateTheCaret( lpcd ); 
}

/*
 *	Load a string from the resources.
 */
LPCTSTR GetString( int nID )
{
	static TCHAR	cBuffer[ 1024 ];
	LoadString( hDLL, nID, cBuffer, 1024 );
	return cBuffer;
}

/*
 *	Set/Clear modified mode.
 */
void SetModified( LPCLASSDATA lpcd, BOOL bModified )
{
	if ( bModified != lpcd->bModified )
	{
		lpcd->bModified = bModified;
		SendStatusMessage( lpcd );
	}
}

/*
 *	Create a string copy (mempool version).
 */
LPTSTR CopyStringPool( POOL pMemPool, LPCTSTR pszString )
{
	LPTSTR	pszCopy;

	/*
	 *	Allocate copy.
	 */
	if (( pszCopy = AllocPooled( pMemPool, REAL_SIZE( _tcslen( pszString ) + 1 ))) != NULL )
		/*
		 *	Copy the string.
		 */
		_tcscpy( pszCopy, pszString );

	return pszCopy;
}

/*
 *	Create a string copy.
 */
LPTSTR CopyString( LPCLASSDATA lpcd, LPCTSTR pszString )
{
	return CopyStringPool( lpcd->pMemPool, pszString);
}

/*
 *	Any text in the buffer?
 */
BOOL AnyText( LPCLASSDATA lpcd )
{
	if ( ArrayGetSize( lpcd->lpLines ) == 1 && (( LPLINE )ArrayGetAt( lpcd->lpLines, 0 ))->nLength == 0 )
		return FALSE;
	return TRUE;
}

LRESULT OnGetDlgCode( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	return DLGC_WANTALLKEYS;
}
