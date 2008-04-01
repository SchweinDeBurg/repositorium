#ifndef _WINDOW_H_
#define _WINDOW_H_
//
// window.h
//
// (C) Copyright 2000-2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../coords/rect.h"
#include "../coords/size.h"
#include "../collectors/linkedlist.h"
#include "../strings/string.h"
#include "../gdi/font.h"
#include "../gdi/rgn.h"
#include "../gdi/gdiobject.h"
#include "../menus/menu.h"
#include "../menus/bitmapmenu.h"

// Just in case...
#ifndef UISF_HIDEFOCUS
#define UISF_HIDEFOCUS                  0x1
#define UISF_HIDEACCEL                  0x2
#define UISF_ACTIVE                     0x4
#endif

// Forward declarations.
class ClsDC;
class ClsMenu;

// This class is a HWND wrapper.
class ClsWindow : public ClsNode
{
	_NO_COPY( ClsWindow );
public:
	// Friends.
	friend class ClsApp;

	// Constructor.
	ClsWindow();

	// Destructor.
	virtual ~ClsWindow();

	// Interface
	HWND Detach();
	BOOL Attach( HWND hWnd, BOOL bDestroy = FALSE );
	BOOL Create( DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu );
	BOOL Create( DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const ClsRect& crBounds, HWND hwndParent, HMENU nIDorHMenu );
	LONG GetWindowLong( int nIndex ) const;
	LONG_PTR GetWindowLong64( int nIndex ) const;
	LONG SetWindowLong( int nIndex, LONG dwNewLong );
	LONG_PTR SetWindowLong64( int nIndex, LONG_PTR dwNewLong );
	DWORD GetStyle() const;
	DWORD GetExStyle() const;
	BOOL IsWindowEnabled() const;
	BOOL EnableWindow( BOOL bEnable = TRUE );
	HICON GetIcon( BOOL bLargeIcon ) const;
	HICON SetIcon( HICON hIcon, BOOL bLargeIcon );
	void ModifyStyle( DWORD dwRemove, DWORD dwAdd, DWORD dwFlags = 0 );
	void ModifyExStyle( DWORD dwRemove, DWORD dwAdd, DWORD dwFlags = 0 );
	BOOL GetWindowPlacement( WINDOWPLACEMENT *pPlacement ) const;
	BOOL SetWindowPlacement( const WINDOWPLACEMENT *pPlacement );
	int GetWindowRgn( HRGN hRgn ) const;
	int SetWindowRgn( HRGN hRgn, BOOL bRedraw );
	BOOL IsIconic() const;
	BOOL IsZoomed() const;
	void MoveWindow( int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE );
	void MoveWindow( const ClsRect& crBounds, BOOL bRepaint = TRUE );
	BOOL SetWindowPos( HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags );
	BOOL SetWindowPos( HWND hWndInsertAfter, LPCRECT lpRect, UINT uFlags );
	UINT ArrangeIconicWindows();
	void BringWindowToTop();
	void GetWindowRect( ClsRect& crRect ) const;
	ClsRect GetWindowRect() const;
	void GetClientRect( ClsRect& crRect ) const;
	ClsRect GetClientRect() const;
	void GetChildRect( ClsRect& crRect, UINT nID ) const;
	ClsRect GetChildRect( UINT nID ) const;
	ClsWindow *ChildWindowFromPoint( POINT pPoint, UINT nFlags = CWP_ALL ) const;
	static ClsWindow *FindWindow( LPCTSTR pszClassName, LPCTSTR pszWindowName );
	ClsWindow *GetWindow( UINT nDirection = GW_HWNDNEXT  ) const;
	ClsWindow *GetTopWindow() const;
	ClsWindow *GetLastActivePopup() const;
	ClsWindow *GetParent() const;
	HWND GetParentHandle() const;
	ClsWindow *SetParent( ClsWindow *pParent );
	static ClsWindow *WindowFromPoint( POINT pPoint );
	ClsWindow *GetDlgItem( int nID ) const;
	HWND GetDlgItemHandle( int nID ) const;
	static ClsWindow *GetCapture();
	ClsWindow *SetCapture();
	static ClsWindow *GetFocus();
	ClsWindow *SetFocus();
	static ClsWindow *GetDesktopWindow();
	static ClsWindow *GetForegroundWindow();
	BOOL SetForegroundWindow();
	int GetDlgCtrlID() const;
	int SetDlgCtrlID( int nNewID );
	BOOL IsChild( const ClsWindow *pWindow ) const;
	BOOL IsChild( HWND hWnd ) const;
	ClsDC *BeginPaint( LPPAINTSTRUCT pPaintStruct );
	void EndPaint( LPPAINTSTRUCT pPaintStruct );
	BOOL LockWindowUpdate();
	void UnlockWindowUpdate();
	ClsDC *GetDC();
	ClsDC *GetDCEx( ClsRgn *prgnClip, DWORD flags );
	BOOL RedrawWindow( LPCRECT lpRectUpdate = NULL, ClsRgn *prgnUpdate = NULL, UINT nFlags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE );
	ClsDC *GetWindowDC();
	int ReleaseDC( ClsDC *pDC );
	void UpdateWindow();
	void SetRedraw( BOOL bRedraw = TRUE );
	BOOL GetUpdateRect( LPRECT pRect, BOOL bErase = FALSE );
	int GetUpdateRgn( ClsRgn* pRgn, BOOL bErase = FALSE );
	void Invalidate( BOOL bErase = TRUE );
	void InvalidateRect( LPCRECT lpRect, BOOL bErase = TRUE );
	void InvalidateRgn( ClsRgn* pRgn, BOOL bErase = TRUE );
	void ValidateRect( LPCRECT lpRect );
	void ValidateRgn( ClsRgn* pRgn );
	BOOL ShowWindow( int nCmdShow );
	BOOL IsWindowVisible( ) const;
	void ShowOwnedPopups( BOOL bShow = TRUE );
	void MapWindowPoints( ClsWindow* pWnd, LPRECT pRect ) const;
	void MapWindowPoints( ClsWindow* pWnd, LPPOINT pPoint, UINT nCount ) const;
	void ClientToScreen( LPPOINT pPoint ) const;
	void ClientToScreen( LPRECT pRect ) const;
	void ScreenToClient( LPPOINT pPoint ) const;
	void ScreenToClient( LPRECT pRect ) const;
	void SetWindowText( LPCTSTR pszText );
	int GetWindowText( LPTSTR pszBuffer, int nMaxCount ) const;
	int GetWindowText( ClsString& strText ) const;
	int GetWindowTextLength() const;
	void GetFont( ClsFont& font ) const;
	void SetFont( ClsFont *pFont, BOOL bRedraw = TRUE );
	int GetScrollPos( int nBar ) const;
	void GetScrollRange( int nBar, LPINT pMinPos, LPINT pMaxPos ) const;
	void ScrollWindow( int xAmount, int yAmount, LPCRECT lpRect = NULL, LPCRECT lpClipRect = NULL );
	BOOL GetScrollInfo( int nBar, LPSCROLLINFO lpScrollInfo, UINT nMask = SIF_ALL ) const;
	BOOL SetScrollInfo( int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE );
	int SetScrollPos( int nBar, int nPos, BOOL bRedraw = TRUE );
	void SetScrollRange( int nBar, int nMinPos, int nMaxPos, BOOL bRedraw = TRUE );
	void ShowScrollBar( UINT nBar, BOOL bShow = TRUE );
	void DragAcceptFiles( BOOL bAccept = TRUE );
	void CheckDlgButton( int nIDButton, UINT nCheck );
	void CheckRadioButton( int nIDFirstButton, int nIDLastButton, int nIDCheckButton );
	int GetCheckedRadioButton( int nIDFirstButton, int nIDLastButton );
	int DlgDirList( LPTSTR lpPathSpec, int nIDListBox, int nIDStaticPath, UINT nFileType );
	int DlgDirListComboBox( LPTSTR lpPathSpec, int nIDComboBox, int nIDStaticPath, UINT nFileType );
	BOOL DlgDirSelect( LPTSTR lpString, int nCount, int nIDListBox );
	BOOL DlgDirSelectComboBox( LPTSTR lpString, int nCount, int nIDComboBox );
	UINT GetDlgItemInt( int nID, BOOL* lpTrans = NULL, BOOL bSigned = TRUE ) const;
	int GetDlgItemText( int nID, LPTSTR lpStr, int nMaxCount ) const;
	int GetDlgItemText( int nID, ClsString& rString ) const;
	ClsWindow *GetNextDlgGroupItem( ClsWindow* pWndCtl, BOOL bPrevious = FALSE ) const;
	ClsWindow *GetNextDlgTabItem( ClsWindow* pWndCtl, BOOL bPrevious = FALSE ) const;
	UINT IsDlgButtonChecked( int nIDButton ) const;
	BOOL IsDialogMessage( LPMSG lpMsg );
	LRESULT SendDlgItemMessage( int nID, UINT message, WPARAM wParam = 0, LPARAM lParam = 0 );
	BOOL SetDlgItemInt( int nID, UINT nValue, BOOL bSigned = TRUE );
	BOOL SetDlgItemText( int nID, LPCTSTR lpszString );
	UINT_PTR SetTimer( UINT_PTR nIDEvent, UINT nElapse, TIMERPROC lpfnTimer = NULL );
	BOOL KillTimer( UINT_PTR nIDEvent );
	BOOL FlashWindow( BOOL bInvert );
	int MessageBox( LPCTSTR lpszText, LPCTSTR lpszCaption = NULL, UINT nType = MB_OK ) const;
	LRESULT SendMessage( UINT message, WPARAM wParam = 0, LPARAM lParam = 0 );
	BOOL PostMessage( UINT message, WPARAM wParam = 0, LPARAM lParam = 0 );
	BOOL SendNotifyMessage( UINT message, WPARAM wParam, LPARAM lParam );
	BOOL ChangeClipboardChain( HWND hWndNext );
	BOOL CenterWindow( ClsWindow *pOn );
	HWND SetClipboardViewer();
	HMENU GetMenu();
	BOOL SetMenu( HMENU hMenu );
	BOOL DrawMenuBar();
	BOOL AnimateWindow( DWORD dwTime, DWORD dwFlags );
	BOOL SetLayeredWindowAttributes( COLORREF crKey, BYTE bAlpha, DWORD dwFlags );
	DWORD GetUIState() const;
	static ClsWindow *GetClipboardOwner();
	static ClsWindow *GetOpenClipboardWindow();
	static ClsWindow *GetClipboardViewer();
	static ClsWindow *FromHandle( HWND hWnd );
	static void DeleteTempObjects();
	HWND GetSafeHWND() const;
	LRESULT HandleMessageTraffic();
	inline BOOL IsDialog() { return m_bIsDialog; }
	int EnumProps( PROPENUMPROC lpEnumFunc );
	int EnumPropsEx( PROPENUMPROCEX lpEnumFunc, LPARAM lParam );
	HANDLE GetProp( LPCTSTR lpString );
	BOOL SetProp( LPCTSTR lpString, HANDLE hData );
	HANDLE RemoveProp( LPCTSTR lpString );
	static ClsMenu *GetActiveMenu();

