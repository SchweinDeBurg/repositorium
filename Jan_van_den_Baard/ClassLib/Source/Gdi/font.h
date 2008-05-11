#ifndef _FONT_H_
#define _FONT_H_
//
// font.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "gdiobject.h"

// A wrapper class for the HFONT handle.
class ClsFont : public ClsGdiObject
{
public:
	// Constructor.
	ClsFont();
	ClsFont( const ClsFont& cFont );
	ClsFont( HFONT hFont );
	ClsFont( const LOGFONT *pLogFont );

	// Destructor.
	virtual ~ClsFont();

	// Flags for CreatePointFont().
	enum {
		CPFF_NORMAL	= 0x0001,
		CPFF_BOLD	= 0x0002,
		CPFF_ITALIC	= 0x0004,
		CPFF_UNDERLINE	= 0x0008,
		CPFF_STRIKEOUT	= 0x0010
	};

	// Interface.
	BOOL CreateFontIndirect( const LOGFONT *pLogFont );
	BOOL CreateFont( int nHeight, int nWidth, int nEscapement, int nOrientation,
			 int fnWeight, DWORD fdwItalic, DWORD fdwUnderline,
			 DWORD fdwStrikeOut, DWORD fdwCharSet, DWORD fdwOutputPrecision,
			 DWORD fdwClipPrecision, DWORD fdwQuality, DWORD fdwPitchAndFamily,
			 LPCTSTR lpszFace );
	BOOL CreatePointFont( LPCTSTR pszTypeFace, UINT nHeight, DWORD dwFlags = CPFF_NORMAL );
	BOOL GetLogFont( LOGFONT *pLogFont ) const;
	BOOL GetStockFont( int nObject );
	static ClsFont *FromHandle( HFONT hFont );

	// Operator overloads.
	operator HFONT() const;
	BOOL operator=( const ClsFont& cFont );
	BOOL operator=( HFONT hFont );
};

#endif // _FONT_H_