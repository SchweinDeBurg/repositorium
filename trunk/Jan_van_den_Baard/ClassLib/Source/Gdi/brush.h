#ifndef _BRUSH_H_
#define _BRUSH_H_
//
// brush.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "gdiobject.h"

// A wrapper class for the HBRUSH handle.
class ClsBrush : public ClsGdiObject
{
public:
	// Constructors.
	ClsBrush();
	ClsBrush( const ClsBrush& cBrush );
	ClsBrush( HBRUSH hBrush );
	ClsBrush( COLORREF crColor );
	ClsBrush( int nHatchStyle, COLORREF crColor );
	ClsBrush( HBITMAP hBitmap );

	// Destructor.
	virtual ~ClsBrush();
	
	// Interface.
	BOOL CreateSolidBrush( COLORREF crColor );
	BOOL CreateHatchBrush( int nHatchStyle, COLORREF crColor );
	BOOL CreatePatternBrush( HBITMAP hBitmap );
	BOOL CreateBrushIndirect( const LOGBRUSH *pLogBrush );
	BOOL CreateDIBPatternBrush( LPVOID pPackedDIB, UINT iUsage );
	BOOL CreateSysColorBrush( int nColor );
	BOOL GetLogBrush( LOGBRUSH *pLogBrush ) const;
	BOOL GetStockBrush( int nObject );
	static ClsBrush *FromHandle( HBRUSH hBrush );

	// Operator overloads.
	operator HBRUSH() const;
	BOOL operator=( const ClsBrush& cBrush );
	BOOL operator=( HBRUSH hBrush );
};

#endif // _BRUSH_H_