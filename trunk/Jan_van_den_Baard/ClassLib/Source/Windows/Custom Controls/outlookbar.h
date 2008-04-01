#ifndef _OUTLOOKBAR_H_
#define _OUTLOOKBAR_H_
//
// outlookbar.h
//
// (C) Copyright 2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../common controls/imagelist.h"
#include "panelbar.h"

// Scroll buttons for the ClsOutlookBar
// class. This is a simple ClsFlatButton
// derived class.
class ClsScrollButton : public ClsFlatButton
{
	_NO_COPY( ClsScrollButton );
public:
	ClsScrollButton() 
	{
		// Default is down scroll button.
		m_bUp = FALSE;

		// Repeat triggers. This will repeat the
		// event while the button is pressed with
		// the same interval as keyboard repeats.
		RepeatTrigger( TRUE );

	}
	virtual ~ClsScrollButton() {;}

	// Attributes.
	BOOL& Up() { return ( BOOL& )m_bUp; }

protected:
	// Overidables.
	virtual LRESULT OnPaint( ClsDC *pDC )
	{	
		// Input DC?
		if ( pDC ) PaintControl( pDC );
		else
		{
			ClsPaintDC dc( this );
			PaintControl( &dc );
		}
		return 0;
	}

	// Render the button.
	void PaintControl( ClsDC *pDC )
	{
		// Render the frame including scroll
		// button arrow.
		pDC->DrawFrameControl( GetClientRect(), 
				       DFC_SCROLL, 
				       ( m_bUp ? DFCS_SCROLLUP : DFCS_SCROLLDOWN ) | 
				       ( IsDown() ? DFCS_PUSHED : 0 ));
	}

	// Data.
	BOOL	m_bUp;
};

// An "Outlook" style panel bar.
class ClsOutlookBar : public ClsPanel
{
	_NO_COPY( ClsOutlookBar );
public:
	// Construction/destruction.
	ClsOutlookBar();
	virtual ~ClsOutlookBar();

	// Implementation.
	COLORREF SetBackgroundColor( COLORREF crColor, BOOL bRedraw = TRUE );
	int SetBackgroundSysColor( int nSysColor, BOOL bRedraw = TRUE );
	COLORREF SetLabelColor( COLORREF crColor, BOOL bRedraw = TRUE );
	int SetLabelSysColor( int nSysColor, BOOL bRedraw = TRUE );
	HIMAGELIST SetImageList( HIMAGELIST hImages, BOOL bSmallImages = FALSE, BOOL bRedraw = TRUE );
	BOOL InsertOutlookButton( LPCTSTR pszTitle, int nImageIndex, UINT nID, UINT nIDPred = 0, BOOL bRedraw = TRUE, DWORD dwData = 0L );
	BOOL RemoveOutlookButton( UINT nID, BOOL bRedraw = TRUE );
	BOOL SetOutlookButtonLabel( UINT nID, LPCTSTR pszTitle, BOOL bRedraw = TRUE );
	BOOL SetOutlookButtonImage( UINT nID, int nImageIndex, BOOL bRedraw = TRUE );
	BOOL EnableOutlookButton( UINT nID, BOOL bEnable = TRUE, BOOL bRedraw = TRUE );
	void UseSmallImages( BOOL bUse = TRUE, BOOL bRedraw = TRUE );
	LPCTSTR GetOutlookButtonLabel( UINT nID ) const;
	int GetOutlookButtonImage( UINT nID ) const;
	BOOL IsOutlookButtonEnabled( UINT nID ) const;

protected:
	enum
	{
		OBS_ISHOT	= 0x00000001,
		OBS_DISABLED	= 0x00000002,
	};

	typedef struct OutlookData
	{
		int	m_cbSize;
		LPCTSTR	m_pszButtonTitle;
		int	m_nButtonImageIndex;
		UINT	m_nButtonID;
		ClsRect m_IconBounds;
		ClsRect	m_TitleBounds;
		DWORD	m_dwState;
		DWORD	m_dwData;
	} OUTLOOKDATA, *LPOUTLOOKDATA;

	// Overidables.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnEraseBkgnd( ClsDC *pDC );
	virtual LRESULT OnPaint( ClsDC *pDC );
	virtual LRESULT OnCreate( LPCREATESTRUCT pCS );
	virtual LRESULT OnSize( UINT nSizeType, int nWidth, int nHeight );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );

	virtual void OnGetOutlookButtonSize( ClsDC *pDC, ClsSize& szIcon, ClsSize& szText, LPOUTLOOKDATA pData );
	virtual void OnDrawOutlookBackground( ClsDC *pDC, ClsRect& rc );
	virtual void OnDrawOutlookButton( ClsDC *pDC, LPOUTLOOKDATA pData );
	virtual void OnOutlookButtonClicked( UINT nID );

	// Helpers.
	void PaintControl( ClsDC *pDC );
	inline BOOL Tracking() const { return m_bTracking; }
	int GetButtonIndex( UINT nID ) const;

	// Data.
	ClsArray<OUTLOOKDATA>	m_Buttons;
	int			m_nHotIndex;
	int			m_nTrackingIndex;
	int			m_nTopIndex;
	BOOL			m_bTracking;
	COLORREF		m_crLabelColor;
	int			m_nLabelSysColor;
	BOOL			m_bUseSysColor;
	COLORREF		m_crBackgroundColor;
	int			m_nBackgroundSysCol;
	BOOL			m_bUseSysBgColor;
	BOOL			m_bSmallImages;
	ClsImageList		m_SmallImages;
	ClsImageList		m_LargeImages;
	ClsScrollButton		m_sbUp;
	ClsScrollButton		m_sbDown;
};

// A simple "ClsOutlookBar" derived class to
// support XP style rendering.
class ClsXPOutlookBar : public ClsOutlookBar
{
	_NO_COPY( ClsXPOutlookBar );
public:
	// Construction/destruction.
	ClsXPOutlookBar() { m_bOldStyle = FALSE; }
	virtual ~ClsXPOutlookBar() {;}

	// Attributes.
	inline BOOL& OldStyle() { return ( BOOL& )m_bOldStyle; }

protected:
	// Overidables.
	virtual void OnDrawOutlookBackground( ClsDC *pDC, ClsRect& rc );
	virtual void OnDrawOutlookButton( ClsDC *pDC, OutlookData *pData );

	// Data.
	BOOL			m_bOldStyle;
};
#endif // _OUTLOOKBAR_H_