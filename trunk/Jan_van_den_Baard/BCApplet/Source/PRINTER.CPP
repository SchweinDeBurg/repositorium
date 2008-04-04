//
//	printer.cpp
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
PrinterPage::PrinterPage() 
{
	// Setup icon.
	m_nIcon = IDI_PRINTER;
}

// Destructor.
PrinterPage::~PrinterPage()
{
}

// Refresh page contents.
void PrinterPage::RefreshData( LPPARSER pParser )
{
	_ASSERT_VALID( pParser );

	// Save the parser pointer.
	m_pParser = pParser;

	// Update checkboxes.
	m_Header.SetCheck(   pParser->bPrintHeader	   ? BST_CHECKED : BST_UNCHECKED );
	m_Footer.SetCheck(   pParser->bPrintFooter	   ? BST_CHECKED : BST_UNCHECKED );
	m_Colors.SetCheck(   pParser->bPrintSyntaxColoring ? BST_CHECKED : BST_UNCHECKED );
	m_LineNums.SetCheck( pParser->bPrintLineNumbers    ? BST_CHECKED : BST_UNCHECKED );

	// Fill edit controls.
	m_HLeft.SetWindowText(  pParser->pszHeaderLeft );
	m_HMid.SetWindowText(   pParser->pszHeaderCenter );
	m_HRight.SetWindowText( pParser->pszHeaderRight );
	m_FLeft.SetWindowText(  pParser->pszFooterLeft );
	m_FMid.SetWindowText(   pParser->pszFooterCenter );
	m_FRight.SetWindowText( pParser->pszFooterRight );

	// Setup radiobuttons and font selection button.
	m_Screen.SetCheck( pParser->bPrintScreenFont ? BST_CHECKED : BST_UNCHECKED );
	m_Select.SetCheck( pParser->bPrintScreenFont ? BST_UNCHECKED : BST_CHECKED );
	m_Font.EnableWindow( ! pParser->bPrintScreenFont );
}

// Save page/parser copntents.
BOOL PrinterPage::SavePage( ClsStdioFile *pFile, LPPARSER pParser )
{
	try
	{
		// Printer comment.
		pFile->PrintF( ClsString( MAKEINTRESOURCE( IDS_COMMENT_PRINT )));

		// Write header/footer texts.
		if ( pParser->pszHeaderLeft   && *pParser->pszHeaderLeft   ) pFile->PrintF( _T( "HeaderLeft=%s\n"   ), pParser->pszHeaderLeft );
		if ( pParser->pszHeaderCenter && *pParser->pszHeaderCenter ) pFile->PrintF( _T( "HeaderCenter=%s\n" ), pParser->pszHeaderCenter );
		if ( pParser->pszHeaderRight  && *pParser->pszHeaderRight  ) pFile->PrintF( _T( "HeaderRight=%s\n"  ), pParser->pszHeaderRight );
		if ( pParser->pszFooterLeft   && *pParser->pszFooterLeft   ) pFile->PrintF( _T( "FooterLeft=%s\n"   ), pParser->pszFooterLeft );
		if ( pParser->pszFooterCenter && *pParser->pszFooterCenter ) pFile->PrintF( _T( "FooterCenter=%s\n" ), pParser->pszFooterCenter );
		if ( pParser->pszFooterRight  && *pParser->pszFooterRight  ) pFile->PrintF( _T( "FooterRight=%s\n"  ), pParser->pszFooterRight );

		// And the booleans.
		pFile->PrintF( _T( "PrintSyntaxColoring=%lc\n"	), pParser->bPrintSyntaxColoring ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "PrintHeader=%lc\n"		), pParser->bPrintHeader	 ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "PrintFooter=%lc\n"		), pParser->bPrintFooter	 ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "PrintScreenFont=%lc\n"	), pParser->bPrintScreenFont	 ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "PrintLineNumbers=%lc\n"	), pParser->bPrintLineNumbers	 ? _T( 'Y' ) : _T( 'N' ));

		// Write the printer font.
		pFile->PrintF( _T( "PrinterFont=%s,%ld,%ld,%ld,%ld,%ld\n" ), pParser->lfPrinterFont.lfFaceName, 
									     pParser->lfPrinterFont.lfHeight, 
									     pParser->lfPrinterFont.lfWeight, 
									     pParser->lfPrinterFont.lfCharSet, 
									     pParser->lfPrinterFont.lfItalic, 
									     pParser->lfPrinterFont.lfUnderline );
	}
	
	catch( ClsException& e )
	{
		// Error.
		UNREFERENCED_PARAMETER( e );
		return FALSE;
	}

	return TRUE;
}

