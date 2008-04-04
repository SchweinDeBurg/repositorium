//
//	insertedit.cpp
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
TextInsert::TextInsert()
{
}

// Destruction.
TextInsert::~TextInsert()
{
}

// Window procedure override.
LRESULT TextInsert::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Make sure the dialog does not get resized
	// smaller than it should.
	if ( uMsg == WM_GETMINMAXINFO )
	{
		LPMINMAXINFO pmmi = ( LPMINMAXINFO )lParam;
		pmmi->ptMinTrackSize.x = m_MinSize.CX();
		pmmi->ptMinTrackSize.y = m_MinSize.CY();
		return 0;
	}
	// Pass the rest on to the base class.
	return ClsSizeDialog::WindowProc( uMsg, wParam, lParam );
}

// WM_SIZE handler.
LRESULT TextInsert::OnSize( UINT nSizeType, int nWidth, int nHeight )
{
	// Re-layout the master layout engine.
	m_Master.Relayout();

	// Pass to the base class.
	return ClsSizeDialog::OnSize( nSizeType, nWidth, nHeight );
}

// WM_INITDIALOG handler.
LRESULT TextInsert::OnInitDialog( LPPROPSHEETPAGE pPsp )
{
	// Create layout engines.
	m_Master.Create( *this, LAYOUT_BottomOffset, ::GetSystemMetrics( SM_CYVSCROLL ), LAYOUT_Horizontal, FALSE, LAYOUT_Master, TRUE, TAG_END );
	m_Buttons.Create( *this, Offsets( 0, 0, 0, 0 ), LAYOUT_EqualMinWidth, TRUE, TAG_END );

	// Attach controls.
	m_Static.Attach( GetDlgItemHandle( IDC_INSERT ));
	m_Ok.Attach( GetDlgItemHandle( IDC_OK ));
	m_Cancel.Attach( GetDlgItemHandle( IDC_CANCEL ));

	// Setup brainchild control.
	m_Edit.Attach( GetDlgItemHandle( IDC_EDIT ));
	m_Edit.SetColor( BCC_TEXT, CLR_DEFAULT );
	m_Edit.SetColor( BCC_BACKGROUND, CLR_DEFAULT );

	// Setup button group.
	m_Buttons.AddSpacingMember();
	m_Buttons.AddMember( &m_Ok, NULL, ATTR_FixMinWidth, TRUE, ATTR_UseControlSize, TRUE, TAG_END );
	m_Buttons.AddMember( &m_Cancel, NULL, ATTR_FixMinWidth, TRUE, ATTR_UseControlSize, TRUE, TAG_END );

	// Setup master group.
	m_Master.AddMember( &m_Static, NULL, ATTR_FixMinHeight, TRUE, ATTR_UseControlSize, TRUE, TAG_END );
	m_Master.AddMember( &m_Edit, NULL, ATTR_UseControlSize, TRUE, TAG_END );
	m_Master.AddMember( &m_Buttons, NULL, ATTR_FixMinHeight, TRUE, TAG_END );

	// OK?
	if ( m_Master.Error())
	{
		EndDialog( FALSE );
		return FALSE;
	}

	// Compute the master it's minimum size.
	if ( m_Master.OnGetMinSize( m_MinSize ) == FALSE )
	{
		EndDialog( FALSE );
		return FALSE;
	}

	// Add frames to the minimum size.
	m_MinSize.CY() += ( ::GetSystemMetrics( SM_CYFRAME ) * 2 ) + ::GetSystemMetrics( SM_CYCAPTION );
	m_MinSize.CX() += ::GetSystemMetrics( SM_CXFRAME ) * 2;

	// Layout the master layout engine.
	m_Master.Relayout();

	// Convert the node text.
	LPTSTR pszText = CNode2Str( m_pCNode->pcStr );
	if ( pszText == NULL )
	{
		// Error... bliep...
		MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
		EndDialog( FALSE );
		return FALSE;
	}

	// Put it into the edit control and
	// deallocate it.
	m_Edit.SetText( pszText, TRUE );
	::FreePooled( pParserPool, pszText );

	// Setup icon.
	SetIcon( m_hIcon, FALSE );
	return TRUE;
}

