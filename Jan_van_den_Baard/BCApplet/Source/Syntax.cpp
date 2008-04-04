//
//	syntax.cpp
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
SyntaxPage::SyntaxPage() 
{
	// Setup icon.
	m_nIcon = IDI_SYNTAX;
}

// Destructor.
SyntaxPage::~SyntaxPage()
{
}

// Setup the toolbar.
void SyntaxPage::SetupToolbar()
{
	// Get current list selection and count.
	int nSel = m_Blocks.GetCurSel();
	int nCnt = m_Blocks.GetCount();

	// Do the toolbar buttons.
	m_Tools.EnableButton( EditToolbar::ETID_DELETE, ( BOOL )( nSel != LB_ERR ));
	m_Tools.EnableButton( EditToolbar::ETID_UP,     ( BOOL )( nSel != LB_ERR && nSel > 0 ));
	m_Tools.EnableButton( EditToolbar::ETID_DOWN,   ( BOOL )( nSel != LB_ERR && nSel < nCnt - 1 ));

	// Also for the common colors.
	nSel = m_Common.GetCurSel();
	m_Color.EnableButton( EditToolbar::ETID_COLOR, ( BOOL )( nSel != LB_ERR ));
}

// Refresh page contents.
void SyntaxPage::RefreshData( LPPARSER pParser )
{
	_ASSERT_VALID( pParser );

	// Save parser.
	m_pParser = pParser;

	// Setup lists.
	m_Blocks.SetParser( pParser );
	m_Common.SetParser( pParser );
	m_Blocks.SetBlockList();
	m_Common.SetCommonList();

	// Setup other controls.
	m_Escape.SetWindowText( pParser->cEscape ? ( LPCTSTR )ClsString( pParser->cEscape ) : NULL );
	m_Syntax.SetCheck( pParser->bSyntaxColoring ? BST_CHECKED : BST_UNCHECKED );

	// Setup the toolbar.
	SetupToolbar();
}

// Save a color value.
void SyntaxPage::SaveColor( ClsStdioFile *pFile, LPPARSER pParser, LPCTSTR pszDesc, DWORD dwFlag, int nIndex )
{
	if ( pParser->crColors[ nIndex ] == CLR_DEFAULT )
		pFile->PrintF( _T( "%s*\n" ), pszDesc );
	else
	{
		pFile->PrintF( pszDesc );
		pFile->PrintF( ( pParser->dwColorFlags & dwFlag ) ? _T( "*\n" ) : _T( "%ld,%ld,%ld\n" ), GetRValue( pParser->crColors[ nIndex ] ), 
													GetGValue( pParser->crColors[ nIndex ] ), 
													GetBValue( pParser->crColors[ nIndex ] ));
	}
}

