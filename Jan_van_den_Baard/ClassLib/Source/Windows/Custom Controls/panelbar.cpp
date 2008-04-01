//
// panelbar.cpp
//
// (C) Copyright 2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "../../windows/window.h"
#include "../../collectors/simplearray.h"
#include "../../gdi/dc.h"
#include "panelbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ClsPanelBar::ClsPanelBar() 
{
	// Initialize data.
	m_nActivePanel = 0;
}

ClsPanelBar::~ClsPanelBar() 
{
}

BOOL ClsPanelBar::InsertPanel( ClsPanel *pPanel, ClsPanel *pPred /* = NULL */ )
{
	BOOL bInsert = FALSE;
	int nPos = -1;

	// Insert after a predecessor?
	if ( pPred )
	{
		// Iterate the panels to find the predecessor.
		for ( nPos = 0; nPos < m_PanelArray.GetSize(); nPos++ )
		{
			// Is this the one?
			if ( m_PanelArray[ nPos ].m_pPanel == pPred )
			{
				// Yes. Set the insert flag and break
				// the loop.
				bInsert = TRUE;
				break;
			}
		}

		// If the predecessor was not found we
		// return failure.
		if ( bInsert == FALSE )
			return FALSE;
	}

	// Setup a PanelData structure.
	PanelData Data;
	if ( pPanel->OnCreatePanelCaption())
	{
		// Make the panel a child of us.
		pPanel->ModifyStyle( 0xFFFFFFFF, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN );
		pPanel->SetParent( this );

		// Store the panel in the structure.
		Data.m_pPanel = pPanel;

		// Add or insert the panel structure in the
		// array.
		if ( bInsert == FALSE ) m_PanelArray.Add( Data );
		else 
		{
			// Insert the panel.
			m_PanelArray.InsertAt( Data, nPos + 1 );

			// If we inserted at the currently active
			// position we adjust the active panel.
			if ( nPos + 1 == m_nActivePanel )
				m_nActivePanel++;
		}

		// Set the font to the panel.
		ClsFont font;
		ClsWindow *pParent = GetParent();
		if ( pParent ) pParent->GetFont( font );
		else GetFont( font );
		pPanel->SetFont( &font );

		// Call the overidables.
		pPanel->OnPanelAdded();
		if ( m_PanelArray.GetSize() == 1 ) pPanel->OnPanelActivated();

		// Layout the panels.
		LayoutPanels();
		return TRUE;
	}
	return FALSE;
}

