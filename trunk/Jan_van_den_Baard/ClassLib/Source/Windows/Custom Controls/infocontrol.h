#ifndef _INFOCONTROL_H_
#define _INFOCONTROL_H_
//
// infocontrol.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../controls/static.h"
#include "../../gdi/font.h"
#include "../../gdi/brush.h"
#include "../../collectors/array.h"

// Infotext command sequences.
//
// Alignment commands. The alignment commands are restricted
// in the way that they will affect all text on the line
// on which they are located. Also the alignment commands are
// mutually exclusive. The last issued alignment command on 
// the text line will be executed.

#define ISEQ_CENTER                     _T( "\33c" )
#define ISEQ_RIGHT                      _T( "\33r" )
#define ISEQ_LEFT                       _T( "\33l" )

// Color commands. Note that the macro takes a
// string in the form of "r,g,b". For example
//
// ISEQ_TEXTRGB( _T( "255,0,0" )) will turn the text
// color red.
//
// The ISEQ_TRANSPARENT command will turn of the
// background color rendering.

#define ISEQ_TEXTRGB(rgb)               _T( "\33t" ) rgb
#define ISEQ_BACKRGB(rgb)               _T( "\33b" ) rgb
#define ISEQ_TRANSPARENT                _T( "\33s" )

// Text style commands. You can combine any
// of the styles available. You can also turn 
// of any of the styles simply by inserting a
// ISEQ_NOxxx command. For example:
//
// ISEQ_BOLD ISEQ_ITALIC _T( "This text will be bold )
//                       _T( "and italic" )
// ISEQ_NOITALIC         _T( "This text will only be" )
//		         _T( "bold" )
//
// The ISEQ_NORMAL command will turn of all styles.

#define ISEQ_NORMAL                     _T( "\33n" )
#define ISEQ_BOLD                       _T( "\33+b" )
#define ISEQ_ITALIC                     _T( "\33+i" )
#define ISEQ_UNDERLINE                  _T( "\33+u" )
#define ISEQ_NOBOLD                     _T( "\33-b" )
#define ISEQ_NOITALIC                   _T( "\33-i" )
#define ISEQ_NOUNDERLINE                _T( "\33-u" )

// The following two control sequences allow
// you to set the text and/or background colors
// to any of the system colors defined below.

#define ISEQ_TEXTSYSCOLOR(idx)          _T( "\33gt" ) idx
#define ISEQ_BACKSYSCOLOR(idx)          _T( "\33gb" ) idx

// Color values compatible with the system
// GetSysColor() function. These values are to be
// used with the ISEQ_TEXTSYSCOLOR and the
// ISEQ_BACKSYSCOLOR control sequences.

#define IG_COLOR_SCROLLBAR              _T( "0" )
#define IG_COLOR_BACKGROUND             _T( "1" )
#define IG_COLOR_ACTIVECAPTION          _T( "2" )
#define IG_COLOR_INACTIVECAPTION        _T( "3" )
#define IG_COLOR_MENU                   _T( "4" )
#define IG_COLOR_WINDOW                 _T( "5" )
#define IG_COLOR_WINDOWFRAME            _T( "6" )
#define IG_COLOR_MENUTEXT               _T( "7" )
#define IG_COLOR_WINDOWTEXT             _T( "8" )
#define IG_COLOR_CAPTIONTEXT            _T( "9" )
#define IG_COLOR_ACTIVEBORDER           _T( "10" )
#define IG_COLOR_INACTIVEBORDER         _T( "11" )
#define IG_COLOR_APPWORKSPACE           _T( "12" )
#define IG_COLOR_HIGHLIGHT              _T( "13" )
#define IG_COLOR_HIGHLIGHTTEXT          _T( "14" )
#define IG_COLOR_BTNFACE                _T( "15" )
#define IG_COLOR_BTNSHADOW              _T( "16" )
#define IG_COLOR_GRAYTEXT               _T( "17" )
#define IG_COLOR_BTNTEXT                _T( "18" )
#define IG_COLOR_INACTIVECAPTIONTEXT    _T( "19" )
#define IG_COLOR_BTNHIGHLIGHT           _T( "20" )

#if(WINVER >= 0x0400)
#define IG_COLOR_3DDKSHADOW             _T( "21" )
#define IG_COLOR_3DLIGHT                _T( "22" )
#define IG_COLOR_INFOTEXT               _T( "23" )
#define IG_COLOR_INFOBK                 _T( "24" )
#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0500)
#define IG_COLOR_HOTLIGHT                _T( "26" )
#define IG_COLOR_GRADIENTACTIVECAPTION   _T( "27" )
#define IG_COLOR_GRADIENTINACTIVECAPTION _T( "28" )
#endif /* WINVER >= 0x0500 */

#if(WINVER >= 0x0400)
#define IG_COLOR_DESKTOP                IG_COLOR_BACKGROUND
#define IG_COLOR_3DFACE                 IG_COLOR_BTNFACE
#define IG_COLOR_3DSHADOW               IG_COLOR_BTNSHADOW
#define IG_COLOR_3DHIGHLIGHT            IG_COLOR_BTNHIGHLIGHT
#define IG_COLOR_3DHILIGHT              IG_COLOR_BTNHIGHLIGHT
#define IG_COLOR_BTNHILIGHT             IG_COLOR_BTNHIGHLIGHT
#endif /* WINVER >= 0x0400 */

// Forward declarations.
class ClsDC;
class ClsRect;

// Custom control class
class ClsInfoControl : public ClsStatic
{
	_NO_COPY( ClsInfoControl );
public:
	// Constructor.
	ClsInfoControl();
	
	// Destructor.
	virtual ~ClsInfoControl();

	// Implementation.
	inline BOOL& VCenter()
	{ return ( BOOL& )m_bVertCenter; }

	inline COLORREF& BackgroundColor()
	{ return ( COLORREF& )m_crBkColor; }

	int TotalWidth( ClsDC *pDC, LPCTSTR pszText );
	int TotalHeight( ClsDC *pDC, LPCTSTR pszText );

protected:
	// Helpers.
	void PaintControl( ClsDC *pDC );
	void RenderInfoText( ClsDC *pDC, LPCTSTR pszText, ClsRect *pDomain );
	int XPos( ClsDC *pDC, LPCTSTR pszText, ClsRect *pDomain, DWORD dwFlags );
	LPCTSTR ParseCommandSequence( ClsDC *pDC, LPCTSTR pszText, DWORD& dwFlags );
	BOOL ChangeFontStyle( ClsDC *pDC, DWORD dwFlags );
	LPCTSTR GetDigits( LPCTSTR pszText, int& nNum );

	// Overidables.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnPaint( ClsDC *pDC );
	virtual LRESULT OnEraseBkgnd( ClsDC *pDC )
	{ return 1; }


	// For the layout engine.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize );

	// Data used to store font's used.
	struct FontData
	{
		HFONT	m_hFont;
		DWORD	m_dwStyles;
	};

	// Data.
	ClsArray<FontData>	m_aFonts;
	ClsFont			m_cfUseFont;
	HFONT			m_hInitFont;
	COLORREF		m_crBkColor;
	BOOL			m_bVertCenter;
};

#endif // _INFOCONTROL_H_