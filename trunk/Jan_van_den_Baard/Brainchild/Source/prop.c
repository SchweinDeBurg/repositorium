/*
 *	prop.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"
#include "LangRes.rh"

/*
 *	File types.
 */
static LPCTSTR lpszFileModes[] = 
{
	_T("MS DOS"), _T("Unix"), _T("Macintosh")
};

static void SetSizeControls( HWND hDlg )
{
	LPCLASSDATA	lpcd = ( LPCLASSDATA )GetWindowLong( hDlg, DWL_USER );
	TCHAR		szBuffer[ 256 ];
	SHFILEINFO	psfi;
	int		nLines = Brainchild_ArrayGetSize( lpcd->lpLines ), i, nFileMode;
	unsigned int	nBytes = 0, nLineTerm = 1;

	/*
	 *	Get system file type.
	 */
	SHGetFileInfo( lpcd->szFileName, 0, &psfi, sizeof( SHFILEINFO ), SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME );

	SetDlgItemText( hDlg, IDC_PROP_SYSTYPE, psfi.szTypeName[ 0 ] ? psfi.szTypeName : GetString( IDS_NO_TYPENAME ));

	/*
	 *	Setup language name.
	 */
	SetDlgItemText( hDlg, IDC_PROP_LANGUAGE, Parser->pszTitle );

	/*
	 *	Setup tab-size.
	 */
	SetDlgItemInt( hDlg, IDC_PROP_TABSIZE, Parser->nTabSize, FALSE );

	/*
	 *	Setup icon.
	 */
	SendDlgItemMessage( hDlg, IDC_PROP_ICON, STM_SETICON, ( WPARAM )lpcd->hIconLarge, 0 );

	/*
	 *	Setup file mode.
	 */
	SendDlgItemMessage( hDlg, IDC_PROP_TYPE, CB_SETCURSEL, Parser->nFileMode, 0 );

	/*
	 *	Get file mode.
	 */
	nFileMode = SendDlgItemMessage( hDlg, IDC_PROP_TYPE, CB_GETCURSEL, 0, 0 );

	/*
	 *	Compute size of the
	 *	line terminator.
	 */
	if ( nFileMode == FMODE_MSDOS || nFileMode == FMODE_UNIX )
		nLineTerm = 2;

	/*
	 *	Compute the file size
	 *	in bytes.
	 */
	for ( i = 0; i < nLines; i++ )
	{
		/*
		 *	Add line length.
		 */
		nBytes += (( LPLINE )Brainchild_ArrayGetAt( lpcd->lpLines, i ))->nLength;

		/*
		 *	Add line terminator bytes
		 *	if this is not the last line.
		 */
		if ( i < nLines - 1 )
			nBytes += nLineTerm;
	}

	/*
	 *	Format and set size string.
	 */
#if (_MSC_VER < 1500)
	_stprintf( szBuffer, GetString( IDS_FILE_SIZE ), nBytes, nLines );
#else
	_stprintf_s( szBuffer, _countof( szBuffer ), GetString( IDS_FILE_SIZE ), nBytes, nLines );
#endif   // _MSC_VER
	SetDlgItemText( hDlg, IDC_PROP_SIZE, szBuffer );
}

/*
 *	Default data.
 */
int nFileMode = 0, nTabSize = 0;

