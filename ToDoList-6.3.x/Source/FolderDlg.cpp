/////////////////////////////////////////////////////////////////////////////
/* 
DESCRIPTION:
	CFolderDialog  - Folder Selection Dialog Class	
	http://www.codeproject.com/dialog/cfolderdialog.asp

NOTES:
	Copyright(C) Armen Hakobyan, 2002
	mailto:armen.h@web.am
	
VERSION HISTORY:
	24 Mar 2002 - First release
	30 Mar 2003 - Some minor changes
				- Added missing in old Platform SDK new flag definitions  
				- Added support for both MFC 6.0 and 7.0
				- Added OnIUnknown handler for Windows XP folder filtration
				- Added SetExpanded and SetOKText and GetSelectedFolder functions
	24 May 2003 - Added OnSelChanged implementation
	14 Jul 2003 - Added custom filtration for Windows XP, thanks to Arik Poznanski
	29 Nov 2003 - Added SetRootFolder, thanks to Eckhard Schwabe ( and Jose Insa )
	02 Jan 2004 - Added GetRootFolder, uncomment if needed
*/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FolderDlg.h"

#if defined(__INTEL_COMPILER)
// remark #171: invalid type conversion
#pragma warning(disable: 171)
// remark #174: expression has no effect
#pragma warning(disable: 174)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
// remark #593: variable was set but never used
#pragma warning(disable: 593)
#endif	// __INTEL_COMPILER

/////////////////////////////////////////////////////////////////////////////

#ifndef BFFM_VALIDATEFAILED
	#ifndef UNICODE
		#define BFFM_VALIDATEFAILED		3
	#else
		#define BFFM_VALIDATEFAILED		4	
	#endif
#endif

#ifndef BFFM_IUNKNOWN
	#define BFFM_IUNKNOWN				5
#endif

/////////////////////////////////////////////////////////////////////////////
// CFolderDialog

IMPLEMENT_DYNAMIC( CFolderDialog, CDialog )

CFolderDialog::CFolderDialog( IN LPCTSTR pszTitle	 /*NULL*/,
							  IN LPCTSTR pszSelPath	 /*NULL*/,
							  IN CWnd*	 pWndParent	 /*NULL*/,
							  IN UINT	 uFlags		 /*BIF_RETURNONLYFSDIRS*/ )
			 : CCommonDialog( pWndParent )
			 , m_hWnd( NULL )
{	
	ASSERT( AfxIsValidString( pszSelPath, MAX_PATH ) );
	
	::ZeroMemory( &m_bi, sizeof( BROWSEINFO ) );
	::ZeroMemory( m_szFolPath, MAX_PATH );
	::ZeroMemory( m_szSelPath, MAX_PATH );
	
	if( pszSelPath != NULL )
		SetSelectedFolder( pszSelPath );
	
	// Fill data	
	m_bi.hwndOwner	= pWndParent->GetSafeHwnd();
	m_bi.pidlRoot	= NULL;	
	m_bi.lpszTitle	= pszTitle;
	m_bi.ulFlags	= uFlags;
	m_bi.lpfn		= (BFFCALLBACK)BrowseCallbackProc;
	m_bi.lParam		= (LPARAM)this;

	// The size of this buffer is assumed to be MAX_PATH bytes:
	
	m_bi.pszDisplayName = new TCHAR[ MAX_PATH ];
	ASSERT( m_bi.pszDisplayName != NULL );

	::ZeroMemory( m_bi.pszDisplayName, ( MAX_PATH * sizeof( TCHAR ) ) );
}

CFolderDialog::~CFolderDialog( void )
{	
	_coTaskMemFree( m_bi.pidlRoot );
	_delete2( m_bi.pszDisplayName );

	::ZeroMemory( &m_bi, sizeof( BROWSEINFO ) );	
}

BEGIN_MESSAGE_MAP( CFolderDialog, CCommonDialog )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFolderDialog message handlers

// SetRootFolder By Jose Insa
// Microsoft knowledge Base Article
// ID Q132750: Convert a File Path to an ITEMIDLIST

BOOL CFolderDialog::SetRootFolder( IN LPCTSTR pszPath )
{
	ASSERT_VALID( this );

	if( pszPath == NULL )
	{
		_coTaskMemFree( m_bi.pidlRoot );
		return ( TRUE );
	}

	ASSERT( AfxIsValidString( pszPath, MAX_PATH ) );

	HRESULT		  hResult	  = S_FALSE;
	IShellFolder* pDeskFolder = NULL;

	if ( ( hResult = ::SHGetDesktopFolder( &pDeskFolder ) ) == S_OK )
	{
		LPITEMIDLIST pidlRoot = NULL;
		LPTSTR       pszRoot  = const_cast< LPTSTR >( pszPath );

		// Convert the path to an ITEMIDLIST:
				
		USES_CONVERSION;

		hResult = pDeskFolder->ParseDisplayName(
			NULL, NULL, T2W( pszRoot ), NULL, &pidlRoot, NULL 
		);

		if ( hResult == S_OK )
		{
			_coTaskMemFree( m_bi.pidlRoot );
			m_bi.pidlRoot = pidlRoot;
		}

		_releaseInterface( pDeskFolder );		
	}
	
	return ( hResult == S_OK );
} 

