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

#ifndef __ICONDLG_H__
#define __ICONDLG_H__
#pragma once

/////////////////////////////////////////////////////////////////////////////

#define _freeLibrary( h )	{ if( h != NULL ){ VERIFY( ::FreeLibrary( h ) ); h = NULL; } }
#define _destroyIcon( h )	{ if( h != NULL ){ VERIFY( ::DestroyIcon( h ) ); h = NULL; } }
#define _delete2( p )		{ if( p != NULL ){ delete[] p; p = NULL; } }

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
	#define SelectIcon SelectIconW
#else
	#define SelectIcon SelectIconA
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
		virtual INT DoModal( void );
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
