//
// brush.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "brush.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor. Initializes to NULL.
ClsBrush::ClsBrush()
{
}

// Constructor. Copy constructor.
ClsBrush::ClsBrush( const ClsBrush& cBrush )
{
	// Copy the brush.
	operator=(( HBRUSH )cBrush.m_hGdiObject );
}

// Constructor. Copy from a brush.
ClsBrush::ClsBrush( HBRUSH hBrush )
{
	// Copy the brush.
	operator=( hBrush );
}

// Constructor. Creates a solid brush.
ClsBrush::ClsBrush( COLORREF crColor )
{
	// Create the brush.
	Attach( ::CreateSolidBrush( crColor ));
}

// Constructor. Creates a hatch brush.
ClsBrush::ClsBrush( int nHatch, COLORREF crColor )
{
	// Create the hatch brush.
	Attach( ::CreateHatchBrush( nHatch, crColor ));
}

// Constructor. Creates a bitmap brush.
ClsBrush::ClsBrush( HBITMAP hBitmap )
{
	// Create a bitmap brush.
	Attach( ::CreatePatternBrush( hBitmap ));
}

// Destructor. Does nothing.
ClsBrush::~ClsBrush()
{
}

// Create a solid brush.
BOOL ClsBrush::CreateSolidBrush( COLORREF crColor )
{
	// Attach the solid brush.
	Attach( ::CreateSolidBrush( crColor ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create a hatch brush.
BOOL ClsBrush::CreateHatchBrush( int nStyle, COLORREF crColor )
{
	// Attach the solid brush.
	Attach( ::CreateHatchBrush( nStyle, crColor ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create a pattern brush.
BOOL ClsBrush::CreatePatternBrush( HBITMAP hBitmap )
{
	// Attach the pattern brush.
	Attach( ::CreatePatternBrush( hBitmap ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create a brush using a LOGBRUSH structure.
BOOL ClsBrush::CreateBrushIndirect( const LOGBRUSH *pLogBrush )
{
	// Attach the brush.
	Attach( ::CreateBrushIndirect( pLogBrush ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create a DIB pattern brush.
BOOL ClsBrush::CreateDIBPatternBrush( LPVOID pPackedDIB, UINT iUsage )
{
	// Attach the DIB brush.
	Attach( ::CreateDIBPatternBrush( pPackedDIB, iUsage ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create a system color brush.
BOOL ClsBrush::CreateSysColorBrush( int nColor )
{
	// Attach the system color brush.
	return CreateSolidBrush( ::GetSysColor( nColor ));
}

// Get the LOGBRUSH structure.
BOOL ClsBrush::GetLogBrush( LOGBRUSH *pLogBrush ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.
	_ASSERT_VALID( pLogBrush ); // Must be valid.

	// Get the information/
	return GetObject( sizeof( LOGBRUSH ), ( LPVOID )pLogBrush ) == sizeof( LOGBRUSH ) ? TRUE : FALSE;
}

// Get a stock brush.
BOOL ClsBrush::GetStockBrush( int nObject )
{
	// Get the stock brush.
	HBRUSH	hBrush = ( HBRUSH )GetStockObject( nObject );
	
	// Make sure it is a brush.
	if ( hBrush && ::GetObjectType( hBrush ) != OBJ_BRUSH )
		return FALSE;

	// Attach the object.
	Attach( hBrush );
	return TRUE;
}

// Return the object of the handle.
ClsBrush *ClsBrush::FromHandle( HBRUSH hBrush )
{
	return ( ClsBrush * )ClsGdiObject::FromHandle( hBrush );
}

// Operator to return the brush handle.
ClsBrush::operator HBRUSH() const
{
	// Return the handle.
	return ( HBRUSH )m_hGdiObject;
}

// Assignment operator.
BOOL ClsBrush::operator=( const ClsBrush& cBrush )
{
	// Can't assign to ourself.
	if ( &cBrush == this )
		return TRUE;

	// Copy the brush.
	return operator=(( HBRUSH )cBrush.m_hGdiObject );
}

// Assignment operator.
BOOL ClsBrush::operator=( HBRUSH hBrush )
{
	// Get brush information.
	LOGBRUSH	lb;

	if ( //::GetObjectType( hBrush ) == OBJ_BRUSH &&
	     ::GetObject( hBrush, sizeof( LOGBRUSH ), ( void * )&lb ))
	     // Create the object.
	     return CreateBrushIndirect( &lb );

	return FALSE;
}