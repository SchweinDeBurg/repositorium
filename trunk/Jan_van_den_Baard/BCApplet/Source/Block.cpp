//
//	block.cpp
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
BlockEdit::BlockEdit()
{
}

// Destruction.
BlockEdit::~BlockEdit()
{
}

LRESULT BlockEdit::OnInitDialog( LPPROPSHEETPAGE pPsp )
{
	// Setup edit controls.
	m_Name.Attach( GetDlgItemHandle( IDC_NAME ));
	m_Name.SetWindowText( m_Block.pszName );
	m_Start.Attach( GetDlgItemHandle( IDC_START ));
	m_Start.SetWindowText( m_Block.pszStart );
	m_End.Attach( GetDlgItemHandle( IDC_END ));
	if ( m_Block.pszEnd != END_OF_LINE ) m_End.SetWindowText( m_Block.pszEnd );

	// The checkbox and radio buttons.
	m_SOL.Attach( GetDlgItemHandle( IDC_SOL ));
	m_SOL.SetCheck( m_Block.bStartOfLine ? BST_CHECKED : BST_UNCHECKED );
	m_EOL.Attach( GetDlgItemHandle( IDC_EOL ));
	m_EOL.SetCheck( m_Block.pszEnd == END_OF_LINE ? BST_CHECKED : BST_UNCHECKED );
	m_White.Attach( GetDlgItemHandle( IDC_WHITE ));
	m_White.SetCheck( m_Block.pszEnd == NULL ? BST_CHECKED : BST_UNCHECKED );
	m_Chars.Attach( GetDlgItemHandle( IDC_CHARS ));
	m_Chars.SetCheck( m_Block.pszEnd && m_Block.pszEnd != END_OF_LINE ? BST_CHECKED : BST_UNCHECKED );

	// The color toolbar.
	m_Color.Attach( GetDlgItemHandle( IDC_COLOR ));
	m_BkColor.Attach( GetDlgItemHandle( IDC_BGCOLOR ));
	m_Color.Color() = m_Block.crColor;
	m_BkColor.Color() = m_Block.crBgColor;
	m_Color.DropDownArrow() = TRUE;
	m_BkColor.DropDownArrow() = TRUE;
	m_Color.DefaultString() = _T( "" );
	m_BkColor.DefaultString() = ClsString( MAKEINTRESOURCE( IDS_TRANSPARENT ));
	m_Color.ShowToolTip();
	m_BkColor.ShowToolTip();
	if ( ! ThemingAPI.IsThemingEnabled())
	{
		m_Color.SetHotTrack( FALSE );
		m_BkColor.SetHotTrack( FALSE );
	}
	
	// Enable/disable end control.
	m_End.EnableWindow(( BOOL )( m_Chars.GetCheck() == BST_CHECKED ));
	return TRUE;
}

