//
//	keywords.cpp
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

#define CS_FORE		0x1000
#define CS_BACK		0x1001

// Construction.
KeywordEdit::KeywordEdit()
{
}

// Destruction.
KeywordEdit::~KeywordEdit()
{
}

// Setup GUI.
void KeywordEdit::SetupControls()
{
	// Get list selection.
	int nSel = m_List.GetCurSel();

	// Disable/enable GUI elements.
	m_Tools.EnableButton( EditToolbar::ETID_DELETE,  ( BOOL )( nSel != LB_ERR ));
	m_Tools.EnableButton( EditToolbar::ETID_COLOR,   ( BOOL )( nSel != LB_ERR ));
	m_Tools.EnableButton( EditToolbar::ETID_BGCOLOR, ( BOOL )( nSel != LB_ERR ));
	m_Keys.EnableButton(  EditToolbar::ETID_EDIT,    ( BOOL )( nSel != LB_ERR ));
	m_Keys.EnableButton(  EditToolbar::ETID_INSERT,  ( BOOL )( nSel != LB_ERR ));
}

// WM_INITDIALOG handler.
LRESULT KeywordEdit::OnInitDialog( LPPROPSHEETPAGE pPsp )
{
	// Setup controls.
	m_Tools.Attach( GetDlgItemHandle( IDC_TOOLS ));
	m_Tools.SetupToolbar( TRUE, FALSE, FALSE, TRUE, TRUE );
	m_Keys.Attach( GetDlgItemHandle( IDC_KEYS ));
	m_Keys.SetupToolbar( FALSE, FALSE, TRUE, FALSE );
	m_List.Attach( GetDlgItemHandle( IDC_LIST ));
	m_KeyList.Attach( GetDlgItemHandle( IDC_WORDS ));
	m_Case.Attach( GetDlgItemHandle( IDC_CASE ));

	// Setup values.
	m_Case.SetCheck( m_bCase ? BST_CHECKED : BST_UNCHECKED );
	m_List.SetKeywordList( &m_KeyLst );
	
	// Setup the GUI.
	SetupControls();
	return TRUE;
}

// Windows procedure...
LRESULT KeywordEdit::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// What's the message?
	switch ( uMsg )
	{
		case	WM_CLOSE:
			// Abort...
			EndDialog( FALSE );
			return 0;
			
		case	CPN_SELENDOK:
		{
			// Get the current selection.
			int nSel = m_List.GetCurSel();
			_ASSERT( nSel != LB_ERR );

			// Obtain pointer to the data.
			LPKEYWORDS pKW = ( LPKEYWORDS )m_List.GetItemData( nSel );
			if ( pKW != ( LPKEYWORDS )LB_ERR )
			{
				// Setup the selection...
				if ( lParam      == CS_FORE ) pKW->crColor   = wParam;
				else if ( lParam == CS_BACK ) pKW->crBgColor = wParam;
				Invalidate();

				// Change made.
				pSettings->Changed( m_pParser );
			}
			return 0;
		}
	}
	// Baseclass...
	return ClsDialog::WindowProc( uMsg, wParam, lParam );
}

