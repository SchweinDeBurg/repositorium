//
//	general.cpp
//
//	(C) Copyright 2000-2005 by Jan van den Baard.
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
GeneralPage::GeneralPage() 
{
	// Setup icon.
	m_nIcon = IDI_GENERAL;
}

// Destructor.
GeneralPage::~GeneralPage()
{
}

// Refresh page contents.
void GeneralPage::RefreshData( LPPARSER pParser )
{
	_ASSERT_VALID( pParser );

	// Save parser.
	m_pParser = pParser;
	
	// Setup option tree.
	m_Options.SetOptions( pParser );
	m_Options.SelectItem( m_Options.GetRoot());

	// Setup edit controls.
	m_Class.SetWindowText( pParser->bDefault ? ClsString( MAKEINTRESOURCE( IDS_DEFAULT_TITLE )) : pParser->pszTitle );
	m_Ext.SetWindowText( pParser->bDefault ? _T( "" ) : pParser->pszExtentions );
	m_Delim.SetWindowText( pParser->pszDelimiters );
	SetDlgItemInt( IDC_HOPS, pParser->nUndoSize, FALSE );
	SetDlgItemInt( IDC_COLMARKER_OFFSET, pParser->nMarkerOffset, FALSE );

	// And the other controls.
	m_Caret.SetCurSel( pParser->nCaretType );
	m_UDHops.SetPos( pParser->nUndoSize );
	m_UDHist.SetPos( pParser->nMaxTrack );
	m_UDMarkerOfs.SetPos( pParser->nMarkerOffset );
	m_Syntax.SetCheck( pParser->bGlobalSyntaxColoring ? BST_CHECKED : BST_UNCHECKED );
	m_Marker.SetCheck( pParser->bColumnMarker ? BST_CHECKED : BST_UNCHECKED );

	// Disable controls.
	m_Class.EnableWindow( ! pParser->bDefault );
	m_Ext.EnableWindow( ! pParser->bDefault );
	m_MarkerOfs.EnableWindow( pParser->bColumnMarker );
	m_UDMarkerOfs.EnableWindow( pParser->bColumnMarker );

	// Hide the global settings controls if this is not
	// the default parser.
	m_Syntax.ShowWindow( pParser->bDefault ? SW_SHOW : SW_HIDE );
	m_Hops.ShowWindow( pParser->bDefault ? SW_SHOW : SW_HIDE );
	m_UDHops.ShowWindow( pParser->bDefault ? SW_SHOW : SW_HIDE );
	m_Hist.ShowWindow( pParser->bDefault ? SW_SHOW : SW_HIDE );
	m_UDHist.ShowWindow( pParser->bDefault ? SW_SHOW : SW_HIDE );
	m_STGlobal.ShowWindow( pParser->bDefault ? SW_SHOW : SW_HIDE );
	m_Sep.ShowWindow( pParser->bDefault ? SW_SHOW : SW_HIDE );
	m_Label1.ShowWindow( pParser->bDefault ? SW_SHOW : SW_HIDE );
	m_Label2.ShowWindow( pParser->bDefault ? SW_SHOW : SW_HIDE );

	// Compute the optionlist size. When this is the
	// default parser the list is smaller than when
	// it is not.
	ClsRect rcOpt = m_Options.GetWindowRect();
	ClsRect rcHst = m_Hist.GetWindowRect();
	BOOL bMoveOptions = FALSE;

	// Is this the default parser?
	if ( ! m_pParser->bDefault ) 
	{
		// Did the size really change?
		if ( rcOpt.Bottom() != rcHst.Bottom())
		{
			rcOpt.Bottom() = rcHst.Bottom();
			bMoveOptions = TRUE;
		}
	}
	else 
	{
		// Did the size really change?
		if ( rcOpt.Bottom() != ( rcOpt.Bottom() + ( LONG )m_nListHeight ))
		{
			rcOpt.Bottom() = rcOpt.Top() + m_nListHeight;
			bMoveOptions = TRUE;
		}
	}

	// Move the optionlist if necessary.
	if ( bMoveOptions )
	{
		ScreenToClient( rcOpt );
		m_Options.MoveWindow( rcOpt, TRUE );
	}
}

