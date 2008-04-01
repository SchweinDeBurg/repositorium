#ifndef _RGN_H_
#define _RGN_H_
//
// rgn.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "gdiobject.h"

// A wrapper class for the HRGN handle.
class ClsRgn : public ClsGdiObject
{
	_NO_COPY( ClsRgn );
public:
	// Constructor.
	ClsRgn();

	// Destructor.
	virtual ~ClsRgn();
	
	// Interface.
	BOOL CreateRectRgn( int x1, int y1, int x2, int y2 );
	BOOL CreateRectRgn( LPCRECT pRect );
	BOOL CreateEllipticRgn( int x1, int y1, int x2, int y2 );
	BOOL CreateEllipticRgn( LPCRECT pRect );
	BOOL CreatePolygonRgn( LPPOINT lpPoints, int nCount, int nMode );
	BOOL CreatePolyPolygonRgn( LPPOINT lpPoints, LPINT lpPolyCounts, int nCount, int nPolyFillMode );
	BOOL CreateRoundRectRgn( int x1, int y1, int x2, int y2, int x3, int y3 );
	BOOL CreateFromData( const XFORM *lpXForm, int nCount, const RGNDATA *pRgnData );
	int CombineRgn( ClsRgn *pRgn1, ClsRgn *pRgn2, int nCombineMode );
	int CopyRgn( ClsRgn *pRgnSrc );
	BOOL EqualRgn( ClsRgn *pRgn ) const;
	int GetRegionData( LPRGNDATA lpRgnData, int nCount ) const;
	int GetRgnBox( LPRECT lpRect ) const;
	int OffsetRgn( int x, int y );
	int OffsetRgn( POINT point );
	BOOL PtInRegion( int x, int y ) const;
	BOOL PtInRegion( POINT point ) const;
	BOOL RectInRegion( LPCRECT lpRect ) const;
	void SetRectRgn( int x1, int y1, int x2, int y2 );
	void SetRectRgn( LPCRECT lpRect );
	static ClsRgn *FromHandle( HRGN hRgn );

	// Operator overloads.
	operator HRGN() const;
};

#endif // _RGN_H_
