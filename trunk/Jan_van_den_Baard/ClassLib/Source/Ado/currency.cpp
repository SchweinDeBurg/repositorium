//
// currency.cpp
//
// (C) Copyright 2003 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "currency.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor, default.
ClsCurrency::ClsCurrency()
{ 
	// Validate and set to 0.
	m_status = valid; 
	m_cy.int64 = 0; 
} 

// Constructor, CURRENCY. 
ClsCurrency::ClsCurrency( const CURRENCY& cy )
{ 
	// Validate and set value.
	m_status = valid; 
	m_cy = cy; 
}
// Constructor, copy.
ClsCurrency::ClsCurrency( const ClsCurrency& cySrc )
{ 
	// Copy status and value.
	m_status = cySrc.m_status; 
	m_cy = cySrc.m_cy; 
}

// Constructor, units & franctional units.
ClsCurrency::ClsCurrency( long nUnits, long nFractionalUnits )
{ 
	// Convert the units.
	SetCurrency( nUnits, nFractionalUnits ); 
}

// Constructor, VARIANT.
ClsCurrency::ClsCurrency( const VARIANT& vt )
{ 
	// Assign to the variant.
	*this = vt; 
}

// Destructor, no-op.
ClsCurrency::~ClsCurrency()
{
}

// Format the currency into a string.
ClsString ClsCurrency::Format( DWORD dwFlags /* = 0 */, LCID lcid /* = LANG_USER_DEFAULT */ ) const
{
	// Format the output.
	_bstr_t bstr;
	ClsString str;
	if ( ::VarBstrFromCy( m_cy, lcid, dwFlags, &bstr.GetBSTR()) == S_OK )
		// Assign the formatted output.
		str = ( LPCTSTR )bstr;

	// Return the result. Failure returns an
	// empty string.
	return str;
}

// Parse the given currency string.
BOOL ClsCurrency::ParseCurrency( LPCTSTR pszCy, DWORD dwFlags /* = 0 */, LCID lcid /* = LANG_USER_DEFAULT */ )
{
	// Convert the input string to a CURRENCY value.
	_bstr_t oleStr( pszCy );
	if ( ::VarCyFromStr( oleStr, lcid, dwFlags, &m_cy ) != S_OK )
	{
		// Invalidate this.
		m_status = invalid;
		return FALSE;
	}
	return TRUE;
}

// Set the currency value.
BOOL ClsCurrency::SetCurrency( long nUnits, long nFractionalUnits )
{
	// Fill the fractional units.
	CURRENCY cyFract;
	m_status = invalid;
	if ( ::VarCyFromI4( nFractionalUnits, &cyFract ) == S_OK )
	{
		// Devide by 10000.
		cyFract.int64 /= 10000;
		
		// Fill the units currency.
		CURRENCY cyUnits;	
		if ( ::VarCyFromI4( nUnits, &cyUnits ) == S_OK )
		{
			// Add the two together.
			if ( ::VarCyAdd( cyUnits, cyFract, &m_cy ) == S_OK )
			{
				// Success.
				m_status = valid;
				return TRUE;
			}
		}
	}
	return FALSE;
}

// Obtain object status.
ClsCurrency::CyStatus ClsCurrency::GetStatus() const
{ 
	// Return the status value.
	return m_status; 
}

// Set the object status.
void ClsCurrency::SetStatus( CyStatus status )
{ 
	// Store the status value.
	m_status = status; 
}

// Multiply this with a long and return the result.
ClsCurrency& ClsCurrency::operator*=( long nOperand )
{ 
	// Perform the operation.
	*this = *this * nOperand; 
	return *this; 
}
ClsCurrency ClsCurrency::operator*( long nOperand ) const
{ 
	// Are we valid? If not
	// simply return this.
	if ( m_status != valid )
		return *this;

	// Define result.
	ClsCurrency cyResult;

	// One of the values 0?
	if ( m_cy.int64 != 0 && nOperand != 0 )
	{
		// Multiply the value.
		if ( ::VarCyMulI4( m_cy, nOperand, cyResult ) != S_OK )
			// Invalidate result.
			cyResult.m_status = invalid;
	}
	return cyResult;
}

// Devide this by a long and return the result.
ClsCurrency& ClsCurrency::operator/=( long nOperand )
{ 
	// Perform the operattion.
	*this = *this / nOperand; 
	return *this; 
}
ClsCurrency ClsCurrency::operator/( long nOperand ) const
{ 
	_ASSERT( nOperand != 0 ); // Devision by zero...

	// Devide...
	ClsCurrency cyResult;

	// Are we valid?
	if ( m_status == valid )
	{
		// Do the devision if the operand
		// is non-zero.
		if ( nOperand )	
			cyResult.m_cy.int64 = m_cy.int64 / nOperand;
	}
	else
		// Set the result to no-valid
		cyResult.m_status = invalid;

	return cyResult; 
}

