#ifndef _HOTBUTTON_H_
#define _HOTBUTTON_H_
//
// hotbutton.h
//
// (C) Copyright 2000-2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../window.h"
#include "../controls/button.h"

// A ClsWindow derived class which handles the
// hot-tracking for derived classes.
class ClsHotButton : public ClsButton
{
	_NO_COPY( ClsHotButton );
public:
	// Constructor.
	ClsHotButton();

	// Destructor.
	virtual ~ClsHotButton();

	// Implementation.
	BOOL ShowToolTip( BOOL bShowTip = TRUE );
	BOOL RepeatTrigger( BOOL bRepeat = TRUE );
	inline BOOL IsHot() const
	{ return ( BOOL )(( m_dwFlags & HBF_ISHOT ) == HBF_ISHOT ); }
	inline BOOL IsDown() const
	{ return ( BOOL )(( m_dwFlags & HBF_DOWN ) == HBF_DOWN ); }
	inline BOOL HasFocus() const
	{ return ( BOOL )(( m_dwFlags & HBF_FOCUS ) == HBF_FOCUS ); }
	inline void SetHotTrack( BOOL bAllowHot = TRUE )
	{ if ( bAllowHot ) m_dwFlags &= ~HBF_NOHOT; else m_dwFlags |= HBF_NOHOT; }
	inline void SetHotFrame( BOOL bHotFrame = TRUE )
	{ if ( bHotFrame ) m_dwFlags |= HBF_HOTFRAME; else m_dwFlags &= ~HBF_HOTFRAME; }
	inline HIMAGELIST GetImageList() const
	{ return m_hImages; }
	inline HIMAGELIST SetImageList( HIMAGELIST hImages, BOOL bRedraw = FALSE )
	{ HIMAGELIST himl = m_hImages; m_hImages = hImages; if ( bRedraw ) Invalidate(); return himl; }
	inline COLORREF& BackgroundColor()
	{ return ( COLORREF& )m_crBkColor; }
	inline COLORREF& HotBackgroundColor()
	{ return ( COLORREF& )m_crHotBkColor; }
	inline COLORREF& LabelColor()
	{ return ( COLORREF& )m_crLabelColor; }
	inline COLORREF& HotLabelColor()
	{ return ( COLORREF& )m_crHotLabelColor; }
	inline BOOL& Transparent()
	{ return ( BOOL& )m_bTransparent; }
	inline BOOL& IsPanelHeader() 
	{ return ( BOOL& )m_bPanelHeader; }

protected:
	// Message handler override. This override will handle all
	// necessary messages to implement hot-tracking. 
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnNotify( LPNMHDR pNMHDR );
	
	// Overidables.
	virtual void Trigger();
	virtual BOOL TipString( LPNMTTDISPINFO lpDispInfo );
	virtual LRESULT OnReflectedCommand( UINT nNotifyCode, BOOL& bNotifyParent );
	void RenderFrame( ClsDC *pDC, LPRECT lpRect );

	// Flags
	enum
	{
		HBF_FOCUS    = 0x00000001, // We have the focus.
		HBF_DOWN     = 0x00000002, // We are down-pressed.
		HBF_ISHOT    = 0x00000004, // The button is hot.
		HBF_TRACK    = 0x00000008, // We are tracking the mouse.
		HBF_NOHOT    = 0x00000010, // We will not go hot.
		HBF_HOTFRAME = 0x00000020  // Render hot-frame.
	};

	// Data.
	ClsFont		m_Font;
	HWND		m_hToolTip;
	DWORD		m_dwFlags;
	HIMAGELIST	m_hImages;
	COLORREF	m_crBkColor;
	COLORREF	m_crHotBkColor;
	COLORREF	m_crLabelColor;
	COLORREF	m_crHotLabelColor;
	BOOL		m_bTransparent;
	BOOL		m_bPanelHeader;
	BOOL		m_bRepeatTrigger;
	HANDLE		m_hTheme;
	DWORD		m_dwDelay;
	DWORD		m_dwSpeed;

	// Below is the functionality defined which has been taken from the
	// COddButton source code which is:
	//
	// Copyright (c) 2001 Paolo Messina and Jerzy Kaczorowski

	// used to enable default state handling
	void EnableDefault(BOOL bEnable);

	// use to know when the control has default state
	BOOL IsDefault();

	// use to know the type of control to draw
	UINT GetControlType();

	// Overidables. The BM_SETSTYLE and WM_GETDLGCODE
	// messages are handled in the WindowProc() overidable
	// defined above.
	virtual void PreSubclassWindow();
	
private:
	BOOL		m_bCanBeDefault;
	BOOL		m_bDefault;
	UINT		m_nTypeStyle;
};

#endif // _HOTBUTTON_H_