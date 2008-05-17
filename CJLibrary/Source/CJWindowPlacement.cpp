// CJWindowPlacement.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/25/99 10:52p $
 * $Revision: 7 $
 * $Archive: /CodeJock/CJLibrary/CJWindowPlacement.cpp $
 *
 * $History: CJWindowPlacement.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 10/25/99   Time: 10:52p
 * Updated in $/CodeJock/CJLibrary
 * Modified resource include for static builds.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:14p
 * Updated in $/CodeJockey/CJLibrary
 * Added Unicode compliance, thanks to Barry Burton for his help with
 * this.
 * 
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 6/11/99    Time: 10:15p
 * Updated in $/CodeJockey/CJ60Lib
 * Modified LoadWindowPos(...) to allow registry names to be directly
 * passed in.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 6/11/99    Time: 9:41p
 * Updated in $/CodeJockey/CJ60Lib
 * Initial creation.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 6/01/99    Time: 9:29p
 * Created in $/CodeJockey/CJ60Lib
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CJResource.h"
#include "CJWindowPlacement.h"

/////////////////////////////////////////////////////////////////////////////
// CCJWindowPlacement

CCJWindowPlacement::CCJWindowPlacement()
{
	length					= sizeof( WINDOWPLACEMENT );
	flags					= NULL;
	showCmd					= SW_SHOWMAXIMIZED;
	ptMinPosition.x			= 0;
	ptMinPosition.y			= 0;
	ptMaxPosition.x			= 0;
	ptMaxPosition.y			= 0;
	rcNormalPosition.left	= 0;
	rcNormalPosition.top	= 0;
	rcNormalPosition.right	= 0;
	rcNormalPosition.bottom = 0;
}

CCJWindowPlacement::~CCJWindowPlacement()
{
}

/////////////////////////////////////////////////////////////////////////////

void CCJWindowPlacement::SaveWindowPos(CFrameWnd *pFrameWnd)
{
	ASSERT_VALID( pFrameWnd ); // must be valid.

	// Get the current window position.
	pFrameWnd->GetWindowPlacement( this );

	// Save position to registry.
	CString strBuffer;
	strBuffer.Format( m_strFormat,
		flags,
		showCmd,
		ptMinPosition.x,
		ptMinPosition.y,
		ptMaxPosition.x,
		ptMaxPosition.y,
		rcNormalPosition.left,
		rcNormalPosition.top,
		rcNormalPosition.right,
		rcNormalPosition.bottom);

	AfxGetApp()->WriteProfileString( m_strSection, m_strWndPos, strBuffer );
}

BOOL CCJWindowPlacement::LoadWindowPos( CFrameWnd* pFrameWnd, LPCTSTR lpszFormat/*=NULL*/, LPCTSTR lpszSection/*=NULL*/, LPCTSTR lpszWndPos/*=NULL*/ )
{
	ASSERT_VALID( pFrameWnd ); // must be valid.

	// load the strings for the registry.
	if( lpszFormat == NULL )
		m_strFormat.LoadString( IDS_WIN_PLACEMENT );
	else
		m_strFormat = lpszFormat;

	if( lpszSection == NULL )
		m_strSection.LoadString( IDS_SETTINGS );
	else
		m_strSection = lpszSection;
	
	if( lpszWndPos == NULL )
		m_strWndPos.LoadString( IDS_WNDPOS	);
	else
		m_strWndPos = lpszWndPos;

	// Load the previous window placement settings.
	CString strBuffer = AfxGetApp()->GetProfileString( m_strSection, m_strWndPos, NULL );

	int nRead = _stscanf( strBuffer, m_strFormat,
		&flags,
		&showCmd,
		&ptMinPosition.x,
		&ptMinPosition.y,
		&ptMaxPosition.x,
		&ptMaxPosition.y,
		&rcNormalPosition.left,
		&rcNormalPosition.top,
		&rcNormalPosition.right,
		&rcNormalPosition.bottom);

	// Set the window position.
	if( nRead == 10 )
	{
		pFrameWnd->SetWindowPlacement( this );
		pFrameWnd->ShowWindow( showCmd );
		pFrameWnd->UpdateWindow();
		return TRUE;
	}

	return FALSE;
}
