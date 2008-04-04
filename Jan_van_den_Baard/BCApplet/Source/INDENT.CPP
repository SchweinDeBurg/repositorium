//
//	indent.cpp
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
IndentPage::IndentPage() 
{
	// Setup icon.
	m_nIcon = IDI_INDENTION;
}

// Destructor.
IndentPage::~IndentPage()
{
}

// Refresh page contents.
void IndentPage::RefreshData( LPPARSER pParser )
{
	_ASSERT_VALID( pParser );

	// Save parser.
	m_pParser = pParser;

	// Setup checkboxes.
	m_Auto.SetCheck( pParser->bAutoIndent );
	m_Smart.SetCheck( pParser->bSmartIndent );

	// Setup tab-size.
	SetDlgItemInt( IDC_TABSIZE, pParser->nTabSize, FALSE );

	// Clear list and edit control.
	m_List.ResetContent();
	m_Edit.SetWindowText( NULL );

	// Add nodes to the list.
	m_List.SetParser( pParser );
	m_List.SetIndentionList( &pParser->lSmartIndention );

	// Enable/Disable controls.
	SetupControls();
}

// Save page/parser contents.
BOOL IndentPage::SavePage( ClsStdioFile *pFile, LPPARSER pParser )
{
	try
	{
		LPSMART		lpSmart;

		// Write comment header.
		pFile->PrintF( ClsString( MAKEINTRESOURCE( IDS_COMMENT_INDENTION )));

		// Write booleans.
		pFile->PrintF( _T( "AutoIndent=%lc\n" ), pParser->bAutoIndent ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "SmartIndent=%lc\n" ), pParser->bSmartIndent ? _T( 'Y' ) : _T( 'N' ));

		// Write the tab size.
		pFile->PrintF( _T( "Tab=%ld\n" ), pParser->nTabSize );

		// Iterate nodes.
		for ( lpSmart = pParser->lSmartIndention.lpFirst; lpSmart->lpNext; lpSmart = lpSmart->lpNext )
			// Write command.
			 pFile->PrintF( _T( "Indent%s=%s\n" ), lpSmart->nDirection == INDENT_LEFT ? _T( "Left" ) : _T( "Right" ), lpSmart->lpszSmartString );
	}
	catch ( ClsException& e )
	{
		// Error...
		UNREFERENCED_PARAMETER( e );
		return FALSE;
	}
	return TRUE;
}

// Setup GUI.
void IndentPage::SetupControls()
{
	int	nSel = m_List.GetCurSel(), nMax = m_List.GetCount() - 1;

	// Setup booleans.
	BOOL bSmart  = ( BOOL )( m_pParser->bAutoIndent && m_pParser->bSmartIndent );
	BOOL bSelect = ( BOOL )( bSmart && nSel != LB_ERR );

	// Enable/disable toolbar buttons.
	m_Tools.EnableButton( EditToolbar::ETID_NEW,    bSmart );
	m_Tools.EnableButton( EditToolbar::ETID_DELETE, bSelect );
	m_Tools.EnableButton( EditToolbar::ETID_UP,     ( BOOL )( bSelect && nSel > 0 ));
	m_Tools.EnableButton( EditToolbar::ETID_DOWN,   ( BOOL )( bSelect && nSel < nMax ));

	// Enable/disable controls.
	m_Smart.EnableWindow( m_pParser->bAutoIndent );
	m_List.EnableWindow( bSmart );
	m_Edit.EnableWindow( bSelect );
	m_Left.EnableWindow( bSelect );
	m_Right.EnableWindow( bSelect );
}