BOOL ClsPanelBar::RemovePanel( ClsPanel *pPanel )
{
	// Find the panel in the array.
	for ( int i = 0; i < m_PanelArray.GetSize(); i++ )
	{
		// Is this the one?
		if ( m_PanelArray[ i ].m_pPanel == pPanel )
		{
			// Yes. Is it the active panel?
			if ( i == m_nActivePanel )
			{
				// Call the overidables.
				m_PanelArray[ i ].m_pPanel->OnPanelDeactivated();
				m_PanelArray[ i ].m_pPanel->OnPanelRemoved();

				// If this was the last panel in the array we
				// activate it's predecessor.
				if ( i == m_PanelArray.GetSize() - 1 ) 
					m_nActivePanel--;

				// Was it the last one?
				if ( m_nActivePanel < 0 ) 
					m_nActivePanel = 0;

				// Remove it from the array.
				m_PanelArray.RemoveAt( i, 1 );

				// If there are panels left we call the overidable
				// of the newly activated one.
				if ( m_PanelArray.GetSize()) 
				{
					m_PanelArray[ m_nActivePanel ].m_pPanel->ShowClient( TRUE );
					m_PanelArray[ m_nActivePanel ].m_pPanel->OnPanelActivated();
				}

				// Layout the remaining panels.
				LayoutPanels();
				return TRUE;
			}
			else 
			{
				// Call the overidable.
				m_PanelArray[ i ].m_pPanel->OnPanelRemoved();

				// If the removed panel was located before the
				// active panel we must decrease the active panel 
				// it's index by one.
				if ( i < m_nActivePanel ) m_nActivePanel--;

				// Remove it from the array.
				m_PanelArray.RemoveAt( i, 1 );

				// Layout the remaining panels.
				LayoutPanels();
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL ClsPanelBar::ActivatePanel( ClsPanel *pPanel )
{
	// Look up the panel.
	for ( int i = 0; i < m_PanelArray.GetSize(); i++ )
	{
		// Is this the one?
		if ( m_PanelArray[ i ].m_pPanel == pPanel )
		{
			// We only do this if it is not already the
			// active panel.
			if ( i != m_nActivePanel )
			{
				// Call the overidable.
				m_PanelArray[ m_nActivePanel ].m_pPanel->OnPanelDeactivated();

				// Tell the currently active panel to hide it's client if it
				// has a client.
				m_PanelArray[ m_nActivePanel ].m_pPanel->ShowClient( FALSE );

				// Make the new panel the active one..
				m_nActivePanel = i;

				// Tell the new active panel to show it's client if it
				// has a client.
				m_PanelArray[ m_nActivePanel ].m_pPanel->ShowClient();

				// Call the overidable.
				m_PanelArray[ m_nActivePanel ].m_pPanel->OnPanelActivated();

				// Layout the panels.
				LayoutPanels();
			}
			return TRUE;
		}
	}
	return FALSE;
}

void ClsPanelBar::LayoutPanels()
{
	// If we do not have panels we simply re-render
	// ourself.
	if ( m_PanelArray.GetSize() == 0 )
	{
		Invalidate();
		return;
	}

	// Initialize data.
	ClsRect rc = GetClientRect();
	ClsPanel *pPanel;
	int nActiveHeight = rc.Height();
	int yOffset = 0, nMaxWidth = 0;

	// Iterate the panels to find out the largest minimum
	// width of the panels.
	for ( int i = 0; i < m_PanelArray.GetSize(); i++ )
	{
		// Get it's minimum size.
		m_PanelArray[ i ].m_pPanel->OnGetMinSize( m_PanelArray[ i ].m_szMinSize );

		// Is the minimum width of this panel larger that the
		// ones we had before? If so store it.
		if ( m_PanelArray[ i ].m_szMinSize.CX() > nMaxWidth )
			nMaxWidth = m_PanelArray[ i ].m_szMinSize.CX();

		// If this is not the active panel we subtract it's minimum
		// height from the client height. When we are done we are left
		// with the height of the active panel.
		if ( i != m_nActivePanel )
			nActiveHeight -= m_PanelArray[ i ].m_szMinSize.CY();
	}

	// Iterate the panels.
	for ( int i = 0; i < m_PanelArray.GetSize(); i++ )
	{
		// Get a pointer to the panel window.
		pPanel = reinterpret_cast<ClsPanel *>( m_PanelArray[ i ].m_pPanel );

		// Is this the active panel?
		if ( i == m_nActivePanel )
		{
			// Reposition and resize it and adjust the vertical
			// offset for the rest of the panels.
			pPanel->SetWindowPos( NULL, 0, yOffset, rc.Width() > nMaxWidth ? rc.Width() : nMaxWidth, nActiveHeight > m_PanelArray[ i ].m_szMinSize.CY() ? nActiveHeight : m_PanelArray[ i ].m_szMinSize.CY(), SWP_NOZORDER );
			yOffset += nActiveHeight > m_PanelArray[ i ].m_szMinSize.CY() ? nActiveHeight : m_PanelArray[ i ].m_szMinSize.CY();
		}
		else
		{
			// Reposition and resize it and adjust the vertical
			// offset for the rest of the panels.
			pPanel->SetWindowPos( NULL, 0, yOffset, rc.Width() > nMaxWidth ? rc.Width() : nMaxWidth, m_PanelArray[ i ].m_szMinSize.CY(), SWP_NOZORDER );
			yOffset += m_PanelArray[ i ].m_szMinSize.CY();
		}
	}
}

LRESULT ClsPanelBar::OnEraseBkgnd( ClsDC *pDC )
{
	// If we do not have any panels we erase the
	// background ourself.
	if ( m_PanelArray.GetSize() == 0 )
		pDC->FillSolidRect( GetClientRect(), ::GetSysColor( COLOR_APPWORKSPACE ));
	return 1;
}

LRESULT ClsPanelBar::OnSize( UINT nSizeType, int nWidth, int nHeight )
{
	// Relayout the panels.
	LayoutPanels();
	return ClsWindow::OnSize( nSizeType, nWidth, nHeight );
}

LRESULT ClsPanelBar::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// Get our parent.
	ClsWindow *pParent = GetParent();

	// Everything else get's passed to our parent
	// or our base class.
	if ( pParent ) return pParent->SendMessage( WM_COMMAND, MAKEWPARAM( nCtrlID, nNotifyCode ), ( LPARAM )hWndCtrl );
	return ClsWindow::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

LRESULT ClsPanelBar::OnNotify( LPNMHDR pNMHDR )
{
	// These will all get passed to our parent
	// if we have one.
	ClsWindow *pParent = GetParent();
	if ( pParent ) return pParent->SendMessage( WM_NOTIFY, pNMHDR->idFrom, ( LPARAM )pNMHDR );
	return ClsWindow::OnNotify( pNMHDR );
}

ClsPanel *ClsPanelBar::GetActivePanel() const
{
	// If the active panel is valid return a
	// pointer to it.
	if ( m_nActivePanel < m_PanelArray.GetSize())
		return m_PanelArray[ m_nActivePanel ].m_pPanel;
	return NULL;
}