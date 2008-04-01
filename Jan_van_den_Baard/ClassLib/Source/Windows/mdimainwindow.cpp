//
// mdimainwindow.cpp
//
// (C) Copyright 2000-2005 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "mdiwindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor.
ClsMDIMainWindow::ClsMDIMainWindow()
{
	// We are a MDI frame...
	m_bIsMDIFrame = TRUE;
	m_nBaseID     = 0x0BAD;
}

// Destructor.
ClsMDIMainWindow::~ClsMDIMainWindow()
{
}

// Obtain a pointer to the active MDI child.
ClsMDIChildWindow *ClsMDIMainWindow::MDIGetActive( LPBOOL pbMaximized /* = NULL */ ) const
{
	_ASSERT_VALID( GetSafeHWND());

	// Obtain handle to the active child and
	// see if it is valid.
	HWND hWnd = ( HWND )::SendMessage( m_Client.GetSafeHWND(), WM_MDIGETACTIVE, 0, ( LPARAM )pbMaximized );
	if ( ::IsWindow( hWnd ))
	{
		// Iterate the child list.
		for ( int i = 0; i < m_Children.GetSize(); i++ )
		{
			// Is this the one we are looking for?
			if ( m_Children[ i ].m_pChild->GetSafeHWND() == hWnd )
				// Return a pointer to it.
				return m_Children[ i ].m_pChild;
		}
	}
	// No active child found...
	return NULL;
}

// Set a new frame menu.
ClsMenu *ClsMDIMainWindow::MDISetMenu( ClsMenu *pFrameMenu, ClsMenu *pWindowMenu )
{
	_ASSERT_VALID( GetSafeHWND());

	// Set the new menu.
	HMENU hMenu = ( HMENU )m_Client.SendMessage( WM_MDISETMENU, ( WPARAM )( HMENU )( pFrameMenu  ? *pFrameMenu  : NULL ), 
								    ( LPARAM )( HMENU )( pWindowMenu ? *pWindowMenu : NULL ));
	// Wrap the old menu.
	if ( hMenu )
		return ClsMenu::FromHandle( hMenu );
	return NULL;
}

LRESULT ClsMDIMainWindow::OnDestroy()
{
	_ASSERT( s_hMDIClient != NULL ); // Should be NULL.

	// Clear the client handle.
	s_hMDIClient = NULL;
	return ClsWindow::OnDestroy();
}

// Overidable which will create the MDI client.
LRESULT ClsMDIMainWindow::OnCreate( LPCREATESTRUCT pCS )
{
	CLIENTCREATESTRUCT	ccs;

	// Setup structure.
	ccs.hWindowMenu  = NULL;
	ccs.idFirstChild = m_nBaseID;

	// Create the client window.
	HWND hClient = ::CreateWindowEx( WS_EX_CLIENTEDGE, _T( "MDICLIENT" ),
					NULL,
					WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE,
					0, 0, 0, 0,
					GetSafeHWND(),
					( HMENU )0xCAC, 
					ClsGetInstanceHandle(),
					( LPSTR )&ccs );

	// Did it create successfully?
	if ( hClient )
	{
		// There can be only one MDII client.
		_ASSERT( s_hMDIClient == NULL );

		// Setup the MDI client.
		s_hMDIClient = hClient;

		// Attach it to our client object.
		m_Client.Attach( hClient, TRUE );

		// Success.
		return ClsWindow::OnCreate( pCS );
	}

	// Failed to create...
	return -1;
}

// Child removed.
void ClsMDIMainWindow::OnMDIChildRemoved( ClsMDIChildWindow *pWnd )
{
}

// Add child.
BOOL ClsMDIMainWindow::AddChild( ClsMDIChildWindow *pChild )
{
	_ASSERT_VALID( pChild );

	// Build structure and add it to the array.
	MDIChild child;
	child.m_pChild = pChild;
	return m_Children.Add( child );
}

// Remove child.
BOOL ClsMDIMainWindow::RemChild( ClsMDIChildWindow *pChild )
{
	_ASSERT_VALID( pChild );

	// Look up the entry.
	for ( int i = 0; i < m_Children.GetSize(); i++ )
	{
		// Is this the one?
		if ( m_Children[ i ].m_pChild == pChild )
		{
			// Get the window which is removed.
			ClsMDIChildWindow *pWnd = m_Children[ i ].m_pChild;

			// Remove it.
			m_Children.RemoveAt( i, 1 );

			// Call overridable.
			OnMDIChildRemoved( pWnd );
			return TRUE;
		}
	}
	return FALSE;
}
