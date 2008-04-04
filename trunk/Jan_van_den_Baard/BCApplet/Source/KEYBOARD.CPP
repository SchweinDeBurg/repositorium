//
//	keyboard.cpp
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
KeyboardPage::KeyboardPage() 
{
	// Setup icon.
	m_nIcon = IDI_KEYBOARD;

	// Load bitmaps.
	m_hImages = ImageList_LoadBitmap( ClsGetResourceHandle(), MAKEINTRESOURCE( IDB_CLIST ), 16, 0, RGB( 255, 0, 255 ));
}

// Destructor.
KeyboardPage::~KeyboardPage()
{
	// Destroy image list.
	if ( m_hImages ) ImageList_Destroy( m_hImages );
}

// Refresh page contents.
void KeyboardPage::RefreshData( LPPARSER pParser )
{
	_ASSERT_VALID( pParser );

	// Save parser.
	m_pParser = pParser;

	// Reset list box.
	m_Keys.SetRedraw( FALSE );
	m_Keys.ResetContent();

	// Go through the key hashes.
	LPHASH lpHash, lpNext;
	TCHAR szKeyString[ 256 ];
	for ( int i = 0; i < HASHSIZE; i++ )
	{
                for ( lpHash = pParser->aHash[ i ]; lpHash; lpHash = lpNext )
		{
			// Pick up next hash in this chain.
			lpNext = lpHash->lpNext;

			// Convert code and qualifier into
			// a string.
			::CodeQual2Str( lpHash->cCode, lpHash->cQual, szKeyString );

			// Add the string to the list.
			m_Keys.AddString( szKeyString );
		}
	}
	m_Keys.SetRedraw();

	// Setup the GUI.
	SetupControls();
}

// Save page/parser contents.
BOOL KeyboardPage::SavePage( ClsStdioFile *pFile, LPPARSER pParser )
{
	try
	{
		// Write comment.
		pFile->PrintF( ClsString( MAKEINTRESOURCE( IDS_COMMENT_KEYS )));

		// Go through the key hashes.
		LPHASH pHash;
		LPCNODE pCNode;
		TCHAR szKeyString[ 64 ];
		for ( int i = 0; i < HASHSIZE; i++ )
		{
			for ( pHash = pParser->aHash[ i ]; pHash; pHash = pHash->lpNext )
			{
				// Convert code and qualifier into
				// a string.
				::CodeQual2Str( pHash->cCode, pHash->cQual, szKeyString );

				// Write the key identifier.
				pFile->PrintF( _T( "Key=%s\n" ), szKeyString );

				// Write the command nodes.
				for ( pCNode = pHash->lpCommands->lpFirst; pCNode->lpNext; pCNode = pCNode->lpNext )
				{
					// What is the command type?
					switch ( pCNode->nType )
					{
						case	CTYPE_HARDCODED:
							// Write command and it's ID.
							pFile->PrintF( _T( "\tHardcoded=%ld\n" ), ::FindCommandID( pCNode->lpFunc ));
							break;

						case	CTYPE_RUN:
							// Write command and it's
							// string.
							pFile->PrintF( _T( "\tRun=%s\n" ), pCNode->pcStr );
							break;

						case	CTYPE_TEXT:
							// Write command and it's
							// string.
							pFile->PrintF( _T( "\tInsertText=%s\n" ), pCNode->pcStr );
							break;

						case	CTYPE_SHELLOPEN:
							// Write the command and it's
							// string.
							pFile->PrintF( _T( "\tOpen=%s\n" ), pCNode->pcStr );
							break;

						case	CTYPE_END:
							break;
					}
				}

				// End the key description.
				pFile->PrintF( _T( "EndKey\n" ) );
			}
		}
	}
	catch ( ClsException& e )
	{
		// Error...
		UNREFERENCED_PARAMETER( e );
		return FALSE;
	}
	return TRUE;
}