// Save page/parser contents.
BOOL GeneralPage::SavePage( ClsStdioFile *pFile, LPPARSER pParser )
{
	try
	{
		// Save comment.
		pFile->PrintF( ClsString( MAKEINTRESOURCE( IDS_COMMENT_GENERAL )), pParser->pszTitle );

		// Save strings.
		if ( pParser->pszTitle      && *pParser->pszTitle )				       pFile->PrintF( _T( "Name=%s\n" ), pParser->pszTitle );
		if ( pParser->pszExtentions && *pParser->pszExtentions && pParser->bDefault == FALSE ) pFile->PrintF( _T( "Extentions=%s\n" ), pParser->pszExtentions );
		if ( pParser->pszDelimiters && *pParser->pszDelimiters )			       pFile->PrintF( _T( "Delimiters=%s\n" ), pParser->pszDelimiters );


		// Write booleans.
		pFile->PrintF( _T( "AutoMatch=%lc\n" ),		    pParser->bAutoMatch		  ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "VisibleAutoMatchError=%lc\n" ), pParser->bVisibleMatchError	  ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "ClearSpaces=%lc\n" ),	    pParser->bClearTrailingSpaces ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "DnDEnabled=%lc\n" ),	    pParser->bDnDEnabled	  ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "SelectionMargin=%lc\n" ),	    pParser->bSelectionMargin	  ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "SolidMargin=%lc\n" ),	    pParser->bSolidMargin	  ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "Default=%lc\n" ),		    pParser->bDefault		  ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "FastScroll=%lc\n" ),	    pParser->bFastScroll	  ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "ClearUndoAfterSave=%lc\n" ),    pParser->bClearUndoSave	  ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "VisibleSpaces=%lc\n" ),	    pParser->bVisibleSpaces	  ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "ShowLineNumbers=%lc\n" ),	    pParser->bLineNumbers	  ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "CaseFixing=%lc\n" ),	    pParser->bCaseFixing	  ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "SolidMargin=%lc\n" ),	    pParser->bSolidMargin	  ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "ColorBracketMatches=%lc\n" ),   pParser->bColorBracketMatches ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "HighlightLine=%lc\n" ),	    pParser->bHighlightCurrentLine? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "ParseHyperlinks=%lc\n" ),	    pParser->bParseHyperLinks	  ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "ShowHyperTips=%lc\n" ),	    pParser->bShowHyperTips	  ? _T( 'Y' ) : _T( 'N' ));

		// Write caret style.
		pFile->PrintF( _T( "CaretType=%ld\n" ), pParser->nCaretType );

		// Column marker.
		pFile->PrintF( _T( "ColumnMarker=%lc\n" ),	    pParser->bColumnMarker	  ? _T( 'Y' ) : _T( 'N' ));
		pFile->PrintF( _T( "ColumnMarkerOffset=%ld\n" ),    pParser->nMarkerOffset );

		// Default?
		if ( pParser->bDefault )
		{
			// Write settings only available in
			// the default parser mode.
			pFile->PrintF( _T( "UndoSize=%ld\n" ),		   pParser->nUndoSize );
			pFile->PrintF( _T( "GlobalSyntaxColoring=%lc\n" ), pParser->bGlobalSyntaxColoring ? _T( 'Y' ) : _T( 'N' ));
			pFile->PrintF( _T( "MaxTrack=%ld\n" ),		   pParser->nMaxTrack );
		}
	}
	catch ( ClsException& e )
	{
		// Error...
		UNREFERENCED_PARAMETER( e );
		return FALSE;
	}
	return TRUE;
}

