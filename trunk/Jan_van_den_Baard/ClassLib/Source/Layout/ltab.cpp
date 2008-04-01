//
// ltab.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "../standard.h"
#include "../application.h"
#include "../exceptions/memoryexception.h"
#include "../gdi/getdc.h"
#include "../gdi/selector.h"
#include "../gdi/font.h"
#include "ltab.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Defined in layout.cpp. These are layout engine
// internal messages which this control needs
// access.
#define WM_ISLAYOUTENGINE	WM_APP+1
#define WM_RELAYOUT		WM_APP+2
#define WM_SHOWMEMBERS		WM_APP+5

// Constructor.
ClsLTab::ClsLTab()
{
	// Clear data.
	m_bError	= FALSE;
	m_bMadeVisible	= FALSE;
}

// Destructor. Destroys member objects.
ClsLTab::~ClsLTab()
{
	// Iterate members and delete them.
	ClsTMember *pMember;
	while (( pMember = m_Members.RemoveHead()) != NULL )
		delete pMember;
}

// Create the control.
BOOL ClsLTab::Create( ClsWindow *parent, UINT nID, DWORD dwStyle /* = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS */ )
{
	// Get parent client rectangle. For some reason creating
	// the control with an empty rectangle will result in problems
	// rendering the tabs.
	ClsRect rc;
	if ( parent )
		parent->GetClientRect( rc );

	// Create the control.
	return ClsTab::Create( parent, rc, nID, dwStyle | TCS_SINGLELINE );
}

// Make the currently selected tab visible.
void ClsLTab::MakeCurrentVisible()
{
	// Get the current selection.
	int nCurr = GetCurSel();

	// No current selection?
	if ( nCurr == -1 ) 
	{
		// Make the first page the current
		// selection.
		ClsTab::SetCurSel( 0 );
		
		// Get it again.
		nCurr = GetCurSel();
	}

	// No selection? bye...
	if ( nCurr == -1 )
		return;

	// Setup data to retrieve.
	TCITEM tci;
	tci.mask = TCIF_PARAM;

	// Get the data of the current selection.
	if ( GetItem( nCurr, tci ))
	{
		// Scan members.
		ClsTMember *pMember, *pCurr = NULL;
		for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember )) 
		{
			// Is this the one?
			if ( pMember->m_pMember == ( ClsWindow * )tci.lParam )
				// Mark the member.
				pCurr = pMember;
			else	
			{
				// Hide the control or it's
				// members.
				if ( pMember->m_pMember->SendMessage( WM_ISLAYOUTENGINE )) pMember->m_pMember->SendMessage( WM_SHOWMEMBERS, FALSE );
				else							   pMember->m_pMember->ShowWindow( SW_HIDE );
			}
		}

		// Show the control or it's
		// members.
		if ( pCurr )
		{
			// Setup focus to the member.
			::SetFocus( *pCurr->m_pMember );

			// Is it a layout engine?
			if ( pCurr->m_pMember->SendMessage( WM_ISLAYOUTENGINE ))
				// Tell it to show it's members.
				pCurr->m_pMember->SendMessage( WM_SHOWMEMBERS, TRUE );
			else
				// Show the member.
				pCurr->m_pMember->ShowWindow( SW_SHOW );
		}
		// It's been made visible.
		m_bMadeVisible = TRUE;
	}
}

