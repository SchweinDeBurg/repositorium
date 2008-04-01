//
// menu.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//
// Ownerdraw frames is completly based on the NewMenu code by
// Bruno Podetti (Podetti@gmx.net) as found on www.codeproject.com.
// Full credit to him on that one.
//

#include "../stdafx.h"

#include "menu.h"
#include "../windows/window.h"
#include "../coords/rect.h"
#include "../gdi/dc.h"
#include "../gdi/bufferdc.h"
#include "../gdi/getdc.h"
#include "../gdi/windowdc.h"
#include "../tools/multimonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Lists of all temporary objects created.
ClsLinkedList<ClsMenu> global_menu_list;
ClsLinkedList<ClsMenu> temporary_menu_list;

// Finds a menu object in the list by it's handle
// value.
static ClsMenu *ClsFindObjectByHandle( ClsLinkedList<ClsMenu>& list, HMENU hMenu )
{
	_ASSERT_VALID( hMenu ); // This must be valid.

	// Iterate the nodes.
	for ( ClsMenu *pMenu = list.GetFirst(); pMenu; pMenu = list.GetNext( pMenu ))
	{
		// Is the handle wrapped by this object
		// the one we are looking for?
		if ( *pMenu == hMenu )
		{
			// Yes. Return a pointer to the object.
			return pMenu;
		}
	}
	// Object not in the list.
	return NULL;
}

// Constructor. Initializes to the
// passed handle or NULL.
ClsMenu::ClsMenu( HMENU hMenu /* = NULL */ )
{
	// Setup handle.
	m_hMenu = hMenu;

	// Defaults...
	m_bIsDropped = FALSE;

	// Add us to the global list.
	global_menu_list.AddHead( this );
}

// Destructor. Deletes the wrapped menu.
ClsMenu::~ClsMenu()
{
	// Destroy the handle.
	Destroy();

	// Remove us from the global list
	// if we are still linked into it.
	ClsMenu *pMenu;
	for ( pMenu = global_menu_list.GetFirst(); pMenu; pMenu = global_menu_list.GetNext( pMenu ))
	{
		// Is this us?
		if ( pMenu == this )
		{
			// Yes remove us.
			global_menu_list.Remove( pMenu );
			break;
		}
	}
}

// Load a menu from the resources.
BOOL ClsMenu::Load( LPCTSTR pszName )
{
	// Loading a menu destroys the
	// handle previously wrapped by this
	// object unless you detach it first.
	Destroy();

	// Load the menu.
	if (( m_hMenu = ::LoadMenu( ClsGetResourceHandle(), pszName )) != NULL )
		return TRUE;
	return FALSE;
}

// Create a normal or a popup menu handle.
BOOL ClsMenu::Create( BOOL bPopup /* = FALSE */ )
{
	// Creating a menu destroys the
	// handle previously wrapped by this
	// object unless you detach it first.
	Destroy();

	// Create the handle.
	if ( bPopup == FALSE ) m_hMenu = CreateMenu();
	else		       m_hMenu = CreatePopupMenu();

	// Return success or failure.
	return ( BOOL )( m_hMenu != NULL );
}

// Destroy the menu.
BOOL ClsMenu::Destroy()
{
	// Save handle to destroy.
	HMENU	hHandle = m_hMenu;

	// Clear the handle.
	m_hMenu = NULL;

	// Destroy the menu.
	return ::DestroyMenu( hHandle );
}

// Attach a handle.
void ClsMenu::Attach( HMENU hMenu )
{
	_ASSERT_VALID( hMenu ); // Passed handle must be valid.

	// Attaching a new handle will destroy
	// the currently wrapped handle unless
	// you detach it first.
	Destroy();

	// Setup the handle.
	m_hMenu = hMenu;
}

// Detach the wrapped handle.
HMENU ClsMenu::Detach()
{
	_ASSERT_VALID( m_hMenu ); // Unable to detach a NULL handle.

	// Pickup the handle.
	HMENU	hHandle = m_hMenu;

	// Clear the handle.
	m_hMenu = NULL;

	// return the detached handle.
	return hHandle;
}

