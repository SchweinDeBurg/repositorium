#ifndef _MONTHCALENDAR_H_
#define _MONTHCALENDAR_H_
//
// monthcalendar.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// A wrapper for the mont calendar common control.
class ClsMonthCal : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsMonthCal, ClsCommonControl );
public:
	// Constructor.
	ClsMonthCal() : ClsCommonControl( ICC_DATE_CLASSES )
	{;}

	// Destructor.
	virtual ~ClsMonthCal()
	{;}

	// Create a month calendar control.
	inline BOOL Create( ClsWindow *parent, ClsRect& rBounds, UINT nID, DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | MCS_DAYSTATE )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		return ClsCommonControl::Create( 0, MONTHCAL_CLASS, NULL, dwStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Get the specified color.
	inline COLORREF GetColor( int iColor ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the color.
		return ( COLORREF )MonthCal_GetColor( m_hWnd, iColor );
	}

	// Get the current selection.
	inline BOOL GetCurSel( SYSTEMTIME& time ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the selection.
		return MonthCal_GetCurSel( m_hWnd, &time );
	}

	// Get the first day of the week.
	inline DWORD GetFirstDayOfWeek() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the day.
		return MonthCal_GetFirstDayOfWeek( m_hWnd );
	}

	// Get the maximum selection.
	inline DWORD GetMaxSelCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the count.
		return MonthCal_GetMaxSelCount( m_hWnd );
	}

	// Get the width of the "today" string.
	inline DWORD GetMaxTodayWidth() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the width.
		return MonthCal_GetMaxTodayWidth( m_hWnd );
	}

	// Get the minimum control bounds.
	inline BOOL GetMinReqRect( LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the minimum control bounds.
		return ( BOOL )MonthCal_GetMinReqRect( m_hWnd, pRect );
	}

	// Get scroll rate.
	inline int GetMonthDelta() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the scroll rate.
		return MonthCal_GetMonthDelta( m_hWnd );
	}

	// Get the month range.
	inline DWORD GetMonthRange( DWORD dwFlags, LPSYSTEMTIME pSysTimeArray ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the range.
		return MonthCal_GetMonthRange( m_hWnd, dwFlags, pSysTimeArray );
	}

	// Get the range.
	inline DWORD GetRange( LPSYSTEMTIME pSysTimeArray ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the range.
		return MonthCal_GetRange( m_hWnd, pSysTimeArray );
	}

	// Get the selection range.
	inline BOOL GetSelRange( LPSYSTEMTIME pSysTimeArray ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the range.
		return ( BOOL )MonthCal_GetSelRange( m_hWnd, pSysTimeArray );
	}

	// Get the "today" date.
	inline BOOL GetToday( SYSTEMTIME& time ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the date.
		return MonthCal_GetToday( m_hWnd, &time );
	}

	// Get unicode mode.
	inline BOOL GetUnicodeFormat() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the flag.
		return MonthCal_GetUnicodeFormat( m_hWnd );
	}

	// Hittest the control.
	inline DWORD HitTest( MCHITTESTINFO& info ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Hittest the control.
		return ( DWORD )MonthCal_HitTest( m_hWnd, &info );
	}

	// Set the specific color.
	inline COLORREF SetColor( int iColor, COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the color.
		return ( COLORREF )MonthCal_SetColor( m_hWnd, iColor, crColor );
	}

	// Set the selection.
	inline BOOL SetCurSel( SYSTEMTIME& time )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the selection.
		return MonthCal_SetCurSel( m_hWnd, &time );
	}

	// Set the day states for the visible months.
	inline BOOL SatDayState( int iMonths, LPMONTHDAYSTATE pDayStateArray )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the states.
		return ( BOOL )MonthCal_SetDayState( m_hWnd, iMonths, pDayStateArray );
	}

	// Set the first day of the week.
	inline DWORD SetFirstDayOfWeek( int iDay )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the day.
		return ( DWORD )MonthCal_SetFirstDayOfWeek( m_hWnd, iDay );
	}

	// Set the maximum selection count.
	inline BOOL SetMaxSelCount( int iMax )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the maximum.
		return MonthCal_SetMaxSelCount( m_hWnd, iMax );
	}

	// Set the scroll rate.
	inline int SetMonthDelta( int iDelta )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set scroll rate.
		return MonthCal_SetMonthDelta( m_hWnd, iDelta );
	}

	// Set the range.
	inline BOOL SetRange( DWORD dwFlags, LPSYSTEMTIME pSysTimeArray )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the range.
		return MonthCal_SetRange( m_hWnd, dwFlags, pSysTimeArray );
	}

	// Set the selection range.
	inline BOOL SetSelRange( LPSYSTEMTIME pSysTimeArray )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the selection range.
		return ( BOOL )MonthCal_SetSelRange( m_hWnd, pSysTimeArray );
	}

	// Set today.
	inline void SetToday( SYSTEMTIME& time )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set today.
		MonthCal_SetToday( m_hWnd, &time );
	}

	// Set the unicode flag.
	inline BOOL SetUnicodeFormat( BOOL bUnicode )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the flag.
		return MonthCal_SetUnicodeFormat( m_hWnd, bUnicode );
	}
protected:
	// Overidables.
	virtual LRESULT OnGetDayState( LPNMDAYSTATE pDayState, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnSelChange( LPNMSELCHANGE pSelChange, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnSelect( LPNMSELCHANGE pSelChange, BOOL& bNotifyParent )
	{ return 0; }

	// Reflected notification handler which will call
	// the virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// Evaluate notification code.
		switch ( pNMHDR->code )
		{
			case	MCN_GETDAYSTATE: return OnGetDayState(( LPNMDAYSTATE )pNMHDR, bNotifyParent );
			case	MCN_SELCHANGE:   return OnSelChange(( LPNMSELCHANGE )pNMHDR, bNotifyParent );
			case	MCN_SELECT:	 return OnSelect(( LPNMSELCHANGE )pNMHDR, bNotifyParent );
		}
		// Call the base class.
		return ClsCommonControl::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _MONTCALENDAR_H_