// WM_COMMAND message handler.
LRESULT KeywordEdit::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What's the ID?
	switch ( nCtrlID )
	{
		case	EditToolbar::ETID_EDIT:
		{
			// Valid entry?
			LPKEYWORDS pKW = ( LPKEYWORDS )m_List.GetItemData( m_List.GetCurSel());
			if ( pKW != ( LPKEYWORDS )LB_ERR )
			{
				// Edit the keywords.
				KWEdit kwe;
				kwe.Edit( *this, pKW, this );
			}
			return 0;
		}

		case	EditToolbar::ETID_INSERT:
		{
			// Popup the file dialog.
			ClsFileDialog fd;
			fd.Filters() = _T( "Text files (*.txt)\0*.txt\0All files (*.*)\0*.*\0\0" );
			fd.Caption().LoadString( IDS_INSERT_KEYWORDS );
			
			if ( fd.DoModal( this, NULL, NULL, TRUE, OFN_FILEMUSTEXIST ))
			{
				// Get the selected filename.
				ClsString name;
				fd.GetName( 0, name );

				// Read the file and add the read keywords
				// to the selected node.
				ReadKeywords( name );

				// Make sure the keywords are
				// displayed.
				SendMessage( WM_COMMAND, MAKEWPARAM( IDC_LIST, LBN_SELCHANGE ), ( LPARAM )m_List.GetSafeHWND());
			}
			return 0;
		}

			
		case	EditToolbar::ETID_COLOR:
		{
			// Get the current selection.
			int nSel = m_List.GetCurSel();

			// Only show the popup when there is
			// a valid selection.
			if ( nSel != LB_ERR )
			{
				// Get the position of the selected entry.
				ClsRect rc;
				m_Tools.GetItemRect( 6, rc );
				m_Tools.ClientToScreen( rc );

				// Obtain the entry data.
				LPKEYWORDS pKW = ( LPKEYWORDS )m_List.GetItemData( nSel );
				if ( pKW != ( LPKEYWORDS )LB_ERR )
				{
					// Create the popup. The popup will automatically destroy
					// itself.
					new ClsColorPopup( ClsPoint( rc.Left(), rc.Bottom()), 
							   pKW->crColor, 
							   this,
							   CS_FORE,
							   NULL, 
							   ClsString( MAKEINTRESOURCE( IDS_CUSTOM )), 
							   NULL, 
							   TRUE,
							   FALSE );
				}
			}
			return 0;
		}

		case	EditToolbar::ETID_BGCOLOR:
		{
			// Get the current selection.
			int nSel = m_List.GetCurSel();

			// Only show the popup when there is
			// a valid selection.
			if ( nSel != LB_ERR )
			{
				// Get the position of the selected entry.
				ClsRect rc;
				m_Tools.GetItemRect( 7, rc );
				m_Tools.ClientToScreen( rc );

				// Obtain the entry data.
				LPKEYWORDS pKW = ( LPKEYWORDS )m_List.GetItemData( nSel );
				if ( pKW != ( LPKEYWORDS )LB_ERR )
				{
					// Create the popup. The popup will automatically destroy
					// itself.
					new ClsColorPopup( ClsPoint( rc.Left(), rc.Bottom()), 
							   pKW->crBgColor, 
							   this,
							   CS_BACK,
							   ClsString( MAKEINTRESOURCE( IDS_TRANSPARENT )), 
							   ClsString( MAKEINTRESOURCE( IDS_CUSTOM )), 
							   NULL, 
							   TRUE,
							   FALSE );
				}
			}
			return 0;
		}

		case	IDC_LIST:
		{
			// Selection change?
			if ( nNotifyCode == LBN_SELCHANGE )
			{
				// Valid entry?
				LPKEYWORDS pKW = ( LPKEYWORDS )m_List.GetItemData( m_List.GetCurSel());
				if ( pKW != ( LPKEYWORDS )LB_ERR )
				{
					// Reset words content.
					m_KeyList.ResetContent();

					// Add keywords.
					m_KeyList.SetRedraw( FALSE );
					for ( int i = 0; i < ::Brainchild_ArrayGetSize( pKW->lpaKeywords ); i++ )
						m_KeyList.AddString(( LPCTSTR )*(( LPTSTR * )::Brainchild_ArrayGetAt( pKW->lpaKeywords, i )));
					m_KeyList.SetRedraw();

					// Setup UI.
					SetupControls();
				}
			}
			return 0;
		}

		case	EditToolbar::ETID_DELETE:
		{			
			// Valid entry?
			LPKEYWORDS pKW = ( LPKEYWORDS )m_List.GetItemData( m_List.GetCurSel());
			if ( pKW != ( LPKEYWORDS )LB_ERR )
			{
				ClsMessageBox mb;
				mb.Title() = ClsGetApp()->GetAppTitle();
				mb.Buttons().LoadString( IDS_YESNO );
				mb.Body().LoadString( IDS_KEYWORD_DELETE );
				mb.Flags() = ClsMessageBox::MBF_ICONEXCLAMATION;
				// Sure?
				if ( mb.MsgBox( GetSafeHWND()))
				{
					// Save current selection
					// position.
					int nSel = m_List.GetCurSel();
					int nPos = nSel;

					// Select the next one or
					// the previous one.
					if ( nSel == m_List.GetCount() - 1 ) nSel--;
					else				     nSel++;

					// Select the other item.
					m_List.SetCurSel( nSel );
					
					// Setup UI.
					SetupControls();

					// Clear the list contents
					m_KeyList.ResetContent();

					// Reset selection.
					SendMessage( WM_COMMAND, MAKEWPARAM( IDC_LIST, LBN_SELCHANGE ), ( LPARAM )m_List.GetSafeHWND());

					// Remove the selection.
					m_List.DeleteString( nPos );

					// Remove the node.
					Remove(( LPNODE )pKW );

					// Free the keyword array and the node.
					if ( pKW->lpaKeywords ) ::Brainchild_ArrayDelete( pKW->lpaKeywords );
					::Brainchild_FreePooled( pParserPool, pKW );
				}	
			}
			return 0;
		}

		case	EditToolbar::ETID_NEW:
		{
			// Allocate a new node.
			LPKEYWORDS pKW = ( LPKEYWORDS )::Brainchild_AllocPooled( pParserPool, sizeof( KEYWORDS ));
			if ( pKW )
			{
				// Allocate an array.
				pKW->lpaKeywords = ::Brainchild_ArrayCreate( 0, 500, sizeof( LPTSTR * ));
				if ( pKW->lpaKeywords )
				{
					// Set color.
					pKW->crColor   = ::GetSysColor( COLOR_WINDOWTEXT );
					pKW->crBgColor = CLR_DEFAULT;

					// Add the node to the list.
					AddTail(( LPLIST )&m_KeyLst, ( LPNODE )pKW );

					// Add it to the listview and select it.
					int nPos = m_List.AddString(( LPCTSTR )pKW );
					m_List.SetCurSel( nPos );

					// Clear the keywords list.
					m_KeyList.ResetContent();

					// Setup UI.
					SetupControls();
					return 0;
				}
				else
					::Brainchild_FreePooled( pParserPool, pKW );
			}
			// Error.
			MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
			return 0;
		}

		case	IDC_CASE:
			// Get value.
			m_bCase = ( BOOL )( m_Case.GetCheck() == BST_CHECKED );
			return 0;

		case	IDC_OK:
			// Return OK
			EndDialog( TRUE );
			return 0;

		case	IDC_CANCEL:
			// Cancel.
			EndDialog( FALSE );
			return 0;
	}
	// Pass onto the base class.
	return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// Create a string copy in the
