//
// window.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "window.h"
#include "mdiwindow.h"
#include "../application.h"
#include "../gdi/gdiobject.h"
#include "../gdi/dc.h"
#include "../menus/menu.h"
#include "../menus/bitmapmenu.h"
#include "../tools/multimonitor.h"
#include "../tools/xpcolors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Just in case...
#ifndef WM_QUERYUISTATE
#define WM_CHANGEUISTATE		0x0127
#define WM_UPDATEUISTATE                0x0128
#define WM_QUERYUISTATE                 0x0129
#endif

// Lists of all window objects created.
ClsLinkedList<ClsWindow>	temporary_window_list;
ClsLinkedList<ClsWindow>	global_window_list;

typedef BOOL ( CALLBACK *ANIMATEWINDOW )( HWND, DWORD, DWORD );
typedef BOOL ( CALLBACK *SETLAYEREDWINDOWATTRIBUTES )( HWND, COLORREF, BYTE, DWORD );

// Statics.
HMENU ClsWindow::s_hMenu = NULL;
HWND ClsWindow::s_hMDIClient = NULL;

static ANIMATEWINDOW StaticAnimateWindow = NULL;
static SETLAYEREDWINDOWATTRIBUTES StaticSetLayeredWindowAttributes = NULL;

// Defined in "bitmapmenu.cpp".
extern ClsLinkedList<ClsMenu> global_menu_list;

// Finds a window object in the list by it's handle
// value.
ClsWindow *ClsFindObjectByHandle( ClsLinkedList<ClsWindow>& list, HWND hWnd )
{
	_ASSERT_VALID( hWnd ); // This must be valid.
	_ASSERT( IsWindow( hWnd )); // Only window handles.

	// Iterate the nodes.
	for ( ClsWindow *pWindow = list.GetFirst(); pWindow; pWindow = list.GetNext( pWindow ))
	{
		// Is the handle wrapped by this object
		// the one we are looking for?
		if ( pWindow->GetSafeHWND() == hWnd )
			// Yes. Return a pointer to the object.
			return pWindow;
	}
	// Object not in the list.
	return NULL;
}

// Constructor. Setup data field(s).
ClsWindow::ClsWindow()
{
	// Setup window type.
	m_bIsDialog		= FALSE;
	m_bIsMDIFrame		= FALSE;
	m_bIsPopup		= FALSE;

	// Clear data.
	m_hWnd			= NULL;
	m_lpfnOldWndProc	= NULL;
	m_bDestroyHandle	= TRUE;
	s_hMenu			= NULL;

	// Add this object to the global list.
	global_window_list.AddHead( this );
}

// Destructor. Destroys the window
// unless it is detached.
ClsWindow::~ClsWindow()
{
	// Destroy the window.
	Destroy();

	// Remove us from the global list if we where
	// still linked into it.
	ClsWindow *pWindow;
	for ( pWindow = global_window_list.GetFirst(); pWindow; pWindow = global_window_list.GetNext( pWindow ))
	{
		// Is this us?
		if ( pWindow == this )
		{
			// Yes. remove us from the list.
			global_window_list.Remove( this );
			return;
		}
	}
}

// Destroys the wrapped window if
// possible.
void ClsWindow::Destroy()
{
	// Do we wrap a valid handle?
	if ( GetSafeHWND())
	{
		// Destroy the window.
		if ( m_bDestroyHandle )
			// Destroy the handle.
			::DestroyWindow( m_hWnd );
		else
			// Detach the handle to reset
			// the original window procedure.
			Detach();
	}
}

// Detaches the window from the
// object.
HWND ClsWindow::Detach()
{
	// Handle valid?
	if ( GetSafeHWND())
	{
		// Did we subclass the window? If not we can't
		// detach it. If we did the window would be left
		// without a window procedure.
		if ( m_lpfnOldWndProc && m_lpfnOldWndProc != ( WNDPROC )ClsWindow::StaticWindowProc )
		{
			// Set back the original window procedure.
			::SetWindowLongPtr( m_hWnd, m_bIsDialog ? DWL_DLGPROC : GWL_WNDPROC, ( LONG_PTR )m_lpfnOldWndProc );

			// Get return code.
			HWND hWnd = m_hWnd;

			// Clear fields.
			m_bIsDialog	 = FALSE;
			m_hWnd		 = NULL;
			m_lpfnOldWndProc = NULL;

			// Return result.
			return hWnd;
		}
	}
	return NULL;
}

// Attach a window handle to this object. This will only
// work if this object does not already have a handle
// attached to it.
BOOL ClsWindow::Attach( HWND hWnd, BOOL bDestroy /* = FALSE */ )
{
	_ASSERT_VALID( IsWindow( hWnd ) ); // Handle must be valid.
	_ASSERT( GetSafeHWND() == NULL ); // Already has a handle.

	// Is the handle already attached to
	// another object? If so it may not
	// be set to be destroyed by this object.
	if ( bDestroy )
	{
		if ( ClsFindObjectByHandle( global_window_list, hWnd ) ||
		     ClsFindObjectByHandle( temporary_window_list, hWnd ))
		{
			_ASSERT( bDestroy == FALSE );
			return FALSE;
		}
	}

	// First we see if the handle is a normal window
	// or a dialog.
	TCHAR	szClassName[ 10 ];
	if ( ::GetClassName( hWnd, szClassName, 10 ))
	{
		// Is it a dialog?
		m_bIsDialog = ( BOOL )( _tcscmp( _T( "#32770" ), szClassName ) == 0 );

		// Save the handle.
		m_hWnd		 = hWnd;
		m_bDestroyHandle = bDestroy;

		// Get the window procedure.
		m_lpfnOldWndProc = ( WNDPROC )::GetWindowLongPtr( hWnd, m_bIsDialog ? DWL_DLGPROC : GWL_WNDPROC );
		
		// Is the window proc already our static
		// window procedure?
		if ( m_lpfnOldWndProc && m_lpfnOldWndProc != ( WNDPROC )ClsWindow::StaticWindowProc )
		{
			// Call the PreSubclassWindow() override.
			PreSubclassWindow();

			// Subclass the original window
			// procedure.
			::SetWindowLongPtr( hWnd, m_bIsDialog ? DWL_DLGPROC : GWL_WNDPROC, ( LONG_PTR )ClsWindow::StaticWindowProc );
		}
		// Return success.
		return TRUE;
	}

	// Failed.
	m_lpfnOldWndProc = NULL;
	m_hWnd		 = NULL;

	return FALSE;
}