// Save page/parser contents.
BOOL SyntaxPage::SavePage( ClsStdioFile *pFile, LPPARSER pParser )
{
	try
	{
		// Write colors comment.
		pFile->PrintF( ClsString( MAKEINTRESOURCE( IDS_COMMENT_COLORS )));

		// Write colors.
		SaveColor( pFile, pParser, _T( "TextRGB=" ),			CF_DEFAULT_TEXT, CARR_TEXT );
		SaveColor( pFile, pParser, _T( "BackgroundRGB=" ),		CF_DEFAULT_BACKGROUND, CARR_BACKGROUND );
		SaveColor( pFile, pParser, _T( "BackgroundReadOnlyRGB=" ),	CF_DEFAULT_BACKGROUND_RO, CARR_BACKGROUND_READONLY );
		SaveColor( pFile, pParser, _T( "MarginRGB=" ),			CF_DEFAULT_MARGIN, CARR_SELECTION_MARGIN );
		SaveColor( pFile, pParser, _T( "BookmarkRGB=" ),		CF_DEFAULT_BOOKMARK, CARR_BOOKMARK );
		SaveColor( pFile, pParser, _T( "NumberRGB=" ),			CF_DEFAULT_NUMBER, CARR_NUMBER );
		SaveColor( pFile, pParser, _T( "DelimiterRGB=" ),		CF_DEFAULT_DELIMITER, CARR_DELIMITER );		  
		SaveColor( pFile, pParser, _T( "SelectedTextRGB=" ),		CF_DEFAULT_SELECTED_TEXT, CARR_SELECTED_TEXT );
		SaveColor( pFile, pParser, _T( "SelectedBackgroundRGB=" ),	CF_DEFAULT_SELECTED_BKGND, CARR_SELECTED_BKGND );
		SaveColor( pFile, pParser, _T( "LineNumberRGB=" ),		CF_DEFAULT_LINE_NUMBERS, CARR_LINE_NUMBERS );
		SaveColor( pFile, pParser, _T( "BkgndLineNumberRGB=" ),		CF_DEFAULT_LINE_NUMBERS_BKGND, CARR_BACKGROUND_LINE_NUMBERS );
		SaveColor( pFile, pParser, _T( "BkgndNumberRGB="),		CF_DEFAULT_BACKGROUND_NUMBER, CARR_BACKGROUND_NUMBER );
		SaveColor( pFile, pParser, _T( "BkgndDelimiterRGB="),		CF_DEFAULT_BACKGROUND_DELIMITER, CARR_BACKGROUND_DELIMITER );
		SaveColor( pFile, pParser, _T( "BracketMatchRGB="),		CF_DEFAULT_BRACKET_MATCH, CARR_BRACKET_MATCH );
		SaveColor( pFile, pParser, _T( "LineHighlightRGB="),		CF_DEFAULT_LINE_HIGHLIGHT, CARR_LINE_HIGHLIGHT );
		SaveColor( pFile, pParser, _T( "HyperlinkRGB="),		CF_DEFAULT_HYPERLINK, CARR_HYPERLINK );
		SaveColor( pFile, pParser, _T( "BkgndHyperlinkRGB="),		CF_DEFAULT_BACKGROUND_HYPERLINK, CARR_BACKGROUND_HYPERLINK );


		// Write the screen and printer
		// fonts.
		pFile->PrintF( _T( "ScreenFont=%s,%ld,%ld,%ld,%ld,%ld\n" ), pParser->lfScreenFont.lfFaceName, pParser->lfScreenFont.lfHeight, pParser->lfScreenFont.lfWeight, pParser->lfScreenFont.lfCharSet, pParser->lfScreenFont.lfItalic, pParser->lfScreenFont.lfUnderline );

		// Write general syntax coloring comment.
		pFile->PrintF( ClsString( MAKEINTRESOURCE( IDS_COMMENT_SYNTAXGENERAL )));

		// Save general syntax coloring settings.
		if ( pParser->cEscape ) pFile->PrintF( _T( "Escape=%lc\n" ), pParser->cEscape );
		pFile->PrintF( _T( "SyntaxColoring=%lc\n" ), pParser->bSyntaxColoring ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "Case=%lc\n" ), pParser->bCaseOn ? _T( 'Y' ) : _T( 'N' ));

		// Write blocks comment.
		pFile->PrintF( ClsString( MAKEINTRESOURCE( IDS_COMMENT_BLOCKS )));

		// Write blocks.
		for ( int i = 0; i < ::ArrayGetSize( pParser->lpaBlocks ); i++ )
		{
			// Get block.
			LPBLOCK lpBlock = ( LPBLOCK )::ArrayGetAt( pParser->lpaBlocks, i );

			// Write block opening.
			if ( lpBlock->pszName ) pFile->PrintF( _T( "Block=%s\n" ), lpBlock->pszName );
			else			pFile->PrintF( _T( "Block\n" ));

			// Write color commands.
			pFile->PrintF( _T( "\tRGB=%ld,%ld,%ld\n" ), GetRValue( lpBlock->crColor ), 
								    GetGValue( lpBlock->crColor ), 
								    GetBValue( lpBlock->crColor ));

			if ( lpBlock->crBgColor == CLR_DEFAULT )
				pFile->PrintF( _T( "\tBkRGB=*\n" ));
			else
				pFile->PrintF( _T( "\tBkRGB=%ld,%ld,%ld\n" ), GetRValue( lpBlock->crBgColor ), 
									      GetGValue( lpBlock->crBgColor ), 
									      GetBValue( lpBlock->crBgColor ));

			// Write start command.
			pFile->PrintF( _T( "\tStart=%s\n" ), lpBlock->pszStart );

			// At the start of the line?
			if ( lpBlock->bStartOfLine )
				pFile->PrintF( _T( "\tStartOfLine=Y\n" ));

			// Write end command.
			if ( lpBlock->pszEnd ) pFile->PrintF( _T( "\tEnd=%s\n" ), lpBlock->pszEnd == END_OF_LINE ? _T( "\\n" ) : lpBlock->pszEnd );
			else		       pFile->PrintF( _T( ";\tEnd=\n" ));

			// Write block terminator.
			pFile->PrintF( _T( "EndBlock\n\n" ));
		}

		// Write keywords comment.
		pFile->PrintF( ClsString( MAKEINTRESOURCE( IDS_COMMENT_KEYWORDS )));

		// Initialize buffer list.
		KEYLIST kl;
		BOOL bKeywords = FALSE;
		NewList(( LPLIST )&kl );
		
		// Convert hashes.
		if ( KeywordEdit::Hash2List( &kl, pParser ))
		{
			// Iterate nodes.
			LPKEYWORDS pKW;
			for ( pKW = kl.lpFirst; pKW->lpNext; pKW = pKW->lpNext )
			{
				// Keywords command written?
				if ( bKeywords == FALSE )
				{
					// Write keywords command.
					pFile->PrintF( _T( "Keywords\n" ));
					bKeywords = TRUE;
				}

				// Write color values.
				pFile->PrintF( _T( "RGB=%ld,%ld,%ld\n" ), GetRValue( pKW->crColor ), 
									  GetGValue( pKW->crColor ), 
									  GetBValue( pKW->crColor ));
				if ( pKW->crBgColor == CLR_DEFAULT ) pFile->PrintF( _T( "BkRGB=*\n" ) );
				else
					pFile->PrintF( _T( "BkRGB=%ld,%ld,%ld\n" ), GetRValue( pKW->crBgColor ), 
										  GetGValue( pKW->crBgColor ), 
										  GetBValue( pKW->crBgColor ));

				// Write keywords from this group.
				for ( int i = 0; i < ::ArrayGetSize( pKW->lpaKeywords ); i++ )
					pFile->PrintF( _T( "%s\n" ), *(( LPCTSTR * )::ArrayGetAt( pKW->lpaKeywords, i )));
			}

			// Free the converted list.
			KeywordEdit::FreeKeyList( &kl );
		}
		else
			return FALSE;

		// Write endkeywords command.
		if ( bKeywords )
			pFile->PrintF( _T( "EndKeywords\n" ));
	}
	catch ( ClsException& e )
	{
		// Error...
		UNREFERENCED_PARAMETER( e );
		return FALSE;
	}
	return TRUE;
}