// array memory.
LPTSTR KeywordEdit::StringArrayCopy( LPARRAY lpa, LPTSTR pszString )
{
	// Allocate memory from the
	// array.
	LPTSTR pszCopy = ( LPTSTR )::Brainchild_ArrayAllocMem( lpa, ( _tcslen( pszString ) + 1 ) * sizeof( TCHAR ));
	if ( pszCopy )
		// Copy the string.
		_tcscpy( pszCopy, pszString );
	return pszCopy;
}

// Free a keyword list.
void KeywordEdit::FreeKeyList( LPKEYLIST pKeyList )
{
	// Iterate the nodes.
	LPKEYWORDS	pKey;
	while (( pKey = ( LPKEYWORDS )RemHead(( LPLIST )pKeyList )) != NULL )
	{
		// Free the array.
		if ( pKey->lpaKeywords )
			::Brainchild_ArrayDelete( pKey->lpaKeywords );

		// And the node.
		::Brainchild_FreePooled( pParserPool, pKey );
	}
}

// Free keyword hashes.
void KeywordEdit::FreeKeyHashes()
{
	// Free the keyword hashes.
	for ( int i = 0; i < 256; i++ )
	{
		LPKEYHASH pNext;
		for ( LPKEYHASH pHash = m_pParser->aKeywords[ i ]; pHash; pHash = pNext )
		{
			// Save next pointer.
			pNext = pHash->lpNext;

			// Delete keyword string.
			if ( pHash->pszKeyword ) 
				::Brainchild_FreePooled( pParserPool, pHash->pszKeyword );

			// And the hash entry itself.
			::Brainchild_FreePooled( pParserPool, pHash );
		}
	}
}

