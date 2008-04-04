//
//	hardcoded.cpp
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
Hardcoded::Hardcoded()
{
}

// Destruction.
Hardcoded::~Hardcoded()
{
}

LRESULT Hardcoded::OnInitDialog( LPPROPSHEETPAGE pPsp )
{
	// Setup listbox control.
	m_List.Attach( GetDlgItemHandle( IDC_LIST ));


	// Add commands to the list.
	LPCOMMAND lpCTable = ::GetCommandTable();
	int nSel;
	while ( lpCTable->lpfnFunc )
	{
		// Add command description.
		nSel = m_List.AddString( lpCTable->pszCommandDesc );

		// Current selection?
		if ( m_pCNode->lpFunc == ( LPFUNC )lpCTable->lpfnFunc )
			// Yes. Select it.
			m_List.SetCurSel( nSel );

		// Next...
		lpCTable++;
	}

	// Make sure the current selection
	// is visible.
	m_List.SetCurSel( m_List.GetCurSel());
	return TRUE;
}

LRESULT Hardcoded::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What the ID?
	switch ( nCtrlID )
	{
		case	IDC_CANCEL:
			// Cancel...
			EndDialog( FALSE );
			return 0;

		case	IDC_LIST:
			// Entry double-clicked?
			if ( nNotifyCode == LBN_DBLCLK )
				// Simulate user clicking
				// the OK button.
				SendMessage( WM_COMMAND, MAKEWPARAM( IDC_OK, BN_CLICKED ), ( LPARAM )GetDlgItemHandle( IDC_OK ));
			return 0;

		case	IDC_OK:
		{
			// Selection OK?
			int nSel = m_List.GetCurSel();
			if ( nSel != LB_ERR )
			{
				// Copy entry text.
				TCHAR szEntry[ 256 ];
				m_List.GetText( nSel, szEntry );

				// Find the correct command.
				LPCOMMAND lpCTable = ::GetCommandTable();
				while ( lpCTable->lpfnFunc )
				{
					// Is this the one?
					if ( ! _tcsicmp( lpCTable->pszCommandDesc, szEntry ))
					{
						// Store function pointer.
						m_pCNode->lpFunc = ( LPFUNC )lpCTable->lpfnFunc;
						
						// Bye...
						EndDialog( TRUE );
						return 0;
					}
					// Next...
					lpCTable++;
				}
			}
			// Bye...
			EndDialog( FALSE );
			return 0;
		}
	}
	// Pass onto the base class.
	return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// Open the hardcoded command selector.
BOOL Hardcoded::Select( ClsWindow& Parent, LPCNODE pCNode )
{
	// Save node.
	m_pCNode = pCNode;

	// Open the dialog.
	return DoModal( Parent, IDD_HARDCODED );
}
