//
// dc.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "dc.h"
#include "brush.h"
#include "pen.h"
#include "selector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Global list of device context object.
ClsLinkedList<ClsDC>	global_dc_list;
ClsLinkedList<ClsDC>	temporary_dc_list;

// Count the created DC's.
#ifdef _DEBUG
int nDCObjects = 0;
#endif

// Static dc functions pointers. These are OS dependant...
typedef BOOL ( CALLBACK *GRADIENTFILL )( HDC, CONST PTRIVERTEX, DWORD, CONST PVOID, DWORD, DWORD );

static GRADIENTFILL StaticGradientFill = NULL;

// Finds a device context object in the list by it's handle
// value.
static ClsDC *ClsFindObjectByHandle( ClsLinkedList<ClsDC>& list, HDC hDC )
{
	_ASSERT_VALID( hDC ); // This must be valid.

	// Iterate the nodes.
	for ( ClsDC *pDC = list.GetFirst(); pDC; pDC = list.GetNext( pDC ))
	{
		// Is the handle wrapped by this object
		// the one we are looking for?
		if ( *pDC == hDC )
			// Yes. Return a pointer to the object.
			return pDC;
	}
	// Object not in the list.
	return NULL;
}

// Constructor.
ClsDC::ClsDC()
{
	// Clear handle.
	m_hDC = NULL;

	// Add us to the global list.
	global_dc_list.AddHead( this );
}

// Constructor. Initializes to a handle.
ClsDC::ClsDC( HDC hDC )
{
	_ASSERT_VALID( hDC ); // Must be valid.

	// Clear handle.
	m_hDC = NULL;

	// Attach the handle.
	Attach( hDC );

	// Add us to the global list.
	global_dc_list.AddHead( this );
}

// Destructor.
ClsDC::~ClsDC()
{
	// The destructor does nothing so
	// you are responsible for calling
	// DeleteDC() or ReleaseDC().
	//_ASSERT( m_hDC == NULL );

	// Remove us from the list if we are still
	// in there.
	ClsDC *pDC;
	for ( pDC = global_dc_list.GetFirst(); pDC; pDC = global_dc_list.GetNext( pDC ))
	{
		// Is this us?
		if ( pDC == this )
		{
			// Yes. Remove us from the list.
			global_dc_list.Remove( pDC );
			break;
		}
	}
}

// Detach the handle.
HDC ClsDC::Detach()
{
	_ASSERT_VALID( m_hDC );

	// Save handle.
	HDC hDC = m_hDC;

	// Set it to NULL.
	m_hDC = NULL;

	// Return the handle.
	return hDC;
}

// Attach a handle to this object. Only works
// if the object is empty.
BOOL ClsDC::Attach( HDC hDC )
{
	_ASSERT( m_hDC == NULL ); // The object must be empty.
	_ASSERT_VALID( hDC );

	// Can we attach the handle?
	if ( m_hDC == NULL )
	{
		// Attach it.
		m_hDC = hDC;
		return TRUE;
	}
	return FALSE;
}