// Create the window.
BOOL ClsWindow::Create( DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu )
{
	_ASSERT( m_hWnd == NULL ); // Must be zero!

	// Do we already have a handle?
	if ( GetSafeHWND() )
		return FALSE;

	// Setup the CREATESTRUCT.
	CREATESTRUCT	cs;
	cs.dwExStyle	  = dwExStyle;
	cs.lpszClass	  = lpszClassName;
	cs.lpszName	  = lpszWindowName;
	cs.style	  = dwStyle;
	cs.x		  = x;
	cs.y		  = y;
	cs.cx		  = nWidth;
	cs.cy		  = nHeight;
	cs.hwndParent	  = hwndParent;
	cs.hMenu	  = nIDorHMenu;
	cs.lpCreateParams = 0L;
	
	// Call the PreCreateWindow() function.
	if ( PreCreateWindow( &cs ))
	{
		_ASSERT_VALID( cs.lpszClass ); // This must be known by now!

		// Create the window.
		m_hWnd = ::CreateWindowEx( cs.dwExStyle,
					      cs.lpszClass,
					      cs.lpszName,
					      cs.style,
					      cs.x,
					      cs.y,
					      cs.cx,
					      cs.cy,
					      cs.hwndParent,
					      cs.hMenu,
					      ClsGetInstanceHandle(),
					      ( LPVOID )this );

		// Subclass the window.
		if ( m_hWnd )
		{
			// We destroy the handle.
			m_bDestroyHandle = TRUE;

			// Get the window procedure.
			m_lpfnOldWndProc = ( WNDPROC )::GetWindowLongPtr( m_hWnd, m_bIsDialog ? DWL_DLGPROC : GWL_WNDPROC );
		
			// Is the window proc already our static
			// window procedure?
			if ( m_lpfnOldWndProc && m_lpfnOldWndProc != ( WNDPROC )ClsWindow::StaticWindowProc )
			{
				// Call the PreSubclassWindow() override.
				PreSubclassWindow();

				// Subclass the original window
				// procedure.
				if ( ::SetWindowLongPtr( m_hWnd, m_bIsDialog ? DWL_DLGPROC : GWL_WNDPROC, ( LONG_PTR )ClsWindow::StaticWindowProc ))
					// Return success.
					return TRUE;
			}
			else
				// Return success.
				return TRUE;
		}

		// return success or failure.
		return ( BOOL )( m_hWnd );
	}
	return FALSE;
}

// Create the window.
BOOL ClsWindow::Create( DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const ClsRect& crBounds, HWND hwndParent, HMENU nIDorHMenu )
{
	// Create the window.
	return Create( dwExStyle, lpszClassName, lpszWindowName, dwStyle, crBounds.Left(), crBounds.Top(), crBounds.Width(), crBounds.Height(), hwndParent, nIDorHMenu );
}

// Modify the window style.
void ClsWindow::ModifyStyle( DWORD dwRemove, DWORD dwAdd, DWORD dwFlags /* = 0 */ )
{
	_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

	// First we get the current window style.
	DWORD dwStyle = ( DWORD )::GetWindowLongPtr( m_hWnd, GWL_STYLE );

	// Change bits.
	dwStyle &= ~dwRemove;
	dwStyle |= dwAdd;

	// Change the style.
	::SetWindowLongPtr( m_hWnd, GWL_STYLE, ( LONG_PTR )( dwStyle | dwFlags ));
}

// Modify the extended window style.
void ClsWindow::ModifyExStyle( DWORD dwRemove, DWORD dwAdd, DWORD dwFlags /* = 0 */ )
{
	_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

	// First we get the current window extended style.
	DWORD dwExStyle = ( DWORD )::GetWindowLongPtr( m_hWnd, GWL_EXSTYLE );

	// Change bits.
	dwExStyle &= ~dwRemove;
	dwExStyle |= dwAdd;

	// Change the extended style.
	::SetWindowLongPtr( m_hWnd, GWL_EXSTYLE, ( LONG_PTR )( dwExStyle | dwFlags ));
}

// Get the rectangle of a child window.
ClsRect ClsWindow::GetChildRect( UINT nID ) const
{
	_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

	ClsRect crRect;
	GetChildRect( crRect, nID );
	return crRect;
}

// Get the rectangle of a child window.
void ClsWindow::GetChildRect( ClsRect& crRect, UINT nID ) const
{
	_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

	// Get the child window.
	HWND hWndChild = ::GetDlgItem( m_hWnd, nID );

	// OK?
	if ( hWndChild )
	{
		// Get it's rectangle.
		::GetWindowRect( hWndChild, crRect );

		// Map to the window.
		::MapWindowPoints( NULL, m_hWnd, ( LPPOINT )( LPRECT )crRect, 2 );
	}
}