// Add another ClsCurrency to this one and return the result.
ClsCurrency ClsCurrency::operator+( const CURRENCY& cur ) const
{
	// Create a result object.
	ClsCurrency cyResult( *this );
	
	// Add the result.
	if (( m_status != valid ) || ( ::VarCyAdd( cyResult, cur, cyResult ) != S_OK ))
		// Invalidate result.
		cyResult.m_status = invalid;

	// Return the result.
	return cyResult;
}

// Add the ClsCurrency to this.
ClsCurrency& ClsCurrency::operator+=( const CURRENCY& cur )
{ 
	_ASSERT( m_status == valid );

	// Perform the addition.
	*this = *this + cur; 
	return *this; 
}

// Subtract another ClsCurrency from this one and return the result.
ClsCurrency ClsCurrency::operator-( const CURRENCY& cur ) const
{
	// Create a result object.
	ClsCurrency cyResult( *this );
	
	// Subtract the result.
	if (( m_status != valid ) || ( ::VarCySub( cyResult, cur, cyResult ) != S_OK ))
		// Invalidate result.
		cyResult.m_status = invalid;

	// Return the result.
	return cyResult;
}

// Subtract the ClsCurrency from this.
ClsCurrency& ClsCurrency::operator-=( const CURRENCY& cur )
{ 
	_ASSERT( m_status == valid );

	// Perform the subtraction.
	*this = *this - cur; 
	return *this; 
}

// Negate this value and return the result.
ClsCurrency ClsCurrency::operator-() const
{ 
	// Create a result object.
	ClsCurrency cyResult( *this );
	
	// Subtract the result.
	if (( m_status != valid ) || ( ::VarCyNeg( cyResult, cyResult ) != S_OK ))
		// Invalidate result.
		cyResult.m_status = invalid;

	// Return the result.
	return cyResult;
}

// Set this to the CURRENCY.
ClsCurrency& ClsCurrency::operator=( const CURRENCY& cySrc )
{ 
	// Set the status and the value.
	m_status = valid; 
	m_cy = cySrc; 
	return *this; 
}

// Set this to the ClsCurrency.
ClsCurrency& ClsCurrency::operator=( const ClsCurrency& curSrc )
{ 
	// Set the status and the value.
	m_status = curSrc.m_status; 
	m_cy = curSrc.m_cy; 
	return *this; 
}

// Set this to the CY of the VARIANT.
ClsCurrency& ClsCurrency::operator=( const VARIANT& vt )
{
	// Is the variant of the correct type?
	m_status = valid;
	if ( vt.vt == VT_CY )
		// Assign the value to this.
		m_cy = vt.cyVal;
	else if ( vt.vt == VT_EMPTY || vt.vt == VT_NULL )
		// Null this.
		m_status = null;
	else
	{
		// Create destination variant.
		VARIANTARG vtDest;
		vtDest.vt = VT_EMPTY;

		// Do the conversion.
		if ( ::VariantChangeType( &vtDest, const_cast< VARIANTARG * >( &vt ), 0, VT_CY ) == S_OK )
			// Assign the result to this.
			// and validate us.
			m_cy = vtDest.cyVal;
		else
			// Invalidate this.
			m_status = invalid;
	}
	return *this;
}

// CURRENCY == this?
BOOL ClsCurrency::operator==( const CURRENCY& cur ) const
{ 
	_ASSERT( m_status == valid );
	return ( m_cy.int64 == cur.int64 ); 
}

// CURRENCY != this?
inline BOOL ClsCurrency::operator!=( const CURRENCY& cur ) const
{ 
	_ASSERT( m_status == valid );
	return ( m_cy.int64 != cur.int64 ); 
}

// CURRENCY < this?
BOOL ClsCurrency::operator<( const CURRENCY& cur ) const
{ 
	_ASSERT( m_status == valid );
	return ( m_cy.int64 < cur.int64 ); 
}

// CURRENCY > this?
BOOL ClsCurrency::operator>( const CURRENCY& cur ) const
{ 
	_ASSERT( m_status == valid );
	return ( m_cy.int64 > cur.int64 ); 
}

// CURRENCY <= this?
BOOL ClsCurrency::operator<=( const CURRENCY& cur ) const
{ 
	_ASSERT( m_status == valid );
	return ( m_cy.int64 <= cur.int64 ); 
}

// CURRENCY >= this?
BOOL ClsCurrency::operator>=( const CURRENCY& cur ) const
{ 
	_ASSERT( m_status == valid );
	return ( m_cy.int64 >= cur.int64 ); 
}