	// Window procedure for all ClsWindow windows.
	static LRESULT CALLBACK StaticWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Overidables.
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow( LPCREATESTRUCT pCS );
	virtual void Destroy();
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage( LPMSG pMsg );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );
	virtual LRESULT OnReflectedCommand( UINT nNotifyCode, BOOL& bNotifyParent );
	virtual LRESULT OnNotify( LPNMHDR pNMHDR );
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL &bNotifyParent );
	virtual LRESULT OnPaint( ClsDC *pDC );
	virtual LRESULT OnEraseBkgnd( ClsDC *pDC );
	virtual LRESULT OnSize( UINT nSizeType, int nWidth, int nHeight );
	virtual LRESULT OnMove( int xPos, int yPos );
	virtual LRESULT OnDestroy();
	virtual LRESULT OnClose();
	virtual LRESULT OnMeasureItem( UINT nID, LPMEASUREITEMSTRUCT pMis );
	virtual LRESULT OnDrawItem( UINT nID, LPDRAWITEMSTRUCT pDis );
	virtual LRESULT OnReflectedMeasureItem( UINT nID, LPMEASUREITEMSTRUCT pMis, BOOL& bNotifyParent );
	virtual LRESULT OnReflectedDrawItem( UINT nID, LPDRAWITEMSTRUCT pDis, BOOL& bNotifyParent );
	virtual LRESULT OnCreate( LPCREATESTRUCT pCS );
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	// A spedcial overidable used by the layout engine.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize );

	// Operator overloads.
	operator HWND() const;