// Set the window text.
void ClsWindow::SetWindowText( LPCTSTR pszText )
{
	_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

	// If the hi-order word of "pszText" is 0
	// we load the string from the resources.
	if ( pszText && HIWORD( pszText ) == 0 )
	{
		// Load the string.
		ClsString str( pszText );
		::SetWindowText( m_hWnd, str );
		return;
	}

	// Set the text.
	::SetWindowText( m_hWnd, pszText );
}

// Get the checked radion button.
int ClsWindow::GetCheckedRadioButton( int nIDFirstButton, int nIDLastButton )
{
	_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

	// Get button checked states.
	for ( int i = nIDFirstButton; i <= nIDLastButton; i++ )
	{
		// Is it checked?
		if ( ::IsDlgButtonChecked( m_hWnd, i ))
			return i;
	}
	return 0;
}

int ClsWindow::GetDlgItemText( int nID, LPTSTR lpStr, int nMaxCount ) const	
{	_ASSERT_VALID( GetSafeHWND() ); 

	ClsWindow *pWindow = GetDlgItem( nID ); 
	if ( pWindow ) 
		return pWindow->GetWindowText( lpStr, nMaxCount ); 
	return 0; 
}

int ClsWindow::GetDlgItemText( int nID, ClsString& rString ) const
{
	_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

	ClsWindow *pWindow = GetDlgItem( nID );
	if ( pWindow )
		return pWindow->GetWindowText( rString );
	return 0;
}

void ClsWindow::DeleteTempObjects()					
{ 
	ClsWindow *pWindow;

	// Remove all objects and delete them.
	while (( pWindow = temporary_window_list.RemoveHead()) != NULL )
		delete pWindow;
}

// Called just before the window is subclassed.
void ClsWindow::PreSubclassWindow()
{
}

// Called just before the window is created.
BOOL ClsWindow::PreCreateWindow( LPCREATESTRUCT pCS )
{
	// Use default class if none is given.
	if ( pCS->lpszClass == NULL )
		pCS->lpszClass = _T( "ClsWindowClass" );

	// Continue creating the window.
	return TRUE;
}

// Handle message traffic.
LRESULT ClsWindow::HandleMessageTraffic()
{
	BOOL bTranslate = FALSE;
	MSG  msg;

	// Enter the message loop.
	while ( GetMessage( &msg, NULL, 0, 0 ) > 0 )
	{
		// Accelerator?
		if ( msg.hwnd == NULL || ( ClsGetApp()->TranslateAccelerator( &msg )))
			continue;

		// Is there an MDI client? If so try to translate the
		// accelerators.
		if ( ClsWindow::s_hMDIClient && TranslateMDISysAccel( ClsWindow::s_hMDIClient, &msg ))
			continue;

		// Get the message window object. The message window may not
		// be located in the global window list since it might be a
		// child of the window located in the global window list.
		//
		// For example the messages from a edit control in a combobox
		// control. In this case, when the message window is not found,
		// we look up it's parent in the global window list.
		ClsWindow *pWindow = ClsFindObjectByHandle( global_window_list, msg.hwnd );
		if ( pWindow == NULL && IsChild( msg.hwnd )) pWindow = ClsFindObjectByHandle( global_window_list, ::GetParent( msg.hwnd ));

		// Call the PreTranslateMessage() function to pre-process
		// the message.
		if ( pWindow ) 
		{ 
			bTranslate = pWindow->PreTranslateMessage( &msg );
		}
		else
		{
			// A dialog message for the active window?
			bTranslate = ::IsDialogMessage( GetActiveWindow(), &msg );
			if ( ! bTranslate )
			{
				// Is it a child window? If so we iterate the
				// parent windows until we find one that 
				// translates the message or has no parent.
				if ( IsChild( msg.hwnd ))
				{
					HWND hParent = ::GetParent( msg.hwnd );
					ClsWindow *pTemp;

					// Does the parent exist?
					if ( hParent )
					{
						// Wrap the handle.
						pTemp = ClsWindow::FromHandle( hParent );

						// Do a translation.
						if ( ! pTemp->PreTranslateMessage( &msg ))
						{
							// The message did not translate. Iterate until we
							// find a parent which does.
							while (( hParent = ::GetParent( hParent )) != NULL )
							{
								// Wrap the handle.
								pTemp = ClsWindow::FromHandle( hParent );

								// Does this one translate?
								if ( pTemp->PreTranslateMessage( &msg ) == TRUE )
								{
									// Yes. Break up the loop.
									bTranslate = TRUE;
									break;
								}
							}
						}
						else
							// Message was translated.
							bTranslate = TRUE;
					}
					else
					{
						// Try it as a dialog message for the active window.
						bTranslate = ::IsDialogMessage( ::GetActiveWindow(), &msg );
					}
				}
				else
				{
					// Try it as a dialog message for the active window.
					bTranslate = ::IsDialogMessage( ::GetActiveWindow(), &msg );
				}
			}
		}

		// Can we dispatch?
		if ( ! bTranslate )
		{
			// Message was not handled. Dispatch it.
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}

		// Delete temporary GDI, Window and (Bitmap)Menu objects.
		ClsGdiObject::DeleteTempObjects();
		ClsMenu::DeleteTempObjects();
		ClsBitmapMenu::DeleteTempObjects();
		ClsWindow::DeleteTempObjects();
	}
	// Return the result.
	return ( LRESULT )msg.wParam;
}

// Called just after the WM_NCDESTROY message
// was handled.
void ClsWindow::PostNcDestroy()
{
	// Reset variables.
	m_bIsDialog	 = FALSE;
	m_hWnd		 = NULL;
	m_lpfnOldWndProc = NULL;
}

