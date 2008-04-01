#ifndef _COMMONCONTROL_H_
#define _COMMONCONTROL_H_
//
// commoncontrol.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../window.h"
#include "../../application.h"

// Base class for all common control classes.
class ClsCommonControl : public ClsWindow
{
	_NO_COPY( ClsCommonControl );

protected:
	// Constructor.
	ClsCommonControl( DWORD dwCtrlType )
	{
		// Load up the control.
		ClsGetApp()->AddCommonControls( dwCtrlType );
	}

	// Destructor.
	virtual ~ClsCommonControl()
	{;}

	// Overidables.
	virtual LRESULT OnChar(	LPNMCHAR pChar, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnClick( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnCustomDraw( LPNMCUSTOMDRAW pCustomDraw, BOOL& bNotifyParent )
	{ return CDRF_DODEFAULT; }

	virtual LRESULT OnDblclk( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnHover( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnKeydown( LPNMKEY pKey, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnKillFocus( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnNCHitTest( LPNMMOUSE pMouse, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnOutOfMemory( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnRClick( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnRDblclk( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnReleasedCapture( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnReturn( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnSetCursor( LPNMMOUSE pMouse, BOOL& bNotifyParent )
	{ return 1; }

	virtual LRESULT OnSetFocus( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnToolTipsCreated( LPNMTOOLTIPSCREATED pToolTipsCreated, BOOL& bNotifyParent )
	{ return 0; }

	// Handle reflected notification messages and call the
	// virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// What's the notification code?
		switch ( pNMHDR->code )
		{
			case	NM_CHAR:	    return OnChar(( LPNMCHAR )pNMHDR, bNotifyParent );
			case	NM_CLICK:	    return OnClick( pNMHDR, bNotifyParent );
			case	NM_CUSTOMDRAW:	    return OnCustomDraw(( LPNMCUSTOMDRAW )pNMHDR, bNotifyParent );
			case	NM_DBLCLK:	    return OnDblclk( pNMHDR, bNotifyParent );
			case	NM_HOVER:	    return OnHover( pNMHDR, bNotifyParent );
			case	NM_KEYDOWN:	    return OnKeydown(( LPNMKEY )pNMHDR, bNotifyParent );
			case	NM_KILLFOCUS:	    return OnKillFocus( pNMHDR, bNotifyParent );
			case	NM_NCHITTEST:	    return OnNCHitTest(( LPNMMOUSE )pNMHDR, bNotifyParent );
			case	NM_OUTOFMEMORY:     return OnOutOfMemory( pNMHDR, bNotifyParent );
			case	NM_RCLICK:	    return OnRClick( pNMHDR, bNotifyParent );
			case	NM_RDBLCLK:	    return OnRDblclk( pNMHDR, bNotifyParent );
			case	NM_RELEASEDCAPTURE: return OnReleasedCapture( pNMHDR, bNotifyParent );
			case	NM_RETURN:	    return OnReturn( pNMHDR, bNotifyParent );
			case	NM_SETCURSOR:	    return OnSetCursor(( LPNMMOUSE )pNMHDR, bNotifyParent );
			case	NM_SETFOCUS:	    return OnSetFocus( pNMHDR, bNotifyParent );
			case	NM_TOOLTIPSCREATED: return OnToolTipsCreated(( LPNMTOOLTIPSCREATED )pNMHDR, bNotifyParent );
		}
		// Pass to the base class.
		return ClsWindow::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _COMMONCONTROL_H_