// Add a keyword to a node.
BOOL KeywordEdit::AddKeyword2Node( LPKEYWORDS pKeywords, LPTSTR pszKeyword )
{
	// Add the keyword string to the
	// array of this color.
	LPTSTR	pszCopy = StringArrayCopy( pKeywords->lpaKeywords, pszKeyword );
	if ( pszCopy )
	{
		// Add it to the array.
		if ( ::Brainchild_ArrayAdd( pKeywords->lpaKeywords, &pszCopy, 1 ) == TRUE )
			return TRUE;
	}
	return FALSE;
}

// Convert keywords hashes to
// a keyword list.
BOOL KeywordEdit::Hash2List( LPKEYLIST pKeyList, LPPARSER pParser )
{
	BOOL		bAdded = FALSE;

	// Initialize the list.
	NewList(( LPLIST )pKeyList );

	// Iterate the hashes.
	for ( int i = 0; i < 256; i++ )
	{
		LPKEYHASH pHash, pNext;
		for ( pHash = pParser->aKeywords[ i ]; pHash; pHash = pNext )
		{
			// Save next pointer.
			pNext = pHash->lpNext;

			// See if this color has
			// already been added?
			LPKEYWORDS pKey;
			for ( pKey = pKeyList->lpFirst; pKey->lpNext; pKey = pKey->lpNext )
			{
				// Are the colors the same as the
				// new keyword colors?
				if ( pKey->crColor == pHash->crColor && pKey->crBgColor == pHash->crBkColor )
				{
					// Yes. Simply add the keyword string to the
					// array of this color.
					if ( AddKeyword2Node( pKey, pHash->pszKeyword ) == FALSE )
					{
						// Error.
						FreeKeyList( pKeyList );
						return FALSE;
					}

					// Added keyword.
					bAdded = TRUE;
					break;
				}
			}

			// Added keyword?
			if ( bAdded == FALSE )
			{
				// Allocate a new keyword node.
				if (( pKey = ( LPKEYWORDS )::Brainchild_AllocPooled( pParserPool, sizeof( KEYWORDS ))) != NULL )
				{
					// Create a keyword array.
					if (( pKey->lpaKeywords = ::Brainchild_ArrayCreate( 0, 500, sizeof( LPTSTR * ))) != NULL )
					{
						// Setup the node colors.
						pKey->crColor   = pHash->crColor;
						pKey->crBgColor = pHash->crBkColor;

						// Add to the list.
						AddTail(( LPLIST )pKeyList, ( LPNODE )pKey );

						// Add the keyword.
						if ( AddKeyword2Node( pKey, pHash->pszKeyword ) == FALSE )
						{
							// Error.
							FreeKeyList( pKeyList );
							return FALSE;
						} 
					}
					else
					{
						// Error.
						::Brainchild_FreePooled( pParserPool, pKey );
						FreeKeyList( pKeyList );
						return FALSE;
					}
				}
				else
				{
					// Error.
					FreeKeyList( pKeyList );
					return FALSE;
				}
			}
			// Clear added flag.
			bAdded = FALSE;
		}
	}

	// Sort the arrays.
	for ( LPKEYWORDS pKey = pKeyList->lpFirst; pKey->lpNext; pKey = pKey->lpNext )
		::Brainchild_ArraySort( pKey->lpaKeywords, ( COMPFUNC )CompareKeywords );

	return TRUE;
}

