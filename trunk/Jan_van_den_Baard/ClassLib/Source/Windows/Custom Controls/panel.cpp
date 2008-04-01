//
// panelbar.cpp
//
// (C) Copyright 2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "../../windows/window.h"
#include "../../collectors/simplearray.h"
#include "../../windows/custom controls/flatbutton.h"
#include "../../gdi/dc.h"
#include "panelbar.h"

// ID Caption button.
#define ID_CAPTIONBUTTON	0xBEEF

ClsPanel::ClsPanel() 
{
	// Initialize data.
	m_hFont		= NULL;
	m_pClient	= NULL;
	m_pPanelCaption = NULL;
	m_bIsActive	= FALSE;
}

ClsPanel::~ClsPanel() 
{
	// Do we have a caption button.
	if ( m_pPanelCaption )
	{
		// Destroy the caption button and delete
		// the object.
		m_pPanelCaption->Destroy();
		delete m_pPanelCaption;
		m_pPanelCaption = NULL;
	}
}

BOOL ClsPanel::Create( LPCTSTR pszTitle, ClsRect& rcBounds )
{
	// Create the control.
	if ( ClsWindow::Create( 0, NULL, NULL, WS_CLIPCHILDREN, rcBounds, NULL, NULL ))
	{
		// Setup it's caption.
		if ( SetPanelTitle( pszTitle ))
			return TRUE;

		// Failed...
		Destroy();
	}
	return FALSE;
}

BOOL ClsPanel::OnGetMinSize( ClsSize& szMinSize )
{
	// Use the minimum button size.
	szMinSize.CX() = szMinSize.CY() = 0;
	m_pPanelCaption->OnGetMinSize( szMinSize );

	// Store the button height.
	m_nCaptionHeight = szMinSize.CY();
	return TRUE;
}

BOOL ClsPanel::ShowClient( BOOL bShow /* = TRUE */ )
{
	// Do we have a client?
	if ( m_pClient )
		// Show or hide the client.
		m_pClient->ShowWindow( bShow ? SW_SHOW : SW_HIDE );
	return FALSE;
}

ClsWindow *ClsPanel::SetPanelClient( ClsWindow *pWindow )
{
	// Store the previous client.
	ClsWindow *pPrevClient = m_pClient;

	// Store the client.
	m_pClient = pWindow;

	// Make us it's parent.
	pWindow->SetParent( this );

	// Are we the active panel? If so size the client so that it
	// takes up the client area of the panel.
	if ( m_bIsActive )
		pWindow->SetWindowPos( NULL, 0, m_nCaptionHeight, GetClientRect().Width(), GetClientRect().Height() - m_nCaptionHeight, SWP_NOZORDER );

	// Return the previous client.
	return pPrevClient;
}

BOOL ClsPanel::SetPanelTitle( LPCTSTR pszTitle )
{
	// Store the title pointer. No need to copy the
	// string because the pointer is not used outside
	// this method and the OnCreatePanelCaption()
	// overidable.
	m_strCaption = pszTitle;

	// Do we have a caption button?
	if ( m_pPanelCaption == NULL )
	{
		// No. Create one now.
		if ( OnCreatePanelCaption() == FALSE )
			return FALSE;
	}
	else
		// Set the new label on the caption button.
		m_pPanelCaption->SetWindowText( pszTitle );

	// Relayout the parent.
	ClsPanelBar *pParent = reinterpret_cast<ClsPanelBar *>( GetParent());
	if ( pParent )
	{
		pParent->LayoutPanels();
		pParent->Invalidate();
	}
	return TRUE;
}

LRESULT ClsPanel::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// What's up?
	switch ( uMsg )
	{
		case	WM_SETFONT:
			// Pass to the client and the caption
			// button.
			if ( m_pPanelCaption ) 
				m_pPanelCaption->SendMessage( WM_SETFONT, wParam, lParam );
			if ( m_pClient ) 
				m_pClient->SendMessage( WM_SETFONT, wParam, lParam );
			m_hFont = ( HFONT )wParam;

			// Do we have a panel caption?
			if ( m_pPanelCaption )
			{
				// Get the minimum size of the
				// panel caption.
				ClsSize sz;
				ClsRect rc = GetClientRect();
				m_pPanelCaption->OnGetMinSize( sz );

				// Save the new value.
				m_nCaptionHeight = sz.CY();

				// Simulate a resize of the control. This will
				// re-evaluate the caption and client positions.
				OnSize( 0, rc.Width(), rc.Height());

				// Make sure our parent, which _MUST_
				// be a ClsPanelBar or drived object,
				// relayouts it's panels.
				ClsPanelBar *pParent = reinterpret_cast<ClsPanelBar *>( GetParent());
				if ( pParent ) pParent->LayoutPanels();
			}
			break;

		case	WM_GETFONT: 
			// Return the stored font.
			return ( LRESULT )m_hFont;
	}
	// Pass to the base class.
	return ClsWindow::WindowProc( uMsg, wParam, lParam );
}

