#ifndef _RECT_H_
#define _RECT_H_
//
// rect.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "size.h"
#include "point.h"

// This class is a wrapper for the RECT structure.
class ClsRect
{
public:
	// Initializes the ClsRect with the passed values or 0.
	ClsRect( LONG left = 0, LONG top = 0, LONG right = 0, LONG bottom = 0 )
	{
		m_Rect.left   = left;
		m_Rect.top    = top;
		m_Rect.right  = right;
		m_Rect.bottom = bottom;
	}

	// Initialize the ClsRect with the values from the RECT structure.
	ClsRect( const RECT& rc )
	{ m_Rect = rc; }

	// Virtual destructor, does nothing.
	virtual ~ClsRect()
	{}

	// Copy the rectangle.
	void CopyRect( LPCRECT pRect )
	{ m_Rect = *pRect; }

	// Normalize the rectangle. This will make sure that the
	// top/left coordinates are less than the bottom/right coordinates.
	void Normalize()
	{
		// Swap left and right values if necessary.
		if ( m_Rect.left > m_Rect.right )
		{
			LONG	temp = m_Rect.left;
			m_Rect.left  = m_Rect.right;
			m_Rect.right = temp;
		}
		// Swap top and bottom values if necessary.
		if ( m_Rect.top > m_Rect.bottom )
		{
			LONG	temp  = m_Rect.top;
			m_Rect.top    = m_Rect.bottom;
			m_Rect.bottom = temp;
		}
	}

	// Get/Set the left coordinate.
	inline LONG& Left() const
	{ return ( LONG& )m_Rect.left; }

	// Get/Set the right coordinate.
	inline LONG& Right() const
	{ return ( LONG& )m_Rect.right; }

	// Get/Set the top coordinate.
	inline LONG& Top() const
	{ return ( LONG & )m_Rect.top; }

	// Get/Set the bottom coordinate.
	inline LONG& Bottom() const
	{ return ( LONG& )m_Rect.bottom; }

	// Return the width of the rectangle.
	inline LONG Width() const
	{ return m_Rect.right - m_Rect.left; }

	// Return the height of the rectangle.
	inline LONG Height() const
	{ return m_Rect.bottom - m_Rect.top; }

	// Return the size of the rectangle.
	inline ClsSize Size() const
	{ return ClsSize( Width(), Height()); }

	// Return the top/left of the rectangle.
	inline ClsPoint TopLeft() const
	{ return ClsPoint( m_Rect.left, m_Rect.top ); }

	// Return the bottom right of the rectangle.
	inline ClsPoint BottomRight() const
	{ return ClsPoint( m_Rect.right, m_Rect.bottom ); }

	// Return the center of the rectangle.
	inline ClsPoint CenterPoint() const
	{ return ClsPoint( (( m_Rect.left + m_Rect.right ) / 2 ), (( m_Rect.top + m_Rect.bottom ) / 2 )); }

	// Determine wether the width or height of the
	// rectangle <= 0.
	inline BOOL IsEmpty() const
	{ return ::IsRectEmpty( &m_Rect ) ? TRUE : FALSE; }

	// Determine wether or not all values in the
	// rectangle are 0.
	inline BOOL IsNull() const
	{ return ( m_Rect.left == 0 && m_Rect.top == 0 && m_Rect.right == 0 && m_Rect.bottom == 0 ) ? TRUE : FALSE; }

	// Determine wether the two rectangles match.
	inline BOOL IsEqual( const RECT& rc ) const
	{ return ::EqualRect( &m_Rect, &rc ) ? TRUE : FALSE; }

	// Determines wether the point is located inside the
	// rectangle area.
	inline BOOL PtInRect( const POINT& pt )
	{ return ::PtInRect( &m_Rect, pt ) ? TRUE : FALSE; }

	// Offset the rectangle.
	inline BOOL Offset( int dx, int dy )
	{ return ::OffsetRect( &m_Rect, dx, dy ) ? TRUE : FALSE; }
	inline BOOL Offset( const POINT& pt )
	{ return ::OffsetRect( &m_Rect, pt.x, pt.y ) ? TRUE : FALSE; }

	// Inflate the rectangle.
	inline BOOL Inflate( int dx, int dy )
	{ return ::InflateRect( &m_Rect, dx, dy ) ? TRUE : FALSE; }
	inline BOOL Inflate( const SIZE& sz )
	{ return ::InflateRect( &m_Rect, sz.cx, sz.cy ) ? TRUE : FALSE; }

	// Inflate the rectangle.
	inline BOOL Deflate( int dx, int dy )
	{ m_Rect.left += dx; m_Rect.top += dy; m_Rect.right -= dx; m_Rect.bottom -= dy; return TRUE; }
	inline BOOL Deflate( const SIZE& sz )
	{ m_Rect.left += sz.cx; m_Rect.top += sz.cy; m_Rect.right -= sz.cx; m_Rect.bottom -= sz.cy; return TRUE; }

	// Set this rectangle to represent the intersection
	// rectangle of two other rectangles.
	inline BOOL Intersect( const RECT& rc1, const RECT& rc2 )
	{ return ::IntersectRect( &m_Rect, &rc1, &rc2 ) ? TRUE : FALSE; }

	// Set this rectangle to represent the subtraction
	// rectangle of two other rectangles.
	inline BOOL Subtract( const RECT& rc1, const RECT& rc2 )
	{ return ::SubtractRect( &m_Rect, &rc1, &rc2 ) ? TRUE : FALSE; }

	// Set this rectangle to represent the union
	// rectangle of two other rectangles.
	inline BOOL Union( const RECT& rc1, const RECT& rc2 )
	{ return ::UnionRect( &m_Rect, &rc1, &rc2 ) ? TRUE : FALSE; }

	// Empty the rectangle.
	inline BOOL Empty()
	{ return ::SetRectEmpty( &m_Rect ) ? TRUE : FALSE; }

	// Conversion operator overloads.
	inline operator LPRECT()
	{ return &m_Rect; }
	inline operator RECT() const
	{ return m_Rect; }
	inline operator LPCRECT() const
	{ return &m_Rect; }

	// Assignment operator overload.
	inline void operator=( const RECT& rc )
	{ m_Rect = rc; }
	inline void operator=( LPRECT rc )
	{ m_Rect = *rc; }

	// Comparisson operator overloads.
	inline BOOL operator==( const RECT& rc ) const
	{ return IsEqual( rc ); }
	inline BOOL operator!=( const RECT& rc ) const
	{ return !IsEqual( rc ); }

protected:
	RECT	m_Rect;
};
#endif // _RECT_H_