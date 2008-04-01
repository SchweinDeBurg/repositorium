//
// mdichildwindow.cpp
//
// (C) Copyright 2000-2001 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "mdiwindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// As defined in "window.cpp".
extern ClsLinkedList<ClsWindow>		global_window_list;
extern ClsWindow *ClsFindObjectByHandle( ClsLinkedList<ClsWindow>& list, HWND hWnd );

// Constructor.
ClsMDIChildWindow::ClsMDIChildWindow()
{
	// Default data.
	m_pMainWindow = NULL;
}

// Destructor.
ClsMDIChildWindow::~ClsMDIChildWindow()
{
}

// Create MDI child.
BOOL ClsMDIChildWindow::Create(	LPCTSTR lpszWindowName, DWORD dwStyle, const ClsRect& crBounds, ClsMDIMainWindow *pFrame )
{
	_ASSERT( GetSafeHWND() == NULL );
	_ASSERT_VALID( pFrame );

	// Create us.
	MDICREATESTRUCT mdcs;
	mdcs.x		= crBounds.Left();
	mdcs.y		= crBounds.Top();
	mdcs.cx		= crBounds.Width();
	mdcs.cy		= crBounds.Height();
	mdcs.hOwner	= ClsGetInstanceHandle();
	mdcs.style	= dwStyle;
	mdcs.szClass	= _T( "ClsMDIChildClass" );
	mdcs.szTitle	= lpszWindowName;
	mdcs.lParam	= ( LPARAM )this;

	HWND hWnd = ( HWND )pFrame->m_Client.SendMessage( WM_MDICREATE, 0, ( LPARAM )&mdcs );
	if ( hWnd )
	{
		// Add us to the frame.
		pFrame->AddChild( this );

		// Store frame.
		m_pMainWindow = pFrame;
		return TRUE;
	}
	return FALSE;
}

// Create MDI child.
BOOL ClsMDIChildWindow::Create(	LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, ClsMDIMainWindow *pFrame )
{
	return Create( lpszWindowName, dwStyle, ClsRect( x, y, x + nWidth, y + nHeight ), pFrame );
}

// Called when activated.
LRESULT ClsMDIChildWindow::OnMDIActivated( ClsWindow *pDeactivated )
{
	return 0;
}

// Called when deactivated.
LRESULT ClsMDIChildWindow::OnMDIDeactivated( ClsWindow *pActivated )
{
	return 0;
}

// Called by the window procedure.
LRESULT ClsMDIChildWindow::OnMDINCCreate( LPCREATESTRUCT pCS )
{
	return TRUE;
}

// Window procedure override.
LRESULT ClsMDIChildWindow::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Get message.
	switch ( uMsg )
	{
		// Activation change?
		case	WM_MDIACTIVATE:
		{
			// Call the appropiate override.
			if ( GetSafeHWND() == ( HWND )wParam      ) return OnMDIDeactivated( lParam ? ClsFindObjectByHandle( global_window_list, ( HWND )lParam ) : NULL );
			else if ( GetSafeHWND() == ( HWND )lParam ) return OnMDIActivated(   wParam ? ClsFindObjectByHandle( global_window_list, ( HWND )wParam ) : NULL );
			break;
		}
	
		case	WM_DESTROY:
		{
			// Remove us from the array.
			if ( m_pMainWindow )
				m_pMainWindow->RemChild( this );
			break;
		}
	}


	// Call the default MDI procedure.
	return DefMDIChildProc( GetSafeHWND(), uMsg, wParam, lParam );
}

// Default MDI child procedure.
LRESULT CALLBACK ClsMDIChildWindow::StaticMDIChildProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// What's the message...
	switch ( uMsg )
	{
		// We capture the WM_NCCREATE message here to link the window handle
		// to it's owner object.
		case	WM_NCCREATE:
		{
			// Get the MDICREATESTRUCT pointer.
			LPMDICREATESTRUCT pmcs = ( LPMDICREATESTRUCT )(( LPCREATESTRUCT )lParam )->lpCreateParams;
			_ASSERT_VALID( pmcs );

			// Attach the handle to it's owner object.
			_ASSERT_VALID( pmcs->lParam );
			if ( ! (( ClsMDIChildWindow * )pmcs->lParam )->Attach( hWnd, TRUE ))
				return FALSE;

			// Call the OnMDINCCreate() override.
			if ((( ClsMDIChildWindow * )pmcs->lParam )->OnMDINCCreate(( LPCREATESTRUCT )lParam ) == FALSE )
				return FALSE;
		}
	}

	// Call the default message handler.
	return DefMDIChildProc( hWnd, uMsg, wParam, lParam );
}