// Setup the controls.
void KeyboardPage::SetupControls()
{
	// Get selections.
	int nKeySel = m_Keys.GetCurSel();
	int nComSel = m_Commands.GetCurSel();
	int nCount = m_Commands.GetCount();

	// Any key selected?
	if ( nKeySel == LB_ERR )
	{
		// Clear command list.
		m_Commands.ResetContent();
		nComSel = nCount = LB_ERR;
	}

	m_Commands.EnableWindow(( BOOL )( nKeySel != LB_ERR ));
	m_Tools.EnableButton( EditToolbar::ETID_DELETE, ( BOOL )( nKeySel != LB_ERR ));
	m_ComTools.EnableButton( EditToolbar::ETID_NEW, ( BOOL )( nKeySel != LB_ERR ));
	m_ComTools.EnableButton( EditToolbar::ETID_DELETE, ( BOOL )( nKeySel != LB_ERR && nComSel != LB_ERR ));
	m_ComTools.EnableButton( EditToolbar::ETID_UP, ( BOOL )( nKeySel != LB_ERR && nComSel != LB_ERR && nComSel > 0 ));
	m_ComTools.EnableButton( EditToolbar::ETID_DOWN, ( BOOL )( nKeySel != LB_ERR && nComSel != LB_ERR && nComSel < nCount - 1 ));
}

// WM_MEASUREITEM handler.
LRESULT KeyboardPage::OnMeasureItem( UINT nID, LPMEASUREITEMSTRUCT pMis )
{
	if ( nID == IDC_COMMANDS )
	{
		// Get a device context.
		ClsGetDC dc( this );

		// Select the font.
		ClsFont font;
		GetFont( font );
		ClsSelector sel( &dc, font );

		// Compute the height.
		pMis->itemHeight = dc.GetTextExtent( _T( " " ), 1 ).CY();
		return TRUE;
	}
	return FALSE;
}

// WM_DRAWITEM message handler.
LRESULT KeyboardPage::OnDrawItem( UINT nID, LPDRAWITEMSTRUCT pDis )
{
	if ( nID == IDC_COMMANDS )
	{
		// Render data if valid.
		LPCNODE lpCNode = ( LPCNODE )pDis->itemData;
		if ( lpCNode )
		{
			// Ontain string to render.
			LPCTSTR pszStr = ( LPCTSTR )( lpCNode->nType == CTYPE_HARDCODED ? ::FindCommandDesc( lpCNode->lpFunc ) : lpCNode->pcStr );

			// Images valid?.
			if ( m_hImages )
			{
				// Clear background.
				::FillRect( pDis->hDC, &pDis->rcItem, ( HBRUSH )( COLOR_WINDOW + 1 ));

				// Render the image and make room for
				// it.
				ImageList_Draw( m_hImages, lpCNode->nType, pDis->hDC, pDis->rcItem.left + 3, pDis->rcItem.top, ILD_NORMAL );
				pDis->rcItem.left += 20;
			}
			
			// Draw it.
			ColorList::DrawTextItem( pDis, pszStr );
			return TRUE;
		}
	}
	return FALSE;
}

// Find the current selection.
LPHASH KeyboardPage::GetSelection()
{
	// Get the key descriptor.
	TCHAR szKeyString[ 64 ];
	if ( m_Keys.GetText( m_Keys.GetCurSel(), szKeyString ) != LB_ERR )
	{
		// Convert to a code and qualifier.
		TCHAR cCode, cQual;
		if ( ::Str2CodeQual( szKeyString, &cCode, &cQual ))
		{
			// Find the hash.
			LPHASH lpHash;
			for ( lpHash = m_pParser->aHash[ cCode & HASHMASK ]; lpHash; lpHash = lpHash->lpNext )
			{
				// Is this the one?
				if ( lpHash->cCode == cCode && lpHash->cQual == cQual )
					// Return it.
					return lpHash;
			}
		}
	}
	return NULL;
}

// Create the popup menu.
BOOL KeyboardPage::CreatePopupMenu()
{
	// Load the popup menu.
	if ( m_RealMenu.Load( MAKEINTRESOURCE( IDR_POPUP )))
	{
		// Wrap the sub-menu.
		HMENU hSubMenu = ::GetSubMenu( m_RealMenu, 0 );
		if ( hSubMenu )
		{
			m_Menu.Attach( hSubMenu );

			// Setup the menu.
			m_RealMenu.IconFrame( FALSE );
			m_RealMenu.SetImageList( m_hImages );

			// Convert the menus.
			if ( m_RealMenu.ConvertMenus())
			{
				// Setup the item bitmaps.
				StrItemBitmap	sib[ 5 ] =
				{
					{ 0, IDS_HARDCODED },
					{ 1, IDS_INSERT    },
					{ 2, IDS_RUN	   },
					{ 3, IDS_OPEN	   },
					{ 0, SBM_END	   }
				};
				if ( m_RealMenu.SetItemBitmap( sib ))
					return TRUE;
			}
		}
	}
	return FALSE;
}

