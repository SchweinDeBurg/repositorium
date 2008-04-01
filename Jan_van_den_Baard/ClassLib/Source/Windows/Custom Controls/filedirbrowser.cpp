//
// filedirbrowser.cpp
//
// (C) Copyright 2002-2003 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "filedirbrowser.h"
#include "filedirtree.h"
#include "../../tools/multimonitor.h"
#include "../../tools/theming.h"
#include "../../gdi/getdc.h"
#include "../../gdi/windowdc.h"
#include "../../coords/rect.h"
#include "../../coords/size.h"

// Just in case...
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif

// A popup version of the "ClsFileDirTree" control.
// Handles positioning and destruction itself.
class ClsFileDirTreePopup : public ClsFileDirTree
{
	_NO_COPY( ClsFileDirTreePopup );
public:
	// Constructor. Create and position the control.
	ClsFileDirTreePopup( ClsWindow *pParent, int nMaxLines, LPCTSTR pszLoadingText, LPCTSTR pszPath, LPCTSTR pszFilters, COLORREF crLoadingTextColor, BOOL bShowFiles )
	{
		_ASSERT_VALID( pParent ); // Must be valid.

		// Initialize data.
		m_pParent	   = pParent;
		m_bAbove	   = FALSE;
		m_hTheme	   = NULL;

		// Close any other popups.
		ClosePopups();

		// Create the control.
		ClsRect rc( 0, 0, 0, 0 );
		if ( Create( pParent, rc, 0, WS_POPUP | WS_BORDER | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_DISABLEDRAGDROP | TVS_FULLROWSELECT ))
		{
			// Setup data...
			LoadingTextColor() = crLoadingTextColor;
			LoadingText()	   = pszLoadingText;
			FileFilter()	   = pszFilters;
			ShowFiles( bShowFiles );

			// Get rectangle of the parent or owner control.
			ClsRect parent = pParent->GetWindowRect();

			// Obtain the monitor rectangle which
			// intersects the most with the parent control.
			ClsRect screen;
			ClsMultiMon mon;
			int nMonitor;
			mon.MonitorNumberFromWindow( pParent->GetSafeHWND(), MONITOR_DEFAULTTONEAREST, nMonitor );
			mon.GetMonitorRect( nMonitor, screen, TRUE );

			// Setup the tree defaults.
			SetupTree( pszPath );

			// Select the path in the tree currently in the
			// parent control.
			ClsString pos( *pParent );
			if ( pos.GetStringLength())
				SelectPath( pos );

			// Get selection. If there is none and there is an input path
			// we select and expand the root item.
			HTREEITEM hSel = GetSelection();
			if ( hSel == NULL && pszPath && _tcslen( pszPath ))
			{
				hSel = GetRoot();
				Expand( hSel, TVE_EXPAND );
				SelectItem( hSel );
			}

			// Compute how many lines the popup will show.
			// This will be nMaxLines or the amount
			// expanded items which ever is the smallest value.
			nMaxLines = CountExpandedItems( nMaxLines );

			// Create the rectangle of the popup.
			ClsRect popup;
			popup.Left()   = parent.Left();
			popup.Top()    = parent.Bottom();
			popup.Right()  = parent.Right();
			popup.Bottom() = parent.Bottom() + ( nMaxLines * GetItemHeight()) + ( 2 * ::GetSystemMetrics( SM_CYBORDER ));

			// Store the popup it's minimum size.
			m_szMinSize = popup.Size();

			// Create cursors.
			m_hCursors[ 0 ] = ::LoadCursor( NULL, IDC_ARROW );
			m_hCursors[ 1 ] = ::LoadCursor( NULL, IDC_SIZENWSE );

			// Make sure the popup remains on the screen completely.
			if ( popup.Bottom() > screen.Bottom()) { m_bAbove = TRUE; popup.Offset( 0, -( popup.Height() + parent.Height())); }
			if ( popup.Left() < screen.Left()) popup.Offset( screen.Left() - popup.Left(), 0 );
			else if ( popup.Right() > screen.Right()) popup.Offset( -( popup.Right() - screen.Right()), 0 );

			// Show the popup.
			EnsureVisible( hSel );
			MoveWindow( popup, FALSE );
			ShowWindow( SW_SHOW );
		}
	}