LRESULT BlockEdit::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What the ID?
	switch ( nCtrlID )
	{
		case	IDC_COLOR:
			// Save the color.
			if ( nNotifyCode == COLPN_COLORCHANGED )
				m_Block.crColor = m_Color.Color();
			return 0;

		case	IDC_BGCOLOR:
			// Save the color.
			if ( nNotifyCode == COLPN_COLORCHANGED )
				m_Block.crBgColor = m_BkColor.Color();
			return 0;
			
		case	IDC_NAME:
			// Pickup the new contents.
			if ( nNotifyCode == EN_KILLFOCUS )
			{
				// Get buffer size.
				int nLength = m_Name.GetWindowTextLength();

				// Any characters?
				LPTSTR pszString = NULL;
				if ( nLength )
				{
					// Allocate memory for the name.
					pszString = ( LPTSTR )::ArrayAllocMem( m_pParser->lpaBlocks, nLength + 1 );
					if ( pszString )
						// Get the text.
						m_Name.GetWindowText( pszString, nLength + 1 );
				}

				// Free the old string.
				if ( m_Block.pszName )
					::ArrayFreeMem( m_pParser->lpaBlocks, m_Block.pszName );

				// Setup new name.
				m_Block.pszName = pszString;
			}
			return 0;

		case	IDC_START:
		{
			// Pickup the new contents.
			if ( nNotifyCode == EN_KILLFOCUS )
			{
				// Get buffer size.
				int nLength = m_Start.GetWindowTextLength();

				// Any characters?
				LPTSTR pszString = NULL;
				if ( nLength )
				{
					// Allocate memory for the name.
					pszString = ( LPTSTR )::ArrayAllocMem( m_pParser->lpaBlocks, nLength + 1 );
					if ( pszString )
					{
						// Get the text.
						m_Start.GetWindowText( pszString, nLength + 1 );

						// Setup it's length.
						m_Block.nStart = _tcslen( pszString );

						// Free the old string.
						if ( m_Block.pszStart )
							::ArrayFreeMem( m_pParser->lpaBlocks, m_Block.pszStart );

						// Setup new string.
						m_Block.pszStart = pszString;
					}
				}
				else
				{
					// Empty strings are illegal.
					ClsRect rc;
					m_Start.GetWindowRect( rc );
					ScreenToClient( rc );
					ClsBalloonHelp::LaunchBalloon( ClsGetApp()->GetAppTitle(), MAKEINTRESOURCE( IDS_ERROR_NOEMPTY ), rc.CenterPoint(), IDI_ERROR, ClsBalloonHelp::unSHOW_INNER_SHADOW | ClsBalloonHelp::unSHOW_CLOSE_BUTTON, this );

					// Setup old value and focus the control.
					m_Start.SetWindowText( m_Block.pszStart );
					m_Start.SetSel( 0, m_Start.GetWindowTextLength());
					m_Start.SetFocus();
				}
				return 0;
			}
		}

		case	IDC_END:
		{
			// Pickup the new contents.
			if ( nNotifyCode == EN_KILLFOCUS )
			{
				// Get buffer size.
				int nLength = m_End.GetWindowTextLength();

				// Any characters?
				LPTSTR pszString = NULL;
				if ( nLength )
				{
					// Allocate memory for the name.
					pszString = ( LPTSTR )::ArrayAllocMem( m_pParser->lpaBlocks, nLength + 1 );
					if ( pszString )
					{
						// Get the text.
						m_End.GetWindowText( pszString, nLength + 1 );

						// Setup length.
						m_Block.nEnd = _tcslen( pszString );
					}
				}
				else
					// No length.
					m_Block.nEnd = 0;

				// Free the old string.
				if ( m_Block.pszEnd && m_Block.pszEnd != END_OF_LINE )
					::ArrayFreeMem( m_pParser->lpaBlocks, m_Block.pszEnd );

				// Setup new string.
				m_Block.pszEnd = pszString;
			}
			return 0;
		}

		case	IDC_SOL:
			// Setup start of line boolean.
			m_Block.bStartOfLine = ( BOOL )( m_SOL.GetCheck() == BST_CHECKED );
			return 0;

		case	IDC_EOL:
			// Free end string.
			if ( m_Block.pszEnd && m_Block.pszEnd != END_OF_LINE )
				::ArrayFreeMem( m_pParser->lpaBlocks, m_Block.pszEnd );

			// Clear end edit control.
			m_End.SetWindowText( NULL );

			// No length.
			m_Block.nEnd = 0;

			// Make it EOL.
			m_Block.pszEnd = END_OF_LINE;

			// Enable/disable end control.
			m_End.EnableWindow(( BOOL )( m_Chars.GetCheck() == BST_CHECKED ));
			return 0;

		case	IDC_WHITE:
			// Free end string.
			if ( m_Block.pszEnd && m_Block.pszEnd != END_OF_LINE )
				::ArrayFreeMem( m_pParser->lpaBlocks, m_Block.pszEnd );

			// No length.
			m_Block.nEnd = 0;

			// Clear end edit control.
			m_End.SetWindowText( NULL );

			// Make it NULL.
			m_Block.pszEnd = NULL;

			// Enable/disable end control.
			m_End.EnableWindow(( BOOL )( m_Chars.GetCheck() == BST_CHECKED ));
			return 0;

		case	IDC_CHARS:
			// Set characters if any.
			if ( m_Block.pszEnd )
			{
				if ( m_Block.pszEnd != END_OF_LINE ) 
				{
					m_End.SetWindowText( m_Block.pszEnd );
					m_Block.nEnd = _tcslen( m_Block.pszEnd );
				}
				else				      
				{
					m_Block.pszEnd = NULL;
					m_Block.nEnd = 0;
				}
			}
			// Enable/disable end control.
			m_End.EnableWindow(( BOOL )( m_Chars.GetCheck() == BST_CHECKED ));
			break;
	
		case	IDC_OK:
			// End with an OK.
			EndDialog( TRUE );
			return 0;

		case	IDC_CANCEL:
			// End with a cancellation.
			EndDialog( FALSE );
			break;
	}
	// Pass onto the base class.
	return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// Open the block editor.
