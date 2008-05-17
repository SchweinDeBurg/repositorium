// CJBrowseEdit.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/31/99 11:35p $
 * $Revision: 14 $
 * $Archive: /CodeJock/CJLibrary/CJBrowseEdit.cpp $
 *
 * $History: CJBrowseEdit.cpp $
 * 
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:35p
 * Updated in $/CodeJock/CJLibrary
 * Modified resource include for static builds.
 * 
 * Fixed potential resource and memory leak problems.
 * 
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:14p
 * Updated in $/CodeJockey/CJLibrary
 * Added Unicode compliance, thanks to Barry Burton for his help with
 * this.
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:15a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 6/25/99    Time: 11:03a
 * Updated in $/CodeJockey/CJLibrary
 * Chaned image list color mask and export macros.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 6/14/99    Time: 10:41p
 * Updated in $/CodeJockey/CJ60Lib
 * Cleaned up control and pointer initialization, creation and activation.
 * Added support for CCJMenu popup.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 6/01/99    Time: 11:57p
 * Updated in $/CodeJockey/CJ60Lib
 * Minor fix with build using error level 4 warning.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 5/29/99    Time: 12:53a
 * Updated in $/CodeJockey/CJ60Lib
 * Changed reference to CCJShellFile to CCJShell, to reflect class
 * renaming.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 10:54p
 * Updated in $/CodeJockey/CJ60Lib
 * Added comments and cleaned up code.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 1/16/99    Time: 10:53p
 * Created in $/CodeJockey/CJ60Lib
 * Initial release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJBrowseEdit.h"
#include "CJShell.h"
#include "CJMenu.h"        // CCJMenu class declaration
#include "CJResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJBrowseButton class
/////////////////////////////////////////////////////////////////////////////

CCJBrowseButton::CCJBrowseButton()
{
	m_pParentWnd = NULL;
	m_pEditWnd	 = NULL;
	m_hIcon		 = NULL;
}

CCJBrowseButton::~CCJBrowseButton()
{
	// fix potential resource leak - KStowell - 10-22-99.
	m_Font.DeleteObject();
	_destroyIcon(m_hIcon);
}

BEGIN_MESSAGE_MAP(CCJBrowseButton, CButton)
	//{{AFX_MSG_MAP(CCJBrowseButton)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCJBrowseButton::Create(CWnd* pWnd, BROWSE_TYPE eType, UINT nMenu, UINT nSubMenuIndx)
{
	ASSERT( eType );
	ASSERT_VALID( pWnd );
	ASSERT_VALID( pWnd->GetParent() );

	m_pEditWnd		= pWnd;
	m_pParentWnd	= m_pEditWnd->GetParent();
	m_eType			= eType;
	m_nMenu			= nMenu;
	m_nSubMenuIndx	= nSubMenuIndx;
	
	CRect rc;
	m_pEditWnd->GetWindowRect( &rc );
	m_pEditWnd->SetWindowPos( NULL, 0, 0, rc.Width()-23, rc.Height(), SWP_NOZORDER|SWP_NOMOVE );
	m_pParentWnd->ScreenToClient( &rc );
	rc.left = rc.right-18;

	DWORD dwStyle = WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_CENTER|BS_VCENTER;
	
	// if this is a browse for directory or file edit control.
	if(( m_eType == BT_DIRECTORY ) || ( m_eType == BT_FILE ))
	{
		if( !CButton::Create(_T("..."), dwStyle, rc, m_pParentWnd, GetNextID( m_pParentWnd )))
		{
			TRACE0( "Failed to create browse edit browse button.\n" );
			return FALSE;
		}

		// set the tab order and font, enable the window based on parent's state.
		SetWindowPos( m_pEditWnd, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE );
		EnableWindow( m_pEditWnd->IsWindowEnabled( ));
		UpdateFont();
		
		return TRUE;
	}

	// if this is a popup menu edit control.
	else if( m_eType == BT_POPUPMENU )
	{
		if( !CButton::Create( NULL, dwStyle|BS_ICON, rc, m_pParentWnd, GetNextID( m_pParentWnd )))
		{
			TRACE0( "Failed to create browse edit browse button.\n" );
			return FALSE;
		}

		// set the tab order and font, enable the window based on parent's state.
		SetWindowPos( m_pEditWnd, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE );
		EnableWindow( m_pEditWnd->IsWindowEnabled( ));
		UpdateFont();

		// create the icon arrow that is displayed on the button.
		CImageList imageList;
		CBitmap    bitmap;

		bitmap.LoadBitmap( IDB_BTN_ARROW );
		imageList.Create( 15, 17, ILC_COLORDDB|ILC_MASK, 1, 1 );
		imageList.Add( &bitmap, RGB( 255,0,255 ));

		// set the icon and free resources.
		// fix potential resource leak - KStowell - 10-22-99.
		m_hIcon = imageList.ExtractIcon( 0 );
		SetIcon(m_hIcon);
		imageList.Detach();
		bitmap.DeleteObject();

		return TRUE;
	}

	return FALSE;
}

