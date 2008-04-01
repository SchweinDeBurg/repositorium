//
// palette.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "palette.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor.
ClsPalette::ClsPalette()
{
}

// Destructor. Does nothing.
ClsPalette::~ClsPalette()
{
}

// Create palette.
BOOL ClsPalette::CreatePalette( const LOGPALETTE *pLogPalette )
{
	// Attach the palette.
	Attach( ::CreatePalette( pLogPalette ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create half-tone palette.
BOOL ClsPalette::CreateHalftonePalette( HDC hDC )
{
	// Attach the palette.
	Attach( ::CreateHalftonePalette( hDC ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Animate palette.
void ClsPalette::AnimatePalette( UINT nStartIndex, UINT nNumEntries, LPPALETTEENTRY pPaletteColors )
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	// Animate.
	::AnimatePalette(( HPALETTE )m_hGdiObject, nStartIndex, nNumEntries, pPaletteColors );
}

// Get palette index.
UINT ClsPalette::GetNearestPaletteIndex( COLORREF crColor ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	// Get index.
	return ::GetNearestPaletteIndex(( HPALETTE )m_hGdiObject, crColor );
}

// Resize the palette.
BOOL ClsPalette::ResizePalette( UINT nNumEntries )
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	// Resize...
	return ::ResizePalette(( HPALETTE )m_hGdiObject, nNumEntries );
}

// Realize the palette.
UINT ClsPalette::RealizePalette( HDC hDC )
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	// Realize...
	return ::RealizePalette( hDC );
}

// Retrieve palette entries.
UINT ClsPalette::GetPaletteEntries( UINT nStartIndex, UINT nNumEntries, LPPALETTEENTRY pPaletteColors ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	// Get the entries.
	return ::GetPaletteEntries(( HPALETTE )m_hGdiObject, nStartIndex, nNumEntries, pPaletteColors );
}

// Set palette entries.
UINT ClsPalette::SetPaletteEntries( UINT nStartIndex, UINT nNumEntries, LPPALETTEENTRY pPaletteColors )
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	// Set the entries.
	return ::SetPaletteEntries(( HPALETTE )m_hGdiObject, nStartIndex, nNumEntries, pPaletteColors );
}

// Get the entry count of the palette.
WORD ClsPalette::GetEntryCount() const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.
	WORD	wCount = 0;

	// Get the object count.
	GetObject( sizeof( WORD ), ( LPVOID )&wCount );
	return wCount;
}

// Get a stock palette.
BOOL ClsPalette::GetStockPalette( int nObject )
{
	// Get the stock palette.
	HPALETTE	hPalette = ( HPALETTE )GetStockObject( nObject );
	
	// Make sure it is a palette.
	if ( hPalette && ::GetObjectType( hPalette ) != OBJ_PAL )
		return FALSE;

	// Attach the object.
	Attach( hPalette );
	return TRUE;
}

// Return the object of the handle.
ClsPalette *ClsPalette::FromHandle( HPALETTE hPalette )
{
	return ( ClsPalette * )ClsGdiObject::FromHandle( hPalette );
}

// Operator to return the palette handle.
ClsPalette::operator HPALETTE() const
{
	// Return the handle.
	return ( HPALETTE )m_hGdiObject;
}