// Returns FALSE by default.
BOOL ClsWindow::PreTranslateMessage( LPMSG pMsg )
{
	// Are we a dialog? If so see if the message can be
	// processed by IsDialogMessage.
	if ( m_bIsDialog && IsDialogMessage( pMsg ))
		return TRUE;
	
	// Are we a child window? If so we see if our parent
	// is a dialog and let it have a go at the message
	// first.
	ClsWindow *pParent = GetParent();
	if ( pParent /*&& pParent->m_bIsDialog*/ && pParent->IsDialogMessage( pMsg ))
		return TRUE;

	// Message not processed.
	return FALSE;
}

// WM_COMMAND message handler.
LRESULT ClsWindow::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// Not handled.
	return -1;
}

// Reflected WM_COMMAND message handler.
LRESULT ClsWindow::OnReflectedCommand( UINT nNotifyCode, BOOL& bNotifyParent )
{
	// Not handled.
	return -1;
}

// WM_NOTIFY message handler.
LRESULT ClsWindow::OnNotify( LPNMHDR pNMHDR )
{
	// Not handled.
	return -1;
}

// Reflected WM_NOTIFY message handler.
LRESULT ClsWindow::OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
{
	// Not handled.
	return -1;
}

// WM_PAINT message handler.
LRESULT ClsWindow::OnPaint( ClsDC *pDC )
{
	// Not handled.
	return -1;
}

// WM_ERASEBKGND message handler.
LRESULT ClsWindow::OnEraseBkgnd( ClsDC *pDC )
{
	// Not handled.
	return -1;
}

// WM_SIZE message handler.
LRESULT ClsWindow::OnSize( UINT nSizeType, int nWidth, int nHeight )
{
	// Not handled.
	return -1;
}

// WM_MOVE message handler.
LRESULT ClsWindow::OnMove( int xPos, int yPos )
{
	// Not handled.
	return -1;
}

// WM_DESTROY message handler.
LRESULT ClsWindow::OnDestroy()
{
	// Not handled.
	return -1;
}

// WM_CLOSE message handler.
LRESULT ClsWindow::OnClose()
{
	// Not handled.
	return -1;
}

// WM_MEASUREITEM message handler.
LRESULT ClsWindow::OnMeasureItem( UINT nID, LPMEASUREITEMSTRUCT pMis )
{
	// Not handled.
	return -1;
}

// WM_DRAWITEM message handler.
LRESULT ClsWindow::OnDrawItem( UINT nID, LPDRAWITEMSTRUCT pDis )
{
	// Not handled.
	return -1;
}

// Reflected WM_MEASUREITEM message handler.
LRESULT ClsWindow::OnReflectedMeasureItem( UINT nID, LPMEASUREITEMSTRUCT pMis, BOOL& bNotifyParent )
{
	// Not handled.
	return -1;
}

// Reflected WM_DRAWITEM message handler.
LRESULT ClsWindow::OnReflectedDrawItem( UINT nID, LPDRAWITEMSTRUCT pDis, BOOL& bNotifyParent )
{
	// Not handled.
	return -1;
}

// WM_CREATE message handler.
LRESULT ClsWindow::OnCreate( LPCREATESTRUCT pCS )
{
	// Not handled. Note this is a special case. -1
	// means failure with WM_CREATE messages.
	return -2;
}

// Window procedure.
LRESULT ClsWindow::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult = 0;

	// Do we have an original window procedure to call?
	if ( m_lpfnOldWndProc && m_lpfnOldWndProc != ( WNDPROC )ClsWindow::StaticWindowProc )
		// Call the original window procedure.
		lResult = ::CallWindowProc( m_lpfnOldWndProc, GetSafeHWND(), uMsg, wParam, lParam );
	else if ( m_bIsDialog == FALSE )
	{
		// Are we an MDI frame?
		if ( m_bIsMDIFrame )
			// Call the default procedure for MDI frames. 
			//
			// The casting of the this pointer I do here is dangerous. If someone 
			// decides to create a "ClsWindow" derived class and set the "m_bIsMDIFrame"
			// we are screwed. For now it works but I should consider another approach...
			lResult = ::DefFrameProc( GetSafeHWND(), reinterpret_cast< ClsMDIMainWindow * >( this )->GetMDIClient()->GetSafeHWND(), uMsg, wParam, lParam );
		else
			// Call the default window procedure.
			lResult = ::DefWindowProc( GetSafeHWND(), uMsg, wParam, lParam );
	}

	// Return the result.
	return lResult;
}

// By default we use the current window size...
BOOL ClsWindow::OnGetMinSize( ClsSize& szMinSize )
{
	ClsRect rc;
	GetWindowRect( rc );
	szMinSize = rc.Size();
	return TRUE;
}

// Operator overload.
ClsWindow::operator HWND() const
{
	return m_hWnd;
}

