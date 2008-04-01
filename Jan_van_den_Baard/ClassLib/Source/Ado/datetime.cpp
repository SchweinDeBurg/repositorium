//
// datetime.cpp
//
// (C) Copyright 2003 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "datetime.h"
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor, SYSTEMTIME.
ClsDateTime::ClsDateTime( const LPSYSTEMTIME pst )
{ 
	// Convert.
	m_status = valid;
	if ( ! ::SystemTimeToVariantTime( pst, &m_date ))
		// Invalidate this.
		m_status = invalid;
}

// Constructor, initializes to either 0 or the current
// system time.
ClsDateTime::ClsDateTime( BOOL bSystemTime /* = FALSE */ )
{
	// Initialize to the system time?
	m_status = valid;
	if ( bSystemTime )
	{
		// Get the system time.
		SYSTEMTIME st;
		::GetSystemTime( &st );

		// Make the conversion.
		if ( ! ::SystemTimeToVariantTime( &st, &m_date ))
			// Invalidate this.
			m_status = invalid;
	}
	else
		// 0 means 30 December 1899, midnight
		m_date = 0.0;
}

// Constructor, copy.
ClsDateTime::ClsDateTime( const ClsDateTime& dtSrc )
{ 
	// Copy status and value.
	m_status = dtSrc.m_status; 
	m_date = dtSrc.m_date; 
}

// Constructor, values.
ClsDateTime::ClsDateTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec )
{ 
	// Initialize object.
	SetDateTime( nYear, nMonth, nDay, nHour, nMin, nSec ); 
}

// Constructor, DATE.
ClsDateTime::ClsDateTime( DATE dtSrc )
{ 
	// Validate and set value.
	m_status = valid; 
	m_date = dtSrc; 
}

// Constructor, VARIANT.
ClsDateTime::ClsDateTime( const VARIANT& vt )
{ 
	// Assign value.
	*this = vt;  
}

// Destructor, no-op.
ClsDateTime::~ClsDateTime()
{
}

// Convert to a SYSTEMTIME.
BOOL ClsDateTime::GetAsSystemTime( SYSTEMTIME& st ) const
{ 
	// Perform conversion.
	return ( BOOL )::VariantTimeToSystemTime( m_date, &st ); 
}

// Convert to a DBTIMESTAMP.
BOOL ClsDateTime::GetAsDBTIMESTAMP( DBTIMESTAMP& dbt ) const
{
	// Convert to a UDATE structure.
	UDATE udt;
	if ( m_status == valid && ::VarUdateFromDate( m_date, 0, &udt ) == S_OK )
	{
		// Copy values into a DBTIMESTAMP structure.
		dbt.day		= udt.st.wDay;
		dbt.fraction	= 0;
		dbt.hour	= udt.st.wHour;
		dbt.minute	= udt.st.wMinute;
		dbt.month	= udt.st.wMonth;
		dbt.second	= udt.st.wSecond;
		dbt.year	= udt.st.wYear;
		return TRUE;
	}
	return FALSE;
}

// Return as UDATE.
BOOL ClsDateTime::GetAsUDATE( UDATE& udt ) const
{ 
	// Convert...
	return ( BOOL )( ::VarUdateFromDate( m_date, 0, &udt ) == S_OK );
}

// Obtain the day.
int ClsDateTime::GetDay() const
{
	SYSTEMTIME st; 
	return GetAsSystemTime( st ) ? st.wDay : -1; 
}

// Obtain the day of the week.
int ClsDateTime::GetDayOfWeek() const
{ 
	SYSTEMTIME st; 
	return GetAsSystemTime( st ) ? st.wDayOfWeek + 1 : -1; 
}

// Get the day of the year.
int ClsDateTime::GetDayOfYear() const
{ 
	UDATE udt;
	return GetAsUDATE( udt ) ? udt.wDayOfYear : -1;
}

// Get the hour.
int ClsDateTime::GetHour() const
{ 
	SYSTEMTIME st; 
	return GetAsSystemTime( st ) ? st.wHour : -1; 
}

// Get the minute.
int ClsDateTime::GetMinute() const
{ 
	SYSTEMTIME st; 
	return GetAsSystemTime( st ) ? st.wMinute : -1; 
}

