//
//	keyrec.cpp
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

// KeyRecEdit windows procedure.
LRESULT KeyRecEdit::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// What's up?
	switch ( uMsg )
	{
		case	WM_GETDLGCODE:
			// We want 'm all.
			return DLGC_WANTALLKEYS;

		case	WM_CHAR:
		case	WM_DEADCHAR:
		case	WM_SYSCHAR:
		case	WM_SYSDEADCHAR:
			// We intercept these...
			return 0;

		case	WM_KEYUP:
		case	WM_SYSKEYUP:
			// Check if a qualifier
			// key was released.
			if (	  wParam == VK_SHIFT   ) m_cQual &= ~QUAL_SHIFT;
			else if ( wParam == VK_CONTROL ) m_cQual &= ~QUAL_CTRL;
			else if ( wParam == VK_MENU    ) m_cQual &= ~QUAL_ALT;
			return 0;

		case	WM_KEYDOWN:
		case	WM_SYSKEYDOWN:
		{
			TCHAR		szKeyString[ 64 ];
			BYTE		bStates[ 256 ];

			// No repeated keys.
			if ( ! ( lParam & ( 1 << 30 )))
			{
				// Qualifier pressed?
				if (	  wParam == VK_SHIFT   ) { m_cQual |= QUAL_SHIFT; return 0; }
				else if ( wParam == VK_CONTROL ) { m_cQual |= QUAL_CTRL;  return 0; }
				else if ( wParam == VK_MENU    ) { m_cQual |= QUAL_ALT;   return 0; }

				// Get the keyboard state.
				if ( ::GetKeyboardState( bStates ))
				{
					// Copy code value.
					m_cCode = ( TCHAR )wParam;

					// Convert the values into a
					// human readable form.
					::Brainchild_CodeQual2Str(( TCHAR )wParam, m_cQual, szKeyString );

					// Display and copy the text.
					SetWindowText( szKeyString );
					m_KeyString = szKeyString;
					return 0;
				}
				// Clear the control contents.
				SetWindowText( NULL );
				m_KeyString.Empty();
			}
			return 0;
		}
	}
	// Base class.
	return ClsEdit::WindowProc( uMsg, wParam, lParam );
}

// Construction.
KeyRec::KeyRec()
{
}

// Destruction.
KeyRec::~KeyRec()
{
}

LRESULT KeyRec::OnInitDialog( LPPROPSHEETPAGE pPsp )
{
	// Setup edit control.
	m_Key.Attach( GetDlgItemHandle( IDC_RECORD_EDIT ));
	return TRUE;
}

LRESULT KeyRec::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What's up.
	switch ( nCtrlID )
	{
		case	IDC_RECORD_EDIT:
			::EnableWindow( GetDlgItemHandle( IDC_RECORD_OK ), ( BOOL )( m_Key.GetWindowTextLength()));
			return 0;

		case	IDC_RECORD_CANCEL:
			*m_pKeyString = _T( "" );
			EndDialog( FALSE );
			return 0;

		case	IDC_RECORD_OK:
			// Copy the entered key string.
			m_pKeyString->GetWindowText( m_Key );
			EndDialog( TRUE );
			return 0;
	}

	// Pass onto the base class.
	return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// Open the block editor.
BOOL KeyRec::KeyRecord( ClsWindow& Parent, ClsString& str )
{
	// Setup string.
	m_pKeyString = &str;
	
	// Open the dialog.
	return DoModal( Parent, IDD_KEYRECORD );
}
