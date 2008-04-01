#ifndef _MDIWINDOW_H_
#define _MDIWINDOW_H_
//
// mdiwindow.h
//
// (C) Copyright 2000-2005 Jan van den Baard.
//     All Rights Reserved.
//

#include "window.h"
#include "../menus/menu.h"
#include "../collectors/array.h"

// Forward declarations.
class ClsMDIMainWindow;

// A "ClsWindow" derived class which the functionality of a multiple
// document interface (MDI) child window.
class ClsMDIChildWindow : public ClsWindow
{
	_NO_COPY( ClsMDIChildWindow );
public:
	// Friends.
	friend class ClsApp;

	// Construction/destruction.
	ClsMDIChildWindow();
	virtual ~ClsMDIChildWindow();

	// Implementation.
	BOOL Create( LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, ClsMDIMainWindow *pFrame );
	BOOL Create( LPCTSTR lpszWindowName, DWORD dwStyle, const ClsRect& crBounds, ClsMDIMainWindow *pFrame );
	void MDIDestroy();
	void MDIActivate();
	void MDIMaximize();
	void MDIRestore();

protected:
	// Overidables.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnMDIActivated( ClsWindow *pDeactivated );
	virtual LRESULT OnMDIDeactivated( ClsWindow *pActivated );
	virtual LRESULT OnMDINCCreate( LPCREATESTRUCT pCS );

	// Default MDI child message handling procedure.
	static LRESULT CALLBACK StaticMDIChildProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Data.
	ClsMDIMainWindow *m_pMainWindow;
};

// A "ClsWindow" derived class with functionality for a multiple
// document interface (MDI) frame window.
class ClsMDIMainWindow : public ClsWindow
{
	_NO_COPY( ClsMDIMainWindow );
public:
	// Friends.
	friend class ClsMDIChildWindow;

	// Construction destruction.
	ClsMDIMainWindow();
	virtual ~ClsMDIMainWindow();

	// Implementation.
	void MDIActivate( ClsWindow *pWnd );
	ClsMDIChildWindow *MDIGetActive( LPBOOL pbMaximized = NULL ) const;
	void MDIIconArrange();
	void MDIMaximize( ClsWindow *pWnd );
	void MDINext();
	void MDIPred();
	void MDIRestore( ClsWindow *pWnd );
	ClsMenu *MDISetMenu( ClsMenu *pFrameMenu, ClsMenu *pWindowMenu );
	void MDITile( UINT nFlags = MDITILE_HORIZONTAL | MDITILE_SKIPDISABLED );
	void MDICascade( UINT nFlags = MDITILE_SKIPDISABLED );
	void SetBaseID( UINT nBaseID = 0x0BAD );
	ClsWindow *GetMDIClient();
	UINT MDINumChilds() const;
	
protected:
	// Overidable.
	virtual LRESULT OnCreate( LPCREATESTRUCT pCS );
	virtual LRESULT OnDestroy();
	virtual void OnMDIChildRemoved( ClsMDIChildWindow *pWnd );

	// Helpers.
	BOOL AddChild( ClsMDIChildWindow *pChild );
	BOOL RemChild( ClsMDIChildWindow *pChild );
	
	// Used to track children.
	struct MDIChild
	{
		ClsMDIChildWindow *m_pChild;
	};

	// Data.
	ClsWindow			m_Client;
	ClsSimpleArray<MDIChild>	m_Children;
	UINT				m_nBaseID;
};

// Inlined functions (MDI Child).
inline void ClsMDIChildWindow::MDIActivate()
{ _ASSERT_VALID( GetSafeHWND()); _ASSERT_VALID( m_pMainWindow ); m_pMainWindow->GetMDIClient()->SendMessage( WM_MDIACTIVATE, ( WPARAM )GetSafeHWND()); }
inline void ClsMDIChildWindow::MDIMaximize()
{ _ASSERT_VALID( GetSafeHWND()); _ASSERT_VALID( m_pMainWindow ); m_pMainWindow->GetMDIClient()->SendMessage( WM_MDIMAXIMIZE, ( WPARAM )GetSafeHWND()); }
inline void ClsMDIChildWindow::MDIRestore()
{ _ASSERT_VALID( GetSafeHWND()); _ASSERT_VALID( m_pMainWindow ); m_pMainWindow->GetMDIClient()->SendMessage( WM_MDIRESTORE, ( WPARAM )GetSafeHWND()); }
inline void ClsMDIChildWindow::MDIDestroy()
{ _ASSERT_VALID( GetSafeHWND()); _ASSERT_VALID( m_pMainWindow ); m_pMainWindow->GetMDIClient()->SendMessage( WM_MDIDESTROY, ( WPARAM )GetSafeHWND()); }

// Inlined functions (MDI Main).
// Activate an MDI child window.
inline void ClsMDIMainWindow::MDIActivate( ClsWindow *pWnd )
{ _ASSERT_VALID( GetSafeHWND()); _ASSERT_VALID( pWnd );	m_Client.SendMessage( WM_MDIACTIVATE, ( WPARAM )pWnd->GetSafeHWND()); }
inline void ClsMDIMainWindow::MDIIconArrange()
{ _ASSERT_VALID( GetSafeHWND()); m_Client.SendMessage( WM_MDIICONARRANGE ); }
inline void ClsMDIMainWindow::MDIMaximize( ClsWindow *pWnd )
{ _ASSERT_VALID( GetSafeHWND()); _ASSERT_VALID( pWnd ); m_Client.SendMessage( WM_MDIMAXIMIZE, ( WPARAM )( pWnd->GetSafeHWND())); }
inline void ClsMDIMainWindow::MDINext()
{ _ASSERT_VALID( GetSafeHWND()); m_Client.SendDlgItemMessage( WM_MDINEXT, NULL, FALSE ); }
inline void ClsMDIMainWindow::MDIPred()
{ _ASSERT_VALID( GetSafeHWND()); m_Client.SendDlgItemMessage( WM_MDINEXT, NULL, TRUE ); }
inline void ClsMDIMainWindow::MDIRestore( ClsWindow *pWnd )
{ _ASSERT_VALID( GetSafeHWND()); _ASSERT_VALID( pWnd ); m_Client.SendMessage( WM_MDIRESTORE, ( WPARAM )( pWnd->GetSafeHWND())); }
inline void ClsMDIMainWindow::MDITile( UINT nFlags /* = MDITILE_HORIZONTAL | MDITILE_SKIPDISABLED */ )
{ _ASSERT_VALID( GetSafeHWND()); m_Client.SendMessage( WM_MDITILE, ( WPARAM )nFlags ); }
inline void ClsMDIMainWindow::MDICascade( UINT nFlags /* = MDITILE_SKIPDISABLED */ )
{ _ASSERT_VALID( GetSafeHWND()); m_Client.SendMessage( WM_MDICASCADE, ( WPARAM )nFlags ); }
inline void ClsMDIMainWindow::SetBaseID( UINT nBaseID /* = 0x0BAD */ )
{ m_nBaseID = nBaseID; }
inline ClsWindow *ClsMDIMainWindow::GetMDIClient()
{ return &m_Client; }
inline UINT ClsMDIMainWindow::MDINumChilds() const
{ return m_Children.GetSize(); }

#endif // _MDIWINDOW_H_