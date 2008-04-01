#ifndef _DATETIME_H_
#define _DATETIME_H_
//
// datetime.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// A wrapper for the date and time picker common control.
class ClsDateTime : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsDateTime, ClsCommonControl );
public:
	// Constructor.
	ClsDateTime() : ClsCommonControl( ICC_DATE_CLASSES )
	{;}

	// Destructor.
	virtual ~ClsDateTime()
	{;}

	// Create a date/time picker control.
	inline BOOL Create( ClsWindow *parent, ClsRect& rBounds, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | DTS_SHOWNONE )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsCommonControl::Create( 0, DATETIMEPICK_CLASS, NULL, dwStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Get the mont calendar control.
	inline HWND GetMonthCal() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the control.
		return DateTime_GetMonthCal( m_hWnd );
	}

	// Get the month calendar color.
	inline COLORREF GetMonthCalColor( int iColor ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the specific color.
		return ( COLORREF )DateTime_GetMonthCalColor( m_hWnd, iColor );
	}

	// Get the month calendar font.
	inline ClsFont *GetMonthCalFont() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the font.
		return ClsFont::FromHandle(( HFONT )DateTime_GetMonthCalFont( m_hWnd ));
	}

	// Get minimum and maximum allowable system times.
	inline DWORD GetRange( LPSYSTEMTIME lpSystemTimeArray ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the range.
		return DateTime_GetRange( m_hWnd, lpSystemTimeArray );
	}

	// Get the system time.
	inline DWORD GetSystemtime( SYSTEMTIME& sysTime ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get system time.
		return DateTime_GetSystemtime( m_hWnd, &sysTime );
	}

	// Set the format string.
	inline void SetFormat( LPCTSTR pszFormat )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the format string.
		DateTime_SetFormat( m_hWnd, pszFormat );
	}

	// Set the month calendar color.
	inline COLORREF SetMonthCalColor( int iColor, COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the color.
		return ( COLORREF )DateTime_SetMonthCalColor( m_hWnd, iColor, crColor );
	}

	// Set the month calendar font.
	inline void SetMonthCalFont( HFONT hFont, BOOL bRedraw = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the font.
		DateTime_SetMonthCalFont( m_hWnd, hFont, MAKELPARAM( bRedraw, 0 ));
	}

	// Set the range.
	inline BOOL SetRange( LPSYSTEMTIME lpSystemTimeArray, DWORD dwFlags = GDTR_MIN | GDTR_MAX )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the range.
		return DateTime_SetRange( m_hWnd, dwFlags, lpSystemTimeArray );
	}

	// Set the system time.
	inline BOOL SetSystemtime( LPSYSTEMTIME lpSysTime, DWORD dwFlags )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set system time.
		return DateTime_SetSystemtime( m_hWnd, dwFlags, lpSysTime );
	}

protected:
	// Overidables.
	virtual LRESULT OnCloseUp( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnDateTimeChange( LPNMDATETIMECHANGE pDateTimeChange, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnDropDown( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnFormat( LPNMDATETIMEFORMAT pDateTimeFormat, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnFormatQuery( LPNMDATETIMEFORMATQUERY pDateTimeFormatQuery, BOOL& bNotifyParent )
	{ return 0; /* OK? */ }

	virtual LRESULT OnUserString( LPNMDATETIMESTRING pUserString, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnWMKeyDown( LPNMDATETIMEWMKEYDOWN pDateTimeWNKeyDown, BOOL& bNotifyParent )
	{ return 0; }

	// Reflected notification handler which will call
	// the virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// Evaluate notification code.
		switch ( pNMHDR->code )
		{
			case	DTN_CLOSEUP:	    return OnCloseUp( pNMHDR, bNotifyParent );
			case	DTN_DATETIMECHANGE: return OnDateTimeChange(( LPNMDATETIMECHANGE )pNMHDR, bNotifyParent );
			case	DTN_DROPDOWN:	    return OnDropDown( pNMHDR, bNotifyParent );
			case	DTN_FORMAT:	    return OnFormat(( LPNMDATETIMEFORMAT )pNMHDR, bNotifyParent );
			case	DTN_FORMATQUERY:    return OnFormatQuery(( LPNMDATETIMEFORMATQUERY )pNMHDR, bNotifyParent );
			case	DTN_USERSTRING:     return OnUserString(( LPNMDATETIMESTRING )pNMHDR, bNotifyParent );
			case	DTN_WMKEYDOWN:	    return OnWMKeyDown(( LPNMDATETIMEWMKEYDOWN )pNMHDR, bNotifyParent );
		}
		// Call the base class.
		return ClsCommonControl::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _DATETIME_H_