// WM_COMMAND handler.
LRESULT GeneralPage::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// What's the message?
	switch ( nCtrlID )
	{
		case	IDC_CLASS:
			// Change string.
			HandleEditControl( nNotifyCode, nCtrlID, &m_pParser->pszTitle );

			// Setup the name into the tree aswell.
			TVITEM tvi;
			tvi.mask = TVIF_TEXT;
			tvi.pszText = m_pParser->pszTitle;
			tvi.hItem = pSettings->GetTree()->GetSelection();
			pSettings->GetTree()->SetItem( tvi );			
			return 0;

		case	IDC_EXT:
			// Change string.
			HandleEditControl( nNotifyCode, nCtrlID, &m_pParser->pszExtentions );
			return 0;

		case	IDC_DELIM:
			// Change string.
			HandleEditControl( nNotifyCode, nCtrlID, &m_pParser->pszDelimiters );
			return 0;

		case	IDC_HIST:
		{
			// Loosing focus?
			if ( nNotifyCode == EN_KILLFOCUS )
			{
				// Get value.
				int nValue = GetDlgItemInt( IDC_HIST, NULL, FALSE );
				
				// Is it in range?
				if ( nValue < 10 || nValue > 30 )
				{
					// No. Give a message and reset the value.
					ClsRect rc;
					m_Hist.GetWindowRect( rc );
					ScreenToClient( rc );
					ClsBalloonHelp::LaunchBalloon( ClsGetApp()->GetAppTitle(), MAKEINTRESOURCE( IDS_HIST_RANGE ), rc.CenterPoint(), IDI_WARNING, ClsBalloonHelp::unSHOW_INNER_SHADOW | ClsBalloonHelp::unCLOSE_ON_MOUSE_MOVE, this );
					SetDlgItemInt( IDC_HIST, m_pParser->nMaxTrack, FALSE );
					m_Hist.SetSel( 0, m_Hist.GetWindowTextLength());
					m_Hist.SetFocus();
					return 0;
				}

				// Set the value.
				m_pParser->nMaxTrack = nValue;
			}
			// Value changed?
			else if ( nNotifyCode == EN_CHANGE && ( ::GetFocus() == GetDlgItemHandle( nCtrlID )))			
				pSettings->Changed( m_pParser );
			return 0;
		}

		case	IDC_HOPS:
		{
			// Loosing focus?
			if ( nNotifyCode == EN_KILLFOCUS )
			{
				// Get value.
				int nValue = GetDlgItemInt( IDC_HOPS, NULL, FALSE );
				
				// Is it in range?
				if ( nValue < 100 || nValue > 100000 )
				{
					// No. Give a message and reset the value.
					ClsRect rc;
					m_Hops.GetWindowRect( rc );
					ScreenToClient( rc );
					ClsBalloonHelp::LaunchBalloon( ClsGetApp()->GetAppTitle(), MAKEINTRESOURCE( IDS_HOPS_RANGE ), rc.CenterPoint(), IDI_WARNING, ClsBalloonHelp::unSHOW_INNER_SHADOW | ClsBalloonHelp::unCLOSE_ON_MOUSE_MOVE, this );
					SetDlgItemInt( IDC_HOPS, m_pParser->nUndoSize, FALSE );
					m_Hops.SetSel( 0, m_Hops.GetWindowTextLength());
					m_Hops.SetFocus();
					return 0;
				}

				// Set the value.
				m_pParser->nUndoSize = nValue;
			}
			// Value changed?
			else if ( nNotifyCode == EN_CHANGE && ( ::GetFocus() == GetDlgItemHandle( nCtrlID )))			
				pSettings->Changed( m_pParser );
			return 0;
		}

		case	IDC_COLMARKER_OFFSET:
		{
			// Loosing focus?
			if ( nNotifyCode == EN_KILLFOCUS )
			{
				// Get value.
				int nValue = GetDlgItemInt( IDC_COLMARKER_OFFSET, NULL, FALSE );
				
				// Is it in range?
				if ( nValue < 0 || nValue > 256 )
				{
					// No. Give a message and reset the value.
					ClsRect rc;
					m_MarkerOfs.GetWindowRect( rc );
					ScreenToClient( rc );
					ClsBalloonHelp::LaunchBalloon( ClsGetApp()->GetAppTitle(), MAKEINTRESOURCE( IDS_ERROR_COLUMNMARKER ), rc.CenterPoint(), IDI_WARNING, ClsBalloonHelp::unSHOW_INNER_SHADOW | ClsBalloonHelp::unCLOSE_ON_MOUSE_MOVE, this );
					SetDlgItemInt( IDC_COLMARKER_OFFSET, m_pParser->nMarkerOffset, FALSE );
					m_MarkerOfs.SetSel( 0, m_MarkerOfs.GetWindowTextLength());
					m_MarkerOfs.SetFocus();
					return 0;
				}

				// Set the value.
				m_pParser->nMarkerOffset = nValue;
			}
			// Value changed?
			else if ( nNotifyCode == EN_CHANGE && ( ::GetFocus() == GetDlgItemHandle( nCtrlID )))			
				pSettings->Changed( m_pParser );
			return 0;
		}

		case	IDC_COLMARKER:
			// Get new value.
			m_pParser->bColumnMarker = ( BOOL )( m_Marker.GetCheck() == BST_CHECKED );
			m_MarkerOfs.EnableWindow( m_pParser->bColumnMarker );
			pSettings->Changed( m_pParser );
			return 0;

		case	IDC_SYNTAX:
			// Get new value.
			m_pParser->bGlobalSyntaxColoring = ( BOOL )( m_Syntax.GetCheck() == BST_CHECKED );
			pSettings->Changed( m_pParser );
			return 0;

		case	IDC_CARET:
			// Get new value.
			if ( nNotifyCode == LBN_SELCHANGE )
			{
				m_pParser->nCaretType = m_Caret.GetCurSel();
				pSettings->Changed( m_pParser );
			}
			return 0;
	}
	// Pass to the base class.
	return Page::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// WM_INITDIALOG handler.
