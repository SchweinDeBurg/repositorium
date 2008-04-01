#ifndef _SIZE_H_
#define _SIZE_H_
//
// size.h
//
// (C) Copyright 2000 Jan van den Baard
//     All Rights Reserved.
//

#include "../standard.h"

// This class is a wrapper for the SIZE structure.
class ClsSize
{
public:
	// Initializes the ClsSize with the passed coordinates
	// or 0.
	ClsSize( int cx = 0, int cy = 0 )
	{ m_Size.cx = cx; m_Size.cy = cy; }

	// Initializes the ClsSize with the coordinates
	// from the SIZE structure.
	ClsSize( const SIZE& sz )
	{ m_Size = sz; }

	// Virtual destructor, does nothing.
	virtual ~ClsSize()
	{}

	// Compare two sizes.
	inline BOOL IsEqual( const SIZE& sz ) const
	{ return ( sz.cx == m_Size.cx && sz.cy == m_Size.cy ); }

	// Adds the passed size to the existing size.
	inline void Add( int cx, int cy )
	{ m_Size.cx += cx; m_Size.cy += cy; }
	inline void Add( const SIZE& sz )
	{ m_Size.cx += sz.cx; m_Size.cy += sz.cy; }
	inline void AddCX( int cx )
	{ m_Size.cx += cx; }
	inline void AddCY( int cy )
	{ m_Size.cy += cy; }

	// Subtracts the passed size from the existing size.
	inline void Subtract( int cx, int cy )
	{ m_Size.cx -= cx; m_Size.cy -= cy; }
	inline void Subtract( const SIZE& sz )
	{ m_Size.cx -= sz.cx; m_Size.cy -= sz.cy; }
	inline void SubtractCX( int cx )
	{ m_Size.cx -= cx; }
	inline void SubtractCY( int cy )
	{ m_Size.cy -= cy; }

	// Return the cx or cy value.
	inline int& CX() const
	{ return ( int& )m_Size.cx; }
	inline int& CY() const
	{ return ( int& )m_Size.cy; }

	// Conversion operator overloads.
	inline operator SIZE*()
	{ return &m_Size; }
	inline operator SIZE() const
	{ return m_Size; }

	// Assignment operator overload.
	inline void operator=( const SIZE& sz )
	{ m_Size = sz; }
	inline void operator=( LPSIZE sz )
	{ m_Size = *sz; }

	// Comparisson operator overloads.
	inline BOOL operator==( const SIZE& sz ) const
	{ return IsEqual( sz ); }
	inline BOOL operator!=( const SIZE& sz ) const
	{ return !IsEqual( sz ); }

	// Addition operator overloads.
	inline void operator+=( const SIZE& sz )
	{ Add( sz ); }
	inline ClsSize operator+( const SIZE& sz ) const
	{ ClsSize szTmp( *this ); szTmp += sz; return szTmp; }

	// Subtraction operator overloads.
	inline void operator-=( const SIZE& sz )
	{ Subtract( sz ); }
	inline ClsSize operator-( const SIZE& sz ) const
	{ ClsSize szTmp( *this ); szTmp -= sz; return szTmp; }

protected:
	SIZE	m_Size;
};
#endif _SIZE_H_