// Create an object from a handle.
ClsMenu *ClsMenu::FromHandle( HMENU hMenu )
{
	ClsMenu	*pMenu;

	// Valid?
	if ( hMenu == NULL )
		return NULL;

	// Do we have it in the global list?
	if (( pMenu = ClsFindObjectByHandle( global_menu_list, hMenu )) == NULL )
	{
		// No, see if we can find it in the temporary list.
		if (( pMenu = ClsFindObjectByHandle( temporary_menu_list, hMenu )) == NULL )
		{
			// No. Create a temporary new object
			// and add it to the temporary list.
			pMenu = new ClsMenu( hMenu );
			global_menu_list.Remove( pMenu );
			temporary_menu_list.AddHead( pMenu );
		}
	}
	return pMenu;
}

void ClsMenu::DeleteTempObjects()
{
	// Iterate temporary objects deleting
	// them.
	ClsMenu *pMenu;
	while (( pMenu = temporary_menu_list.RemoveHead()) != NULL )
	{
		// Detach the handle from the temporary object.
		pMenu->Detach();
		delete pMenu;
	}
}

// Delete a menu item and it's children.
BOOL ClsMenu::DeleteMenu( UINT uPosition, BOOL bByPos /* = FALSE */ )
{
	_ASSERT_VALID( m_hMenu ); // Can't delete from a NULL handle.

	// Delete the item.
	return ::DeleteMenu( m_hMenu, uPosition, bByPos ? MF_BYPOSITION : MF_BYCOMMAND );
}

// Open the popup menu.
BOOL ClsMenu::TrackPopupMenu( UINT nFlags, int x, int y, HWND hWnd, LPCRECT lpRect /* = NULL */ ) const
{
	_ASSERT_VALID( m_hMenu ); // Must be valid.

	// Open the popup menu.
	return ::TrackPopupMenu( m_hMenu, nFlags, x, y, 0, hWnd, lpRect );
}

// Enable or disable a menu item.
BOOL ClsMenu::EnableItem( UINT uPosition, UINT uFlags, BOOL bByPos /* = FALSE */ )
{
	_ASSERT_VALID( m_hMenu ); // Can't enable in a NULL handle.

	// By position or ID?
	if ( bByPos )
	{
		uFlags &= ~MF_BYCOMMAND;
		uFlags |= MF_BYPOSITION;
	}
	else
	{
		uFlags &= ~MF_BYPOSITION;
		uFlags |= MF_BYCOMMAND;
	}

	// Enable/disable the menu item.
	return ::EnableMenuItem( m_hMenu, uPosition, uFlags );
}

// Get the default item.
UINT ClsMenu::GetDefaultItem( UINT uFlags, BOOL bGetPos /* = FALSE */ ) const
{
	_ASSERT_VALID( m_hMenu ); // No default item in a NULL handle.

	// Return the ID or position of the default item.
	return ::GetMenuDefaultItem( m_hMenu, bGetPos, uFlags );
}

// Set the default item.
BOOL ClsMenu::SetDefaultItem( UINT uPosition, BOOL bByPos /* = FALSE */ )
{
	_ASSERT_VALID( m_hMenu ); // Must be valid.

	// Set the default item.
	return ::SetMenuDefaultItem( m_hMenu, uPosition, bByPos );
}

// Get the number of items in the menu.
int ClsMenu::GetItemCount() const
{
	_ASSERT_VALID( m_hMenu ); // No items in a NULL menu.

	// Return the item count.
	return ::GetMenuItemCount( m_hMenu );
}

// Get the ID of a menu item.
UINT ClsMenu::GetItemID( int nPos ) const
{
	_ASSERT_VALID( m_hMenu ); // No items in a NULL handle.

	// Return the ID of the item.
	return ::GetMenuItemID( m_hMenu, nPos );
}