protected:
	// Helpers.
	static ClsWindow *FindWindow( HWND hWnd );
	static void ClosePopups();
	
	// EnumChildWindows() procedure to pass messages to the children.
	static BOOL CALLBACK DistributeMessage( HWND hWndChild, LPARAM lParam )
	{
		// Simply pass on the message.
		::SendMessage( hWndChild, ( UINT )lParam, 0, 0 );
		return TRUE;
	}

	// Data.
	BOOL				m_bIsMDIFrame;
	BOOL				m_bIsDialog;
	BOOL				m_bDestroyHandle;
	BOOL				m_bIsPopup;
	HWND				m_hWnd;
	WNDPROC				m_lpfnOldWndProc;
	static HMENU			s_hMenu;
	static HWND			s_hMDIClient;
};

#ifndef _WIN64
#pragma warning( push )
#pragma warning( disable : 4244 )
#endif

inline LONG ClsWindow::GetWindowLong( int nIndex ) const		{ _ASSERT_VALID( GetSafeHWND() ); return ::GetWindowLong( m_hWnd, nIndex ); }
inline LONG ClsWindow::SetWindowLong( int nIndex, LONG dwNewLong )	{ _ASSERT_VALID( GetSafeHWND() ); return ::SetWindowLong( m_hWnd, nIndex, dwNewLong ); }
inline LONG_PTR ClsWindow::GetWindowLong64( int nIndex ) const		{ _ASSERT_VALID( GetSafeHWND() ); return ::GetWindowLongPtr( m_hWnd, nIndex ); }
inline LONG_PTR ClsWindow::SetWindowLong64( int nIndex, LONG_PTR dwNewLong ) { _ASSERT_VALID( GetSafeHWND() ); return ::SetWindowLongPtr( m_hWnd, nIndex, dwNewLong ); }
inline DWORD ClsWindow::GetStyle() const				{ _ASSERT_VALID( GetSafeHWND() ); return ::GetWindowLong( m_hWnd, GWL_STYLE ); }
inline DWORD ClsWindow::GetExStyle() const				{ _ASSERT_VALID( GetSafeHWND() ); return ::GetWindowLong( m_hWnd, GWL_EXSTYLE ); }
inline BOOL ClsWindow::IsWindowEnabled() const				{ _ASSERT_VALID( GetSafeHWND() ); return ::IsWindowEnabled( m_hWnd ); }
inline BOOL ClsWindow::EnableWindow( BOOL bEnable /* = TRUE */ )	{ _ASSERT_VALID( GetSafeHWND() ); return ::EnableWindow( m_hWnd, bEnable ); }
inline HICON ClsWindow::GetIcon( BOOL bLargeIcon ) const		{ _ASSERT_VALID( GetSafeHWND() ); return ( HICON )::SendMessage( m_hWnd, WM_GETICON, bLargeIcon ? ICON_BIG : ICON_SMALL, 0 ); }
inline HICON ClsWindow::SetIcon( HICON hIcon, BOOL bLargeIcon )		{ _ASSERT_VALID( GetSafeHWND() ); _ASSERT_VALID( hIcon ); return ( HICON )::SendMessage( m_hWnd, WM_SETICON, bLargeIcon ? ICON_BIG : ICON_SMALL, ( LPARAM )hIcon ); }
inline BOOL ClsWindow::GetWindowPlacement( WINDOWPLACEMENT *pPlacement ) const	{ _ASSERT_VALID( GetSafeHWND() ); return ::GetWindowPlacement( m_hWnd, pPlacement ); }
inline BOOL ClsWindow::SetWindowPlacement( const WINDOWPLACEMENT *pPlacement )	{ _ASSERT_VALID( GetSafeHWND() ); return ::SetWindowPlacement( m_hWnd, pPlacement ); }
inline int ClsWindow::GetWindowRgn( HRGN hRgn ) const			{ _ASSERT_VALID( GetSafeHWND() ); return ::GetWindowRgn( m_hWnd, hRgn ); }
inline int ClsWindow::SetWindowRgn( HRGN hRgn, BOOL bRedraw )		{ _ASSERT_VALID( GetSafeHWND() ); return ::SetWindowRgn( m_hWnd, hRgn, bRedraw ); }
inline BOOL ClsWindow::IsIconic() const					{ _ASSERT_VALID( GetSafeHWND() ); return ::IsIconic( m_hWnd ); }
inline BOOL ClsWindow::IsZoomed() const					{ _ASSERT_VALID( GetSafeHWND() ); return ::IsZoomed( m_hWnd ); }
inline void ClsWindow::MoveWindow( int x, int y, int nWidth, int nHeight, BOOL bRepaint /* = TRUE */ )	{ _ASSERT_VALID( GetSafeHWND() ); ::MoveWindow( m_hWnd, x, y, nWidth, nHeight, bRepaint ); }
inline void ClsWindow::MoveWindow( const ClsRect& crBounds, BOOL bRepaint /* = TRUE */ )		{ _ASSERT_VALID( GetSafeHWND() ); ::MoveWindow( m_hWnd, crBounds.Left(), crBounds.Top(), crBounds.Width(), crBounds.Height(), bRepaint ); }
inline BOOL ClsWindow::SetWindowPos( HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags )	{ _ASSERT_VALID( GetSafeHWND() ); return ::SetWindowPos( m_hWnd, hWndInsertAfter, x, y, cx, cy, uFlags ); }
inline BOOL ClsWindow::SetWindowPos( HWND hWndInsertAfter, LPCRECT lpRect, UINT uFlags )		{ _ASSERT_VALID( GetSafeHWND() ); return ::SetWindowPos( m_hWnd, hWndInsertAfter, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, uFlags ); }
inline UINT ClsWindow::ArrangeIconicWindows()				{ _ASSERT_VALID( GetSafeHWND() ); return ::ArrangeIconicWindows( m_hWnd ); }
inline void ClsWindow::BringWindowToTop()				{ _ASSERT_VALID( GetSafeHWND() ); ::BringWindowToTop( m_hWnd ); }
inline void ClsWindow::GetWindowRect( ClsRect& crRect ) const		{ _ASSERT_VALID( GetSafeHWND() ); ::GetWindowRect( m_hWnd, crRect ); }
inline ClsRect ClsWindow::GetWindowRect() const				{ _ASSERT_VALID( GetSafeHWND() ); ClsRect crRect; ::GetWindowRect( m_hWnd, crRect ); return crRect; }
inline void ClsWindow::GetClientRect( ClsRect& crRect ) const		{ _ASSERT_VALID( GetSafeHWND() ); ::GetClientRect( m_hWnd, crRect ); }
inline ClsRect ClsWindow::GetClientRect() const				{ _ASSERT_VALID( GetSafeHWND() ); ClsRect crRect; ::GetClientRect( m_hWnd, crRect ); return crRect; }
inline ClsWindow *ClsWindow::ChildWindowFromPoint( POINT pPoint, UINT nFlags /* = CWP_ALL */ ) const	{ _ASSERT_VALID( GetSafeHWND() ); return FindWindow( ::ChildWindowFromPointEx( m_hWnd, pPoint, nFlags )); }
inline ClsWindow *ClsWindow::FindWindow( LPCTSTR pszClassName, LPCTSTR pszWindowName )	{ return FindWindow( ::FindWindow( pszClassName, pszWindowName )); }
inline ClsWindow *ClsWindow::GetWindow( UINT nDirection /* = GW_HWNDNEXT */  ) const	{ _ASSERT_VALID( GetSafeHWND() ); return FindWindow( ::GetWindow( m_hWnd, nDirection )); }
inline ClsWindow *ClsWindow::GetTopWindow() const			{ _ASSERT_VALID( GetSafeHWND() ); return FindWindow( ::GetTopWindow( m_hWnd )); }
inline ClsWindow *ClsWindow::GetLastActivePopup() const			{ _ASSERT_VALID( GetSafeHWND() ); return FindWindow( ::GetLastActivePopup( m_hWnd )); }
inline ClsWindow *ClsWindow::GetParent() const				{ _ASSERT_VALID( GetSafeHWND() ); return FindWindow( ::GetParent( m_hWnd )); }
inline HWND ClsWindow::GetParentHandle() const				{ _ASSERT_VALID( GetSafeHWND() ); return ::GetParent( m_hWnd ); }
inline ClsWindow *ClsWindow::SetParent( ClsWindow *pNewParent )		{ _ASSERT_VALID( GetSafeHWND() ); return FindWindow( ::SetParent( m_hWnd, pNewParent ? pNewParent->m_hWnd : NULL )); }
inline ClsWindow *ClsWindow::WindowFromPoint( POINT pPoint )		{ return FindWindow( ::WindowFromPoint( pPoint )); }
inline ClsWindow *ClsWindow::GetDlgItem( int nID ) const		{ _ASSERT_VALID( GetSafeHWND() ); return FindWindow( ::GetDlgItem( m_hWnd, nID )); }
inline HWND ClsWindow::GetDlgItemHandle( int nID ) const		{ _ASSERT_VALID( GetSafeHWND() ); return ::GetDlgItem( m_hWnd, nID ); }
inline ClsWindow *ClsWindow::GetCapture()				{ return FindWindow( ::GetCapture()); }
inline ClsWindow *ClsWindow::SetCapture()				{ _ASSERT_VALID( GetSafeHWND() ); return FindWindow( ::SetCapture( m_hWnd )); }
inline ClsWindow *ClsWindow::GetFocus()					{ return FindWindow( ::GetFocus()); }
inline ClsWindow *ClsWindow::SetFocus()					{ _ASSERT_VALID( GetSafeHWND() ); return FindWindow( ::SetFocus( m_hWnd )); }
inline ClsWindow *ClsWindow::GetDesktopWindow()				{ return FindWindow( ::GetDesktopWindow()); }
inline ClsWindow *ClsWindow::GetForegroundWindow()			{ return FindWindow( ::GetForegroundWindow()); }
inline BOOL ClsWindow::SetForegroundWindow()				{ _ASSERT_VALID( GetSafeHWND() ); return ::SetForegroundWindow( m_hWnd ); }
inline int ClsWindow::GetDlgCtrlID() const				{ _ASSERT_VALID( GetSafeHWND() ); return ::GetDlgCtrlID( m_hWnd ); }
inline int ClsWindow::SetDlgCtrlID( int nNewID )			{ _ASSERT_VALID( GetSafeHWND() ); return ::SetWindowLong( m_hWnd, GWL_ID, ( LONG )nNewID ); }
inline BOOL ClsWindow::IsChild( const ClsWindow *pWindow ) const	{ _ASSERT_VALID( GetSafeHWND() ); return ::IsChild( m_hWnd, pWindow->m_hWnd ); }
inline BOOL ClsWindow::IsChild( HWND hWnd ) const			{ _ASSERT_VALID( GetSafeHWND() ); return ::IsChild( m_hWnd, hWnd ); }
inline void ClsWindow::EndPaint( LPPAINTSTRUCT pPaintStruct )		{ _ASSERT_VALID( GetSafeHWND() ); _ASSERT_VALID( pPaintStruct ); ::EndPaint( m_hWnd, pPaintStruct ); }
inline BOOL ClsWindow::LockWindowUpdate()				{ _ASSERT_VALID( GetSafeHWND() ); return ::LockWindowUpdate( m_hWnd ); }
inline void ClsWindow::UnlockWindowUpdate()				{ _ASSERT_VALID( GetSafeHWND() ); ::LockWindowUpdate( NULL ); }
inline BOOL ClsWindow::RedrawWindow( LPCRECT lpRectUpdate /* = NULL */, ClsRgn *prgnUpdate /* = NULL */, UINT nFlags /* = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE */ )	{ _ASSERT_VALID( GetSafeHWND() ); return ::RedrawWindow( m_hWnd, lpRectUpdate, prgnUpdate ? ( HRGN )*prgnUpdate : NULL, nFlags ); }
inline void ClsWindow::UpdateWindow()					{ _ASSERT_VALID( GetSafeHWND() ); ::UpdateWindow( m_hWnd ); }
inline void ClsWindow::SetRedraw( BOOL bRedraw /* = TRUE */ )		{ _ASSERT_VALID( GetSafeHWND() ); ::SendMessage( m_hWnd, WM_SETREDRAW, bRedraw, 0 ); }
inline BOOL ClsWindow::GetUpdateRect( LPRECT pRect, BOOL bErase /* = FALSE */ )	{ _ASSERT_VALID( GetSafeHWND() ); return ::GetUpdateRect( m_hWnd, pRect, bErase ); }
inline int ClsWindow::GetUpdateRgn( ClsRgn* pRgn, BOOL bErase /* = FALSE */ )	{ _ASSERT_VALID( GetSafeHWND() ); return ::GetUpdateRgn( m_hWnd, *pRgn, bErase ); }
inline void ClsWindow::Invalidate( BOOL bErase /* = TRUE */ )		{ _ASSERT_VALID( GetSafeHWND() ); ::InvalidateRect( m_hWnd, NULL, bErase ); }
inline void ClsWindow::InvalidateRect( LPCRECT lpRect, BOOL bErase /* = TRUE */ )	{ _ASSERT_VALID( GetSafeHWND() ); ::InvalidateRect( m_hWnd, lpRect, bErase ); }
inline void ClsWindow::InvalidateRgn( ClsRgn *pRgn, BOOL bErase /* = TRUE */ )	{ _ASSERT_VALID( GetSafeHWND() ); ::InvalidateRgn( m_hWnd, *pRgn, bErase ); }
inline void ClsWindow::ValidateRect( LPCRECT lpRect )			{ _ASSERT_VALID( GetSafeHWND() ); ::ValidateRect( m_hWnd, lpRect ); }
inline void ClsWindow::ValidateRgn( ClsRgn *pRgn )			{ _ASSERT_VALID( GetSafeHWND() ); ::ValidateRgn( m_hWnd, pRgn ? ( HRGN )*pRgn : NULL ); }
inline BOOL ClsWindow::IsWindowVisible( ) const				{ _ASSERT_VALID( GetSafeHWND() ); return ::IsWindowVisible( m_hWnd ); }
inline void ClsWindow::ShowOwnedPopups( BOOL bShow /* = TRUE */ )	{ _ASSERT_VALID( GetSafeHWND() ); ::ShowOwnedPopups( m_hWnd, bShow ); }
inline void ClsWindow::MapWindowPoints( ClsWindow *pWnd, LPRECT pRect ) const	{ _ASSERT_VALID( GetSafeHWND() ); ::MapWindowPoints( m_hWnd, pWnd->m_hWnd, ( LPPOINT )pRect, 2 ); }
inline void ClsWindow::MapWindowPoints( ClsWindow *pWnd, LPPOINT pPoint, UINT nCount ) const	{ _ASSERT_VALID( GetSafeHWND() ); ::MapWindowPoints( m_hWnd, pWnd->m_hWnd, pPoint, nCount ); }
inline void ClsWindow::ClientToScreen( LPPOINT pPoint ) const		{ _ASSERT_VALID( GetSafeHWND() ); ::ClientToScreen( m_hWnd, pPoint ); }
inline void ClsWindow::ClientToScreen( LPRECT pRect ) const		{ _ASSERT_VALID( GetSafeHWND() ); ::ClientToScreen( m_hWnd, ( LPPOINT )pRect ); ::ClientToScreen( m_hWnd, ( LPPOINT )&pRect->right ); }
inline void ClsWindow::ScreenToClient( LPPOINT pPoint ) const		{ _ASSERT_VALID( GetSafeHWND() ); ::ScreenToClient( m_hWnd, pPoint ); }
inline void ClsWindow::ScreenToClient( LPRECT pRect ) const		{ _ASSERT_VALID( GetSafeHWND() ); ::ScreenToClient( m_hWnd, ( LPPOINT )pRect ); ::ScreenToClient( m_hWnd, ( LPPOINT )&pRect->right ); }
inline int ClsWindow::GetWindowText( LPTSTR pszBuffer, int nMaxCount ) const	{ _ASSERT_VALID( GetSafeHWND() ); return ::GetWindowText( m_hWnd, pszBuffer, nMaxCount ); }
inline int ClsWindow::GetWindowText( ClsString& strText ) const		{ _ASSERT_VALID( GetSafeHWND() ); return strText.GetWindowText( m_hWnd ); }
inline int ClsWindow::GetWindowTextLength() const			{ _ASSERT_VALID( GetSafeHWND() ); return ::GetWindowTextLength( m_hWnd ); }
inline void ClsWindow::GetFont( ClsFont& font ) const			{ _ASSERT_VALID( GetSafeHWND() ); HFONT	hFont = ( HFONT )::SendMessage( m_hWnd, WM_GETFONT, 0, 0 ); if ( hFont ) font.Attach( hFont, FALSE ); }
inline void ClsWindow::SetFont( ClsFont *pFont, BOOL bRedraw /* = TRUE */ )	{ _ASSERT_VALID( GetSafeHWND() ); ::SendMessage( m_hWnd, WM_SETFONT, ( WPARAM )( HFONT )*pFont, MAKELPARAM( bRedraw, 0 )); }
inline int ClsWindow::GetScrollPos( int nBar ) const			{ _ASSERT_VALID( GetSafeHWND() ); return ::GetScrollPos( m_hWnd, nBar ); }
inline void ClsWindow::GetScrollRange( int nBar, LPINT pMinPos, LPINT pMaxPos ) const	{ _ASSERT_VALID( GetSafeHWND() ); ::GetScrollRange( m_hWnd, nBar, pMinPos, pMaxPos ); }
inline void ClsWindow::ScrollWindow( int xAmount, int yAmount, LPCRECT lpRect /* = NULL */, LPCRECT lpClipRect /* = NULL */ )	{ _ASSERT_VALID( GetSafeHWND() ); ::ScrollWindow( m_hWnd, xAmount, yAmount, lpRect, lpClipRect ); }
inline BOOL ClsWindow::GetScrollInfo( int nBar, LPSCROLLINFO lpScrollInfo, UINT nMask /* = SIF_ALL */ ) const	{ _ASSERT_VALID( GetSafeHWND() ); lpScrollInfo->cbSize = sizeof( SCROLLINFO ); 	lpScrollInfo->fMask  = nMask; return ::GetScrollInfo( m_hWnd, nBar, lpScrollInfo ); }
inline BOOL ClsWindow::SetScrollInfo( int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw /* = TRUE */ )	{ _ASSERT_VALID( GetSafeHWND() ); return ::SetScrollInfo( m_hWnd, nBar, lpScrollInfo, bRedraw ); }
inline int ClsWindow::SetScrollPos( int nBar, int nPos, BOOL bRedraw /* = TRUE */ )	{ _ASSERT_VALID( GetSafeHWND() ); return ::SetScrollPos( m_hWnd, nBar, nPos, bRedraw ); }
inline void ClsWindow::SetScrollRange( int nBar, int nMinPos, int nMaxPos, BOOL bRedraw /* = TRUE */ )	{ _ASSERT_VALID( GetSafeHWND() ); ::SetScrollRange( m_hWnd, nBar, nMinPos, nMaxPos, bRedraw ); }
inline void ClsWindow::ShowScrollBar( UINT nBar, BOOL bShow /* = TRUE */ )	{ _ASSERT_VALID( GetSafeHWND() ); ::ShowScrollBar( m_hWnd, nBar, bShow ); }
inline void ClsWindow::DragAcceptFiles( BOOL bAccept /* = TRUE */ )	{ _ASSERT_VALID( GetSafeHWND() ); ::DragAcceptFiles( m_hWnd, bAccept ); }
inline void ClsWindow::CheckDlgButton( int nIDButton, UINT nCheck )	{ _ASSERT_VALID( GetSafeHWND() ); ::CheckDlgButton( m_hWnd, nIDButton, nCheck ); }
inline void ClsWindow::CheckRadioButton( int nIDFirstButton, int nIDLastButton, int nIDCheckButton )	{ _ASSERT_VALID( GetSafeHWND() ); ::CheckRadioButton( m_hWnd, nIDFirstButton, nIDLastButton, nIDCheckButton ); }
inline int ClsWindow::DlgDirList( LPTSTR lpPathSpec, int nIDListBox, int nIDStaticPath, UINT nFileType )	{ _ASSERT_VALID( GetSafeHWND() ); return ::DlgDirList( m_hWnd, lpPathSpec, nIDListBox, nIDStaticPath, nFileType ); }
inline int ClsWindow::DlgDirListComboBox( LPTSTR lpPathSpec, int nIDListBox, int nIDStaticPath, UINT nFileType )	{ _ASSERT_VALID( GetSafeHWND() ); return ::DlgDirListComboBox( m_hWnd, lpPathSpec, nIDListBox, nIDStaticPath, nFileType ); }
inline BOOL ClsWindow::DlgDirSelect( LPTSTR lpString, int nCount, int nIDListBox )	{ _ASSERT_VALID( GetSafeHWND() ); return ::DlgDirSelectEx( m_hWnd, lpString, nCount, nIDListBox ); }
inline BOOL ClsWindow::DlgDirSelectComboBox( LPTSTR lpString, int nCount, int nIDComboBox )	{ _ASSERT_VALID( GetSafeHWND() ); return ::DlgDirSelectComboBoxEx( m_hWnd, lpString, nCount, nIDComboBox ); }
inline UINT ClsWindow::GetDlgItemInt( int nID, BOOL* lpTrans /* = NULL */, BOOL bSigned /* = TRUE */ ) const	{ _ASSERT_VALID( GetSafeHWND() ); return ::GetDlgItemInt( m_hWnd, nID, lpTrans, bSigned ); }
inline ClsWindow* ClsWindow::GetNextDlgGroupItem( ClsWindow* pWndCtl, BOOL bPrevious /* = FALSE */ ) const	{ _ASSERT_VALID( GetSafeHWND() ); return FindWindow( ::GetNextDlgGroupItem( m_hWnd, *pWndCtl, bPrevious )); }
inline ClsWindow* ClsWindow::GetNextDlgTabItem( ClsWindow* pWndCtl, BOOL bPrevious /* = FALSE */ ) const	{ _ASSERT_VALID( GetSafeHWND() ); return FindWindow( ::GetNextDlgTabItem( m_hWnd, *pWndCtl, bPrevious )); }
inline UINT ClsWindow::IsDlgButtonChecked( int nIDButton ) const	{ _ASSERT_VALID( GetSafeHWND() ); return ::IsDlgButtonChecked( m_hWnd, nIDButton ); }
inline BOOL ClsWindow::IsDialogMessage( LPMSG lpMsg )			{ _ASSERT_VALID( GetSafeHWND() ); return ::IsDialogMessage( m_hWnd, lpMsg ); }
inline BOOL ClsWindow::SetDlgItemInt( int nID, UINT nValue, BOOL bSigned /* = TRUE */ )	{ _ASSERT_VALID( GetSafeHWND() ); return ::SetDlgItemInt( m_hWnd, nID, nValue, bSigned ); }
inline LRESULT ClsWindow::SendDlgItemMessage( int nID, UINT message, WPARAM wParam /* = 0 */, LPARAM lParam /* = 0 */ )	{ _ASSERT_VALID( GetSafeHWND() ); return ::SendDlgItemMessage( m_hWnd, nID, message, wParam, lParam ); }
inline BOOL ClsWindow::SetDlgItemText( int nID, LPCTSTR lpszString )	{ _ASSERT_VALID( GetSafeHWND() ); return ::SetDlgItemText( m_hWnd, nID, lpszString ); }
inline UINT_PTR ClsWindow::SetTimer( UINT_PTR nIDEvent, UINT nElapse, TIMERPROC lpfnTimer /* = NULL */ )	{ _ASSERT_VALID( GetSafeHWND() ); return ::SetTimer( m_hWnd, nIDEvent, nElapse, lpfnTimer ); }
inline BOOL ClsWindow::KillTimer( UINT_PTR nIDEvent )			{ _ASSERT_VALID( GetSafeHWND() ); return ::KillTimer( m_hWnd, nIDEvent ); }
inline BOOL ClsWindow::FlashWindow( BOOL bInvert )			{ _ASSERT_VALID( GetSafeHWND() ); return ::FlashWindow( m_hWnd, bInvert ); }
inline int ClsWindow::MessageBox( LPCTSTR lpszText, LPCTSTR lpszCaption /* = NULL */, UINT nType /* = MB_OK */ ) const	{ return ::MessageBox( m_hWnd, lpszText, lpszCaption, nType ); }
inline LRESULT ClsWindow::SendMessage( UINT message, WPARAM wParam /* = 0 */, LPARAM lParam /* = 0 */ )	{ _ASSERT_VALID( GetSafeHWND() ); return ::SendMessage( m_hWnd, message, wParam, lParam ); }
inline BOOL ClsWindow::PostMessage( UINT message, WPARAM wParam /* = 0 */, LPARAM lParam /* = 0 */ )	{ _ASSERT_VALID( GetSafeHWND() ); return ::PostMessage( m_hWnd, message, wParam, lParam ); }
inline BOOL ClsWindow::SendNotifyMessage( UINT message, WPARAM wParam, LPARAM lParam )	{ _ASSERT_VALID( GetSafeHWND() ); return ::SendNotifyMessage( m_hWnd, message, wParam, lParam ); }
inline BOOL ClsWindow::ChangeClipboardChain( HWND hWndNext )		{ _ASSERT_VALID( GetSafeHWND() ); return ::ChangeClipboardChain( m_hWnd, hWndNext ); }
inline HWND ClsWindow::SetClipboardViewer()				{ _ASSERT_VALID( GetSafeHWND() ); return ::SetClipboardViewer( m_hWnd ); }
inline ClsWindow* ClsWindow::GetClipboardOwner()			{ return FindWindow( ::GetClipboardOwner()); }
inline ClsWindow* ClsWindow::GetOpenClipboardWindow()			{ return FindWindow( ::GetOpenClipboardWindow()); }
inline ClsWindow* ClsWindow::GetClipboardViewer()			{ return FindWindow( ::GetClipboardViewer()); }
inline BOOL ClsWindow::ShowWindow( int nCmdShow )			{ _ASSERT_VALID( GetSafeHWND() ); return ::ShowWindow( m_hWnd, nCmdShow ); }
inline ClsWindow *ClsWindow::FromHandle( HWND hWnd )			{ return FindWindow( hWnd ); }
inline HWND ClsWindow::GetSafeHWND() const				{ if ( m_hWnd && ::IsWindow( m_hWnd )) return m_hWnd; return NULL; }
inline HMENU ClsWindow::GetMenu()					{ _ASSERT_VALID( GetSafeHWND() ); return ::GetMenu( m_hWnd ); }
inline BOOL ClsWindow::SetMenu( HMENU hMenu )				{ _ASSERT_VALID( GetSafeHWND() ); return ::SetMenu( m_hWnd, hMenu ); }
inline BOOL ClsWindow::DrawMenuBar()					{ _ASSERT_VALID( GetSafeHWND() ); return ::DrawMenuBar( m_hWnd ); };
inline int ClsWindow::EnumProps( PROPENUMPROC lpEnumFunc )		{ _ASSERT_VALID( GetSafeHWND() ); return ::EnumProps( m_hWnd, lpEnumFunc ); };
inline int ClsWindow::EnumPropsEx( PROPENUMPROCEX lpEnumFunc, LPARAM lParam )	{ _ASSERT_VALID( GetSafeHWND() ); return ::EnumPropsEx( m_hWnd, lpEnumFunc, lParam ); };
inline HANDLE ClsWindow::GetProp( LPCTSTR lpString )			{ _ASSERT_VALID( GetSafeHWND() ); return ::GetProp( m_hWnd, lpString ); };
inline BOOL ClsWindow::SetProp( LPCTSTR lpString, HANDLE hData )	{ _ASSERT_VALID( GetSafeHWND() ); return ::SetProp( m_hWnd, lpString, hData ); };
inline HANDLE ClsWindow::RemoveProp( LPCTSTR lpString )			{ _ASSERT_VALID( GetSafeHWND() ); return ::RemoveProp( m_hWnd, lpString ); };