// Window procedure.
LRESULT SyntaxPage::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// What's the message?
	switch ( uMsg )
	{
		case	CPN_SELENDOK:
		{
			// Get the current selection.
			int nSel = m_Common.GetCurSel();
			_ASSERT( nSel != LB_ERR );

			// Default?
			if ( wParam == CLR_DEFAULT )
			{
				// Setup default.
				m_pParser->dwColorFlags |= m_Common.GetFlagArray()[ nSel ];
				m_pParser->crColors[ nSel ] = m_Common.GetDefArray()[ nSel ];
			}
			else
			{
				// Setup the color.
				m_pParser->dwColorFlags &= ~m_Common.GetFlagArray()[ nSel ];
				m_pParser->crColors[ nSel ] = wParam;
			}
			// Change made.
			m_Common.Invalidate();
			pSettings->Changed( m_pParser );
			return 0;
		}
	}
	// Baseclass...
	return Page::WindowProc( uMsg, wParam, lParam );
}

// WM_COMMAND handler.
LRESULT SyntaxPage::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What's the trouble...
	switch ( nCtrlID )
	{
		case	EditToolbar::ETID_COLOR:
		{
			// Get the current selection.
			int nSel = m_Common.GetCurSel();

			// Only show the popup when there is
			// a valid selection.
			if ( nSel != LB_ERR )
			{
				// Get the position of the selected entry.
				ClsRect rc;
				m_Color.GetItemRect( 6, rc );
				m_Color.ClientToScreen( rc );

				// Create the popup. The popup will automatically destroy
				// itself.
				new ClsColorPopup( ClsPoint( rc.Left(), rc.Bottom()), 
						   m_pParser->dwColorFlags & m_Common.GetFlagArray()[ nSel ] ? CLR_DEFAULT : m_pParser->crColors[ nSel ], 
						   this,
						   0,
						   ClsString( MAKEINTRESOURCE( IDS_DEFAULT )), 
						   ClsString( MAKEINTRESOURCE( IDS_CUSTOM )), 
						   NULL, 
						   TRUE,
						   FALSE );
			} 
			return 0;
		}

		case	EditToolbar::ETID_UP:
			// Move entry up.
			m_Blocks.MoveSelUp();
			SetupToolbar();
			return 0;

		case	EditToolbar::ETID_DOWN:
			// Move entry down.
			m_Blocks.MoveSelDown();
			SetupToolbar();
			return 0;

		case	EditToolbar::ETID_DELETE:
			// Remove entry.
			m_Blocks.RemoveSel();
			SetupToolbar();
			return 0;

		case	EditToolbar::ETID_NEW:
		{
			// Create a block as follows:
			//
			//		Name=New Block...
			//			RGB=50,100,150
			//			Start=;
			//			End=\n
			//		EndBlock
			ClsString strName( MAKEINTRESOURCE( IDS_NEW_BLOCK ));
			BLOCK	  bBlock;
			bBlock.pszName = ( LPTSTR )::ArrayAllocMem( m_pParser->lpaBlocks, ( strName.GetStringLength() + 1 ) * sizeof( TCHAR ));
			if ( bBlock.pszName )
			{
				// Copy the name.
				_tcscpy( bBlock.pszName, strName );

				// Allocate start string.
				bBlock.pszStart = ( LPTSTR )::ArrayAllocMem( m_pParser->lpaBlocks, ( _tcslen( _T( ";" )) + 1 ) * sizeof( TCHAR ));
				if ( bBlock.pszStart )
				{
					// Copy the name.
					_tcscpy( bBlock.pszStart, _T( ";" ));

					// Setup the length.
					bBlock.nStart = _tcslen( _T( ";" ));

					// Block end is EOL.
					bBlock.pszEnd = END_OF_LINE;

					// Add it to the array.
					if ( ::ArrayAdd( m_pParser->lpaBlocks, &bBlock, 1 ))
					{
						// Get the added block.
						LPBLOCK	lpAdded = ( LPBLOCK )::ArrayGetAt( m_pParser->lpaBlocks, ::ArrayGetSize( m_pParser->lpaBlocks ) - 1 );

						// Setup color.
						lpAdded->crColor = RGB( 50, 100, 150 );
						lpAdded->crBgColor = CLR_DEFAULT;

						// We have to re-add the array contents since
						// adding items to the array may have
						// caused a re-allocation of the array
						// elements which, in turn, causes the
						// listview contents to be faulty.
						m_Blocks.ResetContent();
						for ( int i = 0; i < ::ArrayGetSize( m_pParser->lpaBlocks ); i++ )
							m_Blocks.AddString(( LPCTSTR )::ArrayGetAt( m_pParser->lpaBlocks, i ));

						// Select last added entry.
						m_Blocks.SetCurSel( ::ArrayGetSize( m_pParser->lpaBlocks ) - 1 );

						// Setup toolbar.
						SetupToolbar();
						
						// Changes made...
						pSettings->Changed( m_pParser );

						// Edit it.
						BlockEdit be;
						if ( be.EditBlock( *GetParent(), lpAdded, m_pParser ))
							// Refresh the list.
							m_Blocks.Invalidate();
						return 0;
					}
					::ArrayFreeMem( m_pParser->lpaBlocks, bBlock.pszStart );
				}
				::ArrayFreeMem( m_pParser->lpaBlocks, bBlock.pszName );
			}
			MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
			return 0;
		}

		case	IDC_COMMON:
			// Setup toolbar.
			SetupToolbar();
			return 0;
			
		case	IDC_BLOCKS:
			// Double-click?
			if ( nNotifyCode == LBN_DBLCLK )
			{
				// Get the current selection.
				int nSel = m_Blocks.GetCurSel();

				// Edit it.
				BlockEdit be;
				if ( be.EditBlock( *GetParent(), ( LPBLOCK )::ArrayGetAt( m_pParser->lpaBlocks, nSel ), m_pParser ))
				{
					// Refresh the list.
					m_Blocks.Invalidate();
					pSettings->Changed( m_pParser );
				}
			}
			else
				// Setup the toolbar.
				SetupToolbar();
			return 0;

		case	IDC_ESCAPE:
		{
			TCHAR	szBuf[ 2 ] = { 0 };

			// Pickup the buffer.
			m_Escape.GetWindowText( szBuf, 2 );

			// did it really change?
			if ( m_pParser->cEscape != szBuf[ 0 ] )
			{
				// Save escape character.
				m_pParser->cEscape = szBuf[ 0 ];
			
				// Changes made...
				pSettings->Changed( m_pParser );
			}
			return 0;
		}

		case	IDC_SYNTAX:
			// Get setting.
			m_pParser->bSyntaxColoring = ( BOOL )( m_Syntax.GetCheck() == BST_CHECKED );
			pSettings->Changed( m_pParser );
			return 0;

		case	IDC_FONT:
		{
			// Copy the font input.
			LOGFONT lf = m_pParser->lfScreenFont;

			// Popup font dialog.
			ClsFontDialog fd;
			if ( fd.DoModal( this, &lf, CF_BOTH | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT ))
			{
				// Copy the contents.
				m_pParser->lfScreenFont = lf;
			
				// Changes made...
				pSettings->Changed( m_pParser );
			}
			return 0;
		}

		case	IDC_KEYWORDS:
		{
			// Open the keyword editor...
			KeywordEdit ke;
			ke.Edit( *GetParent(), m_pParser );
			return 0;
		}
	}
	// Pass to the base class.
	return Page::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// WM_INITDIALOG handler.
LRESULT SyntaxPage::OnInitDialog( PROPSHEETPAGE * pPropSheetPage )
{
	// Setup toolbars.
	m_Tools.Attach( GetDlgItemHandle( IDC_TOOLS ));
	m_Tools.SetupToolbar( TRUE, TRUE, FALSE, FALSE );
	m_Color.Attach( GetDlgItemHandle( IDC_COLOR ));
	m_Color.SetupToolbar( FALSE, FALSE, FALSE, TRUE );
	
	// Setup listboxes.
	m_Blocks.Attach( GetDlgItemHandle( IDC_BLOCKS ));
	m_Common.Attach( GetDlgItemHandle( IDC_COMMON ));

	// And the rest of the controls.
	m_Escape.Attach( GetDlgItemHandle( IDC_ESCAPE ));
	m_Escape.SetLimitText( 1 );
	m_Syntax.Attach( GetDlgItemHandle( IDC_SYNTAX ));

	// Call the base-class.
	return Page::OnInitDialog( pPropSheetPage );
}
