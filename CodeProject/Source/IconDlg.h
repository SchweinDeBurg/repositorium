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

#ifndef __ICONDLG_H__
#define __ICONDLG_H__
#pragma once

/////////////////////////////////////////////////////////////////////////////

BOOL 
SelectIconW (
	IN HWND			hWndParent, 
	IN OUT LPWSTR	lpszFilename, 
	IN DWORD		dwBufferSize, 
	IN OUT LPDWORD	lpdwIndex
);

BOOL
SelectIconA (
	IN HWND			hWndParent, 
	IN OUT LPSTR	lpszFilename, 
	IN DWORD		dwBufferSize, 
	IN OUT LPDWORD	lpdwIndex
);

#ifdef UNICODE
	#define SelectIcon	SelectIconW
#else
	#define SelectIcon	SelectIconA
#endif

/////////////////////////////////////////////////////////////////////////////

#ifndef		_freeLibrary
	#define _freeLibrary( h )	{ if( h != NULL ){ VERIFY( ::FreeLibrary( (HMODULE)h ) ); h = NULL; } }
#endif

#ifndef		_destroyIcon
	#define _destroyIcon( h )	{ if( h != NULL ){ VERIFY( ::DestroyIcon( (HICON)h ) ); h = NULL; } }
#endif

#ifndef		_delete2
	#define _delete2( p )		{ if( p != NULL ){ delete[] p; p = NULL; } }
#endif

/////////////////////////////////////////////////////////////////////////////

#ifndef __AFXDLGS_H__
	#include < AfxDlgs.h >
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconDialog

class CIconDialog : public CCommonDialog
{
	DECLARE_DYNAMIC( CIconDialog )

public:	
	CIconDialog( IN LPCTSTR lpszIconFile = NULL,
				 IN DWORD	dwIconIndex  = 0,
				 IN CWnd*	pParentWnd	 = NULL );
	virtual ~CIconDialog( void );

public:
	#if ( _MFC_VER < 0x0700 )
		virtual INT		DoModal( void );
	#else
		virtual INT_PTR DoModal( void );
	#endif

	AFX_INLINE HICON	GetIconHandle( void ) const;
	AFX_INLINE UINT_PTR	GetIconCount( void )  const;
	AFX_INLINE DWORD	GetIconIndex( void )  const;
	AFX_INLINE LPCTSTR	GetIconFile( void )   const;	

protected:
	TCHAR		m_szIconFile[ MAX_PATH ];
	DWORD		m_dwIconIndex;
	HICON		m_hIconHandle;
	UINT_PTR	m_uIconCount;	

protected:
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE LPCTSTR CIconDialog::GetIconFile( void ) const
	{ return m_szIconFile; }

AFX_INLINE DWORD CIconDialog::GetIconIndex( void ) const
	{ return m_dwIconIndex; }

AFX_INLINE UINT_PTR CIconDialog::GetIconCount( void ) const
	{ return m_uIconCount; }

AFX_INLINE HICON CIconDialog::GetIconHandle( void ) const
	{ return ::DuplicateIcon( AfxGetInstanceHandle(), m_hIconHandle ); }

/////////////////////////////////////////////////////////////////////////////
#endif // __ICONDLG_H__
/////////////////////////////////////////////////////////////////////////////