// Get the info of a menu item.
BOOL ClsMenu::GetItemInfo( UINT uItem, LPMENUITEMINFO lpItemInfo, BOOL bByPos /* = FALSE */ ) const
{
	_ASSERT_VALID( m_hMenu ); // No items in a NULL handle.

	// Get the item info.
	return ::GetMenuItemInfo( m_hMenu, uItem, bByPos, lpItemInfo );
}

// Set the info of a menu item.
BOOL ClsMenu::SetItemInfo( UINT uItem, LPMENUITEMINFO lpItemInfo, BOOL bByPos /* = FALSE */ )
{
	_ASSERT_VALID( m_hMenu ); // No items in a NULL handle.

	// Get the item info.
	return ::SetMenuItemInfo( m_hMenu, uItem, bByPos, lpItemInfo );
}

// Get a submenu.
ClsMenu *ClsMenu::GetSubMenu( int nPos )
{
	_ASSERT_VALID( m_hMenu ); // No items in a NULL handle.

	// Get the sub menu.
	HMENU hMenu = ::GetSubMenu( m_hMenu, nPos );

	// OK?
	if ( hMenu )
		return FromHandle( hMenu );
	return NULL;
}

// Insert a menu item.
BOOL ClsMenu::InsertItem( UINT uItem, LPMENUITEMINFO lpItemInfo, BOOL bByPos /* = FALSE */ )
{
	_ASSERT_VALID( m_hMenu ); // Can't insert in a NULL handle.

	// Insert the menu item.
	return ::InsertMenuItem( m_hMenu, uItem, bByPos, lpItemInfo );
}

// Check a menu item.
BOOL ClsMenu::CheckItem( UINT uIDCheckItem, UINT uCheck, BOOL bByPos /* = FALSE */ )
{
	_ASSERT_VALID( m_hMenu ); // Must be valid.

	// Fix flags.
	if ( bByPos )
	{
		uCheck &= ~MF_BYCOMMAND;
		uCheck |= MF_BYPOSITION;
	}
	else
	{
		uCheck &= ~MF_BYPOSITION;
		uCheck |= MF_BYCOMMAND;
	}

	// (Un)Check the menu item.
	return ::CheckMenuItem( m_hMenu, uIDCheckItem, uCheck );
}

// Check a radio item.
BOOL ClsMenu::CheckRadioItem( UINT uIDFirst, UINT uIDLast, UINT uIDCheck, BOOL bByPos /* = FALSE */ )
{
	_ASSERT_VALID( m_hMenu ); // Must be valid.

	// Check the radio item
	return ::CheckMenuRadioItem( m_hMenu, uIDFirst, uIDLast, uIDCheck, bByPos ? MF_BYPOSITION : MF_BYCOMMAND );
}

// Get the item state.
UINT ClsMenu::GetItemState( UINT uID, BOOL bByPos /* = FALSE */ ) const
{
	_ASSERT_VALID( m_hMenu ); // Must be valid.

	// Setup a MENUITEMINFO structure.
	ClsMenuItemInfo	mi;

	// Get the state information.
	mi.fMask = MIIM_STATE;

	if ( GetItemInfo( uID, &mi, bByPos ))
		return mi.fState;
	return 0;
}

// Obtain the menu context help ID.
DWORD ClsMenu::GetMenuContextHelpId() const
{
	_ASSERT_VALID( m_hMenu ); // Must be valid.

	// Return the ID.
	return ::GetMenuContextHelpId( m_hMenu );
}

// Set the menu context help ID.
BOOL ClsMenu::SetMenuContextHelpId( DWORD dwID )
{
	_ASSERT_VALID( m_hMenu ); // Must be valid.

	// Set the ID.
	return ::SetMenuContextHelpId( m_hMenu, dwID );
}

