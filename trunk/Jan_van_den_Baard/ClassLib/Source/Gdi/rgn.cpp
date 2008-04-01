//
// rgn.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "rgn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor.
ClsRgn::ClsRgn()
{
}

// Destructor.
ClsRgn::~ClsRgn()
{
}

// Creates a rectangle region.
BOOL ClsRgn::CreateRectRgn( int x1, int y1, int x2, int y2 )
{
	// Attach the object.
	Attach( ::CreateRectRgn( x1, y1, x2, y2 ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Creates a rectangle region.
BOOL ClsRgn::CreateRectRgn( LPCRECT pRect )
{
	// Attach the object.
	Attach( ::CreateRectRgnIndirect( pRect ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Creates an elliptic region.
BOOL ClsRgn::CreateEllipticRgn( int x1, int y1, int x2, int y2 )
{
	// Attach the object.
	Attach( ::CreateEllipticRgn( x1, y1, x2, y2 ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Creates an elliptic region.
BOOL ClsRgn::CreateEllipticRgn( LPCRECT pRect )
{
	// Attach the object.
	Attach( ::CreateEllipticRgnIndirect( pRect ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Creates a polygon region.
BOOL ClsRgn::CreatePolygonRgn( LPPOINT lpPoints, int nCount, int nMode )
{
	// Attach the object.
	Attach( ::CreatePolygonRgn( lpPoints, nCount, nMode ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Creates a polygon region.
BOOL ClsRgn::CreatePolyPolygonRgn( LPPOINT lpPoints, LPINT lpPolyCounts, int nCount, int nPolyFillMode )
{
	// Attach the object.
	Attach( ::CreatePolyPolygonRgn( lpPoints, lpPolyCounts, nCount, nPolyFillMode ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Creates a rounded rectangle region.
BOOL ClsRgn::CreateRoundRectRgn( int x1, int y1, int x2, int y2, int x3, int y3 )
{
	// Attach the object.
	Attach( ::CreateRoundRectRgn( x1, y1, x2, y2, x3, y3 ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create a region from the data.
BOOL ClsRgn::CreateFromData( const XFORM *lpXForm, int nCount, const RGNDATA *lpRgnData )
{
	// Attach the object.
	Attach( ::ExtCreateRegion( lpXForm, nCount, lpRgnData ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create a region which is a combination of
// two other regions.
int ClsRgn::CombineRgn( ClsRgn *pRgn1, ClsRgn *pRgn2, int nCombineMode )
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	// Combine the regions.
	return ::CombineRgn(( HRGN )m_hGdiObject, *pRgn1, *pRgn2, nCombineMode );
}

// Copy a region into this region.
int ClsRgn::CopyRgn( ClsRgn *pRgnSrc )
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	// Copy the region.
	return ::CombineRgn(( HRGN )m_hGdiObject, *pRgnSrc, NULL, RGN_COPY );
}

// Is the region equal to this one?
BOOL ClsRgn::EqualRgn( ClsRgn *pRgnSrc ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	return ::EqualRgn(( HRGN )m_hGdiObject, *pRgnSrc );
}

// Get region data.
int ClsRgn::GetRegionData( LPRGNDATA lpRgnData, int nCount ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	return ::GetRegionData(( HRGN )m_hGdiObject, nCount, lpRgnData );
}

// Get region box.
int ClsRgn::GetRgnBox( LPRECT pRect ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	return ::GetRgnBox(( HRGN )m_hGdiObject, pRect );
}

// Offset the region.
int ClsRgn::OffsetRgn( POINT point )
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	return ::OffsetRgn(( HRGN )m_hGdiObject, point.x, point.y );
}

// Offset the region.
int ClsRgn::OffsetRgn( int x, int y )
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	return ::OffsetRgn(( HRGN )m_hGdiObject, x, y );
}

// Is the point inside the region.
BOOL ClsRgn::PtInRegion( POINT point ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	return ::PtInRegion(( HRGN )m_hGdiObject, point.x, point.y );
}

// Is the point inside the region.
BOOL ClsRgn::PtInRegion( int x, int y ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	return ::PtInRegion(( HRGN )m_hGdiObject, x, y );
}

// Set the region to the rectangle.
void ClsRgn::SetRectRgn( int x1, int y1, int x2, int y2 )
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	::SetRectRgn(( HRGN )m_hGdiObject, x1, y1, x2, y2 );
}

// Set the region to the rectangle.
void ClsRgn::SetRectRgn( LPCRECT pRect )
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	::SetRectRgn(( HRGN )m_hGdiObject, pRect->left, pRect->top, pRect->right, pRect->bottom );
}

// Return the object of the handle.
ClsRgn *ClsRgn::FromHandle( HRGN hRgn )
{
	return ( ClsRgn * )ClsGdiObject::FromHandle( hRgn );
}

// Operator overload.
ClsRgn::operator HRGN() const
{
	return ( HRGN )m_hGdiObject;
}