	// Destructor.
	virtual ~ClsFileDirTreePopup()
	{
		// Destroy theme.
		if ( m_hTheme ) ThemingAPI.CloseThemeData( m_hTheme );

		// Destroy the cursors.
		if ( m_hCursors[ 0 ] ) ::DestroyCursor( m_hCursors[ 0 ] );
		if ( m_hCursors[ 1 ] ) ::DestroyCursor( m_hCursors[ 1 ] );
	}

protected:
	// Count the number of expanded item upto nMaxLines.
	int CountExpandedItems( int nMaxLines, HTREEITEM hItem = NULL )
	{
		int nItems = 0;

		// Start at the root item if there
		// is no input item.
		if ( ! hItem ) hItem = GetRoot();

		// Iterate items.
		TVITEM tvi;
		do
		{
			// Get item information.
			tvi.mask      = TVIF_STATE;
			tvi.hItem     = hItem;
			tvi.stateMask = TVIS_EXPANDED;
			if ( GetItem( tvi ))
			{
				// Is this one expanded?
				if ( tvi.state & TVIS_EXPANDED )
					// Recursivly count it's children.
					nItems += CountExpandedItems( nMaxLines, GetChild( hItem ));
				else
					// One more...
					nItems++;

				// Did we reach the limit?
				if ( nItems >= nMaxLines )
					return nMaxLines;
			}
		  // Next...
		} while (( hItem = GetNextSibling( hItem )) != NULL );

		// Return the item count.
		return max( 10, nItems );
	}

	// Adjust creation parameters.
	virtual BOOL PreCreateWindow( LPCREATESTRUCT pCS )
	{
		// Adjust EX flags and pass to the base class.
		pCS->dwExStyle |= WS_EX_TOOLWINDOW;
		return ClsFileDirTree::PreCreateWindow( pCS );
	}

	// Paint the sizegrip.
	void PaintSizeGrip( ClsDC *pDC )
	{
		// Theming?
		if ( ThemingAPI.IsThemingEnabled() && ! m_hTheme )
			m_hTheme = ThemingAPI.OpenThemeData( GetSafeHWND(), L"SCROLLBAR" );

		// Get the clipping rectangle.
		GetClientRect( m_rcSizeGrip );

		// Adjust to fit the gripper.
		m_rcSizeGrip.Top() = m_rcSizeGrip.Bottom() - ::GetSystemMetrics( SM_CYHSCROLL );
		m_rcSizeGrip.Left() = m_rcSizeGrip.Right() - ::GetSystemMetrics( SM_CXVSCROLL );
		m_rcSizeGrip.Offset( -1, -1 );

		// Render the gripper.
		if ( ! m_hTheme )
		{
			// Draw the sizing grip.
			pDC->DrawFrameControl( m_rcSizeGrip, DFC_SCROLL, DFCS_SCROLLSIZEGRIP );

			// Determine the control it's background
			// color.
			COLORREF crBk = GetBkColor();
			if ( crBk == CLR_NONE ) crBk = ::GetSysColor( COLOR_WINDOW );

			// If the top-left pixel of the sizegrip rectangle 
			// differs from the background color we floodfill it with
			// the background color.
			if ( pDC->GetPixel( m_rcSizeGrip.TopLeft()) != crBk )
				pDC->FloodFill( m_rcSizeGrip.Left(), m_rcSizeGrip.Top(), crBk );
		}
		else
			// Themed...
			ThemingAPI.DrawThemeBackground( m_hTheme, *pDC, SBP_SIZEBOX, SZB_RIGHTALIGN, m_rcSizeGrip, NULL );

		// Convert to screen coordinates (for the hittest).
		ClientToScreen( m_rcSizeGrip );
	}

	// WM_SIZE handler. This simply calls the baseclass
	// method and then invalidate the client area so
	// that the control is re-rendered.
	virtual LRESULT OnSize( UINT nSizeType, int nWidth, int nHeight )
	{
		LRESULT rs = ClsFileDirTree::OnSize( nSizeType, nWidth, nHeight );
		Invalidate();
		return rs;
	}

	// WM_PAINT override. Render the control and, when that
	// is done, the sizegrip.
	virtual LRESULT OnPaint( ClsDC *pDC )
	{
		// First the baseclass.
		LRESULT rs = ClsFileDirTree::OnPaint( pDC );

		// We only render the sizegrip when the
		// input DC is not provided.
		if ( ! pDC )
		{
			// Obtain the window DC.
			ClsWindowDC dc( this );
			PaintSizeGrip( &dc );
		}

		// Return the result.
		return rs;
	}