BOOL BlockEdit::EditBlock( ClsWindow& Parent, LPBLOCK pBlock, LPPARSER pParser )
{
	// Save pointer.
	m_pParser = pParser;

	// Clone input block.
	BOOL bRC = FALSE;
	if ( CloneBlock( pBlock, &m_Block ))
		// Open the dialog.
		bRC = ( BOOL )DoModal( Parent, IDD_BLOCK );

	// User confirmed?
	LPBLOCK pToFree;
	if ( bRC ) pToFree = pBlock;
	else	   pToFree = &m_Block;

	// Free the strings from the
	// block structure.
	if ( pToFree->pszStart ) ::ArrayFreeMem( pParser->lpaBlocks, pToFree->pszStart );
	if ( pToFree->pszEnd && pToFree->pszEnd != END_OF_LINE ) ::ArrayFreeMem( pParser->lpaBlocks, pToFree->pszEnd );
	if ( pToFree->pszName ) ::ArrayFreeMem( pParser->lpaBlocks, pToFree->pszName );

	// Setup new values.
	if ( bRC ) *pBlock = m_Block;
	return bRC;
}

// Clone a block structure.
BOOL BlockEdit::CloneBlock( LPBLOCK pSource, LPBLOCK pDest )
{
	// Copy contents.
	*pDest = *pSource;

	// Clear pointers.
	pDest->pszName = pDest->pszEnd = pDest->pszStart = NULL;

	// Copy name string.
	if ( pSource->pszName )
	{
		// Allocate memory.
		pDest->pszName = ( LPTSTR )::ArrayAllocMem( m_pParser->lpaBlocks, ( _tcslen( pSource->pszName ) + 1 ) * sizeof( TCHAR ));
		if ( pDest->pszName )
			// Copy string.
			_tcscpy( pDest->pszName, pSource->pszName );
		else
			return FALSE;
	}

	// Copy start string,.
	pDest->pszStart = ( LPTSTR )::ArrayAllocMem( m_pParser->lpaBlocks, ( _tcslen( pSource->pszStart ) + 1 ) * sizeof( TCHAR ));
	if ( pDest->pszStart )
		// Copy string.
		_tcscpy( pDest->pszStart, pSource->pszStart );
	else
	{
		if ( pDest->pszName ) ::ArrayFreeMem( m_pParser->lpaBlocks, pDest->pszName );
		return FALSE;
	}

	// Copy end string.
	if ( pSource->pszEnd )
	{
		// End of line>?
		if ( pSource->pszEnd == END_OF_LINE )
			pDest->pszEnd = END_OF_LINE;
		else
		{
			// Allocate memory.
			pDest->pszEnd = ( LPTSTR )::ArrayAllocMem( m_pParser->lpaBlocks, ( _tcslen( pSource->pszEnd ) + 1 ) * sizeof( TCHAR ));
			if ( pDest->pszEnd )
				// Copy string.
				_tcscpy( pDest->pszEnd, pSource->pszEnd );
			else
			{
				if ( pDest->pszName ) ::ArrayFreeMem( m_pParser->lpaBlocks, pDest->pszName );
				::ArrayFreeMem( m_pParser->lpaBlocks, pDest->pszStart );
				return FALSE;
			}
		}
	}
	return TRUE;
}
