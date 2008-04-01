#ifndef __FOLDER_TABCTRL_H__
#define __FOLDER_TABCTRL_H__

/////////////////////////////////////////////////////////////////////////////
//
// CFolderTabCtrl
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Updated for new CCustomTabCtrl by Daniel Bowen (dbowen@es.com).
// Copyright (c) 2001-2002 Bjarke Viksoe.
//
// CFolderTabCtrl code based on a Paul DiLascia MSJ 1999 article.
//
// ClassLib version by Jan van den Baard.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//
// History (Date/Author/Description):
// ----------------------------------
//
// 2004/12/26: Jan van den Baard
// - First implementation for the ClassLib library.
// - Adjusted class names to fit the names of the classes in ClassLib. This 
//   basically means the "C" prefixes at the start of the class names are 
//   replaced by "Cls".
// - Adjusted indentation etc. a bit more to my style, This is not a snear
//   to the style of the original authors. It is just my way to force me to
//   read all the code so that I can understand it better.
// - ClsFolderCtrl is now based on the "ClsCloseScrollTabCtrl" class for the 
//   handling and rendering of the close and scroll buttons.
// - Both the "normal" and CTCS_BOTTOM styles are supported now. Formaly
//   only the CTCS_BOTTOM style was supported which was used no matter if the
//   style was set or not. Now the style must explicitly be set otherwise
//   the normal style is used.
// - The CTCS_BOLDSELECTEDTAB style is now also supported.
// - The font is now based on the small caption font.
//

#include "customtabctrl.h"

// ClsCloseScrollTabCtrl derived class implementing the trapezoid
// style tabs. Derived from ClsCloseScrollTabCtrl to support
// scroll and close buttons.
template <class TItem = ClsCustomTabItem, bool t_bDotNetAdjust = false >
class ClsFolderTabCtrl : public ClsCloseScrollTabCtrl< TItem, t_bDotNetAdjust >
{
	_NO_COPY( ClsFolderTabCtrl );
protected:
	// Baseclass definition.
	typedef ClsCloseScrollTabCtrl< TItem, t_bDotNetAdjust > baseClass;

public:
	enum { CXOFFSET = 8 };     // defined pitch of trapezoid slant
	enum { CXMARGIN = 2 };     // left/right text margin
	enum { CYMARGIN = 1 };     // top/bottom text margin
	enum { CYBORDER = 1 };     // top border thickness

	ClsFolderTabCtrl() {;}
	virtual ~ClsFolderTabCtrl() {;}

	// Message map.
	BEGIN_MSG_MAP( baseClass )
		MESSAGE_HANDLER( WM_SETTINGCHANGE, OnSettingChange )
	END_MSG_MAP()