	// Window procedure.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		// What's up?
		switch ( uMsg )
		{
			// A bit expensive but it ensures the Gfx is
			// updated correctly...
			case	WM_MOUSEWHEEL:
			case	WM_HSCROLL:
			case	WM_VSCROLL:
			{
				LRESULT rc;
				rc = ClsFileDirTree::WindowProc( uMsg, wParam, lParam );
				Invalidate();
				return rc;
			}

			// Setup the correct cursor.
			case	WM_SETCURSOR:
			{
				HCURSOR hCursor = NULL;
				switch ( LOWORD( lParam ))
				{
					case	HTCLIENT:
						hCursor = m_hCursors[ 0 ];
						break;

					case	HTBOTTOMRIGHT:
						hCursor = m_hCursors[ 1 ];
						break;
				}

				// If a valid cursor was chosen, set it.
				if ( hCursor )
				{
					SetCursor( hCursor );
					return TRUE;
				}

				// Default handling...
				break;
			}

			case	WM_GETMINMAXINFO:
			{
				// Fill in the minimum size of the control.
				LPMINMAXINFO pmmi = ( LPMINMAXINFO )lParam;
				pmmi->ptMinTrackSize.x = m_szMinSize.CX();
				pmmi->ptMinTrackSize.y = m_szMinSize.CY();
				return 0;
			}

			case	WM_NCHITTEST:
				{
					// Is the cursor inside the size grip?
					if ( m_rcSizeGrip.PtInRect( ClsPoint( LOWORD( lParam ), HIWORD( lParam ))))
						// Set the result to HTBOTTOMRIGHT.
						return HTBOTTOMRIGHT;
				}
				break;

			case	WM_THEMECHANGED:
				// Theming API available?
				if ( ThemingAPI.IsThemingEnabled())
				{
					// Destroy old theme and load the new one.
					if ( m_hTheme ) ThemingAPI.CloseThemeData( m_hTheme );
					m_hTheme = ThemingAPI.OpenThemeData( GetSafeHWND(), L"SCROLLBAR" );
				}
				break;

			case	WM_CREATE:
				m_bIsPopup = TRUE;
				break;

			case	WM_RBUTTONDOWN:
				return 0;

			case	WM_LBUTTONDOWN:
			{
				// Copy mouse position in the hit-test
				// structure.
				TVHITTESTINFO hinf;
				hinf.pt.x = ( int )( SHORT )LOWORD( lParam );
				hinf.pt.y = ( int )( SHORT )HIWORD( lParam );

				// See on what the mouse is located.
				HitTest( hinf );

				// Was the click on the item label?
				if ( hinf.flags & ( TVHT_ONITEMLABEL | TVHT_ONITEMICON ))
				{
					// Are we a file or not?
					BOOL bIsFile = TypeOfSelection() == STYPE_FILE ? TRUE : FALSE;

					// Obtain the full path of the treeview
					// selection.
					ClsString path;
					GetSelectedPath( path );

					// We only accept clicks on the right item type.
					if (( m_bShowFiles && bIsFile ) || ( ! m_bShowFiles && ! bIsFile ))
					{
						// Copy it into the parent.
						m_pParent->SetWindowText( path );

						// Kill the focus which will destroy the popup.
						::SetFocus( NULL );
						return 0;
					}
				}
				break;
			}

			case	WM_SHOWWINDOW:
				// Roll-up or down the popup or hide it whichever is requested.
				if ( AnimateWindow( 100, wParam ? ( m_bAbove ? AW_VER_NEGATIVE : AW_VER_POSITIVE ) : AW_HIDE ))
					return 0;
				break;

			case	WM_MOUSEMOVE:
			{
				// Copy mouse position in the hit-test
				// structure.
				TVHITTESTINFO hinf;
				hinf.pt.x = ( int )( SHORT )LOWORD( lParam );
				hinf.pt.y = ( int )( SHORT )HIWORD( lParam );

				// Select the item under the mouse.
				HTREEITEM hItem = HitTest( hinf );
				if ( hItem ) SelectItem( hItem );
				break;
			}

			case	WM_KILLFOCUS:
				// Delete the object.
				Destroy();
				return 0;

			case	WM_SETFOCUS:
				// Make the parent appear to be active in the
				// title bar.
				GetParent()->SendMessage( WM_NCACTIVATE, TRUE );
				break;
		}
		// Pass to the base class.
		return ClsWindow::WindowProc( uMsg, wParam, lParam );
	}

	void PostNcDestroy()
	{
		// Pass to the base class and delete
		// ourself.
		ClsTreeView::PostNcDestroy();
		delete this;
	}

	// Data.
	BOOL		m_bAbove;	// Open popup above or below the owner.
	ClsWindow      *m_pParent;	// Owner control.

	ClsSize		m_szMinSize;
	ClsRect		m_rcSizeGrip;
	HTHEME		m_hTheme;
	HCURSOR		m_hCursors[ 2 ];
};