// WM_COMMAND message handler.
LRESULT PrinterPage::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What's the ID?
	switch ( nCtrlID )
	{
		case	IDC_PRINT_HEADER:
			// Setup new value.
			m_pParser->bPrintHeader = ( BOOL )( m_Header.GetCheck() == BST_CHECKED );
			pSettings->Changed( m_pParser );
			return 0;

		case	IDC_PRINT_FOOTER:
			// Setup new value.
			m_pParser->bPrintFooter = ( BOOL )( m_Footer.GetCheck() == BST_CHECKED );
			pSettings->Changed( m_pParser );
			return 0;

		case	IDC_COLORS:
			// Setup new value.
			m_pParser->bPrintSyntaxColoring = ( BOOL )( m_Colors.GetCheck() == BST_CHECKED );
			pSettings->Changed( m_pParser );
			return 0;

		case	IDC_LINENUMS:
			// Setup new value.
			m_pParser->bPrintLineNumbers = ( BOOL )( m_LineNums.GetCheck() == BST_CHECKED );
			pSettings->Changed( m_pParser );
			return 0;

		case	IDC_SELECT:
		case	IDC_SCREEN:
			// Enable/disable the font button.
			m_Font.EnableWindow(( BOOL )( nCtrlID == IDC_SELECT ));

			// Setup new value.
			m_pParser->bPrintScreenFont = ( BOOL )( m_Screen.GetCheck() == BST_CHECKED );
			pSettings->Changed( m_pParser );
			return 0;

		case	IDC_HLEFT:
			// Handle the control.
			HandleEditControl( nNotifyCode, nCtrlID, &m_pParser->pszHeaderLeft );
			return 0;

		case	IDC_HMID:
			// Handle the control.
			HandleEditControl( nNotifyCode, nCtrlID, &m_pParser->pszHeaderCenter );
			return 0;

		case	IDC_HRIGHT:
			// Handle the control.
			HandleEditControl( nNotifyCode, nCtrlID, &m_pParser->pszHeaderRight );
			return 0;

		case	IDC_FLEFT:
			// Handle the control.
			HandleEditControl( nNotifyCode, nCtrlID, &m_pParser->pszFooterLeft );
			return 0;

		case	IDC_FMID:
			// Handle the control.
			HandleEditControl( nNotifyCode, nCtrlID, &m_pParser->pszFooterCenter );
			return 0;

		case	IDC_FRIGHT:
			// Handle the control.
			HandleEditControl( nNotifyCode, nCtrlID, &m_pParser->pszFooterRight );
			return 0;

		case	IDC_FONT:
		{
			// Copy the current font.
			LOGFONT lf = m_pParser->lfPrinterFont;

			// Popup a font selection dialog.
			ClsFontDialog fd;
			if ( fd.DoModal( this, &lf, CF_BOTH | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT ))
			{
				// Copy the contents.
				m_pParser->lfPrinterFont = lf;
			
				// Changes made...
				pSettings->Changed( m_pParser );
			}
			return 0;
		}
	}
	// Pass to the base class.
	return Page::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}		

// Dialog initialization.
LRESULT PrinterPage::OnInitDialog( PROPSHEETPAGE *pPsp )
{
	// Setup static controls.
	m_STHead.Attach( GetDlgItemHandle( IDC_ST_HEAD ));
	m_STFoot.Attach( GetDlgItemHandle( IDC_ST_FOOT ));
	m_STOpt.Attach(  GetDlgItemHandle( IDC_ST_OPT ));

	// Setup checkbox controls.
	m_Header.Attach(  GetDlgItemHandle( IDC_PRINT_HEADER ));
	m_Footer.Attach(  GetDlgItemHandle( IDC_PRINT_FOOTER ));
	m_Colors.Attach(  GetDlgItemHandle( IDC_COLORS ));
	m_LineNums.Attach( GetDlgItemHandle( IDC_LINENUMS ));

	// Setup edit controls.
	m_HLeft.Attach(  GetDlgItemHandle( IDC_HLEFT  ));
	m_HMid.Attach(   GetDlgItemHandle( IDC_HMID   ));
	m_HRight.Attach( GetDlgItemHandle( IDC_HRIGHT ));
	m_FLeft.Attach(  GetDlgItemHandle( IDC_FLEFT  ));
	m_FMid.Attach(   GetDlgItemHandle( IDC_FMID   ));
	m_FRight.Attach( GetDlgItemHandle( IDC_FRIGHT ));

	// Setup radiobuttons.
	m_Screen.Attach( GetDlgItemHandle( IDC_SCREEN ));
	m_Select.Attach( GetDlgItemHandle( IDC_SELECT ));

	// And the button.
	m_Font.Attach( GetDlgItemHandle( IDC_FONT ));

	// Pass onto the base class.
	return Page::OnInitDialog( pPsp );
}
