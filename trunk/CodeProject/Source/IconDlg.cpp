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

VERSION REQUIREMENTS:
	As far as this class uses undocumented Windows API functions,
	it may possibly fail depending on the shell version ( service 
	packs installed - different or missing ordinal, etc ). However 
	it was tested and ran properly under:

		1. Windows 95 OSR2 - 4.00.950B
		2. Windows 98 - 4.10.1998
		3. Windows Me - 4.90.3000
		4. Windows NT 4.0 Workstation - NT 4.0.1381 with SP 6i
		5. Windows 2000 Professional - NT 5.0.2195 with/out SP1/SP2/SP3 
		6. Windows XP Professional - NT 5.1.2600
	
	It was not tested (though should run properly) under:

		1. Windows 95 - 4.00.950
		2. Windows 95 SP1 - 4.00.950A
		3. Windows 95 OSR 2.5 - 4.00.950C
		4. Windows 98 SE - 4.10.2222A ( Second Edition ) 
		5. Windows NT 4.0 Workstation - NT 4.0.1381 with SP1-SP5
		6. Windows NT 4.0 Server -NT 4.0.1381  with SP1-SP6i
		7. Windows 2000 Server - NT 5.0.2195 with/out SP1-SP3 
		8. Windows XP Professional - NT 5.1.2600 with SP1
		9. Windows XP Server 
	
	If anyone tested it on above-mentioned versions of Windows,
	please mail.

VERSION HISTORY:
	09 Mar 2002 - First release
	04 Nov 2002 - Fixed support for MFC 7.0 ( INT DoModal 2 INT_PTR )
				- Disable warning for ExtractIcon to compile with Level 4
				- Changed ExpandEnvironmentStrings to DoEnvironmentSubst
				- Changed OS detection to GetVersion
	24 May 2003 - Some code changes
				- Changed DoEnvironmentSubst back to ExpandEnvironmentStrings
				- Added more detailed version support
*/
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "IconDlg.h"

#if defined(__INTEL_COMPILER)
// remark #171: invalid type conversion
#pragma warning(disable: 171)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
#endif	// __INTEL_COMPILER

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

CIconDialog::CIconDialog( IN LPCTSTR lpszIconFile   /*= NULL*/, 
						  IN DWORD   dwIconIndex    /*= 0*/, 
						  IN CWnd*   pParentWnd     /*= NULL*/ ) 
		   : CCommonDialog( pParentWnd )
		   , m_hIconHandle( NULL )		   
		   , m_uIconCount( 0 )
		   , m_dwIconIndex( dwIconIndex )
{
	::ZeroMemory( m_szIconFile, MAX_PATH );
	if( ::lstrlen( lpszIconFile ) )		
		::ExpandEnvironmentStrings( lpszIconFile, m_szIconFile, MAX_PATH );
}

CIconDialog::~CIconDialog( void )
{
	_destroyIcon( m_hIconHandle );
}

BEGIN_MESSAGE_MAP( CIconDialog, CCommonDialog )
END_MESSAGE_MAP()

// CIconDialog message handlers

#if ( _MFC_VER < 0x0700 )
	INT		CIconDialog::DoModal( void )
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
		TCHAR szTmp[ MAX_PATH ] = { 0 };
		::lstrcpy( szTmp, m_szIconFile );
		::ExpandEnvironmentStrings( szTmp, m_szIconFile, MAX_PATH );
		
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
