#ifndef _CFWINDOW_H_
#define _CFWINDOW_H_
//
// cfwindow.h
//
// (C) Copyright 2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "window.h"
#include "common controls/tooltip.h"
#include "../gdi/font.h"

// A class which simplifies the creation of
// custom window frames including buttons
// inside the frame.
class ClsCFWindow : public ClsWindow
{
	_NO_COPY( ClsCFWindow );
public:
	// Sent to the OnRenderCaptionButton() overridable.
	typedef struct
	{
		int	cbSize;		// Structure size.
		UINT	nID;		// ID of the button.
		UINT	nState;		// Button state (see below).
		RECT	rcButton;	// Button boundaries.
		DWORD	dwData;		// User defined.
	} DRAWCAPTIONBUTTON, *LPDRAWCAPTIONBUTTON;

	// Caption button states.
	enum
	{
		CBS_NORMAL  = 0,	// Normal.
		CBS_HOT	    = 1,	// Mouse hovering.
		CBS_CLICKED = 2,	// Button clicked.
		CBS_CHECKED = 4		// Checked.
	};

	// Pre-defined caption button types.
	enum
	{
		CBT_CLOSE	= 0,
		CBT_MENU	= 1,
		CBT_PIN		= 2,
		CBT_UP		= 3,
		CBT_DOWN	= 4,
		CBT_LEFT	= 5,
		CBT_RIGHT	= 6,
		CBT_CUSTOM	= 7,	// User defined.
		CBT_SIZEOF	= 8
	};

	// Construction/destruction.
	ClsCFWindow();
	virtual ~ClsCFWindow();

	// Implementation.
	BOOL AddCaptionButton( UINT nType, UINT nID, BOOL bCheckable = FALSE, BOOL bLeft = FALSE, LPTSTR pszTooltip = NULL, DWORD dwData = 0L );
	BOOL RemoveCaptionButton( UINT nID );
	BOOL CheckCaptionButton( UINT nID, BOOL bChecked = TRUE );
	BOOL IsCaptionButtonChecked( UINT nID ) const;
	void DoCustomFrames( BOOL bDoCustomFrames );

	// Attributes.
	inline LONG LeftFrameSize() const { return m_rcFrameSize.Left(); }
	inline LONG TopFrameSize() const { return m_rcFrameSize.Top(); }
	inline LONG RightFrameSize() const { return m_rcFrameSize.Right(); }
	inline LONG BottomFrameSize() const { return m_rcFrameSize.Bottom(); }
	inline LONG CaptionSize() const { return m_lCaptionSize; }
	void SetOldStyle( BOOL bOldStyle = TRUE );

	// Overidables.
	virtual void OnComputeFrameSize( ClsRect& rcFrame, LONG& nCaptionSize );
	virtual BOOL OnRenderWindowFrame( ClsDC *pDC, ClsRect rcWindowRect );
	virtual BOOL OnRenderCaptionBackground( ClsDC *pDC, ClsRect rcCaption );
	virtual BOOL OnRenderCaptionText( ClsDC *pDC, ClsRect rcCaptionText );
	virtual BOOL OnRenderCaptionButton( ClsDC *pDC, LPDRAWCAPTIONBUTTON pDrawCaptionButton );
	virtual void OnCaptionButtonClicked( UINT nID );

protected:
	// Overidables.
	virtual LRESULT OnEraseBkgnd( ClsDC *pDC );
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Helpers.
	void CreateCaptionFonts();
	void PositionCaptionButtons();
	int HitTestCaptionButton();
	int HitTestCaptionButton( ClsPoint pt );
	void SetButtonState( int nIndex, DWORD dwState );
	void RenderButton( ClsDC *pDC, int nIndex );
	void RenderAllButtons( ClsDC *pDC );
	void RenderWholeFrame( ClsDC *pDC );
	void HideToolTip();
	void ShowToolTip( LPPOINT pt, LPTSTR pszTip );

	// Internal storage for the caption buttons.
	struct StrCapButton
	{
		ClsRect		rcButton;	// Button boundaries.
		BOOL		bLeftAlign;	// Left align.
		BOOL		bCheckable;	// Checkable button.
		DWORD		dwState;	// Button state.
		UINT		nID;		// Button ID.
		UINT		nType;		// Button type.
		UINT		nImage;		// Normal image.
		UINT		nCheckedImage;	// Checked image.
		DWORD		dwButtonData;	// User data.
		LPTSTR		pszTooltip;	// Tooltip.
	};
	
	// Data.
	ClsTooltip		m_Tip;			// Tooltip control.
	ClsRect			m_rcFrameSize;		// Frame sizes.
	LONG			m_lCaptionSize;		// Captionsize.
	ClsArray<StrCapButton>	m_CaptionButtons;	// Button definitions.
	ClsRect			m_Caption;		// Caption text boundaries.
	int			m_nHotButton;		// Index of the hot button.
	BOOL			m_bTracking;		// Tracking a button click?
	BOOL			m_bTipVisible;		// Tooltip shown?
	BOOL			m_bIsHot;		// Tracking button hot?
	BOOL			m_bDoCustomFrames;	// Do we do cutom frames?
	BOOL			m_bOldStyle;		// "Old-style" rendering.
	ClsFont			m_CaptionFont;		// Caption font.
	ClsFont			m_SmCaptionFont;	// Small caption font.
};
#endif