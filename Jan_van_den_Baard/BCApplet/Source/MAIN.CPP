//
//	main.cpp
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Main dialog.
//

#include "stdafx.h"

#include "bcc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor.
SettingsDialog::SettingsDialog() 
{
	// Default data.
	m_hLast		= m_hAbout = NULL;
	m_dwAddCounter	= 0;
}

// Destructor.
SettingsDialog::~SettingsDialog()
{
}

// Read the location of the Brainchild
// settings directory.
BOOL SettingsDialog::GetSettingsLocation()
{
	ClsRegKey	reg;

	// Open the registry key under which the
	// brainchild settings directory is specified.
	if ( reg.OpenKey( HKEY_LOCAL_MACHINE, _T( "Software\\BCDLL\\CurrentVersion" )) == ERROR_SUCCESS )
	{
		TCHAR	sz[ MAX_PATH ];
		DWORD	dwSize = MAX_PATH;

		// Read the "Settings" value.
		if ( reg.QueryValue( _T( "Settings" ), ( LPTSTR )sz, &dwSize ) == ERROR_SUCCESS )
		{
			// Copy the string and add a backslash 
			// if necessary.
			m_strSettings = sz;
			if ( m_strSettings[ m_strSettings.GetStringLength() - 1 ] != _T( '\\' ))
				m_strSettings += _T( '\\' );

			// Obtain the name of the current user.
			dwSize = MAX_PATH;
			if ( GetUserName( sz, &dwSize ))
			{
				// Append the user name to the settings
				// location.
				m_strSettings += sz;
				return TRUE;
			}
			else
			{
				// Use the default settings...
				m_strSettings += _T( "Defaults" );
				return TRUE;
			}
		}
	}
	return FALSE;
}

// A setting was changed.
void SettingsDialog::Changed( LPPARSER pParser )
{
	// Enable the Apply button.
	::EnableWindow( GetDlgItemHandle( IDC_APPLY ), TRUE );

	// Mark the parser as being changed.
	pParser->bPrefsChanged = TRUE;
}

/*
 *	Set changes.
 */