#ifndef _WIN64
#pragma warning( pop )
#endif

// Some message macros. These will make it easier to define a
// windows procedure and message handler definitions for a 
// ClsWindow derived class.
#define BEGIN_MSG_MAP_HEADER( base ) \
	typedef class base baseClass; \
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) \
	{ \
		BOOL bHandled = TRUE; \
		LRESULT rc;

#define MESSAGE_RANGE_HANDLER( start, end, handler ) \
		if ( uMsg >= start && uMsg <= end ) \
		{ \
			rc = handler( uMsg, wParam, lParam, bHandled ); \
			if ( bHandled ) return rc; \
		}

#define START_MAP() \
		switch ( uMsg ) \
		{

#define BEGIN_MSG_MAP( base ) \
	typedef class base baseClass; \
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) \
	{ \
		BOOL bHandled = TRUE;\
		switch( uMsg ) \
		{

#define MESSAGE_HANDLER( message, handler ) \
			case message: \
			{ \
				LRESULT rc = handler( uMsg, wParam, lParam, bHandled ); \
				if ( bHandled ) return rc; \
				break; \
			}
				
#define END_MSG_MAP() \
		} \
		return baseClass::WindowProc( uMsg, wParam, lParam ); \
	}

#define DEFINE_MSG_HANDLER(handler) virtual LRESULT handler( UINT uMsg, WPARAM lParam, LPARAM wParam, BOOL& bHandled );

#define BEGIN_NOTIFICATION_MAP( base ) \
	typedef class base baseClass; \
	virtual LRESULT OnNotify( LPNMHDR pHdr ) \
	{ \
		BOOL bHandled = TRUE;\
		LRESULT rc; \
		switch( pHdr->code ) \
		{

#define NOTIFICATION_HANDLER( message, handler ) \
			case message: \
			{ \
				rc = handler(( int )pHdr->idFrom, pHdr, bHandled ); \
				if ( bHandled ) return rc; \
				break; \
			}

#define END_NOTIFICATION_MAP() \
		} \
		return baseClass::OnNotify( pHdr ); \
	}

#define DEFINE_NOTIFICATION_HANDLER(handler) virtual LRESULT handler( int nIDCtrl, LPNMHDR pHdr, BOOL& bHandled );

#endif // _WINDOW_H_