// Get the second.
int ClsDateTime::GetSecond() const
{ 
	SYSTEMTIME st; 
	return GetAsSystemTime( st ) ? st.wSecond : -1; 
}

// Get the year.
int ClsDateTime::GetYear() const
{ 
	SYSTEMTIME st; 
	return GetAsSystemTime( st ) ? st.wYear : -1; 
}

// Format the date/time into a string.
ClsString ClsDateTime::Format( DWORD dwFlags /* = 0 */, LCID lcid /* = LANG_USER_DEFAULT */ ) const
{
	// Format the output.
	_bstr_t bstr;
	ClsString str;
	if ( ::VarBstrFromDate( m_date, lcid, dwFlags, &bstr.GetBSTR()) == S_OK )
		// Assign the formatted output.
		str = ( LPCTSTR )bstr;

	// Return the result.
	return str;
}

// Format the date/time into a string.
ClsString ClsDateTime::Format( LPCTSTR pszFormat ) const
{
	// Convert the date.time to an UDATE format.
	TCHAR sz[ 512 ] = { 0 };
	UDATE udt;
	
	// Do the conversion to UDATE.
	if ( GetAsUDATE( udt ))
	{
		// Convert the UDATE to a tm structure.
		struct tm	tm;
		tm.tm_hour	= udt.st.wHour;
		tm.tm_isdst	= 0;
		tm.tm_mday	= udt.st.wDay;
		tm.tm_min	= udt.st.wMinute;
		tm.tm_mon	= udt.st.wMonth - 1;
		tm.tm_sec	= udt.st.wSecond;
		tm.tm_wday	= udt.st.wDayOfWeek;
		tm.tm_yday	= udt.wDayOfYear - 1;
		tm.tm_year	= udt.st.wYear - 1900;
		
		// Convert to the temporary buffer and then assign
		// to the result string.
		_tcsftime( sz, 512, pszFormat, &tm );
	}
	return ClsString( sz );
}

// Format the date/time into a string specified by the
// resource identifier.
ClsString ClsDateTime::Format( UINT nFormatID ) const
{
	ClsString str( MAKEINTRESOURCE( nFormatID ));
	return Format( str );
}

// Set the date/time to the specified values.
BOOL ClsDateTime::SetDateTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec )
{
	// Store values in a SYSTEMTIME structure.
	SYSTEMTIME st = { 0 };
	st.wDay		= ( WORD )nDay;
	st.wHour	= ( WORD )nHour;
	st.wMinute	= ( WORD )nMin;
	st.wMonth	= ( WORD )nMonth;
	st.wSecond	= ( WORD )nSec;
	st.wYear	= ( WORD )nYear;

	// Make the conversion...
	m_status = valid;
	if ( ! ::SystemTimeToVariantTime( &st, &m_date ))
	{
		// Invalidate this.
		m_status = invalid;
		return FALSE;
	}
	return TRUE;
}

// Set the date.
BOOL ClsDateTime::SetDate( int nYear, int nMonth, int nDay )
{ 
	return SetDateTime( nYear, nMonth, nDay, 0, 0, 0 ); 
}

// Set the time.
BOOL ClsDateTime::SetTime( int nHour, int nMin, int nSec )
{ 
	return SetDateTime( 1899, 12, 30, nHour, nMin, nSec ); 
}

// Parse the given date/time string.
BOOL ClsDateTime::ParseDateTime( LPCTSTR pszDate, DWORD dwFlags /* = 0 */, LCID lcid /* = LANG_USER_DEFAULT */ )
{
	// Convert the input string to a DATE value.
	_bstr_t oleStr( pszDate );
	if ( ::VarDateFromStr( oleStr, lcid, dwFlags, &m_date ) != S_OK )
	{
		// Invalidate this.
		m_status = invalid;
		return FALSE;
	}
	return TRUE;
}

// Add a time.
void ClsDateTime::AddTime( int nDays, int nHours, int nMins, int nSecs )
{
	// Values in range?
	_ASSERT( nDays >= 0 && nDays <= 365 );
	_ASSERT( nHours >= 0 && nHours <= 23 );
	_ASSERT( nMins >= 0 && nMins <= 59 );
	_ASSERT( nSecs >= 0 && nSecs <= 59 );

	// Make the conversion...
	m_date += ( DATE )( nDays + ODT_HOURS( nHours ) + ODT_MINS( nMins ) + ODT_SECS( nSecs ));
}

