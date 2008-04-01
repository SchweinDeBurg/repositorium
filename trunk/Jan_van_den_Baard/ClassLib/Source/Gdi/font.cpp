//
// font.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "font.h"
#include "getdc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor. Initializes to NULL.
ClsFont::ClsFont()
{
}

// Constructor. Copy constructor.
ClsFont::ClsFont( const ClsFont& cFont )
{
	// Copy the font.
	operator=(( HFONT )cFont.m_hGdiObject );
}

// Constructor. Copy from a handle.
ClsFont::ClsFont( HFONT hFont )
{
	// Copy the font.
	operator=( hFont );
}

// Constructor. Create from a LOGFONT.
ClsFont::ClsFont( const LOGFONT *pLogFont )
{
	CreateFontIndirect( pLogFont );
}

// Destructor. Does nothing.
ClsFont::~ClsFont()
{
}

// Create a font using a LOGFONT structure.
BOOL ClsFont::CreateFontIndirect( const LOGFONT *pLogFont )
{
	// Attach the font.
	Attach( ::CreateFontIndirect( pLogFont ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create a specified font.
BOOL ClsFont::CreateFont( int nHeight, int nWidth, int nEscapement, int nOrientation,
			  int fnWeight, DWORD fdwItalic, DWORD fdwUnderline, DWORD fdwStrikeOut, 
			  DWORD fdwCharSet, DWORD fdwOutputPrecision, DWORD fdwClipPrecision,
			  DWORD fdwQuality, DWORD fdwPitchAndFamily, LPCTSTR lpszFace )
{
	// Attach the font.
	Attach( ::CreateFont( nHeight, nWidth, nEscapement, nOrientation,
			      fnWeight, fdwItalic, fdwUnderline, fdwStrikeOut, 
			      fdwCharSet, fdwOutputPrecision, fdwClipPrecision,
			      fdwQuality, fdwPitchAndFamily, lpszFace ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create a specific font.
BOOL ClsFont::CreatePointFont( LPCTSTR pszTypeFace, UINT nHeight, DWORD dwFlags /* = CPFF_NORMAL */ )
{
	LOGFONT		lf = { 0 };
	ClsGetDC	dc( NULL );

	// Valid display device context?
	if ( dc.IsValid())
	{
		// Setup LOGFONT structure.
		lf.lfHeight		= -MulDiv( nHeight, dc.GetDeviceCaps( LOGPIXELSY ), 72);
		lf.lfWeight		= dwFlags & CPFF_BOLD ? FW_BOLD	: FW_DONTCARE;
		lf.lfItalic		= ( BYTE )( dwFlags & CPFF_ITALIC    ? 1 : 0 );
		lf.lfUnderline		= ( BYTE )( dwFlags & CPFF_UNDERLINE ? 1 : 0 );
		lf.lfStrikeOut		= ( BYTE )( dwFlags & CPFF_STRIKEOUT ? 1 : 0 );
		lf.lfCharSet		= DEFAULT_CHARSET;
		lf.lfOutPrecision	= OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
		lf.lfQuality		= DEFAULT_QUALITY;
		lf.lfPitchAndFamily	= DEFAULT_PITCH | FF_DONTCARE;

		// Copy face name.
		_tcsncpy( lf.lfFaceName, pszTypeFace, LF_FACESIZE );

		// Create...
		return CreateFontIndirect( &lf );
	}
	return FALSE;
}

// Get the LOGFONT structure.
BOOL ClsFont::GetLogFont( LOGFONT *pLogFont ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.
	_ASSERT_VALID( pLogFont ); // Must be valid.

	// Get the information.
	return GetObject( sizeof( LOGFONT ), ( LPVOID )pLogFont ) == sizeof( LOGFONT ) ? TRUE : FALSE;
}

// Get a stock font.
BOOL ClsFont::GetStockFont( int nObject )
{
	// Get the stock font.
	HFONT	hFont = ( HFONT )GetStockObject( nObject );
	
	// Make sure it is a font.
	if ( hFont && ::GetObjectType( hFont ) != OBJ_FONT )
		return FALSE;

	// Attach the object.
	Attach( hFont );
	return TRUE;
}

// Return the object of the handle.
ClsFont *ClsFont::FromHandle( HFONT hFont )
{
	return ( ClsFont * )ClsGdiObject::FromHandle( hFont );
}

// Operator to return the font handle.
ClsFont::operator HFONT() const
{
	// Return the handle.
	return ( HFONT )m_hGdiObject;
}

// Assignment operator.
BOOL ClsFont::operator=( const ClsFont& cFont )
{
	// Can't assign to ourself.
	if ( &cFont == this )
		return TRUE;

	// Copy the font.
	return operator=(( HFONT )cFont.m_hGdiObject );
}

// Assignment operator.
BOOL ClsFont::operator=( HFONT hFont )
{
	// Get font information.
	LOGFONT		lf = { 0 };

	if ( hFont && // ::GetObjectType( hFont ) == OBJ_FONT &&
	     ::GetObject( hFont, sizeof( LOGFONT ), ( void * )&lf ))
	     // Create the object.
	     return CreateFontIndirect( &lf );

	return FALSE;
}