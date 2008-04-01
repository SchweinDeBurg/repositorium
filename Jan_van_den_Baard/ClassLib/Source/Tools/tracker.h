#ifndef _TRACKER_H_
#define _TRACKER_H_
//
// tracker.h
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../windows/window.h"
#include "../coords/all.h"

// This class will take mouse capture and track it's
// movements.
class ClsTracker
{
	_NO_COPY( ClsTracker );
public:
	ClsTracker() {;}
	virtual ~ClsTracker() {;}

	// Implementation.
	void Track( ClsWindow *pWindow );
	
protected:
	// Overidables.
	virtual BOOL OnTrackEnter( ClsWindow *pWindow );
	virtual BOOL OnTrackCancel( ClsWindow *pWindow );
	virtual void OnTrackExit( ClsWindow *pWindow );
	virtual BOOL OnTrackMove( ClsWindow *pWindow );
	virtual BOOL OnMouseButton( ClsWindow *pWindow, UINT uMsg );
	virtual BOOL OnRouteMsg( ClsWindow *pWindow, LPMSG pMsg );

	// Data.
	ClsPoint	m_ptOrigin;
	ClsPoint	m_ptCurrent;
};

#endif // _TRACKER_H_