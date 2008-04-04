//
//	datetime.cpp
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
DateTimePage::DateTimePage() 
{
	// Setup icon.
	m_nIcon = IDI_DATETIME;
}

// Destructor.
DateTimePage::~DateTimePage()
{
}

// Refresh page contents.
void DateTimePage::RefreshData( LPPARSER pParser )
{
	_ASSERT_VALID( pParser );

	// Save parser.
	m_pParser = pParser;

	// Setup the edit controls.
	m_DFormat.SetWindowText( pParser->pszDateFormat );
	m_TFormat.SetWindowText( pParser->pszTimeFormat );

	// Simulate a timer event.
	SendMessage( WM_TIMER, IDT_UPDATE, 0L );
}

// Save page/parser contents.
BOOL DateTimePage::SavePage( ClsStdioFile *pFile, LPPARSER pParser )
{
	try
	{
		// Write date settings comment.
		pFile->PrintF( ClsString( MAKEINTRESOURCE( IDS_COMMENT_DATETIME )));

		// Write strings.
		if ( pParser->pszDateFormat && *pParser->pszDateFormat ) pFile->PrintF( _T( "DateFormat=%s\n" ), pParser->pszDateFormat );
		if ( pParser->pszTimeFormat && *pParser->pszTimeFormat ) pFile->PrintF( _T( "TimeFormat=%s\n" ), pParser->pszTimeFormat );
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
LRESULT DateTimePage::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Is it our timer event?
	if ( uMsg == WM_TIMER && wParam == IDT_UPDATE )
	{
		// Are we visible?
		if ( IsWindowVisible())
		{
			TCHAR		szBuffer[ 1024 ];
			ClsString	fmt;

			// Convert and set time string.
			m_TFormat.GetWindowText( fmt );
			if ( m_pParser && GetTimeFormat( LOCALE_SYSTEM_DEFAULT, fmt.GetStringLength() ? 0 : LOCALE_NOUSEROVERRIDE, 0, fmt, szBuffer, 1024 ) > 0 )
				m_TSample.SetWindowText( szBuffer );

			// Convert and set date string.
			m_DFormat.GetWindowText( fmt );
			if ( m_pParser && GetDateFormat( LOCALE_SYSTEM_DEFAULT, fmt.GetStringLength() ? 0 : LOCALE_NOUSEROVERRIDE, 0, fmt, szBuffer, 1024 ) > 0 )
				m_DSample.SetWindowText( szBuffer );
			
			return 0;
		}
	}
	else if ( uMsg == WM_DESTROY )
		// Kill the timer...
		KillTimer( IDT_UPDATE );

	// Pass onto the base class.
	return Page::WindowProc( uMsg, wParam, lParam );
}

// WM_COMMAND handler.
LRESULT DateTimePage::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What's the message?
	switch ( nCtrlID )
	{
		case	IDC_DATE:
			// Get the new value.
			HandleEditControl( nNotifyCode, nCtrlID, &m_pParser->pszDateFormat );

			// Simulate a timer event.
			SendMessage( WM_TIMER, IDT_UPDATE, 0L );
			return 0;

		case	IDC_TIME:
			// Get the new value.
			HandleEditControl( nNotifyCode, nCtrlID, &m_pParser->pszTimeFormat );

			// Simulate a timer event.
			SendMessage( WM_TIMER, IDT_UPDATE, 0L );
			return 0;
	}
	// Pass to the base class.
	return Page::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// WM_INITDIALOG handler.
LRESULT DateTimePage::OnInitDialog( PROPSHEETPAGE * pPropSheetPage )
{
	// Setup static controls.
	m_STFormat.Attach( GetDlgItemHandle( IDC_ST_DTF ));
	m_STSample.Attach( GetDlgItemHandle( IDC_ST_DTS ));	

	// Setup edit controls.
	m_DFormat.Attach( GetDlgItemHandle( IDC_DATE ));
	m_TFormat.Attach( GetDlgItemHandle( IDC_TIME ));

	// Setup the static controls.
	m_DSample.Attach( GetDlgItemHandle( IDC_DATE_SAMP ));
	m_TSample.Attach( GetDlgItemHandle( IDC_TIME_SAMP ));

	// Fire up a timer.
	SetTimer( IDT_UPDATE, 1000, NULL );

	// Call the base-class.
	return Page::OnInitDialog( pPropSheetPage );
}
