/*
 *	io.c
 *
 *	(C) Copyright 1993-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	File IO.
 */

#include "stdafx.h"

#include "defs.h"
#include "LangRes.rh"

void SetFileName( LPCLASSDATA lpcd, LPCTSTR lpszFileName )
{
	HWND			hParent;
	NMFILENAMECHANGED	nmfnc;
	SHFILEINFO		psfi;

	/*
	 *	Copy the file name.
	 */
	_tcscpy( &lpcd->szFileName[ 0 ], lpszFileName );

	/*
	 *	Control text is the file name.
	 */
	SetWindowText( lpcd->hWnd, lpszFileName );

	/*
	 *	Setup the parser.
	 */
	SetupParser( lpcd, lpszFileName );

	/*
	 *	Destroy current icons.
	 */
	if ( lpcd->hIcon ) DestroyIcon( lpcd->hIcon );
	if ( lpcd->hIconLarge ) DestroyIcon( lpcd->hIconLarge );

	/*
	 *	Extract small file icon.
	 */
	if ( SHGetFileInfo( lpszFileName, 0, &psfi, sizeof( SHFILEINFO ), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON ))
		/*
		 *	Setup the new icon.
		 */
		lpcd->hIcon = psfi.hIcon;
	else
		/*
		 *	Use the default icon.
		 */
		lpcd->hIcon = NULL;

	/*
	 *	Extract the large file icon.
	 */
	if ( SHGetFileInfo( lpszFileName, 0, &psfi, sizeof( SHFILEINFO ), SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_LARGEICON ))
		/*
		 *	Setup the new icon.
		 */
		lpcd->hIconLarge = psfi.hIcon;
	else
		/*
		 *	Use the default icon.
		 */
		lpcd->hIconLarge = NULL;

	/*
	 *	Send the NMFILENAMECHANGED notification.
	 */
	if (( hParent = GetParent( lpcd->hWnd )) != NULL )
	{
		/*
		 *	Fill the structure.
		 */
		nmfnc.hdr.code		= NMBC_FILENAMECHANGED;
		nmfnc.hdr.hwndFrom	= lpcd->hWnd;
		nmfnc.hdr.idFrom	= GetWindowLong( lpcd->hWnd, GWL_ID );
		nmfnc.lpszFileName	= &lpcd->szFileName[ 0 ];
		nmfnc.lpszFileTypeDesc	= Parser->pszTitle;

		/*
		 *	Setup icons.
		 */
		nmfnc.hFileTypeIconSmall = lpcd->hIcon;
		nmfnc.hFileTypeIconLarge = lpcd->hIconLarge;

		/*
		 *	Send the notification.
		 */
		SendMessage( hParent, WM_NOTIFY, nmfnc.hdr.idFrom, ( LPARAM )&nmfnc );
	}
}

LRESULT OnSetReadOnly( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	BOOL	bStatus = FALSE;

	/*
	 *	Set it?
	 */
	if ( wParam )
	{
		/*
		 *	Already set?
		 */
		if (( lpcd->dwFileAttr & FILE_ATTRIBUTE_READONLY ) != FILE_ATTRIBUTE_READONLY )
		{
			/*
			 *	No. Set it and re-render.
			 */
			lpcd->dwFileAttr |= FILE_ATTRIBUTE_READONLY;
			InvalidateRect( hWnd, NULL, FALSE );
			bStatus = TRUE;
		}
	}
	else
	{
		/*
		 *	Already cleared?
		 */
		if (( lpcd->dwFileAttr & FILE_ATTRIBUTE_READONLY ) == FILE_ATTRIBUTE_READONLY )
		{
			/*
			 *	No. Clear it and re-render.
			 */
			lpcd->dwFileAttr &= ~FILE_ATTRIBUTE_READONLY;
			InvalidateRect( hWnd, NULL, FALSE );
			bStatus = TRUE;
		}
	}

	/*
	 *	Send status message.
	 */
	if ( bStatus )
		SendStatusMessage( lpcd );
	return 0;
}

LRESULT OnGetFileInfo( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	LPFILEINFO	lpfi = ( LPFILEINFO )lParam;

	/*
	 *	Size of structure OK?
	 */
	if ( lpfi->dwSize >= 20 )
	{
		/*
		 *	Store the data.
		 */
		lpfi->hFileTypeIconLarge = lpcd->hIconLarge;
		lpfi->hFileTypeIconSmall = lpcd->hIcon;
		lpfi->lpszFileTypeDesc   = Parser->pszTitle;
		lpfi->lpszFileName	 = &lpcd->szFileName[ 0 ];
		return TRUE;
	}
	return FALSE;
}

LRESULT OnSetFileName( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Valid file name?
	 */
	if ( lParam && _tcslen(( LPCTSTR )lParam ))
	{
		/*
		 *	Set it up.
		 */
		SetFileName( lpcd, ( LPCTSTR )lParam );
		return TRUE;
	}
	return FALSE;
}