void CCJBrowseButton::OnClicked()
{
	if( m_eType == BT_DIRECTORY )
	{
		CCJShell sfi;
		sfi.m_strTitle = m_strTitle;

		if( sfi.BrowseForFolder( m_pParentWnd ) == IDOK ) {
			m_pEditWnd->SetWindowText( sfi.m_strPath );
		}
	}

	else if( m_eType == BT_FILE )
	{
		CFileDialog dlg( m_bOpenFileDialog, m_strDefExt, m_strFileName, 
			m_dwFlags, m_strFilter, m_pParentWnd );

		if( dlg.DoModal() == IDOK ) {
			m_pEditWnd->SetWindowText( dlg.GetPathName( ));
		}
	}

	else if( m_eType == BT_POPUPMENU )
	{
		ASSERT( m_nMenu != 0 );
		SetState( true );

		CRect rc;
		GetWindowRect( &rc );

		CMenu menu;
		VERIFY( menu.LoadMenu( m_nMenu ));

		CCJMenu* pPopup = (CCJMenu*)menu.GetSubMenu( m_nSubMenuIndx );
		ASSERT( pPopup != NULL );

		pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
			rc.right, rc.top, m_pParentWnd, &rc );

		// Return the button state to normal.
		SetState( false );
	}
}

void CCJBrowseButton::UpdateFont()
{
	m_Font.DeleteObject();
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(ncm);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
	VERIFY(m_Font.CreateFontIndirect(&ncm.lfMessageFont));
	SetFont(&m_Font);
}

/////////////////////////////////////////////////////////////////////////////
// CCJBrowseEdit

CCJBrowseEdit::CCJBrowseEdit() : m_eType( BT_DIRECTORY ), m_nMenu( (UINT)-1 )
{

}

CCJBrowseEdit::~CCJBrowseEdit()
{
}

BEGIN_MESSAGE_MAP(CCJBrowseEdit, CEdit)
	//{{AFX_MSG_MAP(CCJBrowseEdit)
	ON_WM_ENABLE()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJBrowseEdit message handlers

void CCJBrowseEdit::OnEnable(BOOL bEnable) 
{
	CEdit::OnEnable(bEnable);
	m_btnBrowse.EnableWindow(bEnable);
}

void CCJBrowseEdit::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CEdit::OnShowWindow( bShow, nStatus );
	m_btnBrowse.ShowWindow( bShow?SW_SHOW:SW_HIDE );
}

void CCJBrowseEdit::PreSubclassWindow() 
{
	CEdit::PreSubclassWindow();

	// initialize control to defaults.
	InitializeControl();
}

int CCJBrowseEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// initialize control to defaults.
	InitializeControl();

	return 0;
}

void CCJBrowseEdit::InitializeControl()
{
	// create the browse button associated with this control.
	if( m_btnBrowse.Create( this, m_eType, m_nMenu, m_nSubMenuIndx ))
	{
		// initialize some defaults
		SetDlgOpenFile();
		SetDlgDefExt( _T( "*.*" ));
		SetDlgFileName();
		SetDlgStyle( OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_EXPLORER );
		SetDlgFilter( _T( "All Files (*.*)|*.*||" ));
		SetDlgOwner( GetParent( ));
		SetDlgTitle( _T( "Select Folder" ));
	}
}