// Convert a string to a CNode string.
LPTSTR TextInsert::Str2CNode( LPCTSTR pszString )
{
	// Compute the length necessary to
	// store the conversion.
	int nLength = _tcslen( pszString ), i, a, nNewLength;
	for ( i = 0, nNewLength = 0; i < nLength; i++ )
	{
		if (      pszString[ i ] == _T( '\n' )) nNewLength += 2;
		else if ( pszString[ i ] != _T( '\r' )) nNewLength ++;
	}

	// Allocate string.
	LPTSTR pszRc = ( LPTSTR )::AllocPooled( pParserPool, ( nNewLength + 1 ) * sizeof( TCHAR ));
	if ( pszRc != NULL )
	{
		// Copy the string and convert
		// it to the CNode form.
		for ( i = 0, a = 0; i < nLength; i++ )
		{
			if ( pszString[ i ] == _T( '\n' ))
			{
				pszRc[ a++ ] = _T( '\\' );
				pszRc[ a++ ] = _T( 'n' );
			}
			else if ( pszString[ i ] != _T( '\r' ))
				pszRc[ a++ ] = pszString[ i ];
		}
		// Terminate.
		pszRc[ a ] = 0;
	}
	return pszRc;
}

// Convert a CNode string to a normal string.
LPTSTR TextInsert::CNode2Str( LPCTSTR pszString )
{
	// Compute the length necessary to
	// store the conversion.
	int nLength = _tcslen( pszString ), i, a, nNewLength;
	for ( i = 0, nNewLength = 0; i < nLength; i++ )
	{
		if ( pszString[ i ] == _T( '\\' ) && i < nLength - 1 && pszString[ i + 1 ] == _T( 'n' ))
		{
			i++;
			nNewLength += 2;
		}
		else
			nNewLength++;
	}

	// Allocate string.
	LPTSTR pszRc = ( LPTSTR )::AllocPooled( pParserPool, ( nNewLength + 1 ) * sizeof( TCHAR ));
	if ( pszRc != NULL )
	{
		// Copy the string and convert
		// it to the string form.
		for ( i = 0, a = 0; i < nLength; i++ )
		{
			if ( pszString[ i ] == _T( '\\' ) && i < nLength - 1 && pszString[ i + 1 ] == _T( 'n' ))
			{
				i++;
				pszRc[ a++ ] = _T( '\r' );
				pszRc[ a++ ] = _T( '\n' );
			}
			else	
				pszRc[ a++ ] = pszString[ i ];
		}
		// Terminate.
		pszRc[ a ] = 0;
	}
	return pszRc;
}

// WM_NOTIFY overide.
LRESULT TextInsert::OnNotify( LPNMHDR pNMHDR )
{
	switch ( pNMHDR->code )
	{
		case	NMBC_STATUSUPDATE:
			// Disable the OK button
			// when there is no text
			// in the editor control.
			::EnableWindow( GetDlgItemHandle( IDC_OK ), (( LPNMSTATUSUPDATE )pNMHDR )->bAnyText );
			return 0;
	}
	return ClsSizeDialog::OnNotify( pNMHDR );
}

LRESULT TextInsert::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What the ID?
	switch ( nCtrlID )
	{
		case	IDC_CANCEL:
			// Cancel...
			EndDialog( FALSE );
			return 0;

		case	IDC_OK:
		{
			// Get the text.
			BOOL bRC = FALSE;
			HGLOBAL hContent = ( HGLOBAL )m_Edit.GetText();
			if ( hContent != NULL )
			{
				// Lock it...
				LPTSTR pszLocked = ( LPTSTR )::GlobalLock( hContent );
				if ( pszLocked != NULL )
				{
					// Create converted copy.
					LPTSTR pszCopy = Str2CNode( pszLocked );
					if ( pszCopy != NULL )
					{
						// Free the old contents and
						// setup the new.
						::FreePooled( pParserPool, m_pCNode->pcStr );
						m_pCNode->pcStr = ( LPTSTR )pszCopy;

						// We are OK...
						bRC = TRUE;
					}
					// Unlock the memory handle.
					::GlobalUnlock( hContent );
				}
				// Free the global memory.
				::GlobalFree( hContent );
			}

			// OK?
			if ( bRC == FALSE )
				MessageBox( ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
			
			// Bye...
			EndDialog( bRC );
			return 0;
		}
	}
	// Pass onto the base class.
	return ClsSizeDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// Open the hardcoded command selector.
BOOL TextInsert::Edit( ClsWindow& Parent, LPCNODE pCNode, HICON hIcon )
{
	// Save data.
	m_pCNode = pCNode;
	m_hIcon = hIcon;

	// Open the dialog.
	return DoModal( Parent, IDD_INSERTEDIT );
}
