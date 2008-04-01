#ifndef _POINT_H_
#define _POINT_H_
//
// point.h
//
// (C) Copyright 2000 Jan van den Baard
//     All Rights Reserved.
//

#include "../standard.h"

// This class is a wrapper for the POINT structure.
class ClsPoint
{
public:
	// Initializes the ClsPoint with the passed coordinates
	// or 0.
	ClsPoint( LONG x = 0, LONG y = 0 )
	{ m_Point.x = x; m_Point.y = y; }

	// Initializes the ClsPoint with the coordinates
	// from the POINT structure.
	ClsPoint( const POINT& pt )
	{ m_Point = pt; }

	// Virtual destructor, does nothing.
	virtual ~ClsPoint()
	{}

	// Compare two points.
	inline BOOL IsEqual( const POINT& pt ) const
	{ return ( pt.x == m_Point.x && pt.y == m_Point.y ); }

	// Returns TRUE if the coordinates are 0,0
	inline BOOL IsEmpty() const
	{ return ( m_Point.x == 0 && m_Point.y == 0 ) ? TRUE : FALSE; }

	// Empty the point.
	inline void Empty()
	{ m_Point.x = m_Point.y = 0; }

	// Adds the passed coordinates to the existing coordinates.
	inline void Add( LONG x, LONG y )
	{ m_Point.x += x; m_Point.y += y; }
	inline void Add( const POINT& pt )
	{ m_Point.x += pt.x; m_Point.y += pt.y; }
	inline void AddX( LONG x )
	{ m_Point.x += x; }
	inline void AddY( LONG y )
	{ m_Point.y += y; }

	// Subtracts the passed coordinates from the existing coordinates.
	inline void Subtract( LONG x, LONG y )
	{ m_Point.x -= x; m_Point.y -= y; }
	inline void Subtract( const POINT& pt )
	{ m_Point.x -= pt.x; m_Point.y -= pt.y; }
	inline void SubtractX( LONG x )
	{ m_Point.x -= x; }
	inline void SubtractY( LONG y )
	{ m_Point.y -= y; }

	// Return the x or y coordinate.
	inline LONG& X() const
	{ return ( LONG& )m_Point.x; }
	inline LONG& Y() const
	{ return ( LONG& )m_Point.y; }

	// Conversion operator overloads.
	inline operator POINT*()
	{ return &m_Point; }
	inline operator POINT() const
	{ return m_Point; }

	// Assignment operator overload.
	inline void operator=( const POINT& pt )
	{ m_Point = pt; }
	inline void operator=( LPPOINT pt )
	{ m_Point = *pt; }

	// Comparisson operator overloads.
	inline BOOL operator==( const POINT& pt ) const
	{ return IsEqual( pt ); }
	inline BOOL operator!=( const POINT& pt ) const
	{ return !IsEqual( pt ); }

	// Addition operator overloads.
	inline void operator+=( const POINT& pt )
	{ Add( pt ); }
	inline ClsPoint operator+( const POINT& pt ) const
	{ ClsPoint ptTmp( *this ); ptTmp += pt; return ptTmp; }

	// Subtraction operator overloads.
	inline void operator-=( const POINT& pt )
	{ Subtract( pt ); }
	inline ClsPoint operator-( const POINT& pt ) const
	{ ClsPoint ptTmp( *this ); ptTmp -= pt; return ptTmp; }

protected:
	POINT	m_Point;
};
#endif // _POINT_H_