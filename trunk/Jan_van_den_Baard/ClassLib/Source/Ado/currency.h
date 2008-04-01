#ifndef _CURRENCY_H_
#define _CURRENCY_H_
//
// currency.h
//
// (C) Copyright 2003 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../strings/string.h"
#include <oleauto.h>
#include <oledb.h>
#include <comutil.h>

// Simple wrapper which encapsulates the CURRENCY data type.
// Loosly based on the ATL COleCurrency class.
class ClsCurrency
{
public:
	// Status values.
	enum CyStatus
	{
		invalid = -1,
		null = 0,
		valid = 1
	};

	// Construction.
	ClsCurrency();
	ClsCurrency( const CURRENCY& cy );
	ClsCurrency( const ClsCurrency& cySrc );
	ClsCurrency( long nUnits, long nFractionalUnits );
	ClsCurrency( const VARIANT& vt );

	// Destruction.
	~ClsCurrency();

	// Implementation.
	ClsString Format( DWORD dwFlags = 0, LCID lcid = LANG_USER_DEFAULT ) const;
	BOOL ParseCurrency( LPCTSTR lpszCurrency, DWORD dwFlags = 0, LCID lcid = LANG_USER_DEFAULT );
	BOOL SetCurrency( long nUnits, long nFractionalUnits );
	CyStatus GetStatus() const;
	void SetStatus( CyStatus status );

	// Operator overloads.
	inline operator CURRENCY*()		{ return &m_cy; }
	inline operator CURRENCY() const	{ return m_cy; }
	inline operator ClsString()		{ return Format(); }
	inline operator LPCTSTR()		{ return Format(); }

	ClsCurrency& operator*=( long nOperand );
	ClsCurrency operator*( long nOperand ) const;

	ClsCurrency& operator/=( long nOperand );
	ClsCurrency operator/( long nOperand ) const;

	ClsCurrency operator+( const CURRENCY& cur ) const;
	ClsCurrency& operator+=( const CURRENCY& cur );
	ClsCurrency operator-( const CURRENCY& cur ) const;
	ClsCurrency& operator-=( const CURRENCY& cur );

	ClsCurrency operator-() const;

	ClsCurrency& operator=( const CURRENCY& cySrc );
	ClsCurrency& operator=( const ClsCurrency& curSrc );
	ClsCurrency& operator=( const VARIANT& vt );

	BOOL operator==( const CURRENCY& cur ) const;
	BOOL operator!=( const CURRENCY& cur ) const;
	BOOL operator<( const CURRENCY& cur ) const;
	BOOL operator>( const CURRENCY& cur ) const;
	BOOL operator<=( const CURRENCY& cur ) const;
	BOOL operator>=( const CURRENCY& cur ) const;

protected:
	CURRENCY	m_cy;
	CyStatus	m_status;
};

#endif // _CURRENCY_H_