LRESULT OnCheckModified( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Are we modified?
	 */
	if ( lpcd->bModified )
	{
		switch ( YesNoCancelWarning( lpcd, NULL, GetString( IDS_FILE_MODIFIED ), lpcd->szFileName ))
		{
			case	IDYES:
				/*
				 *	Save the file first.
				 */
				if ( OnSaveFile( hWnd, TRUE, 0L, lpcd ) == FALSE )
					return FALSE;
				break;

			case	IDCANCEL:
				/*
				 *	Bye...
				 */
				return FALSE;
		}
	}
	return TRUE;
}

/*
 *	LPARAM = ( LPCTSTR )lpszFileName
 *	WPARAM = ( BOOL )bForce
 */
LRESULT OnLoadFile( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	TCHAR		szFileName[ MAX_PATH ] = { 0 };
	DWORD		dwErr = ERROR_SUCCESS;
	BOOL		bRC = TRUE;

	/*
	 *	Force the load?
	 */
	if ( wParam == FALSE )
	{
		/*
		 *	Are we modified?
		 */
		if ( OnCheckModified( hWnd, 0, 0, lpcd ) == FALSE )
			return BCMR_SAVEFIRST;
	}

	/*
	 *	File name passed?
	 */
	if ( lParam != 0L )
		_tcscpy( szFileName, ( LPCTSTR )lParam );
	else
		/*
		 *	Filename is required...
		 */
		 return BCMR_ERROR;
	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Delete the current contents.
	 */
	DeleteContents( lpcd );

	/*
	 *	Show busy Cursor.
	 */
	SetCursor( lpcd->hWait );

	/*
	 *	Load the file.
	 */
	if (( bRC = LoadFile( lpcd, szFileName )) == FALSE )
		dwErr = GetLastError();

	/*
	 *	Error?
	 */
	if ( bRC == FALSE )
		OkIOError( lpcd, lpcd->hWnd, GetString( IDS_CANT_OPEN ), szFileName );

	/*
	 *	Original file read-only?
	 */
	if (( lpcd->dwFileAttr & FILE_ATTRIBUTE_READONLY ) == FILE_ATTRIBUTE_READONLY )
		/*
		 *	Not necessary.
		 */
		 lpcd->bOpenAsReadOnly = FALSE;

	/*
	 *	Open as read-only?
	 */
	if ( lpcd->bOpenAsReadOnly == TRUE )
		/*
		 *	Force read only.
		 */
		lpcd->dwFileAttr |= FILE_ATTRIBUTE_READONLY;

	/*
	 *	No longer modified.
	 */
	SetModified( lpcd, FALSE );

	/*
	 *	Reset view.
	 */
	lpcd->cQualifier	= 0;
	lpcd->nLastColumnPos	= 0;
	lpcd->ptCaretPos.x	= lpcd->ptCaretPos.y = 0;
	lpcd->ptViewPos.x	= lpcd->ptViewPos.y  = 0;
	lpcd->ptSelStart.x	= lpcd->ptSelStart.y = -1;
	lpcd->ptSelEnd.x	= lpcd->ptSelEnd.y   = -1;

	/*
	 *	Save off path name.
	 */
	SetFileName( lpcd, szFileName );

	/*
	 *	No lines pre-parsed yet.
	 */
	lpcd->dwParsedUpTo = 0;

	/*
	 *	Re-render.
	 */
	UpdateWindow( hWnd );
	SetupHScroller( lpcd );
	SetupVScroller( lpcd );

	/*
	 *	Show I-beam Cursor.
	 */
	SetCursor( lpcd->hIBeam );

	/*
	 *	Send notification messages.
	 */
	SendStatusMessage( lpcd );
	SendCaretMessage( lpcd );

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );

	/*
	 *	Reset error code.
	 */
	if ( bRC == FALSE )
	{
		// Set last error code.
		SetLastError( dwErr );
		return BCMR_ERROR;
	}

	return BCMR_OK;
}

void SaveFileAsIs( LPCLASSDATA lpcd )
{
	/*
	 *	Save the file under it's
	 *	current name.
	 */
	OnSaveFile( lpcd->hWnd, FALSE, ( LPARAM )lpcd->szFileName, lpcd );
}

/*
 *	Perform file backup.
 */