// Popup is about to open.
void ClsMenu::OnReflectedInitMenuPopup( ClsWindow *pWindow, HMENU hMenu, int nIndex, BOOL bWindowMenu )
{
	// Something dropped? We do not render frames for system menus.
	if ( ! bWindowMenu && ! IsSystemMenu( hMenu ))
		m_bIsDropped = TRUE;

	// Clear parent rectangle.
	if ( ! m_bSetByHand )
		m_LastParent.Empty();

	// Iterate top-level popups.
	for ( int i = 0; i < ::GetMenuItemCount( m_hMenu ); i++ )
	{
		// Is this the one that is about to open?
		if ( hMenu == ::GetSubMenu( m_hMenu, i ))
		{
			// Only set the parent if the menu is the
			// menu attach to the window.
			if ( pWindow->GetMenu() == m_hMenu )
			{
				// Were dropped. Now we determine the screen rectangle
				// of the selected top-level item. This is stored is
				// the m_LastParent rectangle.
				::GetMenuItemRect( pWindow->GetSafeHWND(), m_hMenu, i, m_LastParent );
				m_bSetByHand = FALSE;

				// Re-render menu-bar.
				pWindow->DrawMenuBar();
			}
			return;
		}
	}
}

// Popup is destroyed.
void ClsMenu::OnReflectedUnInitMenuPopup( ClsWindow *pWindow, HMENU hMenu, LPARAM lParam )
{
	// Iterate top-level popups.
	for ( int i = 0; i < ::GetMenuItemCount( m_hMenu ); i++ )
	{
		// Is this the one destroyed?
		if ( hMenu == ::GetSubMenu( m_hMenu, i ))
		{
			// No longer dropped.
			m_bIsDropped = FALSE;
			pWindow->DrawMenuBar();
			return;
		}
	}
}

// Menu loop is finished.
void ClsMenu::OnReflectedExitMenuLoop( ClsWindow *pWindow, BOOL bShortcut )
{
	// No longer dropped.
	m_bIsDropped = FALSE;
}

// ---- Custom frame rendering. This functionalitty is copied from the
// CNewMenu classes by:
//
// Author  : Bruno Podetti
// Email   : Podetti@gmx.net
//
// The original article was found on www.codeproject.com.

HHOOK ClsMenu::s_hNextHook = NULL;	// Next message hook.
ATOM ClsMenu::s_hPropAtom = NULL;	// Property atom.

// Data set as property in the menu window handle.
struct MenuData
{
	WNDPROC		m_lpfnOrigMenuProc;	// Original menu window procedure.
	ClsRect		m_Frame;		// Frame size;
	ClsPoint	m_Orig;			// Position of the window.
	BOOL		m_bDoFrames;		// Do we render our own frames?
	BOOL		m_bDrawBorder;		// Render the frame?
	int		cx, cy;			// Menu width and height.
};

