#ifndef _PEN_H_
#define _PEN_H_
//
// pen.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "gdiobject.h"

// A wrapper class for the HPEN handle.
class ClsPen : public ClsGdiObject
{
public:
	// Constructors.
	ClsPen();
	ClsPen( const ClsPen& cPen );
	ClsPen( HPEN hPen );
	ClsPen( DWORD dwStyle, DWORD dwWidth, const LOGBRUSH *pLogBrush, DWORD dwStyleCount, const LPDWORD lpdwStyle );
	ClsPen( int nStyle, int nWidth, COLORREF crColor );
	ClsPen( const LOGPEN *pLogPen );

	// Destructor.
	virtual ~ClsPen();
	
	// Interface.
	BOOL CreatePen( int nStyle, int nWidth, COLORREF crColor );
	BOOL CreatePenIndirect( const LOGPEN *pLogPen );
	BOOL ExtCreatePen( DWORD dwStyle, DWORD dwWidth, const LOGBRUSH *pLogBrush, DWORD dwStyleCount, const LPDWORD lpdwStyle );
	BOOL GetLogPen( LOGPEN *pLogPen ) const;
	BOOL GetExtLogPen( EXTLOGPEN *pLogPen ) const;
	BOOL GetStockPen( int nObject );
	static ClsPen *FromHandle( HPEN hPen );

	// Operator overloads.
	operator HPEN() const;
	operator=( const ClsPen& cPen );
	operator=( HPEN hPen );
};

#endif // _PEN_H_
