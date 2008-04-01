#ifndef _PAGER_H_
#define _PAGER_H_
//
// pager.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// A wrapper for the pager common control.
class ClsPager : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsPager, ClsCommonControl );
public:
	// Constructor.
	ClsPager() : ClsCommonControl( ICC_PAGESCROLLER_CLASS  )
	{;}

	// Destructor.
	virtual ~ClsPager()
	{;}

	// Create a pager control.
	inline BOOL Create( ClsWindow *parent, ClsRect& rBounds, UINT nID, DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | PGS_HORZ )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		return ClsCommonControl::Create( 0, WC_PAGESCROLLER, NULL, dwStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Set mouse forwarding flag.
	inline void ForwardMouse( BOOL bForward )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the flag.
		Pager_ForwardMouse( m_hWnd, bForward );
	}

	// Get the background color.
	inline COLORREF GetBkColor() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the color.
		return Pager_GetBkColor( m_hWnd );
	}

	// Get the border size.
	inline int GetBorder() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the size.
		return Pager_GetBorder( m_hWnd );
	}

	// Get the button size.
	inline int GetButtonSize() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the size.
		return Pager_GetButtonSize( m_hWnd );
	}

	// Get button state.
	inline DWORD GetButtonState( int iButton ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the state.
		return Pager_GetButtonState( m_hWnd, iButton );
	}

	// Get the drop target.
	inline void GetDropTarget( IDropTarget **ppDropTarget ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the drop target.
		Pager_GetDropTarget( m_hWnd, ppDropTarget );
	}

	// Get the paosition.
	inline int GetPos() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get position.
		return Pager_GetPos( m_hWnd );
	}

	// Recompute contained window size.
	inline void RecalcSize()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Recalc.
		Pager_RecalcSize( m_hWnd );
	}

	// Set the background color.
	inline COLORREF SetBkColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the color.
		return Pager_SetBkColor( m_hWnd, crColor );
	}

	// Set the border size.
	inline int SetBorder( int iBorder )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the border size.
		return Pager_SetBorder( m_hWnd, iBorder );
	}

	// Set the button size.
	inline int SetButtonSize( int iSize )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the size.
		return Pager_SetButtonSize( m_hWnd, iSize );
	}

	// Set the contained window.
	inline void SetChild( HWND hWndContained )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the window.
		Pager_SetChild( m_hWnd, hWndContained );
	}

	// Set the position.
	inline int SetPos( int iPos )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the position.
		return Pager_SetPos( m_hWnd, iPos );
	}
protected:
	// Overidables.
	virtual LRESULT OnCalcSize( LPNMPGCALCSIZE pCalcSize, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnScroll( LPNMPGSCROLL pScroll, BOOL& bNotifyParent )
	{ return 0; }

	// Reflected notification handler which will call
	// the virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// Evaluate notification code.
		switch ( pNMHDR->code )
		{
			case	PGN_CALCSIZE: return OnCalcSize(( LPNMPGCALCSIZE )pNMHDR, bNotifyParent );
			case	PGN_SCROLL:   return OnScroll(( LPNMPGSCROLL )pNMHDR, bNotifyParent );
		}
		// Call the base class.
		return ClsCommonControl::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _PAGER_H_