BOOL ClsDC::CreateDC( LPCTSTR lpszDriverName, LPCTSTR lpszDeviceName, LPCTSTR lpszOutput, const DEVMODE *lpInitData )
{
	_ASSERT( m_hDC == NULL ); // The object must be empty.

	// Can we create the handle?
	if ( m_hDC == NULL )
	{
		// Attach the handle.
		if ( Attach( ::CreateDC( lpszDriverName, lpszDeviceName, lpszOutput, lpInitData )))
		{
			#ifdef _DEBUG
			nDCObjects++;
			#endif
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ClsDC::CreateIC( LPCTSTR lpszDriverName, LPCTSTR lpszDeviceName, LPCTSTR lpszOutput, const DEVMODE *lpInitData )
{
	_ASSERT( m_hDC == NULL ); // The object must be empty.

	// Can we create the handle?
	if ( m_hDC == NULL )
	{
		// Attach the handle.
		if ( Attach( ::CreateIC( lpszDriverName, lpszDeviceName, lpszOutput, lpInitData )))
		{
			#ifdef _DEBUG
			nDCObjects++;
			#endif
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ClsDC::CreateCompatibleDC( ClsDC *pDC )
{
	_ASSERT( m_hDC == NULL ); // The object must be empty.

	// Can we create the handle?
	if ( m_hDC == NULL )
	{
		// Attach the handle.
		if ( Attach( ::CreateCompatibleDC( pDC ? pDC->m_hDC : NULL )))
		{
			#ifdef _DEBUG
			nDCObjects++;
			#endif
			return TRUE;
		}
	}
	return FALSE;
}

BOOL ClsDC::DeleteDC()
{
	_ASSERT_VALID( m_hDC ); // Object must be valid.

	// Delete the device context.
	BOOL bRC = ::DeleteDC( m_hDC );

	// OK?
	if ( bRC )
	{
		#ifdef _DEBUG
		nDCObjects--;
		#endif
		// Clear handle.
		m_hDC = NULL;
	}

	// Return error flag.
	return bRC;
}

ClsDC *ClsDC::FromHandle( HDC hDC )
{
	ClsDC	*pDC;

	// Valid?
	if ( hDC == NULL )
		return NULL;

	// Do we have it in the global list?
	if (( pDC = ClsFindObjectByHandle( global_dc_list, hDC )) == NULL )
	{
		// No, see if we can find it in the temporary list.
		if (( pDC = ClsFindObjectByHandle( temporary_dc_list, hDC )) == NULL )
		{
			// No. Create a temporary new object
			// and add it to the temporary list.
			pDC = new ClsDC( hDC );
			global_dc_list.Remove( pDC );
			temporary_dc_list.AddHead( pDC );
		}
	}
	return pDC;
}

void ClsDC::DeleteTempObjects()
{
	// Iterate temporary objects deleting
	// them.
	ClsDC *pDC;
	while (( pDC = temporary_dc_list.RemoveHead()) != NULL )
	{
		// Detach the handle from the temporary object.
		pDC->Detach();
		delete pDC;
	}
}

ClsBitmap *ClsDC::GetCurrentBitmap() const
{
	_ASSERT_VALID( m_hDC ); // Must be valid.

	// Get the device context bitmap.
	HBITMAP	hBitmap = ( HBITMAP )::GetCurrentObject( m_hDC, OBJ_BITMAP );

	// If it is valid we create a temporary object of
	// it.
	return ClsBitmap::FromHandle( hBitmap );
}

ClsBrush *ClsDC::GetCurrentBrush() const
{
	_ASSERT_VALID( m_hDC ); // Must be valid.

	// Get the device context brush.
	HBRUSH	hBrush = ( HBRUSH )::GetCurrentObject( m_hDC, OBJ_BRUSH );

	// If it is valid we create a temporary object of
	// it.
	return ClsBrush::FromHandle( hBrush );
}

ClsFont *ClsDC::GetCurrentFont() const
{
	_ASSERT_VALID( m_hDC ); // Must be valid.

	// Get the device context font.
	HFONT	hFont = ( HFONT )::GetCurrentObject( m_hDC, OBJ_FONT );

	// If it is valid we create a temporary object of
	// it.
	return ClsFont::FromHandle( hFont );
}

ClsPalette *ClsDC::GetCurrentPalette() const
{
	_ASSERT_VALID( m_hDC ); // Must be valid.

	// Get the device context palette.
	HPALETTE hPalette = ( HPALETTE )::GetCurrentObject( m_hDC, OBJ_PAL );

	// If it is valid we create a temporary object of
	// it.
	return ClsPalette::FromHandle( hPalette );
}

ClsPen *ClsDC::GetCurrentPen() const
{
	_ASSERT_VALID( m_hDC ); // Must be valid.

	// Get the device context pen.
	HPEN hPen = ( HPEN )::GetCurrentObject( m_hDC, OBJ_PEN );

	// If it is valid we create a temporary object of
	// it.
	return ClsPen::FromHandle( hPen );
}

ClsPalette *ClsDC::SelectPalette( ClsPalette *pPalette, BOOL bForceBackground )
{
	_ASSERT_VALID( m_hDC ); // Must be valid.

	// Select the palette.
	HPALETTE hPal = ::SelectPalette( m_hDC, *pPalette, bForceBackground );

	// OK?
	if ( hPal )
		// Create temp object.
		return ClsPalette::FromHandle( hPal );
	return NULL;
}

// GradientFill() API.
BOOL ClsDC::GradientFill( TRIVERTEX* pVertices, ULONG nVertices, void* pMesh, ULONG nMeshElements,  DWORD dwMode )
{
	_ASSERT_VALID( m_hDC ); // Must be valid.

	// Function known?
	if ( StaticGradientFill )
		return ( *StaticGradientFill )( m_hDC, pVertices, nVertices, pMesh, nMeshElements, dwMode );
	
	// Get the procedure address.
	StaticGradientFill = ( GRADIENTFILL )GetProcAddress( GetModuleHandle( _T( "msimg32.dll" )), "GradientFill" );
	if ( StaticGradientFill )
		return ( *StaticGradientFill )( m_hDC, pVertices, nVertices, pMesh, nMeshElements, dwMode );
	return FALSE;
}

// Render an outlined rectangle.
void ClsDC::OutlinedRectangle( LPCRECT pRect, COLORREF crOuter, COLORREF crInner )
{
	_ASSERT_VALID( m_hDC );

	// Create GDI objects.
	ClsBrush inner( crInner );
	ClsPen outer( PS_SOLID, 1, crOuter );

	// Select them into the DC.
	ClsSelector bsel( this, inner );
	ClsSelector psel( this, outer );

	// Render rectangle.
	Rectangle( pRect );
}

// Fill a solid rectangle.
void ClsDC::FillSolidRect( LPCRECT lpRect, COLORREF crColor )
{
	ClsBrush brush( crColor );
	if ( brush.IsValid()) FillRect( lpRect, brush );
}

void ClsDC::FillSolidRect( int x, int y, int cx, int cy, COLORREF crColor )
{
	ClsBrush brush( crColor );
	if ( brush.IsValid()) FillRect( ClsRect( x, y, x + cx, y + cy ), brush );
}