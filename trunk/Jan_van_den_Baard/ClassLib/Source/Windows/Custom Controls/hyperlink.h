#ifndef _HYPERLINK_H_
#define _HYPERLINK_H_
//
// hyperlink.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "hotbutton.h"
#include "../../gdi/bitmap.h"
#include "../../strings/string.h"

// A ClsHotButton derived class which can be used
// to display a click-able hyperlink in your UI.
class ClsHyperLink : public ClsHotButton
{
	_NO_COPY( ClsHyperLink );
public:
	// Constructor.
	ClsHyperLink();

	// Destructor.
	virtual ~ClsHyperLink();

	// Possible image place.
	enum { IMPLACE_LEFT = 0, IMPLACE_TOP, IMPLACE_RIGHT, IMPLACE_BOTTOM };
	enum { LBPLACE_LEFT = 0, LBPLACE_CENTER, LBPLACE_RIGHT };
	
	// Implementation.
	void VisitURL() { Trigger(); }
	inline UINT& LabelPlace()
	{ return ( UINT& )m_nLabelPlace; }
	inline UINT& ImagePlace()
	{ return ( UINT& )m_nImagePlace; }
	inline ClsString& URL() 
	{ return ( ClsString& )m_strURL; }

	inline COLORREF& NormalColor()
	{ return ( COLORREF& )m_crNormal; }

	inline COLORREF& VisitedColor()
	{ return ( COLORREF& )m_crVisited; }

	inline COLORREF& HotColor()
	{ return ( COLORREF& )m_crHot; }

	inline BOOL& IsVisited() 
	{ return ( BOOL& )m_bIsVisited; }

	inline BOOL& ShowFocus()
	{ return ( BOOL& )m_bRenderFocus; }

protected:
	// Overidables.
	virtual void Trigger();
	virtual BOOL TipString( LPNMTTDISPINFO lpDispInfo );
	virtual LRESULT OnPaint( ClsDC *pDC );
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnEraseBkgnd( ClsDC *pDC )
	{ return 1; }

	// Helpers.
	void PaintControl( ClsDC *pDC );

	// For the layout engine.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize );

	// Data.
	ClsFont		m_urlFont;
	ClsString	m_strURL;
	COLORREF	m_crNormal;
	COLORREF	m_crVisited;
	COLORREF	m_crHot;
	HCURSOR		m_curHand;
	UINT		m_nImagePlace;
	UINT		m_nLabelPlace;
	BOOL		m_bIsVisited;
	BOOL		m_bRenderFocus;
};

#endif // _HYPERLINK_H_