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

#ifndef __FOLDERDLG_H__
#define __FOLDERDLG_H__

#if defined( _MSC_VER ) && ( _MSC_VER >= 1020 )
	#pragma once
#endif

/////////////////////////////////////////////////////////////////////////////

#ifndef __AFXDLGS_H__
	#include < AfxDlgs.h >
#endif

#ifndef __ATLCONV_H__
	#include < AtlConv.h >	// MBCS/Unicode Conversion Macros
#endif

// Uncomment if using GetRootFolder
#ifndef _INC_SHLWAPI
	#include < shlwapi.h >
#endif
#pragma comment( lib, "shlwapi.lib" )

/////////////////////////////////////////////////////////////////////////////

#ifndef BFFM_SETOKTEXT					// Version 5.0 or later
	#define BFFM_SETOKTEXT				( WM_USER + 105 )	// Unicode only, req. BIF_USENEWUI
	#define BFFM_SETEXPANDED			( WM_USER + 106 )	// Unicode only, req. BIF_USENEWUI
#endif

#ifndef BIF_NEWDIALOGSTYLE				// Version 5.0 or later
	#define BIF_NEWDIALOGSTYLE			0x0040	
	#define BIF_BROWSEINCLUDEURLS		0x0080
	#define BIF_UAHINT					0x0100				// Req. BIF_NEWDIALOGSTYLE
	#define BIF_NONEWFOLDERBUTTON		0x0200
	#define BIF_NOTRANSLATETARGETS		0x0400	
	#define BIF_SHAREABLE				0x8000				// Req. BIF_USENEWUI
	#define BIF_USENEWUI				( BIF_NEWDIALOGSTYLE | BIF_EDITBOX )	
#endif

/////////////////////////////////////////////////////////////////////////////

#ifndef		_delete2
	#define _delete2( p )			{ if( p ){ delete[] p; p = NULL; } }
#endif

#ifndef		_releaseInterface
	#define	_releaseInterface( p )	{ if( p ){ p->Release(); p = NULL; } }
#endif

#ifndef		_coTaskMemFree
	#define	_coTaskMemFree( p )		{ if( p ){ ::CoTaskMemFree( (LPVOID)p ); p = NULL; } }
#endif

#ifndef _NOINLINE
	#if ( _MSC_VER < 1700 )
		#define _NOINLINE	
	#else
		#define _NOINLINE			__declspec( noinline )	
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////

class CFolderDialog : public CCommonDialog
{
	DECLARE_DYNAMIC( CFolderDialog )

public:
	CFolderDialog(	IN LPCTSTR	pszTitle	= NULL, 
					IN LPCTSTR	pszSelPath	= NULL,
					IN CWnd*	pWndParent	= NULL,
					IN UINT		uFlags		= BIF_RETURNONLYFSDIRS );
	virtual ~CFolderDialog( void );

public:
	#if ( _MFC_VER >= 0x0700 )
		virtual INT_PTR		DoModal( void );
	#else
		virtual INT			DoModal( void );
	#endif

	BOOL	SetRootFolder( IN LPCTSTR pszPath );
	BOOL	GetRootFolder( IN OUT LPTSTR pszPath );

public:
	AFX_INLINE BOOL		SetSelectedFolder( IN LPCTSTR pszPath );
	AFX_INLINE LPCTSTR	GetFolderPath( void )  const;
	AFX_INLINE LPCTSTR	GetFolderName( void )  const;
	AFX_INLINE INT		GetFolderImage( void ) const;		
	AFX_INLINE LPCTSTR	GetSelectedFolder( void ) const;
	AFX_INLINE BROWSEINFO&		 GetBI( void );
	AFX_INLINE const BROWSEINFO& GetBI( void ) const;

protected:	
	BROWSEINFO	m_bi;	
	TCHAR		m_szSelPath[ MAX_PATH ];
	TCHAR		m_szFolPath[ MAX_PATH ];

protected:
	DECLARE_MESSAGE_MAP()
	
