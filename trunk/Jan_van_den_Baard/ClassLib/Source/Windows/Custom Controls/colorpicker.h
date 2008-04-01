#ifndef _COLORPICKER_H_
#define _COLORPICKER_H_
//
// colorpicker.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "hotbutton.h"

// Notifications.
#define COLPN_COLORCHANGED	WM_USER+0xDEAD

// A ClsHotButton derived class which enables the user
// to select colors.
class ClsColorPicker : public ClsHotButton
{
	_NO_COPY( ClsColorPicker );
public:
	// Constructor.
	ClsColorPicker();

	// Destructor.
	virtual ~ClsColorPicker();

	// Implementation.
	inline COLORREF& Color() { return ( COLORREF& )m_crColor; }
	inline COLORREF& DefaultColor() { return ( COLORREF& )m_crDefaultColor; }
	inline BOOL& FullOpen() { return ( BOOL& )m_bFullOpen; }
	inline BOOL& DropDownArrow() { return ( BOOL& )m_bRenderArrow; }
	inline void SetXPStyle( BOOL bPopup = TRUE, BOOL bControl = TRUE ) { m_bXPPopup = bPopup; m_bXPControl = bControl; }
	inline ClsString& DefaultString() { return ( ClsString& )m_strDefault; }
	inline ClsString& CustomString() { return ( ClsString& )m_strCustom; }

protected:
	// Overidables.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Trigger();
	virtual BOOL TipString( LPNMTTDISPINFO lpDispInfo );
	virtual LRESULT OnPaint( ClsDC *pDC );
	virtual LRESULT OnEraseBkgnd( ClsDC *pDC )
	{ return 1; }

	// For the layout engine.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize );

	// Helpers.
	void PaintControl( ClsDC *pDC );

	// Data.
	COLORREF	m_crDefaultColor;
	COLORREF	m_crColor;
	COLORREF	m_crTempColor;
	BOOL		m_bFullOpen;
	BOOL		m_bRenderArrow;
	BOOL		m_bXPPopup;
	BOOL		m_bXPControl;
	BOOL		m_bIsDropped;
	ClsString	m_strTip;
	ClsString	m_strDefault;
	ClsString	m_strCustom;
};

#endif // _COLORPICKER_H_