// Constructor.
ClsFileDirBrowser::ClsFileDirBrowser()
{
	// Setup data.
	wcscpy( m_szTheme, L"COMBOBOX" );
	m_bShowFiles	     = TRUE;
	m_sLoadingText	     = _T( "Loading..." );
	m_sFilters	     = _T( "*.*" );
	m_crLoadingTextColor = CLR_DEFAULT;
}

void ClsFileDirBrowser::OnBrowserClicked()
{
	// Create/open popup.
	new ClsFileDirTreePopup( this, 15, m_sLoadingText, m_sBasePath,  m_sFilters, m_crLoadingTextColor, m_bShowFiles );
}

void ClsFileDirBrowser::GetButtonSize( ClsDC *pDC, ClsSize& sButtonSize )
{
	// Measure the size of the button.
	ClsRect	rcr;
	pDC->DrawText( _T( " " ), 3, rcr, DT_CALCRECT );
	sButtonSize.CX() = ::GetSystemMetrics( SM_CXVSCROLL );
	sButtonSize.CY() = rcr.Height() + ( 3 * ::GetSystemMetrics( SM_CYFRAME ));
}

void ClsFileDirBrowser::RenderButton( ClsDC *pDC, ClsRect rcClip )
{
	// Theming on?
	if ( ThemingAPI.IsThemingEnabled())
	{
		// Open the theme if it does not exist already.
		if ( ! m_hTheme ) m_hTheme = ThemingAPI.OpenThemeData( GetSafeHWND(), m_szTheme );
		if ( m_hTheme )
		{
			// Determine the rendering flags.
			DWORD dwFlags = CBXS_NORMAL;
			if ( IsWindowEnabled() == FALSE ) dwFlags = CBXS_DISABLED;
			else 
			{
				if ( m_bDown ) dwFlags = CBXS_PRESSED;
				else if ( m_bHasCapture && CursorOnButton()) dwFlags = CBXS_HOT;
			}

			// Clear rectangle and render the button.
			rcClip.Inflate( 1, 1 );
			ThemingAPI.DrawThemeBackground( m_hTheme, *pDC, CP_DROPDOWNBUTTON, dwFlags, rcClip, NULL );
			return;
		}
	}

	// Render the outer button.
	pDC->FillRect( rcClip, ( HBRUSH )( COLOR_BTNFACE + 1 ));
	pDC->DrawEdge( rcClip, BDR_RAISEDOUTER, BF_BOTTOMRIGHT );
	
	// Draw the button frame.
	pDC->DrawFrameControl( rcClip, DFC_SCROLL, ( ! IsWindowEnabled() ? DFCS_INACTIVE : 0 ) | DFCS_SCROLLDOWN | ( m_bDown ? DFCS_FLAT : 0 ));
}

// File dropped...
void ClsFileDirBrowser::OnFileDropped( HDROP hDrop )
{
	// Get the number of files dropped. If there is more than
	// one we use the first.
	if ( UINT nFiles = ::DragQueryFile( hDrop, 0xFFFFFFFF, NULL, 0 ))
	{
		// Get the name of the first file or directory. NOTE: This does
		// not resolve links or shortcuts.
		TCHAR szFile[ MAX_PATH ];
		for ( UINT i = 0; i < nFiles; i++ )
		{
			if ( ::DragQueryFile( hDrop, i, szFile, MAX_PATH ))
			{
				// Is it a directory?
				if (( ::GetFileAttributes( szFile ) & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY )
				{
					// Are we showing files?
					if ( ! m_bShowFiles )
					{
						// Set the name.
						SetWindowText( szFile );
						return;
					}
				}
				else
				{
					// Are we showing files?
					if ( m_bShowFiles )
					{
						// Set the name.
						SetWindowText( szFile );
						return;
					}
				}
			}
		}
		// Get the name of the first drop and strip it's
		// file component if necessary.
		::DragQueryFile( hDrop, 0, szFile, MAX_PATH );
		
		// Show files?
		if ( ! m_bShowFiles )
		{
			// Strip the file part from the name.
			LPTSTR pFile = ::PathFindFileName( szFile );
			if ( pFile ) *pFile = _T( '\0' );
		}

		// Set the name in the control.
		SetWindowText( szFile );
	}
}