// Subtract a time.
void ClsDateTime::SubTime( int nDays, int nHours, int nMins, int nSecs )
{
	// Values in range?
	_ASSERT( nDays >= 0 && nDays <= 365 );
	_ASSERT( nHours >= 0 && nHours <= 23 );
	_ASSERT( nMins >= 0 && nMins <= 59 );
	_ASSERT( nSecs >= 0 && nSecs <= 59 );

	// Make the conversion...
	m_date -= ( DATE )( nDays + ODT_HOURS( nHours ) + ODT_MINS( nMins ) + ODT_SECS( nSecs ));
}

// Obtain object status.
ClsDateTime::DtStatus ClsDateTime::GetStatus() const
{ 
	// Return the status value.
	return m_status; 
}

// Set the object status.
void ClsDateTime::SetStatus( DtStatus status )
{ 
	// Store the status value.
	m_status = status; 
}

// Set this to the DATE.
ClsDateTime& ClsDateTime::operator=( DATE dt )
{ 
	// Validate and set value.
	m_status = valid; 
	m_date = dt; 
	return *this; 
}

// Set this to the ClsDateTime.
ClsDateTime& ClsDateTime::operator=( const ClsDateTime& dtSrc )
{ 
	// Copy the state and value.
	m_status = dtSrc.m_status; 
	m_date = dtSrc.m_date; 
	return *this; 
}

// Set this to the DATE of the VARIANT.
ClsDateTime& ClsDateTime::operator=( const VARIANT& vt )
{
	// Is the variant of the correct type?
	m_status = valid;
	if ( vt.vt == VT_DATE )
		// Assign the value to this.
		m_date = vt.date;
	else if ( vt.vt == VT_EMPTY || vt.vt == VT_NULL )
		// Null this.
		m_status = null;
	else
	{
		// Create destination variant.
		VARIANTARG vtDest;
		vtDest.vt = VT_EMPTY;

		// Do the conversion.
		if ( ::VariantChangeType( &vtDest, const_cast< VARIANTARG * >( &vt ), 0, VT_DATE ) == S_OK )
			// Assign the result to this.
			// and validate us.
			m_date = vtDest.date;
		else
			// Invalidate this.
			m_status = invalid;
	}
	return *this;
}

// Add DATE to this.
ClsDateTime& ClsDateTime::operator+=( DATE dt )
{
	_ASSERT( m_status == valid );
	
	// Add the value to this.
	m_date += dt;
	return *this;
}

// Add DATE to this.
ClsDateTime ClsDateTime::operator+( DATE dt )
{
	_ASSERT( m_status == valid );
	
	// Add the value to this.
	return ClsDateTime( m_date + dt );
}

// Subract DATE from this.
ClsDateTime& ClsDateTime::operator-=( DATE dt )
{
	_ASSERT( m_status == valid );
	
	// Subtract the value from this.
	m_date -= dt;
	return *this;
}

// Subtract DATE from this.
ClsDateTime ClsDateTime::operator-( DATE dt )
{
	_ASSERT( m_status == valid );
	
	// Subtract the value from this.
	return ClsDateTime( m_date - dt );
}

// DATE >= this?
BOOL ClsDateTime::operator>=( DATE dt )
{ 
	_ASSERT( m_status == valid );
	return ( m_date >= dt ); 
}

// DATE <= this?
BOOL ClsDateTime::operator<=( DATE dt )
{ 
	_ASSERT( m_status == valid );
	return ( m_date <= dt ); 
}

// DATE > this?
BOOL ClsDateTime::operator>( DATE dt )
{ 
	_ASSERT( m_status == valid );
	return ( m_date > dt ); 
}

// DATE < this?
BOOL ClsDateTime::operator<( DATE dt )
{ 
	_ASSERT( m_status == valid );
	return ( m_date < dt ); 
}

// DATE equals this?
BOOL ClsDateTime::operator==( DATE dt )
{ 
	_ASSERT( m_status == valid );
	return ( m_date == dt ); 
}

// DATE differs this?
BOOL ClsDateTime::operator!=( DATE dt )
{ 
	_ASSERT( m_status == valid );
	return ( m_date != dt ); 
}