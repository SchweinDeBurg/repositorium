/////////////////////////////////////////////////////////////////////////////
/* 
DESCRIPTION:
	CIconDialog  - Icon Selection Dialog Class
	http://www.codeproject.com/dialog/cicondialogapi.asp

NOTES:
	Copyright(C) Armen Hakobyan, 2002
	mailto:armenh@web.am

	Undocumented SelectIcon APIs by Henk Devos
	Article _T("How to display the Pick Icon Dialog") :)
	http://www.codeproject.com/shell/selecticon.asp
	
VERSION HISTORY:
	09 Mar 2002 - First release
	04 Nov 2002 - Fixed support for MFC 7.0 ( "int DoModal( void )" to INT_PTR )
				- Disable warning for ExtractIcon to compile with Level 4
				- Changed ExpandEnvironmentStrings to DoEnvironmentSubst, 
				  no temp variable needed
				- Changed OS detection to GetVersion ( shorter )
*/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IconDialog.h"

/////////////////////////////////////////////////////////////////////////////

typedef
BOOL ( WINAPI* PICKICONDLGPROC )(
	HWND		hWndParent, 
	LPTSTR		lpszFilename, 
	LPDWORD		lpdwBufferSize, 
	LPDWORD		lpdwIndex
);

BOOL SelectIconA( IN HWND		 hWndParent, 
				  IN OUT LPSTR   lpszFilename, 
				  IN DWORD		 dwBufferSize, 
				  IN OUT LPDWORD lpdwIndex)
{
	BOOL bResult = FALSE;		
	HMODULE hShell32 = ::LoadLibrary( _T( "Shell32.dll" ) );

	if ( hShell32 != NULL )
	{
		PICKICONDLGPROC lpfnPickIconDlg = 
			(PICKICONDLGPROC)::GetProcAddress( hShell32, (LPCSTR)62 );

		if ( lpfnPickIconDlg != NULL )
		{
			if ( ::GetVersion() < 0x80000000 )	// Windows NT/2000/XP
			{
				LPWSTR lpwszWideName = new WCHAR[ dwBufferSize ];
				::MultiByteToWideChar( CP_ACP, 0, lpszFilename, -1, lpwszWideName, dwBufferSize );
				bResult = lpfnPickIconDlg( hWndParent, (LPTSTR)lpwszWideName, &dwBufferSize, lpdwIndex );
				::WideCharToMultiByte( CP_ACP, 0, lpwszWideName, -1, lpszFilename, dwBufferSize, NULL, NULL );				
				_delete2( lpwszWideName );
			}
			else
			{
				bResult = lpfnPickIconDlg( hWndParent, 
					(LPTSTR)lpszFilename, &dwBufferSize, lpdwIndex );
			}

			lpfnPickIconDlg = NULL;
		}
		
		_freeLibrary( hShell32 );
	}
	
	return bResult;
}


BOOL SelectIconW( IN HWND		 hWndParent, 
				  IN OUT LPWSTR  lpszFilename, 
				  IN DWORD		 dwBufferSize, 
				  IN OUT LPDWORD lpdwIndex )
{
	BOOL bResult = FALSE;	
	HMODULE hShell32 = ::LoadLibrary( _T( "Shell32.dll" ) );

	if( hShell32 != NULL )
	{
		PICKICONDLGPROC lpfnPickIconDlg = 
			(PICKICONDLGPROC)::GetProcAddress( hShell32, (LPCSTR)62 );

		if ( lpfnPickIconDlg != NULL )
		{
			if( ::GetVersion() < 0x80000000 )	// Windows NT/2000/XP
			{
				bResult = lpfnPickIconDlg( hWndParent, 
					(LPTSTR)lpszFilename, &dwBufferSize, lpdwIndex );
			}
			else
			{
				LPSTR lpszName = new CHAR[ dwBufferSize ];
				::WideCharToMultiByte( CP_ACP, 0, lpszFilename, -1, lpszName, dwBufferSize, NULL, NULL );
				bResult = lpfnPickIconDlg( hWndParent, (LPTSTR)lpszName, &dwBufferSize, lpdwIndex );
				::MultiByteToWideChar( CP_ACP, 0, lpszName, -1, lpszFilename, dwBufferSize );				
				_delete2( lpszName );
			}

			lpfnPickIconDlg = NULL;
		}
		
		_freeLibrary( hShell32 );
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CIconDialog

IMPLEMENT_DYNAMIC( CIconDialog, CDialog )

CIconDialog::CIconDialog( IN LPCTSTR lpszIconFile /*= NULL*/, 
						  IN DWORD dwIconIndex    /*= NULL*/, 
						  IN CWnd* pParentWnd     /*= NULL*/ ) 
		   : CCommonDialog( pParentWnd )
		   , m_hIconHandle( NULL )		   
		   , m_uIconCount( 0 )
		   , m_dwIconIndex( dwIconIndex )
{
	::ZeroMemory( m_szIconFile, MAX_PATH );
	if( ::lstrlen( lpszIconFile ) )
	{
		::lstrcpy( m_szIconFile, lpszIconFile );
		VERIFY( ::DoEnvironmentSubst( m_szIconFile, ::lstrlen( m_szIconFile ) ) );
	}
}

CIconDialog::~CIconDialog( void )
{
	_destroyIcon( m_hIconHandle );
}

BEGIN_MESSAGE_MAP( CIconDialog, CCommonDialog )
END_MESSAGE_MAP()

// CIconDialog message handlers

#if ( _MFC_VER < 0x0700 )
	INT CIconDialog::DoModal( void )
#else
	INT_PTR CIconDialog::DoModal( void )
#endif
{
	ASSERT_VALID( this );

	INT_PTR nRet = -1;
	HWND	hWndParent = PreModal();
	BOOL	bRet = ::SelectIcon( hWndParent, m_szIconFile, MAX_PATH, &m_dwIconIndex );
	
	if( bRet == TRUE )
	{		
		VERIFY( ::DoEnvironmentSubst( m_szIconFile, ::lstrlen( m_szIconFile ) ) );		
		HINSTANCE hInst = AfxGetInstanceHandle();
		
		// "Conversion from 'const int' to 'unsigned int"
		#pragma warning( disable : 4245 )
		m_uIconCount = (UINT_PTR)::ExtractIcon( hInst, m_szIconFile, -1 );
		#pragma warning( default : 4245 )
		
		_destroyIcon( m_hIconHandle );
		m_hIconHandle = ::ExtractIcon( hInst, m_szIconFile, (UINT)m_dwIconIndex );
		nRet = IDOK;
	}
	else
		nRet = IDCANCEL;
	
	PostModal();
	return nRet;
}
/////////////////////////////////////////////////////////////////////////////
