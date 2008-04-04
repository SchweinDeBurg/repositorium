//
//	files.cpp
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//

#include "stdafx.h"

#include "bcc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor.
FilesPage::FilesPage() 
{
	// Setup icon.
	m_nIcon = IDI_FILES;
}

// Destructor.
FilesPage::~FilesPage()
{
}

// Refresh page contents.
void FilesPage::RefreshData( LPPARSER pParser )
{
	_ASSERT_VALID( pParser );

	// Save parser.
	m_pParser = pParser;

	// Setup the Combo-box.
	m_Mode.SetCurSel( pParser->nFileMode );

	// Setup the check-box.
	m_Backup.SetCheck( pParser->bBackup ? BST_CHECKED : BST_UNCHECKED );

	// Setup the edit control.
	m_Folder.SetWindowText( pParser->pszBackupPath );

	// Setup radiobuttons.
	for ( int i = 0; i < BACKUP_MODES; i++ )
		m_BakMode[ i ].SetCheck( i == pParser->nBackupMode ? BST_CHECKED : BST_UNCHECKED );

	// Enable/disable controls.
	for ( int i = 0; i < BACKUP_MODES; i++ )
		m_BakMode[ i ].EnableWindow( pParser->bBackup );
	m_Folder.EnableWindow(( BOOL )( pParser->bBackup && pParser->nBackupMode == BMODE_FILE_EXT_PATH ));
}

// Save page/parser contents.
BOOL FilesPage::SavePage( ClsStdioFile *pFile, LPPARSER pParser )
{
	try
	{
		// Write file settings comment.
		pFile->PrintF( ClsString( MAKEINTRESOURCE( IDS_COMMENT_FILES )));

		// Write file mode.
		pFile->PrintF( _T( "FileMode=%ld\n" ), pParser->nFileMode );

		// Write backup settings.
		pFile->PrintF( _T( "BackupFiles=%lc\n" ), pParser->bBackup ? _T( 'Y' ) : _T( 'N' ));
		if ( pParser->pszBackupPath && *pParser->pszBackupPath ) pFile->PrintF( _T( "BackupDirectory=%s\n" ), pParser->pszBackupPath );
		pFile->PrintF( _T( "BackupMode=%ld\n" ), pParser->nBackupMode );
	}
	catch ( ClsException& e )
	{
		// Error...
		UNREFERENCED_PARAMETER( e );
		return FALSE;
	}
	return TRUE;
}