// Setup current selection bounds.
void ClsLTab::SetCurrentBounds()
{
	// Only need the lParam field which
	// holds the ClsWindow object of the member.
	TCITEM tci;
	tci.mask = TCIF_PARAM;

	// Get current selection data.
	if ( GetItem( GetCurSel(), tci )) 
	{
		// Get control bounds.
		ClsRect rc = GetWindowRect();

		// Adjust the rectangle to fit
		// inside it's bounds.
		//
		// There are a few problems with the tab control and
		// the TCM_ADJUSTRECT message.
		//
		// 1) The combination TCS_BUTTONS | TCS_BOTTOM results
		//    in a faulty rectangle. Specifying the TCS_FLATBUTTONS
		//    style does not help.
		//
		// 2) The combination TCS_BUTTONS | TCS_VERTICAL seems to screw
		//    things up even more.
		//
		AdjustRect( FALSE, rc );

		// Buttons?
		DWORD dwStyle = GetStyle();
		if (( dwStyle & TCS_BUTTONS ) == TCS_BUTTONS ) 
		{
			// Adjust rectangle for the frames.
			rc.Left()   -= ::GetSystemMetrics( SM_CXEDGE );
			rc.Right()  += ::GetSystemMetrics( SM_CXEDGE );
		}
		else
		{
			// We reserve this room in the OnGetMinSize()
			// method.
			if ( dwStyle & TCS_VERTICAL )
			{
				// When the tabs are at the right side we steal 
				// two pixels from the right. Otherwise we steal
				// 'm from the left.
				if ( dwStyle & TCS_RIGHT ) rc.Right() -= 2;
				else			   rc.Left()  += 2;
			}
			else
			{
				// When the tabs are at the bottom we steal 
				// two pixels from the bottom. Otherwise we steal
				// 'm from the top.
				if ( dwStyle & TCS_BOTTOM ) rc.Bottom() -= 2;
				else			    rc.Top()  += 2;
			}
		}

		// Map points to the parent
		// window.
		GetParent()->ScreenToClient( rc );

		// Move/size the member control.
		ClsWindow *pWindow = ( ClsWindow * )tci.lParam;
		_ASSERT_VALID( pWindow ); // Must be valid.
		pWindow->MoveWindow( rc, FALSE );
		
		// Is the member a layout control?
		if ( pWindow->SendMessage( WM_ISLAYOUTENGINE )) 
			// Relayout it's members.
			pWindow->SendMessage( WM_RELAYOUT );

		// Has the current selection been made visible yet?
		if ( m_bMadeVisible == FALSE )
			MakeCurrentVisible();
	}
	// Put the tab control at the bottom
	// of the Z-order so that it does not get
	// in the way of it's members.
	SetWindowPos( HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
}

// Setup a tab.
BOOL ClsLTab::SetTabInfo( ClsWindow *pWindow, LPCTSTR pszTitle, int iImage )
{
	// We only need the lParam value.
	TCITEM tci;
	tci.mask = TCIF_PARAM;

	// Iterate members.
	for ( int i = 0; i <= GetItemCount(); i++ ) 
	{
		// Get the information of the
		// current tab.
		if ( GetItem( i, tci )) 
		{
			// Is this the one?.
			if (( ClsWindow * )tci.lParam == pWindow ) 
			{
				// Yes. Clear mask.
				tci.mask = 0;

				// Set title?
				if ( pszTitle ) 
				{
					tci.mask   |= TCIF_TEXT;
					tci.pszText = ( LPTSTR )pszTitle;
				}

				// Set image?
				if ( iImage != -1 ) 
				{
					tci.mask  |= TCIF_IMAGE;
					tci.iImage = iImage;
				}

				// Set the data.
				return SetItem( i, tci );
			}
		}
	}
	return FALSE;
}

// Add a member.
BOOL ClsLTab::AddMember( ClsWindow *pWindow, ClsWindow *pPred, LPCTSTR pszTitle, int iImage )
{
	_ASSERT_VALID( pWindow ); // Must be valid.
	_ASSERT( pWindow != this ); // We can't be added to ourselves.
	_ASSERT( pPred != this ); // We can't be the predecessor.

	// We need the control to be created.
	if ( GetSafeHWND() == NULL )
	{
		// Set error flag.
		m_bError = TRUE;
		return FALSE;
	}

	try
	{
		// Valid member?
		if ( pWindow ) 
		{
			// Allocate a tab member object.
			ClsTMember *pMember = new ClsTMember;

			// Setup the member.
			pMember->m_pMember = pWindow;

			// Setup a tab item structure.
			TCITEM tci;
			tci.mask   = TCIF_PARAM;
			tci.lParam = ( LPARAM )pWindow;

			// Title?
			if ( pszTitle )
			{
				tci.mask |= TCIF_TEXT;
				tci.pszText = ( LPTSTR )pszTitle;
			}

			// Image?
			if ( iImage != -1 )
			{
				tci.mask  |= TCIF_IMAGE;
				tci.iImage = iImage;
			}

			// Predeccessor known?
			if ( pPred == WND_HEAD )
			{
				// Insert the tab.
				if ( ClsTab::InsertItem( 0, tci ) != -1 )
				{
					// Add the node to the list.
					m_Members.AddHead( pMember );
					return TRUE;
				} 
				else
				{
					// Set error flag.
					m_bError = TRUE;

					// Free the node.
					delete pMember;
					return FALSE;
				}
			}
			else if ( pPred ) 
			{
				// Lookup the predecessor.
				ClsTMember *pTmp;
				int i = 0;
				for ( pTmp = m_Members.GetFirst(); pTmp; pTmp = m_Members.GetNext( pTmp ), i++ ) 
				{
					// Is this the one?
					if ( pTmp->m_pMember == pPred ) 
					{
						// Yes. Since the list is in sequence with the tab
						// control we now know the index for the new member aswell.
						i++;

						// Insert the tab.
						if ( ClsTab::InsertItem( i, tci ) != -1 ) 
						{
							// Insert the node in the list.
							m_Members.Insert( pMember, pTmp );
							return TRUE;
						} 
						else
						{
							// Set error flag.
							m_bError = TRUE;

							// Free the node.
							delete pMember;
							return FALSE;
						}
					}
				}
			}
			else 
			{
				// Insert the tab.
				if ( ClsTab::InsertItem( GetItemCount(), tci ) != -1 )
				{
					// Add the node to the list.
					m_Members.AddTail( pMember );
					return TRUE;
				} 
				else
				{
					// Set error flag.
					m_bError = TRUE;

					// Free the node.
					delete pMember;
					return FALSE;
				}
			}
		}
	}
	catch( ClsMemoryException& me )
	{
		// Out of memory...
		m_bError = TRUE;
		me;
	}
	return FALSE;
}

// Remove a member.
BOOL ClsLTab::RemMember( ClsWindow *pWindow )
{
	_ASSERT( pWindow != this ); // Can't be us.

	// Valid member?
	if ( pWindow ) 
	{
		// First we look it up.
		ClsTMember *pMember;
		int i = 0;
		for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember ), i++ ) {
			// Is this the one?
			if ( pMember->m_pMember == pWindow ) 
			{
				// Remove it from the list.
				m_Members.Remove( pMember );

				// Free the node.
				delete pMember;

				// Remove the tab.
				ClsTab::DeleteItem( i );
				return TRUE;
			}
		}
	}
	return FALSE;
}