// NOTE: pszPath buffer must be at least
// MAX_PATH characters in size:

BOOL CFolderDialog::GetRootFolder( IN OUT LPTSTR pszPath )
{
	ASSERT_VALID( this );
	ASSERT( AfxIsValidString( pszPath, MAX_PATH ) );

	return ::SHGetPathFromIDList( m_bi.pidlRoot, pszPath );
} 

/////////////////////////////////////////////////////////////////////////////

#if ( _MFC_VER >= 0x0700 )
	INT_PTR CFolderDialog::DoModal( void )
#else
	INT		CFolderDialog::DoModal( void )
#endif
{
	ASSERT_VALID( this );	
	ASSERT( m_bi.lpfn != NULL );
		
	m_bi.hwndOwner = PreModal();	
	INT_PTR nRet   = -1;

	LPITEMIDLIST pItemIDList = ::SHBrowseForFolder( &m_bi );
	if( pItemIDList )
	{
		if( ::SHGetPathFromIDList( pItemIDList, m_szFolPath ) )
			nRet = IDOK;
		else
			nRet = IDCANCEL;

		_coTaskMemFree( pItemIDList );
	}

	PostModal();
	return ( nRet );
}

/////////////////////////////////////////////////////////////////////////////
// Overridables:

void CFolderDialog::OnInitialized( void )
{
	if( ::lstrlen( m_szSelPath ) > 0 )
		SetSelection( m_szSelPath );
}

void CFolderDialog::OnSelChanged( IN LPITEMIDLIST pItemIDList )
{
	if( m_bi.ulFlags & BIF_STATUSTEXT )
	{
		TCHAR szSelFol[ MAX_PATH ] = { 0 };
		if( ::SHGetPathFromIDList( pItemIDList, szSelFol ) )
			SetStatusText( szSelFol );
	}
}

INT CFolderDialog::OnValidateFailed( IN LPCTSTR /*pszPath*/ )
{	
	::MessageBeep( MB_ICONHAND );
	return 1; // Return 1 to leave dialog open, 0 - to end one
}

void CFolderDialog::OnIUnknown( IN IUnknown* /*pIUnknown*/ )
{
}

/////////////////////////////////////////////////////////////////////////////
// Callback function used with the SHBrowseForFolder function. 

INT CALLBACK CFolderDialog::BrowseCallbackProc( IN HWND   hWnd, 
												IN UINT   uMsg, 
												IN LPARAM lParam, 
												IN LPARAM lpData )
{
	CFolderDialog* pThis = (CFolderDialog*)lpData;
	pThis->m_hWnd = hWnd;
	INT nRet = 0;

	switch( uMsg )
	{
	case BFFM_INITIALIZED:
		pThis->OnInitialized();
		break;
	case BFFM_SELCHANGED:
		pThis->OnSelChanged( (LPITEMIDLIST)lParam );
		break;
	case BFFM_VALIDATEFAILED:
		nRet = pThis->OnValidateFailed( (LPCTSTR)lParam );
		break;
	case BFFM_IUNKNOWN:
		pThis->OnIUnknown( (IUnknown*)lParam );
		break;
	default:
		ASSERT( FALSE );
		break;
	}

	pThis->m_hWnd = NULL;
	return nRet;	
}

/////////////////////////////////////////////////////////////////////////////

void CFolderDialog::SetExpanded( IN LPCTSTR pszPath )
{
	USES_CONVERSION;

	ASSERT( m_hWnd != NULL );
	ASSERT( AfxIsValidString( pszPath, MAX_PATH ) );
	
	::SendMessage( m_hWnd, BFFM_SETEXPANDED, (WPARAM)TRUE, (LPARAM)T2CW( pszPath ) );
}

void CFolderDialog::SetOKText( IN LPCTSTR pszText )
{
	USES_CONVERSION;
	ASSERT( m_hWnd != NULL );	
	
	::SendMessage( m_hWnd, BFFM_SETOKTEXT, (WPARAM)0, (LPARAM)T2CW( pszText ) );
}


/////////////////////////////////////////////////////////////////////////////
