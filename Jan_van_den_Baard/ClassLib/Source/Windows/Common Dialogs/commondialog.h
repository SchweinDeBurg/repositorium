#ifndef _COMMONDIALOG_H_
#define _COMMONDIALOG_H_
//
// commondialog.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../dialog.h"
#include "../../coords/point.h"
#include "../../tools/multimonitor.h"

// Base class for the common dialog classes.
class ClsCommonDialog : public ClsDialog
{
	_NO_COPY( ClsCommonDialog );

public:
	// Implementation.
	inline BOOL& CenterOnParent() { return ( BOOL & )m_bCenterOnParent; }

protected:
	// Constructor. Defaults data.
	ClsCommonDialog()
	{
		// Setup defaults...
		m_bCenterOnParent = FALSE;
	}

	// Destructor. Does nothing.
	virtual ~ClsCommonDialog()
	{;}

	// Prevent the ClsDialog OnCommand() overidable
	// to handle IDOK and IDCANCEL messages.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
	{ return -1; }

	// Re-position the dialog.
	void RePositionDialog( ClsWindow *pWindow )
	{
		// Center on parent?
		if ( m_bCenterOnParent )
			pWindow->CenterWindow( pWindow->GetParent() );
		else
			// Do we need to re-position the dialog?
			if ( m_ptPos.X() != CW_USEDEFAULT )
		{
			// Get screen dimensions.
			ClsMultiMon mon;
			ClsRect rcmon;
			int nMonitor;
			mon.MonitorNumberFromWindow( pWindow->GetSafeHWND(), MONITOR_DEFAULTTONEAREST, nMonitor );
			mon.GetMonitorRect( nMonitor, rcmon, TRUE );

			// Get the position as set by the derived classes.
			int x = m_ptPos.X();
			int y = m_ptPos.Y();

			// Get the dialog bounds.
			ClsRect rc = pWindow->GetWindowRect();

			// Make sure the whole dialog remains visible.
			if (( x + rc.Width())  > rcmon.Right())  x = rcmon.Right() - rc.Width();
			if ( x < rcmon.Left())		         x = rcmon.Left();
			if (( y + rc.Height()) > rcmon.Bottom()) y = rcmon.Bottom() - rc.Height();
			if ( y < rcmon.Top())		         y = rcmon.Top();
			
			// Move the dialog.
			pWindow->SetWindowPos( NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
		}
	}

	// Data.
	ClsPoint m_ptPos;
	BOOL	 m_bCenterOnParent;
};

#endif // _COMMONDIALOG_H_