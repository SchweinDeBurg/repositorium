#ifndef _APPLICATION_H_
#define _APPLICATION_H_
//
// application.h
//
// (C) Copyright 2000-2001 Jan van den Baard.
//     All Rights Reserved.
//

#include "standard.h"
#include "strings/string.h"
#include "collectors/array.h"

// Application class.
class ClsApp
{
	// No copy constructor.
	_NO_COPY( ClsApp );
public:
	// Image indexes in the GetClsImages() image list.
	enum 
	{ 
		CII_DROPDOWN = 0, 
		CII_DROPDOWN_SMALL, 
		CII_CHECKMARK, 
		CII_CLOSE, 
		CII_UP, 
		CII_DOWN, 
		CII_LEFT, 
		CII_RIGHT, 
		CII_PIN, 
		CII_PINNED,
		CII_NUMICONS
	};

	// Cursors.
	enum {
		STC_HAND = 0,
		STC_HSPLIT,
		STC_VSPLIT,
		STC_NUMCURSORS
	};

	// Constructor.
	ClsApp();

	// Virtual destructor.
	~ClsApp();

	// Interface.
	BOOL Setup( HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow, LPCTSTR pszIconName = NULL, LPCTSTR pszAppTitle = NULL, WNDCLASSEX *pWndClass = NULL, int nClassCount = 0, DWORD dwCommonControls = 0 );
	BOOL AreWeRunning( LPCTSTR pszAtomName );
	BOOL LoadAcceleratorTable( HWND hWnd, LPCTSTR pszAccelName );
	BOOL TranslateAccelerator( LPMSG pMsg );
	void AddCommonControls( DWORD dwCtrlType );
	HINSTANCE GetResourceHandle();
	BOOL IsShadowEnabled();
	BOOL IsMenuFadeEnabled();
	BOOL InitRichedit();

	// Inlined
	inline LPSTR GetCmdLine() const { return m_lpCmdLine; }
	inline int GetCmdShow() const { return m_nCmdShow; }
	inline HINSTANCE GetInstanceHandle() const { return m_hInstance; }
	inline DWORD GetMajorVersion() const { return m_osVersion.dwMajorVersion; }
	inline DWORD GetMinorVersion() const { return m_osVersion.dwMinorVersion; }
	inline DWORD GetBuildNumber() const { return m_osVersion.dwBuildNumber; }
	inline DWORD GetPlatformID() const { return m_osVersion.dwPlatformId; }
	inline ClsString& GetAppTitle() const { return ( ClsString& )m_sAppTitle; }
	inline HICON GetIcon( BOOL bSmall = FALSE ) const { return bSmall ? m_hIconSmall : m_hIcon; }
	inline HCURSOR GetCursor( int nCursor ) const { _ASSERT( nCursor >= STC_HAND && nCursor < STC_NUMCURSORS ); return m_hCursors[ nCursor ]; }
	inline HIMAGELIST GetSystemImageList() const { return m_hSystemImages; }
	inline HBRUSH GetPatternBrush() const { return m_hPatternBrush; }
	inline void SetResourceHandle( HINSTANCE hInstance ) { m_hResources = hInstance; }
	inline UINT GetComCtlVersion() const { return m_nVer; }
	inline UINT GetComCtlRevision() const { return m_nRev; }
	inline HICON GetCIIIcon( int nIcon ) const { _ASSERT( nIcon >= CII_DROPDOWN && nIcon < CII_NUMICONS ); return m_hIcons[ nIcon ]; }
	inline HIMAGELIST GetClsImages() const { return m_hImages; }

	// For compatibility only. Do not use in new code.
	inline HICON GetDropdownIcon( BOOL bSmall = FALSE ) const { return bSmall ? m_hIcons[ CII_DROPDOWN_SMALL ] : m_hIcons[ CII_DROPDOWN ]; }
	inline HICON GetCheckmarkIcon() const { return m_hIcons[ CII_CHECKMARK ]; }

protected:
	// Accelerator->Window connection.
	struct AccelWnd
	{
		HWND	m_hWnd;
		HACCEL	m_hAccel;
	};

	// Helper.
	void ComCtlVersion();

	// Data.
	UINT			m_nVer, m_nRev;
	HINSTANCE		m_hInstance;
	HINSTANCE		m_hResources;
	HIMAGELIST		m_hSystemImages;
	HCURSOR			m_hCursors[ STC_NUMCURSORS ];
	OSVERSIONINFO		m_osVersion;
	ATOM			m_hAtom;
	ClsString		m_sAppTitle;
	ClsArray<AccelWnd>	m_Accelerators;
	HICON			m_hIcon, m_hIconSmall;
	BOOL			m_bPrivClass;
	LPSTR			m_lpCmdLine;
	int			m_nCmdShow;
	ClsArray<ClsString>	m_aClasses;
	HBITMAP			m_hPattern;
	HBRUSH			m_hPatternBrush;
	HICON			m_hIcons[ CII_NUMICONS ];
	HIMAGELIST		m_hImages;
	HMODULE			m_hRichedit;
};

#endif // _APPLICATION_H_