#ifndef _FLATBUTTON_H_
#define _FLATBUTTON_H_
//
// flatbutton.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "hotbutton.h"
#include "../../strings/string.h"

// Send through a WM_COMMAND to the parent when the
// control was clicked.
#define FBN_CLICKED	0x0001

// A ClsHotButton derived class implementing a
// flat button with or without a bitmap image.
class ClsFlatButton : public ClsHotButton
{
	_NO_COPY( ClsFlatButton );
public:
	// For SetImageIndex()...
	enum
	{
		FIIF_NORMAL = 0,
		FIIF_HOT = 1,
		FIIF_MAX = 2
	};

	// Constructor.
	ClsFlatButton();

	// Destructor.
	virtual ~ClsFlatButton();

	// Implementation.
	inline ClsString& Tip() 
	{ return ( ClsString& )m_Tip; }
	inline void SetImageIndex( int nImage, int nIndex, BOOL bRedraw = FALSE )
	{ _ASSERT( nImage >= 0 && nImage < FIIF_MAX ); m_aImageIndex[ nImage ] = nIndex; if ( bRedraw ) Invalidate(); }
	inline void SetXPStyle( BOOL bXPStyle = TRUE, BOOL bRedraw = FALSE ) { if ( bXPStyle != m_bXPStyle ) { m_bXPStyle = bXPStyle; if ( bRedraw ) Invalidate(); }}
	inline void ShowXPDefault( BOOL bXPDefault = TRUE, BOOL bRedraw = FALSE ) { m_bXPDefault = bXPDefault; if ( bRedraw) Invalidate(); }
	
protected:
	// Overidables.
	virtual void Trigger();
	virtual BOOL TipString( LPNMTTDISPINFO lpDispInfo );
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnPaint( ClsDC *pDC );
	virtual LRESULT OnEraseBkgnd( ClsDC *pDC )
	{ return 1; }

	// Helpers.
	void PaintControl( ClsDC *pDC );

	// For the layout engine.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize );

	// Data.
	ClsString	m_Tip;
	int		m_aImageIndex[ FIIF_MAX ];
	BOOL		m_bXPStyle;
	BOOL		m_bXPDefault;
};

#endif // _FLATBUTTON_H_