// Read keywords from a file.
BOOL KeywordEdit::ReadKeywords( LPCTSTR pszFileName )
{
	// Get item position and data.
	int		nPos = m_List.GetCurSel();
	_ASSERT( nPos != LB_ERR );
	LPKEYWORDS	pKey = ( LPKEYWORDS )m_List.GetItemData( nPos );
	ClsStdioFile	file;

	// Valid node?
	if ( pKey != ( LPKEYWORDS )LB_ERR )
	{
		try
		{
			// Open the file.
			file.Open( pszFileName, _T( "ra" ));

			// Read the file...
			TCHAR szBuffer[ 1024 ], *pszPtr, *pszCopy;
			while (( pszPtr = file.GetS( szBuffer, 1024 )) != NULL )
			{
				// Skip leading blanks.
				while ( _istspace( *pszPtr )) pszPtr++;

				// Also remove trailing
				// blanks.
				RemoveTrailingBlanks( pszPtr );

				// Any text left?
				if ( _tcslen( pszPtr ))
				{
					// Create a copy of the
					// input string.
					if (( pszCopy = StringArrayCopy( pKey->lpaKeywords, pszPtr )) != NULL )
					{
						 // Add it to the array.
						if ( ::Brainchild_ArrayAdd( pKey->lpaKeywords, &pszCopy, 1 ) == FALSE )
						 {
							// Error...
							MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
							break;
						 }
					}
					else
					{
						// Error...
						MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
						break;
					}
				}
			}

			// Close the input file.
			file.Close();
		}
		catch( ClsException& ex )
		{
			UNREFERENCED_PARAMETER( ex );
			return FALSE;
		}
	}
	// Sort the array.
	::Brainchild_ArraySort( pKey->lpaKeywords, ( COMPFUNC )CompareKeywords );
	return TRUE;
}

// Use the edited keyword list.
BOOL KeywordEdit::UseKeywords( LPKEYLIST pKeyList )
{
	// Default the parser.
	PARSER pParser;
	::Brainchild_DefaultParser( &pParser );

	// Now we are going to use this parser to 
	// add the edited keywords to. When this 
	// does not fail we free the old hashes
	// and copy these back to the parser.
	for ( LPKEYWORDS pKey = pKeyList->lpFirst; pKey->lpNext; pKey = pKey->lpNext )
	{
		// Iterate the array.
		for ( int i = 0; i < ::Brainchild_ArrayGetSize( pKey->lpaKeywords ); i++ )
		{
			// Get string pointer.
			LPTSTR	pszString = *(( LPTSTR * )::Brainchild_ArrayGetAt( pKey->lpaKeywords, i ));

			// Add it to the hash table.
			if ( ::Brainchild_AddKeyword( pParserPool, &pParser, pszString, pKey->crColor, pKey->crBgColor ) == FALSE )
			{
				// Free node.
				::Brainchild_FreeParserNode( pParserPool, &pParser);

				// Free the list and fail.
				FreeKeyList( pKeyList );
				return FALSE;
			}
		}
	}

	// Free old hashes.
	FreeKeyHashes();

	// Free the keylist.
	FreeKeyList( pKeyList );

	// Setup the new hashes.
	for ( int i = 0; i < 256; i++ )
		m_pParser->aKeywords[ i ] = pParser.aKeywords[ i ];

	return TRUE;
}

// Remove trailing blanks.
void KeywordEdit::RemoveTrailingBlanks( LPTSTR lpszString )
{
	int	nLength = _tcslen( lpszString ) - 1;

	// Replace all trailing white spaces
	// by a 0 character.
	while ( _istspace( lpszString[ nLength ] ))
		lpszString[ nLength-- ] = _T( '\0' );
}

// Keyword comparisson
// routine.
int _cdecl KeywordEdit::CompareKeywords( LPTSTR *ppStr1, LPTSTR *ppStr2 )
{
	return _tcsicmp( *ppStr1, *ppStr2 );
}

// Open the keysword editor.
void KeywordEdit::Edit( ClsWindow& Parent, LPPARSER pParser )
{
	// Save parser pointer.
	m_pParser = pParser;
	
	// Save current case setting.
	m_bCase = pParser->bCaseOn;

	// Convert the hash table into a list.
	if ( Hash2List( &m_KeyLst, pParser ))
	{
		// Open the dialog.
		if ( DoModal( Parent, IDD_KEYWORDS ) <= 0 )
			// Free the list.
			FreeKeyList( &m_KeyLst );
		else
		{
			// Use the changes.
			if ( UseKeywords( &m_KeyLst ) == FALSE )
				// Error...
				MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
			
			// Save case setting.
			pParser->bCaseOn = m_bCase;
			
			// Changes where made.
			pSettings->Changed( pParser );
		}
	}
	else
		// Error...
		MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
}