static BOOL FileBackup( LPCLASSDATA lpcd )
{
	TCHAR		szPath[ MAX_PATH ], szExt[ 50 ], *pszPtr, *pszBak = _T( ".bak" );
	BOOL		bRC = TRUE;

	/*
	 *	Backup enabled?
	 */
	if ( Parser->bBackup )
	{
		/*
		 *	Copy the name into
		 *	the buffer.
		 */
		_tcscpy( szPath, lpcd->szFileName );

		/*
		 *	What is the file backup mode?
		 */
		switch ( Parser->nBackupMode )
		{
			case	BMODE_FILE_BAK:
				/*
				 *	Replace the file extention
				 *	with the .bak extention.
				 */
				if (( pszPtr = _tcsrchr( szPath, _T( '.' ))) != NULL ) _tcscpy( pszPtr, pszBak );
				else						      _tcscat( szPath, pszBak );
				break;

			case	BMODE_FILE_BAK_EXT:
				/*
				 *	Make the file extention
				 *	".bak.ext".
				 */
				if (( pszPtr = _tcsrchr( szPath, _T( '.' ))) != NULL )
				{
					_tcscpy( szExt, pszPtr );
					_tcscpy( pszPtr, pszBak );
					_tcscat( szPath, szExt );
				}
				else
					_tcscat( szPath, pszBak );
				break;

			case	BMODE_FILE_EXT_BAK:
				/*
				 *	Append the ".bak" extention.
				 */
				_tcscat( szPath, pszBak );
				break;

			case	BMODE_FILE_EXT_PATH:
				/*
				 *	Copy the folder name.
				 */
				if ( Parser->pszBackupPath )
				{
					_tcscpy( szPath, Parser->pszBackupPath );
					_tcscat( szPath, _T( "\\" ));
				}
				else
				{
					GetTempPath( MAX_PATH, szPath );
					_tcscat( szPath, _T( "\\" ));
				}

				/*
				 *	Add the file part.
				 */
				if (( pszPtr = _tcsrchr( lpcd->szFileName, _T( '\\' ))) == NULL ) pszPtr = lpcd->szFileName;
				else							         pszPtr++;

				/*
				 *	Append file name.
				 */
				_tcscat( szPath, pszPtr );
				break;
		}

		/*
		 *	Backup the current file.
		 */
		if ( CopyFile( lpcd->szFileName, szPath, FALSE ) == FALSE )
		{
			/*
			 *	Backup failed?
			 */
			if ( GetLastError() != ERROR_FILE_NOT_FOUND )
			{
				/*
				 *	Ask if the user wishes
				 *	to continue.
				 */
				if ( YesNoWarning( lpcd, lpcd->hWnd, GetString( IDS_BACKUP_FAILED )) == IDNO )
					bRC = FALSE;
			}
		}
	}
	return bRC;
}

/*
 *	LPARAM = ( LPCTSTR )lpszFileName
 *	WPARAM = Not Used
 */
LRESULT OnSaveFile( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	TCHAR		szFileName[ MAX_PATH ] = { 0 };
	LPLINE		lpLine = NULL;
	DWORD		dwErr = ERROR_SUCCESS;
	BOOL		bRC = TRUE;

	/*
	 *	File name passed?
	 */
	if ( lParam != 0L ) _tcscpy( szFileName, ( LPCTSTR )lParam );
	else		    _tcscpy( szFileName, lpcd->szFileName );

	/*
	 *	Show busy Cursor.
	 */
	SetCursor( lpcd->hWait );

	/*
	 *	Backup existing file.
	 */
	if ( FileBackup( lpcd ) == FALSE )
		return BCMR_BACKUP_ERROR;

	/*
	 *	Save the file...
	 */
	if (( bRC = SaveFile( lpcd, szFileName )) == TRUE )
	{
		/*
		 *	Setup the parser for this
		 *	file name.
		 */
		SetupParser( lpcd, szFileName );

		/*
		 *	Make sure the caret is
		 *	on a valid text position.
		 *
		 *	This may not be the case if
		 *	it is located on a blank which
		 *	was stripped from the line.
		 */
		lpLine = GETLINE( lpcd );

		if ( lpcd->ptCaretPos.x > lpLine->nLength )
		{
			lpcd->ptCaretPos.x = lpLine->nLength;
			UpdateCaret( lpcd );
		}

		/*
		 *	Setup file name.
		 */
		SetFileName( lpcd, szFileName );

		/*
		 *	Mark position.
		 */
		lpcd->nSavePosition = lpcd->nUndoPosition;

		/*
		 *	No longer modified.
		 */
		SetModified( lpcd, FALSE );

		/*
		 *	Force a status message.
		 */
		SendStatusMessage( lpcd );
	}
	else
	{
		/*
		 *	Save the error.
		 */
		dwErr = GetLastError();

		/*
		 *	Issue dialog.
		 */
		OkIOError( lpcd, lpcd->hWnd, GetString( IDS_CANT_SAVE ), szFileName );
	}

	/*
	 *	Show I-beam Cursor.
	 */
	SetCursor( lpcd->hIBeam );

	/*
	 *	Reset error uppon failure.
	 */
	if ( bRC == FALSE )
	{
		SetLastError( dwErr );
		return BCMR_ERROR;
	}
	return BCMR_OK;
}

LRESULT OnReadOnly( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Return wether or not the file
	 *	is read-only.
	 */
	return ( BOOL )(( lpcd->dwFileAttr & FILE_ATTRIBUTE_READONLY ) == FILE_ATTRIBUTE_READONLY );
}