BOOL SettingsDialog::SetChanges()
{
	BOOL bChangesWritten = FALSE;
	BOOL bContinue = FALSE;

	// Get current directory.
	TCHAR szCurrent[ MAX_PATH ];
	if ( ::GetCurrentDirectory( MAX_PATH, szCurrent ))
	{
		// Get the destination path.
		TCHAR szPath[ MAX_PATH ], szBackup[ MAX_PATH ];
		if ( ::Brainchild_CheckUser( szPath ))
		{
			// Create a backup path.
			_tcscpy( szBackup, szPath );
			_tcscat( szBackup, _T( "\\Backup" ));

			// Create it.
			if ( ::CreateDirectory( szBackup, NULL ) == FALSE )
			{
				// Does it already exist?
				if ( GetLastError() != ERROR_ALREADY_EXISTS )
				{
					// Ask if the user wishes to continue.
					// anyway.
					ClsMessageBox mb;
					mb.Title() = ClsGetApp()->GetAppTitle();
					mb.Buttons().LoadString( IDS_YESNO );
					mb.Body().LoadString( IDS_NO_BACKUP );
					mb.Flags() = ClsMessageBox::MBF_ICONEXCLAMATION;
					if ( mb.MsgBox( GetSafeHWND()) == 0 )
						return FALSE;

					// Continue file saving...
					bContinue = TRUE;
				}
			}

			// Go to the destination directory.
			if ( ::SetCurrentDirectory( szPath ))
			{
				// Backup old settings files.
				ClsFindFile find;
				if ( find.FindFile( _T( "*.*" ) ))
				{
					// Copy all files.
					do
					{
						// Construct target name.
						_tcscpy( szBackup, _T( "Backup\\" ));

						// Get the found filename.
						ClsString name;
						if ( find.GetFileName( name ))
						{
							// Append the name.
							_tcscat( szBackup, name );

							// Do not try and copy the "." and ".." files
							// or directories.
							if ( ! find.IsDots() && ! find.IsDirectory())
							{
								// Copy the file...
								if ( CopyFile( name, szBackup, FALSE ) == FALSE )
								{
									// Ask if the user wishes to continue.
									// anyway.
									if ( bContinue == FALSE )
									{
										ClsMessageBox mb;
										mb.Title() = ClsGetApp()->GetAppTitle();
										mb.Buttons().LoadString( IDS_YESNO );
										mb.Body().LoadString( IDS_NO_BACKUP );
										mb.Flags() = ClsMessageBox::MBF_ICONEXCLAMATION;
										if ( mb.MsgBox( GetSafeHWND()) == 0 )
										{
											SetCurrentDirectory( szCurrent );
											return FALSE;
										}

										// Set continue flag.
										bContinue = TRUE;
									}
								}
							}
						}
					} while ( find.FindNextFile());
				}

				// Iterate the parsers.
				LPPARSER pParser;
				for ( pParser = plParsers.lpFirst; pParser->lpNext; pParser = pParser->lpNext )
				{
					// Changes made?
					if ( pParser->bPrefsChanged == TRUE )
					{
						// We have made changes.
						bChangesWritten = TRUE;

						// No more changes.
						pParser->bPrefsChanged = FALSE;

						// Write the file or delete it?
						if ( pParser->nUndoSize == 0 )
							// Remove the output file.
							::DeleteFile( pParser->pszFileName );
						else
						{
							ClsStdioFile file;
							try
							{
								// Open the file.
								file.Open( pParser->pszFileName, _T( "w" ));

								// Write all pages.
								if ( m_General.SavePage(  &file, pParser ) == FALSE ||
								     m_Printer.SavePage(  &file, pParser ) == FALSE ||
								     m_DateTime.SavePage( &file, pParser ) == FALSE ||
								     m_Files.SavePage(    &file, pParser ) == FALSE ||
								     m_Syntax.SavePage(   &file, pParser ) == FALSE ||
								     m_Indent.SavePage(   &file, pParser ) == FALSE ||
								     m_Keyboard.SavePage( &file, pParser ) == FALSE )
								{
									// Restore directory and show error message.
									::SetCurrentDirectory( szCurrent );
									wsprintf( szPath, ClsString( MAKEINTRESOURCE( IDS_ERROR_FILE )), pParser->pszFileName );
									MessageBox( szPath, ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
									file.Close();
									return FALSE;
								}
								// Close the file.
								file.Close();
							}
							catch( ClsException& ex )
							{
								UNREFERENCED_PARAMETER( ex );
								// Restore directory and show error message.
								::SetCurrentDirectory( szCurrent );
								wsprintf( szPath, ClsString( MAKEINTRESOURCE( IDS_ERROR_FILE )), pParser->pszFileName );
								MessageBox( szPath, ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
								file.Close();
								return FALSE;
							}
						}
					}
				}
			}
		}

		// Did we actually saved
		// any changes?
		if ( bChangesWritten )
		{
			// Tell'm we are changing the
			// settings.
			ClsDialog dlg;
			dlg.Create( MAKEINTRESOURCE( IDD_APPLY ));

			// Signal settings changes.
			::SendMessage( HWND_BROADCAST, ::Brainchild_GetUpdateMessage(), 0, 0 );
		}

		// Reset directory.
		::SetCurrentDirectory( szCurrent );
		return TRUE;
	}
	return FALSE;
}

// Add a language to the tree.
BOOL SettingsDialog::AddLanguage( LPCTSTR pszName, BOOL bFirst /* = FALSE */ )
{
	// First we setup the string by copying it or
	// loading it from the resources.
	ClsString	str( pszName );

	// Create the item structure.
	TVINSERTSTRUCT	tvs;
	tvs.hParent		= TVI_ROOT;
	tvs.hInsertAfter	= bFirst ? TVI_FIRST : ( m_hLast ? m_hLast : TVI_LAST );
	tvs.item.mask		= TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvs.item.pszText	= str;
	tvs.item.lParam		= ( LPARAM )&m_General;
	tvs.item.iImage		= 2;
	tvs.item.iSelectedImage = 3;

	// Add the language item itself.
	HTREEITEM hParent;
	if (( hParent = m_Tree.InsertItem( tvs )) != NULL )
	{
		// Make it the last added.
		if ( ! bFirst ) 
			m_hLast = hParent;

		// Add child pages.
		if ( m_Tree.AddStringItem( MAKEINTRESOURCE( IDS_PAGE_SYNTAX   ), &m_Syntax,   hParent, FALSE ) &&
		     m_Tree.AddStringItem( MAKEINTRESOURCE( IDS_PAGE_DATETIME ), &m_DateTime, hParent, FALSE ) &&
		     m_Tree.AddStringItem( MAKEINTRESOURCE( IDS_PAGE_FILES    ), &m_Files,    hParent, FALSE ) &&
		     m_Tree.AddStringItem( MAKEINTRESOURCE( IDS_PAGE_PRINTER  ), &m_Printer,  hParent, FALSE ) &&
		     m_Tree.AddStringItem( MAKEINTRESOURCE( IDS_PAGE_INDENT   ), &m_Indent,   hParent, FALSE ) &&
		     m_Tree.AddStringItem( MAKEINTRESOURCE( IDS_PAGE_KEYBOARD ), &m_Keyboard, hParent, FALSE ))
			return TRUE;
	}
	return FALSE;
}

// Really exit without saving?
BOOL SettingsDialog::ReallyExit()
{
	LPPARSER pParser;
	ClsString parsers(_T(""));

	for ( pParser = plParsers.lpFirst; pParser->lpNext; pParser = pParser->lpNext )
	{
		if ( pParser->bPrefsChanged )
		{
			parsers += pParser->pszTitle;
			parsers += _T( "\n" );
		}
	}

	if ( parsers.GetStringLength())
	{
		// Are you sure?
		ClsMessageBox mb;
		mb.Title() = ClsGetApp()->GetAppTitle();
		mb.Buttons().LoadString( IDS_YESAPPLYNO );
		mb.Flags() = ClsMessageBox::MBF_ICONEXCLAMATION;


		// Format the body.
		mb.Body().Format( ClsString( MAKEINTRESOURCE( IDS_REALLY_EXIT )), ( LPCTSTR )parsers );

		// Popup the dialog.
		switch ( mb.MsgBox( GetSafeHWND()))
		{
			case	2:
				// Apply changes and exit...
				SetChanges();

			case	1:
				// Exit...
				return TRUE;
		}
		// Don't exit.
		return FALSE;
	}
	// There where no changes.
	return TRUE;
}

// Initialization.
LRESULT SettingsDialog::OnInitDialog( PROPSHEETPAGE *pPropSheetPage )
{
	// Setup toolbar.
	m_Tools.Attach( GetDlgItemHandle( IDC_TOOLS ));
	m_Tools.SetupToolbar( TRUE, FALSE, FALSE, FALSE );
	
	// Setup the bar.
	m_BarFont.CreatePointFont( _T( "Tahoma" ), 13, 0 );
	m_Bar.Attach( GetDlgItemHandle( IDC_PAGEBAR ));
	m_Bar.SetFont( &m_BarFont );
	m_Bar.IconRight() = FALSE;
	m_Bar.Gradient() = TRUE;

	// Setup the treeview.
	m_Tree.Attach( GetDlgItemHandle( IDC_TREE ));
	m_Tree.SetBar( &m_Bar );

	// Setup the dialog icon.
	SetIcon( ClsGetApp()->GetIcon( TRUE ), FALSE );

	// Locate the brainchild settings files
	// of the current user.
	if ( GetSettingsLocation() == FALSE )
	{
		// Show error.
		MessageBox( ClsString( MAKEINTRESOURCE( IDS_SETTINGS_LOCATION )), ClsGetApp()->GetAppTitle());
		EndDialog( 0 );
	}

	// Create pages.
	m_About.SetIcon( IDI_ABOUT );
	if (( m_About.Create(    *this, MAKEINTRESOURCE( IDD_ABOUT    )) == FALSE ) ||
	    ( m_Printer.Create(  *this, MAKEINTRESOURCE( IDD_PRINTING )) == FALSE ) ||
	    ( m_General.Create(  *this, MAKEINTRESOURCE( IDD_GENERAL  )) == FALSE ) ||
	    ( m_Files.Create(    *this, MAKEINTRESOURCE( IDD_FILES    )) == FALSE ) ||
	    ( m_Syntax.Create(   *this, MAKEINTRESOURCE( IDD_SYNTAX   )) == FALSE ) ||
	    ( m_Indent.Create(   *this, MAKEINTRESOURCE( IDD_INDENT   )) == FALSE ) ||
	    ( m_Keyboard.Create( *this, MAKEINTRESOURCE( IDD_KEYBOARD )) == FALSE ) ||
	    ( m_DateTime.Create( *this, MAKEINTRESOURCE( IDD_DATETIME )) == FALSE ))
	{
		// Bye...
		MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_PAGE )), ClsGetApp()->GetAppTitle());
		EndDialog( 0 );
		return FALSE;
	}

	// Add the laguages to the tree.
	LPPARSER pParser;
	for ( pParser = ( LPPARSER )plParsers.lpFirst; pParser->lpNext; pParser = pParser->lpNext )
	{
		// Add the language.
		if ( AddLanguage( pParser->pszTitle, pParser->bDefault ) == FALSE )
		{
			// Bye...
			MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_ITEM )), ClsGetApp()->GetAppTitle());
			EndDialog( 0 );
			return FALSE;
		}
	}

	// Add the about page to the tree.
	if (( m_hAbout = m_Tree.AddStringItem( MAKEINTRESOURCE( IDS_ABOUT ), &m_About, TVI_ROOT, FALSE )) == NULL )
	{
		// Error...
		MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_ITEM )), ClsGetApp()->GetAppTitle());
		EndDialog( 0 );
		return FALSE;
	}

	// Focus the tree and select the first item
	// in the tree.
	m_Tree.SelectItem( m_Tree.GetRoot());
	m_Tree.Expand( m_Tree.GetSelection(), TVE_EXPAND );
	m_Tree.SetFocus();

	// Call the base.
	ClsDialog::OnInitDialog( pPropSheetPage );

	// We have already focussed the tree.
	return FALSE;
}