LRESULT ClsPanel::OnEraseBkgnd( ClsDC *pDC )
{
	// If we do not have a client we erase the background
	// ourselves if we are active
	if ( m_pClient == NULL && m_bIsActive )
	{
		ClsRect rc = GetClientRect();
		pDC->FillSolidRect( rc.Left(), rc.Top() + m_nCaptionHeight, rc.Width(), rc.Height() - m_nCaptionHeight, ::GetSysColor( COLOR_APPWORKSPACE ));
	}
	return 1;
}

LRESULT ClsPanel::OnDestroy()
{
	// Delete the caption if we have one.
	if ( m_pPanelCaption )
	{
		// Destroy the caption button and delete
		// the object.
		m_pPanelCaption->Destroy();
		delete m_pPanelCaption;
		m_pPanelCaption = NULL;
	}
	return ClsWindow::OnDestroy();
}

LRESULT ClsPanel::OnSize( UINT nSizeType, int nWidth, int nHeight )
{
	// Do we hava a caption?
	if ( m_pPanelCaption )
		// Resize and reposition it.
		m_pPanelCaption->SetWindowPos( NULL, 0, 0, nWidth, m_nCaptionHeight, SWP_NOMOVE | SWP_NOZORDER );
	
	// Do we have a client?
	if ( m_pClient )
		// Resize and reposition it.
		m_pClient->SetWindowPos( NULL, 0, m_nCaptionHeight, nWidth, nHeight - m_nCaptionHeight,  SWP_NOZORDER );

	// Pass to the base class.
	return ClsWindow::OnSize( nSizeType, nWidth, nHeight );
}

LRESULT ClsPanel::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// Get our parent.
	ClsWindow *pParent = GetParent();

	// Is it our caption button?
	if ( pParent && nCtrlID == ID_CAPTIONBUTTON )
	{
		// Activate us.
		ClsPanelBar *pBar = reinterpret_cast< ClsPanelBar *>( pParent );
		pBar->ActivatePanel( this );
		return 0;
	}

	// Everything else get's passed to our parent
	// or our base class.
	if ( pParent ) return pParent->SendMessage( WM_COMMAND, MAKEWPARAM( nCtrlID, nNotifyCode ), ( LPARAM )hWndCtrl );
	return ClsWindow::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

LRESULT ClsPanel::OnNotify( LPNMHDR pNMHDR )
{
	// These will all get passed to our parent
	// if we have one.
	ClsWindow *pParent = GetParent();
	if ( pParent ) return pParent->SendMessage( WM_NOTIFY, pNMHDR->idFrom, ( LPARAM )pNMHDR );
	return ClsWindow::OnNotify( pNMHDR );
}

void ClsPanel::OnPanelAdded()
{
	// Simply show ourselves.
	ShowWindow( SW_SHOW );
}

void ClsPanel::OnPanelRemoved()
{
	// Simply hide ourselves.
	ShowWindow( SW_HIDE );
}

void ClsPanel::OnPanelActivated()
{
	// We are active.
	m_bIsActive = TRUE;
	if ( m_pClient ) m_pClient->SetFocus();
}

void ClsPanel::OnPanelDeactivated()
{
	// We are inactive.
	m_bIsActive = FALSE;
}

BOOL ClsPanel::OnCreatePanelCaption()
{
	// Do we have a button?
	if ( m_pPanelCaption == NULL )
	{
		// Allocate a caption button.
		ClsCaptionButton *pCaption = new ClsCaptionButton();
		if ( pCaption )
		{
			// Create the button.
			if ( pCaption->Create( this, ClsRect( 0, 0, 0, 0 ), ID_CAPTIONBUTTON, m_strCaption ) == TRUE )
			{
				// Configure the button.
				pCaption->SetHotFrame( TRUE );
				pCaption->IsPanelHeader() = TRUE;
				m_pPanelCaption = reinterpret_cast<ClsButton *>( pCaption );
				return TRUE;
			}
		}
	}
	else
		return TRUE;
	return FALSE;
}