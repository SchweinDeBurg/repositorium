#ifndef _RECTTRACKER_H_
#define _RECTTRACKER_H_
//
// recttracker.h
//
// (C) Copyright 2002-2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "tracker.h"
#include "../gdi/dc.h"
#include "../gdi/pen.h"

// Show a rectange during mouse-tracking.
class ClsRectTracker : public ClsTracker
{
	_NO_COPY( ClsRectTracker );
public:
	// Rectangle rendering types.
	enum
	{ 
		rttSolid = 0,
		rttHatched = 1
	};

	// Construction/destruction.
	ClsRectTracker();
	virtual ~ClsRectTracker();

	// Implementation.
	void SetRectStyle( int nStyle );
	void GetTrackedRect( ClsWindow *pWindow, ClsRect& rcRect, BOOL bClient = FALSE ); 

protected:
	// Overidables.
	virtual BOOL OnMouseButton( ClsWindow *pWindow, UINT uMsg );
	virtual BOOL OnTrackEnter( ClsWindow *pWindow );
	virtual BOOL OnTrackCancel( ClsWindow *pWindow );
	virtual void OnTrackExit( ClsWindow *pWindow );
	virtual BOOL OnTrackMove( ClsWindow *pWindow );
	
	// Helpers.
	void SetRectSize( LPSIZE lpSize );
	void DrawRect( ClsWindow *pWindow );
	void Rectangle();
	void ComputeRect( ClsWindow *pWindow );
	
	// Data.
	ClsDC		m_DC;
	ClsRect		m_rcRect;
	ClsPoint	m_ptCurrPos;
	BOOL		m_bVisible;
	BOOL		m_bTracking;
	int		m_nStyle;
	int		m_nOldROP2;
};

#endif // _RECTTRACKER_H_
