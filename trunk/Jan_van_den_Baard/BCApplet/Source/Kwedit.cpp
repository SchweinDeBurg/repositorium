//
//	kwedit.cpp
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
KWEdit::KWEdit() : m_hIcon( NULL )
{
}

// Destruction.
KWEdit::~KWEdit()
{
	// Destroy icon.
	if ( m_hIcon ) ::DestroyIcon( m_hIcon );
}

// Window procedure override.
LRESULT KWEdit::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
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
LRESULT KWEdit::OnSize( UINT nSizeType, int nWidth, int nHeight )
{
	// Re-layout the master layout engine.
	m_Master.Relayout();

	// Pass to the base class.
	return ClsSizeDialog::OnSize( nSizeType, nWidth, nHeight );
}

LRESULT KWEdit::OnInitDialog( LPPROPSHEETPAGE pPsp )
{
	// Create layout engines.
	m_Master.Create( *this, LAYOUT_BottomOffset, ::GetSystemMetrics( SM_CYVSCROLL ), LAYOUT_Horizontal, FALSE, LAYOUT_Master, TRUE, TAG_END );
	m_Buttons.Create( *this, Offsets( 0, 0, 0, 0 ), LAYOUT_EqualMinWidth, TRUE, TAG_END );

	// Attach controls.
	m_Static.Attach( GetDlgItemHandle( IDC_TITLE ));
	m_Ok.Attach( GetDlgItemHandle( IDC_OK ));
	m_Cancel.Attach( GetDlgItemHandle( IDC_CANCEL ));

	// Setup brainchild control.
	m_Edit.Attach( GetDlgItemHandle( IDC_EDIT ));
	m_Edit.SetColor( BCC_TEXT, m_pKW->crColor );
	m_Edit.SetColor( BCC_BACKGROUND, m_crBkgnd );

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

	// Loadup the keyword file.
	if ( m_Edit.LoadFile( m_szOutput, TRUE ) != BCMR_OK )
		EndDialog( FALSE );

	// Load and setup the icon.
	m_hIcon = ( HICON )::LoadImage( ClsGetResourceHandle(), MAKEINTRESOURCE( IDI_SYNTAX ), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
	SetIcon( m_hIcon, FALSE );
	return TRUE;
}

LRESULT KWEdit::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What's the ID?
	switch ( nCtrlID )
	{
		case	IDC_CANCEL:
			// Cancel.
			EndDialog( FALSE );
			return 0;

		case	IDC_OK:
		{
			// Make sure the file is
			// saved.
			if ( m_Edit.SaveFile( m_szOutput ) == BCMR_OK )
			{
				// Delete the current contents.
				for ( int i = 0; i < ::Brainchild_ArrayGetSize( m_pKW->lpaKeywords ); i++ )
					// Free the string contents.
					::Brainchild_ArrayFreeMem( m_pKW->lpaKeywords, *(( LPTSTR * )::Brainchild_ArrayGetAt( m_pKW->lpaKeywords, i )));

				// Empty the array.
				::Brainchild_ArrayRemoveAll( m_pKW->lpaKeywords );
				EndDialog( TRUE );
				return 0;
			}
			else
				// Error.
				MessageBox( ClsString( MAKEINTRESOURCE( IDS_ERROR_SAVETEMP )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
			
			// Bye...
			EndDialog( FALSE );
			return 0;
		}
	}
	// Pass onto the base class.
	return ClsSizeDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// Save the keywords to a temporary
// file.
BOOL KWEdit::SaveKeywords()
{
	// Get the temporary path.
	TCHAR	szFilePath[ MAX_PATH ];
	if ( ::GetTempPath( MAX_PATH, szFilePath ))
	{
		// Construct a temporary file name.
		if ( ::GetTempFileName( szFilePath, _T( "bcp" ), 0, m_szOutput ))
		{
			try
			{
				ClsStdioFile file;

				// Open the output file.
				file.Open( m_szOutput, _T( "wa" ));

				// Write keywords.
				for ( int i = 0; i < ::Brainchild_ArrayGetSize( m_pKW->lpaKeywords ); i++ )
					file.PrintF( _T( "%s\n" ), *(( LPSTRING * )::Brainchild_ArrayGetAt( m_pKW->lpaKeywords, i )));

				// Close the file.
				file.Close();
			}
			catch( ClsException& ex )
			{
				UNREFERENCED_PARAMETER( ex );
				::DeleteFile( m_szOutput );
				return FALSE;
			}
		}
	}
	return TRUE;
}

// Open the keysword editor.
void KWEdit::Edit( ClsWindow& Parent, LPKEYWORDS pKW, KeywordEdit *pKWE )
{
	// Save values.
	m_pKW = pKW;
	m_crBkgnd = pKW->crBgColor;
	
	// Save the keywords.
	if ( SaveKeywords())
	{
		// Popup the dialog.
		if ( DoModal( Parent, IDD_KWEDIT ))
		{
			// Setup the file.
			pKWE->ReadKeywords( m_szOutput );

			// Make sure the keywords are
			// displayed.
			pKWE->SendMessage( WM_COMMAND, MAKEWPARAM( IDC_LIST, LBN_SELCHANGE ), ( LPARAM )pKWE->m_List.GetSafeHWND());
		}

		// Delete the output file.
		::DeleteFile( m_szOutput );
	}
}