// Notification handler.
LRESULT SettingsDialog::OnNotify( LPNMHDR pHdr )
{
	// Tree selection changed?
	if ( pHdr->code == TVN_SELCHANGED )
	{
		// We can only delete if:
		//
		// 1) There is a parser selected.
		// 2) It is not the default parser.
		// 3) The parser itself is selected, not
		//    any of it's sub-pages.
		LPPARSER pParser = m_Tree.GetCurrParser();
		if ( pParser && pParser->bDefault == FALSE && ( m_Tree.GetParentItem( m_Tree.GetSelection()) == NULL ))
			// We can be deleted.
			m_Tools.EnableButton( EditToolbar::ETID_DELETE, TRUE );
		else
			// We cannot be deleted.
			m_Tools.EnableButton( EditToolbar::ETID_DELETE, FALSE );
	}
	// The base class.
	return ClsDialog::OnNotify( pHdr );
}

// Copy the contents of a ClsString object to the
// memory pool.
LPTSTR SettingsDialog::StrToPool( ClsString& str )
{
	LPTSTR pStr = ( LPTSTR )::Brainchild_AllocPooled( pParserPool, str.GetStringLength() + 1 );
	if ( pStr )
		_tcscpy( pStr, str );
	return pStr;
}

// Command handler.
LRESULT SettingsDialog::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What's up doc...
	switch ( nCtrlID )
	{
		case	IDC_APPLY:
			// Save changes.
			SetChanges();
			::EnableWindow( GetDlgItemHandle( IDC_APPLY ), FALSE );
			return 0;

		case	IDC_OK:
			// Save changes...
			SetChanges();
			EndDialog(0);
			return 0;

		case	IDC_CANCEL:
			if ( ReallyExit())
				EndDialog( 0 );
			return 0;

		case	EditToolbar::ETID_NEW:
		{
			// Get a file name.
			ClassName cn;
			ClsString name;
			if ( cn.Name( *this, name ))
			{
				// Allocate a parser node.
				LPPARSER pParser = ( LPPARSER )::Brainchild_AllocPooled( pParserPool, sizeof( PARSER ));
				if ( pParser )
				{
					// Allocate a block array.
					pParser->lpaBlocks = ::Brainchild_ArrayCreate( 0, 5, sizeof( BLOCK ));
					if ( pParser->lpaBlocks )
					{
						// Default the parser.
						::Brainchild_DefaultParser( pParser );

						// Setup the file name.
						pParser->pszFileName = StrToPool( name );
						if ( pParser->pszFileName )
						{
							// Date string.
							name = _T( "dddd',' dd MMMM yyyy" );
							pParser->pszDateFormat = StrToPool( name );
							if ( pParser->pszDateFormat )
							{
								// Time format.
								name = _T( "HH':'mm':'ss" );
								pParser->pszTimeFormat = StrToPool( name );
								if ( pParser->pszTimeFormat )
								{
									// Setup keyboard.
									if ( ::Brainchild_SetupHash( pParserPool, &pParser->aHash[ 0 ] ))
									{
										// Give it a name.
										name.Format( ClsString( MAKEINTRESOURCE( IDS_NEWCLASS )), m_dwAddCounter++ );
										pParser->pszTitle = StrToPool( name );
										if ( pParser->pszTitle )
										{
											// Add it to the list.
											AddTail(( LPLIST )&plParsers, ( LPNODE )pParser );

											// And to the tree.
											if ( AddLanguage( pParser->pszTitle, FALSE ))
											{												
												// Select it.
												m_Tree.SelectItem( m_hLast );
												m_Tree.SetFocus();
												
												// Changes are made.
												Changed( pParser );

												// Enable delete button.
												m_Tools.EnableButton( EditToolbar::ETID_DELETE, TRUE );
												return 0;
											}
										}
									}
								}
							}
						}
					}
					// Free the parser node.
					::Brainchild_FreeParserNode( pParserPool, pParser );
				}
			}
			else
				return 0;
			MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle());			
			return 0;
		}
			
		case	EditToolbar::ETID_DELETE:
		{
			// Are you sure?
			ClsMessageBox mb;
			mb.Title() = ClsGetApp()->GetAppTitle();
			mb.Buttons().LoadString( IDS_YESNO );
			mb.Flags() = ClsMessageBox::MBF_ICONEXCLAMATION;


			// Format the body.
			TVITEM tvi;
			TCHAR sz[ 256 ];
			tvi.hItem = m_Tree.GetSelection();
			tvi.pszText = sz;
			tvi.cchTextMax = 256;
			tvi.mask = TVIF_TEXT;
			m_Tree.GetItem( tvi );
			mb.Body().Format( ClsString( MAKEINTRESOURCE( IDS_SURE )), sz );

			// Popup the dialog.
			if ( mb.MsgBox( GetSafeHWND()))
			{
				// Obtain the parser to be deleted.
				LPPARSER pParser = m_Tree.GetCurrParser();
				_ASSERT_VALID( pParser );

				// Remove the selected entry.
				m_Tree.DeleteItem( m_Tree.GetSelection());

				// Setting the undo size to 0
				// is the mark for the file to be
				// deleted.
				pParser->nUndoSize = 0;

				// Destroy the filename.
				_tcscpy( pParser->pszFileName, _T( "\0" ));

				// Select the first item.
				m_Tree.SelectItem( m_Tree.GetRoot());

				// Now we must reset the m_hLast item in case
				// it was deleted.
				m_hLast = m_Tree.GetPrevSibling( m_hAbout );
				
				// Changes made...
				Changed( pParser );
			}
			break;
		}
	}
	// Pass to the base class.
	return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}