LRESULT CALLBACK ClsWindow::StaticWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// We will need this.
	ClsWindow *pWindow = NULL;
	BOOL	   bFromTemp = FALSE;

	// Do we need to attach the handle to it's
	// object?
	if ( uMsg == WM_NCCREATE )
	{
		// The object pointer of this window must be
		// passed using the lParam field of the 
		// CREATESTRUCT structure.
		pWindow = ( ClsWindow * )(( LPCREATESTRUCT )lParam )->lpCreateParams;

		// Should be valid.
		//_ASSERT_VALID( pWindow );

		// Attach us to the object.
		if ( pWindow ) pWindow->Attach( hWnd, TRUE );
	}
	else if ( uMsg == WM_INITDIALOG )
	{
		_ASSERT_VALID( lParam ); // Must be a valid pointer.

		// First we check to see if the lParam parameter is
		// an object from our global window list.
		for ( pWindow = global_window_list.GetFirst(); pWindow != ( ClsWindow * )lParam && pWindow; pWindow = global_window_list.GetNext( pWindow ));

		// If the object was not found in the list it must be
		// a PROPSHEETPAGE pointer. In this case the object should
		// be in the lParam field of this structure.
		if ( pWindow == NULL )
			pWindow = ( ClsWindow * )(( PROPSHEETPAGE * )lParam )->lParam;

		// Should be valid.
		_ASSERT_VALID( pWindow );

		// Attach us to the object.
		pWindow->Attach( hWnd, TRUE );
	}
	else
	{
		// When we reach this place the handle must be attached
		// already.
		pWindow = ClsFindObjectByHandle( global_window_list, hWnd );
		if ( pWindow == NULL )
		{
			// We were not located in the globals list
			// so we must be in the temporary list.
			bFromTemp = TRUE;
			pWindow = ClsFindObjectByHandle( temporary_window_list, hWnd );
		}
	}

	// Do we have a valid object pointer?
	if ( pWindow != NULL && pWindow->GetSafeHWND() )
	{
		// Preset message result.
		LRESULT	lResult = -1;

		// Get message type.
		switch ( uMsg )
		{
			case	WM_UPDATEUISTATE:
			case	WM_SYSCOLORCHANGE:
				// Update XP colorschemes.
				XPColors.CreateColorTable();

				// Pass on to the children.
				EnumChildWindows( pWindow->GetSafeHWND(), DistributeMessage, WM_SYSCOLORCHANGE );

				// Fall through when the message was WM_UPDATEUISTATE...
				if ( uMsg == WM_SYSCOLORCHANGE )
					break;

			case	WM_CHANGEUISTATE:
				// Are we a child window?
				if ( pWindow->GetStyle() & WS_CHILD )
					// Repaint...
					pWindow->Invalidate();
				break;

			case	WM_CREATE:
				// Call virtual message handler.
				lResult = pWindow->OnCreate(( LPCREATESTRUCT )lParam );
				break;

			case	WM_CLOSE:
				// Call virtual message handler.
				lResult = pWindow->OnClose();
				break;

			case	WM_DESTROY:
			{
				// Does this window have a menu?
				HMENU hMenu = pWindow->GetMenu();
				if ( hMenu )
				{
					// See if it is wrapped by an object in the global
					// menu list.
					for ( ClsMenu *pMenu = global_menu_list.GetFirst(); pMenu; pMenu = global_menu_list.GetNext( pMenu ))
					{
						// Is this it?
						if ( *pMenu == hMenu )
						{
							// We detach the menu from the window before the window
							// destroys the menu handle. This is necessary because the
							// destructor of "ClsBitmapMenu" derived classes need the
							// handle valid to free used resources.
							pWindow->SetMenu( NULL );
							break;
						}
					}
				}
				// Call virtual message handler.
				lResult = pWindow->OnDestroy();
				break;
			}

			case	WM_MOVE:
				// Call virtual message handler.
				lResult = pWindow->OnMove(( int )LOWORD( lParam ), ( int )HIWORD( lParam ));
				break;

			case	WM_SIZE:
				// Call virtual message handler.
				lResult = pWindow->OnSize(( UINT )wParam, ( int )LOWORD( lParam ), ( int )HIWORD( lParam ));
				break;

			case	WM_PAINT:
			{
				// Do we have a DC?
				ClsDC *pDC = wParam ? ClsDC::FromHandle(( HDC )wParam ) : NULL;
				
				// Call virtual message handler.
				lResult = pWindow->OnPaint( pDC );
				break;
			}

			case	WM_ERASEBKGND:
			{
				// Wrap handle.
				ClsDC *pDC = ClsDC::FromHandle(( HDC )wParam );

				// Call virtual message handler.
				lResult = pWindow->OnEraseBkgnd( pDC );
				break;
			}

			case	WM_INITMENU:
				// Store the menu handle which just opened.
				ClsWindow::s_hMenu = ( HMENU )wParam;

				// OK?
				if ( ClsWindow::s_hMenu && ::IsMenu( ClsWindow::s_hMenu ))
				{
					// Get object and call OnReflectedInitMenu() overidable.
					ClsMenu *pMenu = ClsMenu::FromHandle( ClsWindow::s_hMenu );
					if ( pMenu ) pMenu->OnReflectedInitMenu( pWindow );
				}
				break;

			case	WM_INITMENUPOPUP:
				// Menu handle OK?
				if ( ClsWindow::s_hMenu && ::IsMenu( ClsWindow::s_hMenu ))
				{
					// Get object and call OnReflectedInitMenuPopup() overidable.
					ClsMenu *pMenu = ClsMenu::FromHandle( ClsWindow::s_hMenu );
					if ( pMenu ) pMenu->OnReflectedInitMenuPopup( pWindow, ( HMENU )wParam, LOWORD( lParam ), HIWORD( lParam ));
				}
				break;

			case	WM_UNINITMENUPOPUP:
				// Menu handle OK?
				if ( ClsWindow::s_hMenu && ::IsMenu( ClsWindow::s_hMenu ))
				{
					// Get object and call OnReflectedUnInitMenuPopup() overidable.
					ClsMenu *pMenu = ClsMenu::FromHandle( ClsWindow::s_hMenu );
					if ( pMenu ) pMenu->OnReflectedUnInitMenuPopup( pWindow, ( HMENU )wParam, lParam );
				}
				break;

			case	WM_EXITMENULOOP:
				// Menu handle OK?
				if ( ClsWindow::s_hMenu && ::IsMenu( ClsWindow::s_hMenu ))
				{
					// Get object and call OnReflectedInitMenuPopup() overidable.
					ClsMenu *pMenu = ClsMenu::FromHandle( ClsWindow::s_hMenu );
					if ( pMenu ) pMenu->OnReflectedExitMenuLoop( pWindow, ( BOOL )wParam );
				}
				// Clear the menu handle.
				ClsWindow::s_hMenu = NULL;
				break;
				
			case	WM_MEASUREITEM:
				// Control?
				if ( wParam )
				{
					// Try to get the window object of the control.
					ClsWindow *pChild = pWindow->FromHandle( pWindow->GetDlgItemHandle((( LPMEASUREITEMSTRUCT )lParam )->CtlID ));

					// Found it?
					if ( pChild )
					{
						// By default we do notify the parent.
						BOOL bNotifyParent = TRUE;

						// Reflect the measure item message to the child
						// window.
						lResult = pChild->OnReflectedMeasureItem(( UINT )wParam, ( LPMEASUREITEMSTRUCT )lParam, bNotifyParent );

						// Are we a dialog?
						if ( lResult != -1 && pWindow->IsDialog())
						{
							// Set message result.
							pWindow->SetWindowLong64( DWL_MSGRESULT, ( LONG_PTR )lResult );
							lResult = TRUE;
						}

						// Should we notify the parent?
						if ( ! bNotifyParent )
							break;
					}
				}
				else
				{
					// Find the menu.
					if ( ! ClsWindow::s_hMenu ) ClsWindow::s_hMenu = pWindow->GetMenu();
					if ( ClsWindow::s_hMenu && ::IsMenu( ClsWindow::s_hMenu ))
					{
						// By default we do notify the parent.
						BOOL bNotifyParent = TRUE;

						// Wrap the handle.
						ClsMenu *pMenu = ClsMenu::FromHandle( ClsWindow::s_hMenu );
						if ( pMenu )
						{
							// Call the overidable.
							lResult = pMenu->OnReflectedMeasureItem(( LPMEASUREITEMSTRUCT ) lParam, bNotifyParent );

							// Are we a dialog?
							if ( lResult != -1 && pWindow->IsDialog())
							{
								// Set message result.
								pWindow->SetWindowLong64( DWL_MSGRESULT, ( LONG_PTR )lResult );
								lResult = TRUE;
							}

							// Should we notify the parent?
							if ( ! bNotifyParent )
								break;
						}
					}
				}
				// Call virtual message handler.
				lResult = pWindow->OnMeasureItem(( UINT )wParam, ( LPMEASUREITEMSTRUCT )lParam );
				break;

			case	WM_DRAWITEM:
				// Control?
				if ( wParam )
				{
					// Try to get the window object of the control.
					ClsWindow *pChild = pWindow->FromHandle( pWindow->GetDlgItemHandle((( LPDRAWITEMSTRUCT )lParam )->CtlID ));

					// Found it?
					if ( pChild )
					{
						// By default we do notify the parent.
						BOOL bNotifyParent = TRUE;

						// Reflect the draw item message to the child
						// window.
						lResult = pChild->OnReflectedDrawItem(( UINT )wParam, ( LPDRAWITEMSTRUCT )lParam, bNotifyParent );

						// Are we a dialog?
						if ( lResult != -1 && pWindow->IsDialog())
						{
							// Set message result.
							pWindow->SetWindowLong64( DWL_MSGRESULT, ( LONG_PTR )lResult );
							lResult = TRUE;
						}

						// Should we notify the parent?
						if ( ! bNotifyParent )
							break;
					}
				}
				else
				{
					// Find the menu.
					if ( ! ClsWindow::s_hMenu ) ClsWindow::s_hMenu = pWindow->GetMenu();
					if ( ClsWindow::s_hMenu && ::IsMenu( ClsWindow::s_hMenu ))
					{
						// By default we do notify the parent.
						BOOL bNotifyParent = TRUE;

						// Wrap the handle.
						ClsMenu *pMenu = ClsMenu::FromHandle( ClsWindow::s_hMenu );
						if ( pMenu )
						{
							// Call the overidable.
							lResult = pMenu->OnReflectedDrawItem(( LPDRAWITEMSTRUCT )lParam, bNotifyParent );

							// Are we a dialog?
							if ( lResult != -1 && pWindow->IsDialog())
							{
								// Set message result.
								pWindow->SetWindowLong64( DWL_MSGRESULT, ( LONG_PTR )lResult );
								lResult = TRUE;
							}

							// Should we notify the parent?
							if ( ! bNotifyParent )
								break;
						}
					}
				}
				// Call virtual message handler.
				lResult = pWindow->OnDrawItem(( UINT )wParam, ( LPDRAWITEMSTRUCT )lParam );
				break;

			case	WM_COMMAND:
			{
				// Message originates from a control?
				if ( lParam )
				{
					// Try to get the window object of the control which
					// sent the message.
					ClsWindow *pChild = ClsFindObjectByHandle( global_window_list, ( HWND )lParam );

					// Found it?
					if ( pChild )
					{
						// By default we do notify the parent.
						BOOL bNotifyParent = TRUE;

						// Reflect the command message to the child
						// window.
						lResult = pChild->OnReflectedCommand(( UINT )HIWORD( wParam ), bNotifyParent );

						// Are we a dialog?
						if ( lResult != -1 && pWindow->IsDialog())
						{
							// Set message result.
							pWindow->SetWindowLong64( DWL_MSGRESULT, ( LONG_PTR )lResult );
							lResult = TRUE;
						}

						// Should we notify the parent?
						if ( ! bNotifyParent )
							break;
					}
				}
				// Call virtual message handler.
				lResult = pWindow->OnCommand(( UINT )HIWORD( wParam ), ( UINT )LOWORD( wParam ), ( HWND )lParam );
				break;
			}

			case	WM_NOTIFY:
				LPNMHDR pNMHDR = ( LPNMHDR )lParam;

				// Try to get the window object of the
				// notification control.
				ClsWindow *pChild = ClsFindObjectByHandle( global_window_list, pNMHDR->hwndFrom );

				// Where we able to find the object in our
				// global list?
				if ( pChild )
				{
					// By default we do notify the parent.
					BOOL bNotifyParent = TRUE;

					// Reflect the notification message to the
					// child window.
					lResult = pChild->OnReflectedNotify( pNMHDR, bNotifyParent );

					// Are we a dialog?
					if ( lResult != -1 && pWindow->IsDialog())
					{
						// Set message result.
						pWindow->SetWindowLong64( DWL_MSGRESULT, ( LONG_PTR )lResult );
						lResult = TRUE;
					}

					// Must we notify the parent?
					if ( ! bNotifyParent )
						break;
				}
				// Simply forward the message.
				pWindow->OnNotify( pNMHDR );
				break;
		}

		// Are we still alive?
		//if ( ! bFromTemp )
		//	for ( ClsWindow *pTmp = global_window_list.GetFirst(); pTmp != pWindow && pTmp; pTmp = global_window_list.GetNext( pTmp ));
		//else
		//	for ( ClsWindow *pTmp = temporary_window_list.GetFirst(); pTmp != pWindow && pTmp; pTmp = temporary_window_list.GetNext( pTmp ));
		//
		// We may not have been deleted by any of the virtual
		// message handlers!
		//_ASSERT( pTmp != NULL );

		// If the message was not handled (lResult is -1) and the
		// object window handle is still valid we call the virtual
		// window procedure.
		//
		// Note: The WM_CREATE message is a special case which returns
		// -2 when the message was not handled since -1 indicates that
		// the window should not be created when -1 is returned.
		if ((( lResult == -1 && uMsg != WM_CREATE ) || ( lResult == -2 && uMsg == WM_CREATE )) && pWindow->GetSafeHWND())
			// Call the procedure.
			lResult = pWindow->WindowProc( uMsg, wParam, lParam );

		// Are we being destroyed?
		if ( uMsg == WM_NCDESTROY )
		{
			// detach the handle from the object.
			if ( pWindow->GetSafeHWND())
			{
				// Detach the handle.
				pWindow->Detach();

				// Just in case these are not cleared which will
				// happen when the window has the ClsWindow::StaticWindowProc
				// as the default window procedure.
				pWindow->m_hWnd = NULL;
				pWindow->m_lpfnOldWndProc = NULL;
			}

			// Call the PostNcDestroy() routine if the class pointer
			// still exists in the global window list.
			ClsWindow *pTmp;
			for ( pTmp = global_window_list.GetFirst(); pTmp; pTmp = global_window_list.GetNext( pTmp ))
			{
				// Is this it?
				if ( pTmp == pWindow )
				{
					// Call it.
					pWindow->PostNcDestroy();
					return lResult;
				}
			}
		}
		// return the result of the message handling.
		return lResult;
	}

	// We did not find the handle in our global
	// window list which means that it was not (yet)
	// attached to a ClsWindow object.
	//
	// In this case we simply let windows handle
	// the messages.
	TCHAR	szClassName[ 10 ];

	// Get the class name of the window.
	if ( ::GetClassName( hWnd, szClassName, 10 ))
	{
		// Is it a dialog box? If so we return 0 and do not
		// call the default window procedure.
		if ( _tcscmp( _T( "#32770" ), szClassName ) == 0 )
			return 0;
	}

	// Return the result of the default window
	// procedure.
	return ::DefWindowProc( hWnd, uMsg, wParam, lParam );
}

