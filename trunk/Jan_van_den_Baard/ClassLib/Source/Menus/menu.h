#ifndef _MENU_H_
#define _MENU_H_
//
// menu.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "menuiteminfo.h"
#include "../collectors/linkedlist.h"
#include "../coords/rect.h"

// Forward declarations.
class ClsWindow;

// A wrapper class for the HMENU handle.
class ClsMenu : public ClsNode
{
	friend class ClsWindow;
	
	// No copy constructor.
	_NO_COPY( ClsMenu );
public:
	// Constructor.
	ClsMenu( HMENU hMenu = NULL );

	// Virtual destructor.
	virtual ~ClsMenu();

	// Interface.
	BOOL Load( LPCTSTR pszName );
	BOOL Create( BOOL bPopup = FALSE );
	virtual BOOL Destroy();
	void Attach( HMENU hMenu );
	HMENU Detach();
	static ClsMenu *FromHandle( HMENU hMenu );
	static void DeleteTempObjects();
	virtual BOOL DeleteMenu( UINT uPosition, BOOL bByPos = FALSE );
	BOOL TrackPopupMenu( UINT nFlags, int x, int y, HWND hWnd, LPCRECT lpRect = NULL ) const;
	BOOL EnableItem( UINT uPosition, UINT uFlags, BOOL bByPos = FALSE );
	UINT GetDefaultItem( UINT uFlags, BOOL bGetPos = FALSE ) const;
	BOOL SetDefaultItem( UINT uPosition, BOOL bByPos = FALSE );
	int GetItemCount() const;
	UINT GetItemID( int nPos ) const;
	BOOL GetItemInfo( UINT uItem, LPMENUITEMINFO lpItemInfo, BOOL bByPos = FALSE ) const;
	BOOL SetItemInfo( UINT uItem, LPMENUITEMINFO lpItemInfo, BOOL bByPos = FALSE );
	ClsMenu *GetSubMenu( int nPos );
	BOOL InsertItem( UINT uItem, LPMENUITEMINFO lpItemInfo, BOOL bByPos = FALSE );
	BOOL CheckItem( UINT nIDCheckItem, UINT uCheck, BOOL bByPos = FALSE );
	BOOL CheckRadioItem( UINT uIDFirst, UINT uIDLast, UINT uIDCheck, BOOL bByPos = FALSE );
	UINT GetItemState( UINT uID, BOOL bByPos = FALSE ) const;
	DWORD GetMenuContextHelpId() const;
	BOOL SetMenuContextHelpId( DWORD dwID );
	inline BOOL IsDropped() const { return m_bIsDropped; }
	inline void SetParentRect( LPCRECT pRect ) { m_LastParent = *pRect; m_bSetByHand = TRUE; };

	// Custom menu frame hook. See below.
	static void InstallMessageHook();
	static void RemoveMessageHook();

	// Operator overloads.
	inline operator HMENU() { return m_hMenu; };

protected:
	// Overidables.
	virtual LRESULT OnReflectedMeasureItem( LPMEASUREITEMSTRUCT pMis, BOOL& bNotifyParent ) { return FALSE; }
	virtual LRESULT OnReflectedDrawItem( LPDRAWITEMSTRUCT pDis, BOOL& bNotifyParent ) { return FALSE; }

	// Helpers.
	static BOOL IsSystemMenu( HMENU hMenu );

	// ---- Custom frame rendering. This functionalitty is copied from the
	// CNewMenu classes by:
	// 
	// Author  : Bruno Podetti
	// Email   : Podetti@gmx.net
	// 
	// The original article was found on www.codeproject.com. 

	// The following overidables are called to enable custom
	// frames on menus.
	virtual BOOL DoFrameRendering() { return FALSE; }
	virtual void OnMeasureFrame( LPRECT pRect ) {;}
	virtual void PreDrawFrame( LPCRECT pRect, LPCRECT pRectScr ) {;}
	virtual void OnDrawFrame( HDC hDC, LPCRECT pRect, LPCRECT pRectScr ) {;}
	virtual void OnChangeWindowPos( LPWINDOWPOS pWindowPos ) {;}

	// Helpers for the custom menu frame rendering. There is a Windows hook procedure which will
	// subclass the menus and a Menu window procedure which takes care of dispatching the messages
	// to the overidables.
	static LRESULT CALLBACK MessageHookProc( int nCode, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK MenuWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	static HHOOK s_hNextHook;
	static ATOM  s_hPropAtom;

	// These are called when the menu owner window receives a WM_INITMENU or WM_INITMENUPOPUP.
	virtual void OnReflectedInitMenu( ClsWindow *pWindow ) {;}
	virtual void OnReflectedInitMenuPopup( ClsWindow *pWindow, HMENU hPopup, int nIndex, BOOL bWindowMenu );
	virtual void OnReflectedUnInitMenuPopup( ClsWindow *pWindow, HMENU hPopup, LPARAM lParam );
	virtual void OnReflectedExitMenuLoop( ClsWindow *pWindow, BOOL bShortcut );

	// Data.
	HMENU		m_hMenu;
	BOOL		m_bIsDropped;
	ClsRect		m_LastParent;
	BOOL		m_bSetByHand;
};

#endif // _MENU_H_