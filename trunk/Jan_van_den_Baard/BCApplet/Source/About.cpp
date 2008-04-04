//
//	about.cpp
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
AboutPage::AboutPage()
{
	// Setup icon.
	m_nIcon = IDI_ABOUT;
}

// Destructor.
AboutPage::~AboutPage()
{
}

// Dialog initialization.
LRESULT AboutPage::OnInitDialog( PROPSHEETPAGE *pPsp )
{
	// Setup the controls.
	m_Copyright.Attach( GetDlgItemHandle( IDC_COPYRIGHT ));
	m_Copyright.SetWindowText( MAKEINTRESOURCE( IDS_COPYRIGHT ));
	m_Disclaimer.Attach( GetDlgItemHandle( IDC_DISCLAIMER ));
	m_Disclaimer.SetWindowText( MAKEINTRESOURCE( IDS_DISCLAIMER ));
	m_Http.Attach( GetDlgItemHandle( IDC_HTTP ));
	m_Http.URL() = _T( "http://users.bart.nl/~baard" );
	m_Http.ShowToolTip();
	m_Email.Attach( GetDlgItemHandle( IDC_EMAIL ));
	m_Email.SetWindowText( _T( "jcbaar@croon.nl" ));
	m_Email.URL() = _T( "mailto:jcbaar@croon.nl" );
	m_Email.ShowToolTip();
	m_Email.ShowFocus() = FALSE;
	return Page::OnInitDialog( pPsp );
}

// WM_COMMAND handler.
LRESULT AboutPage::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// More button clicked?
	if ( nCtrlID == IDC_MORE )
	{
		// About box (Not localized)
		ClsMessageBox	mb;
		mb.Body().Format( ISEQ_BOLD ISEQ_UNDERLINE _T( "Brainchild Configuration Tool\n" )
				  ISEQ_NORMAL _T( "(C) Copyright 1999-2005 by Jan van den Baard\n" )
				  _T( "All Rights Reserved\n\n" )
			          _T( "Version %ld.%ld" )
				  #ifdef _DEBUG
				  ISEQ_TEXTRGB( _T( "255,0,0" )) _T( ".Debug" ) ISEQ_TEXTSYSCOLOR( IG_COLOR_BTNTEXT )
				  #endif
				  _T( "\nCompiled: %s (%s)\n\n" )
				  _T( "Parts of this program by:\n\n" )
				  ISEQ_TEXTRGB( _T( "0,0,150" ) ) _T( "ColourPopup" ) ISEQ_TEXTSYSCOLOR( IG_COLOR_BTNTEXT )
				  _T( " - (C) 1998 Chris Maunder ( www.codeproject.com )\n" )
				  ISEQ_TEXTRGB(  _T( "0,0,150" ) ) _T( "BalloonHelp" ) ISEQ_TEXTSYSCOLOR( IG_COLOR_BTNTEXT )
				  _T( " - (C) 2001 Joshua Heyer\n" )
				  ISEQ_TEXTRGB(  _T( "0,0,150" ) ) _T( "OddButton" ) ISEQ_TEXTSYSCOLOR( IG_COLOR_BTNTEXT )
				  _T( " - (C) 2001 Paolo Messina and Jerzy Kaczorowski" )
				  , BCC_VERSION, BCC_REVISION, __DATE__, __TIME__ );
		mb.Buttons() = _T( "*&OK");
		mb.Title()   = ClsGetApp()->GetAppTitle();
		mb.Flags()   = ClsMessageBox::MBF_ICONCUSTOM;
		mb.Icon()    = ClsGetApp()->GetIcon();
		mb.MsgBox( GetParent()->GetSafeHWND());
		return 0;
	}

	// Call the base class.
	return Page::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}