LRESULT GeneralPage::OnInitDialog( PROPSHEETPAGE * pPropSheetPage )
{
	// Setup the option treeview.
	m_Options.Attach( GetDlgItemHandle( IDC_OPTIONS ));
	m_Options.Initialize();

	// Setup static controls.
	m_STGeneral.Attach( GetDlgItemHandle( IDC_ST_GEN ));
	m_STOptions.Attach( GetDlgItemHandle( IDC_ST_OPT ));	
	m_STGlobal.Attach(  GetDlgItemHandle( IDC_ST_GLOB ));
	m_Sep.Attach(       GetDlgItemHandle( IDC_SEP ));
	m_Label1.Attach(    GetDlgItemHandle( IDC_LABEL1 ));
	m_Label2.Attach(    GetDlgItemHandle( IDC_LABEL2 ));

	// Setup edit controls.
	m_Class.Attach( GetDlgItemHandle( IDC_CLASS ));
	m_Ext.Attach(   GetDlgItemHandle( IDC_EXT ));
	m_Delim.Attach( GetDlgItemHandle( IDC_DELIM ));
	m_Hops.Attach( GetDlgItemHandle(  IDC_HOPS ));
	m_Hist.Attach( GetDlgItemHandle(  IDC_HIST ));
	m_MarkerOfs.Attach( GetDlgItemHandle( IDC_COLMARKER_OFFSET ));

	// Setup checkboxes.
	m_Syntax.Attach( GetDlgItemHandle( IDC_SYNTAX ));
	m_Marker.Attach( GetDlgItemHandle( IDC_COLMARKER ));

	// And the other controls...
	m_Caret.Attach( GetDlgItemHandle( IDC_CARET ));
	m_UDHops.Attach( GetDlgItemHandle( IDC_UDHOPS ));
	m_UDHist.Attach( GetDlgItemHandle( IDC_UDHIST ));
	m_UDMarkerOfs.Attach( GetDlgItemHandle( IDC_UD_OFFSET ));

	// Setup combo-box entries.
	m_Caret.AddString( ClsString( MAKEINTRESOURCE( IDS_NORMAL )));
	m_Caret.AddString( ClsString( MAKEINTRESOURCE( IDS_WIDE )));
	m_Caret.AddString( ClsString( MAKEINTRESOURCE( IDS_BLOCK )));
	m_Caret.AddString( ClsString( MAKEINTRESOURCE( IDS_HORIZONTAL )));

	// And the up/down control ranges.
	m_UDHops.SetRange32( 100, 100000 );
	m_UDHist.SetRange32( 10, 30 );
	m_UDMarkerOfs.SetRange32( 0, 256 );
	
	// Store optionlist height.
	ClsRect rc = m_Options.GetWindowRect();
	m_nListHeight = rc.Height();

	// Call the base-class.
	return Page::OnInitDialog( pPropSheetPage );
}
