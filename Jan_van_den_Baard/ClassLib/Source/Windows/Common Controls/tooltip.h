#ifndef _TOOLTIP_H_
#define _TOOLTIP_H_
//
// tooltip.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// A wrapper for the Tooltip common control.
class ClsTooltip : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsTooltip, ClsCommonControl );
public:
	// Constructor.
	ClsTooltip() : ClsCommonControl( ICC_WIN95_CLASSES )
	{;}

	// Destructor.
	virtual ~ClsTooltip()
	{;}

	// Create a tooltip control.
	inline BOOL Create( ClsWindow *parent, DWORD dwStyle = TTS_ALWAYSTIP )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsCommonControl::Create( 0, TOOLTIPS_CLASS, NULL, dwStyle, ClsRect( CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT ), parent ? parent->GetSafeHWND() : NULL, NULL );
	}

	// (De)activates the control.
	inline void Activate( BOOL bActivate = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TTM_ACTIVATE, bActivate );
	}

	// Add a tool.
	inline BOOL AddTool( TOOLINFO& toolinfo )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TTM_ADDTOOL, 0, ( LPARAM )&toolinfo );
	}

	// Delete a tool.
	inline void DelTool( TOOLINFO& toolinfo )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TTM_DELTOOL, 0, ( LPARAM )&toolinfo );
	}

	// Are the tools enumerated.
	inline BOOL EnumTools( UINT iTool, TOOLINFO& toolinfo ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TTM_ENUMTOOLS, iTool, ( LPARAM )&toolinfo );
	}

	// Get current tool info.
	inline BOOL GetCurrentTool( TOOLINFO& toolinfo ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TTM_GETCURRENTTOOL, 0, ( LPARAM )&toolinfo );
	}

	// Get the delay time.
	inline int GetDelayTime( DWORD dwDuration ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, TTM_GETDELAYTIME, dwDuration, 0 );
	}

	// Get the tip/text margins.
	inline void GetMargin( LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		::SendMessage( m_hWnd, TTM_GETMARGIN, 0, ( LPARAM )pRect );
	}

	// Get the maximum tip width.
	inline int GetMaxTipWidth() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, TTM_GETMAXTIPWIDTH, 0, 0 );
	}

	// Get tool text.
	inline void GetText( TOOLINFO& toolinfo ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		::SendMessage( m_hWnd, TTM_GETTEXT, 0, ( LPARAM )&toolinfo );
	}

	// Get the tip background color.
	inline COLORREF GetTipBkColor() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( COLORREF )::SendMessage( m_hWnd, TTM_GETTIPBKCOLOR, 0, 0 );
	}

	// Get the tip text color.
	inline COLORREF GetTipTextColor() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( COLORREF )::SendMessage( m_hWnd, TTM_GETTIPTEXTCOLOR, 0, 0 );
	}

	// Get the number of tools.
	inline int GetToolCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, TTM_GETTOOLCOUNT, 0, 0 );
	}

	// Get information about a tool.
	inline BOOL GetToolInfo( TOOLINFO& toolinfo ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TTM_GETTOOLINFO, 0, ( LPARAM )&toolinfo );
	}

	// Hittest the control.
	inline BOOL HitTest( TTHITTESTINFO& htinfo ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TTM_HITTEST, 0, ( LPARAM )&htinfo );
	}

	// Set a new bounding rectange for the tool.
	inline void NewToolRect( TOOLINFO& toolinfo )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TTM_NEWTOOLRECT, 0, ( LPARAM )&toolinfo );
	}

	// Remove tool from the window.
	inline void Pop()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TTM_POP, 0, 0 );
	}

	// Pass a mouyse event to the tooltip.
	inline void RelayEvent( LPMSG pMsg )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TTM_RELAYEVENT, 0, ( LPARAM )pMsg );
	}

	// Setup the delay time.
	inline void SetDelayTime( DWORD dwDuration, int iTime )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TTM_SETDELAYTIME, dwDuration, MAKELONG( iTime, 0 ));
	}

	// Setup tip/text margins.
	inline void SetMargin( LPRECT pRect )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TTM_SETMARGIN, 0, ( LPARAM )pRect );
	}

	// Setup the maximum tip width.
	inline int SetMaxTipWidth( int iWidth )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )SendMessage( TTM_SETMAXTIPWIDTH, 0, iWidth );
	}

	// Setup the tip background color.
	inline void SetTipBkColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TTM_SETTIPBKCOLOR, crColor );
	}

	// Setup the tip text color.
	inline void SetTipTextColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TTM_SETTIPTEXTCOLOR, crColor );
	}

	// Set tool info.
	inline void SetToolInfo( TOOLINFO& toolinfo )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TTM_SETTOOLINFO, 0, ( LPARAM )&toolinfo );
	}
	
	// (De)activates tool tracking.
	inline void TrackActivate( BOOL bActivate, TOOLINFO& toolinfo )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TTM_TRACKACTIVATE, bActivate, ( LPARAM )&toolinfo );
	}

	// Set the position of a tracking tooltip.
	inline void TrackPosition( int xPos, int yPos )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TTM_TRACKPOSITION, 0, MAKELONG( xPos, yPos ));
	}

	// Force the tool to be redrawn.
	inline void Update()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TTM_UPDATE, 0, 0 );
	}

	// Update a tool text.
	inline void UpdateTipText( TOOLINFO& toolinfo )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TTM_UPDATETIPTEXT, 0, ( LPARAM )&toolinfo );
	}

	// Get window from point.
	inline HWND TTWindowFromPoint( LPPOINT pPoint ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HWND )::SendMessage( m_hWnd, TTM_WINDOWFROMPOINT, 0, ( LPARAM )pPoint );
	}
protected:
	// Overidables.
	virtual LRESULT OnGetDispInfo( LPNMTTDISPINFO pDispInfo, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnPop( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnShow( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	// Reflected notification handler which will call
	// the virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// Evaluate notification code.
		switch ( pNMHDR->code )
		{
			case	TTN_GETDISPINFO: return OnGetDispInfo(( LPNMTTDISPINFO )pNMHDR, bNotifyParent );
			case	TTN_POP:         return OnPop( pNMHDR, bNotifyParent );
			case	TTN_SHOW:	 return OnShow( pNMHDR, bNotifyParent );
		}
		// Call the base class.
		return ClsCommonControl::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _TOOLTIP_H_