// WM_COMMAND handler.
LRESULT FilesPage::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What's the message.
	switch ( nCtrlID )
	{
		case	IDC_DEFAULTMODE:
			// Save new setting.
			if ( nNotifyCode == CBN_SELCHANGE )
			{
				m_pParser->nFileMode = m_Mode.GetCurSel();
				pSettings->Changed( m_pParser );
			}
			return 0;

		case	IDC_BACKUP:
		{
			// Save backup setting.
			m_pParser->bBackup = ( BOOL )( m_Backup.GetCheck() == BST_CHECKED );
			pSettings->Changed( m_pParser );

			// Update GUI.
			for ( int i = 0; i < BACKUP_MODES; i++ )
				m_BakMode[ i ].EnableWindow( m_pParser->bBackup );
			m_Folder.EnableWindow(( BOOL )( m_pParser->bBackup && m_pParser->nBackupMode == BMODE_FILE_EXT_PATH ));
			return 0;
		}
		
		case	IDC_BAK:
		case	IDC_EXTBAK:
		case	IDC_BAKEXT:
		case	IDC_EXTIN:
		{
			// Get new setting.
			CheckRadioButton( IDC_BAK, IDC_EXTIN, nCtrlID );
			
			// Setup new value.
			for ( int i = 0; i < BACKUP_MODES; i++ )
			{
				// Was this the one?
				if ( m_BakMode[ i ].GetWindowLong( GWL_ID ) == ( LONG )nCtrlID )
				{
					// Setup the backup mode.
					m_pParser->nBackupMode = i;
					break;
				}
			}
			// Enable/disable folder control.
			m_Folder.EnableWindow(( BOOL )( nCtrlID == IDC_EXTIN ));

			// Focus the folder control?
			if ( nCtrlID == IDC_EXTIN )
			{
				// Any text?
				int nChars = m_Folder.GetWindowTextLength();

				// Focus the edit control and select it's contents.
				m_Folder.SetFocus();
				m_Folder.SetSel( 0, nChars );

				// Any text? If not we simulate a change in the
				// edit control which makes the balloon popup because
				// the control is empty.
				if ( ! nChars )
					// Simulate typing...
					SendMessage( WM_COMMAND, MAKEWPARAM( IDC_FOLDER, EN_CHANGE ), ( WPARAM )m_Folder.GetSafeHWND());
			}

			// Changes made.
			pSettings->Changed( m_pParser );
			return 0;
		}
		
		case	IDC_FOLDER:
			// Get the data.
			HandleEditControl( nNotifyCode, nCtrlID, &m_pParser->pszBackupPath );

			// Did the contents change while the control
			// has focus?
			if ( nNotifyCode == EN_CHANGE && GetFocus() == &m_Folder )
			{
				// Any text present? Backup in folder option checked?
				if ( m_Folder.GetWindowTextLength() == 0 && m_BakMode[ 3 ].GetCheck() == BST_CHECKED )
				{
					// No empty Path allowed.
					ClsRect rc;
					m_Folder.GetWindowRect( rc );
					ScreenToClient( rc );
					ClsBalloonHelp::LaunchBalloon( ClsGetApp()->GetAppTitle(), MAKEINTRESOURCE( IDS_ERROR_BACKUPPATH ), rc.CenterPoint(), IDI_WARNING, ClsBalloonHelp::unSHOW_INNER_SHADOW | ClsBalloonHelp::unCLOSE_ON_MOUSE_MOVE | ClsBalloonHelp::unCLOSE_ON_KEYPRESS, this );
				}
			}
			return 0;
	}
				
	// Pass to the base class.
	return Page::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// WM_INITDIALOG handler.
LRESULT FilesPage::OnInitDialog( PROPSHEETPAGE * pPropSheetPage )
{
	// Setup static controls.
	m_STDef.Attach( GetDlgItemHandle( IDC_ST_DEF ));
	m_STBack.Attach( GetDlgItemHandle( IDC_ST_BACK ));	
	m_STAs.Attach(  GetDlgItemHandle( IDC_ST_AS ));

	// The checkboxes.
	m_Backup.Attach( GetDlgItemHandle( IDC_BACKUP ));

	// The combo-box.
	m_Mode.Attach( GetDlgItemHandle( IDC_DEFAULTMODE ));

	// Setup combo-box entries.
	m_Mode.InsertString( 0, ClsString( MAKEINTRESOURCE( IDS_DOS )));
	m_Mode.InsertString( 1, ClsString( MAKEINTRESOURCE( IDS_UNIX )));
	m_Mode.InsertString( 2, ClsString( MAKEINTRESOURCE( IDS_MAC )));

	// The radio-buttons.
	m_BakMode[ 0 ].Attach( GetDlgItemHandle( IDC_BAK ));
	m_BakMode[ 1 ].Attach( GetDlgItemHandle( IDC_BAKEXT ));
	m_BakMode[ 2 ].Attach( GetDlgItemHandle( IDC_EXTBAK ));
	m_BakMode[ 3 ].Attach( GetDlgItemHandle( IDC_EXTIN ));

	// The directory browser control.
	m_Folder.Attach( GetDlgItemHandle( IDC_FOLDER ));
	m_Folder.ShowFiles() = FALSE;

	// Call the base-class.
	return Page::OnInitDialog( pPropSheetPage );
}