// Add a keyboard mapping.
void KeyboardPage::AddKeyMapping( ClsString strKey )
{
	// Convert the string to a code and qualifier.
	TCHAR cCode, cQual;
	if ( ::Str2CodeQual( strKey, &cCode, &cQual ) == FALSE )
		return;

	// Is the hash for this key already defined?
	LPHASH lpHash;
	for ( lpHash = m_pParser->aHash[ cCode & HASHMASK ]; lpHash; lpHash = lpHash->lpNext )
	{
		// Is this the one?
		if ( lpHash->cCode == cCode && lpHash->cQual == cQual )
		{
			// Find the entry in the
			// listview control.
			for ( int i = 0; ; i++ )
			{
				// Find it in the list control.
				int nKey = m_Keys.FindString( 0, strKey );
				if ( nKey != LB_ERR )
				{
					// Select the key.
					m_Keys.SetCurSel( nKey );
					SendMessage( WM_COMMAND, MAKEWPARAM( IDC_KEYS, LBN_SELCHANGE ), ( LPARAM )m_Keys.GetSafeHWND());
					return;
				}
			}
		}
	}

	// Hash not available. Allocate
	// an empty command list.
	LPCLIST lpCList = ( LPCLIST )::AllocPooled( pParserPool, sizeof( CLIST ));
	if ( lpCList )
	{
		// Initialize list.
		NewList(( LPLIST )lpCList );

		// Add the hash to the hash table.
		if (( lpHash = ::AddHash( pParserPool, m_pParser->aHash, cCode, cQual, lpCList )) != NULL )
		{
			// Add the string to the
			// listview control.
			int nAdd = m_Keys.AddString( strKey );

			// Select it.
			m_Keys.SetCurSel( nAdd );
			SendMessage( WM_COMMAND, MAKEWPARAM( IDC_KEYS, LBN_SELCHANGE ), ( LPARAM )m_Keys.GetSafeHWND());
			
			// Changes made...
			pSettings->Changed( m_pParser );
			return;
		}
		else
			// Emit an error.
			MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );

		// De-allocate command list.
		::FreePooled( pParserPool, lpCList );
	}
	else
		// Emit an error.
		MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
}

// Create a command list node.
void KeyboardPage::CreateNode( int nType, LPVOID lpvData )
{
	// Get the currently selected keyboard hash.
	LPHASH pHash = GetSelection();
	LPCNODE pCNode = NULL;
	if ( pHash )
	{
		// Allocate node.
		pCNode = ( LPCNODE )::AllocPooled( pParserPool, sizeof( CNODE ));
		if ( pCNode != NULL )
		{
			// Setup type.
			pCNode->nType = nType;

			// Hardcoded?
			if ( nType == CTYPE_HARDCODED )
				// Setup function.
				pCNode->lpFunc = ::FindCommand(( int )lpvData );
			else
			{
				// Allocate string copy.
				pCNode->pcStr = ::CopyStringPool( pParserPool, ( LPCTSTR )lpvData );
				if ( pCNode->pcStr == NULL )
				{
					// Failure.
					MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
					::FreePooled( pParserPool, pCNode );
					pCNode = NULL;
				}
			}
		}
		else
			MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
	}

	// Still there?
	if ( pCNode )
	{
		// Add the node to the list.
		AddTail(( LPLIST )pHash->lpCommands, ( LPNODE )pCNode );

		// Add it to the listview.
		int nSel = m_Commands.AddString(( LPCTSTR )pCNode );
		if ( nSel != LB_ERR )
		{
			// Select the entry.
			m_Commands.SetCurSel( nSel );

			// Edit the node.
			SendMessage( WM_COMMAND, MAKEWPARAM( IDC_COMMANDS, LBN_DBLCLK ), ( LPARAM )m_Commands.GetSafeHWND());

			// Changes have been made.
			pSettings->Changed( m_pParser );
			SetupControls();
		}
		else
		{
			// Show out of memory error. Guess this should be the
			// only reason this could fail.
			MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );

			// Free the node and
			// it's string.
			if ( nType != CTYPE_HARDCODED ) ::FreePooled( pParserPool, pCNode->pcStr );
			::FreePooled( pParserPool, pCNode );
		}
	}
}