// Menu window procedure. This window procedure does all the necessary
// operations to enable custom menu frames.
LRESULT CALLBACK ClsMenu::MenuWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Get the data property. This may not go wrong or
	// we are a shit-load of trouble.
	MenuData *pData = ( MenuData * )::GetProp( hWnd, ( LPCTSTR )ClsMenu::s_hPropAtom );

	// What's up?
	switch ( uMsg )
	{
		case	WM_CREATE:
		case	0x01E2:
		{
			// Obtain the currently active menu.
			ClsMenu *pMenu = ClsWindow::GetActiveMenu();

			// Do we render our own frames or do we let
			// the system take care of this?
			if ( pMenu ) pData->m_bDoFrames = pMenu->DoFrameRendering();
			else	     pData->m_bDoFrames = FALSE;

			// Special message which is one of the first messages sent to the menu
			// window which is opened first. When we get this message we do a
			// SetWindowPos() which will force the system to send a WM_NCCALCSIZE
			// message to the window.
			if ( uMsg == 0x1E2 )
				::SetWindowPos( hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOREDRAW );
			break;
		}

		case	WM_NCCALCSIZE:
		{
			// Are we doing the frames?
			if ( pData->m_bDoFrames )
			{
				// Get the storage structure.
				LPNCCALCSIZE_PARAMS pCalcSize = ( LPNCCALCSIZE_PARAMS )lParam;

				// Get the active menu and see if it is dropped.
				ClsMenu *pMenu = ClsWindow::GetActiveMenu();
				if ( pMenu && pMenu->IsDropped())
				{
					// Measure the frame.
					pMenu->OnMeasureFrame( pData->m_Frame );

					// Adjust the system input accoordingly.
					pCalcSize->rgrc[ 0 ].left   += pData->m_Frame.Left();
					pCalcSize->rgrc[ 0 ].top    += pData->m_Frame.Top();
					pCalcSize->rgrc[ 0 ].right  -= pData->m_Frame.Right();
					pCalcSize->rgrc[ 0 ].bottom -= pData->m_Frame.Bottom();
					return 0;
				}
			}
			break;
		}

		case	WM_PRINT:
		{
			// Are we doing the frames?
			if ( pData->m_bDoFrames )
			{
				// Visible check?
				if (( lParam & PRF_CHECKVISIBLE ) && ! ::IsWindowVisible( hWnd ))
					break;

				// We must suppress the WM_NCPAINT below to render aswell because that
				// will screw-up the roll-animation of the menus.
				pData->m_bDrawBorder = FALSE;

				// Get the active menu and see if it is dropped.
				ClsMenu *pMenu = ClsWindow::GetActiveMenu();
				if ( pMenu && pMenu->IsDropped())
				{
					// Wrap the input DC.
					ClsDC odc(( HDC )wParam );
					if ( odc.IsValid())
					{
						// Obtain clipping rectangle.
						ClsRect rc, rcw;
						odc.GetClipBox( rc );

						// Create a buffer for the whole area. If we do not Windows 2000
						// wil go kaboom. God knows why but if we do a FillRect() or a
						// FrameRect() and perhaps other things aswell on the input DC we
						// are in trouble.
						//
						// The code is braced so that the buffer goes out of scope before
						// the input DC object does.
						{
							ClsBufferDC dcb( &odc, rc );
							if ( dcb.IsValid())
							{
								// Create a screen rectangle of the menu when it is
								// fully opened.
								rcw = rc;
								rcw.Offset( pData->m_Orig );

								// Call the PreDrawFrame() overidable.
								pMenu->PreDrawFrame( rc, rcw );

								// Get the desktop DC and copy the screen rectangle which will
								// be covered by the menu into the input DC.
								{
									ClsWindowDC wdc( NULL );
									if ( wdc.IsValid())
										dcb.BitBlt( 0, 0, rc.Width(), rc.Height(), &wdc, pData->m_Orig.X(), pData->m_Orig.Y(), SRCCOPY );
								}

								// Call the overidable to render the frame.
								if ( lParam & PRF_NONCLIENT )
									pMenu->OnDrawFrame( dcb, rc, rcw );

								// Adjust the rectangle so that the buffer
								// DC we create below will blit the result
								// at the right offset.
								rc.Left()   += pData->m_Frame.Left();
								rc.Top()    += pData->m_Frame.Top();
								rc.Right()  -= pData->m_Frame.Right();
								rc.Bottom() -= pData->m_Frame.Bottom();

								// Client?
								if ( lParam & PRF_CLIENT )
								{
									// Create buffer DC and call WM_PRINTCLIENT to render the menu
									// contents.
									//
									// This code is braced so that the buffer DC will go out of scope
									// before the input DC will.
									ClsBufferDC bdc( dcb, rc );
									if ( bdc.IsValid())
									{
										::SendMessage( hWnd, WM_ERASEBKGND,  ( WPARAM )( HDC )bdc, 0 );
										::SendMessage( hWnd, WM_PRINTCLIENT, ( WPARAM )( HDC )bdc, lParam );
									}
								}
							}
						}
						// Detach the input DC.
						odc.Detach();
						return 0;
					}
				}
			}
			break;
		}

		case	WM_NCPAINT:
		{
			// Do we render frames and are we allowed to?
			if ( pData->m_bDoFrames && pData->m_bDrawBorder )
			{
				// Get the active menu and see if it is dropped.
				ClsMenu *pMenu = ClsWindow::GetActiveMenu();
				if ( pMenu && pMenu->IsDropped())
				{
					// Obtain DC. Either we obtain the clipped DC or the DC of the
					// whole window depending on the value of WPARAM.
					HDC hDC = NULL;
					if ( wParam != 1 ) hDC = ::GetDCEx( hWnd, ( HRGN )wParam, DCX_WINDOW | DCX_INTERSECTRGN );
					else		   hDC = ::GetWindowDC( hWnd );

					// OK?
					if ( hDC )
					{
						// Obtain clip rectangle of the menu and create a screen
						// rectangle of the menu fully open.
						ClsRect rc, rcw;
						::GetClipBox( hDC, rc );
						rcw = rc;
						rcw.Offset( pData->m_Orig );

						// Call the overidable to render the frame.
						pMenu->PreDrawFrame( rc, rcw );
						pMenu->OnDrawFrame( hDC, rc, rcw );

						// Release the DC.
						::ReleaseDC( hWnd, hDC );
						return 0;
					}
				}
			}

			// We are allowed to render frames again.
			pData->m_bDrawBorder = TRUE;
			break;
		}

		case	WM_WINDOWPOSCHANGING:
		{
			// Are we rendering frames?
			if ( pData->m_bDoFrames )
			{
				// Get the active menu and see if it is dropped.
				ClsMenu *pMenu = ClsWindow::GetActiveMenu();
				if ( pMenu && pMenu->IsDropped())
				{
					// Obtain input data.
					LPWINDOWPOS pPos = ( LPWINDOWPOS )lParam;

					// Adjust the width and height of the menu
					// when necessary.
					if ( ! ( pPos->flags & SWP_NOSIZE ))
					{
						// Compute the adjustments we have to make to the
						// window width and height.
						int nXAdjust = pData->m_Frame.Left() + pData->m_Frame.Right()  - ( 2 * ( ::GetSystemMetrics( SM_CXEDGE ) + 1 ));
						int nYAdjust = pData->m_Frame.Top()  + pData->m_Frame.Bottom() - ( 2 * ( ::GetSystemMetrics( SM_CYEDGE ) + 1 ));

						// Adjust the width and the height.
						pPos->cx += nXAdjust;
						pPos->cy += nYAdjust;

						// Store these values.
						pData->cx = pPos->cx;
						pData->cy = pPos->cy;
					}

					// Adjust the window position to make sure it stays
					// visible completely.
					//if ( ! ( pPos->flags & SWP_NOMOVE ))
					//{
						// Get monitor number.
					//	int nMonitor;
					//	ClsMultiMon mon;
					//	if ( mon.MonitorNumberFromWindow( pPos->hwnd, MONITOR_DEFAULTTONEAREST, nMonitor ))
					//	{
							// Obtain it's rectangle.
					//		ClsRect rcMon;
					//		if ( mon.GetMonitorRect( nMonitor, rcMon ))
					//		{
								// Make sure the entire window remains visible horizontally.
					//			if ( pPos->x + pData->cx > rcMon.Right())
					//				pPos->x = rcMon.Right() - pData->cx;
					//		}
					//	}
					//}

					// Call the overidable.
					pMenu->OnChangeWindowPos( pPos );

					// Save the position.
					pData->m_Orig.X() = pPos->x;
					pData->m_Orig.Y() = pPos->y;
				}
			}
			break;
		}

		case	WM_SHOWWINDOW:
			// The menu window subclassed with message 0x01E2 does not get
			// destroyed. Therefore we remove our stuff when it get's hidden.
			if ( wParam )
				break;

		case	WM_NCDESTROY:
		{ 
			// Remove the property, restore the original windows procedure,
			// call the original windows procedure and free the data.
			::SetWindowLongPtr( hWnd, GWL_WNDPROC, ( LONG_PTR )pData->m_lpfnOrigMenuProc );
			::RemoveProp( hWnd, ( LPCTSTR )ClsMenu::s_hPropAtom );
			LRESULT rc = ::CallWindowProc( pData->m_lpfnOrigMenuProc, hWnd, uMsg, wParam, lParam );
			free( pData );
			return rc;
		}
	}
	// Call the original procedure.
	return ::CallWindowProc( pData->m_lpfnOrigMenuProc, hWnd, uMsg, wParam, lParam );
}