	LRESULT OnSettingChange( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
	{
		// Get the NONCLIENTMETRICS. This is used to derive the
		// small caption font.
		NONCLIENTMETRICS ncm = { 0 };
		ncm.cbSize = sizeof(ncm);
		_VERIFY( ::SystemParametersInfo( SPI_GETNONCLIENTMETRICS, 0, &ncm, 0 ));

		// Do we already have a font?
		bool bResetFont = true;
		if( m_font.IsValid())
		{
			// Obtain the font parameters.
			LOGFONT lf = { 0 };
			if( m_font.GetLogFont( &lf ))
			{
				// Did the font change?
				if( _tcsicmp( lf.lfFaceName, ncm.lfSmCaptionFont.lfFaceName ) == 0 &&
					      lf.lfHeight == ncm.lfSmCaptionFont.lfHeight )
				{
					// The same font. No need to reinstall it.
					bResetFont = false;
				}
			}
		}

		// Do we need to setup the new font?
		if( bResetFont )
		{
			// Destroy existing fonts.
			if( m_font.IsValid())    m_font.Delete();
			if( m_fontSel.IsValid()) m_fontSel.Delete();

			// Force a "normal" style for the unselected
			// tabs.
			ncm.lfSmCaptionFont.lfWeight = FW_NORMAL;
			if ( m_font.CreateFontIndirect( &ncm.lfSmCaptionFont ) == FALSE )
			{
				// Oops. Use the default GUI font.
				m_font.Attach( ::GetStockObject( DEFAULT_GUI_FONT ));
			}

			// Do we use a bold font for
			// selected tabs?
			if( CTCS_BOLDSELECTEDTAB == ( GetStyle() & CTCS_BOLDSELECTEDTAB ))
			{
				// Yes. Bold it.
				ncm.lfSmCaptionFont.lfWeight = FW_BOLD;
			}

			// Create the selected tab font.
			if ( m_fontSel.CreateFontIndirect( &ncm.lfSmCaptionFont ) == FALSE )
			{
				// Oops. Use the default GUI font.
				m_fontSel.Attach( ::GetStockObject( DEFAULT_GUI_FONT ));
			}
		}

		// Setup padding and such.
		m_settings.iPadding = CXOFFSET + 10;
		m_settings.iMargin = -CXOFFSET;

		// Redo the layout and re-render.
		this->UpdateLayout();
		this->Invalidate();
		return 0;
	}

	// Overrides from ClsCustomTabCtrl
	virtual void DoItemPaint( LPNMCTCCUSTOMDRAW lpNMCustomDraw )
	{
		ClsDC dc( lpNMCustomDraw->nmcd.hdc );
		RECT &rc = lpNMCustomDraw->nmcd.rc;
		bool bSelected = ( CDIS_SELECTED == ( lpNMCustomDraw->nmcd.uItemState & CDIS_SELECTED ));

		// Determine forground and background colors depending
		// on the selected state of the tab.
		COLORREF bgColor = bSelected ? ::GetSysColor( COLOR_WINDOW )     : lpNMCustomDraw->clrBtnFace;
		COLORREF fgColor = bSelected ? ::GetSysColor( COLOR_WINDOWTEXT ) : lpNMCustomDraw->clrBtnText;

		// Setup colors.		
		dc.SetBkColor( bgColor );              // text background
		dc.SetTextColor( fgColor );            // text color = fg color

		// Create Gfx objects.
		ClsBrush brush( bgColor );
		ClsPen shadowPen( PS_SOLID, 1, lpNMCustomDraw->clrBtnShadow );

		// Fill trapezoid
		POINT pts[ 4 ];
		_GetTrapezoid( rc, pts );
		HGDIOBJ hOldPen = dc.SelectObject( ::GetStockObject( BLACK_PEN ));   
		ClsRgn rgn;
		rgn.CreatePolygonRgn( pts, 4, WINDING );
		dc.FillRgn( &rgn, &brush );

		// Draw edges.
		dc.MoveTo( pts[ 0 ] );		// upper left
		dc.LineTo( pts[ 1 ] );		// bottom left
		dc.SelectObject( shadowPen );	// bottom line is shadow color
		dc.MoveTo( pts[ 1 ] );		// line is inside trapezoid bottom
		dc.LineTo( pts[ 2 ] );		// ...
		dc.SelectObject( ::GetStockObject( BLACK_PEN ));   // upstroke is black
		dc.LineTo( pts[ 3 ] );		// y-1 to include endpoint
		if( ! bSelected ) 
		{
			// If not highlighted, upstroke has a 3D shadow, one pixel inside
			pts[ 2 ].x--;		// offset left one pixel
			pts[ 3 ].x--;		// ...ditto
			dc.SelectObject( shadowPen );
			dc.MoveTo( pts[ 2 ] );
			dc.LineTo( pts[ 3 ] );
		}

		// Restore original pen.
		dc.SelectObject( hOldPen );

		// Get a pointer to the tab item.
		TItem* pItem = this->GetItem( lpNMCustomDraw->nmcd.dwItemSpec );
		_ASSERT( pItem != NULL );
		if( pItem ) 
		{
			// Setup the font to use.
			HGDIOBJ oldFont = dc.SelectObject( bSelected ? m_fontSel : m_font );

			// Render the tab text.
			ClsString sText = pItem->GetText();
			::InflateRect( &rc, -( CXOFFSET + CXMARGIN ), -CYMARGIN );
			dc.DrawText( sText, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );

			// Restore the font.
			dc.SelectObject( oldFont );
		}
	}

	// Helper methods
	void _GetTrapezoid( const RECT& rc, POINT* pts ) const
	{
		// Are we a bottom style tab control? Bottom style means
		// tabs with their opening "up".
		if ( CTCS_BOTTOM == ( GetStyle() & CTCS_BOTTOM ))
		{
			pts[ 0 ].x = rc.left;
			pts[ 0 ].y = rc.top;
			pts[ 1 ].x = rc.left + CXOFFSET;
			pts[ 1 ].y = rc.bottom - 1;
			pts[ 2 ].x = rc.right - CXOFFSET - 1;
			pts[ 2 ].y = rc.bottom - 1;
			pts[ 3 ].x = rc.right - 1;
			pts[ 3 ].y = rc.top;
		}
		else
		{
			pts[ 0 ].x = rc.left;
			pts[ 0 ].y = rc.bottom;
			pts[ 1 ].x = rc.left + CXOFFSET;
			pts[ 1 ].y = rc.top;
			pts[ 2 ].x = rc.right - CXOFFSET - 1;
			pts[ 2 ].y = rc.top;
			pts[ 3 ].x = rc.right - 1, rc.top;
			pts[ 3 ].y = rc.bottom;
		}
	}
};
typedef ClsFolderTabCtrl< ClsCustomTabItem, false >	ClsFolderTabControl;

#endif // __FOLDER_TABCTRL_H__