// Helper which will try to locate the window object of the
// given handle. If it does not find it it will create an
// object for it and append it to the temporary object list.
ClsWindow *ClsWindow::FindWindow( HWND hWnd )
{
	// Return NULL if the input is NULL.
	if ( hWnd == NULL ) return NULL;

	// First we try to locate the handle in the
	// global window list.
	ClsWindow *pWindow = ClsFindObjectByHandle( global_window_list, hWnd );

	// Found it?
	if ( pWindow == NULL )
	{
		// No. Try to locate it in our temporary object list.
		pWindow = ClsFindObjectByHandle( temporary_window_list, hWnd );
		if ( pWindow == NULL )
		{
			// Not found. Create a new object.
			pWindow = new ClsWindow;
			pWindow->Attach( hWnd );

			// Remove it from the global list and
			// move it into the temporary object list.
			global_window_list.Remove( pWindow );
			temporary_window_list.AddHead( pWindow );
		}
	}
	// Return the object.
	return pWindow;
}

// Close all windows marked as being a popup.
void ClsWindow::ClosePopups()
{
	// Iterate the global window list.
	ClsWindow *pWindow;
	for ( pWindow = global_window_list.GetFirst(); pWindow; pWindow = global_window_list.GetNext( pWindow ))
	{
		// A popup?
		if ( pWindow->m_bIsPopup )
		{
			// Get a pointer to the next in the list. Close the popup
			// and set the pointer to the next.
			ClsWindow *pNext = global_window_list.GetNext( pWindow );
			pWindow->SendMessage( WM_CLOSE );
			pWindow = pNext;
		}
	}

	// Iterate the temporary window list.
	for ( pWindow = temporary_window_list.GetFirst(); pWindow; pWindow = temporary_window_list.GetNext( pWindow ))
	{
		// A popup?
		if ( pWindow->m_bIsPopup )
		{
			// Get a pointer to the next in the list. Close the popup
			// and set the pointer to the next.
			ClsWindow *pNext = temporary_window_list.GetNext( pWindow );
			pWindow->SendMessage( WM_CLOSE );
			pWindow = pNext;
		}
	}
}

