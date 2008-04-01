#ifndef _PALETTE_H_
#define _PALETTE_H_
//
// palette.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "gdiobject.h"

// A wrapper class for the HPALETTE handle.
class ClsPalette : public ClsGdiObject
{
	// No copy constructor.
	_NO_COPY( ClsPalette );
public:
	// Constructor.
	ClsPalette();

	// Destructor.
	virtual ~ClsPalette();

	// Interface.
	BOOL CreatePalette( const LOGPALETTE *pLogPalette );
	BOOL CreateHalftonePalette( HDC hDC );
	void AnimatePalette( UINT nStartIndex, UINT nNumEntries, LPPALETTEENTRY pPaletteColors );
	UINT GetNearestPaletteIndex( COLORREF crColor ) const;
	BOOL ResizePalette( UINT nNumEntries );
	UINT RealizePalette( HDC hDC );
	UINT GetPaletteEntries( UINT nStartIndex, UINT nNumEntries, LPPALETTEENTRY pPaletteColors ) const;
	UINT SetPaletteEntries( UINT nStartIndex, UINT nNumEntries, LPPALETTEENTRY pPaletteColors );
	WORD GetEntryCount() const;
	BOOL GetStockPalette( int nObject );
	static ClsPalette *FromHandle( HPALETTE hPalette );

	// Operator overloads.
	operator HPALETTE() const;
};

#endif // _PALETTE_H_