// Message hook procedure.
LRESULT CALLBACK ClsMenu::MessageHookProc( int nCode, WPARAM wParam, LPARAM lParam )
{
	// Do we need to take action on this message?
	if ( nCode == HC_ACTION )
	{
		// Obtain data structure.
		CWPSTRUCT *pCWP = ( CWPSTRUCT * )lParam;

		// We subclass on these two messages only.
		if ( pCWP->message == WM_CREATE || pCWP->message == 0x01E2 )
		{
			// Is the message directed to a menu-class window?
			TCHAR szClassName[ 10 ];
			if ( ::GetClassName( pCWP->hwnd, szClassName, 10 ))
			{
				if ( ! _tcscmp( szClassName, _T( "#32768" )))
				{
					// Does the property already exist?
					if ( ::GetProp( pCWP->hwnd, ( LPCTSTR )ClsMenu::s_hPropAtom ) == NULL )
					{
						// Allocate a data structure.
						MenuData *pData = ( MenuData * )malloc( sizeof( MenuData ));
						if ( pData )
						{
							// Setup the data structure to default values and
							// the original menu window procedure.
							memset( pData, 0, sizeof( MenuData ));
							pData->m_lpfnOrigMenuProc = ( WNDPROC )::SetWindowLongPtr( pCWP->hwnd, GWL_WNDPROC, ( LONG_PTR )ClsMenu::MenuWndProc );
							pData->m_bDrawBorder      = TRUE;

							// Procedure set OK?
							if ( pData->m_lpfnOrigMenuProc )
							{
								// Set property and call the next hook.
								if ( ::SetProp( pCWP->hwnd, ( LPCTSTR )ClsMenu::s_hPropAtom, pData ))
									return ::CallNextHookEx( ClsMenu::s_hNextHook, nCode, wParam, lParam );

								// Ooops, could not set the property. Restore original window procedure.
								::SetWindowLongPtr( pCWP->hwnd, GWL_WNDPROC, ( LONG_PTR )pData->m_lpfnOrigMenuProc );
							}
							// Free the allocated data.
							free( pData );
						}
					}
				}
			}
		}
	}
	// Call the next hook procedure.
	return ::CallNextHookEx( ClsMenu::s_hNextHook, nCode, wParam, lParam );
}

