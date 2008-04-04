//
//	classname.cpp
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
ClassName::ClassName()
{
}

// Destruction.
ClassName::~ClassName()
{
}

LRESULT ClassName::OnInitDialog( LPPROPSHEETPAGE pPsp )
{
	// Setup edit control.
	m_Edit.Attach( GetDlgItemHandle( IDC_NAME ));
	return TRUE;
}

LRESULT ClassName::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What the ID?
	switch ( nCtrlID )
	{
		case	IDC_NAME:
			// Any characters typed?
			::EnableWindow( GetDlgItemHandle( IDC_OK ), ( BOOL )( m_Edit.GetWindowTextLength()));
			break;

		case	IDC_OK:
		{
			// Allocate name buffer.
			m_Edit.GetWindowText( *m_pName );

			// Extention or path
			// supplied?
			if (( m_pName->Find( _T( '.' ), 0 ) >= 0 ) || ( m_pName->Find( _T( '\\' ), 0 ) >= 0 ) || ( m_pName->Find( _T( ':' ), 0 ) >= 0 ))
			{
				// Show error.
				ClsRect rc;
				m_Edit.GetWindowRect( rc );
				ScreenToClient( rc );
				ClsBalloonHelp::LaunchBalloon( ClsGetApp()->GetAppTitle(), MAKEINTRESOURCE( IDS_ERROR_NAME ), rc.CenterPoint(), IDI_ERROR, ClsBalloonHelp::unSHOW_INNER_SHADOW | ClsBalloonHelp::unSHOW_CLOSE_BUTTON | ClsBalloonHelp::unCLOSE_ON_KEYPRESS, this );

				// Focus edit control.
				m_Edit.SetFocus();
				m_Edit.SetSel( 0, m_Edit.GetWindowTextLength());
				break;
			}

			// Add file extention.
			*m_pName += _T( ".bcp" );

			// Safe to use this name?
			for ( LPPARSER pParser = plParsers.lpFirst; pParser->lpNext; pParser = pParser->lpNext )
			{
				// Mathing filename?
				if ( ! m_pName->CompareNoCase( pParser->pszFileName ))
				{
					// Show error.
					ClsRect rc;
					m_Edit.GetWindowRect( rc );
					ScreenToClient( rc );
					ClsBalloonHelp::LaunchBalloon( ClsGetApp()->GetAppTitle(), MAKEINTRESOURCE( IDS_ERROR_SAMENAME ), rc.CenterPoint(), IDI_ERROR, ClsBalloonHelp::unSHOW_INNER_SHADOW | ClsBalloonHelp::unSHOW_CLOSE_BUTTON | ClsBalloonHelp::unCLOSE_ON_KEYPRESS, this );
					
					// Focus edit control.
					m_Edit.SetFocus();
					m_Edit.SetSel( 0, m_Edit.GetWindowTextLength());
					return FALSE;
				}
			}

			// Return OK.
			EndDialog( TRUE );
			break;
		}

		case	IDC_CANCEL:
			EndDialog( FALSE );
			return 0;
	}
	// Pass onto the base class.
	return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// Open the classname editor.
BOOL ClassName::Name( ClsWindow& Parent, ClsString& name )
{
	// Save string.
	m_pName = &name;

	// Open the dialog.
	return DoModal( Parent, IDD_CLASSNAME );
}
