#ifndef _DATETIME_H_
#define _DATETIME_H_
//
// datetime.h
//
// (C) Copyright 2003 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../strings/string.h"
#include <oleauto.h>
#include <oledb.h>
#include <comutil.h>

// Conversion macros. Convert hours, minutes or seconds to the fractional
// part of the DATE datatype.
#define ODT_HOURS(x) (( DATE )(( double )x * ( 1.0 / 24.0 )))
#define ODT_MINS(x)  (( DATE )(( double )x * ( 1.0 / ( 24.0 * 60.0 ))))
#define ODT_SECS(x)  (( DATE )(( double )x * ( 1.0 / ( 24.0 * 60.0 * 60.0 ))))

// Simple wrapper which encapsulates the OLE DATE data type.
// Loosly based on the ATL COleDateTime class.
class ClsDateTime
{
public:
	// Status values.
	enum DtStatus
	{
		invalid = -1,
		null = 0,
		valid = 1,
	};

	// Construction.
	ClsDateTime( const LPSYSTEMTIME pst );
	ClsDateTime( BOOL bSystemTime = FALSE );
	ClsDateTime( const ClsDateTime& dtSrc );
	ClsDateTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec );
	ClsDateTime( DATE dtSrc );
	ClsDateTime( const VARIANT& vt );

	// Destruction.
	~ClsDateTime();

	// Implementation.
	BOOL GetAsSystemTime( SYSTEMTIME& st ) const;
	BOOL GetAsDBTIMESTAMP( DBTIMESTAMP& dbt ) const;
	BOOL GetAsUDATE( UDATE& udt ) const;
	int GetDay() const;
	int GetDayOfWeek() const;
	int GetDayOfYear() const;
	int GetHour() const;
	int GetMinute() const;
	int GetSecond() const;
	int GetYear() const;
	ClsString Format( DWORD dwFlags = 0, LCID lcid = LANG_USER_DEFAULT ) const;
	ClsString Format( LPCTSTR pszFormat ) const;
	ClsString Format( UINT nFormatID ) const;
	BOOL SetDateTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec );
	BOOL SetDate( int nYear, int nMonth, int nDay );
	BOOL SetTime( int nHour, int nMin, int nSec );
	BOOL ParseDateTime( LPCTSTR pszDate, DWORD dwFlags = 0, LCID lcid = LANG_USER_DEFAULT );
	void AddTime( int nDays, int nHours, int nMins, int nSecs );
	void SubTime( int nDays, int nHours, int nMins, int nSecs );
	DtStatus GetStatus() const;
	void SetStatus( DtStatus status );

	// Operator overloads.
	inline operator DATE*()		{ return &m_date; }
	inline operator DATE() const	{ return m_date; }
	inline operator ClsString()	{ return Format(); }
	inline operator LPCTSTR()	{ return Format(); }

	ClsDateTime& operator=( DATE dt );
	ClsDateTime& operator=( const ClsDateTime& dtSrc );
	ClsDateTime& operator=( const VARIANT& vt );

	// Addition/subtraction.
	ClsDateTime& operator+=( const ClsDateTime& dt );
	ClsDateTime operator+( const ClsDateTime dt );
	ClsDateTime& operator+=( DATE dt );
	ClsDateTime operator+( DATE dt );

	ClsDateTime& operator-=( const ClsDateTime& dt );
	ClsDateTime operator-( const ClsDateTime& dt );
	ClsDateTime& operator-=( DATE dt );
	ClsDateTime operator-( DATE dt );
	
	BOOL operator>=( const ClsDateTime& dt ); 
	BOOL operator>=( DATE dt ); 
	BOOL operator<=( DATE dt );
	BOOL operator>( DATE dt );
	BOOL operator<( DATE dt );
	BOOL operator==( DATE dt );
	BOOL operator!=( DATE dt );

protected:
	DATE		m_date;
	DtStatus	m_status;
};

#endif // _DATETIME_H_