// WM_COMMAND handler.
LRESULT KeyboardPage::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What's up?
	switch ( nCtrlID )
	{
		case	EditToolbar::ETID_NEW:
			// Command toolbar?
			if ( hWndCtrl == m_ComTools )
			{
				// Obtain button rectangle.
				ClsRect rc;
				m_ComTools.GetItemRect( 0, rc );

				// Convert rectangle to screen
				// coordinates.
				m_ComTools.ClientToScreen( rc );

				// Show the menu.
				::TrackPopupMenuEx( m_Menu,//*m_Menu.GetSubMenu( 0 ),
						  TPM_LEFTALIGN,
						  rc.Left(),
						  rc.Bottom(),
						  GetSafeHWND(),
						  NULL );
			}
			else
			{
				// Show the key recorder.
				KeyRec kr;
				ClsString str;
				if ( kr.KeyRecord( *GetParent(), str ))
					AddKeyMapping( str );
			}
			return 0;

		case	EditToolbar::ETID_DELETE:
			// Command toolbar?
			if ( hWndCtrl == m_ComTools )
			{
				// Get current selection.
				LPCNODE pCNode = ( LPCNODE )m_Commands.GetItemData( m_Commands.GetCurSel());
				if ( pCNode != ( LPCNODE )LB_ERR )
				{
					// Save selection position.
					int nSel = m_Commands.GetCurSel();
					int nPos = nSel;
					int nMax = m_Commands.GetCount();
					
					// Select the next or previous
					// entry.
					if ( nSel == nMax - 1 && nSel ) nSel--;
					else				nSel++;

					// Select the new item
					m_Commands.SetCurSel( nSel );

					// Remove the node from the listview.
					m_Commands.DeleteString( nPos );

					// Remove it from the command list.
					Remove(( LPNODE )pCNode );

					// Free the node
					// data.
					if ( pCNode->nType != CTYPE_HARDCODED )
						::FreePooled( pParserPool, pCNode->pcStr );

					// And the node itself.
					::FreePooled( pParserPool, pCNode );

					// Setup GUI.
					SetupControls();
					pSettings->Changed( m_pParser );
				}
				return 0;
			}
			else
			{
				// Get the key descriptor.
				TCHAR szKeyString[ 64 ];
				int nSel = m_Keys.GetCurSel(), nMax = m_Keys.GetCount();
				if ( nSel != LB_ERR )
				{
					if ( m_Keys.GetText( nSel, szKeyString ) != LB_ERR )
					{
						// Convert to a code and qualifier.
						TCHAR cCode, cQual;
						if ( ::Str2CodeQual( szKeyString, &cCode, &cQual ))
						{
							// Remove the hash. This will automatically destroy
							// the command-list aswell.
							::RemHash( pParserPool, m_pParser->aHash, cCode, cQual );

							// Remove it from the listview.
							m_Keys.DeleteString( nSel );

							// Was it the last one?
							if ( nSel == nMax - 1 ) nSel--;

							// Select the next or previous key.
							m_Keys.SetCurSel( nSel );
							SendMessage( WM_COMMAND, MAKEWPARAM( IDC_KEYS, LBN_SELCHANGE ), ( LPARAM )m_Keys.GetSafeHWND());

							// Changes made...
							pSettings->Changed( m_pParser );
						}
					}
				}
			}
			return 0;

		case	EditToolbar::ETID_UP:
		{
			// Valid node?
			LPCNODE pCNode = ( LPCNODE )m_Commands.GetItemData( m_Commands.GetCurSel());
			if ( pCNode != ( LPCNODE )LB_ERR )
			{
				// Get it's predecessor.
				LPCNODE pCPrev = pCNode->lpPrev;

				// Swap both data entries.
				CNODE cTemp;
				cTemp.nType	  = pCPrev->nType;
				cTemp.uCommand	  = pCPrev->uCommand;
				pCPrev->nType	  = pCNode->nType;
				pCPrev->uCommand  = pCNode->uCommand;
				pCNode->nType	  = cTemp.nType;
				pCNode->uCommand  = cTemp.uCommand;

				// Select the previous item which
				// in fact is the already selected
				// item.
				m_Commands.SetCurSel( m_Commands.GetCurSel() - 1 );
				SetupControls();
				pSettings->Changed( m_pParser );
				return 0;
			}
		}

		case	EditToolbar::ETID_DOWN:
		{
			// Valid node?
			LPCNODE pCNode = ( LPCNODE )m_Commands.GetItemData( m_Commands.GetCurSel());
			if ( pCNode != ( LPCNODE )LB_ERR )
			{
				// Get it's successor.
				LPCNODE pCNext = pCNode->lpNext;
				
				// Swap both data entries.
				CNODE cTemp;
				cTemp.nType	  = pCNext->nType;
				cTemp.uCommand	  = pCNext->uCommand;
				pCNext->nType	  = pCNode->nType;
				pCNext->uCommand  = pCNode->uCommand;
				pCNode->nType	  = cTemp.nType;
				pCNode->uCommand  = cTemp.uCommand;

				// Select the next item which
				// in fact is the already selected
				// item.
				m_Commands.SetCurSel( m_Commands.GetCurSel() + 1 );
				SetupControls();
				pSettings->Changed( m_pParser );
				return 0;
			}
		}

		case	IDS_INSERT:
			CreateNode( CTYPE_TEXT, ( LPVOID )( LPCTSTR )ClsString( MAKEINTRESOURCE( IDS_NEWINSERT )));
			return 0;

		case	IDS_HARDCODED:
			CreateNode( CTYPE_HARDCODED, ( LPVOID )( ::GetCommandTable()->nCommandID ));
			return 0;

		case	IDS_RUN:
			CreateNode( CTYPE_RUN, ( LPVOID )( _T( "Calc.exe" )));
			return 0;

		case	IDS_OPEN:
			CreateNode( CTYPE_SHELLOPEN, ( LPVOID )( _T( "ReadMe.txt" )));
			return 0;

		case	IDC_KEYS:
			if ( nNotifyCode == LBN_SELCHANGE )
			{
				// Get the current selection.
				LPHASH pSel = GetSelection();
				if ( pSel )
				{
					// Clear the contents of the command list.
					m_Commands.ResetContent();

					// Add commands.
					LPCNODE pNode;
					for ( pNode = pSel->lpCommands->lpFirst; pNode->lpNext; pNode = pNode->lpNext )
						m_Commands.AddString(( LPCTSTR )pNode );
				}
			}
			// Setup controls.
			SetupControls();
			return 0;

		case	IDC_COMMANDS:
			// Double-click?
			if ( nNotifyCode == LBN_DBLCLK )
			{
				// Find the command node.
				LPCNODE pCNode = ( LPCNODE )m_Commands.GetItemData( m_Commands.GetCurSel());
				if ( pCNode != ( LPCNODE )LB_ERR )
				{
					// What's the type?
					switch ( pCNode->nType )
					{
						case	CTYPE_HARDCODED:
						{
							// Open the editor.
							Hardcoded h;
							if ( h.Select( *GetParent(), pCNode ))
							{
								// Refresh list.
								m_Commands.Invalidate();
								pSettings->Changed( m_pParser );
							}
							break;
						}

						case	CTYPE_TEXT:
						{
							// Open the editor.
							TextInsert ti;
							HICON hIcon = ImageList_GetIcon( m_hImages, 1, ILD_NORMAL );
							if ( ti.Edit( *GetParent(), pCNode, hIcon ))
							{
								// Refresh list.
								m_Commands.Invalidate();
								pSettings->Changed( m_pParser );
							}
							if ( hIcon ) ::DestroyIcon( hIcon );
							return 0;
						}
						
						case	CTYPE_RUN:
						case	CTYPE_SHELLOPEN:
						{
							// Open the editor.
							Run r;
							if ( r.Edit( *GetParent(), pCNode ))
							{
								// Refresh list.
								m_Commands.Invalidate();
								pSettings->Changed( m_pParser );
							}
						}
					}
				}
			}
			else
				// Setup GUI.
				SetupControls();
			return 0;
	}
	// Pass to the base class.
	return Page::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// WM_INITDIALOG handler.
LRESULT KeyboardPage::OnInitDialog( PROPSHEETPAGE * pPropSheetPage )
{
	// Wrap controls.
	m_Tools.Attach( GetDlgItemHandle( IDC_TOOLS ));
	m_Tools.SetupToolbar( TRUE, FALSE, FALSE, FALSE );
	m_ComTools.Attach( GetDlgItemHandle( IDC_COMTOOLS ));
	m_ComTools.SetupToolbar( TRUE, TRUE, FALSE, FALSE );
	m_Keys.Attach( GetDlgItemHandle( IDC_KEYS ));
	m_Commands.Attach( GetDlgItemHandle( IDC_COMMANDS ));

	// Create the menu.
	CreatePopupMenu();

	// Call the base-class.
	return Page::OnInitDialog( pPropSheetPage );
}