	virtual void OnInitialized( void );
	virtual void OnSelChanged( IN LPITEMIDLIST  pItemIDList );
	virtual INT	 OnValidateFailed( IN LPCTSTR /*pszPath*/ );		

protected: // Windows XP or later
	virtual void OnIUnknown( IN IUnknown* /*pIUnknown*/ );
	
protected: // Shell version 5.0 or later:
	_NOINLINE void SetExpanded( IN LPCTSTR pszPath );
	_NOINLINE void SetOKText( IN LPCTSTR pszText );	

protected: // Valid to call only from the above handlers
	AFX_INLINE void EnableOK( IN BOOL bEnable = TRUE );
	AFX_INLINE void SetSelection( IN LPITEMIDLIST pItemIDList );
	AFX_INLINE void SetSelection( IN LPCTSTR pszPath );	
	AFX_INLINE void SetStatusText( IN LPCTSTR pszText );

protected: // Shell version 5.0 or later:
	AFX_INLINE void SetExpanded( IN LPITEMIDLIST pItemIDList );
	
private:
	HWND	m_hWnd; // used only in the callback function
	static INT CALLBACK BrowseCallbackProc(
		IN HWND hWnd, IN UINT uMsg, IN LPARAM lParam, IN LPARAM lpData
	);
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CFolderDialog::SetSelectedFolder( IN LPCTSTR pszPath )
	{ ASSERT( AfxIsValidString( pszPath, MAX_PATH ) );
		return( ::lstrcpy( m_szSelPath, pszPath ) != NULL ); }

AFX_INLINE LPCTSTR CFolderDialog::GetSelectedFolder( void ) const
	{ return m_szSelPath; }

AFX_INLINE BROWSEINFO& CFolderDialog::GetBI( void )
	{ return m_bi; }

AFX_INLINE const BROWSEINFO& CFolderDialog::GetBI( void ) const
	{ return m_bi; }

/////////////////////////////////////////////////////////////////////////////
// Filled after a call to DoModal

AFX_INLINE LPCTSTR CFolderDialog::GetFolderPath( void ) const
	{ return m_szFolPath; }

AFX_INLINE LPCTSTR CFolderDialog::GetFolderName( void ) const
	{ return m_bi.pszDisplayName; }

AFX_INLINE INT CFolderDialog::GetFolderImage( void ) const
	{ return m_bi.iImage; }

/////////////////////////////////////////////////////////////////////////////
// Commands, valid to call only from handlers

AFX_INLINE void CFolderDialog::EnableOK( IN BOOL bEnable /*TRUE*/ )
	{ ASSERT( m_hWnd != NULL ); 
		::SendMessage( m_hWnd, BFFM_ENABLEOK, (WPARAM)bEnable, 0L );}

AFX_INLINE void CFolderDialog::SetSelection( IN LPITEMIDLIST pItemIDList )
	{ ASSERT( m_hWnd != NULL ); 
		::SendMessage( m_hWnd, BFFM_SETSELECTION, (WPARAM)FALSE, (LPARAM)pItemIDList ); }

AFX_INLINE void CFolderDialog::SetSelection( IN LPCTSTR pszPath )
	{ ASSERT( m_hWnd != NULL );
		ASSERT( AfxIsValidString( pszPath, MAX_PATH ) );
			::SendMessage( m_hWnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)pszPath ); }

AFX_INLINE void CFolderDialog::SetStatusText( IN LPCTSTR pszText )
	{ ASSERT( m_hWnd != NULL );
		::SendMessage( m_hWnd, BFFM_SETSTATUSTEXT, (WPARAM)0, (LPARAM)pszText ); }

// Shell version 5.0 or later:

AFX_INLINE void CFolderDialog::SetExpanded( IN LPITEMIDLIST pItemIDList )
	{ ASSERT( m_hWnd != NULL ); 
		::SendMessage( m_hWnd, BFFM_SETEXPANDED, (WPARAM)FALSE, (LPARAM)pItemIDList ); }

/////////////////////////////////////////////////////////////////////////////
#endif // __FOLDERDLG_H__
/////////////////////////////////////////////////////////////////////////////