// Center the window on another window or
// on the desktop.
BOOL ClsWindow::CenterWindow( ClsWindow *pOn )
{
	_ASSERT_VALID( GetSafeHWND() );

	// Obtain the child window
	// screen bounds.
	ClsRect rc, prc, screen;
	GetWindowRect( rc );

	// Get the rectangle of the display monitor which
	// the window intersects the most.
	ClsMultiMon mon;
	int nMonitor;
	mon.MonitorNumberFromWindow( GetSafeHWND(), MONITOR_DEFAULTTONEAREST, nMonitor );
	mon.GetMonitorRect( nMonitor, prc, TRUE );
	screen = prc;

	// Do we have a valid parent
	// handle?
	if ( pOn )
		// Obtain the parent window
		// screen bounds.
                pOn->GetWindowRect( prc );

	// Compute offsets...
	int x = prc.Left() + (( prc.Width() / 2 ) - ( rc.Width()  / 2 ));
	int y = prc.Top() + (( prc.Height() / 2 ) - ( rc.Height() / 2 ));

	// Make sure the whole window remains visible on the screen.
	if (( x + rc.Width())  > screen.Right())  x = screen.Right() - rc.Width();
	if ( x < screen.Left())		          x = screen.Left();
	if (( y + rc.Height()) > screen.Bottom()) y = screen.Bottom() - rc.Height();
	if ( y < screen.Top())		          y = screen.Top();

	// Move the window so that it is
	// centered.
        return SetWindowPos( NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
}

// AnimateWindow() API.
BOOL ClsWindow::AnimateWindow( DWORD dwTime, DWORD dwFlags )
{
	_ASSERT_VALID( GetSafeHWND()); // Must be valid.

	// Just in case...
	#ifndef AW_HIDE
	#define AW_HIDE 0x00010000
	#endif

	// Function known?
	if ( StaticAnimateWindow )
		return ( *StaticAnimateWindow )( m_hWnd, dwTime, dwFlags );
	
	// Get the procedure address.
	StaticAnimateWindow = ( ANIMATEWINDOW )GetProcAddress( GetModuleHandle( _T( "user32.dll" )), "AnimateWindow" );
	if ( StaticAnimateWindow )
		return ( *StaticAnimateWindow )( m_hWnd, dwTime, dwFlags );
	return FALSE;
}

// SetLayeredWindowAttributes() API.
BOOL ClsWindow::SetLayeredWindowAttributes( COLORREF crKey, BYTE bAlpha, DWORD dwFlags )
{
	_ASSERT_VALID( GetSafeHWND()); // Must be valid.

	// Function known?
	if ( StaticSetLayeredWindowAttributes )
		return ( *StaticSetLayeredWindowAttributes )( m_hWnd, crKey, bAlpha, dwFlags );
	
	// Get the procedure address.
	StaticSetLayeredWindowAttributes = ( SETLAYEREDWINDOWATTRIBUTES )GetProcAddress( GetModuleHandle( _T( "user32.dll" )), "SetLayeredWindowAttributes" );
	if ( StaticSetLayeredWindowAttributes )
		return ( *StaticSetLayeredWindowAttributes )( m_hWnd, crKey, bAlpha, dwFlags );
	return FALSE;
}

// Get a pointer to the active menu object.
ClsMenu *ClsWindow::GetActiveMenu()
{
	// Is there an active menu?
	if ( ClsWindow::s_hMenu && ::IsMenu( ClsWindow::s_hMenu ))
	{
		// Look it up.
		ClsMenu *pMenu;
		for ( pMenu = global_menu_list.GetFirst(); pMenu; pMenu = global_menu_list.GetNext( pMenu ))
		{
			// Is this the one?
			if (( HMENU )*pMenu == ClsWindow::s_hMenu )
				return pMenu;
		}
	}
	return NULL;
}

// Get state of the UI.
DWORD ClsWindow::GetUIState() const
{
	_ASSERT_VALID( GetSafeHWND()); // Must be valid.

	// Supported?
	if ( ClsGetApp()->GetPlatformID() == VER_PLATFORM_WIN32_NT && ClsGetApp()->GetMajorVersion() >= 5 )
		// Get the UI state.
		return ( DWORD )::SendMessage( m_hWnd, WM_QUERYUISTATE, 0, 0 );
	// No UI state.
	return 0L;
}

// Functions which make use of the ClsDC class.
ClsDC *ClsWindow::BeginPaint( LPPAINTSTRUCT pPaintStruct )	
{ 
	_ASSERT_VALID( GetSafeHWND() ); 
	_ASSERT_VALID( pPaintStruct ); 
	return ClsDC::FromHandle( ::BeginPaint( m_hWnd, pPaintStruct )); 
}

ClsDC *ClsWindow::GetDC()					
{ 
	_ASSERT_VALID( GetSafeHWND() ); 
	return ClsDC::FromHandle( ::GetDC( m_hWnd )); 
}

ClsDC *ClsWindow::GetDCEx( ClsRgn* prgnClip, DWORD flags )	
{ 
	_ASSERT_VALID( GetSafeHWND() ); 
	return ClsDC::FromHandle( ::GetDCEx( m_hWnd, prgnClip ? ( HRGN )*prgnClip : NULL, flags )); 
}

ClsDC *ClsWindow::GetWindowDC()					
{ 
	_ASSERT_VALID( GetSafeHWND() ); 
	return ClsDC::FromHandle( ::GetWindowDC( m_hWnd )); 
}

int ClsWindow::ReleaseDC( ClsDC *pDC )				
{ 
	_ASSERT_VALID( GetSafeHWND() ); 
	return ::ReleaseDC( m_hWnd, *pDC ); 
}
