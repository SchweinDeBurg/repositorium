#ifndef _INFOBAR_H_
#define _INFOBAR_H_
//
// infobar.h
// 
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../controls/static.h"
#include "../../gdi/font.h"

// A class which displays a title in a rectangular
// area. Optionally a icon can be displayed aswell.
class ClsInfoBar : public ClsWindow
{
	_NO_COPY( ClsInfoBar );
public:
	// Constructor.
	ClsInfoBar();

	// Destructor.
	virtual ~ClsInfoBar();
	
	// Implementation.
	inline COLORREF& TitleColor()
	{ return ( COLORREF& )m_crTitleColor; }

	inline COLORREF& BackgroundColor()
	{ return ( COLORREF& )m_crBackgroundColor; }

	inline COLORREF& BackgroundGradient()
	{ return ( COLORREF& )m_crBackgroundGradient; }

	inline BOOL& Gradient()
	{ return ( BOOL& )m_bUseGradient; }

	inline BOOL& Vertical()
	{ return ( BOOL& )m_bIsVertical; }

	inline BOOL& CompactAsPath()
	{ return ( BOOL& )m_bCompactAsPath; }

	inline BOOL& IconRight()
	{ return ( BOOL& )m_bIconRight; }

	BOOL SetIcon( LPCTSTR pszIconName, BOOL bRepaint = TRUE );
	BOOL SetSystemIcon( LPCTSTR pszIconName, BOOL bRepaint = TRUE );
	BOOL TipClippedCaption( BOOL bClip = TRUE );

protected:
	// Overidables.
	virtual LRESULT OnPaint( ClsDC *pDC );
	virtual LRESULT OnNotify( LPNMHDR pNMHDR );
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnEraseBkgnd( ClsDC *pDC )
	{ return 1; }

	// Helpers.
	void PaintControl( ClsDC *pDC );
	
	// For the layout engine.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize );

	// Data.
	HICON		m_hIcon;
	HWND		m_hToolTip;
	ClsFont		m_fNormal;
	ClsFont		m_fVertical;
	ClsString	m_Title;
	COLORREF	m_crTitleColor;
	COLORREF	m_crBackgroundColor;
	COLORREF	m_crBackgroundGradient;
	BOOL		m_bUseGradient;
	BOOL		m_bIsVertical;
	BOOL		m_bIsClipped;
	BOOL		m_bCompactAsPath;
	BOOL		m_bIconRight;
};

#endif // _INFOBAR_H_
