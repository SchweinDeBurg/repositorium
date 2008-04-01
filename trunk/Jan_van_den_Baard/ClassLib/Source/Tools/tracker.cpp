//
// tracker.cpp
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "tracker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Tracking loop.
void ClsTracker::Track( ClsWindow *pWindow )
{
	_ASSERT_VALID( pWindow );
	_ASSERT_VALID( pWindow->GetSafeHWND());

	// Obtain the current mouse position.
	::GetCursorPos( m_ptOrigin );
	pWindow->ScreenToClient( m_ptOrigin );
	m_ptCurrent = m_ptOrigin;

	// Call the OnTrackEnter() overidable.
	BOOL bReleaseCapture = FALSE;
	if ( OnTrackEnter( pWindow ))
	{
		// Obtain mouse capture.
		if ( ClsWindow::GetCapture() != pWindow )
		{
			pWindow->SetCapture();
			bReleaseCapture = TRUE;
		}

		// Must be us...
		_ASSERT( ClsWindow::GetCapture() == pWindow );

		// Enter tracking loop.
		BOOL bContinueLoop = TRUE;
		MSG msg;
		while ( bContinueLoop && ::GetMessage( &msg, NULL, 0, 0 ))
		{
			// Do we still have capture? If for some reason we do
			// not we exit now.
			if ( ClsWindow::GetCapture() != pWindow )
			{
				// Restore position and break the loop.
				m_ptCurrent = m_ptOrigin;
				break;
			}

			// Evaluate message.
			switch ( msg.message )
			{
				case	WM_LBUTTONDOWN:
				case	WM_LBUTTONUP:
				case	WM_RBUTTONDOWN:
				case	WM_RBUTTONUP:
				case	WM_MBUTTONDOWN:
				case	WM_MBUTTONUP:
					// Call the OnMouseButton() override.
					bContinueLoop = OnMouseButton( pWindow, msg.message );
					break;

				case	WM_MOUSEMOVE:
					// Get current mouse position.
					m_ptCurrent.X() = msg.pt.x;
					m_ptCurrent.Y() = msg.pt.y;
					pWindow->ScreenToClient( m_ptCurrent );

					// Call the override.
					bContinueLoop = OnTrackMove( pWindow );
					break;

				case	WM_KEYDOWN:
					// Escape?
					if ( msg.wParam == VK_ESCAPE )
					{
						// Restore position and and call
						// the OnTrackCancel() override.
						m_ptCurrent = m_ptOrigin;
						bContinueLoop = ! OnTrackCancel( pWindow );
					}
					break;

				default:
				{
					// Get message position.
					ClsPoint pos;
					pos.X() = msg.pt.x;
					pos.Y() = msg.pt.y;
					pWindow->ScreenToClient( pos );

					// Route the message.
					bContinueLoop = OnRouteMsg( pWindow, &msg );
					if ( bContinueLoop && pos != m_ptCurrent )
					{
						// Store new position and call the
						// OnTrackMove() override.
						m_ptCurrent = pos;
						bContinueLoop = OnTrackMove( pWindow );
					}
					break;
				}
			}
		}

		// Release the capture if we have it and
		// this class obtained it.
		if ( bReleaseCapture && ClsWindow::GetCapture() == pWindow )
			::ReleaseCapture();

		// Call the exit overridable.
		OnTrackExit( pWindow );
	}
}

// A mouse button message has arrived. Return TRUE to continue and
// FALSE to cancel tracking loop.
BOOL ClsTracker::OnMouseButton( ClsWindow *pWindow, UINT uMsg )
{
	return TRUE;
}

// Enter tracking loop. Return TRUE to continue and
// FALSE to cancel tracking loop.
BOOL ClsTracker::OnTrackEnter( ClsWindow *pWindow )
{
	return TRUE;
}

// Tracking loop is about to be cancelled. Return TRUE if
// the cancel is permitted FALSE if not.
BOOL ClsTracker::OnTrackCancel( ClsWindow *pWindow )
{
	return TRUE;
}

// Exit tracking loop.
void ClsTracker::OnTrackExit( ClsWindow *pWindow )
{
}

// Mouse was moved during tracking. Return TRUE to continue and
// FALSE to cancel (abort) tracking loop.
BOOL ClsTracker::OnTrackMove( ClsWindow *pWindow )
{
	return TRUE;
}

// Messages which where not handled by the tracking loop will be
// routed to this override.
BOOL ClsTracker::OnRouteMsg( ClsWindow *pWindow, LPMSG pMsg )
{
	// Dispatch the message...
	::DispatchMessage( pMsg );
	return TRUE;
}