// Install the menu sub-classing hook.
void ClsMenu::InstallMessageHook()
{
	// Only if it does not exist already.
	if ( ClsMenu::s_hNextHook == NULL )
	{
		// Create ATOM.
		ClsMenu::s_hPropAtom = ::AddAtom( _T( "75DD5B06-53A7-4be7-8E23-62176C93731A" ));
		if ( ClsMenu::s_hPropAtom )
		{
			// Install hook.
			if (( ClsMenu::s_hNextHook = ::SetWindowsHookEx( WH_CALLWNDPROC, ClsMenu::MessageHookProc, ::GetModuleHandle( NULL ), ::GetCurrentThreadId())) != NULL )
				return;

			// Hook did not install...
			::DeleteAtom( ClsMenu::s_hPropAtom );
			ClsMenu::s_hPropAtom = NULL;
		}
	}
}

// Remove the menu sub-classing hook.
void ClsMenu::RemoveMessageHook()
{
	// Only if it is really installed.
	if ( ClsMenu::s_hNextHook )
	{
		// Kill the ATOM and remove the hook.
		::DeleteAtom( s_hPropAtom );
		::UnhookWindowsHookEx( ClsMenu::s_hNextHook );
	}

	// Reset...
	ClsMenu::s_hNextHook	  = NULL;
	ClsMenu::s_hPropAtom	  = NULL;
}

// Determine if the passed handle is a system menu.
// A bit of a kludge... simple looks for the SC_CLOSE item...
BOOL ClsMenu::IsSystemMenu( HMENU hMenu )
{
	ClsMenuItemInfo mi;
	mi.fMask = MIIM_ID;
	return ::GetMenuItemInfo( hMenu, SC_CLOSE, FALSE, &mi );
}