// WM_COMMAND handler.
LRESULT IndentPage::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What's up?
	switch ( nCtrlID )
	{
		case	EditToolbar::ETID_UP:
			// Move it up.
			m_List.MoveSelUp();
			SetupControls();
			pSettings->Changed( m_pParser );
			return 0;

		case	EditToolbar::ETID_DOWN:
			// Mode it down.
			m_List.MoveSelDown();
			SetupControls();
			pSettings->Changed( m_pParser );
			return 0;

		case	EditToolbar::ETID_DELETE:
			// Remove selection.
			m_List.RemoveSel();
			SendMessage( WM_COMMAND, MAKEWPARAM( IDC_LIST, LBN_SELCHANGE ), ( LPARAM )m_List.GetSafeHWND());
			pSettings->Changed( m_pParser );
			return 0;

		case	EditToolbar::ETID_NEW:
		{
			// Allocate new node.
			LPSMART	lpSmart = ( LPSMART )::AllocPooled( pParserPool, sizeof( SMART ));
			if ( lpSmart )
			{
				// Allocate a name...
				ClsString str( MAKEINTRESOURCE( IDS_NEWSMART_ENTRY ));
				lpSmart->lpszSmartString = pSettings->StrToPool( str );
				if ( lpSmart->lpszSmartString )
				{
					// Left indent by default.
					lpSmart->nDirection = INDENT_LEFT;

					// Add entry to the list.
					AddTail(( LPLIST )&m_pParser->lSmartIndention, ( LPNODE )lpSmart );

					// And to the list control
					// and select it.
					int nSel = m_List.AddString(( LPCTSTR )lpSmart );
					m_List.SetCurSel( nSel );

					// Update GUI.
					SendMessage( WM_COMMAND, MAKEWPARAM( IDC_LIST, LBN_SELCHANGE ), ( LPARAM )m_List.GetSafeHWND());

					// Changes made...
					pSettings->Changed( m_pParser );
					return 0;
				}
				// Free the node.
				::FreePooled( pParserPool, lpSmart );
			}
			return 0;
		}
		
		case	IDC_TABSIZE:
			// Focus lost?
			if ( nNotifyCode == EN_KILLFOCUS )
			{
				// Get value.
				m_pParser->nTabSize = GetDlgItemInt( IDC_TABSIZE, NULL, FALSE );

				// Clamp entered value between 2 and 24.
				BOOL bBalloon = FALSE;
				if (      m_pParser->nTabSize < 2  ) { bBalloon = TRUE; m_pParser->nTabSize = 2;  }
				else if ( m_pParser->nTabSize > 24 ) { bBalloon = TRUE; m_pParser->nTabSize = 24; }

				// Set this value to the 
				// edit control.
				if ( bBalloon )
				{
					// Value was adjusted.
					ClsRect rc;
					m_Tab.GetWindowRect( rc );
					ScreenToClient( rc );
					ClsBalloonHelp::LaunchBalloon( ClsGetApp()->GetAppTitle(), MAKEINTRESOURCE( IDS_ERROR_TABSIZE ), rc.CenterPoint(), IDI_WARNING, ClsBalloonHelp::unSHOW_INNER_SHADOW | ClsBalloonHelp::unCLOSE_ON_MOUSE_MOVE, this );
					SetDlgItemInt( IDC_TABSIZE, ( UINT )m_pParser->nTabSize, FALSE );

					// Force focus back to the control.
					m_Tab.SetSel( 0, m_Tab.GetWindowTextLength());
					m_Tab.SetFocus();
				}

				// Changes made...
				pSettings->Changed( m_pParser );
			}
			else
				// Contents changed?
				if ( nNotifyCode == EN_CHANGE && ::GetFocus() == m_Tab )
					// Changes made...
					pSettings->Changed( m_pParser );
			return 0;
			
		case	IDC_LIST:
		{
			// Get the current selection data.
			LPSMART	lpSmart = ( LPSMART )m_List.GetItemData( m_List.GetCurSel());
			if ( lpSmart != ( LPSMART )LB_ERR )
			{
				// Set the data.
				m_Edit.SetWindowText( lpSmart->lpszSmartString );

				// Setup radio buttons.
				m_Left.SetCheck( lpSmart->nDirection == INDENT_LEFT  ? BST_CHECKED : BST_UNCHECKED );
				m_Right.SetCheck( lpSmart->nDirection == INDENT_RIGHT ? BST_CHECKED : BST_UNCHECKED );

				// Setup controls.
				SetupControls();
				pSettings->Changed( m_pParser );
			}
			else
			{
				// No selection...
				m_Edit.SetWindowText( NULL );
				SetupControls();
			}
			return 0;
		}

		case	IDC_AUTO:
			// Get value.
			m_pParser->bAutoIndent = ( BOOL )( m_Auto.GetCheck() == BST_CHECKED );
			SetupControls();
			pSettings->Changed( m_pParser );
			break;

		case	IDC_SMART:
			// Get value.
			m_pParser->bSmartIndent = ( BOOL )( m_Smart.GetCheck() == BST_CHECKED );
			SetupControls();
			pSettings->Changed( m_pParser );
			break;

		case	IDC_LEFT:
		case	IDC_RIGHT:
		{
			// Get edit control data.
			LPSMART pSmart = ( LPSMART )m_List.GetItemData( m_List.GetCurSel());
			if ( pSmart != ( LPSMART )LB_ERR )
			{
				// Get value.
				pSmart->nDirection = ( m_Left.GetCheck() == BST_CHECKED ? INDENT_LEFT : INDENT_RIGHT );
				pSettings->Changed( m_pParser );
			}
			return 0;
		}

		case	IDC_EDIT:
		{
			// Get edit control data.
			LPSMART pSmart = ( LPSMART )m_List.GetItemData( m_List.GetCurSel());
			if ( pSmart != ( LPSMART )LB_ERR )
			{
				// Get edit control contents.
				LPTSTR pStr = NULL;
				HandleEditControl( nNotifyCode, nCtrlID, &pStr );
				
				// Focus killed?
				if ( nNotifyCode == EN_KILLFOCUS )
				{
					// Valid pointer?
					if ( pStr == NULL || _tcslen( pStr ) == 0 )
					{
						// No empty strings allowed.
						ClsRect rc;
						m_Edit.GetWindowRect( rc );
						ScreenToClient( rc );
						ClsBalloonHelp::LaunchBalloon( ClsGetApp()->GetAppTitle(), MAKEINTRESOURCE( IDS_ERROR_NOEMPTY ), rc.CenterPoint(), IDI_ERROR, ClsBalloonHelp::unSHOW_INNER_SHADOW | ClsBalloonHelp::unSHOW_CLOSE_BUTTON, this );

						// Just in case.
						if ( pStr ) ::FreePooled( pParserPool, pStr );

						// Restore original contents.
						m_Edit.SetWindowText( pSmart->lpszSmartString );
						m_Edit.SetSel( 0, m_Edit.GetWindowTextLength());
						m_Edit.SetFocus();
					}
					else
					{
						// Delete the old string.
						if ( pSmart->lpszSmartString ) ::FreePooled( pParserPool, pSmart->lpszSmartString );

						// And setup the new.
						pSmart->lpszSmartString = pStr;
						Invalidate();
						pSettings->Changed( m_pParser );
					}
				}
			}
			return 0;
		}
	}
	// Pass to the base class.
	return Page::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// WM_INITDIALOG handler.
LRESULT IndentPage::OnInitDialog( PROPSHEETPAGE * pPropSheetPage )
{
	// Setup toolbar etc.
	m_Tools.Attach( GetDlgItemHandle( IDC_TOOLS ));
	m_Tools.SetupToolbar( TRUE, TRUE, FALSE, FALSE );
	m_Auto.Attach( GetDlgItemHandle( IDC_AUTO ));
	m_Smart.Attach( GetDlgItemHandle( IDC_SMART ));
	m_Left.Attach( GetDlgItemHandle( IDC_LEFT ));
	m_Right.Attach( GetDlgItemHandle( IDC_RIGHT ));
	m_List.Attach( GetDlgItemHandle( IDC_LIST ));
	m_Edit.Attach( GetDlgItemHandle( IDC_EDIT ));
	m_Tab.Attach( GetDlgItemHandle( IDC_TABSIZE ));
	m_TabUpDown.Attach( GetDlgItemHandle( IDC_TABSIZE_SPIN ));

	// Range the up/down control.
	m_TabUpDown.SetRange( 2, 24 );

	// Call the base-class.
	return Page::OnInitDialog( pPropSheetPage );
}
