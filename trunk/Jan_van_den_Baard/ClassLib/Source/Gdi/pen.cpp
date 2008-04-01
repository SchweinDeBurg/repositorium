//
// pen.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "pen.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor. Initializes to NULL.
ClsPen::ClsPen()
{
}

// Constructor. Copy constructor.
ClsPen::ClsPen( const ClsPen& cPen )
{
	// Copy the pen.
	operator=(( HPEN )cPen.m_hGdiObject );
}

// Constructor. Copy from a handle.
ClsPen::ClsPen( HPEN hPen )
{
	// Copy the pen.
	operator=( hPen );
}

// Constructor. Creates a pen.
ClsPen::ClsPen( int nStyle, int nWidth, COLORREF crColor )
{
	// Create the pen.
	Attach( ::CreatePen( nStyle, nWidth, crColor ));
}

// Constructor. Create a pen width the ExtCreatePen() API.
ClsPen::ClsPen( DWORD dwStyle, DWORD dwWidth, const LOGBRUSH *pLogBrush, DWORD dwStyleCount, const LPDWORD lpdwStyle )
{
	// Create the pen.
	Attach( ::ExtCreatePen( dwStyle, dwWidth, pLogBrush, dwStyleCount, lpdwStyle ));
}

// Constructor. Creates a pen from a LOGPEN.
ClsPen::ClsPen( const LOGPEN *pLogPen )
{
	// Create the pen.
	Attach( ::CreatePenIndirect( pLogPen ));
}

// Destructor. Does nothing.
ClsPen::~ClsPen()
{
}

// Create a pen.
BOOL ClsPen::CreatePen( int nStyle, int nWidth, COLORREF crColor )
{
	// Attach the pen.
	Attach( ::CreatePen( nStyle, nWidth, crColor ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create a pen from a LOGPEN.
BOOL ClsPen::CreatePenIndirect( const LOGPEN *pLogPen )
{
	// Attach the pen.
	Attach( ::CreatePenIndirect( pLogPen ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create a pen.
BOOL ClsPen::ExtCreatePen( DWORD dwStyle, DWORD dwWidth, const LOGBRUSH *pLogBrush, DWORD dwStyleCount, const LPDWORD lpdwStyle )
{
	// Attach the pen.
	Attach( ::ExtCreatePen( dwStyle, dwWidth, pLogBrush, dwStyleCount, lpdwStyle ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Get the LOGPEN structure.
BOOL ClsPen::GetLogPen( LOGPEN *pLogPen ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.
	_ASSERT_VALID( pLogPen ); // Must be valid.

	// Get the information.
	return GetObject( sizeof( LOGPEN ), ( LPVOID )pLogPen ) == sizeof( LOGPEN ) ? TRUE : FALSE;
}

// Get the LOGPENEX structure.
BOOL ClsPen::GetExtLogPen( EXTLOGPEN *pLogPen ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.
	_ASSERT_VALID( pLogPen ); // Must be valid.

	// Get the information.
	return GetObject( sizeof( EXTLOGPEN ), ( LPVOID )pLogPen ) == sizeof( EXTLOGPEN ) ? TRUE : FALSE;
}

// Get a stock pen.
BOOL ClsPen::GetStockPen( int nObject )
{
	// Get the stock pen.
	HPEN	hPen = ( HPEN )GetStockObject( nObject );
	
	// Make sure it is a pen.
	if ( hPen && ::GetObjectType( hPen ) != OBJ_PEN && ::GetObjectType( hPen ) != OBJ_EXTPEN )
		return FALSE;

	// Attach the object.
	Attach( hPen );
	return TRUE;
}

// Return the object of the handle.
ClsPen *ClsPen::FromHandle( HPEN hPen )
{
	return ( ClsPen * )ClsGdiObject::FromHandle( hPen );
}

// Operator to return the pen handle.
ClsPen::operator HPEN() const
{
	// Return the handle.
	return ( HPEN )m_hGdiObject;
}

// Assignment operator.
BOOL ClsPen::operator=( const ClsPen& cPen )
{
	// Can't assign to ourself.
	if ( &cPen == this )
		return TRUE;

	// Copy the font.
	return operator=(( HPEN )cPen.m_hGdiObject );
}

// Assignment operator.
BOOL ClsPen::operator=( HPEN hPen )
{
	// Get pen information.
	LOGPEN		lp;

	if ( //::GetObjectType( hPen ) == OBJ_PEN &&
	     ::GetObject( hPen, sizeof( LOGPEN ), ( void * )&lp ))
		// Create the object.
		return CreatePenIndirect( &lp );

	return FALSE;
}