// Set current selection. Must do this because a TCN_SELCHANGE
// notification is not sent after doing a SetCurSel().
int ClsLTab::SetCurSel( int iItem )
{
	// Let the base have a go at it first.
	int nSel = ClsTab::SetCurSel( iItem );
	if ( nSel != -1 )
	{
		// Not made visible yet.
		m_bMadeVisible = FALSE;

		// Size the current selection.
		SetCurrentBounds();
	}
	return nSel;
}

// Make the newly selected object visible.
LRESULT ClsLTab::OnSelChange( LPNMHDR pNMHDR, BOOL& bNotifyParent )
{
	// Not made visible yet.
	m_bMadeVisible = FALSE;

	// Resize the current selection.
	SetCurrentBounds();
	return 0;
}

// WM_SIZE message handler.
LRESULT ClsLTab::OnSize( UINT nSizeType, int nWidth, int nHeight )
{
	// Size the current member.
	SetCurrentBounds();
	
	// Go to the base class.
	return ClsTab::OnSize( nSizeType, nWidth, nHeight );
}

// Window procedure override.
LRESULT ClsLTab::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Evaluate message.
	switch ( uMsg )
	{
		case	WM_GETFONT:
			// We always return NULL here.
			return NULL;

		case	WM_SETFONT:
		{
			// Pass on to the members when they do not
			// have a font set yet.
			ClsTMember *pMember;
			for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember ))
			{
				// Does it have a font already?
				if ( pMember->m_pMember->SendMessage( WM_GETFONT ) == NULL )
					// No. Set it.
					pMember->m_pMember->SendMessage( WM_SETFONT, wParam, lParam );
			}

			// Pass to the base class.
			break;
		}
	}
	// Call the base class.
	return ClsTab::WindowProc( uMsg, wParam, lParam );
}

// Get the minimum size.
BOOL ClsLTab::OnGetMinSize( ClsSize& szMinSize )
{
	// Adjust an empty rectangle to include the
	// tab and frame sizes.
	ClsRect rc( 0, 0, 0, 0 );
	AdjustRect( TRUE, rc );

	// We need two pixels of extra room on the tab-side. We need
	// this because the TCM_ADJUSTRECT message creates a rectangle
	// which is two pixels closer to the tabs than to the other
	// rectangle sides.
	DWORD dwStyle = GetStyle();
	if (( dwStyle & TCS_BUTTONS ) != TCS_BUTTONS )
	{
		// When we are vertical we grow two pixels
		// wider. Otherwise we grow two pixels higher.
		if ( dwStyle & TCS_VERTICAL ) rc.Right()  += 2;
		else			      rc.Bottom() += 2;
	}

	// The size of the resulting rectangle is the
	// total size of the tabs including frames etc.
	szMinSize = rc.Size();

	// Iterate the members.
	ClsTMember *pMember;
	int cx = 0, cy = 0;
	for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember )) 
	{
		// Initialize structure.
		ClsSize sz( 0, 0 );
		
		// Get dimensions.
		if ( pMember->m_pMember->OnGetMinSize( sz )) 
		{
			// Any larger values?
			if ( sz.CX() > cx ) cx = sz.CX();
			if ( sz.CY() > cy ) cy = sz.CY();
		}
	}

	// Store values.
	szMinSize.Add( cx, cy );
	return TRUE;
}