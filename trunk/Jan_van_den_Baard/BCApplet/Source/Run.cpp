//
//	run.cpp
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

// Construction.
Run::Run()
{
}

// Destruction.
Run::~Run()
{
}

// WM_INITDIALOG handler.
LRESULT Run::OnInitDialog( LPPROPSHEETPAGE pPsp )
{
	// Setup controls.
	m_Run.Attach( GetDlgItemHandle( IDC_COMMAND ));
	m_Args.Attach( GetDlgItemHandle( IDC_ARGS ));

	// Split up the string.
	SplitRunCommand();

	// Setup GUI.
	if ( m_pCNode->nType == CTYPE_RUN ) 
	{
		m_Run.Filters().LoadString( IDS_EXEC_FILTER );
		::SetWindowText( GetDlgItemHandle( IDC_RUN ), _T( "&Run:" ));
		SetWindowText( ClsString( MAKEINTRESOURCE( IDS_RUN_TITLE )));
	}
	else	
	{
		m_Run.Filters().LoadString( IDS_ALL_FILTER );
		::SetWindowText( GetDlgItemHandle( IDC_RUN ), _T( "O&pen:" ));
		SetWindowText( ClsString( MAKEINTRESOURCE( IDS_SHELLOPEN_TITLE )));
		m_Args.EnableWindow( FALSE );
		::EnableWindow( GetDlgItemHandle( IDC_STATIC_ARGS ), FALSE );
	}

	return TRUE;
}

// Split command/args.
void Run::SplitRunCommand()
{
	// Allocate buffer.
	LPTSTR pszTmp = m_pCNode->pcStr;
	LPTSTR pszBuf = ( LPTSTR )::AllocPooled( pParserPool, ( _tcslen( pszTmp ) + 1 ) * sizeof( TCHAR ));
	if ( pszBuf != NULL )
	{
		// Skip blanks.
		while ( _istspace( *pszTmp )) pszTmp++;

		// Command part seperated by
		// double quotes?
		if ( *pszTmp == _T( '"' ))
		{
			// Skip quote.
			pszTmp++;

			// Copy command string.
			int i = 0;
			while ( *pszTmp != _T( '"' ) && *pszTmp != _T( '\0' ))
				pszBuf[ i++ ] = *pszTmp++;

			// 0-terminate.
			pszBuf[ i ] = 0;

			// Set it up.
			m_Run.SetWindowText( pszBuf );

			// Run command?
			if ( m_pCNode->nType == CTYPE_RUN )
			{
				// Skip to the arguments.
				if ( *pszTmp++ != _T( '\0' ))
				{
					// Reset index counter.
					i = 0;

					// Skip blanks.
					while ( _istspace( *pszTmp )) pszTmp++;

					// Copy arguments.
					while ( *pszTmp != _T( '\0' ) && *pszTmp != _T( '\r' ) && *pszTmp != _T( '\n' ))
						pszBuf[ i++ ] = *pszTmp++;

					// 0-terminate.
					pszBuf[ i ] = 0;

					// Set it up.
					m_Args.SetWindowText( pszBuf );
				}
			}
		}
		else
			// Only a command.
			m_Run.SetWindowText( pszTmp );

		// Free buffer.
		::FreePooled( pParserPool, pszBuf );
	}
	else
		// Error...
		MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
}

LRESULT Run::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What the ID?
	switch ( nCtrlID )
	{
		case	IDC_COMMAND:
			// Did the contents change while the control
			// has focus?
			if ( nNotifyCode == EN_CHANGE && GetFocus() == &m_Run )
				// Any text present?
				::EnableWindow( GetDlgItemHandle( IDC_OK ), ( BOOL )( m_Run.GetWindowTextLength() > 0 ));
			return 0;

		case	IDC_CANCEL:
			// Cancel...
			EndDialog( FALSE );
			return 0;

		case	IDC_OK:
		{
			int	nCommand = m_Run.GetWindowTextLength();
			int	nArgs	 = m_Args.GetWindowTextLength();
			BOOL	bRC = FALSE;

			// Allocate string.
			LPTSTR pszCommand = ( LPTSTR )::AllocPooled( pParserPool, ( nCommand + nArgs + 4 ) * sizeof( TCHAR ));
			if ( pszCommand != NULL )
			{
				// Quote command string.
				if ( m_pCNode->nType == CTYPE_RUN )
				{
					_tcscpy( pszCommand, _T( "\"" ));
					m_Run.GetWindowText( pszCommand + 1, nCommand + 1 );
					_tcscat( pszCommand, _T( "\" " ));

					// Add arguments.
					if ( m_pCNode->nType == CTYPE_RUN )
						m_Args.GetWindowText( pszCommand + 3 + nCommand, nArgs + 1 );
				}
				else
					m_Run.GetWindowText( pszCommand, nCommand + 1 );
				
				// Free old contents.
				::FreePooled( pParserPool, m_pCNode->pcStr );

				// Set new contents.
				m_pCNode->pcStr = pszCommand;

				// Success.
				bRC = TRUE;
			}
			else
				// Error.
				MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
			
			// Bye...
			EndDialog( bRC );
			return 0;
		}
	}
	// Pass onto the base class.
	return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// Open the hardcoded command selector.
BOOL Run::Edit( ClsWindow& Parent, LPCNODE pCNode )
{
	// Save node.
	m_pCNode = pCNode;

	// Open the dialog.
	return DoModal( Parent, IDD_RUN );
}
