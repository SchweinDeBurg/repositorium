#ifndef _XPCOLORS_H_
#define _XPCOLORS_H_
//
// xpcolors.h
//
// (C) Copright 2002-2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "color.h"

// A simple class which will hold the colors necessary
// to render the Office XP style menus etc.
class ClsXPColors
{
public:
	// Color indexes.
	enum
	{
		XPC_OUTER_SELECTION = 0,		// Outside selection rectangle.
		XPC_INNER_SELECTION = 1,		// Inside selection rectangle.
		XPC_OUTER_CHECKED = 2,			// Outside checkmark rectangle.
		XPC_INNER_CHECKED = 3,			// Inside checkmark rectangle (unselected).
		XPC_INNER_CHECKED_SELECTED = 4,		// Inside checkmark rectangle (selected).
		XPC_IMAGE_BACKGROUND = 5,		// Image background area.
		XPC_IMAGE_DISABLED = 6,			// Image disabled color.
		XPC_IMAGE_SHADOW = 7,			// Shadow image color.
		XPC_TEXT = 8,				// Text color.
		XPC_TEXT_DISABLED = 9,			// Text color disabled.
		XPC_TEXT_BACKGROUND = 10,		// Background text.
		XPC_SEPARATOR = 11,			// Seperator.
		XPC_TOOLWINDOW_FRAME = 12,		// Outer frame toolwindow.
		XPC_TOOLWINDOW_FRAME_ACTIVE = 13,	// Outer frame active toolwindow.
		XPC_TOOLWINDOW_CAPTION = 14,		// Background toolwindow caption.
		XPC_TOOLWINDOW_CAPTION_ACTIVE = 15,	// Background active toolwindow caption.
		XPC_TOOLWINDOW_CAPTION_TEXT = 16,	// Text toolwindow caption.
		XPC_TOOLWINDOW_CAPTION_TEXT_ACTIVE = 17,// Text active toolwindow caption.
		XPC_TOOLWINDOW_BACKGROUND = 18,		// Client toolwindow caption.
		XPC_MENU_FRAME = 19,			// Menu frame.
		XPC_SIZEOF = 20
	};

	// Construction/destruction.
	ClsXPColors() { CreateColorTable(); }
	virtual ~ClsXPColors() {;}

	// Implementation.
	virtual void CreateColorTable()
	{
		// Selection rectangle.
		ClsColor color( ::GetSysColor( COLOR_HIGHLIGHT ));
		m_crColorTable[ XPC_OUTER_SELECTION ] = color;
		color.LightenColor( 0.70, 0.70, 0.7 );
		m_crColorTable[ XPC_INNER_SELECTION ] = color;

		// Menu background.
		color.SetRGB( ::GetSysColor( COLOR_BTNFACE ));
		color.LightenColor( 0.2, 0.2, 0.2 );
		m_crColorTable[ XPC_IMAGE_BACKGROUND ] = color;
		color.LightenColor( 0.83, 0.83, 0.83 );
		m_crColorTable[ XPC_TEXT_BACKGROUND ] = color;

		// Checkmark rectangle.
		m_crColorTable[ XPC_OUTER_CHECKED ] = m_crColorTable[ XPC_OUTER_SELECTION ];
		color.SetRGB( m_crColorTable[ XPC_OUTER_SELECTION ] );
		color.LightenColor( 0.5, 0.5, 0.5 );
		m_crColorTable[ XPC_INNER_CHECKED_SELECTED ] = color;
		color.SetRGB( m_crColorTable[ XPC_OUTER_SELECTION ] );
		color.LightenColor( 0.83, 0.81, 0.74 );
		m_crColorTable[ XPC_INNER_CHECKED ] = color;

		// Images.
		m_crColorTable[ XPC_IMAGE_DISABLED ] = ::GetSysColor( COLOR_GRAYTEXT );
		color.SetRGB( m_crColorTable[ XPC_OUTER_SELECTION ] );
		color.LightenColor( 0.3, 0.3, 0.3 );
		m_crColorTable[ XPC_IMAGE_SHADOW ] = color;

		// Text.
		m_crColorTable[ XPC_TEXT ] = ::GetSysColor( COLOR_MENUTEXT );
		color.SetRGB( ::GetSysColor( COLOR_GRAYTEXT )); 
		color.LightenColor( 0.3, 0.3, 0.3 );
		m_crColorTable[ XPC_TEXT_DISABLED ] = color;

		// Separator.
		m_crColorTable[ XPC_SEPARATOR ] = color;

		// Menu frame.
		m_crColorTable[ XPC_MENU_FRAME ] = m_crColorTable[ XPC_IMAGE_DISABLED ];

		// Toolwindow. 
		color.SetRGB( ::GetSysColor( COLOR_BTNSHADOW ));
		color.DarkenColor( 0.2, 0.2, 0.2 );
		m_crColorTable[ XPC_TOOLWINDOW_FRAME ] = color;
		m_crColorTable[ XPC_TOOLWINDOW_FRAME_ACTIVE ] = color;
		m_crColorTable[ XPC_TOOLWINDOW_CAPTION ] = m_crColorTable[ XPC_IMAGE_DISABLED ];
		m_crColorTable[ XPC_TOOLWINDOW_CAPTION_ACTIVE ] = m_crColorTable[ XPC_IMAGE_DISABLED ];
		m_crColorTable[ XPC_TOOLWINDOW_CAPTION_TEXT ] = m_crColorTable[ XPC_TEXT_BACKGROUND ];
		m_crColorTable[ XPC_TOOLWINDOW_CAPTION_TEXT_ACTIVE ] = m_crColorTable[ XPC_TEXT_BACKGROUND ];
		m_crColorTable[ XPC_TOOLWINDOW_BACKGROUND ] = m_crColorTable[ XPC_IMAGE_BACKGROUND ];
	}

	// Obtain a color from the table.
	inline COLORREF GetXPColor( int nIndex ){ _ASSERT( nIndex >= 0 && nIndex < XPC_SIZEOF ); return m_crColorTable[ nIndex ]; }

protected:
	// Data.
	COLORREF	m_crColorTable[ XPC_SIZEOF ];
};

// Defined in application.cpp.
extern ClsXPColors XPColors;
#endif _XPCOLORS_H_