static BOOL CALLBACK PropProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case	WM_INITDIALOG:
		{
			LPCLASSDATA	lpcd = ( LPCLASSDATA )lParam;
			HDC		hDC;
			RECT		rcClient;
			HWND		hControl;
			HFONT		hFont, hOFont;
			TCHAR		szBuffer[ MAX_PATH ];
			int		i;

			/*
			 *	Save data.
			 */
			nFileMode = Parser->nFileMode;
			nTabSize  = Parser->nTabSize;

			/*
			 *	Setup class data pointer as
			 *	the dialog user data.
			 */
			SetWindowLong( hDlg, DWL_USER, ( LONG )lpcd );

			/*
			 *	Get handle to the file name control.
			 */
			hControl = GetDlgItem( hDlg, IDC_PROP_FILENAME );

			/*
			 *	Copy the file name into
			 *	a buffer.
			 */
			_tcsncpy( szBuffer, lpcd->szFileName, MAX_PATH );

			/*
			 *	Get the client rectangle of
			 *	the file name control.
			 */
			GetClientRect( hControl, &rcClient );

			/*
			 *	Get the device context.
			 */
			if (( hDC = GetDC( hControl )) != NULL )
			{
				/*
				 *	Get the dialog font.
				 */
				if (( hFont = ( HFONT )SendMessage( hDlg, WM_GETFONT, 0, 0 )) != NULL )
				{
					/*
					 *	Make sure we use the correct font.
					 */
					hOFont = SelectObject( hDC, hFont );

					/*
					 *	Compact the path.
					 */
					PathCompactPath( hDC, szBuffer, rcClient.right );

					/*
					 *	Reset the font.
					 */
					SelectObject( hDC, hOFont );
				}
				/*
				 *	Release the device context.
				 */
				ReleaseDC( hControl, hDC );
			}

			/*
			 *	Set the path.
			 */
			SetWindowText( hControl, szBuffer );

			/*
			 *	Unicode?
			 */
			SetDlgItemText( hDlg, IDC_PROP_UNICODE, GetString( lpcd->bUnicode ? IDS_TRUE : IDS_FALSE ));

			/*
			 *	Setup the up/down range
			 *	and the tab stop.
			 */
			SendDlgItemMessage( hDlg, IDC_PROP_TABSPIN, UDM_SETRANGE, 0, MAKELPARAM( 24,2 ));

			/*
			 *	Setup file modes.
			 */
			for ( i = FMODE_MSDOS; i <= FMODE_MACINTOSH; i++ )
				SendDlgItemMessage( hDlg, IDC_PROP_TYPE, CB_ADDSTRING, 0, ( LPARAM )lpszFileModes[ i ] );

			/*
			 *	Setup the file size and the
			 *	tab size controls.
			 */
			SetSizeControls( hDlg );

			/*
			 *	Valid file date?
			 */
			if ( lpcd->ftFileTime.dwHighDateTime == 0 && lpcd->ftFileTime.dwLowDateTime == 0 )
				SetDlgItemText( hDlg, IDC_PROP_DATE, GetString( IDS_NOT_SAVED ));
			else
			{
				FILETIME	ft;
				SYSTEMTIME	st;

				/*
				 *	Convert file time to
				 *	system time.
				 */
				FileTimeToLocalFileTime( &lpcd->ftFileTime, &ft );
				FileTimeToSystemTime( &ft, &st );

				/*
				 *	Format time.
				 */
				GetTimeFormat( LOCALE_USER_DEFAULT,
					       0, //LOCALE_NOUSEROVERRIDE,
					       &st,
					       NULL,
					       szBuffer,
					       256 );

				/*
				 *	Add space.
				 */
				_tcscat( szBuffer, _T(" "));

				/*
				 *	Format date.
				 */
				GetDateFormat( LOCALE_USER_DEFAULT,
					       0, //LOCALE_NOUSEROVERRIDE,
					       &st,
					       NULL,
					       &szBuffer[ _tcslen( szBuffer ) ],
					       256 - _tcslen( szBuffer ));

				/*
				 *	Output result.
				 */
				SetDlgItemText( hDlg, IDC_PROP_DATE, szBuffer );
			}

			/*
			 *	Focus the OK button.
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
		{
			LPCLASSDATA	lpcd = ( LPCLASSDATA )GetWindowLong( hDlg, DWL_USER );

			switch ( LOWORD( wParam ))
			{
				case	IDC_PROP_TYPE:
					/*
					 *	Get file mode.
					 */
					Parser->nFileMode = SendDlgItemMessage( hDlg, IDC_PROP_TYPE, CB_GETCURSEL, 0, 0 );

					/*
					 *	Send status message.
					 */
					SendStatusMessage( lpcd );
					break;

				case	IDC_PROP_TABSIZE:
				{
					/*
					 *	Lost focus on the edit control?
					 */
					if ( HIWORD( wParam ) == EN_KILLFOCUS )
					{
						/*
						 *	Get value.
						 */
						int	nTabStop = GetDlgItemInt( hDlg, IDC_PROP_TABSIZE, NULL, FALSE );

						/*
						 *	Clamp entered value between 2 and 24.
						 */
						if (      nTabStop < 2  ) nTabStop = 2;
						else if ( nTabStop > 24 ) nTabStop = 24;

						/*
						 *	Set this value to the 
						 *	edit control.
						 */
						SetDlgItemInt( hDlg, IDC_PROP_TABSIZE, ( UINT )nTabStop, FALSE );

						/*
						 *	Save tab value.
						 */
						Parser->nTabSize = nTabStop;

						/*
						 *	Re-render if it changed...
						 */
						if ( nTabStop != nTabSize )
							InvalidateRect( lpcd->hWnd, NULL, FALSE );
					}
					break;
				}

				case	IDCANCEL:
					/*
					 *	Reset tabsize.
					 */
					if ( Parser->nTabSize != nTabSize )
					{
						Parser->nTabSize = nTabSize;
						InvalidateRect( lpcd->hWnd, NULL, FALSE );
					}

					/*
					 *	And file mode.
					 */
					if ( Parser->nFileMode != nFileMode )
					{
						Parser->nFileMode = nFileMode;
						SendStatusMessage( lpcd );
					}

					/*
					 *	Fall through...
					 */

				case	IDOK:
					/*
					 *	Bye...
					 */
					EndDialog( hDlg, 0 );
					break;
			}
		}
	}
	return FALSE;
}

void PropertiesDialog( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Clear qualifiers.
	 */
	lpcd->cQualifier = 0;

	/*
	 *	Popup the dialog.
	 */
	if ( lParam != 0 )
	{
		DialogBoxIndirectParam( hDLL, ( LPCDLGTEMPLATE )lParam, lpcd->hWnd, &PropProc, ( LPARAM )lpcd );
	}
	else {
		DialogBoxParam( hDLL, MAKEINTRESOURCE( IDD_PROPERTIES ), lpcd->hWnd, &PropProc, ( LPARAM )lpcd );
	}
}

LRESULT OnPropDialog( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Popup the properties dialog.
	 */
	PropertiesDialog( lParam, lpcd );
	return 0;
}
